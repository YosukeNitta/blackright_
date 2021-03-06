#include "pch.h"

static IPDATA Ip;	// グローバル?

static BYTE Data[2];	// *2なのは1P2P分
static int UDPNetHandle;        // ネットワークハンドル
static int port[5];
static int Port;
static boolean nCheck;

static const int ip_x = 4;
static const int ip_y = 3;
static int ip[ip_x][ip_y];	// IP([4]に数字,[3]が行)
//============
// TCP
static int NetHandle, LostHandle;    // ネットワークハンドル
static int DataLength;            // 受信データ量保存用変数
//============

static const int HOST = 1;		// ホスト接続前
static const int CLIENT = 2;	// クライアント接続前
static const int IP_SET = 3;
static const int PORT_SET = 4;

static int sPoint;	// 画面の進み度合い
// 0.項目選択
static int yPoint;	// 項目の位置
static int xPoint;	//
static boolean load_f = false;


static void Input();
static void Draw();

// メイン関数
int Network()
{
	Input();

	//////////////////
	// ホスト
	if (sPoint == HOST){
		/*
		// 接続してくるのを待つ状態にする
		PreparationListenNetWork(Port);

		// 接続してくるかＥＳＣキーが押されるまでループ
		NetHandle = -1;
		while (!ProcessMessage() && CheckHitKey(KEY_INPUT_K) == 0)
		{
		// 新しい接続があったらそのネットワークハンドルを得る
		NetHandle = GetNewAcceptNetWork();
		if (NetHandle != -1) break;
		}

		// 接続されていたら次に進む
		if (NetHandle != -1)
		{
		// 接続の受付を終了する
		StopListenNetWork();

		// 接続してきたマシンのＩＰアドレスを得る
		GetNetWorkIP(NetHandle, &Ip);

		// データが送られて来るまで待つ
		while (!ProcessMessage())
		{
		// 取得していない受信データ量が０以外のときはループから抜ける
		if (GetNetWorkDataLength(NetHandle) != 0) break;
		}

		// データ受信
		DataLength = GetNetWorkDataLength(NetHandle);    // データの量を取得
		NetWorkRecv(NetHandle, StrBuf, DataLength);    // データをバッファに取得

		// 受信したデータを描画
		DrawString(0, 200, StrBuf, GetColor(255, 255, 255));

		// 受信成功のデータを送信
		NetWorkSend(NetHandle, "繋がった", 17);

		// ここから通信開始
		Get_Network(true);
		Network_Setting(1, Ip, Port, NetHandle);

		TraningSwitch(false);
		AISwitch(false);
		BattleMode(0);
		ModeChange(GameSelect);


		}
		*/

		int cnwr_ret = CheckNetWorkRecvUDP(UDPNetHandle);
		// データを受信
		if (cnwr_ret && cnwr_ret != -1){

			// 先にデータ受け取り
			//NetWorkRecvUDP(UDPNetHandle, &Ip, NULL, Data, sizeof(Data), FALSE);
			NetWorkRecvUDP(UDPNetHandle, NULL, NULL, Data, sizeof(Data), FALSE);
			// 送り返す
			Data[0] = 2;
			NetWorkSendUDP(UDPNetHandle, Ip, Port, Data, sizeof(Data));

			nCheck = true;

			// 接続成功したら //
			if (Data[0] == 2 && Data[1] == 2)
			{
				Get_Network(true);
				Network_Setting(1, Ip, Port, UDPNetHandle);

				TraningSwitch(false);
				AISwitch(false);
				BattleMode(0);
				ModeChange(GameSelect);
			}
		}
		else {
			DrawFormatString(20, SCREEN_H - 60, Cr, "false");
		}

	}
	// クライアント
	else if (sPoint == CLIENT){
		/*
		// 通信を確立
		NetHandle = ConnectNetWork(Ip, Port);

		// 確立が成功した場合のみ中の処理をする
		if (NetHandle != -1)
		{
		// データ送信
		NetWorkSend(NetHandle, "繋がった", 17);

		// データがくるのを待つ
		while (!ProcessMessage() && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
		{
		// 取得していない受信データ量を得る
		DataLength = GetNetWorkDataLength(NetHandle);

		// 取得してない受信データ量が０じゃない場合はループを抜ける
		if (DataLength != 0) break;
		}

		// データ受信
		NetWorkRecv(NetHandle, StrBuf, DataLength);    // データをバッファに取得

		// 受信したデータを描画
		DrawString(0, 200, StrBuf, GetColor(255, 255, 255));

		// ここから通信開始
		Get_Network(true);
		Network_Setting(2, Ip, Port, NetHandle);

		TraningSwitch(false);
		AISwitch(false);
		BattleMode(0);
		ModeChange(GameSelect);
		}
		*/

		Data[1] = 1;
		if (Data[0] > 0)Data[1] = 2;
		// 文字列の送信
		NetWorkSendUDP(UDPNetHandle, Ip, Port, Data, sizeof(Data));
		// 接続
		int cnwr_ret = CheckNetWorkRecvUDP(UDPNetHandle);
		if (cnwr_ret && cnwr_ret != -1){

			// データを受信
			NetWorkRecvUDP(UDPNetHandle, NULL, NULL, Data, sizeof(Data), FALSE);
			nCheck = true;

			if (Data[0] == 2 && Data[1] == 2)
			{
				Get_Network(true);
				Network_Setting(2, Ip, Port, UDPNetHandle);

				TraningSwitch(false);
				AISwitch(false);
				BattleMode(0);
				ModeChange(GameSelect);
			}
		}
		else {
			DrawFormatString(20, SCREEN_H - 60, Cr, "false");
		}

	}
	//////////////////
	Draw();	// 描画

	// 終了
	if ((sPoint == 0) && (yPoint == 4) && (P1_BInput(1) == 1 || P1_BInput(2) == 1 || P1_BInput(3) == 1)){
		ModeChange(GameMenu);
	}

	return 0;
}

//  ネットワーク読み込み
int Load_Network()
{
	if (!load_f){
		Ip.d1 = 0;
		Ip.d2 = 0;
		Ip.d3 = 0;
		Ip.d4 = 0;


		for (int i = 0; i < 5; i++){
			port[i] = 0;
		}
		for (int i = 0; i < ip_x; i++){
			for (int j = 0; j < ip_y; j++){
				ip[i][j] = 0;
			}
		}
		port[1] = 7;
		port[2] = 5;

		load_f = true;
	}

	Port = 0;
	// UDP
	UDPNetHandle = 0;
	// TCP
	DataLength = 0;

	sPoint = 0;
	yPoint = 0;
	xPoint = 0;
	nCheck = false;



	for (int i = 0; i < sizeof(Data); i++){
		Data[i] = 0;
	}

	Main_GetNetwork(false);
	NB_Reset();

	return 0;
}


void Input(){
	// 選択画面？
	if (sPoint == 0){
		if (P1_BInput(102) == 1){
			yPoint++;
			SEStart(36);
		}
		else if (P1_BInput(108) == 1){
			yPoint--;
			SEStart(36);
		}

		if (yPoint < 0)yPoint = 4;
		else if (yPoint > 4)yPoint = 0;

		// ホストに
		if (yPoint == 0 && (P1_BInput(1) == 1 || P1_BInput(3) == 1)){
			// ポート作成
			Ip.d1 = 0, Ip.d2 = 0, Ip.d3 = 0, Ip.d4 = 0;
			// UDP
			// Ip作成
			Ip.d1 = (ip[0][0] * 100) + (ip[0][1] * 10) + ip[0][2];
			Ip.d2 = (ip[1][0] * 100) + (ip[1][1] * 10) + ip[1][2];
			Ip.d3 = (ip[2][0] * 100) + (ip[2][1] * 10) + ip[2][2];
			Ip.d4 = (ip[3][0] * 100) + (ip[3][1] * 10) + ip[3][2];

			int p = (port[0] * 10000) + (port[1] * 1000) + (port[2] * 100) + (port[3] * 10) + port[4];
			Port = p;
			UDPNetHandle = MakeUDPSocket(Port);
			sPoint = HOST;
			Data[0] = 0;
			Data[1] = 0;
		}
		// クライアントに
		else if (yPoint == 1 && (P1_BInput(1) == 1 || P1_BInput(3) == 1)){
			// ポート作成
			Ip.d1 = 0, Ip.d2 = 0, Ip.d3 = 0, Ip.d4 = 0;
			// Ip作成
			Ip.d1 = (ip[0][0] * 100) + (ip[0][1] * 10) + ip[0][2];
			Ip.d2 = (ip[1][0] * 100) + (ip[1][1] * 10) + ip[1][2];
			Ip.d3 = (ip[2][0] * 100) + (ip[2][1] * 10) + ip[2][2];
			Ip.d4 = (ip[3][0] * 100) + (ip[3][1] * 10) + ip[3][2];

			// ポート作成
			int p = (port[0] * 10000) + (port[1] * 1000) + (port[2] * 100) + (port[3] * 10) + port[4];
			Port = p;
			UDPNetHandle = MakeUDPSocket(Port);
			sPoint = CLIENT;
			yPoint = 0;
			Data[0] = 0;
			Data[1] = 0;
		}
		// IPに
		else if (yPoint == 2 && (P1_BInput(1) == 1 || P1_BInput(3) == 1)){
			sPoint = IP_SET;
			yPoint = 0;
			xPoint = 0;
		}
		// ポートに
		else if (yPoint == 3 && (P1_BInput(1) == 1 || P1_BInput(3) == 1)){
			sPoint = PORT_SET;
			yPoint = 0;
			xPoint = 0;
		}

		// 終了に
		if (P1_BInput(2) == 2){
			sPoint = 0;
			yPoint = 4;
		}
	}
	else if (sPoint == HOST){
		// 選択へ
		if (P1_BInput(2) == 2){
			sPoint = 0;
		}
	}
	else if (sPoint == CLIENT){
		// 選択へ
		if (P1_BInput(2) == 2){
			sPoint = 0;
			yPoint = 1;
		}
	}
	// IPセット画面 //
	else if (sPoint == IP_SET){
		if (P1_BInput(104) == 1){
			xPoint--;
			SEStart(36);
		}
		else if (P1_BInput(106) == 1){
			xPoint++;
			SEStart(36);
		}

		// xPointの位置
		int po = 0, po2 = xPoint;
		if (xPoint > 8)po = 3, po2 = xPoint - 9;
		else if (xPoint > 5)po = 2, po2 = xPoint - 6;
		else if (xPoint > 2)po = 1, po2 = xPoint - 3;

		if (xPoint < 0)xPoint = 11;
		else if (xPoint > 11)xPoint = 0;
		if (P1_BInput(102) == 1){
			ip[po][po2]--;
			SEStart(36);
		}
		else if (P1_BInput(108) == 1){
			ip[po][po2]++;
			SEStart(36);
		}
		// 数値を0〜9にする
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 3; j++){
				if (ip[i][j] < 0)ip[i][j] = 9;
				else if (ip[i][j] > 9)ip[i][j] = 0;
			}
		}

		// 選択へ
		if (P1_BInput(2) == 2){
			sPoint = 0;
			yPoint = 2;
			int kekka = 0;
			for (int i = 0; i < ip_x; i++){
				kekka = (ip[i][0] * 100) + (ip[i][1] * 10) + ip[i][2];
				if (kekka > 255)ip[i][0] = 2, ip[i][1] = 5, ip[i][2] = 5;
			}
		}
	}
	else if (sPoint == PORT_SET){
		if (P1_BInput(104) == 1){
			xPoint--;
			SEStart(36);
		}
		else if (P1_BInput(106) == 1){
			xPoint++;
			SEStart(36);
		}
		if (xPoint < 0)xPoint = 4;
		else if (xPoint > 4)xPoint = 0;
		if (P1_BInput(102) == 1){
			port[xPoint]--;
			SEStart(36);
		}
		else if (P1_BInput(108) == 1){
			port[xPoint]++;
			SEStart(36);
		}
		// 数値を0〜9にする
		for (int i = 0; i < 5; i++){
			if (port[i] < 0)port[i] = 9;
			else if (port[i] > 9)port[i] = 0;
		}

		// 選択へ
		if (P1_BInput(2) == 2){
			sPoint = 0;
			yPoint = 3;
		}
	}
}

void Draw(){
	// ポインター
	DrawBox(0, yPoint * 20, 20, (yPoint + 1) * 20, Cr, true);

	// テスト用位置表示
	DrawFormatString(420, 240, Cr, "S..%d", sPoint);
	DrawFormatString(420, 260, Cr, "X..%d", xPoint);
	DrawFormatString(420, 280, Cr, "Y..%d", yPoint);
	DrawFormatString(420, 300, Cr, "IP..%d", ip[3][2]);

	if (sPoint == 0){
		DrawString(20, 20 * 0, "ホスト", Cr);
		DrawString(20, 20 * 1, "クライアント", Cr);
		// IP数値
		for (int i = 0; i < ip_x; i++){
			for (int j = 0; j < ip_y; j++){
				DrawFormatString(20 + (j * 10) + (i * 40), 20 * 2, Cr, "%d", ip[i][j]);
			}
			if (i != 3)DrawString(20 + (i * 40) + 30, 20 * 2, ".", Cr);
		}
		// ポート数値
		for (int i = 0; i < 5; i++){
			DrawFormatString(20 + (i * 10), 20 * 3, Cr, "%d", port[i]);
		}
		DrawString(20, 20 * 4, "終了", Cr);

		// 項目の説明
		switch (yPoint){
		case 0:
			DrawString(80, 20 * 6, "自分が接続を待つ", Cr);
			break;
		case 1:
			DrawString(80, 20 * 6, "自分が接続する", Cr);
			break;
		case 2:
			DrawString(80, 20 * 6, "相手のグローバルIPアドレス", Cr);
			break;
		case 3:
			DrawString(80, 20 * 6, "接続するポート番号", Cr);
			break;
		case 4:
			DrawString(80, 20 * 6, "終了する", Cr);
			break;
		}
	}
	else if (sPoint == HOST){
		DrawString(20, 20 * 0, "ホスト", Cr);
		if (!nCheck)DrawString(20, 20 * 1, "待機中", Cr);
		else{ DrawString(20, 20 * 1, "接続", Cr); }
		DrawFormatString(20, 20 * 2, Cr, "%d", Port);
		DrawFormatString(20, 20 * 3, Cr, "クライアント %d.%d.%d.%d", Ip.d1, Ip.d2, Ip.d3, Ip.d4);
		DrawFormatString(20, SCREEN_H - 40, Cr, "%d.%d", Data[0], Data[1]);
	}
	else if (sPoint == CLIENT){
		DrawString(20, 20 * 0, "クライアント", Cr);
		if (!nCheck)DrawString(20, 20 * 1, "待機中", Cr);
		else{ DrawString(20, 20 * 1, "接続", Cr); }
		// ポートも一緒に表示
		DrawFormatString(20, 20 * 3, Cr, "%d.%d.%d.%d:%d", Ip.d1, Ip.d2, Ip.d3, Ip.d4, Port);
		DrawFormatString(20, SCREEN_H - 40, Cr, "%d.%d", Data[0], Data[1]);
	}
	else if (sPoint == IP_SET){
		// IP数値
		for (int n = 0; n < ip_x; n++){
			for (int i = 0; i < ip_y; i++){
				if ((n * 3) + i == xPoint)DrawFormatString(20 + (i * 10) + (n * 40), 20 * 0, Oran, "%d", ip[n][i]);
				else{ DrawFormatString(20 + (i * 10) + (n * 40), 20 * 0, Cr, "%d", ip[n][i]); }
			}
			if (n != 3)DrawString(20 + (n * 40) + 30, 20 * 0, ".", Cr);
		}
	}
	else if (sPoint == PORT_SET){
		// ポート数値
		for (int i = 0; i < 5; i++){
			if (i == xPoint)DrawFormatString(20 + (i * 10), 20 * 0, Oran, "%d", port[i]);
			else{ DrawFormatString(20 + (i * 10), 20 * 0, Cr, "%d", port[i]); }
		}
	}
}