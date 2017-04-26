//#ifndef MY_INCLUDE
//#define MY_INCLUDE
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
#include <string>

#include "VisSet.h"

//#endif

class visSet
{
public:
	struct setting {
		//[Display]
		std::string model;
		int sizeX;
		int sizeY;
		int rate; //Reflesh Rate of Display
		int nbit;
		
		//[Stim]
		bool shuffle;
		int interstim;
		int duration;
		unsigned int ntrial;
		int intertrial;
		int posX;
		int	posY;
		int bgcolor;
		std::string imgroot;
		std::string imgext;
		bool seq_file;
		double magni;

		//[Patch]
		bool patch_Exist; //bool of patch showing
		int patch_X;
		int patch_Y;
		int patch_Size;

		//[Debug]
		bool dbg_windowmode;
	};

	int Count = 0;
	int Suc = 0;
	float ctimespan;
	visSet();
	~visSet();

	void SettingScreen(bool wmode, int sizex, int sizey, int bitn, bool vsync, int bg);

	int getInitFileName(char* fileName, int fileNameLength, const char* fileFilter);

	std::vector<std::string> getImgFiles(const std::string& dir_name, std::string& extension);

	void SettingScreen(bool wmode, int sizex, int sizey, int bitn, bool vsync, int bg, int rate=60);

	int loadIni(struct setting* myset, char *fileName);

	int showPatch(int x, int y, int size, unsigned int Col, int durf, bool fill);

	int WaitFramesDraw(int durf);
	
	int showDebugInfo(bool debugF, unsigned int colorhandle, std::string filename, LONGLONG timespan, int iter, int imgnum);

};

