#pragma once
#include <DxLib.h>

/*
---------------------------
 モード毎データ
---------------------------

それぞれ切り替えて使用するので、ゲーム中で動かすのは一つのみ

*/

class ModeData
{
private:
	boolean load_1s = false;

public:
	// 毎フレーム実行する
	virtual void Update() = 0;

	virtual void Draw() = 0;

	virtual void All()
	{
		Update();
		Draw();
	}

	// 読み込む
	virtual void Load()
	{
		if (!load_1s) {
			Initialize();
			load_1s = true;
		}
		Load_Reload();
	}

	// モード変更時のロード
	// ここに記述
	virtual void Load_Reload() = 0;

	// 1回のみのロード(初期化)
	// ここに記述
	virtual void Initialize() = 0;

	// 解放
	virtual void Release() = 0;
};
