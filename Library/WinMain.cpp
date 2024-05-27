#include <Windows.h>
#include <crtdbg.h>
#include "Framework.h"

//--------------------------------------------------------------
//  WinMain
//--------------------------------------------------------------
int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,		// インスタンスハンドル：このソフトウェア一意の値
	_In_opt_ HINSTANCE hPrevInsance,
	_In_ LPWSTR lpCmdLine, 
	_In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(237);	// 引数で指定した回数メモリが確保されたらプログラムを止める

	Framework& framework = Framework::Instance();
	// 初期化
	if (framework.Initialize(hInstance))
	{
		// 更新
		framework.Update();
	}

	// 終了
	framework.Finalize();

	return 0;
}