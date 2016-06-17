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
	if (fileFilter == NULL)ofn.lpstrFilter = "‚·‚×‚Ä‚Ìƒtƒ@ƒCƒ‹(*.*)\0*.*\0\0";
	fileName[0] = 0;
	return GetOpenFileName(&ofn);
}