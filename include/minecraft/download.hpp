#pragma once

#include <curl/curl.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace cnt::minecraft::download {

// 下载任务状态
enum class DownloadStatus {
    pending,     // 等待中
    downloading, // 下载中
    paused,      // 已暂停
    completed,   // 已完成
    failed,      // 失败
    canceled     // 已取消
};

// 下载进度回调参数
struct DownloadProgress {
    std::string url;
    std::string file_path;
    int64_t total_bytes;       // 总字节数，-1 表示未知
    int64_t downloaded_bytes;  // 已下载字节数
    double speed_bytes_per_sec;// 当前速度（字节/秒）
};

// 下载完成回调参数
struct DownloadResult {
    std::string url;
    std::string file_path;
    bool success;
    std::string error_message; // 失败时的错误描述
    int http_status_code;      // HTTP 状态码，非 HTTP 协议时为 0
};

// 回调函数类型定义
using progress_callback = std::function<void(const DownloadProgress&)>;
using completion_callback = std::function<void(const DownloadResult&)>;

// 单个下载任务描述
struct DownloadTask {
    std::string url;                    // 下载 URL
    std::string save_path;              // 保存文件的完整路径
    int64_t limit_speed = 0;            // 限速（字节/秒），0 表示不限速
    int32_t max_retries = 3;            // 失败重试次数
    int64_t resume_from = 0;            // 断点续传起始位置（字节）
    
    progress_callback progress_cb;       // 进度回调（可选）
    completion_callback completion_cb;   // 完成回调（可选）
    
    // 用户自定义数据，供回调使用
    void* user_data = nullptr;
    
    DownloadTask() = default;
    DownloadTask(const std::string& url, const std::string& save_path)
        : url(url), save_path(save_path) {}
};

// 下载管理器（基于 libcurl 多路复用接口）
class DownloadManager {
public:
    // 构造与析构
    DownloadManager();
    ~DownloadManager();
    
    // 禁止拷贝，防止 CURL 句柄被意外复制
    DownloadManager(const DownloadManager&) = delete;
    DownloadManager& operator=(const DownloadManager&) = delete;
    
    // 移动构造/赋值（需谨慎处理句柄所有权）
    DownloadManager(DownloadManager&& other) noexcept;
    DownloadManager& operator=(DownloadManager&& other) noexcept;
    
    // ----- 任务管理 -----
    
    // 添加下载任务，返回任务 ID（>=0），失败返回 -1
    int add_task(const DownloadTask& task);
    
    // 开始下载指定任务（若未指定 id，则启动所有等待中的任务）
    bool start_task(int task_id);
    void start_all_tasks();
    
    // 暂停下载任务
    bool pause_task(int task_id);
    void pause_all_tasks();
    
    // 取消下载任务（会删除任务，未完成的文件是否保留取决于配置）
    bool cancel_task(int task_id, bool delete_file = false);
    void cancel_all_tasks(bool delete_files = false);
    
    // 移除已完成/失败/取消的任务（从内部列表中清除）
    bool remove_task(int task_id);
    void clear_finished_tasks();
    
    // 获取任务状态
    DownloadStatus get_task_status(int task_id) const;
    
    // 获取任务进度信息
    bool get_task_progress(int task_id, DownloadProgress& out_progress) const;
    
    // 获取所有任务 ID 列表
    std::vector<int> get_all_task_ids() const;
    
    // ----- 全局设置 -----
    
    // 设置全局最大并发下载数（默认 5）
    void set_max_concurrent_downloads(int max);
    int get_max_concurrent_downloads() const;
    
    // 设置全局下载速度限制（字节/秒，0 为不限速）
    void set_global_speed_limit(int64_t bytes_per_sec);
    int64_t get_global_speed_limit() const;
    
    // 设置 User-Agent
    void set_user_agent(const std::string& user_agent);
    
    // 设置连接超时/传输超时（秒）
    void set_timeout(long connect_timeout_sec, long transfer_timeout_sec);
    
    // 设置是否允许断点续传（默认允许）
    void set_resume_enabled(bool enabled);
    
    // 设置最大重试次数（全局默认，任务可单独覆盖）
    void set_default_max_retries(int retries);
    
    // ----- 驱动循环 -----
    
    // 处理下载事件（需要在主循环或专用线程中定期调用）
    // 返回当前仍在活动的传输数量（包括等待的）
    int update();
    
    // 是否还有未完成的任务（状态为 pending / downloading / paused）
    bool has_active_tasks() const;
    
    // 等待所有任务完成（阻塞调用，内部循环调用 update）
    void wait_for_all();
    
private:
    // 内部实现细节，此处仅声明类型
    struct impl;
    std::unique_ptr<impl> p_impl;
};

} // namespace cnt::minecraft::download