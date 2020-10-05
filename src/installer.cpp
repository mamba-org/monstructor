#include <mamba/link.hpp>
#include <mamba/validate.hpp>

static std::string magic_numbers = "thisisaverylongstringthatshouldbereplacedbythebuildprocessabcdef";

void write_repodata_json(const fs::path& pkg_file,
						 const fs::path& pkg,
						 const std::string& url,
					     const std::string& channel,
						 const std::string& filename)
{
	fs::path repodata_record_path = pkg / "info" / "repodata_record.json";
	fs::path index_path = pkg / "info" / "index.json";

	nlohmann::json index, solvable_json;
	std::ifstream index_file(index_path);
	index_file >> index;

	// what's missing?!
	// SHA256SUM
	// MD5 SUM
	// ...

	index["sha256"] = validate::sha256sum(pkg_file);
	index["md5"] = validate::md5sum(pkg_file);
	index["size"] = fs::file_size(pkg_file);

	// solvable_json = solvable_to_json(m_solv);
	// merge those two
	// index.insert(solvable_json.cbegin(), solvable_json.cend());

	index["url"] = url;
	index["channel"] = channel;
	index["fn"] = filename;

	std::ofstream repodata_record(repodata_record_path);
	repodata_record << index.dump(4);
}

void link_to_prefix(const fs::path& prefix, const std::vector<std::string>& pkgs)
{
	// new transaction context without Python version!
	mamba::TransactionContext tc(prefix, "");

	// we need to find the root prefix, or use the new prefix as root prefix
	// to place & extract the packages at `$ROOT_PREFIX/pkgs`.
	fs::path root_prefix = mamba::Context::instance().root_prefix;
	if (root_prefix.empty())
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
		std::cout << "Handling " << pkg << std::endl;
		fs::path pkg_name = fs::path(pkg).filename();
		fs::path pkg_extr;

		// TODO check sha256!!!
		if (!fs::exists(root_prefix / "pkgs" / pkg_name))
		{
			fs::copy(pkg, root_prefix / "pkgs");
		}

		if (!fs::exists(root_prefix / mamba::strip_package_extension(pkg_name)))
		{
			pkg_extr = mamba::extract(root_prefix / "pkgs" / pkg_name);
		}

		extracted_pkgs.push_back(pkg_extr);
		std::cout << "Writing repodata_json " << (root_prefix / "pkgs" / pkg_name).string() << std::endl;
		write_repodata_json(
			root_prefix / "pkgs" / pkg_name,
			pkg_extr,
			"NONE",
			"NONE",
			"NONE");
	}

	for (auto& pkg : extracted_pkgs)
	{
		std::ifstream repodata_json(pkg / "info" / "repodata_record.json");
		nlohmann::json pkg_info_json;
		repodata_json >> pkg_info_json;
		mamba::PackageInfo pkg_info(std::move(pkg_info_json));
		mamba::LinkPackage lnk(pkg_info, root_prefix / "pkgs", &tc);
		lnk.execute();
	}
}

int main()
{
	std::cout << magic_numbers << std::endl;
	std::vector<std::string> sizes = mamba::split(magic_numbers, ";");
	int self_size = std::stoi(sizes[0].c_str());
	int json_size = std::stoi(sizes[1].c_str());
	int payload_size = std::stoi(sizes[2].c_str());

	std::cout << "Self exe size: " << self_size << " json size: " << json_size << " payload: " << payload_size;

	mamba::Context::instance().verbosity = 3;
	std::vector<std::string> pkgs = {
		"/home/wolfv/miniconda3/pkgs/xtensor-0.21.7-hc9558a2_0.tar.bz2"
	};

	link_to_prefix("/home/wolfv/miniconda3/envs/testxenv2/", pkgs);



	return 0;
}