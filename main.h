#pragma once

#include <Windows.h>
#include <Xinput.h>
#include <vector>
#include <string>
#include <iostream>

// 진동 테스트를 위한 헤더
#include <winrt/windows.gaming.input.h>
#include <winrt/windows.foundation.collections.h>

// ImGui 
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>

// Library linking
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "d3d11.lib")

using namespace winrt::Windows::Gaming::Input;

// 컨트롤러 상태 구조체
struct GamepadState {
	bool connected = false;
	XINPUT_STATE state;
	std::string deviceName = "";
};

// 전역 변수
GamepadState g_Gamepad;
int g_SelectedIdx = -1;
bool g_ImpulseEnabled = false;
bool g_LeftVibEnabled = false;
bool g_RightVibEnabled = false;

static ID3D11Device*						g_pd3dDevice = nullptr;
static ID3D11DeviceContext*			g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*					g_pSwapChain = nullptr;
static ID3D11RenderTargetView*	g_mainRenderTargetView = nullptr;

// 전방선언
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
