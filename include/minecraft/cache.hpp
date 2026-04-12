#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace cnt::minecraft::cache {

// 前置声明
class CacheObject;

// 缓存管理器：负责设置全局缓存根目录，管理缓存文件
class CacheManager {
public:
    // 构造函数，指定缓存根目录
    explicit CacheManager(const std::filesystem::path& root_path);
    ~CacheManager();

    // 禁止拷贝，允许移动
    CacheManager(const CacheManager&) = delete;
    CacheManager& operator=(const CacheManager&) = delete;
    CacheManager(CacheManager&& other) noexcept;
    CacheManager& operator=(CacheManager&& other) noexcept;

    // 错误状态（如目录创建失败等）
    bool error() const;

    // 获取当前缓存根路径
    std::filesystem::path current_path() const;

    // 获取全局唯一的缓存管理器实例（可选，用于静态访问）
    static CacheManager& instance();

    // 设置/获取静态默认缓存根路径（供 CacheObject 无参构造使用）
    static void set_default_root(const std::filesystem::path& path);
    static std::filesystem::path default_root();

    // 清理所有缓存文件
    void clear_all();

    // 获取所有缓存对象名称列表
    std::vector<std::string> list_caches() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// 缓存对象：表示一个具体的缓存项，提供读写接口
class CacheObject {
public:
    // 构造函数，指定缓存名称（将基于 CacheManager 默认根路径）
    explicit CacheObject(const std::string& name);

    // 构造函数，指定缓存名称和自定义基础路径
    CacheObject(const std::string& name, const std::filesystem::path& base_path);

    ~CacheObject();

    // 移动语义支持
    CacheObject(CacheObject&& other) noexcept;
    CacheObject& operator=(CacheObject&& other) noexcept;

    // 禁止拷贝
    CacheObject(const CacheObject&) = delete;
    CacheObject& operator=(const CacheObject&) = delete;

    // 获取缓存文件的完整路径
    std::filesystem::path path() const;

    // 检查缓存文件是否存在
    bool exists() const;

    // 删除该缓存文件
    bool remove();

    // 模板方法：写入任意可流式输出的数据（覆盖写入）
    template <typename T>
    void write(const T& data) {
        static_assert(std::is_same_v<T, std::string> ||
                      std::is_arithmetic_v<T> ||
                      std::is_constructible_v<std::string, T>,
                      "Unsupported type for write(). Please use string or arithmetic types.");
        ensure_file_open_for_write();
        if (out_stream_) {
            *out_stream_ << data;
        }
    }

    // 针对 std::string 的特化或通用实现已在模板中处理

    // 保存写入的内容（关闭输出流，实际写入磁盘）
    void save();

    // 获取输入流用于读取（返回 std::ifstream 引用）
    std::ifstream& read();

    // 关闭读取流
    void close_read();

    // 关闭写入流（不保存）
    void discard();

    // 错误状态
    bool error() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;

    void ensure_file_open_for_write();
    void ensure_file_open_for_read();
};

} // namespace cnt::minecraft::cache