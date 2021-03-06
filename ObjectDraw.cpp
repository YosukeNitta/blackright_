//インクルード部--------------------------------------------
#include "pch.h"

#define LOG_MAX		(10)

using namespace std;

#pragma region 変数
// 定数
static const int powerPos = 398;
// アナウンス
static double m_size;	// 画像の拡大

static int PowerBar[2];		// ライフバー
static int PowerPoint;	// 画面に表示する体力
static int PowerNum[10];// 画像 [パワーゲージ 0〜9]
static int _drawPowerLamp;	// 点滅
static boolean _drawPowerLampIsPlus = true;	// 点滅

static int Hit;			// 画像「HIT」
static int HitNum[10];	// 画像「ヒット数 0〜9」
static int HitGraph, HitGraph2;	// ヒット数の実際の画像(2は 2ケタ目)
static boolean beat[2];	// 連続ヒットか(falseで青ビ)

static int hitCount[2];		// ヒットorガード数
static int hDrawTime[2];	// 表示する時間
static int hHitCount[2];	// 直前のヒット数

static int Win1, Win2;	// プレイヤーWIN の表示
static int battle[5];	// 画像　FIGHT
static int EX;
static int COUNTER;	// counter

static int Load1;
static int TText[5];
static char TChar[TX_MAX];

//ダメージ計算
static int m_AllDamage;
static int m_DisplayDamage;
//喰らう前の体力
static int m_P1_SLife, m_P2_SLife;
//喰らった後の体力
static int m_P1_ALife, m_P2_ALife;

// ゲージ回収
static int mGauge = 0;
static int mG_startPower = 0;
static int mG_Start = 0;
//発生フレーム
static int mHassei = 0;
static int aSpeed;	// 攻撃フレーム
// フレーム、開始フレーム？
static int mFrame = 0;
static int mF_Start = 0;
static int _drawHassei = 0;
static int _oldState = 0;
//
static float m_cPos;

//
static int m_ReplayTime;	// リプレイ経過時間

// キーログ表示
static int mKey[9];
static int mButton[4];
static int gKey[10];
static int gButton[4];

//static int oKey[LOG_MAX];
static vector<int> oKey;
//static int oButton[LOG_MAX][4];
static vector<int> oButton[LOG_MAX];
static vector<int> oTime;
static int o_nTime = 0;

static boolean sButton, sKey;
static int saveKey = -1;
static int test, test2[2];	// 特殊ゲージ
static int test3[2];
// Add
static int addGaugeTime[2];		// 表示時間
static int addGauge[10];		// 画像
static int addGaugePower[2];	// ゲージ量
/*
* これを使って数値を全て渡す
*/
static int mXA[2][HIT_MAX], mYA[2][HIT_MAX], mWA[2][HIT_MAX], mHA[2][HIT_MAX];
static int mHXA[2][HM], mHYA[2][HM], mHWA[2][HM], mHHA[2][HM];

// 喰らい判定
static int m_XD[HIT_MAX], m_YD[HIT_MAX], m_WD[HIT_MAX], m_HD[HIT_MAX];
static int mHXH[2][HM], mHYH[2][HM], mHWH[2][HM], mHHH[2][HM];

// キーログ表示
static int m_KeyPos, m_Key;
static int m_Button[4];

// デバッグ用
static bool debugDraw;
static int debug_sw;

// 表示時間
static int display_time[2];

// 画面の明るさ
static int blackOut;
#pragma endregion

void BoxCheck();	// 判定チェック
void BoxKurai();	// 喰らい判定表示
void BoXAtt();		// 攻撃判定表示


class Test {

public:
	int x, y;	// 画像サイズ

	Test::Test()
	{
		x = 11;
		y = 22;
	}
};

static Test tes;

void ObjectDraw_KeyDisplay();

/// <summary>
/// キャラ・ステージ以外の描画
/// </summary>
void ObjectDraw()
{

	// 画像
	if (Load1 == 0){
		//パワーバーの画像
		LoadDivGraph("ob/PowerBar.png", 2, 1, 2, 241, 36, PowerBar);
		PowerPoint = LoadGraph("ob/PowerPoint.bmp");	// ゲージのドット
		LoadDivGraph("ob/number.png", 10, 5, 2, 25, 30, PowerNum);	// ゲージ本数の数字

		Hit = LoadGraph("ob/hit.png");	// Hit の文字
		LoadDivGraph("ob/hitnum.png", 10, 5, 2, 36, 56, HitNum);	// ヒット数

		LoadDivGraph("ob/key.png", 10, 5, 2, 20, 20, gKey);
		LoadDivGraph("ob/button.png", 4, 4, 1, 20, 20, gButton);

		Win1 = LoadGraph("ob/1P.png");	// 1P
		Win2 = LoadGraph("ob/2P.png");	// 2P

		test = LoadGraph("char/bouncer/o1.bmp");
		test2[0] = LoadGraph("char/helios/o1.bmp");
		test2[1] = LoadGraph("char/helios/o2.bmp");
		test3[0] = LoadGraph("char/dan/gauge.bmp");
		test3[1] = LoadGraph("char/dan//gauge2.bmp");
		EX = LoadGraph("ob/EX.png");
		COUNTER = LoadGraph("ob/counter.png");

		LoadDivGraph("ob/battle.png", 5, 1, 5, 280, 70, battle);

		// キー画面
		m_Key = LoadGraph("ob/r_Key.png");
		m_KeyPos = LoadGraph("ob/r_KeyPos.png");
		LoadDivGraph("ob/r_Button.png", 4, 4, 1, 20, 20, m_Button);

		LoadDivGraph("ob/num2.png", 10, 5, 2, 12, 20, addGauge);

		// 生成
		//
		for (int i = 0; i < LOG_MAX; i++){
			oKey.push_back(-1);
		}

		//oButton->resize(LOG_MAX);
		//oTime.resize(LOG_MAX);
		for (int a = 0; a < LOG_MAX; a++){
			for (int i = 0; i < 4; i++){
				oButton[a].push_back(0);
			}
			oTime.push_back(-1);
		}
		blackOut = 0;

		Load1 = 1;
	}
		
		/**************
		* ヒット数
		**************/
		Player P[2];
		int hDrawPos[2];
		hDrawPos[0] = 20;
		hDrawPos[1] = (SCREEN_W - 640) + 320 + 122;
		hitCount[0] = 0, hitCount[1] = 0;
		P[0] = P1, P[1] = P2;


		for (int i = 0; i < 2; i++) {
			// 数字入れ替え
			if (i == 1) {
				P[0] = P2, P[1] = P1;
			}

			// ヒット数
			if (P[0].HitCount > 0) {
				HitGraph = HitNum[P[0].HitCount % 10];
				HitGraph2 = HitNum[P[0].HitCount / 10];
				hitCount[0] = P[0].HitCount;
			}
			// ガード数
			else if (P[0].A.guardCount > 1) {
				HitGraph = HitNum[P[0].A.guardCount % 10];
				HitGraph2 = HitNum[P[0].A.guardCount / 10];
				hitCount[0] = P[0].A.guardCount;
			}

			// ガード数

			// 1P側 ヒット数表示
			// 喰らいor連続ガード
			if ((P[1].HFlg) || (P[0].A.guardCount > 1)) {
				// 2ヒット以上で、 1ケタ目表示
				if (hitCount[0] >= 2) {
					// 99ヒット以内
					if (hitCount[0] <= 99) {
						// ヒット数がないなら
						if (P[0].HitCount == 0) {
							// ガード数表示へ
							SetDrawBright(128, 128, 128);
						}
						DrawGraph(hDrawPos[i] + 80, 90 + 5, Hit, true);

						//青ビの場合
						if (beat[i]) {
							// 青色
							SetDrawBright(30, 30, 255);
						}
						// 赤色
						else { SetDrawBright(255, 30, 30); }
						
						DrawGraph(hDrawPos[i] + 39, 90, HitGraph, true);
						
						// ヒット数が 2ケタ以上の時は、2ケタ目表示
						if (hitCount[0] >= 10) {
							DrawGraph(hDrawPos[i] + 8, 90, HitGraph2, true);
						}

						SetDrawBright(255, 255, 255);	// 色リセット
					}
					// 99ヒットでカンスト
					else if (hitCount[0] >= 100) {
						DrawGraph(hDrawPos[i] + 80, 90 + 5, Hit, true);
						DrawGraph(hDrawPos[i] + 39, 90, HitNum[9], true);
						DrawGraph(hDrawPos[i] + 8, 90, HitNum[9], true);
					}
				}
			}
		}

		/**************
		* システム表示
		**************/
		{

			// 設定
			for (int i = 0; i < 2; i++) {
				// 表示位置
				int drawXpos, drawYpos;
				drawYpos = 150;
				if (i == 0) {
					drawXpos = 60;
				}
				else { drawXpos = SCREEN_W - 75 - 60; }
				
				// カウンター
				if (display_time[i] > 0) {

					if (display_time[i] < 10) {
						// 位置を動かす
						drawYpos += -10 + (display_time[i] + 1);
						SetAlpha(0 + ((display_time[i] + 1) * 20));
					}
					DrawGraph(drawXpos, drawYpos, COUNTER, true);

					if (display_time[i] < 10) {
						NoBlend();
					}
				}

				display_time[i]--;	// 時間減少
			}
		}

		/**************
		* 特殊ゲージ
		**************/
		if (!(S.roundState == 0 && S.roundTime == 0) && (S.noGauge == 0)){
			/*
			// EX
			if (P1.Var[30] != 0)SetAlpha(68);
			DrawGraph(0 + 2 + 30, 62 + 2, EX, true);
			NoBlend();
			SetDrawBright(255, 255, 255);

			if (P2.Var[30] != 0)SetAlpha(68);
			DrawGraph(SCREEN_W - 2 - 24 - 30, 62 + 2, EX, true);
			NoBlend();
			SetDrawBright(255, 255, 255);
			*/

			Player P[2];
			int xp, yp, xp2, yp2;
			P[0] = P1, P[1] = P2;

			for (int i = 0; i < 2; i++) {
				// バウンサー
				if (P[i].Name == BOUNCER) {
					
					yp = 62 + 7;
					if (i == 0) {
						xp = 2 + 27;
						xp2 = xp + 1 + (P[i].Var[17] * 0.08);
					}
					else {
						xp = SCREEN_W - 2 - 27;
						xp2 = xp - 1 - (P[i].Var[17] * 0.08);
					}

					// アーマー時間
					if ((P[i].Var[17] != -1) && (P[i].D.armor > 0))DrawBox(xp, yp,
						xp2, yp + 12, Oran, true);


					if (P[i].D.armor <= 0)SetAlpha(68);
					if (P[i].D.armor > 2)SetDrawBright(255, 10, 10);
					else if (P[i].D.armor > 1)SetDrawBright(255, 128, 128);

					yp = 62 + 2;
					if (i == 0) xp = 0 + 2;
					else { xp = SCREEN_W - 2 - 26; }

					// アーマーの輝き
					DrawGraph(xp, yp, test, true);
					NoBlend();
					SetDrawBright(255, 255, 255);


					yp = (SCREEN_H - 480) + 430;
					if (i == 0) xp = 80;
					else { xp = SCREEN_W - 80 - 10; }

					// 3ゲージ時間
					if (P[i].Var[13] > 0) {
						DrawFormatString(xp, yp, Oran, "%d", P[i].Var[13] / 60);
					}
				}

				// ヘリオス
				if (P[i].Name == HELIOS) {
					
					yp = powerPos + 30;
					int color;
					if (i == 0)xp = 80;
					else { xp = SCREEN_W - 80 - 10; }	// 10はテキスト分

					if (P[i].Var[11] >= 3000) {
						color = Oran;
					}
					else if (P[i].Var[11] >= 2000) {
						color = GetColor(255, 255, 0);
					}
					else if (P[i].Var[11] >= 1000) {
						color = Cr;
					}
					else { color = GetColor(128, 128, 128); }
					// 本数
					DrawFormatString(xp, yp, color, "%d", P[i].Var[11] / 1000);
					

					yp = powerPos + 35;
					if (i == 0) {
						xp = 100;
						xp2 = xp + (P[i].Var[11] * 0.05);
					}
					else {
						xp = SCREEN_W - 80 - 20 - (P[i].Var[11] * 0.05);
						xp2 = SCREEN_W - 80 - 20;
					}

					// ゲージ
					DrawBox(xp, yp, xp2, yp + 10, GetColor(255, 215, 0), true);

					if (i == 0) {
						xp = 100;
					}
					else {
						xp = SCREEN_W - 80 - 20 - ((3 * 1000) * 0.05);
					}

					// ゲージ枠
					for (int n = 1; n < 4; n++) {
						DrawBox(xp + (((n - 1) * 1000) * 0.05), yp,
							xp + ((n * 1000) * 0.05), yp + 10, GetColor(20, 20, 20), false);
					}
				}

				// ダン
				if (P[i].Name == HYDE) {

					yp = powerPos + 32;
					int color;
					/*
					if (P[i].Var[11] > 0)color = GetColor(255, 215, 0);
					else if (P[i].Var[10] > 0)color = GetColor(105, 105, 105);
					else { color = GetColor(205, 205, 205); }
					*/
					//if (P[i].Var[10] > 0)color = GetColor(105, 215, 85);
					color = GetColor(105, 215, 85);
					//else { color = GetColor(105, 105, 105); }

					
					int var;
					/*
					if (P[i].Var[11] > 0)
						var = (P[i].Var[11] * 0.15);
					else { var = ((1800 - P[i].Var[10]) * 0.05); }
					*/
					{ var = (P[i].Var[10] * 0.084); }

					if (i == 0) {
						xp = 100;
						xp2 = xp + 91;
					}
					else {
						xp = SCREEN_W - 100;
						xp2 = xp - 91;
					}

					SetAlpha(158);
					// ゲージ黒部分
					DrawBox(xp, yp,
						xp2, yp + 15, GetColor(0, 0, 0), true);
					NoBlend();
					

					if (i == 0) {
						xp = 100;
						xp2 = xp + var;
					}
					else { 
						xp = SCREEN_W - 100; 
						xp2 = xp - var;
					}	// 10はテキスト分

					// ゲージ
					DrawBox(xp, yp,
						xp2, yp + 15, color, true);

					if (i == 0) {
						xp = 100;
						xp2 = xp + 91;
					}
					else {
						xp = SCREEN_W - 100;
						xp2 = xp - 91;
					}

					// ゲージ枠
					DrawBox(xp, yp,
						xp2, yp + 16, GetColor(20, 20, 20), false);

					if (i == 0) {
						xp = 100 - 10;
					}
					else {
						xp = SCREEN_W - 100 + 1;
					}

					// 本数表示
					DrawFormatString(xp, yp, Cr, "%d", P[i].Var[10] / 180);
				}
			}

		}
		/**************
		* 判定確認 （トレモ限定）
		**************/
		if (S.TraningFlg){
			BoxCheck();
		}

		//--------------
		// 合計ダメージ
		//--------------
		if (P2.HFlg == 0){
			m_P2_SLife = P2.Life;
			m_AllDamage = 0;
		}
		if (P2.HFlg){
			//m_AllDamage = m_P2_SLife - P2.Life;
			m_DisplayDamage = m_AllDamage;	// ダメージ表示
		}

		/**************
		* パワーゲージ
		**************/
		if (!(S.roundState == 0 && S.roundTime == 0) && (S.noGauge == 0)){
			//--------------
			// 半透明ゲージ
			//--------------
			// 画像サイズ取得 P1,2
			int bar_x, bar_y;
			GetGraphSize(PowerBar[0], &bar_x, &bar_y);
			
			// 点滅時間の変更
			if(_drawPowerLampIsPlus)
				_drawPowerLamp++;
			else {
				_drawPowerLamp--;
			}
			if (_drawPowerLamp >= 60)_drawPowerLampIsPlus = false;
			if (_drawPowerLamp <= 0)_drawPowerLampIsPlus = true;

			//--------------
			// 半透明ゲージ
			//--------------

			SetAlpha(158);
			
			// パワーバー下　透明表示
			DrawGraph(20, 35 + powerPos, PowerBar[1], true);
			DrawTurnGraph(SCREEN_W - 20 - bar_x, 35 + powerPos, PowerBar[1], true);

			// 半透明解除
			NoBlend();

			// ゲージ1000以上
			{
				SetDrawBright(128,100,0);
				//DrawGraph(20, 435, PowerBar[1], true);

				if (P1.Power >= 1000 && P1.Power < 3000){
					DrawRotaGraph3F(320 - (1000 / 5) - 62, 54 + powerPos,
						0, 0, (1000 / 5), 1, 0, PowerPoint, true, false);
				}
				if (P2.Power >= 1000 && P2.Power < 3000){
					DrawRotaGraph3F(SCREEN_W - 320 + 62, 54 + powerPos,
						0, 0, (1000 / 5), 1, 0, PowerPoint, true, false);
				}

				SetDrawBright(255, 255, 255);
			}


			//パワー量　1P
			//for (int i = 0; i < P1.Power % 1000; i++){
				//DrawGraph((SCREEN_W / 2) - 62 - (i / 5), 454, PowerPoint, true);
			//}
			
			//==========
			// 色彩設定
			//==========
			float r, g, b;
			r = g = b = 0;
			r = _drawPowerLamp * 1 + 210;
			g = _drawPowerLamp * 0.75 + 142.5;
			if (r > 255.0)r = 255.0;
			if (g > 180.0)g = 180.0;

			// ゲージ所持時
			SetDrawBright(250, 170, 0);
			// ゲージMAX時
			if (P1.Power == POWER_MAX)SetDrawBright((int)r, (int)g, (int)b);

			if (P1.Power == POWER_MAX){
				DrawRotaGraph3F(320 - (1000 / 5) - 62, 54 + powerPos,
					0, 0, (1000 / 5), 1, 0, PowerPoint, true, false);
			}
			else{
				DrawRotaGraph3F((float)(320 - ((P1.Power % 1000) / 5) - 62), 54 + powerPos,
					0, 0, ((P1.Power % 1000) / 5), 1, 0, PowerPoint, true, false);
			}
			SetDrawBright(255, 255, 255);

			// ゲージ本数を数字で表示
			DrawGraph(25 + 1, 40 + powerPos - 1, PowerNum[P1.Power / 1000], true);

			//パワー量　2P
			// ゲージMAX時
			//SetDrawBright(255, 180, 0);
			SetDrawBright(250, 170, 0);
			
			// ゲージMAX時
			if (P2.Power == POWER_MAX)SetDrawBright((int)r, (int)g, (int)b);

			if (P2.Power == POWER_MAX){
				DrawRotaGraph3F((SCREEN_W - 640) + 320 + 62, 54 + powerPos,
					0, 0, (1000 / 5), 1, 0, PowerPoint, true, false);
			}
			else{
				DrawRotaGraph3F((SCREEN_W - 640) + 320 + 62, 54 + powerPos,
					0, 0, ((P2.Power % 1000) / 5), 1, 0, PowerPoint, true, false);
			}
			SetDrawBright(255, 255, 255);

			// ゲージ本数
			DrawGraph(SCREEN_W - 50, 40 + powerPos - 1, PowerNum[P2.Power / 1000], true);

			///////////////////
			// パワーバー表示
			DrawGraph(20, 35 + powerPos, PowerBar[0], true);
			DrawTurnGraph(SCREEN_W - 20 - bar_x, 35 + powerPos, PowerBar[0], true);

		}
		
		// デバッグ
		{
			/*
			// デバッグ用
			// 薄い四角
			SetAlpha(128);
			DrawBox(498, SCREEN_H - 42, SCREEN_W, SCREEN_H, GetColor(0, 0, 0), true);
			NoBlend();

			DrawFormatString(500, SCREEN_H - 40, Cr, "ctrl..%d,%d", P1.ctrl, P2.ctrl);
			DrawFormatString(500, SCREEN_H - 20, Cr, "sflg..%d,%d", P1.SFlg, P2.SFlg);
			*/
		}

		/**************
		* エフェクト
		**************/
		{
			EffDraw();
		}

		// リプレイキー表示
		if (Replay_Mode(-1) == 1) {
			ObjectDraw_KeyDisplay();
		}
		else {
			m_ReplayTime = 0;
		}

		/**************
		* アナウンス
		**************/
		{
			// 開幕
			if (S.roundState == 0){

				// 少し黒くする
				if (S.roundTime > 0){
					SetAlpha(98);
					if (S.roundTime <= 110){
						DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(0, 0, 0), true);
					}
					else if (S.roundTime > 110){
						DrawBox(0, 0, SCREEN_W, ((121 - S.roundTime) * 32), GetColor(0, 0, 0), true);
						DrawBox(0, SCREEN_H - ((121 - S.roundTime) * 32), SCREEN_W, SCREEN_H, GetColor(0, 0, 0), true);
					}
					SetAlpha(255);
					
				}
				// SE
				if(S.roundTime == 1)SEStart(34);
				// SE
				if (S.roundTime == 71)SEStart(34);

				// 立ちになったら
				if (S.roundTime >= 1 && S.roundTime <= 10)m_size = (11 - S.roundTime) / 2 + 0.5;
				else if (S.roundTime >= 70 && S.roundTime <= 80)m_size = (81 - S.roundTime) / 2 + 0.5;
				else{ m_size = 1; }
				if (m_size < 0)m_size = 1;

				if (S.roundTime >= 50 && S.roundTime < 70)SetAlpha(255 - (S.roundTime - 50) * 12.8);
				if (S.roundTime >= 1 && S.roundTime < 70){
					/*
					DrawRotaGraph(260, 240, m_size, 0, battle[1], true);
					DrawRotaGraph(400, 240, m_size, 0, battle[0], true);
					*/
					DrawRotaGraph(340, 240, m_size, 0, battle[0], true);
				}


				if(S.roundTime >= 100)SetAlpha(255 - (S.roundTime - 100) * 12.8);
				if (S.roundTime >= 70)DrawRotaGraph(340, 240, m_size, 0, battle[2], true);


				NoBlend();
			}
			// バトル中
			else if (S.roundState == 1){
				//
				/*
				for (int i = 0; i < 2; i++){
					if (addGaugeTime[i] > 0){
						if (addGaugeTime[i] == 60)SetAlpha(128);
						else if (addGaugeTime[i] == 1)SetAlpha(128);
						DrawRotaGraph(80 + (i * (SCREEN_W - 92 - 80)), 120, 1, 0, addGauge[0], true);
						SetAlpha(255);
						addGaugeTime[i] -= 1;
					}
				}
				*/
			}
			// ラウンド取得
			else if (S.roundState == 2){
				
				// SE
				if (S.roundTime == 2)SEStart(34);

				// 少し黒くする
				if (S.roundTime > 0){
					SetAlpha(98);
					if (S.roundTime <= 40){
						DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(0, 0, 0), true);
					}
					else if (S.roundTime > 40){
						DrawBox(0, 0, SCREEN_W, ((51 - S.roundTime) * 32), GetColor(0, 0, 0), true);
						DrawBox(0, SCREEN_H - ((51 - S.roundTime) * 32), SCREEN_W, SCREEN_H, GetColor(0, 0, 0), true);
					}
					SetAlpha(255);
				}

				// DOWN 描画
				if (S.roundTime <= 1 && S.StopTime > 0){
					//SetAlpha(255 - (S.StopTime * 12.8));
					
					/*
					if (S.StopTime > 1){
						m_size = S.StopTime;
					}
					else{ m_size = 1; }
					*/
					DrawRotaGraph(360, 240, 1, 0, battle[3], true);
				}

				if (S.roundTime >= 1 && S.roundTime <= 10)m_size = (11 - S.roundTime) / 2 + 0.5;
				else{ m_size = 1; }
				if (m_size < 0)m_size = 1;

				if (S.roundTime >= 30)SetAlpha(255 - S.roundTime - 30 * 12.8);
				// 立ちになったら
				if (S.roundTime > 0)DrawRotaGraph(340, 240, m_size,0,battle[2], true);

				NoBlend();
			}
			

		}
		//-----------------
		// フェードアウト
		//-----------------
		{
			// 勝利時のホワイトアウト
			if (S.roundState == 3){
				int add;
				add = S.roundTime * 4;
				SetAlpha(add);
				DrawBox(0, 0, SCREEN_W, SCREEN_H, Cr, true);
				NoBlend();
			}

			// ブラックアウト
			if (blackOut != 0){
				SetAlpha(blackOut);
				DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(0,0,0), true);
				NoBlend();
				blackOut = 0;
			}
		}
}


// ヒットボックス以外も調べる
void BoxCheck()
{
	// レコーディング関連、中央に表示
	// いつでも表示する
	if (S.TSwitch[7] > 0) {
		// 薄い四角
		SetAlpha(128);
		DrawBox(SCREEN_W / 2 - 61, 100 - 1,
			SCREEN_W / 2 + 61, 120, GetColor(0, 0, 0), true);
		NoBlend();
	}
	if (S.TSwitch[7] == 1)DrawFormatString(SCREEN_W / 2 - 30, 100, Cr, "録画準備");
	else if (S.TSwitch[7] == 2) {
		DrawString(SCREEN_W / 2 - 30, 100, "録画中", GetColor(255, 255, 0));
		DrawFormatString(SCREEN_W / 2 + 30, 100, Cr, "%d", TText[0]);
	}
	else if (S.TSwitch[7] == 3)DrawFormatString(SCREEN_W / 2 - 30, 100, Cr, "再生中 %d", TText[0]);

	//=================
	// デバッグ //
	//=================
	if (CheckHitKey(KEY_INPUT_F4) != 0)debug_sw++;
	else { debug_sw = 0; }

	if (debug_sw == 1) {
		if (debugDraw)debugDraw = 0;
		else { debugDraw = 1; }
	}

	//==================
	// デバッグ描画
	//==================
	if (debugDraw) {
		// 薄い四角
		SetAlpha(128);
		DrawBox(498, 160, 620, 240, GetColor(0, 0, 0), true);
		NoBlend();

		DrawFormatString(500, 160, Cr, "SFLG:%d :%d", P1.SFlg, P2.SFlg);
		DrawFormatString(500, 180, Cr, "ctrl:%d :%d", P1.ctrl, P2.ctrl);
		DrawFormatString(500, 200, Cr, "lock:%d :%d", P1.Lock, P2.Lock);
		DrawFormatString(500, 260, Cr, "st:%d :%d", P1.stateno, P2.stateno);
		//=====================
		// コンボ補正値
		//=====================
		float comboHosei = 1.0;
		// 強制補正サイン
		int hoseiColor = GetColor(255,255,255);

		//comboHosei = 1.0 - (P1.HitCount * P1.A.comboLate);
		
		if (P1.HitCount == 2)comboHosei = P1.A.cl_max;
		// 3ヒット以降
		else if (P1.HitCount > 2) {
			comboHosei = P1.A.cl_max - ((P1.HitCount - 2) * P1.A.comboLate);
		}

		// 強制補正
		if (P1.A.hosei_K != 1.0) {
			comboHosei *= P1.A.hosei_K;
			hoseiColor = GetColor(255, 128, 128);
		}

		if (comboHosei <= P1.A.cl_min) {
			comboHosei = P1.A.cl_min;
			hoseiColor = GetColor(128, 128, 255);
		}
		DrawFormatString(500, 220, hoseiColor, "補正:%.2f", comboHosei);
		DrawFormatString(500, 240, Cr, "hit %d:%d", P1.MoveHit, P2.MoveHit);
		//DrawFormatString(500, 240, Cr, "st %d:%d", P1.stateno, P2.stateno);

		//
		//====================
		
		// 距離
		boolean isLeftP1 = true;
		boolean isUpP1 = true;
		float xdist, ydist;
		xdist = P1.XPos - P2.XPos;
		ydist = P1.YPos - P2.YPos;

		// -なら逆にする
		if (xdist < 0.0) {
			xdist = -xdist;
			isLeftP1 = false;
		}
		if (ydist < 0.0) {
			ydist = -ydist;
			isUpP1 = false;
		}
		//DrawFormatString(500, 220, Cr, "xdist:%.1f", xdist);
		//DrawFormatString(500, 240, Cr, "ydist:%.1f", ydist);

		//DrawFormatString(500, 260, Cr, "p1:%s", P1.C.name);
		//DrawFormatString(500, 280, Cr, "p2:%s", P2.C.name);

		DrawLine(P1.XPos - S.ScroolX, P1.YPos - S.ScroolY,
			P2.XPos - S.ScroolX, P2.YPos - S.ScroolY, Cr);

		// 位置表示
		float xp, yp;
		int xhosei_x, xhosei_y;
		if (isLeftP1)xp = P1.XPos - S.ScroolX;
		else { xp = P2.XPos - S.ScroolX; }
		if (isUpP1)yp = P1.YPos - S.ScroolY;
		else { yp = P2.YPos - S.ScroolY; }
		
		// 文字の位置補正
		xhosei_x = xhosei_y = 0;
		if (xdist >= 100.0)
			xhosei_x = -20;
		else if(xdist > 10.0)
			xhosei_x = -10;

		DrawFormatString(xp - xdist / 2.0 - 10 + xhosei_x, yp - ydist / 2.0 - 40, Cr, "%.0f", xdist);
		DrawFormatString(xp - xdist / 2.0 + xhosei_y, yp - ydist / 2.0 - 20, Cr, "%.0f", ydist);
	}
	///////////////////

	// 描画、表示オンの時のみ
	if (S.TSwitch[5] == 0 || S.TSwitch[5] == 1){

		// 薄い四角
		SetAlpha(128);
		DrawBox(498, 58, 620, 160, GetColor(0, 0, 0), true);
		NoBlend();

		// その他のチェック
		DrawFormatString(500, 60, Cr, "ダメージ %d", m_DisplayDamage);
		// ゲージ回収
		DrawFormatString(500, 80, Cr, "ゲージ %d", mGauge);

		// フレームチェック・開始
		if ((mG_Start == 0) && (P2.ctrl == 0) &&
			((P2.HFlg == 1) || (P2.stateno >= 50 && P2.stateno <= 59))){
			mG_Start = 1, mGauge = 0;
		}
		// ゲージチェック
		if (mG_Start){
			mGauge = P1.Power - mG_startPower;
		}

		// ゲージ回収・終了
		if ((mG_Start) && (P1.ctrl && P2.ctrl)){
			mG_Start = 0;
		}
		else if (!mG_Start)mG_startPower = P1.Power;



		// 発生フレーム
		//if (P1.ctrl == 0)mHassei++;
		//else { mHassei = 0; }
		
		// 発生表示
		// 変数が表示されるので、どこでも良い？
		//DrawFormatString(500, 100, Cr, "発生 %d", mHassei);
		DrawFormatString(500, 100, Cr, "発生 %d", _drawHassei);

		// ヒットorガードした
		if ((P1.MoveHit == 0)
			&&
			(
			(P2.stateno == 1000) || (P2.stateno == 1010) || (P2.stateno == 1020) || (P2.stateno == 1030)
				|| (P2.stateno == 1060) || (P2.stateno == 1065)
				|| (P2.stateno == 50) || (P2.stateno == 51) || (P2.stateno == 52)
				)
			&& (P2.time == 0)) 
		{
			//mHassei = aSpeed + 1;	// 発生記憶
			_drawHassei = aSpeed + 1;	// ここで設定
			//if ((_oldState < 1000) || (_oldState < 50 && _oldState > 52))_drawHassei = mHassei + 1;
		}

		if (P1.ctrl)mHassei = 0;
		else { 
			mHassei++; 
			if (!P2.ctrl)mHassei--;
		}

		_oldState = P2.stateno;


		// フレームチェック
		DrawFormatString(500, 120, Cr, "硬直差 %d", mFrame);
		
		// フレームチェック・開始
		// 計測が始まっていない 2Pが喰らいになった
		if ((mF_Start == 0) && (P2.ctrl == 0) && 
			((P2.HFlg == 1) || (P2.stateno >= 50 && P2.stateno <= 59))){
			mF_Start = 1, mFrame = 0;
		}

		// 時間チェック
		if (mF_Start){
			if ((P1.ctrl) && (!P2.ctrl)){
				mFrame += 1;
			}
			else if ((!P1.ctrl) && (P2.ctrl)){
				mFrame -= 1;
			}
		}

		// フレームチェック・終了
		if ((mF_Start) && (P1.ctrl && P2.ctrl)){
			mF_Start = 0;
		}

		//DrawFormatString(500, 140, Cr, "補正 %.2f", P1.A.hosei_K);
		if (S.TSwitch[2] <= 2)DrawFormatString(500, 140, Cr, "ダミー");
		else if (S.TSwitch[2] == 3)DrawFormatString(500, 140, Cr, "プレイヤー操作");
		else if (S.TSwitch[2] == 4)DrawFormatString(500, 140, Cr, "コンピュータ");

		//-------------
		// のけぞり
		//-------------
		//if (P2.D.nokezori > 0)
			//DrawBox(340, 80, 340 + (P2.D.nokezori * 2), 80 + 15, Cr, true);

		

		// ヘルパー
		//DrawFormatString(500, 160, Cr, "W.%d time.%d", H1[18].WAtt[1], H1[18].time);
		
		//DrawFormatString(500, 160, Cr, "%.2f", P1.XVel);
		//DrawFormatString(500, 160, Cr, "振り向き %d", P1.muki);
		
		// ガークラ
		//DrawFormatString(500, 160, Cr, "ガード %d", TText[1]);

		/*
		DrawFormatString(500, 160, Cr, "%.2f", P1.YPos);
		if ((int)P1.YPos <= GROUND - 190){
			DrawBox(0, GROUND - S.ScroolY - 190, STAGE_WIDTH, GROUND - S.ScroolY -191, 
				Oran, true);
		}
		else{
			DrawBox(0, GROUND - S.ScroolY - 190, STAGE_WIDTH, GROUND - S.ScroolY -191, 
				Cr, true);
		}
		*/

		//DrawFormatString(500, 160, Cr, "%s", P1.nameC);	// 初期化できてる
		//DrawFormatString(500, 180, Cr, "XV:%.2f", P1.C.runF[0]);	// 初期化できてる
		//DrawFormatString(500, 200, Cr, "t1:%d", TText[1]);	// 初期化できてる
		//DrawFormatString(500, 220, Cr, "t2:%d", TText[2]);	// 初期化できてる
		//DrawFormatString(500, 240, Cr, "t3:%d", TText[3]);	// 初期化できてる
		//DrawFormatString(500, 260, Cr, "1:%d", TText[4]);	// 初期化できてる
		//DrawGraph(300, 260, TText[4], true);
		//DrawFormatString(500, 160, Cr, "%.2f", (float)((float)P1.Life / (float)P1.C.lifeMax));
		//if ((P1.YVel > 0) || ((P1.YVel < 0) && (P1.YPos < GROUND - 135)))DrawString(500, 160, "AirOK", Cr);
		//DrawFormatString(500, 160, Cr, "%d, %d", P1.Muteki, P2.Muteki);
		/*
		DrawFormatString(500, 160, Cr, "%d", P1.MoveHit);
		DrawFormatString(500, 180, Cr, "%d", P1.time);
		DrawFormatString(500, 200, Cr, "%d, %d", P1.WAtt[0], P1.HAtt[0]);
		*/
		//int xDist = (int)(P2.XPos - P1.XPos);
		//if (xDist < 0)xDist = -xDist;
		//int yDist = (int)(P2.YPos - P1.YPos);
		//if (yDist < 0)yDist = -yDist;
		
		
		//DrawFormatString(500, 160, Cr, "%d, %d", P1.Var[11], P1.D.armor);
		//DrawFormatString(500, 180, Cr, "F %d, T %d", P1.mutekiF, P1.Muteki);
		
		//DrawFormatString(500, 160, Cr, "%d", P1.addG);
		//DrawFormatString(500, 160, Cr, "c1:%d c4:%d", P1.cmd[1], P1.cmd[4]);
		//DrawFormatString(500, 180, Cr, "%d %d", P2.button[1], P2.rbutton[1]);
		//DrawFormatString(500, 215, Cr, "文字 %s", TChar);
		//DrawFormatString(500, 180, Cr, "テスト %d", TText);

	}

		//m_cPos = P1.XPos;
		if (S.TSwitch[5] == 0 || S.TSwitch[5] == 2){

		int ke = P1.keyPos;
		
		// 追加
		if (P1.muki == 1){
			if (P1.keyPos == 1)ke = 3;
			else if (P1.keyPos == 3)ke = 1;
			else if (P1.keyPos == 4)ke = 6;
			else if (P1.keyPos == 6)ke = 4;
			else if (P1.keyPos == 7)ke = 9;
			else if (P1.keyPos == 9)ke = 7;
		}
		
		// キーが変更された
		//if (S.StopTime == 0)
		{
		for (int i = 0; i < 9; i++){
			if ((ke == i+1) && (ke != 5) && (ke != oKey[0] + 1 || ke != saveKey)){
				sKey = true;
				break;
			}
			else { sKey = false; }
		}

		// ボタンが入力された
		for (int i = 0; i < 4; i++){
			if (P1.button[i + 1] == 1){
				sButton = true;
				sKey = true;
				break;
			}
			else { sButton = false; }
		}

		// 時間経過
		o_nTime++;
		if (o_nTime > 9999)o_nTime = 9999;
		
		// キーorボタン開始
		if (sKey || sButton){

			// キー
			// ずらす
			for (int a = 1; a < LOG_MAX; a++){
				oKey[LOG_MAX - a] = oKey[LOG_MAX - 1 - a];
			}
			// キー方向
			for (int i = 0; i < 9; i++){
				if (ke == i+1){
					oKey[0] = i;

					//else{ oKey[0] = i; }
					break;
				}
				else { oKey[0] = 0; }
			}

			// ボタン
			for (int a = 1; a < LOG_MAX; a++){
				for (int i = 0; i < 4; i++){
					oButton[LOG_MAX - a][i] = oButton[LOG_MAX - 1 - a][i];
				}
			}
			for (int i = 0; i < 4; i++){
				if (P1.button[i + 1] == 1){
					oButton[0][i] = 1;
				}
				// 押しっぱの表示
				else if (P1.button[i + 1] > 1) {
					oButton[0][i] = 2;
				}
				else { oButton[0][i] = 0; }
			}

			// タイム
			for (int a = 1; a < LOG_MAX; a++) {				
				oTime[LOG_MAX - a] = oTime[LOG_MAX - 1 - a];
			}
			oTime[0] = o_nTime;
			o_nTime = 0;
		}
		}	//  stoptime

		// 描画
		int px;
		for (int a = 0; a < LOG_MAX; a++){
			px = 0;
			if (oKey[a] > -1){
				DrawGraph(px, 95 + (a*20), gKey[oKey[a]], true);
			}
			px = 20;
			for (int i = 0; i < 4; i++){
				if (oButton[a][i] == 1){
					DrawGraph(px, 95 + (a * 20), gButton[i], true);
					px += 20;
				}
				else if (oButton[a][i] > 1) {
					SetDrawBright(128, 128, 128);
					DrawGraph(px, 95 + (a * 20), gButton[i], true);
					SetDrawBright(255, 255, 255);
					px += 20;
				}
			}
			px += 20;
			// タイム
			if (oTime[a] > -1) {
				DrawFormatString(px, 95 + (a * 20), Cr, "%d", oTime[a]);
			}
			DrawFormatString(0, 75, Cr, "%d", o_nTime);
		}
		
		saveKey = ke;
		}	// S.T[5]

		//DrawFormatString(0, 35, Cr, "テンキー %d", P1.keyPos);
		
		// ミニ画像
		//DrawRotaGraph(20, 25, 0.4, 0, P1.Image, true, P1.muki);
		//DrawRotaGraph(SCREEN_W - 20, 25, 0.4, 0, P2.Image, true, P2.muki);
		
		// テスト画像
		//DrawRotaGraph(120, 125, 0.5, 0, TText, true, 0);
		//DrawFormatString(500, 155, Cr, "猶予 %d", TText);
		//DrawFormatString(500, 135, Cr, "C入力 %d", P1.Senkou[3]);
		//DrawFormatString(500, 155, Cr, "停止 %d", S.Anten);


		//DrawFormatString(500, 115, Cr, "X速度 %.1f", P1.XVel);
		//DrawString(500, 115, "ガード属性", Cr);
		//if (P1.GuardF == 0)DrawString(590, 115, "ガー不", Cr);
		//else if (P1.GuardF == 1)DrawString(590, 115, "上段", Cr);
		//else if (P1.GuardF == 2)DrawString(590, 115, "中段", Cr);
		//else if (P1.GuardF == 3)DrawString(590, 115, "下段", Cr);

	

	// ボックス描画
	{
		// 描画ブレンドモードをアルファブレンド（透明）にする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 122);

		// 喰らい判定
		if (S.TSwitch[6] == 1 || S.TSwitch[6] == 3)BoxKurai();
		// 攻撃判定
		if (S.TSwitch[6] == 2 || S.TSwitch[6] == 3)BoXAtt();
		

		// 描画ブレンドモードをノーブレンドにする
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	}
}


void GetP_ObjectDraw(Player GP1, Player GP2)
{
	P1 = GP1;
	P2 = GP2;
}

void GetS_ObjectDraw(System_t GS)
{
	S = GS;
}

void GetH_ObjectDraw(Helper GH1[], Helper GH2[])
{
	int i;
	for (i = 0; i < HM; i++){
		H1[i] = GH1[i];
		H2[i] = GH2[i];
	}
}

void BoxKurai()
{
	int color;

	for (int j = 0; j < 2; j++){

		// 最初に青を準備
		color = GetColor(0, 0, 255);
		// 無敵で緑に
		if (j == 0){
			if (P1.Muteki != 0) {
				switch (P1.mutekiF){
				case 0:
					color = GetColor(0, 255, 0);
					break;
				case 1:
					color = GetColor(0, 255, 255);
					break;
				case 2:
					color = GetColor(255, 255, 0);
					break;
				case 3:
					color = GetColor(255, 255, 0);
					break;
				}
			}

		}
		if(j == 1){
			if (P2.Muteki != 0) {
				switch (P2.mutekiF){
				case 0:
					color = GetColor(0, 255, 0);
					break;
				case 1:
					color = GetColor(0, 255, 255);
					break;
				case 2:
					color = GetColor(255, 255, 0);
					break;
				case 3:
					color = GetColor(255, 255, 0);
					break;
				}
			}
		}
		// 2P 喰らい判定チェック
		for (int i = 0; i < HIT_MAX; i++){
			// スクロール量を足す
			if (j == 0){
				m_XD[i] = P1.XHit[i] - (int)S.ScroolX;
				m_YD[i] = P1.YHit[i] - (int)S.ScroolY;
				m_WD[i] = P1.WHit[i];
				m_HD[i] = P1.HHit[i];
			}
			else if (j == 1){
				m_XD[i] = P2.XHit[i] - (int)S.ScroolX;
				m_YD[i] = P2.YHit[i] - (int)S.ScroolY;
				m_WD[i] = P2.WHit[i];
				m_HD[i] = P2.HHit[i];
			}

			if (m_WD[i] > 0){
				// 描画
				DrawBox(m_XD[i], m_YD[i], m_XD[i] +
					m_WD[i], m_YD[i] + m_HD[i], color, true);
			}
				
		}
		// 座標
		if (j == 0){
			DrawBox((int)P1.XPos - 3 - (int)S.ScroolX, (int)P1.YPos - 3 - (int)S.ScroolY,
				(int)P1.XPos + 3 - (int)S.ScroolX, (int)P1.YPos + 3 - (int)S.ScroolY, Oran, true);
		}
		else if (j == 1){
			DrawBox((int)P2.XPos - 3 - (int)S.ScroolX, (int)P2.YPos - 3 - (int)S.ScroolY,
				(int)P2.XPos + 3 - (int)S.ScroolX, (int)P2.YPos + 3 - (int)S.ScroolY, Oran, true);
		}


		// ヘルパー喰らい判定
		for (int i = 0; i < HM; i++){
			for (int n = 0; n < HIT_MAX; n++){
				mHXH[0][i] = H1[i].XHit[n] - (int)S.ScroolX;
				mHYH[0][i] = H1[i].YHit[n] - (int)S.ScroolY;
				mHWH[0][i] = H1[i].WHit[n];
				mHHH[0][i] = H1[i].HHit[n];

				mHXH[1][i] = H2[i].XHit[n] - (int)S.ScroolX;
				mHYH[1][i] = H2[i].YHit[n] - (int)S.ScroolY;
				mHWH[1][i] = H2[i].WHit[n];
				mHHH[1][i] = H2[i].HHit[n];

				// 攻撃判定チェック
				DrawBox(mHXH[j][i], mHYH[j][i], mHXH[j][i] + mHWH[j][i],
					mHYH[j][i] + mHHH[j][i], GetColor(0, 0, 255), true);

				
			}
			if (H1[i].var){
				// 座標
				DrawBox((int)H1[i].XPos - 3 - (int)S.ScroolX, (int)H1[i].YPos - 3 - (int)S.ScroolY,
					(int)H1[i].XPos + 3 - (int)S.ScroolX, (int)H1[i].YPos + 3 - (int)S.ScroolY, Oran, true);
			}
		}
	}	// for終了

}

void BoXAtt()
{
	for (int j = 0; j < 2; j++){
		
		/*
		{
			mXA[0][0] = P1.X_UL - (int)S.ScroolX;
			mYA[0][0] = P1.Y_UL - (int)S.ScroolY;
			mWA[0][0] = P1.xSize;
			mHA[0][0] = P1.ySize;

			// キャラサイズチェック
			DrawBox(mXA[0][0], mYA[0][0], mXA[0][0] + mWA[0][0],
				mYA[0][0] + mHA[0][0], GetColor(0, 240, 0), true);
		}
		*/

		for (int i = 0; i < HIT_MAX; i++){
			mXA[0][i] = P1.XAtt[i] - (int)S.ScroolX;
			mYA[0][i] = P1.YAtt[i] - (int)S.ScroolY;
			mWA[0][i] = P1.WAtt[i];
			mHA[0][i] = P1.HAtt[i];

			mXA[1][i] = P2.XAtt[i] - (int)S.ScroolX;
			mYA[1][i] = P2.YAtt[i] - (int)S.ScroolY;
			mWA[1][i] = P2.WAtt[i];
			mHA[1][i] = P2.HAtt[i];

			if (mWA[j][i] > 0){
				// 攻撃判定チェック
				DrawBox(mXA[j][i], mYA[j][i], mXA[j][i] + mWA[j][i],
					mYA[j][i] + mHA[j][i], GetColor(255, 0, 0), true);
			}
		}


		// ヘルパー攻撃判定
		for (int i = 0; i < HM; i++){
			for (int n = 0; n < HIT_MAX; n++){
				mHXA[0][i] = H1[i].XAtt[n] - (int)S.ScroolX;
				mHYA[0][i] = H1[i].YAtt[n] - (int)S.ScroolY;
				mHWA[0][i] = H1[i].WAtt[n];
				mHHA[0][i] = H1[i].HAtt[n];

				mHXA[1][i] = H2[i].XAtt[n] - (int)S.ScroolX;
				mHYA[1][i] = H2[i].YAtt[n] - (int)S.ScroolY;
				mHWA[1][i] = H2[i].WAtt[n];
				mHHA[1][i] = H2[i].HAtt[n];

				// 攻撃判定チェック
				DrawBox(mHXA[j][i], mHYA[j][i], mHXA[j][i] + mHWA[j][i],
					mHYA[j][i] + mHHA[j][i], GetColor(255, 0, 0), true);
			}
		}

	}
}


void TestText(int i, int pos)
{
	TText[pos] = i;
}

void TestChar(char ch[], int size)
{
	for (int i = 0; i < size; i++){
		TChar[i] = ch[i];
	}
}

// 発生フレーム
void AttackSpeed(int i)
{
	aSpeed = i;
}

// ダメージ
void Damage_Check(int i)
{
	m_AllDamage += i;
}

// フレームチェック
void Frame_Check()
{
	mF_Start = 1, mFrame = 0;
}

// 連続ヒット(青ビ)確認
void Beat_Check(int state, int pside)
{
	// 1P
	if (pside == 1){
		// 喰らっている
		if (P2.HFlg){
			// 受け身可能なら青ビ
			if (state == 1035){
				beat[0] = false;
			}
		}
		// 喰らってない時は赤ビ
		else{
			beat[0] = true;
		}
	}

	// 2P
	else if (pside == 2){
		if (P1.HFlg){
			if (state == 1035){
				beat[1] = false;
			}
		}
		else{
			beat[1] = true;
		}
	}
}

// キー表示
void ObjectDraw_KeyDisplay()
{
	SetAlpha(158);
	DrawGraph((SCREEN_W / 2) - 60, 65, m_KeyPos, true);

	SetAlpha(255);
	int x = 0, y = 0;
	if (P1.button[102] > 0)y = 1;
	if (P1.button[108] > 0)y = -1;
	if (P1.button[106] > 0)x = 1;
	if (P1.button[104] > 0)x = -1;
	DrawGraph((SCREEN_W / 2) - 60 + 12 + (x * 16), 65 + 12 + (y * 16), m_Key, true);

	SetAlpha(255);
	if (P1.button[1] == 0){
		SetAlpha(128);
	}
	DrawGraph((SCREEN_W/2) + 10, 65, m_Button[0], true);

	SetAlpha(255);
	if (P1.button[2] == 0){
		SetAlpha(128);
	}
	DrawGraph((SCREEN_W / 2) + 35, 65, m_Button[1], true);

	SetAlpha(255);
	if (P1.button[3] == 0){
		SetAlpha(128);
	}
	DrawGraph((SCREEN_W / 2) + 60, 65, m_Button[2], true);

	SetAlpha(255);
	if (P1.button[4] == 0){
		SetAlpha(128);
	}
	DrawGraph((SCREEN_W / 2) + 10, 90, m_Button[3], true);

	NoBlend();

	////////////////////////
	// P2
	SetAlpha(158);
	DrawGraph((SCREEN_W / 2) - 60, 125, m_KeyPos, true);

	SetAlpha(255);
	x = 0, y = 0;
	if (P2.button[102] > 0)y = 1;
	if (P2.button[108] > 0)y = -1;
	if (P2.button[106] > 0)x = 1;
	if (P2.button[104] > 0)x = -1;
	DrawGraph((SCREEN_W / 2) - 60 + 12 + (x * 16), 125 + 12 + (y * 16), m_Key, true);

	SetAlpha(255);
	if (P2.button[1] == 0){
		SetAlpha(128);
	}
	DrawGraph((SCREEN_W / 2) + 10, 125, m_Button[0], true);

	SetAlpha(255);
	if (P2.button[2] == 0){
		SetAlpha(128);
	}
	DrawGraph((SCREEN_W / 2) + 35, 125, m_Button[1], true);

	SetAlpha(255);
	if (P2.button[3] == 0){
		SetAlpha(128);
	}
	DrawGraph((SCREEN_W / 2) + 60, 125, m_Button[2], true);

	SetAlpha(255);
	if (P2.button[4] == 0){
		SetAlpha(128);
	}
	DrawGraph((SCREEN_W / 2) + 10, 150, m_Button[3], true);

	NoBlend();

	// リプレイの経過時間
	SetAlpha(128);
	DrawBox(0, SCREEN_H - 21, 100, SCREEN_H, GetColor(0, 0, 0), true);
	DrawBox(198, SCREEN_H - 21, SCREEN_W, SCREEN_H, GetColor(0, 0, 0), true);
	NoBlend();
	// 経過時間表示
	DrawFormatString(0, SCREEN_H - 20, Cr, "time:%d", m_ReplayTime);
	m_ReplayTime++;

	// 説明
	DrawFormatString(200, SCREEN_H - 20, Cr, "S:リプレイ停止  停止中 S:再生 →:1コマ進める B:終了");
}

// アドゲージ
void AddGaugeDraw(int power, int side)
{
	addGaugeTime[side - 1] = 60;
	addGaugePower[side - 1] = power;
}

// 文字表示
// 1..カウンター
// side..3で両方
void Display_Text(int text, int side)
{
	// カウンター
	if (side == 1) {
		display_time[0] = 30;
	}
	if (side == 2) {
		display_time[1] = 30;
	}
	if (side == 3) {
		display_time[0] = display_time[1] = 30;
	}
}
// 黒の度合い
void SetBlack(int b)
{
	blackOut = b;
	// 最大値設定
	max(0, min(255, blackOut));	// 右に今の数値を
}