#include <mamba/link.hpp>

void link_to_prefix(const fs::path& prefix, const std::vector<std::string>& pkgs)
{
	// new transaction context without Python version!
	TransactionContext tc(prefix, "");

	// we need to find the root prefix, or use the new prefix as root prefix
	// to place & extract the packages at `$ROOT_PREFIX/pkgs`.
	fs::path root_prefix = Context::instance().root_prefix;
	if (root_prefix.size() == 0)
	{
		root_prefix = prefix;
	}
	if (!fs::exists(root_prefix / "pkgs"))
	{
		fs::create_directories(root_prefix / "pkgs");
	}

	std::vector<fs::path> extracted_pkgs;
	for (auto& pkg : pkgs)
	{
		fs::path pkg_name = fs::path(pkg).filename(); 
		// TODO check sha256!!!

		if (!fs::exists(root_prefix / pkg_name))
		{
			fs::copy(pkg, root_prefix / "pkgs");
		}

		if (!fs::exists(root_prefix / mamba::strip_package_extension(pkg_name)))
		{
			mamba::extract(root_prefix / "pkgs" / pkg_name);
		}
		extracted_pkgs.push_back(root_prefix / "pkgs" / pkg_name);
	}

	// We need to write repodata.json ...
	// Question is: HOW!?

	for (auto& pkg : extracted_pkgs)
	{
		std::ifstream(pkg / "info" / "index.json");
		nlohmann_json pkg_info_json 
		PackageInfo()
	}
}


int main()
{
	std::vector<std::string> pkgs = {
		"/home/wolfv/miniconda3/pkgs/xtensor-0.21.7-hc9558a2_0.tar.bz2"
	};

	link_to_prefix("/home/wolfv/miniconda3/envs/testxenv2/");
	return 0;
}