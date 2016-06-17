#pragma once
#include "Windows.h"
#include "DxLib.h"



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

};

