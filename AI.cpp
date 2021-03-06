//インクルード部--------------------------------------------
#include "pch.h"
#include "System.h"

static int oldSt;
static int reversalTime;
static int xDist, yDist;

static void charAI(int num);


void AI_Move()
{

	xDist = (int)(P2.XPos - P1.XPos);
	if (xDist < 0)xDist = -xDist;
	yDist = (int)(P2.YPos - P1.YPos);
	if (yDist < 0)yDist = -yDist;

	// P1,P2の変数に保存
	for (int i = 0; i < PAD_MAX; i++){
		//P2.button[i] = 0;
	}
	//P2.stateno = oldSt;

	// 2P
	if ((P2.ctrl) && (P2.SFlg != 2)){
		// 下入力していて、ステート11じゃない
		if ((P2.SFlg == 1) && (P2.stateno != 11) && (P2.Lock == 0)){
			P2.stateno = 10;
		}
		else if ((P2.SFlg == 0) && (P2.Lock == 0)){
			P2.stateno = 0;
		}

	}

	// 振り向き
	if ((P2.SFlg != 2) && (P2.ctrl) && (P2.Turn) && (P2.stateno < 1000)){
		if ((P2.XPos < P1.XPos) && (P2.muki == 1)){
			P2.muki = 0;
			P2.XVel = -P2.XVel;
			if (P2.SFlg == 1)P2.stateno = 6;
			else{ P2.stateno = 5; }
		}
		if ((P2.XPos > P1.XPos) && (P2.muki == 0)){
			P2.muki = 1;
			P2.XVel = -P2.XVel;
			if (P2.SFlg == 1)P2.stateno = 6;
			else{ P2.stateno = 5; }
		}
	}

	// 前歩き
	if ((random(30)) && (P2.ctrl) && (P2.SFlg != 2) && (P2.stateno != 20)){
		P2.stateno = 20;
	}
	if ((P2.ctrl) && (P2.SFlg != 2) && ((P2.stateno == 20) || (oldSt == 20))
		&& ((P2.stateno != 21) || (oldSt != 21))){
		P2.stateno = 20;
		if ((P2.time >= 13) || (P2.time >= P2.animElem[P2.stateno])){
			// 確率で停止
			if (random(80)) {
				P2.stateno = 0, P2.time = 0, P2.ctrl = 1;
			}
		}
	}
	// 後ろ歩き
	if ((random(30)) && (P2.ctrl) && (P2.SFlg != 2) && (P2.stateno != 21)){
		P2.stateno = 21;
	}
	if ((P2.ctrl) && (P2.SFlg != 2) && ((P2.stateno == 21) || (oldSt == 21))
		&& ((P2.stateno != 20) || (oldSt != 20))){
		P2.stateno = 21;
		if ((P2.time >= 12) || (P2.time >= P2.animElem[P2.stateno])){
			if (random(80)) {
				P2.stateno = 0, P2.time = 0, P2.ctrl = 1;
			}
		}
	}

	// ダッシュ
	if ((xDist > 320) && (P2.ctrl) && (P2.SFlg != 2) && (P2.stateno != 30)){
		if (random(10))P2.stateno = 30;
	}
	if ((P2.ctrl) && (P2.SFlg != 2) && ((P2.stateno == 30) || (oldSt == 30))){
		if ((xDist > 320) || (P1.time <= 10))P2.stateno = 30;
		else{ P2.stateno = 0, P2.time = 0, P2.ctrl = 1; }
	}
	
	// バクステ
	if ((xDist < 120) && (P2.ctrl) && (P2.SFlg != 2)){
		if (random(15))P2.stateno = 35, P2.ctrl = 0, P2.time = 0;
	}
	// 適当な地上攻撃
	if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)){
		if ((random(16)) && (xDist < 80) && (yDist < 120)){
			if (random(500))P2.stateno = 200;
			else{ P2.stateno = 300; }
		}
		else if ((random(16)) && (xDist < 130) && (yDist < 120)){
			if (random(500))P2.stateno = 210;
			else{ P2.stateno = 310; }
		}
		else if ((random(16)) && (xDist < 180) && (yDist < 120)){
			if (random(500))P2.stateno = 220;
			else{ P2.stateno = 320; }
		}
	}
	// 適当な空中攻撃
	if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg == 2)){
		if ((random(20)) && (xDist < 80) && (yDist < 0))P2.stateno = 400;
		else if ((random(20)) && (xDist < 130) && (yDist > 120))P2.stateno = 410;
		else if ((random(12)) && (xDist < 180))P2.stateno = 420;
	}
	
	// チェーン
	if (P2.stateno >= 200 && P2.stateno <= 499 && P1.stateno >= 1000 && P2.scTime > 0 
		&& (P2.CFlg) && (P2.StopTime == 0)){
		if (P2.stateno == 200 || P2.stateno == 300){
			if (random(20)){
				P2.stateno = 210, P2.time = 0, P2.ctrl = 0;
			}
			else if (random(20)){
				P2.stateno = 310, P2.time = 0, P2.ctrl = 0;
			}
		}
		else if (P2.stateno == 210 || P2.stateno == 310){
			if (random(20)){
				P2.stateno = 220, P2.time = 0, P2.ctrl = 0;
			}
			else if (random(20)){
				P2.stateno = 320, P2.time = 0, P2.ctrl = 0;
			}
		}
		else if (P2.stateno == 400){
			if (random(40)){
				P2.stateno = 410, P2.time = 0, P2.ctrl = 0;
			}
		}
		else if (P2.stateno == 410){
			if (random(40)){
				P2.stateno = 420, P2.time = 0, P2.ctrl = 0;
			}
		}
	}

	// 投げ(空中投げ)
	if ((P2.ctrl) && (P2.stateno < 200)){
		if ((random(18)) && (xDist < 70)){
			if (P2.SFlg != 2 && P1.SFlg != 2)P2.stateno = 500;
			else if (P2.SFlg == 2 && P1.SFlg == 2){ P2.stateno = 510; }
		}
	}

	// ジャンプ
	if ((P2.ctrl) && (P2.SFlg != 2) && (xDist < 400)){
		if (random(10)){
			P2.stateno = 40;
			// ジャンプ方向
			if (random(550)){	// 前
				if (P2.muki == 0)P2.button[106] = 1;
				else{ P2.button[104] = 1; }
			}
			else if (random(200)){	// 後ろ
				if (P2.muki == 0)P2.button[104] = 1;
				else{ P2.button[106] = 1; }
			}	// 垂直
			else{ P2.button[104] = 0, P2.button[106] = 0; }
		}
	}
	// 二段ジャンプ
	if ((P2.ctrl) && (P2.SFlg == 2) && ((P2.stateno >= 41) && (P2.stateno <= 43)) && 
		(P2.YPos < GROUND - 30) && (P2.AirJump > 0)){
		if (random(10)){
			P2.stateno = 45;
			// ジャンプ方向
			if (random(550)){	// 前
				P2.keyAtt[6]++;
			}
			else if (random(200)){	// 後ろ
				P2.keyAtt[4]++;
			}	// 垂直
			else{ P2.button[104] = 0, P2.button[106] = 0; }
		}
	}

	// 立ちガード
	if ((P1.stateno >= 200) && (P1.stateno <= 999)
		&& (xDist < 320) && (random(650))){
		if (P1.XPos < P2.XPos){
			P2.button[106]++;
			P2.keyAtt[4] = 1;
		}
		else if (P1.XPos > P2.XPos){
			P2.button[104]++;
			P2.keyAtt[4] = 1;
		}

		if (P1.XPos == P2.XPos){
			if (P2.muki == 0){
				P2.button[104]++;
				P2.keyAtt[4] = 1;
			}
			else { 
				P2.button[106]++; 
				P2.keyAtt[4] = 1;
			}
		}

		// しゃがみガード
		if (((P1.SFlg == 1) && (random(800))) || (random(90))){
			P2.button[102]++;
			P2.keyAtt[2] = 1;
		}
	}
	for (int i = 0; i < HELPER_MAX; i++){
		// ヘルパー
		if ((H1[i].var) &&
			(H1[i].HMoveHit > 0 && H1[i].A.damage > 0) &&
			(H1[i].WHit > 0) && (random(650))
			){
			/*
			((H1[i].XPos - P2.XPos < 105) &&
			(P2.XPos - H1[i].XPos < 105)) ||
			*/
			if (P1.XPos < P2.XPos){
				P2.button[106]++;
				P2.keyAtt[4] = 1;
			}
			else if (P1.XPos > P2.XPos){
				P2.button[104]++;
				P2.keyAtt[4] = 1;
			}
			if (P1.XPos == P2.XPos){
				if (P2.muki == 0){
					P2.button[104]++;
					P2.keyAtt[4] = 1;
				}
				else { 
					P2.button[106]++; 
					P2.keyAtt[4] = 1;
				}
			}

		}
	}
	// しゃがみガード方向維持
	if (P2.stateno == 51){
		P2.button[102]++;
		P2.keyAtt[2] = 1;
	}

	// 受け身
	if ((P2.stateno == 1035 || P2.stateno == 1030) && (random(800))){
		P2.button[1]++;
		// 受け身方向
		if (random(600)){
			P2.keyAtt[6] = 1;
			P2.keyAtt[4] = 0;
		}
		// 前受け身
		else if (random(600)){
			P2.keyAtt[6] = 0;
			P2.keyAtt[4] = 1;
		}
		else{ 
			P2.keyAtt[6] = 0;
			P2.keyAtt[4] = 0;
		}
	}

	// 投げ抜け
	if (((P1.stateno == 505) || (P1.stateno == 515)) && (P1.time == 1) && (random(160))){
		P2.Senkou[1]++;
		P2.Senkou[2]++;
	}

	// キャラ別行動
	charAI(P2.Name);

	//----------------
	// 共通処理
	//----------------
	// ジャンプ回数設定、2回まで
	if ((P2.SFlg != 2) && (P2.YPos == GROUND) && (P2.stateno != 40)) {
		P2.AirJump = P2.C.airJump;
		P2.airDash = P2.C.airDash;
		P2.hyJump = false;
	}

	oldSt = P2.stateno;
	Get_PSet(P1, P2);
}

void charAI(int num)
{
	// アイン
	if (num == EIN){
		// 地上
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)){
			if ((random(40)) && (xDist <= 185) && (yDist < 120))P2.stateno = 210;
			if ((random(5)) && (xDist > 185) && (!H2[1].var))P2.stateno = 600;
			if ((xDist > 210)){
				if (!H2[2].var){
					if ((random(10)))P2.stateno = 620;
				}
				else if ((H2[2].var) && (H2[2].stateno == 2020)){
					if ((random(20)) && (P1.stateno < 1050))P2.stateno = 625;
				}
			}

			// 対空
			if ((random(200)) && (xDist <= 175) && (yDist > 140) && (P1.YVel > -1.0))P2.stateno = 310;
		}
		// 空中
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg == 2)){
			if ((random(200)) && (xDist < 180) && (yDist < 200) && (P2.YVel > 0))P2.stateno = 420;
		}
		// 立ち回りゲージ技
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2) && (P1.stateno < 1050)){
			if ((random(8)) && (xDist < 220) && (P2.Power >= 1000))
				P2.stateno = 800; // ぶっぱ
			if ((random(30)) && (xDist > 200) && (P2.Power >= 1000) && (H2[2].var) &&
				(H2[2].stateno == 2020)){
				// 
				P2.stateno = 820;	// 竜巻
			}
		}

		// キャンセル
		if ((!P2.ctrl) && (P2.CFlg) && (!P1.ctrl) && (P2.StopTime == 0) && (P2.time > 0)){
			if ((P2.stateno == 320) && (random(100)) && (P2.scTime > 5)){
				if (!H2[2].var){
					P2.stateno = 620, P2.time = 0, P2.ctrl = 0;
				}
				else{
					P2.stateno = 625, P2.time = 0, P2.ctrl = 0;
				}
			}

			if ((P2.stateno == 210) && (P2.scTime > 5)){
				if (P1.SFlg == 2){
					P2.stateno = 310, P2.time = 0, P2.ctrl = 0;
				}
				else{
					P2.stateno = 320, P2.time = 0, P2.ctrl = 0;
				}
			}

			// JC
			if ((P2.stateno == 410) && (P2.scTime > 5)){
				if (P1.SFlg == 2){
					P2.stateno = 420, P2.time = 0, P2.ctrl = 0;
				}
			}

			// ジャンプキャンセル
			if ((P2.stateno == 310) && (random(200) || P1.HFlg == 1) && (P2.scTime > 5) && (P1.SFlg == 2)){
				P2.stateno = 40, P2.time = 0, P2.ctrl = 0;
				//P2.button[108]++;
				if (P2.muki == 0)P2.button[106]++;
				else{ P2.button[104]++; }
			}
		}
		// 対空から追撃
		if ((P2.ctrl) && (P2.stateno == 42) && (P1.stateno == 1030)){
			P2.stateno = 410, P2.time = 0, P2.ctrl = 0;
		}
	}

	// コーネル
	else if (num == CORNEL){
		// ジャンプ
		if ((P2.ctrl) && (P2.SFlg != 2) && (xDist < 460)) {
			if (random(8)) {
				P2.stateno = 40;
				// ジャンプ方向
				if (random(600)) {	// 前
					if (P2.muki == 0)P2.button[106] = 1;
					else { P2.button[104] = 1; }
				}
				//
				else if (random(200)) {	// 後ろ
					if (P2.muki == 0)P2.button[104] = 1;
					else { P2.button[106] = 1; }
				}
			}
		}
		// 地上攻撃
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)){
			// シュート
			if ((random(8)) && (xDist > 220 && xDist < 300) && (yDist < 120))P2.stateno = 630;
			// スラ
			if ((random(20)) && (xDist > 180 && xDist < 220) && (yDist < 120))P2.stateno = 320;
			// 5C
			if ((random(50)) && (xDist < 170) && (yDist < 120))P2.stateno = 220;
			if ((random(80)) && (xDist < 130) && (yDist < 120))P2.stateno = 210;
			
			
			// 対空
			if ((random(200)) && (xDist <= 175) && (yDist > 140) && (P1.YVel > -1))P2.stateno = 321;
		}
		// 空中
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg == 2)){
			if ((random(200)) && (xDist < 180) && (yDist < 200) && (P2.YVel > 0))P2.stateno = 420;
			// JD
			if ((random(30)) && (xDist > 80) && (P2.YPos < GROUND - 100))P2.stateno = 430;
		}
		// ゲージ技
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)){
			if ((random(10)) && (xDist > 240) && (P2.Power >= 2000) && (!H2[1].var))P2.stateno = 810;
		}
		// リバサ
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)
			&& (oldSt == 1055) && (P1.stateno < 1000)) {
			// バクステ
			if ((random(100)) && (xDist < 150))P2.stateno = 35;
			// 昇竜
			if ((random(250)) && (xDist < 150))P2.stateno = 621;
			// 1ゲージぶっぱ
			if ((random(100)) && (xDist < 150) && (P2.Power >= 1000))P2.stateno = 820;
		}
		// キャンセル
		if ((!P2.ctrl) && (P2.CFlg) && (!P1.ctrl) && (P2.StopTime == 0) && (P2.time > 0)){
			if ((P2.stateno == 210) && (random(100)) && (P2.scTime > 5)){
				P2.stateno = 220, P2.time = 0, P2.ctrl = 0;
			}
			if ((P2.stateno == 220) && (random(100)) && (P2.scTime > 5)){
				P2.stateno = 600, P2.time = 0, P2.ctrl = 0;
			}
			else if ((P2.stateno == 600) && (random(100)) && (P2.scTime > 5)){
				P2.stateno = 601, P2.time = 0, P2.ctrl = 0;
			}
			else if ((P2.stateno == 601) && (P1.stateno >= 1000)){
				P2.stateno = 602, P2.time = 0, P2.ctrl = 0;
			}

			// 3C追撃
			if ((P2.stateno == 321) && (P2.scTime > 5) && (P1.SFlg == 2)){
				P2.stateno = 620, P2.time = 0, P2.ctrl = 0;
			}
		}
		// エリアル
		if ((!P2.ctrl) && (P2.SFlg == 2) && (P2.CFlg) && (!P1.ctrl) && (P2.StopTime == 0) && (P2.time > 0)){
			// ジャンプキャンセル
			if ((P2.stateno == 410) && (random(100)) && (P2.scTime > 5) && (P2.AirJump > 0) && (P1.SFlg == 2)){
				P2.stateno = 45, P2.time = 0, P2.ctrl = 0;
			}
			if ((P2.stateno == 220) && (random(100)) && (P2.scTime > 5)){
				P2.stateno = 600, P2.time = 0, P2.ctrl = 0;
			}
			else if ((P2.stateno == 600) && (random(100)) && (P2.scTime > 5)){
				P2.stateno = 601, P2.time = 0, P2.ctrl = 0;
			}
		}
	}

	// バウンサー
	else if (num == BOUNCER){
		// ジャンプ
		if ((P2.ctrl) && (P2.SFlg != 2) && (xDist < 350)){
			if (random(10)){
				P2.stateno = 40;
				// ジャンプ方向
				{	// 前
					if (P2.muki == 0)P2.button[106] = 1;
					else{ P2.button[104] = 1; }
				}
			}
		}
		// 地上攻撃
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)){
			// 対空
			if ((random(200)) && (xDist <= 175) && (yDist > 140) && (P1.YVel > -1))P2.stateno = 210;
			// 猿1
			if ((random(10)) && (xDist <= 320))P2.stateno = 225;
		}
		// 空中
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg == 2)){
			if ((random(200)) && (xDist < 180) && (yDist < 200) && (P2.YVel > 0))P2.stateno = 420;
		}
		// アーマー
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2) && (P1.time > 0)){
			if ((random(20)) && (xDist > 320) && (P2.D.armor <= 0))P2.stateno = 600;
			// スタンプ
			if ((random(10)) && (xDist > 400))P2.stateno = 240;
		}
		// 6Cアーマー
		if ((P2.stateno == 226) && (P1.stateno >= 1000)){
			P2.stateno = 600, P2.time = 0, P2.ctrl = 0;
		}
		// キャンセル
		if ((!P2.ctrl) && (P2.CFlg) && (!P1.ctrl) && (P2.StopTime == 0) && (P2.time > 0)){
			if ((P2.stateno == 220) && (P1.stateno >= 1000)){
				P2.stateno = 800, P2.time = 0, P2.ctrl = 0;
			}
			// ジャンプキャンセル
			if ((P2.stateno == 210) && (P1.HFlg == 1) && (P2.scTime > 5) && (P1.SFlg == 2)){
				P2.stateno = 40, P2.time = 0, P2.ctrl = 0;
				if (P2.muki == 0)P2.button[106]++;
				else{ P2.button[104]++; }
			}
		}
		// 対空から追撃
		if ((P2.ctrl) && (P2.stateno == 42) && (P1.HFlg == 1)){
			P2.stateno = 615, P2.time = 0, P2.ctrl = 0;
		}
	}
	// ヘリオス
	if (num == HELIOS){
		// ジャンプ
		if ((P2.ctrl) && (P2.SFlg != 2) && (xDist < 460)) {
			if (random(8)) {
				P2.stateno = 40;
				// ジャンプ方向
				if (random(400)) {	// 前
					if (P2.muki == 0)P2.button[106] = 1;
					else { P2.button[104] = 1; }
				}
				//
				else if (random(300)) {	// 後ろ
					if (P2.muki == 0)P2.button[104] = 1;
					else { P2.button[106] = 1; }
				}
			}
		}
		// 地上
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)){
			// 2C
			if ((random(30)) && (xDist <= 235) && (yDist < 120))P2.stateno = 320;
			// 5B
			if ((random(40)) && (xDist <= 200) && (yDist < 120))P2.stateno = 210;

			// チャージ
			// 走っていないなら
			if ((random(90)) && (xDist >= 260) && (P2.Var[11] < 3000) 
				&& (P2.stateno != 30))P2.stateno = 600;

			// ゲージないと下がる
			if ((random(200)) && (xDist < 260) && (P2.Var[11] < 3000))P2.stateno = 21;

			// 波動
			if ((xDist > 240)){
				if ((!H2[0].var) && (P2.Var[11] >= 2000)) {
					if ((random(20))) {
						P2.stateno = 635;
					}
				}
				else if (!H2[0].var){
					if ((random(10))) {
						P2.stateno = 630;
					}
				}
			}

			// 走る
			if (H2[0].var) {
				if((random(200)) && (xDist > 120))P2.stateno = 30;
			}

			// 対空
			if ((random(200)) && (xDist <= 175) && (yDist > 140) && (P1.YVel > -1.0))P2.stateno = 205;
		}
		// 空中
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg == 2)){
			if ((random(200)) && (xDist < 180) && (yDist < 200) && (P2.YVel > 0))P2.stateno = 420;
		}

		// ゲージ技
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)){
			if ((random(5)) && (xDist > 160) && (P2.Power >= 1000))P2.stateno = 810;
		}
		// リバサ
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)
			&& (oldSt == 1055) && (P1.stateno < 1000)) {
			// バクステ
			if ((random(200)) && (xDist < 150))P2.stateno = 35;
			// 当身
			if ((random(400)) && (xDist < 150) && (P2.Var[11] > 1000))P2.stateno = 625;
		}
		// キャンセル
		if ((!P2.ctrl) && (P2.CFlg) && (!P1.ctrl) && (P2.StopTime == 0) && (P2.time > 0)){

			if ((P2.stateno == 210) && (P2.scTime > 5)){
				if (P1.SFlg == 2){
					P2.stateno = 310, P2.time = 0, P2.ctrl = 0;
				}
				else{
					P2.stateno = 320, P2.time = 0, P2.ctrl = 0;
				}
			}

			// JC
			if ((P2.stateno == 410) && (P2.scTime > 5)){
				if (P1.SFlg == 2){
					P2.stateno = 420, P2.time = 0, P2.ctrl = 0;
				}
			}

			// ジャンプキャンセル
			if ((P2.stateno == 205) && (random(200) || P1.HFlg == 1) && (P2.scTime > 5) && (P1.SFlg == 2)){
				P2.stateno = 40, P2.time = 0, P2.ctrl = 0;
				//P2.button[108]++;
				if (P2.muki == 0)P2.button[106]++;
				else{ P2.button[104]++; }
			}
		}
		// チャージを維持
		if ((!P2.ctrl) && (P2.stateno == 600 || P2.stateno == 601)) {
			if(xDist > 260)P2.button[4]++;
		}
		// 対空から追撃
		if ((P2.ctrl) && (P2.stateno == 42) && (P1.stateno == 1030)){
			P2.stateno = 410, P2.time = 0, P2.ctrl = 0;
		}
	}

	// ダン
	else if (num == HYDE) {
		// ジャンプ
		if ((P2.ctrl) && (P2.SFlg != 2) && (xDist < 420)) {
			if (random(8)) {
				P2.stateno = 40;
				// ジャンプ方向
				if (random(300)) {	// 前
					if (P2.muki == 0)P2.button[106] = 1;
					else { P2.button[104] = 1; }
				}
				//
				else if (random(600)) {	// 後ろ
					if (P2.muki == 0)P2.button[104] = 1;
					else { P2.button[106] = 1; }
				}
			}
		}
		// 地上攻撃
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)) {

			if ((random(100)) && (xDist < 80) && (yDist < 120))P2.stateno = 300;
			if ((random(80)) && (xDist < 130) && (yDist < 120))P2.stateno = 210;
			if ((random(50)) && (xDist > 160 && xDist < 202) && (yDist < 120))P2.stateno = 320;
			
			// 弾撃ち
			if (P2.Var[10] >= 180) {
				// 下弾
				if ((random(50)) && (xDist > 240) && (yDist < 120)) {
					P2.stateno = 602;
					// たまにN
					if (random(300))P2.stateno = 600;
				}
				// 上弾
				if ((random(20)) && (xDist > 200) && (yDist > 20))P2.stateno = 601;
			}
			//  リロード
			else {
				if ((random(80)) && (xDist > 290))P2.stateno = 630;
			}
			// 対空
			if ((random(200)) && (xDist <= 175) && (yDist > 140) && (P1.YVel > -1.2))P2.stateno = 220;
		}
		// 空中
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg == 2)) {
			if ((random(200)) && (xDist < 180) && (yDist < 200) && (P2.YVel > 0))P2.stateno = 420;

			// 弾撃ち
			if (P2.Var[10] >= 180) {
				// 下弾
				if ((random(60)) && (xDist > 100) && (P2.YPos < GROUND - 80))P2.stateno = 605;
				// 上弾
				if ((random(10)) && (xDist > 120))P2.stateno = 606;
			}
		}
		// ゲージ技
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)) {
			// 3ゲージぶっぱ
			if ((random(20)) && (xDist > 190) && (P2.Power >= 3000))P2.stateno = 850;
		}
		// リバサ
		if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)
			&& (oldSt == 1055) && (P1.stateno < 1000)) {
			// バクステ
			if ((random(500)) && (xDist < 150))P2.stateno = 35;
		}
		// エリアル
		if ((!P2.ctrl) && (P2.SFlg == 2) && (P2.CFlg) && (!P1.ctrl) && (P2.StopTime == 0) && (P2.time > 0)) {
			// ジャンプキャンセル
			if ((P2.stateno == 220) && (random(100)) && (P2.scTime > 5) && (P2.AirJump > 0) && (P1.SFlg == 2)) {
				P2.stateno = 45, P2.time = 0, P2.ctrl = 0;
			}
			if ((P2.stateno == 220) && (random(100)) && (P2.scTime > 5)) {
				P2.stateno = 600, P2.time = 0, P2.ctrl = 0;
			}
			else if ((P2.stateno == 600) && (random(100)) && (P2.scTime > 5)) {
				P2.stateno = 601, P2.time = 0, P2.ctrl = 0;
			}
		}
	}
	// シュウイチ
	else if (num == SYUICHI) {
	// ジャンプ
	if ((P2.ctrl) && (P2.SFlg != 2) && (xDist < 400)) {
		if (random(8)) {
			P2.stateno = 40;
			// ジャンプ方向
			if (random(400)) {	// 前
				if (P2.muki == 0)P2.button[106] = 1;
				else { P2.button[104] = 1; }
			}
			//
			else if (random(300)) {	// 後ろ
				if (P2.muki == 0)P2.button[104] = 1;
				else { P2.button[106] = 1; }
			}
		}
	}
	// 地上攻撃
	if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)) {
		if ((random(100)) && (xDist < 100) && (yDist < 120))P2.stateno = 300;
		if ((random(80)) && (xDist > 120 && xDist < 180) && (yDist < 120))P2.stateno = 210;
		if ((random(40)) && (xDist > 180 && xDist < 240) && (yDist < 120))P2.stateno = 320;
		// 6C
		if ((random(30)) && (xDist > 230 && xDist < 330) && (yDist < 120))P2.stateno = 225;
		// 対空
		if ((random(200)) && (xDist <= 175) && (yDist > 140) && (P1.YVel > -0.9))P2.stateno = 205;
	}
	// 空中
	if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg == 2)) {
		// JB
		if ((random(200)) && (xDist < 170) && (yDist < 200) && (P2.YVel > 0))P2.stateno = 410;
		// JC
		if ((random(200)) && (xDist < 180) && (yDist >= 200) && (P2.YPos < P1.YPos))P2.stateno = 420;
	}
	// ゲージ技
	if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2)) {
		// 1ゲージぶっぱ
		if ((random(20)) && (xDist < 120) && (P2.Power >= 1000) && (P1.HFlg == 0))P2.stateno = 810;
	}
	// リバサ
	if ((P2.ctrl) && (P2.stateno < 200) && (P2.SFlg != 2) 
		&& (oldSt == 1055) && (P1.stateno < 1000)) {
		// バクステ
		if ((random(200)) && (xDist < 150))P2.stateno = 35;
		// 当身
		if ((random(80)) && (xDist < 150))P2.stateno = 610;
		// 当身(下段)
		if ((random(60)) && (xDist < 150))P2.stateno = 611;
		// 1ゲージぶっぱ
		if ((random(200)) && (xDist < 150) && (P2.Power >= 1000))P2.stateno = 810;
	}
	// キャンセル
	if ((!P2.ctrl) && (P2.CFlg) && (!P1.ctrl) && (P2.StopTime == 0) && (P2.time > 0)) {

		// 6C > 214C
		if ((P2.stateno == 225) && (P2.scTime > 5) && (P2.Power >= 1000) && (random(50))) {
			P2.stateno = 811, P2.time = 0, P2.ctrl = 0;
			//ゲージ消費 
			P2.Power -= 1000;
		}

		// 236B > 236C
		// 入れ込む
		if ((P2.stateno == 621) && (P2.scTime > 5) && (P2.Power >= 1000)) {
			P2.stateno = 800, P2.time = 0, P2.ctrl = 0;
		}

		// 236A > 236C
		// たまにミスる
		if ((P2.stateno == 620) && (P2.scTime > 5) && (P2.Power >= 1000) && (P1.HFlg == 1 || random(80))) {
			P2.stateno = 800, P2.time = 0, P2.ctrl = 0;
		}

		// C > 236A
		if ((P2.stateno == 220) && (P2.scTime > 5)) {
			P2.stateno = 620, P2.time = 0, P2.ctrl = 0;
			// たまに中段
			if (random(100))P2.stateno = 621;
		}
		// B > C
		if ((P2.stateno == 210) && (P2.scTime > 5)) {
			P2.stateno = 220, P2.time = 0, P2.ctrl = 0;
		}

		// 2A > B
		if ((P2.stateno == 300) && (P2.scTime > 5)) {
			P2.stateno = 210, P2.time = 0, P2.ctrl = 0;
		}

		// JC
		if ((P2.stateno == 410) && (P2.scTime > 5)) {
			/*
			if (P1.SFlg == 2) {
				P2.stateno = 420, P2.time = 0, P2.ctrl = 0;
			}
			*/
		}

		// ジャンプキャンセル
		if ((P2.stateno == 205) && (random(200) || P1.HFlg == 1) && (P2.scTime > 5) && (P1.SFlg == 2)) {
			P2.stateno = 40, P2.time = 0, P2.ctrl = 0;
			//P2.button[108]++;
			if (P2.muki == 0)P2.button[106]++;
			else { P2.button[104]++; }
		}
	}
	// エリアル
	if ((!P2.ctrl) && (P2.SFlg == 2) && (P2.CFlg) && (!P1.ctrl) && (P2.StopTime == 0) && (P2.time > 0)) {
		// ジャンプキャンセル
		if ((P2.stateno == 410) && (P2.scTime > 0) && (P2.AirJump > 0) && (P1.HFlg == 1)) {
			P2.stateno = 45, P2.time = 0, P2.ctrl = 0;
			P2.keyAtt[6]++;
		}
		// JC
		if ((P2.stateno == 410) && (P2.scTime > 5) && (P1.HFlg == 1)) {
			P2.stateno = 420, P2.time = 0, P2.ctrl = 0;
		}
	}
	// 対空から追撃
	if ((P2.ctrl) && (P2.stateno == 42) && (P1.stateno == 1030)) {
		// JBを出す
		P2.stateno = 410, P2.time = 0, P2.ctrl = 0;
	}

	}	// シュウイチここまで
}

// 順番を逆にする
void GetP_AI(Player GP1, Player GP2)
{
	P1 = GP1;
	P2 = GP2;
}

void GetS_AI(System_t GS)
{
	S = GS;
}

void GetH_AI(Helper GH1[], Helper GH2[])
{
	int i;

	for (i = 0; i < HM; i++){
		H1[i] = GH1[i];
		H2[i] = GH2[i];
	}

}

/*
boolean random(int num)
{
	boolean f;
	if(GetRand(999) < num)f = true;
	else{ f = false; }

	return f;
}
*/