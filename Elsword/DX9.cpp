#include "pch.h"

tEndScene			oEndScene;
tluaL_loadbuffer	oluaL_loadbuffer;
tlua_pcall			olua_pcall;

namespace Base
{
	void* pDeviceTable[D3DDEV9_LEN];

	LRESULT APIENTRY D3D9Window::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP)
			g_D3D9Window->WmKeys[wParam] = uMsg;

		if (uMsg == WM_KEYDOWN)
		{
			switch (wParam)
			{
			case VK_INSERT:
				g_GameVariables->m_ShowMenu = !g_GameVariables->m_ShowMenu;
				break;
			case VK_END:
				g_GameVariables->m_DetachDll = true;
				break;
			case VK_DELETE:
				g_GameVariables->m_ShowConsole = !g_GameVariables->m_ShowConsole;

				switch (g_GameVariables->m_ShowConsole)
				{
				case(true):
					ShowWindow(g_Console->g_hWnd, SW_SHOW);
					break;
				case(false):
					ShowWindow(g_Console->g_hWnd, SW_HIDE);
					break;
				}
				break;
			case VK_HOME:
				g_GameData->bDUMP = !g_GameData->bDUMP;
				break;
			}
		}

		if (g_D3D9Window->b_ImGuiInit && g_GameVariables->m_ShowMenu && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
			return TRUE;

		return CallWindowProc((WNDPROC)g_D3D9Window->m_OldWndProc, hwnd, uMsg, wParam, lParam);
	}

	HRESULT APIENTRY D3D9Window::MJEndScene(IDirect3DDevice9* pDevice)
	{
		g_D3D9Window->Overlay(pDevice);

		if (g_GameVariables->m_DetachDll)
			g_Hooks->Unhook();

		return oEndScene(pDevice);
	}

	static int FileCount = 1;
	INT HOOKCALL hkLuaLLoadBuffer(lua_State* State, const char* Buff, size_t Size, const char* Name)
	{
		if (g_GameData->bDUMP)
		{
			std::string DumpPath = g_GameVariables->g_GameExcPath + "\\Dump\\";
			if (!std::filesystem::exists(DumpPath.c_str())) std::filesystem::create_directories(DumpPath.c_str());

			std::ofstream outfile(DumpPath + std::to_string(FileCount) + ".luac", std::ofstream::binary);
			outfile.write(Buff, Size);
			outfile.close();
			FileCount++;

			g_GameVariables->g_Status = "Dumping";
		}

		if (g_GameData->bINJECT)
		{
			if (g_GameData->bINJECT) g_GameData->bINJECT = !g_GameData->bINJECT;

			char* Bytecode = 0;
			size_t Len = 0;
			g_GameData->GetBytecode(g_GameVariables->g_FilePathName.c_str(), &Bytecode, &Len);
			oluaL_loadbuffer(State, Bytecode, Len, Name) || olua_pcall(State, 0, 0, 0);

			g_GameVariables->g_Status = "Loaded " + g_GameVariables->g_FileName;
		}

		if (g_GameData->bAUTO_INJECT)
		{
			if (g_GameData->bAUTO_INJECT) g_GameData->bAUTO_INJECT = !g_GameData->bAUTO_INJECT;

			std::vector<std::string> FileNames;
			std::string FolderPath = g_GameVariables->g_GameExcPath + "\\Script\\Auto\\";
			
			if (!std::filesystem::exists(FolderPath.c_str())) std::filesystem::create_directories(FolderPath.c_str());

			for (const auto& entry : std::filesystem::directory_iterator(FolderPath))
			{
				if (entry.is_regular_file()) FileNames.push_back(entry.path().string());
			}

			for (const auto& FilePath : FileNames)
			{
				char* Bytecode = nullptr;
				size_t Len = 0;

				if (g_GameData->GetBytecode(FilePath.c_str(), &Bytecode, &Len))
					oluaL_loadbuffer(State, Bytecode, Len, Name) || olua_pcall(State, 0, 0, 0);
			}

			g_GameVariables->g_Status = "Auto Load Complete";
		}
#if DEVELOPER
		g_Console->printdbg(" [-] Lua State (LuaL_LoadBuffer): 0x%llX\n", g_Console->color.pink, State);
#endif
		return oluaL_loadbuffer(State, Buff, Size, Name);
	}

	bool D3D9Window::Start()
	{
		if (GetD3D9Device(reinterpret_cast<void**>(pDeviceTable), D3DDEV9_LEN))
		{
			/// <summary>
			/// Initialize all of our hooks
			/// First we start with our window hooks
			oEndScene = reinterpret_cast<tEndScene>(pDeviceTable[42]);
			Detour(oEndScene, MJEndScene);

			oluaL_loadbuffer	= reinterpret_cast<tluaL_loadbuffer>(Signature(LUAL_LOADBUFFER_PATTERN).GetPointer());
			olua_pcall			= reinterpret_cast<tlua_pcall>(Signature(LUA_PCALL_PATTERN).GetPointer());
#if DEVELOPER
			g_Console->printdbg("D3D9Window::Hook Initialized\n", g_Console->color.green);
			g_Console->printdbg(" [-] luaL_loadbuffer Address:	0x%llX\n", g_Console->color.pink, oluaL_loadbuffer);
			g_Console->printdbg(" [-] lua_pcall Address:		0x%llX\n", g_Console->color.pink, olua_pcall);
#endif
			Detour(oluaL_loadbuffer, hkLuaLLoadBuffer);

			return TRUE;
		}
#if DEVELOPER
		g_Console->printdbg("D3D9Window::Hook Failed to Initialize\n", g_Console->color.red);
#endif
		return FALSE;
	}

	BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
	{
		DWORD wndProcId = 0;
		GetWindowThreadProcessId(handle, &wndProcId);

		if (GetCurrentProcessId() != wndProcId)
			return TRUE;

		g_D3D9Window->m_GameWindowHandle = handle;
		return FALSE;
	}

	HWND GetProcessWindow()
	{
		g_D3D9Window->m_GameWindowHandle = (HWND)NULL;
		EnumWindows(EnumWindowsCallback, NULL);
		return g_D3D9Window->m_GameWindowHandle;
	}

	bool D3D9Window::GetD3D9Device(void** pTable, size_t Size)
	{
		if (!pTable)
			return false;

		Size *= sizeof(void*);

		IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

		if (!pD3D)
			return false;

		IDirect3DDevice9* pDummyDevice = NULL;

		D3DPRESENT_PARAMETERS d3dpp = {};
		d3dpp.Windowed = false;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = GetProcessWindow();

		HRESULT dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

		if (dummyDeviceCreated != S_OK)
		{
			d3dpp.Windowed = !d3dpp.Windowed;
			dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

			if (dummyDeviceCreated != S_OK)
			{
				pD3D->Release();
				return false;
			}
		}

		memcpy(pTable, *reinterpret_cast<void***>(pDummyDevice), Size);

		pDummyDevice->Release();
		pD3D->Release();
		return true;
	}

	bool D3D9Window::InitImGui(IDirect3DDevice9* pDevice)
	{
		if (!b_ImGuiInit)
		{
			m_OldWndProc = (WNDPROC)SetWindowLongPtr(m_GameWindowHandle, GWLP_WNDPROC, (LONG_PTR)WndProc);

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

			ImGui_ImplWin32_Init(m_GameWindowHandle);
			ImGui_ImplDX9_Init(pDevice);
			ImGui_ImplDX9_CreateDeviceObjects();

			b_ImGuiInit = true;
#if DEVELOPER
			g_Console->printdbg("D3D9Window::ImGUI Initialized\n", g_Console->color.green);
#endif
			return TRUE;
		}
		b_ImGuiInit = FALSE;
		return FALSE;
	}

	void D3D9Window::Overlay(IDirect3DDevice9* pDevice)
	{
		if (!b_ImGuiInit)
			InitImGui(pDevice);

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		g_Menu->Draw();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	void D3D9Window::Stop()
	{
		SetWindowLongPtr(m_GameWindowHandle, GWLP_WNDPROC, (LONG_PTR)m_OldWndProc);
		Detach();
	}

	void D3D9Window::Detach()
	{
		Restore(oEndScene, MJEndScene);
		Restore(oluaL_loadbuffer, hkLuaLLoadBuffer);
	}

	D3D9Window::~D3D9Window()
	{
		Stop();
	}
}