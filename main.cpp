
#include "main.h"

void RenderGamepadUI() {
    // GUI 창 설정
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Gamepad Tester", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

    // [콘트롤러리스트박스]
    ImGui::Text("Current Controller");
    const char* controllers[] = { "Controller 0", "Controller 1", "Controller 2", "Controller 3" };
    ImGui::PushItemWidth(225);
    ImGui::Combo("##Combo", &g_SelectedIdx, controllers, IM_ARRAYSIZE(controllers));
    ImGui::PopItemWidth();

    ImGui::Text("Device: %s", (g_SelectedIdx != -1 && g_Gamepad.connected) ? controllers[g_SelectedIdx] : "");
    ImGui::Separator();

    // 실시간 INPUT 감지
    if (g_SelectedIdx != -1) {
        ZeroMemory(&g_Gamepad.state, sizeof(XINPUT_STATE));
        if (XInputGetState(g_SelectedIdx, &g_Gamepad.state) == ERROR_SUCCESS) {
            g_Gamepad.connected = true;
        }
        else {
            g_Gamepad.connected = false;
        }
    }

    // [L-STICK & R-STICK] (사각형 70x70, 원 반지름 2px)
    auto DrawStick = [](const char* label, float thumbX, float thumbY) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        float sz = 70.0f;
        draw_list->AddRect(p, ImVec2(p.x + sz, p.y + sz), IM_COL32_WHITE);

        float posX = p.x + (sz / 2.0f) + (thumbX / 32768.0f) * (sz / 2.0f);
        float posY = p.y + (sz / 2.0f) - (thumbY / 32768.0f) * (sz / 2.0f);
        draw_list->AddCircle(ImVec2(posX, posY), 2.0f, IM_COL32(255, 255, 0, 255), 0, 2.0f);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + sz + 5);
        ImGui::Text("%s", label);
        ImGui::Text("X:%.0f Y:%.0f", thumbX, thumbY);
        };

    ImGui::Columns(2, nullptr, false);
    DrawStick("L-STICK", g_Gamepad.state.Gamepad.sThumbLX, g_Gamepad.state.Gamepad.sThumbLY);
    ImGui::NextColumn();
    DrawStick("R-STICK", g_Gamepad.state.Gamepad.sThumbRX, g_Gamepad.state.Gamepad.sThumbRY);
    ImGui::Columns(1);

    // [TRIGGER] (70x28, 진행 바 형태)
    auto DrawTrigger = [](const char* label, BYTE value) {
        float fraction = value / 255.0f;
        ImGui::Text("%s", label);
        ImGui::ProgressBar(fraction, ImVec2(70, 28), std::to_string(value).c_str());
        };

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
    DrawTrigger("LT", g_Gamepad.state.Gamepad.bLeftTrigger);
    ImGui::SameLine();
    DrawTrigger("RT", g_Gamepad.state.Gamepad.bRightTrigger);

    // [MOTOR TEST]
    ImGui::SetCursorPos(ImVec2(150, 150));
    ImGui::BeginGroup();
    ImGui::Text("MOTOR TEST");
    ImGui::Checkbox("Impulse Trigger", &g_ImpulseEnabled);
    ImGui::Checkbox("Left (X to trigger)", &g_LeftVibEnabled);
    ImGui::Checkbox("Right (B to trigger)", &g_RightVibEnabled);
    ImGui::EndGroup();

    // 진동 실행 로직
    if (g_SelectedIdx != -1 && g_Gamepad.connected) {
        XINPUT_VIBRATION vib = { 0, 0 };
        if (g_LeftVibEnabled && (g_Gamepad.state.Gamepad.wButtons & XINPUT_GAMEPAD_X)) vib.wLeftMotorSpeed = 65535;
        if (g_RightVibEnabled && (g_Gamepad.state.Gamepad.wButtons & XINPUT_GAMEPAD_B)) vib.wRightMotorSpeed = 65535;
        XInputSetState(g_SelectedIdx, &vib);
    }

    ImGui::End();
}

// 메인 진입점
int main(int, char**) {
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"GamepadTester", nullptr };
    RegisterClassExW(&wc);
    HWND hwnd = CreateWindowW(wc.lpszClassName, L"Gamepad Tester - XInput", WS_OVERLAPPEDWINDOW, 100, 100, 600, 500, nullptr, nullptr, wc.hInstance, nullptr);

    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // ImGui Init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // 메인 루프
    bool done = false;
    while (!done) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) done = true;
        }
        if (done) break;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        RenderGamepadUI();

        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.1f, 0.1f, 0.1f, 1.00f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // VSync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// DX11 기기 생성 및 관리 도우미 함수들 (Win32 표준 규격)
bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK) return false;

    CreateRenderTarget();
    return true;
}

void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    switch (msg) {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_KEYMENU) return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}