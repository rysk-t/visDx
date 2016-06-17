#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <atltime.h>
#include <chrono>
#include <malloc.h>
#include <math.h>
#include "visSet.h"

#define PI 3.141592654

using namespace std;
//class Fps;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#pragma region Values

	visSet vs;
	FILE *fp;
	CTime theTime;
	CFileTime tstart, tend;
	CFileTime tstart2, tend2; // てすと用
	CFileTimeSpan ctimep;
	string imgroot = "images/";
	vector<std::string> filenames;
	vector<std::string> act_filenames;

	unsigned int blankimg;
	unsigned int Handles[2048 * 16];     // データハンドル格納
	unsigned int sOrder[2048 * 16];     // データハンドル格納

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
	std::string ConfFile = "images/";
	LONGLONG *frameinterval;
	frameinterval = (long long *)calloc(sizeof(long long), 10000);

	// 単位はフレーム (60Hzを想定)
	unsigned int patchSize = 100;
	unsigned int isif = 15;
	unsigned int durf = 15;
	unsigned int endf = 120;
	unsigned int sFrames = 120;
	unsigned int Count = 0;
	unsigned int triNum = 1;
	int SizeX = 1920;
	int SizeY = 1080;
	bool WindowMode = false;

# pragma endregion

#pragma region Config-file
	// 設定ファイルの読み込み
	vs.getInitFileName(filename, sizeof(filename), NULL);
	vs.loadIni(vs.dataset, filename);
	//std::string debugmode = vs.dataset->imgroot;
	imgroot = imgroot;
#pragma endregion

#pragma region Log-writing
	// Log file
	std::ofstream wf;
	wf.open("ShowLog.txt");
	wf << "Images: " << imgroot << endl;

	// 現在時刻
	theTime = CTime::GetCurrentTime();
	wf << theTime.Format("%Y%d%H%M") << endl;
#pragma endregion

#pragma region DXlib-initialize
	vs.SettingScreen(WindowMode, SizeX, SizeY, 32, TRUE, 128);
	if (DxLib_Init())
		return -1;   // DXライブラリ初期化処理
	ScreenFlip();
#pragma endregion


#pragma region  Buffering-Images
	//Get file name 
	filenames = vs.getImgFiles(imgroot, "bmp");
	if (filenames.size() == 0)
		return -1;

	int textc = GetColor(0, 0, 0);
	int ary[ 2048 ]; // TODO
	// Shuffle

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
			DrawFormatString(0, 0, textc, "%d / %d : images, %s", i, filenames.size());
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
	DrawFormatString(0, 0, textc, "READY: %d images, ", filenames.size());
	DrawFormatString(0, 15, textc, ("Configfile: " + ConfFile).c_str());
	DrawFormatString(0, 30, textc, ("Images from: "));
	//DrawFormatString(0, 45, textc, (dataset->dbg_imgname));
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
	

#pragma region Stimulus-loop

	for (size_t j = 0; j < triNum; j++) {
		srand(j);
		act_filenames = filenames;
		for (int i = 0; i < filenames.size(); i++)
		{
			sOrder[i] = i;
		}
		for (int i = 0; i < filenames.size(); i++)
		{
			int r = rand() % filenames.size();
			int t = sOrder[i];
			sOrder[i] = sOrder[r];
			sOrder[r] = t;
		}
		for (int i = 0; i < filenames.size(); i++)
		{
			act_filenames[i] = filenames[sOrder[i]];
			Handles[i] = Handles[sOrder[i]];
		}
		ClearDrawScreen();
		for (size_t i = 0; i < sFrames; i++) {
			//DrawRotaGraph(960, 600, 1, 0 * i % 360 * PI / 180, blankimg, FALSE);
			ScreenFlip();
			ClearDrawScreen();
		}


		// FPS測定用関数
		auto startTime = std::chrono::system_clock::now();
		tstart = CFileTime::GetCurrentTime();

		// 描画開始
		//while (i < filenames.size())
		ctimep = tstart - tstart;
		for (size_t i = 0; i < filenames.size(); i++)
		{
			// デバック
			//printfDx("%d/%d: act_filename: %s", j+1, triNum, act_filenames[i].c_str());
			//DrawFormatString(0, 10, textc, "%s", filenames[i].c_str()); //TODO TEST
			patch = 255;
			tstart2 = CFileTime::GetCurrentTime(); ;

			if (isif != 0) {
				DrawBox(0, 0+SizeY - patchSize, patchSize, SizeY, GetColor(0, 0, 0), TRUE);
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
			//DrawGraph(960-256, 600-256, Handles[i], FALSE);
			DrawBox(0, 0 + SizeY-patchSize, patchSize, SizeY, GetColor(patch, patch, patch), TRUE);
			//fps.Update();	//更新
			//fps.Draw();
			DrawRotaGraph(SizeX/2, SizeY / 2, 1, 0, Handles[i], FALSE);
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
		for (size_t i = 0; i < filenames.size(); i++)
		{
			 wf << act_filenames[i].c_str() << endl;
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


