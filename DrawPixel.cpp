#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <atltime.h>
#include <chrono>
#include <malloc.h>
#include <math.h>
#include "visSet.h"
#include <random>
#include <algorithm>
#define PI 3.141592654

using namespace std;
//class Fps;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#pragma region Values

	visSet vs;
	CTime theTime;
	CFileTime tstart, tend;
	CFileTime tstart2, tend2; // てすと用
	CFileTimeSpan ctimep;
	std::ofstream wf;

	vector<std::string> filenames;
	vector<std::string> act_filenames;

	unsigned int Handles[2048 * 16];     // データハンドル格納
	unsigned int sOrder[2048 * 16];     // データハンドル格納

	unsigned int Key = 0;
	//unsigned char *Data;
	//unsigned char *StrBuffer;

	int patch = 0;
	int i = 0;
	errno_t error;
	bool bRAM_Fullbuffer = false;
	char cfile[256];
	std::string ConfFile = cfile;
	LONGLONG *frameinterval;
	frameinterval = (long long *)calloc(sizeof(long long), 10000);
	auto endTime = std::chrono::system_clock::now();

	//int frameintervalMat[10000][10000];

	// 単位はフレーム (60Hzを想定)
	unsigned int endf = 120;
	unsigned int Count = 0;

	bool dbg_mode = false;

	visSet::setting vSetVals;

	# pragma endregion

	#pragma region Config-file
	// 設定ファイルの読み込み
	if (!vs.getInitFileName(cfile, sizeof(cfile), NULL))
		return -1;
	ConfFile = cfile;
	vs.loadIni(&vSetVals, cfile);
	#pragma endregion

#pragma region Log-writing
	// Log file
	theTime = CTime::GetCurrentTime();

	wf.open("ShowLog_" + theTime.Format("%Y%m%d%H%M") + ".txt");
	wf << "Images: " << vSetVals.imgroot << endl;

	// 現在時刻
	wf << theTime.Format("%Y%m%d%H%M") << endl;
#pragma endregion

#pragma region DXlib-initialize
	vs.SettingScreen(vSetVals.dbg_windowmode,
		vSetVals.sizeX,
		vSetVals.sizeY,
		vSetVals.nbit,
		TRUE, // vsync option
		vSetVals.bgcolor);

	if (DxLib_Init())
		return -1;   // DXライブラリ初期化処理
	SetDrawScreen(DX_SCREEN_BACK);
	ScreenFlip();
#pragma endregion


#pragma region  Buffering-Images
	//Get file name 
	filenames = vs.getImgFiles(vSetVals.imgroot +"\\", vSetVals.imgext);
	if (filenames.size() == 0)
		return -1;

	int textc = GetColor(255, 0, 0);
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
		Handles[i] = LoadGraph((vSetVals.imgroot + filenames[i]).c_str());
		if (0 == i % 256) {
			ClearDrawScreen();
			DrawFormatString(0, 0, textc, "%d / %d : images, %s", i, filenames.size(), filenames[i]);
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
	DrawFormatString(0, 0, textc, "READY: %d images, PRESS T for start (D for test)", filenames.size());
	DrawFormatString(0, 15, textc, ("Configfile: " + ConfFile).c_str());
	DrawFormatString(0, 30, textc, ("Images from: "+ vSetVals.imgroot).c_str());
	DrawFormatString(0, 45, textc, "Number of iteration: %d", vSetVals.ntrial);
	DrawFormatString(0, 60, textc, "Estimated Recording Duration: %f [sec]",
		(1.0 / vSetVals.rate)*vSetVals.ntrial*(
			filenames.size()*(vSetVals.interstim+vSetVals.duration) + 
			vSetVals.intertrial));


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
		{
			break;
		}
		else if (CheckHitKey(KEY_INPUT_D) != 0) 
		{
			dbg_mode = true;
			break;
		}


	}
	

#pragma region Stimulus-loop
	vector<std::int16_t> fileidx(filenames.size());
	std::random_device rnd;
	std::mt19937 mt(rnd());
	auto startTime = std::chrono::system_clock::now();

	for (size_t j = 0; j < vSetVals.ntrial; j++) {
		act_filenames = filenames;

		if (vSetVals.shuffle)
		{
			srand(j*100);
			for (int i = 0; i != filenames.size(); ++i) fileidx[i] = i;
			std::random_shuffle(fileidx.begin(), fileidx.end());
		}
		else
		{
			for (int i = 0; i != filenames.size(); ++i) fileidx[i] = i;
		}


		for (int i = 0; i < filenames.size(); i++)
		{
			act_filenames[i] = filenames[fileidx[i]];
		}

		ClearDrawScreen();
		for (size_t i = 0; i < vSetVals.intertrial; i++) {
			//DrawRotaGraph(960, 600, 1, 0 * i % 360 * PI / 180, blankimg, FALSE);
			ScreenFlip();
			ClearDrawScreen();
		}


		// FPS測定用関数
		startTime = std::chrono::system_clock::now();
		tstart = CFileTime::GetCurrentTime();

		// 描画開始
		//while (i < filenames.size())
		ctimep = tstart - tstart;
		for (size_t i = 0; i < filenames.size(); i++)
		{
			
			//printfDx("%d", fileidx[i]);
			// デバック
			//printfDx("%d/%d: act_filename: %s", j+1, vSetVals.ntrial, act_filenames[i].c_str());
			//DrawFormatString(0, 10, textc, "%s", filenames[i].c_str()); //TODO TEST
			patch = 255;
			tstart2 = CFileTime::GetCurrentTime(); ;


			// patch Exist?
			switch (vSetVals.patch_Exist)
			{
			case 0:
				break;
			case 1:
				if (vSetVals.interstim != 0) {
					DrawBox(vSetVals.patch_X, vSetVals.patch_Y,
						vSetVals.patch_X + vSetVals.patch_Size, vSetVals.patch_Y + vSetVals.patch_Size,
						GetColor(0, 0, 0), TRUE);

					while (!ScreenFlip()) {
						Count++;
						if (Count >= vSetVals.interstim) {
							Count = 0;
							break;
						}
					}
				}
				else {
					patch = 255 + i % 2;
				}
				DrawBox(vSetVals.patch_X, vSetVals.patch_Y,
					vSetVals.patch_X + vSetVals.patch_Size, vSetVals.patch_Y + vSetVals.patch_Size,
					GetColor(patch, patch, patch), TRUE);
			}

			DrawRotaGraph(vSetVals.posX, vSetVals.posY, 1, 0, Handles[fileidx[i]], FALSE);
			while (!ScreenFlip()) {
				Count++;
				if (Count == vSetVals.duration) {
					Count = 0;
					break;
				}
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
		// FPS算出 & 記録
		endTime = std::chrono::system_clock::now();
		tend = CFileTime::GetCurrentTime(); ;
		ctimep = tend - tstart;
		auto timeSpan = endTime - startTime;
		wf << "# Trial:" << j+1 << " ";
		wf << filenames.size() << " [frames], ";
		wf << ctimep.GetTimeSpan() / 10000.0 << " [ms (ctime)], "; //[ms]
		wf << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << " [ms (chrono)], " ;
		wf << float(1000.0*filenames.size()) / (ctimep.GetTimeSpan() / 10000.0) << " [images/sec]" << endl;
		//wf << float(filenames.size()) / float(std:chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << endl;
		for (size_t i = 0; i < filenames.size(); i++)
		{
			wf << act_filenames[i].c_str() << ", " << frameinterval[i] << endl;
		}
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


