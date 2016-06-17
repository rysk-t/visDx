#pragma once
#include "Windows.h"
#include "DxLib.h"
#include <vector>



class visSet
{
public:
	visSet();
	~visSet();

	void Default();
	//void fromINI(char param);
	int load(std::string param);
	int testFunc();
	int getInitFileName(char* fileName, int fileNameLength, const char* fileFilter);
	std::vector<std::string> getImgFiles(const std::string& dir_name, const std::string& extension);
};

