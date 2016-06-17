#ifndef MY_INCLUDE
#define MY_INCLUDE
#pragma once
#include "Windows.h"
#include "DxLib.h"
#include <vector>
#include <stdlib.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/optional.hpp>
#include <stdio.h>
#include <conio.h>
#include "VisSet.h"

#endif

class visSet
{
public:
	typedef struct {
		//[Display]
		//char model[MAX_PATH];
		int sizeX;
		int sizeY;
		int rate; //Reflesh Rate of Display
		
		//[Stim]
		int interf;
		int durf;
		int ntrial;
		int posX;
		int	posY;
		std::string imgroot="samples";

		//[Patch]
		bool patch_Exist; //bool of patch showing
		int patch_X;
		int patch_Y;
		int patch_Size;

		//[Debug]
		bool dbg_values;
		bool dbg_imgname;

	}SETTING;


	visSet();
	~visSet();

	SETTING *dataset = NULL;
	//dataset = (SETTING *)malloc(sizeof(SETTING)); //�������m��

	void SettingScreen(bool wmode, int sizex, int sizey, int bitn, bool vsync, int bg);

	int getInitFileName(char* fileName, int fileNameLength, const char* fileFilter);
	std::vector<std::string> getImgFiles(const std::string& dir_name, const std::string& extension);

	int loadIni(SETTING *myset, char *fileName);
	int showSetting(visSet::SETTING *myset);
};
