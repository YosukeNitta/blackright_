//インクルード部--------------------------------------------
#include "pch.h"
#include "vector"
//using namespace std;

//#include <list>
//using namespace std;
#include "Select.h"
#include "MainSystem.h"

// キャラセレの動いてるキャラ
typedef struct Move_t{

	//int time;	// アニメーション時間
	Name_t name;
	double sizeG;	// 描画サイズ
	Air air;	// アニメーションせってい

	Move_t(){
		//time = 0;
		// name
		sizeG = 0;
	}

}Move_t;

// パレット
typedef struct {
public:
	int r[256];
	int g[256];
	int b[256];
}Pallet_t;

static boolean Load_1;
static boolean FLoad;	// 一度だけのロード
// キャラセレ画面
static int m_P1, m_P2;
static int BMode;	// 0.対戦　1.トレモ  2.VSAI  3.アーケード

static int back;
// 決定、決定の回数、上下左右のカーソル、ステージ番号、カラー
static int kettei;
static int ketteiNum;	// これが2になったら開始、0.キャラセレ 1.ステージ
static int StageNum;

// ステージ画像
static int _drawStagePosX;	// ずらす位置
static float _drawStageSize;	// 表示サイズ
// 立ち絵画像
//static int stand;
// 演出の時間停止
static int _timeStop;

// 選択ポインタの位置初期化

// キャラ・カーソル画像
static int cursor_char;
static int side[2]; //1P, 2P

static std::vector<int>port;	// キャラセレでの位置
//static int PortBlank;	// なし
static std::vector<int>charPos;	// キャラセレでの位置

//
static int gK[10];
static int oCurY[2];


/// 仮表示パレット ///
static std::vector<int>color[COLOR_MAX];

// このへんをvectorに
static std::vector<Move_t>move;				// 画面内で動かす

static Pict pic[CHARACTERS_MAX][IMAGE2_MAX];	// 0番の画像
//static std::vector<Pict>pic[IMAGE2_MAX];
static Pallet_t cpal[CHARACTERS_MAX][COLOR_MAX];// パレット

static void InputSelect();	// 変数・Select_tの設定
//static void Draw();	// セレクトの描画
static void EnterSelect();	// 操作・決定
static int InputSide(int PSide, int key);	// P1_BInputの両対応版
static void CharAnimetion();	// 待機中のアニメ


//基本データの構造体
typedef struct {
	
	int posX, posY;	// カーソルの表示位置
	int curX, curY;	// 上下、左右の座標
	boolean enter;		// 決定
	boolean charEnter;		// キャラ決定
	int name;		// キャラ名
	int color;		// カラー

	// 動くグラ用
	int image;
	int time;	// 画像のアニメ時間
	float drawPosX;	// キャラ表示位置
	int iNum;	// 画像の横番号
}Select_t;

static Select_t P[2];
static Select_t mP;	// ダミー

//キャラクターセレクト
int Select::Mode(void)
{
	// プログラム開始
	{
		//==========
		// 入力
		//==========
		{
			InputSelect();
		}
		//==========
		// 計算
		//==========
		{
			EnterSelect();
		}


		// アーケード
		// キャラ・ステージが決定したら
		if ((P[0].charEnter) && (BMode == 3))
		{
			Arcade_GetData(P[0].name, P[0].color);	// データを送る
			Arcade_Setting();
			if (Arcade_Switch(-1) == 1){
				MainSystem::Instance().SetNextMode("Arcade");
			}
			else{
				//ModeChange(SceneNum(VersusScene));	// 対戦画面へ
				MainSystem::Instance().SetNextMode("Versus");
			}
		}

		// キャラ・ステージが決定したら
		if (ketteiNum == 2)
		{
			//ModeChange(SceneNum(VersusScene));	// 対戦画面へ
			MainSystem::Instance().SetNextMode("Versus");
		}

	}

	return 0;	//終了

}


// 毎回初期化する設定
void Select::Load_Reload()
{
	Load_1 = 0;
	Anten(255);

	// キャラセレBGM
	BGMStart(12);

	// 決定、決定の回数、上下左右のカーソル、ステージ番号、カラー
	kettei = 0, ketteiNum = 0;
	//StageNum = 0;
	// 時間停止
	_timeStop = 0;


	// 選択ポインタの位置初期化
	for (int i = 0; i < 2; i++){
		P[i].name = 0;
		P[i].color = 1; //これだけ一回
		P[i].enter = false;
		P[i].charEnter = false;
		P[i].posY = 0;
		P[i].drawPosX = 0;
	}
	mP = P[1];	// ダミー用意
}

void Select::Load_1second(){
	// 一度きりのロード
	// 画像読み込み
	back = LoadGraph("ob/selectB.png");

	side[0] = LoadGraph("ob/1p.png");
	side[1] = LoadGraph("ob/2p.png");

	// キャラ・カーソル画像
	cursor_char = LoadGraph("ob/selectC.png");
	// 立ち絵
	//stand = LoadGraph("ob/stand2sample.png");

	// キャラセレ絵
	// 名前準備
	std::string fname2, fn1, fn2;
	fn1 = "char/";
	//fn2 = "/pal/port_s.png";
	fn2 = "/pal/port.bmp";

	for (int i = 0; i < Character_Max(); i++){
		charPos.push_back(i);	// charPos
		port.push_back(0);		// port
		fname2 = fn1 + move[i].name.nameC + fn2;	// 名前決定

		// 透過色
		int tr, tg, tb, palName;

		palName = LoadSoftImage(fname2.c_str());
		// パレットの色を取得する 1〜16, 17〜
		GetPaletteSoftImage(palName, 0, &tr, &tg, &tb, 0);
		// 透過色を取るために使ったイメージを削除
		DeleteSoftImage(palName);

		// 透過色を変更
		SetTransColor(tr, tg, tb);
		
		port[i] = LoadGraph(fname2.c_str());	// 画像ロード
	}

	// カーソル位置
	P[0].curX = 2, P[1].curX = 3;
	P[0].posX = P[0].curX * 110 - 110, P[1].posX = P[1].curX * 110 - 110;
	P[0].curY = 0, P[1].curY = 0;
	StageNum = 0;

	// 上下キー
	LoadDivGraph("ob/key.png", 10, 5, 2, 20, 20, gK);

	//char fname[TX_MAX];
	// 名前読み込み
	for (int cm = 0; cm < Character_Max(); cm++){

		// カラー準備
		for (int n = 0; n < COLOR_MAX; n++)
		{
			////
			std::string fn3;
			char ch[2];
			sprintf(ch, "%d", n + 1);	// 文字に変換

			fn1 = "char/";
			fn2 = "/pal/c";
			fn3 = ".bmp";

			fname2 = fn1 + move[cm].name.nameC + fn2 + ch + fn3;	// 名前決定

			int tr, tg, tb, palName;
			int r, g, b;
			// RGBにパレットの色を記憶
			{
				palName = LoadSoftImage(fname2.c_str());
				// パレットの色を取得する 1〜16, 17〜
				GetPaletteSoftImage(palName, 0, &tr, &tg, &tb, 0);

				// 色をパレットに保存
				for (int p = 0; p < 256; p++){
					GetPaletteSoftImage(palName, p, &r, &g, &b, 0);
					cpal[cm][n].r[p] = r;
					cpal[cm][n].g[p] = g;
					cpal[cm][n].b[p] = b;
				}
			}

			// 透過色を変更
			SetTransColor(tr, tg, tb);

			int gr = LoadGraph(fname2.c_str());
			color[cm].push_back(gr);	// 静止画像セット

			////
		}

		// キャラサイズ決定
		move[cm].sizeG = 1.0;


		// シュウイチのサイズ
		if(cm == 5)
		move[cm].sizeG = 0.5;

	}	// cm

	SetTransColor(0, 0, 0);	// 透過色を戻す
	///////////////////////////////////
	// キャラのステータスと適応させる
	if (charPos.size() > 0)charPos[0] = HYDE - 1;
	if (charPos.size() > 1)charPos[1] = BOUNCER - 1;
	if (charPos.size() > 2)charPos[2] = CORNEL - 1;
	if (charPos.size() > 3)charPos[3] = HELIOS - 1;
	if (charPos.size() > 4)charPos[4] = EIN - 1;
	if (charPos.size() > 5)charPos[5] = SYUICHI - 1;
}

void Select::Release(void)
{
}

void Select::Draw()
{
	// 背景
	DrawGraph(0, 0, back, true);

	int pos = (SCREEN_W / 2) - (3 * 70);

	// キャラ大ポトレ
	for (int i = 0; i < Character_Max(); i++){
		if (((P[0].charEnter) && (P[0].curX == i)) ||
			((P[1].charEnter) && (P[1].curX == i)))
			SetDrawBright(128, 128, 128);	// 決定したら暗く
		
											// 表示
		DrawBox(pos + (i * 70), 310, pos + (i * 70) + 60, 310 + 60, GetColor(80, 80, 200), true);
		DrawGraph(pos + (i * 70), 310, port[charPos[i]], true);

		if (((P[0].charEnter) && (P[0].curX == i)) ||
			((P[1].charEnter) && (P[1].curX == i)))
			SetDrawBright(255, 255, 255);	// 色を戻す
	}

	//============
	// キャラ描画
	//============
	CharAnimetion();

	//==============
	// カーソル描画
	for (int i = 0; i < 2; i++){

		// カーソル色設定
		switch (i){
		case 0:
			SetDrawBright(255, 0, 0);
			break;
		case 1:
			SetDrawBright(0, 0, 255);
			break;
		}
		if ((P[0].curX == P[1].curX) && ((BMode == 0) || ((BMode == 1 || BMode == 2) && (P[0].charEnter))))
			SetDrawBright(255, 0, 255);

		// カーソル 
		if ((i == 0) || ((BMode == 0) || ((BMode == 1 || BMode == 2) && (P[0].charEnter)))){
			//DrawGraph((P[i].posX) + SCREEN_W / 2 - 3 - 216, 297,
			DrawGraph(pos + (P[i].posX) - 3, 307,
				cursor_char, true);
		}
		// 描画輝度を戻す
		SetDrawBright(255, 255, 255);

		// プレイヤーサイド
		if ((i == 0) || ((BMode == 0) || ((BMode == 1 || BMode == 2) && (P[0].charEnter)))){
			if (P[i].charEnter)SetDrawBright(128, 128, 128);
			DrawGraph(pos + (P[i].posX) - 3 + (i * 34), 307 - 22,
				side[i], false);
			if (P[i].charEnter)SetDrawBright(255, 255, 255);	// 明るさ戻す
		}
	}

	// カラー番号
	int cr;
	if (P[0].charEnter)cr = Oran;
	else{ cr = Cr; }

	int cy;

	// 1P
	cy = 0;
	if (oCurY[0] == -1)cy -= 2;
	DrawGraph(45, 0 + cy, gK[7], true);
	cy = 0;
	if (oCurY[0] == 1)cy += 2;
	DrawGraph(45, 40 + cy, gK[1], true);
	DrawFormatString(20, 20, cr, "color %d", P[0].color + 1);

	// 2P
	if ((BMode == 0) || ((BMode == 1 || BMode == 2) && (P[0].charEnter))){
		if (P[1].charEnter)cr = Oran;
		else{ cr = Cr; }

		cy = 0;
		if (oCurY[1] == -1)cy -= 2;
		DrawGraph(SCREEN_W - 65, 0 + cy, gK[7], true);
		cy = 0;
		if (oCurY[1] == 1)cy += 2;
		DrawGraph(SCREEN_W - 65, 40 + cy, gK[1], true);
		DrawFormatString(SCREEN_W - 90, 20, cr, "color %d", P[1].color + 1);
	}

	//==================
	// ステージセレクト
	//==================
	if (ketteiNum == 1){

		//DrawFormatString(220, 70, Cr, "ステージ %d", StageNum + 1);
		//DrawString(220, 70, "ステージ選択", Cr);

		/*
		switch(StageNum){
		case 0:
			DrawString(220, 70, "グリッド", Cr);
			break;
		case 1:
			DrawString(220, 70, "荒野", Cr);
			break;
		case 2:
			DrawString(220, 70, "城壁", Cr);
			break;
		case 3:
			DrawString(220, 70, "闘技場", Cr);
			break;
		}
		*/

		// 描画ブレンドモードをアルファブレンド（透明）にする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);

		// 左右のステージ
		//if (StageNum < STAGE_MAX)
		{
			//DrawRotaGraph(420, 180, 0.1, 0, mStage[StageNum + 1], false, 0);
			// 背景
			if (StageNum != getMaxStage()) {
				/*
				DrawRotaGraph(420 + _drawStagePosX, 180, 0.27 * 0.6, 0, mStageB[StageNum + 1], false, 0);
				DrawRotaGraph(420 + _drawStagePosX, 200, 0.169 * 0.6, 0, mStage[StageNum + 1], true, 0);
				*/
				drawStageGraphAll(StageNum + 1, 420 + _drawStagePosX, 190, 0.162);
			}
			else {
				drawStageGraphAll(0, 420 + _drawStagePosX, 190, 0.162);
			}
		}
		
		{
			//DwrawRotaGraph(220, 180, 0.1, 0, mStage[StageNum - 1], false, 0);
			// 背景
			if (StageNum != 0) {
				/*
				DrawRotaGraph(220 + _drawStagePosX, 180, 0.27 * 0.6, 0, mStageB[StageNum - 1], false, 0);
				DrawRotaGraph(220 + _drawStagePosX, 200, 0.169 * 0.6, 0, mStage[StageNum - 1], true, 0);
				*/
				drawStageGraphAll(StageNum - 1, 220 + _drawStagePosX, 190, 0.162);
			}
			else {
				drawStageGraphAll(getMaxStage(), 220 + _drawStagePosX, 190, 0.162);
			}
		}

		// 描画ブレンドモードをアルファブレンド（透明）にする
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		/*
		// 背景
		if (mStage[StageNum] != 0)
			DrawRotaGraph(320 + _drawStagePosX, 180, 0.27 * _drawStageSize, 0, drawStageB[2], false, 0);
		// ステージ
		if (mStage[StageNum] != 0)
			//DrawRotaGraph(320, 180, 0.18, 0, mStage[StageNum], true, 0);
			DrawRotaGraph(320 + _drawStagePosX, 200, 0.169 * _drawStageSize, 0, drawStage[2], true, 0);
			*/
		drawStageGraphAll(StageNum, 320 + _drawStagePosX, 200, 0.27 * _drawStageSize);

	}

	// 暗転
	if (AntenCheck()){
		Anten(0);
	}
	// 終了の暗転
	if (_timeStop > 0) {
		int num = _timeStop * 25;
		if (num > 255) num = 255;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, num);
		DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(0, 0, 0), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	

}


void Load_Select()
{
	Load_1 = 0;
	Anten(255);
}

// P1、P2の名前を返す
int Player_Name_1()
{
	return P[0].name;
}

int Player_Name_2()
{
	return P[1].name;
}

void Select_SetName(int p1, int p2)
{
	P[0].name = p1;
	P[1].name = p2;
}
                                                                                                                                                                                                                                                                                                                                                                                               
int BattleMode(int f)
{
	if (f != -1)BMode = f;
	return BMode;
}

void InputSelect()
{

	// P0,1を決定
	for (int i = 0; i < 2; i++){
		// 前のカーソルの位置
		oCurY[i] = 0;
		if (!AntenCheck()){
			// キャラが決まってない
			if (!P[i].charEnter){
				// カラーを上下に
				if (KInput(InputSide(i, 108))){P[i].curY -= 1;
					SEStart(36);
					oCurY[i] = -1;
				}
				else if (KInput(InputSide(i, 102))){
					P[i].curY += 1;
					SEStart(36);
					oCurY[i] = 1;
				}
			}
		}
		// 上下一周
		if (P[i].curY < 0){
			P[i].curY = (COLOR_MAX - 1);
		}
		else if (P[i].curY >(COLOR_MAX - 1)){
			P[i].curY = 0;
		}

		if (!AntenCheck()){
			// 未決定なら、キャラカーソル左右に
			if ((ketteiNum == 0) && (!P[i].charEnter)){
				if (KInput(InputSide(i, 104))){
					P[i].curX -= 1;
					SEStart(36);	// 移動の音
					P[i].time = 0;
					P[i].drawPosX = 0;	// 表示位置
				}

				else if (KInput(InputSide(i, 106))){
					P[i].curX += 1;
					SEStart(36);
					P[i].time = 0;
					P[i].drawPosX = 0;	// 表示位置
				}
			}
		}


		// 左右一周
		if (P[i].curX < 0){
			P[i].curX = Character_Max() - 1;
			P[i].posX = (Character_Max() - 1) * 70;
		}
		else if (P[i].curX > Character_Max() - 1){
			P[i].curX = 0;
			P[i].posX = 0;
		}

		// カーソル表示移動
		if (P[i].posX != P[i].curX * 70){
			if (P[i].posX < P[i].curX * 70)
			P[i].posX += 10;
			if (P[i].posX > P[i].curX * 70)
				P[i].posX -= 10;
		}
		// 決定
		if (InputSide(i, 1) == 1 || InputSide(i, 3) == 1){
			kettei = 1;
			P[i].enter = true;
		}
		else{
			P[i].enter = false;
		}
		// ランダム
		// !net!
		/*
		if ((ketteiNum == 0) && (InputSide(i, 4) == 1) && (!P[i].charEnter)){
			kettei = 1;
			P[i].curX = GetRand(Character_Max() - 1);
			P[i].curY = GetRand(COLOR_MAX - 1);
			P[i].enter = true;
			SEStart(36);
		}
		*/

		// キャンセル
		if (InputSide(i, 2) == 1)
		{
			if (ketteiNum > 0){
				ketteiNum -= 1;
			}
			// メニューに戻る or キャラ決定解除
			else if (ketteiNum == 0 && !P[i].charEnter){
				MainSystem::Instance().SetNextMode("Menu");
			}
			SEStart(37);
			P[0].charEnter = false;	// キャラも未決定に
			P[1].charEnter = false;
		}

		// 未決定なら、カラー準備
		if (!P[i].charEnter)
		{
			P[i].color = P[i].curY;
		}
	}
	// 終了
}


void EnterSelect()
{
	// 対戦・トレモ共有
	{

		for (int i = 0; i < 2; i++){
			// キャラ決定
			if ((ketteiNum == 0) && (kettei) && (P[i].enter) && (!P[i].charEnter))
			{
				// キャラが選べる
				if (P[i].curX < Character_Max()){
					
					//================
					//P[i].name = P[i].curX + 1;
					
					switch (P[i].curX){
					case 0:
						P[i].name = HYDE;
						break;
					case 1:
						P[i].name = BOUNCER;
						break;
					case 2:
						P[i].name = CORNEL;
						break;
					case 3:
						P[i].name = HELIOS;
						break;
					case 4:
						P[i].name = EIN;
						break;
					default:
						P[i].name = P[i].curX + 1;
						break;
					}
					//=================

					// カラー決定
					{
						// まだ向こうが決まってない
						if ((i == 0 && (!P[1].charEnter)) ||
							(i == 1 && (!P[0].charEnter))){
							//printfDx("Ok1\n");
							Get_Color(P[i].color, i + 1);		// カラー決定
						}
						// 向こうが選択済み
						else{
							// キャラが同じ
							if (P[0].name == P[1].name){
								if (P[0].color == P[1].color){
									if (P[i].color == 0){
										P[i].color = 1;
										Get_Color(P[i].color, i + 1);
									}
									else{
										P[i].color = 0;
										Get_Color(P[i].color, i + 1);
									}
								}
								else{
									Get_Color(P[i].color, i + 1);		// カラー決定
								}
							}
							// 違う
							else{
								Get_Color(P[i].color, i + 1);		// カラー決定
							}
						}
					}
					// カラー終了
					SEStart(35);	// 決定
					P[i].charEnter = true;
				}

				if (P[0].charEnter && P[1].charEnter)kettei = 0;
			}
			// if終了
		}
	}
	// トレモ版終了

	// 準備ができたら、ステージセレクトへ移行
	if ((P[0].charEnter && P[1].charEnter) && (ketteiNum == 0)){
		ketteiNum = 1;
	}
	
	//===================
	// ここから共通操作
	//===================
	
	// ずらしを戻す
	if (_drawStagePosX != 0) {
		if (_drawStagePosX > 0)
			_drawStagePosX -= 2;
		else {
			_drawStagePosX += 2;
		}
	}
	if (_drawStageSize != 1.0) {
		if (_drawStageSize < 1.0)
			_drawStageSize += 0.1;
		// 1.0より小さい
		else {
			_drawStageSize = 1.0;
		}
	}

	for (int i = 0; i < 2; i++){
		// ステージ操作
		if ((ketteiNum == 1) && (InputSide(i, 106) == 1)){
			StageNum += 1;
			SEStart(36);
			_drawStagePosX = 2; // 表示ずらす
			_drawStageSize = 0.5;
		}
		if ((ketteiNum == 1) && (InputSide(i, 104) == 1)){
			StageNum -= 1;
			SEStart(36);
			_drawStagePosX = -2; // ずらす
			_drawStageSize = 0.5;
		}
	}
		// 一周したら戻す
		if (StageNum < 0){
			StageNum = getMaxStage();
		}
		else if (StageNum > getMaxStage()){
			StageNum = 0;
		}

		// 決定したら時間経過
		if ((ketteiNum == 1) && (kettei)) {
			// 一度だけ決定音
			if (_timeStop == 0)SEStart(35);
			_timeStop++;
		}

		// 決定したら
		if ((ketteiNum == 1) && (kettei) && (_timeStop > 10))
		{
			//SEStart(35);

			int stage = StageNum;
			if (StageNum == 0)stage = GetRand(getMaxStage() - 1) + 1;
			
			GetStageNum(stage - 1, 0);	// ステージ受け取り
			Versus_bgmNum(stage);
			
			// 対戦時のみリプレイ設定
			if ((BMode == 0) && (GetPrivateProfileInt("Config", "replay", 0, "./config.ini") == 1)){
				Replay_Mode(2);
				// 再戦で設定は変わらないので、ここだけでいい
				Replay_Setting(P[0].name, P[1].name, P[0].color, P[1].color, StageNum);
			}
			else{ Replay_Mode(0); }

			ketteiNum += 1;
			kettei = 0;
		}

}

// アニメーションセット＆描画
void CharAnimetion()
{
	int bTime, num;
	int homo[2];
	homo[0] = 0, homo[1] = 0;

	// アニメーションver3
	for (int i = 0; i < 2; i++){
		P[i].time++;	// 入る度にアニメ時間加算
		bTime = 0;
		P[i].iNum = 0;

		// 表示位置をスライド
		if (P[i].drawPosX < 50) {
			// 最初はもっと増やす
			if (P[i].drawPosX < 20) {
				P[i].drawPosX += 4;
			}
			else if (P[i].drawPosX < 40) {
				P[i].drawPosX += 2;
			}
			else { P[i].drawPosX++; }
			
		}

		for (num = 0; num < STATE2_MAX - 1; num++){
			bTime += move[charPos[P[i].curX]].air.B[num].time;	// アニメ時間を加算
			// 時間が上回っている＆次がある
			if ((bTime <= P[i].time)
				&& (move[charPos[P[i].curX]].air.B[num].time > -2))
			{
			}
			else{ 
				break; 
			}
		}
		// num2ぶちこむ
		homo[i] = move[charPos[P[i].curX]].air.B[num].num2;
		// でかすぎたら
		if (move[charPos[P[i].curX]].air.B[num].time == -2)P[i].time = -1;

		// パレット
		for (int n = 0; n < 256; n++){
			SetPaletteSoftImage(pic[charPos[P[i].curX]][homo[i]].image, n,
				cpal[charPos[P[i].curX]][P[i].color].r[n],
				cpal[charPos[P[i].curX]][P[i].color].g[n],
				cpal[charPos[P[i].curX]][P[i].color].b[n], 0);
		}
		
		// 透過色決定
		SetTransColor(
			cpal[charPos[P[i].curX]][P[i].color].r[0],
			cpal[charPos[P[i].curX]][P[i].color].g[0], 
			cpal[charPos[P[i].curX]][P[i].color].b[0]);
		// 画像リセット
		if(P[i].image != 0)DeleteGraph(P[i].image);
		// 画像決定
		P[i].image = CreateGraphFromSoftImage(pic[charPos[P[i].curX]][homo[i]].image);
		SetTransColor(0,0,0);
	}

	// アニメーション設定
	/*
	for (int i = 0; i < 2; i++){
		move[i].time++;
		bTime = 0;
		for (num = 0; num < STATE2_MAX - 1; num++){
			bTime += move[charPos[P[i].curX]].air.B[num].time;
			// 時間が上回っている＆次がある
			if ((bTime <= move[i].time++) &&
				(move[charPos[P[i].curX]].air.B[num + 1].time > -2)){
			}
			else{ break; }
		}
		if (move[charPos[P[i].curX]].air.B[num + 1].time == -2)move[i].time++, num = 0;
	}
	*/

	// アニメ内容
	//int num2[2];

	/// キャラクター描画 ///
	// これ
	int xs = 0, ys = 0;	// x,yのサイズ取得
	GetGraphSize(color[charPos[P[0].curX]][P[0].color], &xs, &ys);	// サイズ
	
	DrawRotaGraphF(
		//52.0 + ((double)xs / 2.0) - (double)pic[charPos[P[0].curX]][homo[0]].posX,
		2.0 + ((double)xs / 2.0) + P[0].drawPosX
			- (double)pic[charPos[P[0].curX]][homo[0]].posX + (double)move[charPos[P[0].curX]].air.B[homo[0]].posX,
		200.0 + ((double)ys)
			- (double)pic[charPos[P[0].curX]][homo[0]].posY + (double)move[charPos[P[0].curX]].air.B[homo[0]].posY,
		move[P[0].curX].sizeG, 0,
		P[0].image,
		true, 0);

	// 2P側
	if ((BMode == 0) || ((BMode == 1 || BMode == 2) && (P[0].charEnter))){
		GetGraphSize(color[charPos[P[1].curX]][P[1].color], &xs, &ys);	// サイズ

		DrawRotaGraphF(
			//52.0 + ((double)xs / 2.0) - (double)pic[charPos[P[0].curX]][homo[0]].posX,
			((double)SCREEN_W - 2.0) - ((double)xs / 2.0) - P[1].drawPosX
			+ (double)pic[charPos[P[1].curX]][homo[1]].posX - (double)move[charPos[P[1].curX]].air.B[homo[1]].posX,
			200.0 + ((double)ys)
			- (double)pic[charPos[P[1].curX]][homo[1]].posY + (double)move[charPos[P[1].curX]].air.B[homo[1]].posY,
			move[P[1].curX].sizeG, 0,
			P[1].image,
			true, 1);
	}

}

int InputSide(int PSide, int key)
{
	int n;

	// 対戦
	if (BMode == 0){
		if (PSide == 0)
			n = P1_BInput(key);
		else{
			n = P2_BInput(key);
		}
	}
	// それ以外
	else if (BMode >= 1){
		if (PSide == 0)
			n = P1_BInput(key);
		// 2P側
		else{
			if ((ketteiNum == 0) && (P[0].charEnter)){
				n = P1_BInput(key);
			}
			else{ n = 0; }
		}
	}

	return n;
}

void Select_GetName(std::vector<Name_t>GN){
	for (int i = 0; i < Character_Max(); i++){
		move.push_back(Move_t());
		move[i].name = GN[i];
	}
}

// airを取得
void GetA_Select(Air GA, int name)
{
	// AIR変数準備 //
	// 初期化＆代入
	{
		move[name].air.setup();
		move[name].air = GA;
		
		// アニメ時間
		int n = 0;
		move[name].air.aTime = 0;
		while (move[name].air.B[n].time > 0){
			move[name].air.aTime += move[name].air.B[n].time;
			n++;
		}
	}
}

// pict取得
void GetPI_Select(Pict GPI[IMAGE2_MAX], int name)
{
	for (int i = 0; i < IMAGE2_MAX; i++){
		pic[name][i] = GPI[i];
	}
}

// カーソル位置を戻す
void Select_ReturnCursor()
{
	if (Character_Max() > 3){
		P[0].curX = 2, P[1].curX = 3;
	}
	else{
		P[0].curX = 0, P[1].curX = 0;
	}
}