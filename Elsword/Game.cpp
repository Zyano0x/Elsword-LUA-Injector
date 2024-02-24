#include "pch.h"

namespace Base
{
	typedef struct _WRITER_DATA
	{
		size_t* len;
		char** data;
	} WRITER_DATA;

	GameVariables::GameVariables()
	{
#if DEVELOPER
		g_Console->printdbg("GameVariables::Initialized\n", g_Console->color.green);
#endif
		return;
	}

	// Get Process Window Information
	bool SelectedWindow = FALSE;
	void GameVariables::Initialize()
	{
		while (!SelectedWindow)
		{
			DWORD ForegroundWindowProcessID;
			GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
			if (GetCurrentProcessId() == ForegroundWindowProcessID)
			{
				g_GamePid = GetCurrentProcessId();

				char ExcPath[MAX_PATH];
				GetModuleFileNameA(NULL, ExcPath, sizeof(ExcPath));
				std::string::size_type pos = std::string(ExcPath).find_last_of("\\/");
				g_GameExcPath = std::string(ExcPath).substr(0, pos);
#if DEVELOPER
				g_Console->printdbg("GameData::Init - Process Window Info Established\n", g_Console->color.green);
				g_Console->printdbg(" [-] Game PID: %d\n", g_Console->color.pink, g_GamePid);
				g_Console->printdbg(" [-] Game EXE Path: %s\n", g_Console->color.pink, g_GameExcPath.c_str());
#endif
				SelectedWindow = TRUE;
			}
		}
	}

	GameData::GameData()
	{
		dwGameBase = (uintptr_t)GetModuleHandle(0);
#if DEVELOPER
		g_Console->printdbg("GameData::Initialized\n", g_Console->color.green);
		g_Console->printdbg(" [-] Game Module: 0x%llX\n", g_Console->color.pink, dwGameBase);
		g_Console->printdbg(" [-] Handle Module: 0x%llX\n", g_Console->color.pink, g_hModule);
#endif
		return;
	}

	static int Writer(lua_State* L, const void* p, size_t size, void* ud)
	{
		WRITER_DATA* bd = (WRITER_DATA*)ud;

		char* newData = (char*)realloc(*(bd->data), (*(bd->len)) + size);
		if (newData)
		{
			memcpy(newData + (*(bd->len)), p, size);
			*(bd->data) = newData;
			*(bd->len) += size;
		}
		else
		{
			free(newData);
			return 1;
		}
		return 0;
	}

	bool GameData::GetBytecode(const char* FileName, char** Buffer, size_t* Size)
	{
		lua_State* L = luaL_newstate();

		int LoadFile = luaL_loadfile(L, FileName);
		if (LoadFile != 0)
		{
			Errorf("Failed To Loading Lua File: %s\n", lua_tostring(L, -1));
			lua_close(L);
			return false;
		}

		WRITER_DATA Data{ Size, Buffer };
		if (lua_dump(L, Writer, &Data) != 0)
		{
			Errorf("Failed To Dump Lua Chunk\n");
			lua_close(L);
			return false;
		}

		lua_close(L);
		return true;
	}

	/// <summary>
	/// Resolves Address via Base Address & Offsets
	/// </summary>
	/// <param name="Ptr">BaseAddress</param>
	/// <param name="Offsets">Array of offsets</param>
	/// <returns>Address</returns>
	uintptr_t GameData::ResolvePTRS(uintptr_t Ptr, std::vector<unsigned int> Offsets)
	{
		/// <USAGE EXAMPLE>
		/// uintptr_t PLAYER_OOBJECT = NULL;
		/// uintptr_t BASE_ADDR = 0x69420
		/// std::vector<unsigned int> ptr_Chain = { 0x69. 0x420, 0x666 };
		/// PLAYER_OBJECT = g_GameData->ResolverPTRS(BASE_ADDR, ptr_Chain);

		uintptr_t addr = Ptr;
		for (unsigned int i = 0; i < Offsets.size(); i++)
		{
			addr = *(uintptr_t*)addr;
			if (addr == NULL)
				break;

			addr += Offsets[i];
		}
		return addr;
	}
}