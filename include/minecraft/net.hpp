#pragma once

#include <string>

namespace cnt::minecraft::net {
    // 服务器节点数据 （待添加）
    struct ServerNode {
        // 获取版本列表的地址
        std::string version_manifest;
    };

    // 默认服务器节点 (Mojang 官方节点)
    ServerNode DefaultServerNode();
}