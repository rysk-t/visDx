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
#define PI 3.141592654

using namespace std;
std::vector<std::string> get_file_path_in_dir(const std::string& dir_name, const std::string& extension);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	FILE *fp;
	CTime theTime;
	CFileTime tstart, tend;
	CFileTimeSpan ctimep;
	string imgroot = "samples/";
	vector<std::string> filenames;

	unsigned int blankimg;
	unsigned int Handles[2048 * 16];     // データハンドル格納
	unsigned char *Data;
	int graphSize;
	errno_t error;

	unsigned int isif = 0;
	unsigned int durf = 1;
	unsigned int endf = 60;
	unsigned int Count = 0;
	unsigned int triNum = 10;
	int i = 0;
	int SizeX = 1920;
	int SizeY = 1200;
	int Xbuf, Ybuf;
	bool bRAM_Fullbuffer = false;


	// Log file
	std::ofstream wf;
	wf.open("ShowLog.txt");
	wf << "Images: " << imgroot << endl;

	// 現在時刻
	theTime = CTime::GetCurrentTime();
	wf << theTime.Format("%Y%d%H%M") << endl;

	ChangeWindowMode(true); // ウィンドウモードに設定
	SetGraphMode(SizeX, SizeY, 32);
	SetBackgroundColor(128, 128, 128);
	if (DxLib_Init())
		return -1;   // DXライブラリ初期化処理



	//Get file name 
	filenames = (get_file_path_in_dir(imgroot, "bmp"));
	int textc = GetColor(0, 0, 0);
	for (size_t i = 0; i < filenames.size(); i++)
	{
		//2GB 以上はバッファできない (32bit)
	/*	error = fopen_s(&fp, (imgroot + filenames[i]).c_str(), "rb");
		fseek(fp, 0L, SEEK_END);
		graphSize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		Data = (unsigned char *)malloc(graphSize);
		fread(Data, graphSize, 1, fp);
		Handles[i] = CreateGraphFromMem(Data, graphSize);
		fclose(fp);
		free(Data);*/
		

		Handles[i] = LoadGraph((imgroot + filenames[i]).c_str());
		if (0 == i % 256) {
			ClearDrawScreen();
			DrawFormatString(0, 0, textc, "%d / %d : images", i, filenames.size());
			ScreenFlip();
		}

	}

	// Blank
	ClearDrawScreen();
	blankimg = LoadGraph("blank.bmp");
	DrawFormatString(0, 0, textc, "READY: %d images", filenames.size());
	ScreenFlip();

	WaitKey();
	Beep(440*1000, 100);
	//速度実験用

	for (size_t i = 0; i < 60; i++) {
		ClearDrawScreen();
		DrawRotaGraph(960, 600, 1, 0 * i % 360 * PI / 180, blankimg, FALSE);
		ScreenFlip();
	}

	for (size_t j = 0; j < triNum; j++) {
		// FPS測定用関数
		//auto startTime = std::chrono::system_clock::now();
		tstart = CFileTime::GetCurrentTime();

		// 描画開始
		//while (i < filenames.size())
		for (size_t i = 0; i < filenames.size(); i++)
		{

			//GetMousePoint(&Xbuf, &Ybuf);
			//clsDx();
			//DrawFormatString(0, 0, textc, "%d / %d : images", i, filenames.size()); //TODO TEST
			if (isif != 0) {
				while (!ScreenFlip()) {
					Count++;
					if (Count >= isif) {
						Count = 0;
						break;
					}
				}
			}
			DrawGraph(960, 600, Handles[i], FALSE);
//			DrawRotaGraph(Xbuf % 1920, Ybuf % 1080, 1, 0 * i % 360 * PI / 180, Handles[i], FALSE);
			while (!ScreenFlip()) {
				Count++;
				if (Count == durf) {
					Count = 0;
					break;
				}
				wf << Count;
			}
			ClearDrawScreen();
			//tend = CFileTime::GetCurrentTime(); ;
			//ctimep = tend - tstart;
			//printfDx("%f", 1000.0/ctimep.GetTimeSpan());
		}

		i = 0;
		// FPS算出 & 記録
		const auto endTime = std::chrono::system_clock::now();
		tend = CFileTime::GetCurrentTime(); ;
		ctimep = tend - tstart;
//		auto timeSpan = endTime - startTime;
		wf << filenames.size() << " [frames], ";
		wf << ctimep.GetTimeSpan() / 10000.0 << " [ms (ctime)], "; //[ms]
		//wf << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << " [ms (chrono)], " ;
		wf << float(1000.0*filenames.size()) / (ctimep.GetTimeSpan() / 10000.0) << " [FPS]" << endl;
		//wf << float(filenames.size()) / float(std:chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << endl;
		Beep(440*1000, 100);
	}

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

	Beep(440 * 1000, 100);
	Beep(440 * 1000, 100);
	ProcessMessage();
	ClearDrawScreen();


	//WaitKey();     // キー入力があるまで待機
	DxLib_End();   // DXライブラリ終了処理
	//wf << filenames.size() << endl;
	for (size_t i = 0; i < filenames.size(); i++)
	{
	}

	wf.close();
	return 0;
	FreeConsole();//コンソールの解放

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
