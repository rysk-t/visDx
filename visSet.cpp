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
	if (fileFilter == NULL)ofn.lpstrFilter = "設定ファイル(*.ini)\0*.ini\0\0";
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
		}
	} while (FindNextFile(hFind, &win32fd));

	FindClose(hFind);
	return file_names;
}

void visSet::SettingScreen(bool wmode, int sizex, int sizey, int bitn, bool vsync, int bg)
{
	ChangeWindowMode(wmode);
	SetGraphMode(sizex, sizey, bitn);
	SetBackgroundColor(bg, bg, bg);
	SetWaitVSyncFlag(vsync);
}




int visSet::loadIni(struct setting* myset, char *fileName)
{
	boost::property_tree::ptree pt;
	read_ini(fileName, pt);
	//read_ini(".\\Default.ini", pt);

	//[Display]
	myset->model = (pt.get_optional<std::string>("Display.model")).get();
	myset->sizeX = (pt.get_optional<int>("Display.sizeX")).get();
	myset->sizeY = (pt.get_optional<int>("Display.sizeY")).get();
	myset->rate  = (pt.get_optional<int>("Display.rate")).get();
	myset->nbit  = (pt.get_optional<int>("Display.nbit")).get();

	//[Stim]
	myset->shuffle = (pt.get_optional<int>("Stim.shuffle")).get();
	myset->interstim = (pt.get_optional<int>("Stim.interstim")).get();
	myset->duration   = (pt.get_optional<int>("Stim.duration")).get();
	myset->intertrial = (pt.get_optional<int>("Stim.intertrial")).get();
	myset->posX = (pt.get_optional<int>("Stim.posX")).get();
	myset->posY = (pt.get_optional<int>("Stim.posY")).get();
	myset->imgroot = (pt.get_optional<std::string>("Stim.imgroot")).get();
	myset->imgext = (pt.get_optional<std::string>("Stim.imgext")).get();
	myset->ntrial = (pt.get_optional<int>("Stim.ntrial")).get();
	myset->bgcolor = (pt.get_optional<int>("Stim.bgcolor")).get();
	

	//[Patch]
	myset->patch_Exist = (pt.get_optional<int>("Patch.show")).get();
	myset->patch_X = (pt.get_optional<int>("Patch.posX")).get();
	myset->patch_Y = (pt.get_optional<int>("Patch.posY")).get();
	myset->patch_Size = (pt.get_optional<int>("Patch.size")).get();

	//Debug
	myset->dbg_values = (pt.get_optional<int>("Debug.values")).get();
	myset->dbg_imgname = (pt.get_optional<int>("Debug.imgname")).get();
	myset->dbg_windowmode = (pt.get_optional<int>("Debug.windowmode")).get();
	return 1;
}

