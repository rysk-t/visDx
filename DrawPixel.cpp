#include "DxLib.h"
#include "Windows.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <atltime.h>
#include <chrono>
#include <malloc.h>
#include <stdio.h>

using namespace std;

std::vector<std::string> get_file_path_in_dir(const std::string& dir_name, const std::string& extension);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	FILE *fp;
	CTime theTime;
	CFileTime tstart, tend;
	CFileTimeSpan ctimep;
	string imgroot = "samples/";
	vector<std::string> filenames;

	unsigned int blankimg;
	unsigned int Handles[2048 * 16];     // データハンドル格納
	unsigned int FullHandle[2048 * 16];
	unsigned char *Data;
	int graphSize;


	unsigned int isif = 0;
	unsigned int durf = 1;
	unsigned int endf = 60;
	unsigned int Count = 0;
	unsigned int i = 0;
	int SizeX = 1920;
	int SizeY = 1200;

	// Log file
	std::ofstream wf;
	wf.open("ShowLog.txt");
	wf << "Images: " << imgroot << endl;

	// 現在時刻
	theTime = CTime::GetCurrentTime();
	wf << theTime.Format("%Y%d%H%M") << endl;

	ChangeWindowMode(false); // ウィンドウモードに設定
	SetGraphMode(SizeX, SizeY, 32);
	SetBackgroundColor(128, 128, 128);
	if (DxLib_Init())
		return -1;   // DXライブラリ初期化処理



	//Get file name 
	filenames = (get_file_path_in_dir(imgroot, "bmp"));
	for (size_t i = 0; i < filenames.size(); i++)
	{
		//Handles[i] = LoadGraph((imgroot + filenames[i]).c_str());
		//wf << filenames[i] << endl;

		fp = fopen((imgroot + filenames[i]).c_str(), "rb");
		fseek(fp, 0L, SEEK_END);
		graphSize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		Data = (unsigned char *)malloc(graphSize);

		FullHandle[i] = FileRead_fullyLoad((imgroot + filenames[i]).c_str());
		FileRead_fullyLoad_getImage(FullHandle[i]);
	}

	// Blank
	blankimg = LoadGraph("blank.bmp");
	int textc = GetColor(0, 0, 0);
	DrawFormatString(0, 0, textc, "READY: %d images", filenames.size());
	//WaitKey();
	ClearDrawScreen();
	Sleep(100);
	Beep(440*1000, 100);
	//速度実験用

	for (size_t j = 0; j < 5; j++) {
		// FPS測定用関数
		const auto startTime = std::chrono::system_clock::now();
		tstart = CFileTime::GetCurrentTime();

		// 描画開始
		//while (i < filenames.size())
		for (size_t i = 0; i < filenames.size(); i++)
		{
			ClearDrawScreen();
			if (isif != 0) {
				while (!ScreenFlip()) {
					Count++;
					if (Count >= isif) {
						//1秒たった時の処理
						Count = 0;
						break;
					}
				}
			}
			DrawRotaGraph(960, 600, 1, 0., Handles[i], FALSE);
			while (!ScreenFlip()) {
				Count++;
				if (Count == durf) {
					Count = 0;
					break;
				}
			}

			// こっちは劇的に遅い
			/*while (!ScreenFlip() && Count < durf) {
				Count++;
			}; Count = 0;
			//*/
			i++;
		}
		i = 0;
		// FPS算出 & 記録
		const auto endTime = std::chrono::system_clock::now();
		tend = CFileTime::GetCurrentTime(); ;
		ctimep = tend - tstart;
		const auto timeSpan = endTime - startTime;
		//wf << filenames.size() << " [frames]" <<endl;
		//wf << ctimep.GetTimeSpan() / 10000 << endl; //[ms]
		//wf << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << " [ms]" << std::endl;
		wf << float(1000.0*filenames.size()) / (ctimep.GetTimeSpan() / 10000) << " [FPS]" << endl;
		//wf << float(filenames.size()) / float(std:chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << endl;
		Beep(440*1000, 100);
	}//速度実験用

	// 最後にendframe分だけまつ
	Count = 0;
	while (!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen()) {
		Count++;
		if (Count == endf) {
			//1秒たった時の処理
			Count = 0;
			break;
		}
	}

	Beep(440 * 1000, 1000);
	ProcessMessage();
	ClearDrawScreen();


	//WaitKey();     // キー入力があるまで待機
	DxLib_End();   // DXライブラリ終了処理
	//wf << filenames.size() << endl;
	for (size_t i = 0; i < filenames.size(); i++)
	{
		FileRead_fullyLoad_delete(FullHandle[i]);
	}

	wf.close();
	return 0;
}


std::vector<std::string> get_file_path_in_dir(const std::string& dir_name, const std::string& extension) noexcept(false)
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
