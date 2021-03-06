//==================================================================================
//
//			数値計算の管理部
//
//========================================================================

//インクルード部--------------------------------------------
#include "pch.h"
#include "Define2.h"

//デファイン部---------------------------------------------

//ﾃﾞｰﾀ定義部------------------------------------------------


// どっちのプレイヤー側か判断
static int PSide;


//内部関数宣言部--------------------------------------------

static void SCancel();
static void ACancel();
static void HCancel();

//プログラム部----------------------------------------------


/****
ステートの数値等を設定する
****/
void HydParam(void)
{

	//ワンモアを切る
	if (P1.More)P1.More = 0;

	// コマンドに数値を渡す
	GetPH_EinCmd(P1);

	// ステートが切り替わったら
	if (P1.stateno != P1.OldState){
		GetP_SameParam(P1, P2);
		GetH_SameParam(H1, H2);
		GetS_SameParam(S);
		ChangeState();
	}

	// 射撃ゲージ回復
	if (P1.Var[10] < 180 * 6)P1.Var[10]++;

	// 1P
	if (P1.StopTime == 0 || P1.time == 0){
		//設定開始
		//--------------------
		// 攻撃判定リセット
		//--------------------
		if (P1.stateno < 200 || P1.stateno > 999){
			DamReset();
		}
		//--------------------
		// 5〜6：振り向き
		// 40〜47：
		// ジャンプ
		// 50〜59：
		// ガード
		//--------------------
		if (((P1.stateno >= 5) && (P1.stateno <= 10)) ||	// 振り向き、しゃがみ移行
			((P1.stateno >= 40) && (P1.stateno <= 47)) ||	// ジャンプ
			((P1.stateno >= 50) && (P1.stateno <= 69)) ||
			(P1.stateno == 520) ||	// ガーキャン
			((P1.stateno >= 550) && (P1.stateno <= 599))	// EXアクション
			){	// ガード・空中ダッシュ・ハイジャンプ　
			GetP_SameParam(P1, P2);
			GetH_SameParam(H1, H2);
			GetS_SameParam(S);
			SameParam();
		}
		//------------
		//
		// 喰らい関連
		//
		//------------
		if ((P1.stateno >= 1000) && (P1.stateno <= 1999)){	// 立ち喰らい
			GetP_SameParam(P1, P2);
			GetH_SameParam(H1, H2);
			GetS_SameParam(S);
			DamParam();
		}

		// チェーン初期化
		if (P1.ctrl){
			P1.Var[1] = 0;
			P1.Var[5] = 0;
		}
		// 空ガフラグ
		else {
			P1.Var[5]++;
			if (P1.Var[5] > 600)P1.Var[5] = 600;
		}

		// 強化中Dチェーン
		if ((P1.ctrl) && (P1.Var[11] > 0))P1.Var[12] = 0;

		// 強化時間減少
		/*
		if (P1.Var[11] > 0) {
			P1.Var[11]--;
			// 終了時エフェクト
			if (P1.Var[11] == 0)
				EffStartB(16, P1.XPos, P1.YPos - (P1.GraphH / 2) * P1.GSize, 0, 0,
					0.23, 0.23, P1.muki);
		}
		*/

		// 初回チャージ
		if (P1.Var[11] == 0) {
			P1.Var[10] = 180 * 6;
			P1.Var[11] = -1;
		}
		// チャージ時間
		//if ((P1.Var[11] <= 0) && (P1.Var[10] > 0))
			//P1.Var[10]--;

		// 次弾設定 3発まで
		P1.Var[2] = 0;
		for (int i = 0; i < 3; i++) {
			if (H1[i].var)
				P1.Var[2]++;
			else { break; }
		}

		switch (P1.stateno){

			//********************
			// 0 立ち
			//********************
		case 0:		// 立ち
			//***************
			//  基本設定
			//***************

			// 立ち設定：操作可否
			P1.SFlg = 0, P1.Lock = 0;
			P1.ctrl = 1;
			P1.YPos = GROUND;
			P1.ignoreG = false;
			if (P1.time == 1){
				VelSet(0, 0);
			}

			// 投げ無敵付与
			/*
			if (P1.Muteki == 10 && P1.time == 0 &&
			P1.mutekiF == 0){
			P1.mutekiF = 2;
			P1.Muteki = 10;
			}
			*/

			// 立ち の 全体フレームを超えたらリセット
			if (P1.time >= P1.animElem[P1.stateno])P1.time = 0;

			//***************
			//  行動内容
			//***************

			// 勝利ポーズへ
			if ((P1.time > 30) && (S.roundState == 3) && (P2.stateno == 1070))
				P1.time = 0, P1.More = 1, P1.stateno = 95;


			break;
			//**** 終了 ****//
			//********************
			// 11 しゃがみ
			//********************
		case 11:
			P1.SFlg = 1, P1.ctrl = 1;	//しゃがみ判定に
			VelSet(0, 0);


			// 投げ無敵付与
			if (P1.Muteki == 1 && P1.time == 0 &&
				P1.mutekiF == 0){
				P1.mutekiF = 2;
				P1.Muteki = 10;
			}
			// 下入力解除で 12 へ
			if (InputPAD(102) == 0){
				P1.time = 0, P1.stateno = 12, P1.More = 1;
			}

			break;
			//********************
			// 12 しゃがみ→立ち
			//********************
		case 12:
			P1.SFlg = 0;	//立ち判定に
			// 最初にロック！
			if (P1.time == 0)P1.Lock = 1;
			// 操作が入ったらモーションキャンセル
			if (P1.Key != 0)P1.time = 0, P1.stateno = 0, P1.SFlg = 0, P1.More = 1, P1.Lock = 0;
			if (P1.time >= P1.animElem[P1.stateno]){
				P1.time = 0, P1.stateno = 0, P1.SFlg = 0, P1.More = 1, P1.Lock = 0;
			}

			break;
		case 20:	// 前歩き
			P1.SFlg = 0, P1.ctrl = 1, P1.Lock = 0;
			P1.XVel = P1.C.walkF; //速度を足す
			// SEを鳴らす
			if ((P1.time == 10) || (P1.time == 35))SEStart(7);
			if (P1.time >= ANIMELEM)P1.time = 0;
			break;
		case 21:	// 後ろ歩き
			P1.SFlg = 0, P1.ctrl = 1, P1.Lock = 0;
			P1.XVel = P1.C.walkB; //速度を足す
			// SEを鳴らす
			if (P1.time == 20)SEStart(7);
			if (P1.time >= P1.animElem[P1.stateno])P1.time = 0;

			break;
		case 30:	// ダッシュ
			P1.SFlg = 0, P1.ctrl = 1, P1.Lock = 0;
			P1.XVel = P1.C.runF[0]; //速度を足す
			if (P1.AnimTime <= 8)P1.XVel = P1.C.runF[0] * (0.1 * (P1.AnimTime + 1)); //速度を足す;

			// SEを鳴らす
			if ((P1.time == 20) || (P1.time == 40))SEStart(4);
			if (P1.time >= P1.animElem[P1.stateno])P1.time = 0;

			break;
		case 35:	// バックステップ
			P1.SFlg = 2, P1.ctrl = 0, P1.Lock = 0;
			P1.ignoreG = true;

			// 無敵
			if (P1.time <= 5){
				P1.Muteki = 1, P1.mutekiF = 0;
			}

			// SEを鳴らす
			if (P1.time == 1)SEStart(5);

			VelAdd(0, P1.C.yAccel);

			//速度を足す
			if (P1.time == 0)VelSet(P1.C.runB[0], P1.C.runB[1]);


			// 終了
			if ((P1.YPos >= GROUND) && (P1.time > 1)){
				P1.stateno = 36;
				P1.time = 0;
				P1.SFlg = 0;
				P1.ctrl = 0;
			}

			break;
		case 36:	// 着地硬直
			P1.SFlg = 0, P1.ctrl = 0;

			// SEを鳴らす
			if (P1.time == 1)SEStart(6);
			VelSet(0, 0);
			P1.YPos = GROUND;

			// 着地エフェクト
			if (P1.time == 1) {
				//エフェクト
				EffStartB(16, P1.XPos, P1.YPos, 0, 0,
					0.16, 0.02, P1.muki);
			}

			// 終了
			if (P1.time >= ANIMELEM + 1){
				P1.stateno = 0;
				P1.time = 0;
				P1.ctrl = 1;
			}

			break;

		case 90:	// 開幕
			P1.SFlg = 0, P1.ctrl = 0;
			/*
			if (P1.time == 7)SEStart(32);
			if (AnimElem(13))SEStart(28);
			*/

			// 音を鳴らす
			if (P1.time == 40){
				SEStart(22);
			}

			// 全体フレームを超えたら立ちへ
			if (P1.time >= P1.animElem[P1.stateno]){
				P1.time = 0;
				P1.stateno = 0;
			}
			break;

		case 95:	// ラウンド取得
			P1.SFlg = 0, P1.ctrl = 0;

			break;
		case 100:	// ダッシュ
			P1.SFlg = 2;
			// 前ジャンプ の 全体フレーム
			if (P1.time > 1)P1.More = 1, P1.stateno = 0;

			break;
			//********************
			// 200：立ちA
			//********************
		case 200:
			P1.ctrl = 0, P1.SFlg = 0;
			// 慣性
			if ((P1.time == 0) && (P1.XVel > 0))VelSet(P1.XVel / 1.5, 0);
			// 先行入力消す
			if (P1.time <= 1)P1.Senkou[1] = 0;
			//[ 通常技の自動振り向き ]
			if (P1.time == 0){
				if ((P1.XPos < P2.XPos) && (P1.muki == 1)){
					P1.muki = 0;
				}
				else if ((P1.XPos > P2.XPos) && (P1.muki == 0)){
					P1.muki = 1;
				}
			}
			// 全体フレーム
			if (P1.time >= ANIMELEM){
				P1.time = 0, P1.stateno = 0, P1.ctrl = 1;
			}
			// SEを鳴らす
			if (P1.time == 1){
				SEStart(1);
			}

			// VO
			if (P1.time == 1){
				PVOStart(P1.PSide, 1, 2);
			}

			// ヒット数セット
			if (P1.time == 3){
				P1.MoveHit = 1;	// １回
			}
			// ダメージセット、持続 2 フレーム？
			if (TimeNo(3, 5)){

				// [ダメージ]
				Damage(42, 0);
				// [ゲージ] 
				Power(24);

				// [ヒットストップ・のけぞり時間]
				HitTime(6, 12, 16, 10);
				// [ノックバック]
				HitVel(-3.2, 0, -1.6, -4.8);

				// [ガード属性]
				P1.GuardF = 1;
				// [喰らい中の浮き]
				P1.fallF = 1;
				// [ヒット時のアニメ]
				P1.HitAnim = 1000;
				// [エフェクト]
				HitEff(1, 0.4, 0.4);
				// [ヒットサウンド]
				SESet(10, 16);

				// [フォール状態]
				P1.fallF = 1;
			}
			else {
				DamReset();
			}

			// ヒット時チェーン
			if ((P1.CFlg) && (P1.time >= 1) && (P1.A.ncTime > 0)){
				// [ジャンプキャンセル]
				if (P1.K_Senkou[8]) {		// 先行入力効かせてみる
					P1.stateno = 40, P1.More = 1,
						P1.time = 0, P1.A.damage = 0;
				}
				// [通常技]
				if (P1.Senkou[3] > 0){
					// 6C
					if ((key(6)) && (!key(2)) && (!key(8))){
						P1.stateno = 225, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
					else if (key(2)){
						P1.stateno = 320, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
					else{
						P1.stateno = 220, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
				}
				else if (P1.Senkou[2] > 0){		// 先行入力効かせてみる
					if (key(2)){
						P1.stateno = 310, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
					else{
						P1.stateno = 210, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
				}
				// 先行入力
				else if ((P1.Senkou[1] > 0) && (P1.A.ncTime > NC_TIME - 16)){
					if (key(2)){
						P1.stateno = 300, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
					else{
						P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
				}
				// [必殺技・ゲージ技]
				SCancel();

			}
			// キャンセル、連打キャンセル
			if ((P1.button[1] == 1) && (P1.time >= 9) && (P1.stateno == 200)){
				if (!key(2)){
					P1.More = 1, P1.time = 0, P1.A.damage = 0;
				}
			}

			// 投げ
			DelayThrow(1);
			// 3ゲージ
			Delay3Button(850, 3000);

			break;
			//********************
			// 210：立ちB
			//********************
		case 210:	//立ちB
			P1.ctrl = 0, P1.SFlg = 0;
			// 慣性
			if ((P1.time == 0) && (P1.XVel > 0))VelSet(P1.XVel / 1.5, 0);
			// SE
			if (P1.time == 1){
				SEStart(1);
			}
			// VO
			if (P1.time == 1){
				PVOStart(P1.PSide, 2, 1);
			}

			// ヒット数セット、８フレ
			if (P1.time == 6){
				P1.MoveHit = 1;	// １回
			}
			// ダメージセット
			if ((P1.time >= 6) && (P1.time <= 10)){

				// [ダメージ]
				Damage(76, 0);

				// [ゲージ] 
				Power(88);
				HitTime(8, 18, 20, 16);

				// [ノックバック]
				HitVel(-2.8, 0, -1.7, -4.0);
				P1.GuardF = 1;
				// [喰らい中の浮き]
				P1.fallF = 1;
				P1.HitAnim = 1000;	// 上段喰らい
				P1.HitSE = 11;
			}
			else {
				DamReset();
			}
			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			// キャンセル
			// 応急処置
			if (P1.StopTime == 0){
				if ((P1.CFlg) && (P1.time >= 1) && (P1.A.ncTime > 2)){

					// [通常技]
					if (P1.Senkou[3] > 0){
						// 6C
						if ((key(6)) && (!key(2)) && (!key(8))){
							P1.stateno = 225, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
						}
						// 下要素を確認
						else if (key(2)){
							P1.stateno = 320, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
						}
						else{
							P1.stateno = 220, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
						}
					}
					else if (P1.Senkou[2] > 0){
						// 下要素を確認
						if (key(2) && P1.Var[1] == 0){
							P1.stateno = 310, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
							P1.Var[1] ++;
						}
					}
					// [必殺技・ゲージ技]
					SCancel();
				}
			}

			// 投げ
			DelayThrow(2);
			// 3ゲージ
			Delay3Button(850, 3000);

			break;

			//********************
			// 220：立ちC
			//********************
		case 220:
			P1.ctrl = 0, P1.SFlg = 0;
			// 慣性
			if ((P1.time == 0) && (P1.XVel > 0))VelSet(P1.XVel / 1.5, 0);
			
			// SEを鳴らす
			if (P1.time == 1)SEStart(3);

			// VO
			if (P1.time == 1){
				PVOStart(P1.PSide, 3, 0);
			}

			// 位置をずらす
			if (P1.time == 0){
				PosAdd(8, 0);
			}

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;


			// ヒット数セット 発生：26
			if (P1.time == 9){
				P1.MoveHit = 1;	// １回
			}
			//else{ P1.MoveHit = 0; }

			// ダメージセット
			if ((P1.time >= 9) && (P1.time <= 18)){

				// [ダメージ]
				Damage(98, 0);
				// [ゲージ] 
				Power(120);
				// [ヒットストップ・のけぞり時間]
				HitTime(10, 22, 24, 18);
				// [ノックバック]
				HitVel(-4.0, 0, -2.0, -4);
				// [ガード属性]
				if (P1.Var[5] >= 14)P1.GuardF = 1;	// 空ガ可
				else { P1.GuardF = 4; }	// 空ガ不可


				// [喰らい中の浮き]
				P1.fallF = 1;
				P1.HitAnim = 1020;	// 下段喰らい


				P1.HitSE = 12;
			}
			else {
				DamReset();
			}

			// キャンセル
			// 応急処置
			if (P1.StopTime == 0){
				if ((P1.CFlg) && (P1.time >= 1) && (P1.A.ncTime > 0)){
					// [ジャンプキャンセル]
					if ((P1.K_Senkou[8]) && (P2.HFlg == 1)) {		// 先行入力効かせてみる
						P1.stateno = 40, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
					// [必殺技・ゲージ技]
					SCancel();
				}

			}

			// 3ゲージ
			Delay3Button(850, 3000);
			
			break;
			//********************
			// 225：6C
			//********************
		case 225:
			P1.ctrl = 0, P1.SFlg = 0;
			// 慣性
			if ((P1.time == 0) && (P1.XVel > 0))VelSet(P1.XVel / 1.5, 0);

			// 位置をずらす
			if (P1.time == 0){
				PosAdd(5, 0);
			}
			// SEを鳴らす
			if (P1.time == 1)SEStart(3);
			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			// VO
			if (P1.time == 1){
				PVOStart(P1.PSide, 3, 0);
			}

			// ヒット数セット 発生：26
			if (P1.time == 1){
				P1.MoveHit = 1;	// １回
			}
			// ダメージセット
			if ((P1.time >= 1)){

				// [ダメージ]
				Damage(106, 0);
				// [ゲージ] 
				Power(128);
				// [ヒットストップ・のけぞり時間]
				HitTime(8, 22, 24, 18);
				// [ノックバック]
				HitVel(-5.0, 0, -2.8, -4);
				// [ガード属性]
				P1.GuardF = 2;

				// [喰らい中の浮き]
				P1.fallF = 1;
				P1.HitAnim = 1020;	// 下段喰らい
				P1.A.forceKurai = true;	// 屈喰らいに

				P1.HitSE = 12;
			}
			else {
				DamReset();
			}
			// [ガード距離]
			P1.GuardDist = 300;

			// キャンセル
			// 応急処置
			if (P1.StopTime == 0){
				if ((P1.CFlg) && (P1.time >= 27)){

					// [必殺技・ゲージ技]
					SCancel();

				}
			}
			break;
			//********************
			// 300：しゃがみA
			//********************
		case 300:
			P1.ctrl = 0, P1.SFlg = 1;
			// 慣性
			if ((P1.time == 0) && (P1.XVel > 0))VelSet(P1.XVel / 1.5, 0);
			// 先行入力消す
			if (P1.time <= 1)P1.Senkou[1] = 0;
			//[ 通常技の自動振り向き ]
			if (P1.time == 0){
				if ((P1.XPos < P2.XPos) && (P1.muki == 1)){
					P1.muki = 0;
				}
				else if ((P1.XPos > P2.XPos) && (P1.muki == 0)){
					P1.muki = 1;
				}
			}
			// SEを鳴らす
			if (P1.time == 1)SEStart(1);
			//  全体フレーム、14を超えたらリセット
			if (P1.time >= ANIMELEM)P1.ctrl = 1, P1.More = 1, P1.stateno = 11, P1.time = 0;

			// VO
			if (P1.time == 1){
				PVOStart(P1.PSide, 1, 2);
			}

			// ヒット数セット、発生６フレ
			if (P1.time == 1){
				P1.MoveHit = 1;	// １回
			}
			// ダメージセット
			if (P1.time >= 1){

				// [ダメージ]
				Damage(40, 0);

				// [ゲージ] 
				Power(20);

				// [ヒットストップ・のけぞり時間]
				HitTime(6, 12, 16, 10);

				// [ノックバック]
				HitVel(-3.1, 0, -1.6, -4.8);
				P1.GuardF = 1;
				// [喰らい中の浮き]
				P1.fallF = 1;
				P1.HitAnim = 1010;	// 下段喰らい
				// [エフェクト]
				HitEff(1, 0.4, 0.4);
				P1.HitSE = 10;
			}
			else {
				DamReset();
			}
			// キャンセル
			if ((P1.CFlg) && (P1.time >= 1) && (P1.A.ncTime > 0)){
				if (P1.Senkou[3] > 0){
					// 6C
					if ((key(6)) && (!key(2)) && (!key(8))){
						P1.stateno = 225, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
					// 下要素を確認
					else if (key(2)){
						P1.stateno = 320, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
					else{
						P1.stateno = 220, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
				}
				else if (P1.Senkou[2] > 0){		// 先行入力効かせてみる
					if (key(2)){
						P1.stateno = 310, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
					else{
						P1.stateno = 210, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
				}
				else if ((P1.Senkou[1] > 0) && (P1.A.ncTime > NC_TIME - 16)){		// 先行入力効かせてみる
					if (key(2)){
						P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
					else{
						P1.stateno = 200, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
				}
				// [必殺技・ゲージ技]
				SCancel();
			}// キャンセル、連打キャンセル
			if ((P1.button[1] == 1) && (P1.time >= 9) && (P1.stateno == 300)){
				// 下要素が入ってる
				if (P1.keyAtt[2]){
					P1.More = 1, P1.time = 0, P1.A.damage = 0;
				}
			}

			// 投げ
			DelayThrow(1);
			// 3ゲージ
			Delay3Button(850, 3000);

			break;
			//********************
			// 310：しゃがみB
			//********************
		case 310:
			P1.ctrl = 0, P1.SFlg = 1;
			// 慣性
			if ((P1.time == 0) && (P1.XVel > 0))VelSet(P1.XVel / 1.5, 0);
			// SE
			if (P1.time == 1)SEStart(1);
			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.ctrl = 1, P1.More = 1, P1.stateno = 11, P1.time = 0;

			// VO
			if (P1.time == 1){
				PVOStart(P1.PSide, 2, 1);
			}

			// ヒット数セット
			if (P1.time == 1){
				P1.MoveHit = 1;	// １回
			}


			// ダメージセット
			if (P1.time >= 1){

				// [ダメージ]
				Damage(75, 0);

				// [ゲージ] 
				Power(86);

				HitTime(8, 18, 20, 16);

				// [ノックバック]
				HitVel(-3.6, 0, -1.5, -4.6);
				P1.GuardF = 1;
				// [喰らい中の浮き]
				P1.fallF = 1;
				P1.HitAnim = 1010;	// 下段喰らい
				P1.HitSE = 11;
			}
			else {
				DamReset();
			}

			// キャンセル
			// 応急処置
			if (P1.StopTime == 0){
				if ((P1.CFlg) && (P1.time >= 1) && (P1.A.ncTime > 2)){
					// [通常技]
					if (P1.Senkou[3] > 0){		// 先行入力効かせてみる
						// 6C
						if ((key(6)) && (!key(2)) && (!key(8))){
							P1.stateno = 225, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
						}
						// 下要素を確認
						else if (key(2)){
							P1.stateno = 320, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
						}
						else{
							P1.stateno = 220, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
						}
					}
					else if (P1.Senkou[2] > 0){		// 先行入力効かせてみる						
						// 下要素を確認
						if ((!key(2)) && (P1.Var[1] == 0)){
							P1.stateno = 210, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
							P1.Var[1]++;
						}
					}

					// [必殺技・ゲージ技]
					SCancel();
				}
			}

			// 投げ
			DelayThrow(2);
			// 3ゲージ
			Delay3Button(850, 3000);

			break;
			//********************
			// 320：しゃがみC
			//********************
		case 320:
			P1.ctrl = 0, P1.SFlg = 0;
			// 慣性
			if ((P1.time == 0) && (P1.XVel > 0))VelSet(P1.XVel / 1.5, 0);
			// SEを鳴らす
			if (P1.time == 1)SEStart(2);
			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.ctrl = 1, P1.More = 1, P1.stateno = 11, P1.time = 0;

			// 前進
			if (P1.time == 3)PosAdd(18.0, 0);

			// ヒット数セット 10フレ
			if (P1.time == 1){
				P1.MoveHit = 1;	// １回
			}
			// ダメージセット
			if (P1.time >= 1){

				// [ダメージ]
				Damage(98, 0);
				// [ゲージ] 
				Power(118);
				HitTime(10, 40, 40, 17);
				// [ノックバック]
				HitVel(-1.8, -4, -2.2, -3.5);
				GuardVel(-4.2, -1.5);

				P1.GuardF = 3;
				// [喰らい中の浮き]
				P1.fallF = 1;
				P1.HitAnim = 1030;	// 空中喰らい(確定ダウン)
				P1.HitSE = 12;
			}
			else{ DamReset(); }
			// 応急処置
			if (P1.StopTime == 0){
				// キャンセル
				if ((P1.CFlg) && (P1.time >= 1) && (P1.A.ncTime > 0)){
					// [必殺技・ゲージ技]
					SCancel();
				}
			}

			// 3ゲージ
			Delay3Button(850, 3000);

			break;
			//********************
			// 400：ジャンプA
			//********************
		case 400:
			P1.ctrl = 0, P1.SFlg = 2;
			if (P1.time == 1)SEStart(1);

			// VO
			if (P1.time == 1){
				PVOStart(P1.PSide, 1, 2);
			}
			// 先行入力消す
			if (P1.time <= 1)P1.Senkou[1] = 0;
			// ヒット数セット 6フレ
			if (P1.time == 4)P1.MoveHit = 1;	// １回
			// ダメージセット
			if ((P1.time >= 4) && (P1.time <= 10)){

				// [ダメージ]
				Damage(40, 0);
				// [ゲージ] 
				Power(30, 15);

				// [ヒットストップ・のけぞり時間]
				HitTime(5, 14, 18, 12);
				// [ノックバック]
				HitVel(-4.6, 0, -1.4, -4.2);
				// ガード判定
				if (P1.YVel >= 0.0)P1.GuardF = 2;
				else { P1.GuardF = 1; }
				// [喰らい中の浮き]
				P1.fallF = 1;
				P1.HitAnim = 1000;
				// [エフェクト]
				HitEff(1, 0.4, 0.4);
				P1.HitSE = 10;
			}
			else {
				DamReset();
			}
			// 操作可能フレーム
			//if (P1.time >= 19)P1.ctrl = 1;
			// キャンセル
			if ((P1.CFlg) && (P1.time >= 1) && (P1.scTime > 0)){
				if (P1.Senkou[3] > 0){		// 先行入力効かせてみる
					P1.stateno = 420, P1.More = 1,
						P1.time = 0, P1.A.damage = 0;
				}
				else if (P1.Senkou[2] > 0){		// 先行入力効かせてみる
					P1.stateno = 410, P1.More = 1,
						P1.time = 0, P1.A.damage = 0;
				}
				else if (P1.Senkou[1] > 0) {		// 先行入力効かせてみる
					P1.stateno = 400, P1.More = 1,
						P1.time = 0, P1.A.damage = 0;
				}
				// [空中必殺技]
				ACancel();

			}
			/*
			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM){
				P1.ctrl = 1, P1.More = 1,
					P1.stateno = 46, P1.time = 0;
			}
			*/
			// 投げ
			DelayThrow(1);

			break;
			//********************
			// 410：ジャンプB
			//********************
		case 410:
			P1.ctrl = 0, P1.SFlg = 2;
			// SE
			if (P1.time == 1)SEStart(1);

			// VO
			if (P1.time == 1){
				PVOStart(P1.PSide, 2, 1);
			}

			// ヒット数セット
			if (P1.time == 2)P1.MoveHit = 1;	// １回
			// ダメージセット
			if (P1.time >= 2){

				// [ダメージ]
				Damage(70, 0);

				// [ゲージ] 
				Power(80);
				// [ヒットストップ・のけぞり時間]
				HitTime(8, 16, 18, 14);
				// [ノックバック]
				HitVel(-4.6, 0, -1.6, -4.6);
				// ガード判定
				if (P1.YVel >= 0.0)P1.GuardF = 2;
				else { P1.GuardF = 1; }
				// [喰らい中の浮き]
				P1.fallF = 1;
				P1.HitAnim = 1000;
				P1.HitSE = 11;
			}
			else {
				DamReset();
			}
			/*
			// 操作可能フレーム
			if (P1.time >= 24){
			P1.ctrl = 1;
			}
			*/
			// キャンセル
			// 応急処置
			if (P1.StopTime == 0){
				if ((P1.CFlg) && (P1.time >= 1) && (P1.scTime > 0)){
					if (P1.Senkou[3] > 0){		// 先行入力効かせてみる
						P1.stateno = 420, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}

					// [空中必殺技]
					ACancel();
				}

				// [ジャンプキャンセル]
				if ((P1.CFlg) && (P1.time >= 6) && (P1.AirJump > 0)){
					if (P1.K_Senkou[8] >= 1){		// 先行入力効かせてみる
						P1.AirJump -= 1;
						P1.stateno = 45, P1.More = 1,
							P1.time = 0, P1.A.damage = 0;
					}
				}
			}

			// 投げ
			DelayThrow(2);

			break;
			//********************
			// 420：ジャンプC
			//********************
		case 420:
			P1.ctrl = 0, P1.SFlg = 2;
			// SE
			if (P1.time == 1)SEStart(2);

			// VO
			if (P1.time == 1){
				PVOStart(P1.PSide, 3, 0);
			}

			// ヒット数セット
			if (P1.time == 7)P1.MoveHit = 1;	// １回
			// ダメージセット、持続 5フレ
			if ((P1.time >= 7) && (P1.time <= 11)){

				// [ダメージ]
				Damage(95, 0);

				// [ゲージ] 
				Power(100);

				// [ヒットストップ・のけぞり時間]
				HitTime(8, 18, 40, 16);

				// [ノックバック]
				if (P1.HitCount > 10)
					HitVel(-4.8, 0, -3.8, 4.5);
				else { HitVel(-4.8, 0, -3.8, 6.5); }

				// ガード判定
				if (P1.YVel >= 0.0)P1.GuardF = 2;
				else { P1.GuardF = 1; }
				// [喰らい中の浮き]
				P1.fallF = 1;
				P1.HitAnim = 1000;
				P1.HitSE = 12;
				P1.A.boundLevel = 1;
			}
			else {
				//DamReset();
			}
			// [キャンセル]
			if ((P1.CFlg) && (P1.time >= 9)){
				// [空中必殺技]
				ACancel();
			}

			break;
			//********************
			// 500：地上投げ
			//********************
		case 500:
			P1.ctrl = 0, P1.SFlg = 0;
			// 投げ方向設定
			if (P1.time == 1){
				P1.throwSide = P1.muki;
				P1.A.throwTurn = false;
				if (P1.keyAtt[4]){
					P1.A.throwTurn = true;
				}
				P1.XVel = 0;
			}

			if ((P1.time == 9) && (P1.StopTime == 0))PVOStart(P1.PSide, 11, 0);

			// SE
			if (P1.time == 1)SEStart(2);
			// ヒット数セット、
			// ガード時は無効
			if ((P1.time >= 5 && P1.time <= 7) && (P2.stateno <= 999) &&
				(P1.XPos - P2.XPos < 120 && P2.XPos - P1.XPos < 120))	// 喰らい・空中状態じゃない
			{
				P1.MoveHit = 1;	// １回
				// どっちも投げタイミングが同じなら、2P負ける
				//if ((P1.stateno == P2.stateno) && (P1.time == P2.time) && (P1.PSide == 2))
				//	P1.MoveHit = 0;
			}


			// ダメージセット、持続 1フレ
			if (P1.time == 5){

				// [ダメージ]
				Damage(0, 0);
				// [ゲージ] 
				Power(0, 0);
				HitTime(0, 3, 3, 0);

				P1.GX = 0, P1.GY = 0;
				P1.GuardF = 0;
				P1.attF = 2;

				P1.HitAnim = 1060;
				P1.HitSE = 9;
				P1.A.hitEff = 2;
			}

			// [ガード距離]
			P1.GuardDist = 0;


			// 当たったら変更
			if ((P1.CFlg) && (P1.time == 8)){
				P1.stateno = 505;
				P1.time = 0, P2.time = 0;
				P1.More = 1;
				P1.A.damage = 0;

				P1.StopTime = 6;

				// 投げの向きを変更
				if (P1.A.throwTurn){
					if (P1.muki == 0){
						P1.muki = 1, P2.muki = 0;
					}
					else if (P1.muki == 1){
						P1.muki = 0, P2.muki = 1;
					}
				}
			}
			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			// 3ゲージ
			Delay3Button(850, 3000);

			break;
			//********************
			// 505：投げ（演出）
			//********************
		case 505:
			P1.ctrl = 0, P1.SFlg = 0;
			P1.Muteki = 1, P1.mutekiF = 0;


			P1.XVel = 0;

			// 距離をとる
			if (P1.time == 0){

				int dist = 70;
				if (P1.muki == 0){
					P2.XPos = P1.XPos + dist;
					// もし距離が離れなかったら
					if (P2.XPos > STAGE_WIDTH - GAMENHAJI){
						P2.XPos = STAGE_WIDTH - GAMENHAJI;
						P1.XPos = P2.XPos - dist;
					}
				}
				else{
					P2.XPos = P1.XPos - dist;
					if (P2.XPos < GAMENHAJI){
						P2.XPos = GAMENHAJI;
						P1.XPos = P2.XPos + dist;
					}
				}

				// 近いなら座標ずらす
				if (P2.XPos < GAMENHAJI){
					P2.XPos = GAMENHAJI;
					P1.XPos = P2.XPos + 80;
				}
				if (P2.XPos > STAGE_WIDTH - GAMENHAJI){
					P2.XPos = STAGE_WIDTH - GAMENHAJI;
					P1.XPos = P2.XPos - 80;
				}

				P2.YVel = 0;
				P2.YPos = GROUND;
			}


			// 距離を離す
			//if (P1.time == 3)P2.XVel = -2.5;
			//if (P1.time == 4)P2.XVel = -2.2;
			// ヒット音
			//if(P1.time == 7)SEStart(11);

			// 2P無敵
			if (P1.time < 6) {
				P2.Muteki = 1, P2.mutekiF = 0;
			}
			if (P1.time > 8 && P1.time < 20)P2.Muteki = 1, P2.mutekiF = 0;
			if (P1.time > 22 && P1.time < 44)P2.Muteki = 1, P2.mutekiF = 0;

			// ヒット数セット
			if (P1.time == 7 || P1.time == 21 || P1.time == 45)P1.MoveHit = 1;	// １回

			// ダメージセット、持続 1フレ
			if (P1.time == 7 || P1.time == 21) {

				// [攻撃判定セット]
				//ExAtt(P1.PSide, 0, 100, 90, 20, -100);

				// [ダメージ]
				Damage(0, 30);
				// [ゲージ] 
				Power(35);
				HitTime(0, 45, 45, 0);
				// [ノックバック]
				HitVel(0, 0, 0, 0);
				P1.HitAnim = 1000;
				// [喰らい中の浮き]
				P1.fallF = 0;
				P1.HitSE = 11;
				P1.A.kill = 0;
			}
			// ダメージセット、持続 1フレ
			else if (P1.time == 45){

				// [攻撃判定セット]
				ExAtt(P1.PSide, 0, 100, 90, 20, -100);

				// [ダメージ]
				Damage(0, 100);
				P1.A.hosei_K = 0.5;
				// [ゲージ] 
				Power(200);
				HitTime(6, 45, 45, 0);
				// [ノックバック]
				HitVel(-2, -3.4, 0, 0);
				P1.HitAnim = 1030;
				// [喰らい中の浮き]
				P1.fallF = 1;
				P1.HitSE = 12;
				P1.A.boundLevel = 1;
				P1.A.kill = 1;
			}
			else {
				DamReset();
			}
			// 応急処置
			if (P1.StopTime == 0){
				// キャンセル
				if ((P1.CFlg) && (P1.time >= 46) && (P1.scTime > 0)){
					// [必殺技・ゲージ技]
					HCancel();
				}
			}

			// 投げ抜け
			ThrowReject();

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;
			break;
			//********************
			// 510 空中投げ
			//********************
		case 510:	// 空中投げ
			P1.ctrl = 0, P1.SFlg = 2;

			// 投げ方向設定
			if (P1.time == 1){
				P1.throwSide = P1.muki;
				P1.A.throwTurn = false;
				if (P1.muki == 0){
					if (InputPAD(104) >= 1){
						P1.A.throwTurn = true;
					}
				}
				if (P1.muki == 1){
					if (InputPAD(106) >= 1){
						P1.A.throwTurn = true;
					}
				}
			}
			// SEを鳴らす
			if (P1.time == 1)SEStart(1);
			// ヒット準備
			// ヒット数セット、
			if (P1.time == 4)
			{
				P1.MoveHit = 1;	// １回
			}

			if ((P1.time == 9) && (P1.StopTime == 0))PVOStart(P1.PSide, 11, 0);

			// ダメージセット、持続 1フレ
			if (P1.time == 4){

				// [ダメージ]
				Damage(0, 0);
				// [ゲージ] 
				Power(0, 0);

				HitTime(4, 16, 20, 0);
				// [ノックバック]
				HitVel(0, 0, 0, -5);
				P1.GuardF = 0;
				P1.attF = 2;
				// [ガード距離]
				P1.GuardDist = 0;

				P1.HitAnim = 1065;
				P1.HitSE = 9;
				P1.A.hitEff = 2;
			}
			// 当たったら変更
			if ((P1.CFlg) && (P1.time >= 5)){
				P1.stateno = 515;
				P1.time = 0, P2.time = 0;
				P1.More = 1;
				P1.A.damage = 0;
				// 投げの向きを変更
				if (P1.A.throwTurn){
					if (P1.muki == 0){
						P1.muki = 1, P2.muki = 0;
					}
					else if (P1.muki == 1){
						P1.muki = 0, P2.muki = 1;
					}
				}
			}
			break;

			//********************
			// 515 空投げ演出
			//********************
		case 515:
			P1.ctrl = 0, P1.SFlg = 2;
			P1.Muteki = 1, P1.mutekiF = 0;
			P1.XVel = 0;
			P1.ignoreG = true;

			// バグ防止の浮き
			if ((P1.time == 0) && (P1.YPos + 20 >= GROUND)){
				P1.YPos = GROUND - 20;
			}

			// 距離をとる
			if (P1.time == 0){
				P1.YPos -= 2;
				P2.YPos = P1.YPos - 1;

				int dist = P2.xSize + 5;
				if (P1.muki == 0){
					P2.XPos = P1.XPos + dist;
					// もし距離が離れなかったら
					if (P2.XPos > STAGE_WIDTH - GAMENHAJI){
						P2.XPos = STAGE_WIDTH - GAMENHAJI;
						P1.XPos = P2.XPos - dist;
					}
				}
				else{
					P2.XPos = P1.XPos - dist;
					if (P2.XPos < GAMENHAJI){
						P2.XPos = GAMENHAJI;
						P1.XPos = P2.XPos + dist;
					}
				}

				P1.YVel = 0;
				P2.YVel = 0;
			}
			// 重力加算
			if (AnimElem(5)){
				P1.YVel = -5;
			}
			// 重力
			else if (P1.State2 >= 4){
				P1.YVel += 0.5;
			}

			// ついでに無敵時間
			if (P1.State2 < 3){
				P2.Muteki = 1, P2.mutekiF = 0;
				P2.time = 0;
				//P1.YVel += GRAVITY;
				//P2.YVel += GRAVITY;
			}
			// SEを鳴らす
			if (P1.time == 15)SEStart(10);

			// VO
			if ((P1.time == 9) && (P1.StopTime == 0)){
				PVOStart(P1.PSide, 10, 0);
			}

			// ヒット数セット
			if (P1.time == 1)P1.MoveHit = 1;	// １回
			// ダメージセット、持続 1フレ
			if (P1.time >= 1){

				// 床バウンド
				//P1.A.boundLevel = 1;
				// [攻撃判定セット]
				if (P1.time == 17)
					ExAtt(P1.PSide, 0, 100, 90, 20, -100);

				// [ダメージ]
				Damage(180, 0);
				// [ゲージ] 
				Power(300);

				HitTime(0, 100, 100, 0);
				// [ノックバック]
				HitVel(-4.8, 3.8, -4.8, 3.8);
				P1.HitAnim = 1030;
				// [喰らい中の浮き]
				P1.fallF = 1;
				P1.HitSE = 11;

			}
			else{
				DamReset();
			}
			// 相手の姿勢を後半のに
			if (P1.CFlg && P2.stateno == 1030 && P2.time < 20)P2.time = 20;

			// 投げ抜け
			ThrowReject();

			// 着地
			if (P1.YPos + P1.YVel * 2 >= GROUND){
				P1.time = 0, P1.stateno = 47,
					P1.ctrl = 0, P1.SFlg = 0;
				P1.YPos = GROUND;
				P1.ignoreG = false;
			}
			break;
			//********************
			// 600 ５Ｄ
			//********************
		case 600:
			P1.ctrl = 0, P1.SFlg = 0;
			// SEを鳴らす
			if (P1.time == 1){
				SEStart(2);
			}

			if ((P1.time == 1) && (P1.StopTime == 0))PVOStart(P1.PSide, 15, 0);

			// ヘルパー設定準備.[1]	飛び道具
			if (P1.time == 0){
				P1.Var[3] = P1.Var[2];	// 撃った弾を記憶
				HelperReset(P1.Var[3]);
				H1[P1.Var[3]].var = false;
			}
			// 呼び出し.[1]	飛び道具
			else if (P1.time == 11){
				H1[P1.Var[3]].var = true;
				H1[P1.Var[3]].time = 0;
				H1[P1.Var[3]].stateno = 2000;
				H_PosSet(P1.Var[3], 20, -168);
				P1.Var[10] -= 180;
			}

			if (P1.time == 11) {
				VelSet(-1.4, 0);
				SEStart(40);
			}

			// キャンセル
			if (P1.StopTime == 0){
				//&& (H1[P1.Var[3]].var == false) &&(P1.scTime > 0)
				if (P1.time > 13){
					// Dキャンセル
					if ((P1.Var[11] > 0) && (P1.Senkou[4] > 0) && (P1.Var[12] == 0)) {
						// 地上にいて、ヘルパー[1]が存在しない
						if (P1.SFlg != 2) {
							P1.Var[12]++;
							if ((P1.keyAtt[6]) && (P1.keyAtt[2]))
								P1.stateno = 603, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
							else if (P1.keyAtt[6])
								P1.stateno = 601, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
							else if (P1.keyAtt[2])
								P1.stateno = 602, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
						}
					}
					// [ゲージ技]
					HCancel();
				}
			}

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			break;

			//********************
			// 601 6D
			//********************
		case 601:
			P1.ctrl = 0, P1.SFlg = 0;
			// SEを鳴らす
			if (P1.time == 1) {
				SEStart(2);
			}

			if ((P1.time == 1) && (P1.StopTime == 0))PVOStart(P1.PSide, 15, 0);

			// ヘルパー設定準備.[1]	飛び道具
			if (P1.time == 0) {
				P1.Var[3] = P1.Var[2];	// 撃った弾を記憶
				HelperReset(P1.Var[3]);
				H1[P1.Var[3]].var = false;

			}
			// 呼び出し.[1]	飛び道具
			else if (P1.time == 11) {
				H1[P1.Var[3]].var = true;
				H1[P1.Var[3]].time = 0;
				H1[P1.Var[3]].stateno = 2001;
				P1.Var[10] -= 180;
			}

			if (P1.time == 11) {
				SEStart(40);
				VelSet(-2.4, 0);
			}

			// キャンセル
			if (P1.StopTime == 0) {
				if (P1.time > 13) {
					// Dキャンセル
					if ((P1.Var[11] > 0) && (P1.Senkou[4] > 0) && (P1.Var[12] == 0)) {
						// 地上にいて、ヘルパー[1]が存在しない
						if (P1.SFlg != 2) {
							P1.Var[12]++;
							if ((P1.keyAtt[6]) && (P1.keyAtt[2]))
								P1.stateno = 603, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
							else if (P1.keyAtt[2])
								P1.stateno = 602, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
							else if(!P1.keyAtt[8]){
								P1.stateno = 600, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
							}
						}
					}
					// [ゲージ技]
					HCancel();
				}
			}

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			break;

			//********************
			// 602 2D
			//********************
		case 602:
			P1.ctrl = 0, P1.SFlg = 1;
			// SEを鳴らす
			if (P1.time == 1) {
				SEStart(2);
			}

			if ((P1.time == 1) && (P1.StopTime == 0))PVOStart(P1.PSide, 15, 0);

			// ヘルパー設定準備.[1]	飛び道具
			if (P1.time == 0) {
				P1.Var[3] = P1.Var[2];	// 撃った弾を記憶
				HelperReset(P1.Var[3]);
				H1[P1.Var[3]].var = false;
			}
			// 呼び出し.[1]	飛び道具
			else if (P1.time == 11) {
				H1[P1.Var[3]].var = true;
				H1[P1.Var[3]].time = 0;
				H1[P1.Var[3]].stateno = 2000; 
				H_PosSet(P1.Var[3], 22, -138);
				P1.Var[10] -= 180;
			}

			if (P1.time == 11) {
				VelSet(-1.4, 0);
				SEStart(40);
			}

			// ヒット時キャンセル
			if (P1.StopTime == 0) {
				if (P1.time > 13) {
					// Dキャンセル
					if ((P1.Var[11] > 0) && (P1.Senkou[4] > 0) && (P1.Var[12] == 0)) {
						// 地上にいて、ヘルパー[1]が存在しない
						if (P1.SFlg != 2) {
							P1.Var[12]++;
							if ((P1.keyAtt[6]) && (P1.keyAtt[2]))
								P1.stateno = 603, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
							else if (P1.keyAtt[6])
								P1.stateno = 601, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
							else if (!P1.keyAtt[8]) {
								P1.stateno = 600, P1.More = 1,
									P1.time = 0, P1.A.damage = 0;
							}
						}
					}
					// [ゲージ技]
					HCancel();
				}
			}

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 11, P1.ctrl = 1;

			break;

			//********************
			// 603 3D
			//********************
		case 603:
			P1.ctrl = 0, P1.SFlg = 0;
			// SEを鳴らす
			if (P1.time == 1) {
				SEStart(2);
			}

			if ((P1.time == 1) && (P1.StopTime == 0))PVOStart(P1.PSide, 15, 0);

			// ヘルパー設定準備.[1]	飛び道具
			if (P1.time == 0) {
				P1.Var[3] = P1.Var[2];	// 撃った弾を記憶
				HelperReset(P1.Var[3]);
				H1[P1.Var[3]].var = false;

			}
			// 呼び出し.[1]	飛び道具
			else if (P1.time == 11) {
				H1[P1.Var[3]].var = true;
				H1[P1.Var[3]].time = 0;
				H1[P1.Var[3]].stateno = 2003;
				P1.Var[10] -= 180;
			}

			if (P1.time == 11) {
				VelSet(-1.8, 0);
				SEStart(40);
			}

			// ヒット時キャンセル
			if (P1.StopTime == 0) {
				if (P1.time > 12) {
					// Dキャンセル
					if ((P1.Var[11] > 0) && (P1.Senkou[4] > 0) && (P1.Var[12] == 0)) {
						// 地上にいて、ヘルパー[1]が存在しない
						if (P1.SFlg != 2) {
							P1.Var[12]++;
							if (P1.keyAtt[6])
								P1.stateno = 601, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
							else if (P1.keyAtt[2])
								P1.stateno = 602, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
							else if (!P1.keyAtt[8]) {
								P1.stateno = 600, P1.More = 1,
									P1.time = 0, P1.A.damage = 0;
							}
						}
					}
					// [ゲージ技]
					HCancel();
				}
			}

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			break;


			//********************
			// 605 ＪＤ
			//********************
		case 605:
			P1.ctrl = 0, P1.SFlg = 2;
			P1.ignoreG = true;
			// 速度をつける
			if (P1.time == 13) {
				P1.XVel = -2.0;
				P1.YVel = -3.6;
				SEStart(40);
			}
			// SEを鳴らす
			if (P1.time == 1) {
				SEStart(2);
			}

			if ((P1.time == 1) && (P1.StopTime == 0))PVOStart(P1.PSide, 15, 0);


			// ヘルパー設定準備.[1]	飛び道具
			if (P1.time == 0) {
				P1.Var[3] = P1.Var[2];
				HelperReset(P1.Var[3]);
				H1[P1.Var[3]].var = false;

			}
			// 呼び出し.[1]	飛び道具
			else if (P1.time == 13) {
				H1[P1.Var[3]].var = true;
				H1[P1.Var[3]].time = 0;
				H1[P1.Var[3]].stateno = 2002;
				P1.Var[10] -= 180;
			}

			// 重力緩和
			VelAdd(0, P1.C.yAccel);

			if ((P1.time > 13) && (P1.time < ANIMELEM)) {
				// Dキャンセル
				if ((P1.Var[11] > 0) && (P1.Senkou[4] > 0) && (P1.Var[12] == 0)) {
					// 地上にいて、ヘルパー[1]が存在しない
					if (P1.SFlg == 2) {
						P1.Var[12]++;
						if (P1.keyAtt[6]) {
							P1.stateno = 606, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
						}
						else {
							P1.stateno = 605, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
						}
					}
				}
			}
				

			// 全体フレームを超えたらリセット
			if (P1.YPos + P1.YVel * 2 >= GROUND)P1.time = 0, P1.stateno = 609,
				P1.ctrl = 0, P1.SFlg = 0;

			break;

			//********************
			// 606 J6D
			//********************
		case 606:
			P1.ctrl = 0, P1.SFlg = 2;
			P1.ignoreG = true;
			// 速度をつける
			if (P1.time == 13) {
				P1.XVel = -2.0;
				P1.YVel = -3.6;
				SEStart(40);
			}
			// SEを鳴らす
			if (P1.time == 1) {
				SEStart(2);
			}

			if ((P1.time == 1) && (P1.StopTime == 0))PVOStart(P1.PSide, 15, 0);


			// ヘルパー設定準備.[1]	飛び道具
			if (P1.time == 0) {
				P1.Var[3] = P1.Var[2];
				HelperReset(P1.Var[3]);
				H1[P1.Var[3]].var = false;

			}
			// 呼び出し.[1]	飛び道具
			else if (P1.time == 13) {
				H1[P1.Var[3]].var = true;
				H1[P1.Var[3]].time = 0;
				H1[P1.Var[3]].stateno = 2000;
				H_PosSet(P1.Var[3], 20, -166);
				P1.Var[10] -= 180;
			}

			// 重力緩和
			VelAdd(0, P1.C.yAccel);

			if ((P1.time > 13) && (P1.time < ANIMELEM)) {
				// Dキャンセル
				if ((P1.Var[11] > 0) && (P1.Senkou[4] > 0) && (P1.Var[12] == 0)) {
					// 地上にいて、ヘルパー[1]が存在しない
					if (P1.SFlg == 2) {
						P1.Var[12]++;
						if (P1.keyAtt[6]) {
							P1.stateno = 606, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
						}
						else {
							P1.stateno = 605, P1.More = 1,
								P1.time = 0, P1.A.damage = 0;
						}
					}
				}
			}

			// 全体フレームを超えたらリセット
			if (P1.YPos + P1.YVel * 2 >= GROUND)P1.time = 0, P1.stateno = 609,
				P1.ctrl = 0, P1.SFlg = 0;

			break;


			//********************
			// 609 着地硬直
			//********************
		case 609:
			P1.ctrl = 0, P1.SFlg = 0;
			// 速度をつける
			VelSet(0, 0);
			P1.YPos = GROUND;

			// 着地エフェクト
			if (P1.time == 1) {
				//エフェクト
				EffStartB(16, P1.XPos, P1.YPos, 0, 0,
					0.16, 0.02, P1.muki);
			}
			// SEを鳴らす
			if (P1.time == 1) {
				SEStart(6);
			}

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.stateno = 0, P1.SFlg = 0, P1.ctrl = 1;

			break;

			//********************
			// 610 A強撃
			//********************
		case 610:
			P1.ctrl = 0, P1.SFlg = 0;
			// SEを鳴らす、移動
			if (P1.time == 0) {
				SEStart(3);
				P1.XVel = 0;
			}

			// ヒット数セット
			if (P1.time == 1) {
				P1.MoveHit = 1;	// １回
			}

			if (P1.time == 5) {
				P1.XVel = 5.0;
			}

			// ダメージセット
			if (P1.time >= 1) {

				// [ダメージ]
				Damage(102, 0);
				// [ゲージ] 
				Power(122);
				// [ヒットストップ・のけぞり時間]
				HitTime(10, 22, 26, 18);
				// [ノックバック]
				HitVel(-4.2, 0, -2.2, -3.2);
				P1.HitAnim = 1030;	// 空中喰らい
				// [喰らい中の浮き]
				P1.fallF = 2;
				// [ガード属性]
				P1.GuardF = 1;
				P1.HitSE = 12;
			}
			else {
				DamReset();
			}

			// キャンセル
			if (P1.StopTime == 0) {
				// ヒット時キャンセル
				if ((P1.CFlg) && (P1.time >= 1) && (P1.scTime > 0)) {
					// [ゲージ技]
					HCancel();
				}
			}

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			break;

			//********************
			// 611 B進脚
			//********************
		case 611:
			P1.ctrl = 0, P1.SFlg = 0;
			// SEを鳴らす、移動
			if (P1.time == 0) {
				SEStart(3);
				P1.XVel = 0;
			}

			if (P1.time == 0) {
				EffStart(16, P1.XPos, P1.YPos, -10, -(P1.ySize / 1.5),
					0.04, 0.25, P1.muki);
			}

			// ヒット数セット
			if (P1.time == 1) {
				P1.MoveHit = 1;	// １回
			}

			if (P1.time == 5) {
				P1.XVel = 7.8;
			}

			// ダメージセット
			if (P1.time >= 1) {

				// [ダメージ]
				Damage(114, 0);
				// [ゲージ] 
				Power(136);
				// [ヒットストップ・のけぞり時間]
				HitTime(10, 22, 26, 18);
				// [ノックバック]
				HitVel(-2.4, -3.6, -2.2, -2.8);
				GuardVel(-4.2, -1.8);
				P1.HitAnim = 1030;	// 空中喰らい
				// [喰らい中の浮き]
				P1.fallF = 1;
				// [ガード属性]
				P1.GuardF = 1;
				P1.HitSE = 12;
			}
			else {
				DamReset();
			}

			// キャンセル
			if (P1.StopTime == 0) {
				// ヒット時キャンセル
				if ((P1.CFlg) && (P1.time >= 1) && (P1.scTime > 0)) {
					// [ゲージ技]
					HCancel();
				}
			}

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			break;


			//********************
			// 630 強化
			//********************
		case 630:
			P1.ctrl = 0, P1.SFlg = 0;
			// SEを鳴らす、移動
			if (P1.time == 0) {
				SEStart(22);
				P1.XVel = 0;
			}

			//if (P1.time == 13 - 1)SEStart(9);
			//if (P1.time == 33 - 1)SEStart(9);

			if (AnimElem(11)) {
				//P1.Var[11] = 600;
				// 弾数回復
				P1.Var[10] += 180 * 6;
				if (P1.Var[10] > 180 * 6)P1.Var[10] = 180 * 6;
				// 中段エフェクト
				EffStartB(16, P1.XPos, P1.YPos, 0, -(P1.ySize / 1.5),
					0.20, 0.20, P1.muki);
				SEStart(31);
			}

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			break;

			//********************
			// 700 大ブメ[5]
			//********************
		case 700:
			P1.ctrl = 0, P1.SFlg = 0;
			P1.A.gaugeHosei = true;
			// ゲージ消費
			if (P1.time == 1){
				P1.Power -= 500;
				SEStart(31);
				P1.colorCTime = 20;
				P1.colorC[0] = 255, P1.colorC[1] = 255, P1.colorC[2] = 0;
			}
			// SEを鳴らす
			if (P1.time == 2){
				SEStart(22);
			}

			if ((P1.time == 1) && (P1.StopTime == 0))PVOStart(P1.PSide, 19, 0);

			// ヘルパー呼び出し.[5]
			if (P1.time == 4){
				HelperReset(5);
				H1[5].var = true;
				H1[5].time = 0;
				H1[5].stateno = 2030;
			}
			// 終了・58F
			if (P1.time >= 58)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			break;

			//********************
			// 710 D進脚
			//********************
		case 710:
			P1.ctrl = 0, P1.SFlg = 0;
			P1.A.gaugeHosei = true;
			// ゲージ消費
			if (P1.time == 1) {
				P1.Power -= 500;
				SEStart(31);
				P1.colorCTime = 20;
				P1.colorC[0] = 255, P1.colorC[1] = 255, P1.colorC[2] = 0;
			}

			// SEを鳴らす、移動
			if (P1.time == 0) {
				SEStart(3);
				P1.XVel = 0;
			}

			if (P1.time == 0) {
				EffStart(16, P1.XPos, P1.YPos, -10, -(P1.ySize / 1.5),
					0.04, 0.25, P1.muki);
			}

			// ヒット数セット
			if (P1.time == 1) {
				P1.MoveHit = 1;	// １回
			}

			if (P1.time == 5) {
				P1.XVel = 7.8;
			}

			// ダメージセット
			if (P1.time >= 1) {

				// [ダメージ]
				Damage(111, 5);
				// [ゲージ] 
				Power(0, 68);
				// [ヒットストップ・のけぞり時間]
				HitTime(10, 22, 26, 18);
				// [ノックバック]
				HitVel(-3.4, -3.6, -3.2, -2.8);
				GuardVel(-4.4, -2.8);
				P1.HitAnim = 1030;	// 空中喰らい
				// [喰らい中の浮き]
				P1.fallF = 1;
				// [ガード属性]
				P1.GuardF = 1;
				P1.HitSE = 12;
				P1.A.wallLevel = 2;
				P1.A.wall_xvel = 0.2;
				P1.A.boundLevel = 0;
			}
			else {
				DamReset();
			}

			// キャンセル
			if (P1.StopTime == 0) {
				// ヒット時キャンセル
				if ((P1.CFlg) && (P1.time >= 1) && (P1.scTime > 0)) {
					// [ゲージ技]
					HCancel();
				}
			}

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			break;
			//********************
			// 800 二段切り
			//********************
		case 800:
			P1.ctrl = 0, P1.SFlg = 0;
			P1.A.gaugeHosei = true;
			P1.D.counter = 1;

			// ゲージ消費
			if (P1.time == 0){
				P1.Power -= 1000;
				S.StopTime = 10;
				S.Anten = 25;
				SEStart(25);
				// 中段エフェクト
				EffStartB(17, P1.XPos, P1.YPos - (P1.GraphH / 2) * P1.GSize, 0, 0, 0.25, 0.25, P1.PSide);
			}


			// ヘルパー設定準備.[1]	飛び道具
			if (P1.time == 0) {
				HelperReset(5);
				H1[5].var = false;

			}
			// 呼び出し.[1]	飛び道具
			else if (P1.time == 10) {
				H1[5].var = true;
				H1[5].time = 0;
				H1[5].stateno = 2100;
				SEStart(40);
			}

			if (P1.time == 12) {
				VelSet(-5.0, 0);
			}

			// 全体フレームを超えたらリセット
			if (P1.time >= ANIMELEM)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			break;
			//********************
			// 810 連脚
			//********************
		case 810:
			P1.ctrl = 0, P1.SFlg = 0;
			P1.A.gaugeHosei = true;
			P1.D.counter = 1;
			P1.D.fatal = true;

			// ゲージ消費
			if (P1.time == 0) {
				P1.Power -= 1000;
				S.StopTime = 10;
				S.Anten = 25;
				SEStart(25);
				// 中段エフェクト
				EffStartB(17, P1.XPos, P1.YPos - (P1.GraphH / 2) * P1.GSize, 0, 0, 0.25, 0.25, P1.PSide);
			}

			// 分身
			if (P1.time == 1) {
				afterimageSetting(P1.PSide, 72, 128, 128, 250);
				EffStart(16, P1.XPos, P1.YPos, -10, -(P1.ySize / 1.5),
					0.04, 0.25, P1.muki);
			}

			// SE
			if (P1.time == 1 || P1.time == 23) {
				SEStart(3);
			}
			if (P1.time == 42) {
				SEStart(3);
			}

			// 3456 9
			// 前進
			if (P1.time == 6) {
				P1.XVel = 6.6;
			}

			if (P1.time == 25) {
				P1.XVel = 4.2;
			}

			if (P1.time == 42) {
				P1.XVel = 2.2;
			}

			// ヒット数セット	14フレ〜
			if (AnimElem(4) || AnimElem(12) ||
				AnimElem(18)) {
				P1.MoveHit = 1;	// １回
			}
			// ダメージセット

			if (P1.time <= 1) {
				DamReset();
			}
			if ((P1.time >= 7) && (P1.time < 40)) {

				// [ダメージ]
				Damage(75, 5);
				// [ゲージ] 
				Power(0, 60);
				HitTime(4, 24, 24, 22);
				// [ノックバック]
				HitVel(-2.4, 0, -0.8, -3.0);
				/*
				if (P2.SFlg != 2) {
					P1.HitAnim = 1000;	// 地上
				}
				else if (P2.SFlg == 2) {
					P1.HitAnim = 1060;	// 地上へ
				}
				*/
				P1.HitAnim = 1000;	// 地上へ
				// [喰らい中の浮き]
				P1.fallF = 1;
				// [ガード属性]
				P1.GuardF = 1;
				P1.HitSE = 12;
				// エフェクト
				HitEff(1, 0.6, 0.6);
				P1.A.kill = 0;
			}
			// ダメージセット
			else if (P1.time >= 40) {

				// [ダメージ]
				Damage(40, 75);
				// [ゲージ] 
				Power(0, 115);

				HitTime(10, 100, 100, 10);
				// [ノックバック]
				HitVel(-5.0, -3.6, -4.8, -3.2);

				P1.HitAnim = 1030;	// 空中喰らい
				// [喰らい中の浮き]
				P1.fallF = 1;
				// [ガード属性]
				P1.GuardF = 1;
				P1.HitSE = 14;
				// エフェクト
				HitEff(1, 1.0, 1.0);
				P1.A.kill = 1;
				// 地震エフェクト
				P1.A.quakeTime = 2;
				P1.A.quakeY = 2;
			}

			// 当たったら変更
			if ((P1.CFlg) && (P1.time < 18) && (P2.stateno == 1060) && (P2.SFlg == 0)) {
				P2.YPos = GROUND;

				if (P1.XPos <= GAMENHAJI)P1.XPos = GAMENHAJI + 40;
				if (P1.XPos >= STAGE_WIDTH - GAMENHAJI)P1.XPos = STAGE_WIDTH - GAMENHAJI - 40;
				/*
				if (P1.muki == 0) {
					P2.XPos = P1.XPos + 40;
				}
				else if (P1.muki == 1) {
					P2.XPos = P1.XPos - 40;
				}
				*/
			}

			// 立ち の 全体フレーム、54を超えたらリセット
			if (P1.time >= ANIMELEM) {
				P1.time = 0, P1.stateno = 0, P1.ctrl = 1;
			}

			break;
			//********************
			// 820 真空ブーメラン
			//********************
		case 820:
			P1.ctrl = 0, P1.SFlg = 0;
			P1.A.gaugeHosei = true;
			// ゲージ消費
			if (P1.time == 0){
				P1.Power -= 1000;
				S.StopTime = 10;
				S.Anten = 25;
				SEStart(25);
				// 中段エフェクト
				EffStartB(17, P1.XPos, P1.YPos - (P1.GraphH / 2) * P1.GSize, 0, 0, 0.25, 0.25, P1.PSide);
				// ヒット数設定
				P1.Var[11] = 0;
			}

			if ((P1.time == 0) && (P1.StopTime == 0))PVOStart(P1.PSide, 22, 0);

			// ヘルパー変更.[2]、発射前なら変更
			if (P1.time == 4){
				if ((H1[2].var) && (H1[2].stateno == 2020)){
					H1[2].var = true;
					H1[2].time = 5;

					if (P2.XPos < H1[2].XPos)
						H1[2].muki = 1;
					else if (P2.XPos > H1[2].XPos)
						H1[2].muki = 0;

					H1[2].YPos += 100;
					H1[2].stateno = 2100;
					P1.Var[12] = H1[2].XPos;	// 座標
				}
			}


			// 終了・34
			if (P1.time >= 34)P1.time = 0, P1.stateno = 0, P1.ctrl = 1;

			break;
			
			//********************
			// 850 ヴァンパイアキラー
			//********************
		case 850:
			P1.ctrl = 0;
			P1.A.gaugeHosei = true;
			P1.ignoreG = false;
			P1.SFlg = 0;

			// ゲージ消費
			if (P1.time == 0) {
				P1.Power -= 3000;
				S.StopTime = 15;
				S.noGauge = 15;
				S.Anten = 25;
				SEStart(25);
				// エフェクト
				EffStartB(17, P1.XPos, P1.YPos - (P1.GraphH / 2) * P1.GSize, 0, 0, 0.25, 0.25, P1.PSide);

				DamReset();
				P1.Var[13] = 13;
			}

			if(P1.time == 95)SEStart(22);


			// 呼び出し.[1]	飛び道具
			if ((P1.time == 10) || (P1.time == 17) || 
				(P1.time == 24) || (P1.time == 31) ||
				(P1.time == 38) || (P1.time == 45) ||
				(P1.time == 52) 
				//|| (P1.time == 59)
				) {
				HelperReset(P1.Var[13]);
				H1[P1.Var[13]].var = true;
				H1[P1.Var[13]].time = 0;
				if (P1.time == 52)
					H1[P1.Var[13]].stateno = 2150;
				else 
				{ H1[P1.Var[13]].stateno = 2050; }
				H_PosSet(P1.Var[13], 20, -168);
				P1.Var[13]++;

				VelSet(-0.8, 0);
				if (P1.time == 52)VelSet(-5.4, 0);
				SEStart(40);

				
			}

			if (P1.time == 114) {
				EffStartB(16, P1.XPos, P1.YPos, 0, -(P1.ySize / 1.5),
					0.25, 0.25, P1.muki);
				P1.Var[10] = 180 * 6;
				SEStart(31);
			}

			// 終了
			if (P1.time >= ANIMELEM) {
				P1.time = 0, P1.stateno = 0, P1.ctrl = 0;

			}

			break;

			//===============================//
		}// switch終了

		// ステート番号を保存
		P1.OldState = P1.stateno;
		P1.oldState2 = P1.State2;
	}// P1処理終了


	for (int i = 0; i < HM; i++){
		{
			//********************
			//
			// ヘルパー
			//
			//********************

			// オンだったらステートに飛ばす
			if (H1[i].var){
				switch (H1[i].stateno){
					//********************
					// 2000 5Dor2D
					//********************
				case 2000:


					//***************
					//  基本設定
					//***************
					// 座標・速度設定
					if (H1[i].time == 0) {
						H1[i].muki = P1.muki;

						// 速度なかったら設定
						if (H1[i].HXVel == 0) {
							H_VelSet(i, 18.6, 0);
						}

						//H_PosSet(i, 20, -166);
					}

					// ヒット数・ダメージセット
					if (H1[i].time == 0) {
						H1[i].HMoveHit = 1;
						H1[i].A.damage = 100;
						H1[i].A.kezuri = 25;
						// 強化
						if (P1.Var[11] > 0) {
							H1[i].HMoveHit = 2;
							H1[i].A.damage = 100;
							
						}
					}
					// ダメージ位置セット
					if (H_triggerT(i, 0, 1)) {

						//[ 攻撃判定セット ]
						//H2_AttSet(i, 0, 50, 30, -20, -36);

						// [ヒット・ガードサウンド]
						H1[i].HHitSE = 11, H1[i].HGuardSE = 17;
						// [ゲージ] 
						H1[i].HGetPow = 120, H1[i].HGivePow = 60;
						//[ ガード条件 ]
						H1[i].HGuardF = 1;
						// [ヒット硬直]
						H_HitTime(i, 8, 22, 24, 16);
						H1[i].HSSelf = 0;

						H1[i].fallF = 2;

						// [ヒット時のアニメ]
						H1[i].HHitAnim = 1000;
						// [ノックバック]
						H_HitVel(i, -6.0, 0, -3.4, -3.0);

						if (P1.Var[11] > 0) {
							H_HitTime(i, 6, 24, 26, 18);
							H_HitVel(i, -3.8, 0, -2.6, -3.0);
						}
					}


					H_Move(i);

					//***************
					//  行動内容
					//***************
					// アニメをループさせる
					if (H1[i].HAnimTime >= 16)H1[i].HAnimTime = 0;
					// 終了条件、端に到達or地面に当たるorヒットor本体被ダメ
					if ((H1[i].XPos < (S.ScroolX)) ||
						(H1[i].XPos > (S.ScroolX + SCREEN_W)) ||
						(H1[i].YPos > GROUND) ||
						(H1[i].HMoveHit == 0) ||
						(P1.stateno >= 1000)
						)
					{
						HelperReset(i);
					}

					break;
					
					//********************
					// 2001 斜め上
					//********************
				case 2001:


					//***************
					//  基本設定
					//***************
					// 座標・速度設定
					if (H1[i].time == 0) {
						H1[i].muki = P1.muki;

						// 速度なかったら設定
						if (H1[i].HXVel == 0) {
							H_VelSet(i, 12.4, -12.4);
						}

						H_PosSet(i, 20, -188);
					}

					// ヒット数・ダメージセット
					if (H1[i].time == 0) {
						H1[i].HMoveHit = 1;
						H1[i].A.damage = 100;
						H1[i].A.kezuri = 25;
						// 強化
						if (P1.Var[11] > 0) {
							H1[i].HMoveHit = 2;
							H1[i].A.damage = 100;
						}
					}
					// ダメージ位置セット
					if (H_triggerT(i, 0, 1)) {

						//[ 攻撃判定セット ]
						//H2_AttSet(i, 0, 50, 30, -20, -36);

						// [ヒット・ガードサウンド]
						H1[i].HHitSE = 11, H1[i].HGuardSE = 17;
						// [ゲージ] 
						H1[i].HGetPow = 120, H1[i].HGivePow = 60;
						//[ ガード条件 ]
						// 空ガ不能に
						H1[i].HGuardF = 4;
						// [ヒット硬直]
						H_HitTime(i, 8, 26, 30, 18);
						H1[i].HSSelf = 0;

						H1[i].fallF = 2;

						// [ヒット時のアニメ]
						H1[i].HHitAnim = 1000;
						// [ノックバック]
						H_HitVel(i, -5.6, 0, -2.0, -2.4);
						
						if (P1.Var[11] > 0) {
							H_HitTime(i, 6, 26, 28, 20);
							H_HitVel(i, -4.0, 0, -2.8, -3.0);
						}
					}


					H_Move(i);

					//***************
					//  行動内容
					//***************
					// アニメをループさせる
					if (H1[i].HAnimTime >= 16)H1[i].HAnimTime = 0;
					// 終了条件、端に到達or地面に当たるorヒットor本体被ダメ
					if ((H1[i].XPos < (S.ScroolX)) ||
						(H1[i].XPos > (S.ScroolX + SCREEN_W)) ||
						(H1[i].YPos > GROUND) ||
						(H1[i].HMoveHit == 0) ||
						(P1.stateno >= 1000)
						)
					{
						HelperReset(i);
					}

					break;

					//********************
					// 2002 斜め下
					//********************
				case 2002:


					//***************
					//  基本設定
					//***************
					// 座標・速度設定
					if (H1[i].time == 0) {
						H1[i].muki = P1.muki;

						// 速度なかったら設定
						if (H1[i].HXVel == 0) {
							H_VelSet(i, 12.4, 12.4);
						}

						H_PosSet(i, 20, -138);
					}

					// ヒット数・ダメージセット
					if (H1[i].time == 0) {
						H1[i].HMoveHit = 1;
						H1[i].A.damage = 100;
						H1[i].A.kezuri = 25;
						// 強化
						if (P1.Var[11] > 0) {
							H1[i].HMoveHit = 2;
							H1[i].A.damage = 100;
						}
					}
					// ダメージ位置セット
					if (H_triggerT(i, 0, 1)) {

						//[ 攻撃判定セット ]
						//H2_AttSet(i, 0, 50, 30, -20, -36);

						// [ヒット・ガードサウンド]
						H1[i].HHitSE = 11, H1[i].HGuardSE = 17;
						// [ゲージ] 
						H1[i].HGetPow = 120, H1[i].HGivePow = 60;
						//[ ガード条件 ]
						H1[i].HGuardF = 1;
						// [ヒット硬直]
						H_HitTime(i, 8, 22, 24, 16);
						H1[i].HSSelf = 0;

						H1[i].fallF = 2;

						// [ヒット時のアニメ]
						H1[i].HHitAnim = 1000;
						// [ノックバック]
						H_HitVel(i, -6.0, 0, -3.2, -2.8);
						
						if (P1.Var[11] > 0) {
							H_HitTime(i, 6, 24, 26, 18);
							H_HitVel(i, -3.8, 0, -2.6, -3.0);
						}
					}


					H_Move(i);

					//***************
					//  行動内容
					//***************
					// アニメをループさせる
					if (H1[i].HAnimTime >= 16)H1[i].HAnimTime = 0;
					// 終了条件、端に到達or地面に当たるorヒットor本体被ダメ
					if ((H1[i].XPos < (S.ScroolX)) ||
						(H1[i].XPos > (S.ScroolX + SCREEN_W)) ||
						(H1[i].YPos > GROUND) ||
						(H1[i].HMoveHit == 0) ||
						(P1.stateno >= 1000)
						)
					{
						HelperReset(i);
					}

					break;

					//********************
					// 2003 30度
					//********************
				case 2003:


					//***************
					//  基本設定
					//***************
					// 座標・速度設定
					if (H1[i].time == 0) {
						H1[i].muki = P1.muki;

						// 速度なかったら設定
						if (H1[i].HXVel == 0) {
							H_VelSet(i, 15.2, 7.6);
						}

						H_PosSet(i, 20, -130);
					}

					// ヒット数・ダメージセット
					if (H1[i].time == 0) {
						H1[i].HMoveHit = 1;
						H1[i].A.damage = 100;
						H1[i].A.kezuri = 25;
						// 強化
						if (P1.Var[11] > 0) {
							H1[i].HMoveHit = 2;
							H1[i].A.damage = 100;
						}
					}
					// ダメージ位置セット
					if (H_triggerT(i, 0, 1)) {

						//[ 攻撃判定セット ]
						//H2_AttSet(i, 0, 50, 30, -20, -36);

						// [ヒット・ガードサウンド]
						H1[i].HHitSE = 11, H1[i].HGuardSE = 17;
						// [ゲージ] 
						H1[i].HGetPow = 120, H1[i].HGivePow = 60;
						//[ ガード条件 ]
						H1[i].HGuardF = 1;
						// [ヒット硬直]
						H_HitTime(i, 8, 22, 24, 16);
						H1[i].HSSelf = 0;

						H1[i].fallF = 2;

						// [ヒット時のアニメ]
						H1[i].HHitAnim = 1000;
						// [ノックバック]
						H_HitVel(i, -6.0, 0, -3.2, -3.0);

						if (P1.Var[11] > 0) {
							H_HitTime(i, 6, 24, 26, 18);
							H_HitVel(i, -3.8, 0, -2.6, -3.0);
						}
					}


					H_Move(i);

					//***************
					//  行動内容
					//***************
					// アニメをループさせる
					if (H1[i].HAnimTime >= 16)H1[i].HAnimTime = 0;
					// 終了条件、端に到達or地面に当たるorヒットor本体被ダメ
					if ((H1[i].XPos < (S.ScroolX)) ||
						(H1[i].XPos > (S.ScroolX + SCREEN_W)) ||
						(H1[i].YPos > GROUND) ||
						(H1[i].HMoveHit == 0) ||
						(P1.stateno >= 1000)
						)
					{
						HelperReset(i);
					}

					break;


					//********************
					// 2050 5Dor2D
					//********************
				case 2050:

					//***************
					//  基本設定
					//***************
					// 座標・速度設定
					if (H1[i].time == 0) {
						H1[i].muki = P1.muki;

						// 速度なかったら設定
						if (H1[i].HXVel == 0) {
							H_VelSet(i, 18.6, 0);
						}

						//H_PosSet(i, 20, -166);
					}

					// ヒット数・ダメージセット
					if (H1[i].time == 0) {
						H1[i].HMoveHit = 1;
						H1[i].A.damage = 87;
						H1[i].A.hosyo = 8;
						H1[i].A.kezuri = 24;
					}
					// ダメージ位置セット
					if (H_triggerT(i, 0, 1)) {

						//[ 攻撃判定セット ]
						//H2_AttSet(i, 0, 50, 30, -20, -36);

						// [ヒット・ガードサウンド]
						H1[i].HHitSE = 11, H1[i].HGuardSE = 17;
						// [ゲージ] 
						H1[i].HGetPow = 0, H1[i].HGivePow = 60;
						//[ ガード条件 ]
						H1[i].HGuardF = 1;
						// [ヒット硬直]
						H_HitTime(i, 10, 22, 24, 16);
						H1[i].HSSelf = 0;

						H1[i].fallF = 2;

						// [ヒット時のアニメ]
						H1[i].HHitAnim = 1000;
						// [ノックバック]
						H_HitVel(i, -4.0, 0, -2.8, -2.6);

					}


					H_Move(i);

					//***************
					//  行動内容
					//***************
					// アニメをループさせる
					if (H1[i].HAnimTime >= 16)H1[i].HAnimTime = 0;
					// 終了条件、端に到達or地面に当たるorヒットor本体被ダメ
					if ((H1[i].XPos < (S.ScroolX)) ||
						(H1[i].XPos > (S.ScroolX + SCREEN_W)) ||
						(H1[i].YPos > GROUND) ||
						(H1[i].HMoveHit == 0) ||
						(P1.stateno >= 1000)
						)
					{
						HelperReset(i);
					}

					break;
					
					//********************
					// 2100 凶弾
					//********************
				case 2100:
					//***************
					//  基本設定
					//***************
					P1.A.gaugeHosei = true;

					// 座標・速度設定
					if (H1[i].time == 0) {
						H1[i].muki = P1.muki;

						// 速度なかったら設定
						if (H1[i].HXVel == 0) {
							H_VelSet(i, 24.4, 0);
						}

						H_PosSet(i, 20, -168);
						EffStart(16, (int)H1[i].XPos, (int)H1[i].YPos, 20, 0,
							0.02, 0.14, P1.muki);
					}

					// ヒット数・ダメージセット
					if (H1[i].time == 0) {
						H1[i].HMoveHit = 1;
						H1[i].A.damage = 130;
						H1[i].A.hosyo = 80;
						H1[i].A.kezuri = 58;
						// 強化
						if (P1.Var[11] > 0) {
							H1[i].HMoveHit = 1;
							H1[i].A.damage = 150;
							H1[i].A.hosyo = 100;
						}
					}

					// ダメージ位置セット
					if (H_triggerT(i, 0, 1)) {

						// エフェクト
						h_HitEff(i, 1, 1, 1);

						// [ヒット・ガードサウンド]
						H1[i].HHitSE = 14, H1[i].HGuardSE = 17;
						// [ゲージ] 
						H1[i].HGetPow = 0, H1[i].HGivePow = 220;
						//[ ガード条件 ]
						H1[i].HGuardF = 1;
						// [ヒット硬直]
						H_HitTime(i, 8, 30, 40, 16);
						H1[i].HSSelf = 0;

						H1[i].fallF = 1;

						// [ヒット時のアニメ]
						H1[i].HHitAnim = 1030;
						// [ノックバック]
						H_HitVel(i, -5.6, -3.2, -3.2, -3.0);
						if (P1.Var[11] > 0) {
							H_HitTime(i, 10, 32, 42, 18);
							H_HitVel(i, -5.2, -3.8, -4.4, -3.2);
						}
					}


					H_Move(i);

					//***************
					//  行動内容
					//***************
					// アニメをループさせる
					if (H1[i].HAnimTime >= 16)H1[i].HAnimTime = 0;
					// 終了条件、端に到達or地面に当たるorヒットor本体被ダメ
					if ((H1[i].XPos < (S.ScroolX)) ||
						(H1[i].XPos > (S.ScroolX + SCREEN_W)) ||
						(H1[i].YPos > GROUND) ||
						(H1[i].HMoveHit == 0) ||
						(P1.stateno >= 1000)
						)
					{
						HelperReset(i);
					}

					break;


					//********************
					// 2150 凶弾
					//********************
				case 2150:
					//***************
					//  基本設定
					//***************
					P1.A.gaugeHosei = true;

					// 座標・速度設定
					if (H1[i].time == 0) {
						H1[i].muki = P1.muki;

						// 速度なかったら設定
						if (H1[i].HXVel == 0) {
							H_VelSet(i, 18.6, 0);
						}

						H_PosSet(i, 20, -168);
						EffStart(16, (int)H1[i].XPos, (int)H1[i].YPos, 20, 0,
							0.02, 0.14, P1.muki);
					}

					// ヒット数・ダメージセット
					if (H1[i].time == 0) {
						H1[i].HMoveHit = 1;
						H1[i].A.damage = 130;
						H1[i].A.hosyo = 100;
						H1[i].A.kezuri = 58;
					}

					// ダメージ位置セット
					if (H_triggerT(i, 0, 1)) {

						// エフェクト
						h_HitEff(i, 1, 1, 1);

						// [ヒット・ガードサウンド]
						H1[i].HHitSE = 14, H1[i].HGuardSE = 17;
						// [ゲージ] 
						H1[i].HGetPow = 0, H1[i].HGivePow = 220;
						//[ ガード条件 ]
						H1[i].HGuardF = 1;
						// [ヒット硬直]
						H_HitTime(i, 10, 30, 40, 16);
						H1[i].HSSelf = 0;

						H1[i].fallF = 1;

						// [ヒット時のアニメ]
						H1[i].HHitAnim = 1030;
						// [ノックバック]
						H_HitVel(i, -5.0, -3.2, -3.8, -2.8);
					}


					H_Move(i);

					//***************
					//  行動内容
					//***************
					// アニメをループさせる
					if (H1[i].HAnimTime >= 16)H1[i].HAnimTime = 0;
					// 終了条件、端に到達or地面に当たるorヒットor本体被ダメ
					if ((H1[i].XPos < (S.ScroolX)) ||
						(H1[i].XPos > (S.ScroolX + SCREEN_W)) ||
						(H1[i].YPos > GROUND) ||
						(H1[i].HMoveHit == 0) ||
						(P1.stateno >= 1000)
						)
					{
						HelperReset(i);
					}

					break;

					//===============================//
					//******************
					// ヘルパー処理終了
					//******************
				}// switch終了
			}// オン・オフ判断終了
		}// 次のヘルパーへ
	}// for終了

	// 値を返す
	Get_SSet(S);
	if (P1.PSide == 1)Get_PSet(P1, P2), Get_HSet(H1, H2);//, Get_SSet(S);
	if (P1.PSide == 2)Get_PSet(P2, P1), Get_HSet(H2, H1);//, Get_SSet(S);

}//StateParam終了




//-------------------------------------------------------------------------------
//
// 受け取り用
//
//-------------------------------------------------------------------------------

// 順番を逆にする
void GetP_HydParam(Player GP1, Player GP2)
{
	P1 = GP1;
	P2 = GP2;
}

void GetS_HydParam(System_t GS)
{
	S = GS;
}

void GetH_HydParam(Helper GH1[], Helper GH2[])
{
	int i;

	for (i = 0; i < HM; i++){
		H1[i] = GH1[i];
		H2[i] = GH2[i];
	}

}



//-------------------------------------------------------------------------------
//
// 受け取り２
//
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//
// 数値初期化
//
//-------------------------------------------------------------------------------



//-------------------------------------------------------------------------------
//
// キャンセル設定
//
//-------------------------------------------------------------------------------

// 必殺技キャンセル
void SCancel()
{
	if (P1.Senkou[4] > 0 && (P1.Var[10] > 180)) {
		// 地上にいて、ヘルパー[1]が存在しない
		if (P1.SFlg != 2) {
			P1.stateno = 600, P1.More = 1,
				P1.time = 0, P1.A.damage = 0;
			if (((P1.K_Senkou[2] > 0) && (P1.K_Senkou[6] > 0)) ||
				((P1.keyAtt[2]) && (P1.keyAtt[6])))P1.stateno = 603;
			else if (P1.K_Senkou[2] > 0 || P1.keyAtt[2])
				P1.stateno = 602;
			else if (P1.K_Senkou[6] > 0 || P1.keyAtt[6])
				P1.stateno = 601;
		}
	}

	// 214 + AorB
	if (((P1.Senkou[1] > 0) || (P1.Senkou[2] > 0) || (P1.Senkou[3] > 0))
		&& (P1.cmd[2])) {		// 先行入力効かせてみる
		if (P1.SFlg != 2) {
			P1.stateno = 610, P1.More = 1,
				P1.time = 0, P1.A.damage = 0;
			if (P1.Senkou[2] > 0 || P1.Senkou[3] > 0)P1.stateno = 611;
		}
	}

	// 214 + D
	if ((P1.Senkou[4] > 0) && (P1.Power >= 500)
		&& (P1.cmd[2])) {		// 先行入力効かせてみる
		P1.stateno = 710, P1.More = 1,
			P1.time = 0, P1.A.damage = 0;
	}

	// 22Ｄ
	if ((P1.Senkou[4] > 0) && (P1.cmd[3])) {		// 先行入力効かせてみる
		if (P1.SFlg != 2) {
			P1.stateno = 630, P1.More = 1,
				P1.time = 0, P1.A.damage = 0;
		}
	}

	HCancel();

	//最後に判定を消す
	DamReset();
}

// 必殺技キャンセル
void HCancel()
{
	// 236 + C
	if ((P1.Senkou[3] > 0)
		&& (P1.cmd[1]) && (P1.Power >= 1000)) {		// 先行入力効かせてみる
		P1.stateno = 800, P1.More = 1,
		P1.time = 0, P1.A.damage = 0;
	}

	// 214 + C
	if ((P1.Senkou[3] > 0)
		&& (P1.cmd[2]) && (P1.Power >= 1000)) {		// 先行入力効かせてみる
		P1.stateno = 810, P1.More = 1,
			P1.time = 0, P1.A.damage = 0;
	}

	//  [ ３ゲージ ]
	if ((P1.Senkou[1] > 0) && (P1.Senkou[2] > 0) && (P1.Senkou[3] > 0)
		&& (P1.Power >= 3000)) {		// 地上
		P1.stateno = 850;	// 超必
		P1.More = 1,
		P1.time = 0, P1.A.damage = 0;
	}

	//最後に判定を消す
	DamReset();
}

// 空中必キャンセル
void ACancel()
{
	// [必殺技]
	if (P1.SFlg == 2) {
		// D (ノーマル設置、地上発射)
		if (P1.Senkou[4] > 0 && (P1.Var[10] > 180)) {
			// ヘルパー[2]がある
			if ((P1.keyAtt[6]) && (!P1.keyAtt[2]) && (!P1.keyAtt[8])) {
					P1.stateno = 606, P1.More = 1,
						P1.time = 0, P1.A.damage = 0;
			}
			// 設置がない
			else {
				P1.stateno = 605, P1.More = 1,
					P1.time = 0, P1.A.damage = 0;
			}
		}
	}
}
