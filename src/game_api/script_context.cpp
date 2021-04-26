#include "script_context.hpp"

#include <fstream>

MakeSavePathCallback g_MakeSavePathCallback{ nullptr };

void register_make_save_path(MakeSavePathCallback make_save_path_callback)
{
	g_MakeSavePathCallback = make_save_path_callback;
}

SaveContext::SaveContext(std::string_view _script_path, std::string_view _script_name)
	: script_path{ _script_path }, script_name{ _script_name }
{}
bool SaveContext::Save(std::string data) const
{
	const auto save_file_path = g_MakeSavePathCallback(script_path, script_name);
	if (auto data_file = std::ofstream{ save_file_path })
	{
		data_file << data << std::flush;
		return data_file.good();
	}
	return false;
}

LoadContext::LoadContext(std::string_view _script_path, std::string_view _script_name)
	: script_path{ _script_path }, script_name{ _script_name }
{}
std::string LoadContext::Load() const
{
	std::string data;

	const auto save_file_path = g_MakeSavePathCallback(script_path, script_name);
	if (auto data_file = std::ifstream{ save_file_path })
	{
		data_file.seekg(0, std::ios::end);
		data.reserve(data_file.tellg());
		data_file.seekg(0, std::ios::beg);

		data.assign((std::istreambuf_iterator<char>(data_file)),
			std::istreambuf_iterator<char>());
	}

	return data;
}
