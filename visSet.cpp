#include "visSet.h"


visSet::visSet()
{
}


visSet::~visSet()
{
}

int visSet::getInitFileName(char* fileName, int fileNameLength, const char* fileFilter)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetMainWindowHandle();
	ofn.lpstrFilter = fileFilter;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = fileNameLength;
	ofn.Flags = OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	if (fileName == NULL)return 0;
	if (fileFilter == NULL)ofn.lpstrFilter = "すべてのファイル(*.*)\0*.*\0\0";
	fileName[0] = 0;
	return GetOpenFileName(&ofn);
}

std::vector<std::string> visSet::getImgFiles(const std::string& dir_name, const std::string& extension) noexcept(false)
{
	HANDLE hFind;
	WIN32_FIND_DATA win32fd;//defined at Windwos.h
	std::vector<std::string> file_names;

	//拡張子の設定
	std::string search_name = dir_name + "\\*." + extension;

	hFind = FindFirstFile(search_name.c_str(), &win32fd);

	if (hFind == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("file not found");
	}

	do {
		if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		}
		else {
			file_names.push_back(win32fd.cFileName);
			//printf("%s\n", file_names.back().c_str());

		}
	} while (FindNextFile(hFind, &win32fd));

	FindClose(hFind);
	return file_names;
}
