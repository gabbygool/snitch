#include "file.h"

/**
 * Sends an error message and terminates the program.
 */
void file::error(std::string message)
{
	std::cout << "[Snitch]: " << message << std::endl;
	exit(1);
}

/**
 * Returns the contents of a given file.
 */
std::string file::get_file_contents(std::string path)
{
	std::ifstream file(path);

	if(file)
	{
		file.seekg(0, std::ios::end);
		uint8_t size = file.tellg();

		std::string contents(size, ' ');
		file.seekg(0);
		file.read(&contents[0], size);

		file.close();
		return contents;
	}

	else 
		error("File does not exist.");
}

/**
 * Lists all the files that are inside a given directory.
 * Snitch skips the .git/ directory because we don't want to look for TODO comments in there.
 */
std::vector<std::string> file::get_contents_in_directory(std::string path)
{
	std::vector<std::string> directory_list;

	for(std::filesystem::directory_entry entry : std::filesystem::directory_iterator(path)) 
	{
		std::string dir_path = entry.path();
		
		if(dir_path.find(".git") != std::string::npos)
			continue;

		else 
			directory_list.push_back(dir_path);
	}

	return directory_list;
}

/**
 * Returns the path to the git config file. 
 * We use the git config file to find the github repository url.
 */
std::string file::get_git_config_file_path(std::string path)
{
	char last_char = path[path.size() - 1];
	if(last_char == '/')
	{
		if(std::filesystem::exists(path + ".git/config"))
			return path + ".git/config";
		else
			error("Cannot find git config file.");
	}
	else
	{
		if(std::filesystem::exists(path + "/.git/config"))
			return path + "/.git/config";
		else
			error("Cannot find git config file.");
	}
}

/**
 * Formats and returns the github repository url from the git config file as an API url.
 * e.g. "https://api.github.com/repos/gabrielgavrilov/snitch/issues"
 */
std::string file::get_github_repo_url(std::string path)
{
	std::ifstream input(path);
	for(std::string line; getline(input, line);)
	{
		if(line.find("url") != std::string::npos)
		{
			line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
			std::string url = line.substr(line.find("=") + 1);
			
			url.erase(url.length()-4);
			url.insert(8, "api.");
			url.insert(23, "repos/");
			url += "/issues";
			
			return url;
		}
	}
}

