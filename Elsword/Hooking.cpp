#include "pch.h"

namespace Base
{
	Hooking::Hooking()
	{
		return;
	}

	Hooking::~Hooking()
	{
		return;
	}

	/// <summary>
	/// Initializing hook. Everything begins here
	/// - First we obtain information on our target process (Process ID, Window Name, Window Size & Location
	/// - Then we create a transparent window on top of the target process window
	/// </summary>
	void Hooking::Hook()
	{
		g_GameVariables->Initialize();
		g_D3D9Window->Start();
#if DEVELOPER
		g_Console->printdbg("Hooking::Initialized\n", g_Console->color.green);
#endif
		return;
	}

	/// <summary>
	///
	void Hooking::Unhook()
	{
		g_D3D9Window->Stop();
#if DEVELOPER
		g_Console->DestroyConsole();
#endif
		CreateThread(0, 0, _Deallocate, g_hModule, 0, 0);
		return;
	}
}