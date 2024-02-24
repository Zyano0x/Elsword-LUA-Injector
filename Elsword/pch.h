// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define DEVELOPER 0
#define HOOKCALL __fastcall
#define Errorf(fmt, ...) fprintf(stderr, "\n[Error at %s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define Detour(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourAttach((LPVOID*)&original, (LPVOID)hook), DetourTransactionCommit())
#define Restore(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourDetach((LPVOID*)&original, (LPVOID)hook), DetourTransactionCommit())

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include <detours.h>
#include <lua.hpp>

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include "ImGuiFileDialog.h"
#include "ImGuiFileDialogConfig.h"

#include "Signature.h"
#include "Debug.h"
#include "Hooking.h"
#include "DX9.h"
#include "Game.h"
#include "Menu.h"

#endif //PCH_H
