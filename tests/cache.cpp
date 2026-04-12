#include <iostream>
#include <filesystem>
#include <minecraft/cache.hpp>

namespace mc = cnt::minecraft;

int main()
{
    std::cout << "0. Create the Cache Manager" << std::endl;
    mc::cache::CacheManager caches("~/minecraft-engine/cache");
    if (caches.error())
    {
        std::cerr << "Error: Cannot create the Cache Manager" << std::endl;
        return 1;
    }
    std::cout << std::endl;

    std::cout << "1. Get Cache current path" << std::endl;
    std::cout << "   Current Path: " << caches.current_path() << std::endl;
    std::cout << std::endl;

    std::cout << "2. Create a cache" << std::endl;
    {
        std::string content = "Hello! This is a chache content for utf-8 text";
        std::cout << "Content: " << content << std::endl;

        mc::cache::CacheObject obj("example_cache");
        obj.write<std::string>(content); // freopen
        obj.save();

        std::cout << "Save:    " << obj.path() << std::endl;
    }
    std::cout << std::endl;

    std::cout << "3. Read the cache" << std::endl;
    {
        mc::cache::CacheObject obj("example_cache");
        std::cout << "Content: " << std::endl;
        std::string line;
        while (std::getline(obj.read(), line))
            std::cout << line << '\n';
    }
    std::cout << std::endl;

    // ...

    std::cout << "Test completed!" << std::endl;
}
