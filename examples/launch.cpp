#include <minecraft/minecraft.hpp>

namespace mc = cnt::minecraft;

int main()
{
	try
	{
		if (!mc::index::exists("Default"))
			mc::CreateIndex("Default", "~/.minecraft");
		mc::Index index("Default");

		if (!index.exists("latest"))
		{
			mc::net::ServerNode node = mc::net::DefaultServerNode();
			try
			{
				mc::VersionManifest vm = mc::assets::get_manifest(node); // Use cache
				mc::VersionPackage vp = vm.latest().release();
				vp.set_name("latest");
				vp.download(index);
			}
			catch (std::exception &e)
			{
				std::cerr << "Error: " << e.what() << std::endl;
				return 1;
			}
		}

		mc::Instance instance = index.get("latest");
		mc::Account account("TaimWay");
		mc::Java java = mc::java_home();

		mc::Launch launch;
		launch.config()
			.index(index)
			.instance(instance)
			.account(account)
			.java(java);

		return launch.build().debug(FULL_DEBUG_MODE).run();
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}
