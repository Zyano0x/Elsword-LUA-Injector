#pragma once

namespace Base
{
	inline HMODULE g_hModule;
	inline uintptr_t dwGameBase;
	inline BOOL Detached = FALSE;

	class GameData
	{
	public:
		// CHEATS
		bool bDUMP = false;
		bool bINJECT = false;
		bool bAUTO_INJECT = true;

		explicit GameData();
		~GameData() noexcept = default;
		GameData(GameData const&) = delete;
		GameData(GameData&&) = delete;
		GameData& operator=(GameData const&) = delete;
		GameData& operator=(GameData&&) = delete;

		bool GetBytecode(const char* FileName, char** Buffer, size_t* Size);
		uintptr_t ResolvePTRS(uintptr_t Ptr, std::vector<unsigned int> Offsets);
	};

	class GameVariables
	{
	public:
		// DX9 & ImGui
		int g_GamePid;
		std::string g_GameExcPath;
		std::string g_FilePathName;
		std::string g_FileName;
		std::string g_Status = "Idle";

		// MENU RELATED
		bool m_ShowMenu = true;
		bool m_DetachDll = false;
		bool m_ShowConsole = false;

		explicit GameVariables();
		~GameVariables() noexcept = default;
		GameVariables(GameVariables const&) = delete;
		GameVariables(GameVariables&&) = delete;
		GameVariables& operator=(GameVariables const&) = delete;
		GameVariables& operator=(GameVariables) = delete;

		void Initialize();
	};

	inline std::unique_ptr<GameVariables> g_GameVariables;
	inline std::unique_ptr<GameData> g_GameData;
}