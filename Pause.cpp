//==================================================================================
//
//			|[YæÊÌÇ
//
//========================================================================

//CN[h--------------------------------------------
#include "pch.h"

//ft@C----------------------------------------------




//ÃÞ°Àè`------------------------------------------------

static int Load_1;
static int P1_Key;
static const int TxtDist = 29;
static const int drawMax = 15;	// |[Y\¦Åå
static int drawPos;
static int SPoint;	// Ið|C^ÌÊuú»
static int TimeStop, kettei, kettei2;
static int inputUP, inputDOWN;
static int SentakuNum;
static int selectMax = 12;

static int page;
static const int page_max = 2;
static const int pageBoxSize = SCREEN_H - 40 - 40;

static Player P[2];

// ¤ÊA@R}h\ÆgÝè
static int ComList;

//Êíj[
static int SNum;

// gÝè
static int column, row;
static int c_Max, r_Max;

// ÌÚ
static int nx;

// R}h
static int gK[10], gB[4], gP;

// æ
//static int cursor;

// rawðÏXµÈ¢
static boolean noChangeRow;


//àÖé¾--------------------------------------------

static void CommandList();
static void TraningSet();
static void TrainingList();
//void BackGraph();	// wi`æ
static void BackGraphT();

//vO----------------------------------------------



int Pause()
{

	if (Load_1 == 0){

		// IðÚÌðæ¾
		kettei = 0, SPoint = 0,
		SentakuNum = 5;

		//Êíj[
		SNum = 3;

		// ÚÌÅå
		selectMax = 12;
		// \¦Êu
		drawPos = 0;

		// g
		column = 0, row = 0;
		c_Max = 0, r_Max = 0;
		inputUP = 0, inputDOWN = 0;
		ComList = 0;

		// æÇÝÝ
		// Load_PauseÅs¤

		Load_1 = 1;
	}


	{
		// üÍÍ±±Å`FbN
		//P1_Key = P_Input(1);	// OK
	}

	// [vAvOÀsJn
	// [ ¤ÊÝè]
	{
		
		// 2Pàg¦é
		if (!S.TraningFlg){
			// è
			if ((P_BInput(1) == 1 || P_BInput(3) == 1) && (kettei != 1))
			{
				kettei = 1;
			}
			else{ kettei = 0; }

			// ã
			if (KInput(P_BInput(108))){
				inputUP += 1;
			}
			else{ inputUP = 0; }

			// º
			if (KInput(P_BInput(102))){
				inputDOWN += 1;
			}
			else{ inputDOWN = 0; }
		}
		// gp
		else{
			// è
			if ((P1_BInput(1) == 1 || P1_BInput(3) == 1) && (kettei != 1))
			{
				kettei = 1;
			}
			else{ kettei = 0; }

			// ã
			if (KInput(P1_BInput(108))){
				inputUP += 1;
			}
			else{ inputUP = 0; }

			// º
			if (KInput(P1_BInput(102))){
				inputDOWN += 1;
			}
			else{ inputDOWN = 0; }

			
		}

		// èµÄ½çJEgÁZ
		if (kettei == 1){
			TimeStop += 1;
		}
		else{
			TimeStop = 0;
		}

	}

	// [gj[]
	if (S.TraningFlg)
	{
		
		// î{
		if (ComList == 0){

			/**************
			* wi
			**************/
			if (P1_BInput(4) == 0)
				// wi
				{
					BackGraphT();
				}

				

				// J[\Ú®
				if (kettei == 0){
					if (inputUP == 1) SPoint--, SEStart(36);
					if (inputDOWN == 1) SPoint++, SEStart(36);
					if (SPoint < 0)SPoint = SentakuNum + selectMax - 1;
					if (SPoint > SentakuNum + selectMax - 1)SPoint = 0;
				}
				
				// gÝè
				TraningSet();

				// èL[üÍ
				// Êuú»
				if ((kettei == 1) && (SPoint == selectMax))
				{
					kettei = 0;
					return 1;
				}
				// R}h\
				else if ((kettei == 1) && (SPoint == selectMax + 1))
				{
					kettei = 0;
					ComList = 1;
				}
				// ÎíæÊÉßé
				else if (((kettei == 1) && (SPoint == selectMax + 2)) || (P1_BInput(2) == 1) || (P1_BInput(7) == 1))
				{
					kettei = 0;
					return 4;
				}
				// LZÉßé
				else if ((kettei == 1) && (SPoint == selectMax + 3))
				{
					kettei = 0;
					return 2;
				}
				// j[Éßé
				else if ((kettei == 1) && (SPoint == selectMax + 4))
				{
					kettei = 0;
					Load_1 = 0;
					return 3;
				}
				

				// `æ
				if (P1_BInput(4) == 0)
				{
					// J[\`æ
					{
						// `æuh[hðAt@uhi§¾jÉ·é
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
						// J[\`æ
						DrawBox(0, (SPoint * TxtDist) + 29 + (drawPos * TxtDist), 
							SCREEN_W, (SPoint * TxtDist) + 48 + (drawPos * TxtDist), GetColor(20, 20, 250), true);
						NoBlend();
					}

					// Ýèf[^`æ
					{
						TrainingList();
						//DrawString(0, 0, "DµÁÏÅÚðÁ·", Cr);
						DrawString(TxtDist, TxtDist * (selectMax + 1) + (drawPos * TxtDist), "|WVZbg", Cr);
						DrawString(TxtDist, TxtDist * (selectMax + 2) + (drawPos * TxtDist), "R}h\", Cr);
						DrawString(TxtDist, TxtDist * (selectMax + 3) + (drawPos * TxtDist), "í¬æÊÖßé", Cr);
						
						if (drawPos < 0)	// ¸ç·
						DrawString(TxtDist, TxtDist * (selectMax + 4) + (drawPos * TxtDist), "LN^[ZNgÉßé", Cr);
						if (drawPos < -1)	// ¸ç·
							DrawString(TxtDist, TxtDist * (selectMax + 5) + (drawPos * TxtDist), "j[Éßé", Cr);

						// ÚÊuÌlp
						if (drawPos == 0)
							DrawBox(SCREEN_W - 20, 20, SCREEN_W, 20 + pageBoxSize, Cr, true);
						else if (drawPos == -1)
							DrawBox(SCREEN_W - 20, 40, SCREEN_W, 40 + pageBoxSize, Cr, true);
						else if (drawPos == -2)
							DrawBox(SCREEN_W - 20, SCREEN_H - 20 - pageBoxSize, SCREEN_W, SCREEN_H - 20, Cr, true);
					}
					// J[\`æ
					//DrawBox(5, (SPoint * TxtDist) + 29, 24, (SPoint * TxtDist) + 48, Cr, true);
					
				}
			}
			else if(ComList == 1){
				CommandList();
			}

	}//gj[I¹A
	// Êíj[
	else{

		if (ComList == 0){
			/**************
			* wi
			**************/
			if (P_BInput(4) == 0)
			{
				BackGraphT();
			}

			// J[\Ú®
			if (kettei == 0){
				if (inputUP == 1) SPoint--, SEStart(36);
				if (inputDOWN == 1) SPoint++, SEStart(36);
				if (SPoint < 0)SPoint = SNum - 1;
				if (SPoint > SNum - 1)SPoint = 0;

			}

			// èL[üÍ
			// æÊÉßé
			if (((kettei == 1) && (SPoint == 0)) || (P_BInput(2) == 1) || (P_BInput(7) == 1))
			{
				kettei = 0;
				return 4;
			}
			// R}h\
			else if ((kettei == 1) && (SPoint == 1))
			{			
				kettei = 0;
				ComList = 1;
			}
			// j[Éßé
			else if ((kettei == 1) && (SPoint == 2))
			{
				kettei = 0;
				Load_1 = 0;
				return 3;
			}

			// j[Éßé
			if (Replay_Mode(-1) == 1)
			{
				kettei = 0;
				Load_1 = 0;
				Replay_End();
				return GameScene(ReplayScene);
			}

			// `æ
			if (P_BInput(4) == 0)
			{
				
				// J[\`æ
				{
					// `æuh[hðAt@uhi§¾jÉ·é
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
					// J[\`æ
					DrawBox(0, (SPoint * TxtDist) + 29, SCREEN_W, (SPoint * TxtDist) + 48, GetColor(20, 20, 250), true);
					NoBlend();
				}
				//DrawString(0, 0, "DµÁÏÅÚðÁ·", Cr);
				DrawString(TxtDist, TxtDist * 1, "ÎíæÊÉßé", Cr);
				DrawString(TxtDist, TxtDist * 2, "R}h\", Cr);
				DrawString(TxtDist, TxtDist * 3, "j[Éßé", Cr);
				
			}

		}
		else if (ComList == 1){
			CommandList();
		}
	}// Îíj[I¹

	return 0;
}


void GetP_Pause(Player GP1, Player GP2)
{
	P1 = GP1;
	P2 = GP2;
}

void GetS_Pause(System_t GS)
{
	S = GS;
}


void TraningSet()
{

	/**************
	* vZ
	**************/
	{
		{
			// ¶EÌüÍ
			if ((P1_BInput(104) == 1) || (P1_BInput(106) == 1)){
				if (P1_BInput(104) == 1)row -= 1;
				if (P1_BInput(106) == 1)row += 1;
			}
			else{ row = 0; }

			if (row == 1 || row == -1)SEStart(35);

			// CLXMBI
			
			//if ((P1_BInput(104) == 0) && (P1_BInput(106) == 0)){

			// [hµ½ÅÍÏ¦È¢i¢êÁÏÅ©íéÌh~j
			if (noChangeRow){
				row = 0;
				noChangeRow = false;
			}

			// `æÊu
			if ((SPoint > drawMax - 1) && (drawPos > -1))drawPos = -1;
			else if ((SPoint > drawMax) && (drawPos > -2))drawPos = -2;
			
			if ((SPoint == 0) && (drawPos != 0))drawPos = 0;
			else if ((SPoint == 1) && (drawPos < -1))drawPos = -1;
		}

		{
			nx = 0;
			// ÌÍ
			if (SPoint == nx){
				if (S.T_Life == 0){
					if (row == 1 || row == -1)
						S.T_Life = 1;
				}
				else if (S.T_Life == 1){
					if (row == 1 || row == -1)
						S.T_Life = 0;
				}
			}
			nx++;
			// Q[W
			if (SPoint == nx){
				if (row == 1)
					S.T_Power += 1;
				else if (row == -1)
					S.T_Power -= 1;

				if (S.T_Power < 0)
					S.T_Power = 3;
				if (S.T_Power > 3)
					S.T_Power = 0;
			}
			nx++;
			// K[hQ[W
			if (SPoint == nx){
				if (row == 1)
					S.T_GGauge += 1;
				else if (row == -1)
					S.T_GGauge -= 1;

				if (S.T_GGauge < 0)
					S.T_GGauge = 1;
				if (S.T_GGauge > 1)
					S.T_GGauge = 0;
			}
			nx++;
			// AhQ[W
			if (SPoint == nx){
				if (row == 1)
					S.T_AddGauge += 1;
				else if (row == -1)
					S.T_AddGauge -= 1;

				if (S.T_AddGauge < 0)
					S.T_AddGauge = 1;
				if (S.T_AddGauge > 1)
					S.T_AddGauge = 0;
			}
			nx++;
			// Ì¨
			if (SPoint == nx){
				if (row == 1)
					S.TSwitch[2] += 1;
				else if (row == -1)
					S.TSwitch[2] -= 1;

				if (S.TSwitch[2] < 0)
					S.TSwitch[2] = 4;
				if (S.TSwitch[2] > 4)
					S.TSwitch[2] = 0;
			}
			nx++;
			// ó¯g
			if (SPoint == nx){
				if (row == 1)
					S.TSwitch[3] += 1;
				else if (row == -1)
					S.TSwitch[3] -= 1;

				if (S.TSwitch[3] < 0)
					S.TSwitch[3] = 3;
				if (S.TSwitch[3] > 3)
					S.TSwitch[3] = 0;
			}
			nx++;
			// K[h
			if (SPoint == nx){
				if (row == 1)
					S.TSwitch[4] += 1;
				else if (row == -1)
					S.TSwitch[4] -= 1;

				if (S.TSwitch[4] < 0)
					S.TSwitch[4] = 2;
				if (S.TSwitch[4] > 2)
					S.TSwitch[4] = 0;
			}
			nx++;
			// JE^[
			if (SPoint == nx){
				if (row == 1)
					S.T_Counter += 1;
				else if (row == -1)
					S.T_Counter -= 1;

				if (S.T_Counter < 0)
					S.T_Counter = 1;
				if (S.T_Counter > 1)
					S.T_Counter = 0;
			}
			nx++;
			// °²¯
			if (SPoint == nx){
				if (row == 1)
					S.T_ThrowReject += 1;
				else if (row == -1)
					S.T_ThrowReject -= 1;

				if (S.T_ThrowReject < 0)
					S.T_ThrowReject = 1;
				if (S.T_ThrowReject > 1)
					S.T_ThrowReject = 0;
			}
			nx++;
			// Ú\¦
			if (SPoint == nx){
				if (row == 1)
					S.TSwitch[5] += 1;
				else if (row == -1)
					S.TSwitch[5] -= 1;

				if (S.TSwitch[5] < 0)
					S.TSwitch[5] = 3;
				if (S.TSwitch[5] > 3)
					S.TSwitch[5] = 0;
			}
			nx++;
			// »è
			if (SPoint == nx){
				if (row == 1)
					S.TSwitch[6] += 1;
				else if (row == -1)
					S.TSwitch[6] -= 1;

				if (S.TSwitch[6] < 0)
					S.TSwitch[6] = 3;
				if (S.TSwitch[6] > 3)
					S.TSwitch[6] = 0;
			}
			nx++;
			// Ä¶û@
			if (SPoint == nx){
				if (row == 1)
					S.TSwitch[8] += 1;
				else if (row == -1)
					S.TSwitch[8] -= 1;

				if (S.TSwitch[8] < 0)
					S.TSwitch[8] = 2;
				if (S.TSwitch[8] > 2)
					S.TSwitch[8] = 0;
			}
		}


	}
	
	/**************
	* M
	**************/
	{
		Get_SSet(S);
	}
}

void TrainingList()
{
	/**************
	* `æ
	**************/
	
		nx = 0 + drawPos;

		nx++;
		
		if (drawPos >= 0) {
			// ¸ç·
			DrawString(30, TxtDist * nx, "ÌÍ", Cr);
			if (S.T_Life == 0)DrawString(160, TxtDist * nx, "©®ñ", Oran);
			else { DrawString(160, TxtDist * nx, "Êí", Cr); }
			
		}
		
		nx++;
		
		if (drawPos >= -1) {
			DrawString(30, TxtDist * nx, "p[Q[W", Cr);
			if (S.T_Power == 0)DrawString(160, TxtDist * nx, "©®ñ", Oran);
			else if (S.T_Power == 1) { DrawString(160, TxtDist * nx, "1Q[W", Cr); }
			else if (S.T_Power == 2) { DrawString(160, TxtDist * nx, "2Q[W", Cr); }
			else if (S.T_Power == 3) { DrawString(160, TxtDist * nx, "Êí", Cr); }
		}

		nx++;

		DrawString(30, TxtDist * nx, "K[hQ[W", Cr);
		if (S.T_GGauge == 0)DrawString(160, TxtDist * nx, "©®ñ", Oran);
		else if (S.T_GGauge == 1){ DrawString(160, TxtDist * nx, "Êí", Cr); }

		nx++;

		DrawString(30, TxtDist * nx, "AhQ[W", Cr);
		if (S.T_AddGauge == 0)DrawString(160, TxtDist * nx, "Êí", Oran);
		else if (S.T_AddGauge == 1){ DrawString(160, TxtDist * nx, "MAX", Cr); }

		nx++;

		DrawString(30, TxtDist * nx, "Ì¨", Cr);
		if (S.TSwitch[2] == 0)DrawString(160, TxtDist * nx, "§¿", Oran);
		else if (S.TSwitch[2] == 1)DrawString(160, TxtDist * nx, "µáªÝ", Cr);
		else if (S.TSwitch[2] == 2)DrawString(160, TxtDist * nx, "Wv", Cr);
		else if (S.TSwitch[2] == 3){ DrawString(160, TxtDist * nx, "vC[ì", Cr); }
		else { DrawString(160, TxtDist * nx, "Rs[^ì", Cr); }

		nx++;

		DrawString(30, TxtDist * nx, "ó¯g", Cr);
		if (S.TSwitch[3] == 0)DrawString(160, TxtDist * nx, "j[g", Oran);
		else if (S.TSwitch[3] == 1) { DrawString(160, TxtDist * nx, "Oû", Cr); }
		else if (S.TSwitch[3] == 2) { DrawString(160, TxtDist * nx, "ãû", Cr); }
		else if (S.TSwitch[3] == 3) { DrawString(160, TxtDist * nx, "Èµ", Cr); }

		nx++;

		DrawString(30, TxtDist * nx, "K[h", Cr);
		if (S.TSwitch[4] == 0)DrawString(160, TxtDist * nx, "µÈ¢", Oran);
		else if (S.TSwitch[4] == 1) { DrawString(160, TxtDist * nx, "·é", Cr); }
		else if (S.TSwitch[4] == 2) { DrawString(160, TxtDist * nx, "_", Cr); }

		nx++;

		DrawString(30, TxtDist * nx, "JE^[", Cr);
		if (S.T_Counter == 0)DrawString(160, TxtDist * nx, "Êí", Oran);
		else if (S.T_Counter == 1) { DrawString(160, TxtDist * nx, "·é", Cr); }

		nx++;

		DrawString(30, TxtDist * nx, "°²¯", Cr);
		if (S.T_ThrowReject == 0)DrawString(160, TxtDist * nx, "µÈ¢", Oran);
		else if (S.T_ThrowReject == 1) { DrawString(160, TxtDist * nx, "·é", Cr); }

		nx++;

		DrawString(30, TxtDist * nx, "Ýè\¦", Cr);
		if (S.TSwitch[5] == 0)DrawString(160, TxtDist * nx, "SÄ", Oran);
		else if (S.TSwitch[5] == 1) { DrawString(160, TxtDist * nx, "l", Cr); }
		else if (S.TSwitch[5] == 2) { DrawString(160, TxtDist * nx, "L[", Cr); }
		else if (S.TSwitch[5] == 3) { DrawString(160, TxtDist * nx, "µÈ¢", Cr); }

		nx++;
		DrawString(30, TxtDist * nx, "»è", Cr);
		if (S.TSwitch[6] == 0)DrawString(160, TxtDist * nx, "Èµ", Oran);
		else if (S.TSwitch[6] == 1) { DrawString(160, TxtDist * nx, "òç¢", Cr); }
		else if (S.TSwitch[6] == 2) { DrawString(160, TxtDist * nx, "U", Cr); }
		else if (S.TSwitch[6] == 3) { DrawString(160, TxtDist * nx, "¼û", Cr); }

		nx++;
		DrawString(30, TxtDist * nx, "Ä¶û@", Cr);
		if (S.TSwitch[8] == 0)DrawString(160, TxtDist * nx, "Êí", Oran);
		else if (S.TSwitch[8] == 1) { DrawString(160, TxtDist * nx, "A±", Cr); }
		else if (S.TSwitch[8] == 2) { DrawString(160, TxtDist * nx, "oT", Cr); }
		
}

void CommandList()
{
	/**************
	* wi
	**************/
	{
		BackGraphT();
	}

	if (!S.TraningFlg){
		if (P_BInput(2) == 1)ComList = 0;
	}
	else{
		if (P1_BInput(2) == 1)ComList = 0;
	}
	


	// `æ
	int tx = 29;
	P[0] = P1, P[1] = P2;
	for (int i = 0; i < 2; i++){

		if (i == 1)tx += SCREEN_W/2;

		if (P[i].Name == EIN)
		{
			DrawString(tx, TxtDist * 1, "¨ +b", Cr);
			DrawString(tx, TxtDist * 2, "ciÝuãc óÂj", Cr);
			DrawString(tx, TxtDist * 3, "«_¨ +`oraorbióÂj", Cr);
			DrawString(tx, TxtDist * 4, "«^© +`ora", Cr);
			DrawString(tx - 19, TxtDist * 5, "Q[WZ", Cr);
			DrawString(tx, TxtDist * 6, "«_¨ +ci 0.5 j", Cr);
			DrawString(tx, TxtDist * 7, "«^© +bi 1 óÂ j", Cr);
			DrawString(tx, TxtDist * 8, "«« +ci 1 Ýu j", Cr);
			DrawString(tx, TxtDist * 9, "`+a+bi 3 j", Cr);
		}
		else if (P[i].Name == CORNEL)
		{
			DrawString(tx, TxtDist * 1, "¨ +a", Cr);
			DrawString(tx, TxtDist * 2, "_ +b", Cr);
			DrawString(tx, TxtDist * 3, "ciqbgãc óÂj", Cr);	
			DrawString(tx, TxtDist * 4, "«_¨ +`ora", Cr);
			DrawString(tx, TxtDist * 5, "¨«_ +`ora", Cr);
			DrawString(tx, TxtDist * 6, "«^© +`ora", Cr);
			DrawString(tx - 19, TxtDist * 7, "Q[WZ", Cr);
			DrawString(tx, TxtDist * 8, "«^© +ci 0.5 j", Cr);
			DrawString(tx, TxtDist * 9, "«_¨ +bi 1 j", Cr);
			DrawString(tx, TxtDist * 10, "¨«_ +bi 1 óÂ j", Cr);
			DrawString(tx, TxtDist * 11, "«^© +bi 2 j", Cr);
			DrawString(tx, TxtDist * 12, "`+a+bi 3 j", Cr);
		}
		else if (P[i].Name == BOUNCER)
		{
			DrawString(tx, TxtDist * 1, "¨ +a", Cr);
			DrawString(tx, TxtDist * 2, "¨ +biz[hÂj", Cr);
			DrawString(tx, TxtDist * 3, "c", Cr);
			DrawString(tx, TxtDist * 4, "«_¨ +`ora", Cr);
			DrawString(tx, TxtDist * 5, "«^© +`ora", Cr);
			DrawString(tx - 19, TxtDist * 6, "Q[WZ", Cr);
			DrawString(tx, TxtDist * 7, "«_¨ +bi 0.5 j", Cr);
			DrawString(tx, TxtDist * 8, "«^© +bi 0.5 j", Cr);
			DrawString(tx, TxtDist * 9, "`+a+bi 1 j", Cr);
			DrawString(tx, TxtDist * 10, "«« +ci 3 j", Cr);
		}
		else if (P[i].Name == HELIOS){
			DrawString(tx, TxtDist * 1, "¨ +`", Cr);
			DrawString(tx, TxtDist * 2, "¨ +a", Cr);
			DrawString(tx, TxtDist * 3, "ciz[hÂj", Cr);		
			DrawString(tx, TxtDist * 4, "«_¨ +`oraorc", Cr);
			DrawString(tx, TxtDist * 5, "¨«_ +`oraorc", Cr);
			DrawString(tx, TxtDist * 6, "«^© +`oraorcióÂj", Cr);
			DrawString(tx - 19, TxtDist * 7, "Q[WZ", Cr);
			DrawString(tx, TxtDist * 8, "«_¨ +bi 1 j", Cr);
			DrawString(tx, TxtDist * 9, "«^© +bi 1 j", Cr);
			DrawString(tx, TxtDist * 10, "«« +ci 1 j", Cr);
			DrawString(tx, TxtDist * 11, "`+a+bi 3 j", Cr);
		}
		else if (P[i].Name == HYDE){
			DrawString(tx, TxtDist * 1, "cióÂj", Cr);
			DrawString(tx - 19, TxtDist * 2, "Q[WZ", Cr);
			DrawString(tx, TxtDist * 3, "«_¨ +bi 1 j", Cr);
		}
		else if (P[i].Name == SYUICHI) {
			DrawString(tx, TxtDist * 1, "¨ +`", Cr);
			DrawString(tx, TxtDist * 2, "¨ +a", Cr);
			DrawString(tx, TxtDist * 3, "¨ +b", Cr);
			DrawString(tx, TxtDist * 4, "_ +b", Cr);
			DrawString(tx, TxtDist * 5, "ciz[hÂj", Cr);
			DrawString(tx, TxtDist * 6, "«_¨ +`ora", Cr);
			DrawString(tx, TxtDist * 7, "«^© +`ora", Cr);
			DrawString(tx - 19, TxtDist * 8, "Q[WZ", Cr);
			DrawString(tx, TxtDist * 9, "«_¨ +bi 1 j", Cr);
			DrawString(tx, TxtDist * 10, "«^© +bi 1 j", Cr);
			DrawString(tx, TxtDist * 11, "`+a+bi 3 j", Cr);
		}
	}
	
	
	//«Eü
	DrawLine(SCREEN_W / 2 - 1, 20, SCREEN_W / 2 - 1, SCREEN_H - 20, Cr);

	tx = 29 + (SCREEN_W/2);
}


// gpwi
void BackGraphT()
{
	// `æuh[hðAt@uhi§¾jÉ·é
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 158);
	// ¼§¾É
	DrawBox(0, 20, SCREEN_W, SCREEN_H - 20, GetColor(0, 0, 0), true);
	// `æuh[hðm[uhÉ·é
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// ãºü
	DrawLine(0, 19, SCREEN_W, 19, Cr);
	DrawLine(0, SCREEN_H - 19, SCREEN_W, SCREEN_H - 19, Cr);

}

void BackGraph()
{
	// `æuh[hðAt@uhi§¾jÉ·é
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 158);
	// ¼§¾É
	DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(0, 0, 0), true);
	// `æuh[hðm[uhÉ·é
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void raw_set()
{
	noChangeRow = true;
}

void Load_Pause(){
	if (Load_1 == 0){

		// æÇÝÝ
		//int cursor = LoadGraph("ob/cursor.png");
		LoadDivGraph("ob/key.png", 10, 5, 2, 20, 20, gK);
		LoadDivGraph("ob/button.png", 4, 4, 1, 20, 20, gB);
		int gP = LoadGraph("ob/fontP.png");

		//Load_1 = 1;
	}
}