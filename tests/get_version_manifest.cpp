#include <minecraft/net.hpp>
#include <minecraft/manifest.hpp>

namespace mc = cnt::minecraft;

int main()
{
    mc::net::ServerNode node = mc::net::DefaultServerNode();
    try
    {
        mc::VersionManifest vm = mc::assets::get_manifest(node); // Use cache
        std::cout << vm;
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}