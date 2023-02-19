// basic.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "main.h"
#include "form/EditorForm.h"
#include <ShlObj_core.h>

#include "binding/JsEngine.h"
#include "ui_components/public_define.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 创建主线程
	MainThread thread;
	// 执行主线程循环
	thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);

	return 0;
}

void MainThread::Init()
{
	nbase::ThreadManager::RegisterThread(nim_comp::kThreadUI);

	// 获取资源路径，初始化全局参数
	std::wstring theme_dir = nbase::win32::GetCurrentModuleDirectory();

	ui::GlobalManager::Startup(theme_dir + L"resources\\", ui::CreateControlCallback(), false);

	ui::GlobalManager::EnableAutomation();

	JsEngine engine;
	engine.Init();
	engine.Excute("import {} from 'debug.js'", "<eval>");
	// engine.RunLoop();

	// 创建一个默认带有阴影的居中窗口
	// EditorForm* window = new EditorForm;
	// window->Create(NULL, EditorForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
	// window->ShowWindow();
	// window->CenterWindow();
}

void MainThread::Cleanup()
{
	ui::GlobalManager::Shutdown();
	SetThreadWasQuitProperly(true);
	nbase::ThreadManager::UnregisterThread();
}
