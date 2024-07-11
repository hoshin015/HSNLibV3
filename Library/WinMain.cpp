#include <Windows.h>
#include <crtdbg.h>
#include "Framework.h"

//--------------------------------------------------------------
//  WinMain
//--------------------------------------------------------------
int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,		// �C���X�^���X�n���h���F���̃\�t�g�E�F�A��ӂ̒l
	_In_opt_ HINSTANCE hPrevInsance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(23350);	// �����Ŏw�肵���񐔃��������m�ۂ��ꂽ��v���O�������~�߂�

	Framework& framework = Framework::Instance();
	// ������
	if (framework.Initialize(hInstance))
	{
		// �X�V
		framework.Update();
	}

	// �I��
	framework.Finalize();

	return 0;
}