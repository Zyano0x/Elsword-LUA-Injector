#pragma once

#define LUAL_LOADBUFFER_PATTERN		"48 83 EC ? 48 C7 44 24 ? ? ? ? ? E8 ? ? ? ? 48 83 C4 ? C3 CC CC CC CC CC CC CC CC CC 40 57"
#define LUA_PCALL_PATTERN			"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 59 ? 41 8B F0"
#define D3DDEV9_LEN					119
#define MAX_VIRTUALKEYS				0x100

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef HRESULT(APIENTRY* tEndScene)(IDirect3DDevice9*);
extern tEndScene oEndScene;

typedef int(HOOKCALL* tluaL_loadbuffer)(lua_State* L, const char* Buff, size_t Size, const char* Name);
extern tluaL_loadbuffer oluaL_loadbuffer;

typedef int(HOOKCALL* tlua_pcall)(lua_State* L, int nArgs, int nResults, int errfunc);
extern tlua_pcall olua_pcall;

DWORD WINAPI _Initialize(LPVOID lpThreadParameter);
DWORD WINAPI _Deallocate(LPVOID lpThreadParameter);

namespace Base
{
	class D3D9Window
	{
	public:

		explicit D3D9Window() = default;
		~D3D9Window() noexcept;
		D3D9Window(D3D9Window const&) = delete;
		D3D9Window(D3D9Window&&) = delete;
		D3D9Window& operator=(D3D9Window const&) = delete;
		D3D9Window& operator=(D3D9Window&&) = delete;

		HWND m_GameWindowHandle;
		WNDPROC m_OldWndProc;
		UINT WmKeys[0xFF];

		bool b_ImGuiInit = FALSE;

		bool Start();
		void Stop();
		void Overlay(IDirect3DDevice9* pDevice);

		bool GetD3D9Device(void** pTable, size_t Size);

		bool InitImGui(IDirect3DDevice9* pDevice);
		void Detach();
		static LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static HRESULT APIENTRY MJEndScene(IDirect3DDevice9* pDevice);
	};
	inline std::unique_ptr<D3D9Window> g_D3D9Window;
}