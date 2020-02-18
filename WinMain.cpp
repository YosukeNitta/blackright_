// �i���Q�[���̃v���O����
//
#include <stdio.h>	// �t�@�C������ɕK�v�ȃw�b�_�t�@�C��
#include <string.h>	// �����񑀍�ɕK�v�ȃw�b�_�t�@�C��

#include "pch.h"
/////////

#include "Fps.h"
#include "WindowInfo.h"

#include "MainSystem.h"

// vector���g��
#include <vector>
using namespace std;

#pragma warning(disable : 4996)	// fscanf�̃G���[�\��������

#pragma region	���[�J���ϐ�
static int sw;			// �X�N�V��
static int window;			// �E�B���h�E

static boolean screenMode;		// �X�N���[���̃��[�h
static boolean drawFPS;			// FPS�\��
static int modeCount = 0;	// ���[�h�ɂȂ��Ă���̌o�߃t���[��

static boolean wait;	// �M�ё����҂��āA���[�h�𓮂����Ȃ�
//static int stop = 0;

#pragma endregion

// �e�퍀��
static Fps fps;

// ���C���V�X�e���N���X
//static MainSystem sy;

//�����֐��錾��--------------------------------------------
void Screen_Setting();
void Game_Processing();
void Game_Draw();
void Game_Update();
void Game_End();
void Replay_DrawData();


//�v���O������----------------------------------------------
#pragma region ���C���֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//��ʐݒ�
	MainSystem::Instance();
	Screen_Setting();

	// �c�w���C�u���������������A�G���[���N�����璼���ɏI��
	if (DxLib_Init() == -1){ return -1; }

	// �����keyconfig�ǂݍ���
	P1_BConfig();

	{
		// �񓯊��ǂݍ��ݐݒ�ɕύXz 
		//SetUseASyncLoadFlag(TRUE);

		// �d���t�@�C����ǂݍ���
		GetDef();
		LoadAllFile();
		
		// �����ǂݍ��ݐݒ�ɕύX
		//SetUseASyncLoadFlag(FALSE);
	}
	
	// �V�[����ǉ�����
	//ModeManager::Instance().AddMode("Title", new TitleMode);
	//	�V�[���Ǘ��N���X�ŊJ�n�V�[����o�^
	//ModeManager::Instance().StartMode("Title");
	//	���s
	//ModeManager::Instance().Update();
	
	// ctrl + K + U or C

	// �`���𗠉�ʂɃZ�b�g
	SetDrawScreen(DX_SCREEN_BACK);

	///-------------
	//
	// ���C�����[�v
	//
	//--------------
	// while( ���b�Z�[�W����(�K�v�炵��), ESC���͊m�F, �Q�[�����[�h��0����Ȃ� )
	while (ProcessMessage() == 0 && (CheckHitKey(KEY_INPUT_ESCAPE) == 0))
	{
		//----------
		// �K�{����
		//----------
		{
			// �Q�[���i�s���m�[�}���Ȃ��ʃN���A
			if (ClsDrawScreen() != 0)break;	// ��ʏ�����

			// FPS�X�V
			fps.Update();	// OK
			// ����
			Game_Processing();
		}
		//------
		// �`��
		//------
		{
			//update
			//draw
			//filp
			//wait
			
			Game_Draw();
			//Replay_DrawData();
			fps.Wait();		// 1�t���[���o�߂���܂őҋ@
		}
		//------------
		// ��ʂ̍X�V
		//------------
		{
			Game_Update();
			
		}
		ScreenFlip();	// �\��

		// ���[�v�I��
		if (MainSystem::Instance().CheckEnd())
			break;
	}
	// ���C�����[�v�I��

	// �I������
	Game_End();

	return 0;				// �\�t�g�̏I�� 

}//main�I��
#pragma endregion

/////////////////////////////////////////////////////////////////////////////�����܂�


// ��ʐݒ�
void Screen_Setting()
{
	//
	// �t�H���g�ǂݍ���
	//

	// ini�ł̐ݒ��ǂݍ���                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
	// ���߂̖��O�A�ϐ����A�Ȃ��ꍇ�̒l�A�ꏊ
	screenMode = GetPrivateProfileInt("Config", "windowmode", 1, "./config.ini");	// ./���K�v(�J�����g�t�H���_)
	drawFPS = GetPrivateProfileInt("Config", "drawfps", 1, "./config.ini");

	// �^�C�g����ύX
	SetMainWindowText("blackright");

	// �t���X�N���[�����[�h���̉𑜓x���[�h��4:3���g�p���郂�[�h�ɐݒ�
	SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_DESKTOP);

	// �t���X�N���[�����[�h���̉�ʂ̊g�僂�[�h���ŋߓ_���[�h�ɐݒ�
	SetFullScreenScalingMode(DX_FSSCALINGMODE_NEAREST);

	//�E�B���h�E���[�h��
	ChangeWindowMode(screenMode);

	// ��ʃ��[�h�̐ݒ�A�T�C�Y�𑪂�
	SetGraphMode(WindowInfo::width, WindowInfo::height, 16);

	//SetChangeScreenModeGraphicsSystemResetFlag(FALSE);	// �O���t�B�b�N�n���h���̃��Z�b�g�h�~
														// �E�B���h�E�T�C�Y�ύX���ɏ����Ȃ��悤�ɂ���

	// ��A�N�e�B�u��Ԃł�����(�������Ȃ��ƔM�тł��Ȃ�)
	SetAlwaysRunFlag(true);

	// ��ʃT�C�Y��ύX�\��
	SetWindowSizeChangeEnableFlag(true);

	//= �ϐ��ݒ� ========
	//wait = false;	// �ΐ푊���҂�
}

// ���́EFPS�X�V����
#pragma region
void Game_Processing()
{
	
	// ����
	GetInput();	// �R���g���[���������Ă��邩�m�F

	// �q�����Ă�Ȃ�
	if (Connect_CheckCn()){
		Delay_Check();
		Connect_SetGameMode();
	}

	// �{�^�����̓`�F�b�N�A�X���[���͂Ȃ�
	// 0�`-n�͑��삷��
	P1_BCheck();
	P2_BCheck();

	// �X�N�V���̊m�F
	if (CheckHitKey(KEY_INPUT_F1) != 0)sw++;
	else { sw = 0; }

	//Release���[�h���̂ݕ\���������Ƃ��Ȃ񂩂� #ifndef _DEBUG ��
	//#ifdef _DEBUG
	// ���̊m�F
	if (CheckHitKey(KEY_INPUT_F2) != 0)StopSound(1);
	else{ StopSound(0); }
	//#endif

	// ���[�h������
	// ���[�h�I�����Ă���Ȃ���s
	if (GetASyncLoadNum() == 0){
		MainSystem::Instance().MainMove();
	}
}
#pragma endregion

void Game_Draw()
{
	// �ǂݍ��ݐ���\��
	if (GetASyncLoadNum() != 0){
		int amari = GetASyncLoadNum() % 5;	// 5���X�V
		// �񓯊��ǂݍ��݂̐���`��
		DrawFormatString(0, SCREEN_H - 20, Cr, "�c��t�@�C�� %d", GetASyncLoadNum() - amari);
	}

	//sy.MainDraw();	// FPS����`��
	if (drawFPS) { fps.Draw(SCREEN_W - 80, 0); }
}

/// <summary>
/// ���̍X�V
/// </summary>
void Game_Update()
{
	// �l�b�g���[�N�ɐڑ���
	if (Connect_CheckCn()){
		DrawString(0, SCREEN_H - 20, "�ڑ���", Cr);
		Delay_Draw();	// �f�B���C�\��
		DrawFormatString(0, SCREEN_H - 60, Cr, "side..%d", Connect_CheckSide());
		DrawFormatString(0, SCREEN_H - 40, Cr, "port..%d", Connect_CheckPort());
	}
	// �X�N���[���V���b�g
	if (sw == 1){
		ScreenShot();
	}
}

// �I������
void Game_End()
{
	BGMStart(0);
	// �l�b�g���[�N�ؒf
	if (Connect_CheckCn())End_Network();
	// ���v���C�I��
	if (Replay_Mode(-1) > 0)Replay_End();
	// �g�����ݒ�ۑ�
	Tlist_Save();
	DxLib_End();				// �c�w���C�u�����g�p�̏I������
}

void Replay_DrawData()
{
	DrawFormatString(SCREEN_W - 100, SCREEN_H - 20, GetColor(10,10,255), "���..%d", Replay_Mode(-1));
}