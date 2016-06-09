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
#include <math.h>

#define PI 3.141592654

using namespace std;
std::vector<std::string> get_file_path_in_dir(const std::string& dir_name, const std::string& extension);
int getFileName(char* fileName, int fileNameLength, const char* fileFilter);
//class Fps;
class Fps {
	int mStartTime;         //測定開始時刻
	int mCount;             //カウンタ
	float mFps;             //fps
	static const int N = 60;//平均を取るサンプル数
	static const int FPS = 30;	//設定したFPS

public:
	Fps() {
		mStartTime = 0;
		mCount = 0;
		mFps = 0;
	}

	bool Update() {
		if (mCount == 0) { //1フレーム目なら時刻を記憶
			mStartTime = GetNowCount();
		}
		if (mCount == N) { //60フレーム目なら平均を計算する
			int t = GetNowCount();
			mFps = 1000.f / ((t - mStartTime) / (float)N);
			mCount = 0;
			mStartTime = t;
		}
		mCount++;
		return true;
	}

	void Draw() {
		DrawFormatString(0, 0, GetColor(255, 255, 255), "%.1f", mFps);
	}

	void Wait() {
		int tookTime = GetNowCount() - mStartTime;	//かかった時間
		int waitTime = mCount * 1000 / FPS - tookTime;	//待つべき時間
		if (waitTime > 0) {
			Sleep(waitTime);	//待機
		}
	}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#pragma region Values
	Fps fps;
	FILE *fp;
	CTime theTime;
	CFileTime tstart, tend;
	CFileTime tstart2, tend2; // てすと用
	CFileTimeSpan ctimep;
	string imgroot = "samples/";
	vector<std::string> filenames;

	unsigned int blankimg;
	unsigned int Handles[2048 * 16];     // データハンドル格納
	unsigned int Key = 0;
	unsigned char *Data;
	unsigned char *StrBuffer;

	int patch = 0;
	int i = 0;
	int graphSize;
	errno_t error;
	int Xbuf, Ybuf;
	bool bRAM_Fullbuffer = false;
	char filename[256];
	std::string ConfFile = "samples/";
	LONGLONG *frameinterval;
	frameinterval = (long long *)calloc(sizeof(long long), 10000);

	// 単位はフレーム (60Hzを想定)
	unsigned int patchSize = 100;
	unsigned int isif = 0;
	unsigned int durf = 1;
	unsigned int endf = 300;
	unsigned int Count = 0;
	unsigned int triNum = 32;
	int SizeX = 1920;
	int SizeY = 1080;
	bool WindowMode = false;
# pragma endregion

#pragma region Config-file
	// 設定ファイルの読み込み
	getFileName(filename, sizeof(filename), NULL);
	if (!(filename == ConfFile))
	{
		ConfFile = filename;
	}

#pragma endregion

#pragma region Log-writing
	// Log file
	std::ofstream wf;
	wf.open("ShowLog  .txt");
	wf << "Images: " << imgroot << endl;

	// 現在時刻
	theTime = CTime::GetCurrentTime();
	wf << theTime.Format("%Y%d%H%M") << endl;
#pragma endregion

#pragma region DXlib-initialize
	SetWaitVSyncFlag(TRUE);
	ChangeWindowMode(WindowMode); // ウィンドウモードに設定
	SetGraphMode(SizeX, SizeY, 32);
	SetBackgroundColor(128, 128, 128); // TODO, 輝度ファイルから読めるようにする
	if (DxLib_Init())
		return -1;   // DXライブラリ初期化処理
	ScreenFlip();
#pragma endregion


#pragma region  Buffering-Images
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
		
		// ファイル名だけをバッファするらしいが実行速度に影響しなかった．
		Handles[i] = LoadGraph((imgroot + filenames[i]).c_str());
		if (0 == i % 256) {
			ClearDrawScreen();
			DrawFormatString(0, 0, textc, "%d / %d : images", i, filenames.size());
			DrawFormatString(0, 15, textc, ConfFile.c_str(), filenames.size());
			ScreenFlip();
		}
		if (GetAsyncKeyState(VK_ESCAPE)) {
			ProcessMessage();
			ClearDrawScreen();
			DxLib_End();   // DXライブラリ終了処理
			return -1;
		}

	}
#pragma endregion

	// Blank
	ClearDrawScreen();
	blankimg = LoadGraph("blank.bmp");
	DrawFormatString(0, 0, textc, "READY: %d images, ", filenames.size());
	DrawFormatString(0, 15, textc, ("Configfile: " + ConfFile).c_str(), filenames.size());
	DrawFormatString(0, 30, textc, ("Images from: " + imgroot).c_str(), filenames.size());
	ScreenFlip();

	while (ProcessMessage() == 0)
	{
		if (CheckHitKey(KEY_INPUT_ESCAPE) != 0)
		{
			DxLib_End();   // DXLib end
			return -1;
		}
		if (CheckHitKey(KEY_INPUT_T) != 0)
			break;
	}
	
	//速度実験用
	ClearDrawScreen();

	for (size_t i = 0; i < 60; i++) {
		DrawRotaGraph(960, 600, 1, 0 * i % 360 * PI / 180, blankimg, FALSE);
		ScreenFlip();
		ClearDrawScreen();
	}

#pragma region Stimulus-loop

	for (size_t j = 0; j < triNum; j++) {
		// FPS測定用関数
		auto startTime = std::chrono::system_clock::now();
		tstart = CFileTime::GetCurrentTime();

		// 描画開始
		//while (i < filenames.size())
		ctimep = tstart - tstart;
		for (size_t i = 0; i < filenames.size(); i++)
		{
			// デバック
			printfDx("%d/%d", j, triNum);
			// DrawFormatString(0, 0, textc, "%d / %d : images", i, filenames.size()); //TODO TEST
			patch = 255;
			tstart2 = CFileTime::GetCurrentTime(); ;

			if (isif != 0) {
				DrawBox(0, 540, patchSize, 540 + patchSize, GetColor(0, 0, 0), TRUE);
				while (!ScreenFlip()) {
					Count++;
					if (Count >= isif) {
						Count = 0;
						break;
					}
				}
			}
			else {
				patch = 255+i%2;
			}
			DrawGraph(960-256, 600-256, Handles[i], FALSE);
			DrawBox(0, 540, patchSize, 540+patchSize, GetColor(patch, patch, patch), TRUE);
			//fps.Update();	//更新
			//fps.Draw();
			//DrawRotaGraph(Xbuf % 1920, Ybuf % 1080, 1, 0 * i % 360 * PI / 180, Handles[i], FALSE);
			while (!ScreenFlip()) {
				Count++;
				if (Count == durf) {
					Count = 0;
					break;
				}
				//wf << Count;
			}

			tend2 = CFileTime::GetCurrentTime();
			ctimep = tend2 - tstart2;
			frameinterval[i] = ctimep.GetTimeSpan();

			// 途中終了処理(ESCキー)
			if (GetAsyncKeyState(VK_ESCAPE)) {
				ProcessMessage();
				ClearDrawScreen();
				DxLib_End();   // DXライブラリ終了処理
				return -1;
			}
			//fps.Wait();		//待機
			ClearDrawScreen();
			clsDx();
		}

	
		i = 0;
		// FPS算出 & 記録
		const auto endTime = std::chrono::system_clock::now();
		tend = CFileTime::GetCurrentTime(); ;
		ctimep = tend - tstart;
		auto timeSpan = endTime - startTime;
		wf << filenames.size() << " [frames], ";
		wf << ctimep.GetTimeSpan() / 10000.0 << " [ms (ctime)], "; //[ms]
		wf << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << " [ms (chrono)], " ;
		wf << float(1000.0*filenames.size()) / (ctimep.GetTimeSpan() / 10000.0) << " [FPS]" << endl;
		//wf << float(filenames.size()) / float(std:chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << endl;
		Beep(440*1000, 100);
	}

#pragma endregion
	
	
	
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

	// デバック用領域
	for (size_t i = 0; i < filenames.size(); i++)
	{
		wf << frameinterval[i] << endl;
	}

#pragma region Finalize
	Beep(440 * 1000, 100);
	Beep(440 * 1000, 100);
	free(frameinterval);
	ProcessMessage();
	ClearDrawScreen();
	DxLib_End();
	wf.close();
	return 0;
#pragma endregion
}


// 画像ファイル一覧取得
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

// Filename from windows File-Dialog 
int getFileName(char* fileName, int fileNameLength, const char* fileFilter)
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
