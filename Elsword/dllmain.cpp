#include "pch.h"

using namespace Base;

DWORD WINAPI _Initialize(LPVOID lpThreadParameter)
{
	g_hModule = (HMODULE)lpThreadParameter;
	g_Console = std::make_unique<Console>();

	/// Pre-Processor check, really useful tool for toggling things per build.
	//  Perhaps we don't want to include console output. We can declare that simply with a 1 or 0 before compiling
#if DEVELOPER
	g_Console->InitializeConsole("[ELSWORD] ZX LOADER - DEBUG CONSOLE");
	g_Console->printdbg("[ELSWORD] ZX LOADER::Initializing . . .\n", g_Console->color.green);
#endif

	g_GameVariables = std::make_unique<GameVariables>();
	g_GameData = std::make_unique<GameData>();
	g_D3D9Window = std::make_unique<D3D9Window>();
	g_Hooks = std::make_unique<Hooking>();
	g_Menu = std::make_unique<Menu>();

	// With everything declared and in scope , we can begin creating our cheat
	g_Hooks->Hook();

#if DEVELOPER
	g_Console->printdbg("Main::Initialized\n", g_Console->color.green);
#endif
	return TRUE;
}

DWORD WINAPI _Deallocate(LPVOID lpThreadParameter)
{
	if (!Detached)
	{
		Detached = TRUE;
		FreeLibraryAndExitThread(g_hModule, TRUE);
	}
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);
		CreateThread(0, 0, _Initialize, hModule, 0, 0);
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		if (!Detached)
			CreateThread(0, 0, _Deallocate, hModule, 0, 0);
		break;
	}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	default:
		break;
	}
	return TRUE;
}