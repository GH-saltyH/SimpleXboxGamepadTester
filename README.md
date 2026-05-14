# 🎮 Advanced Gamepad Tester (XInput & WinRT)

Windows 11 환경에서 게임패드의 모든 입력 신호를 실시간으로 시각화하고, 모터 및 임펄스 트리거(Impulse Trigger)의 성능을 정밀하게 테스트할 수 있는 C++ 기반 GUI 애플리케이션입니다.

## ■ Key Features

● **Real-time Input Visualization**:
  * L-Stick / R-Stick의 좌표값 출력 및 클릭(L3/R3) 피드백.
  * A, B, X, Y 및 D-Pad, LB, RB 버튼의 입력을 시각적 하이라이트로 표시.
  * LT, RT 트리거의 정밀한 입력 수치 및 게이지 바 출력.

● **Advanced Vibration Testing**:
  * **Impulse Trigger Test**: Xbox One/Series 컨트롤러 전용 트리거 개별 진동 테스트 (WinRT API 적용).
  * **Motor Feedback**: X/B 버튼과 연동된 좌/우 메인 모터 독립 제어.

● **Controller Management**: PC에 연결된 최대 4개의 Xbox 컨트롤러 리스트업 및 실시간 전환.

## ■ Tech Stack

● **Language**: C++ 17 이상.
● **Graphics API**: DirectX 11. 
● **GUI Library**: [Dear ImGui](https://github.com/ocornut/imgui).
● **Input API**: 
  * XInput (Standard Input) 
  * Windows Runtime (Windows.Gaming.Input) for Impulse Triggers 
● **IDE**: Visual Studio 2022 

## ■ Development Environment Setup

프로젝트를 정상적으로 빌드하기 위해 다음 설정이 필요합니다.

1. **C++ Standard**: 프로젝트 속성 -> C/C++ -> 언어 -> C++ 언어 표준을 **ISO C++17 표준 (/std:c++17)** 이상으로 설정합니다.
2. **Library Linking**: 링커 속성에서 다음 라이브러리를 추가해야 합니다.
   * `d3d11.lib`, `d3dcompiler.lib`, `xinput.lib`, `windowsapp.lib`
3. **ImGui Setup**: `imgui.cpp`, `imgui_impl_win32.cpp`, `imgui_impl_dx11.cpp` 등 관련 소스 파일이 프로젝트에 포함되어 있어야 합니다.

## ■ Layout Guide

이 프로젝트는 다음과 같은 레이아웃 구조를 가집니다.
* **Top**: 컨트롤러 선택 및 장치 정보 출력.
* **Center**: 진동(Motor) 테스트 설정 및 트리거 피드백.
* **Bottom**: 아날로그 스틱, 십자키(D-Pad), 액션 버튼 배치.

## ■ Setup Summary (셋업 요약)

* **Input Logic**: XInput 구조체의 비트마스킹을 통해 버튼 눌림을 판단하며, 스틱의 경우 `-32768 ~ 32767` 범위를 GUI 좌표로 정규화하여 출력합니다.
* **Vibration Logic**: `winrt::init_apartment()`를 통한 WinRT 환경 초기화 후, `GamepadVibration` 구조체를 이용해 트리거와 메인 모터에 독립적인 전압 신호를 전달합니다.

---
**Author**: [Your Name/GitHub ID]  
**Last Updated**: 2026-05-14
