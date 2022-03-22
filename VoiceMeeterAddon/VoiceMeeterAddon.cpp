// VoiceMeeterAddon.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "VoiceMeeterAddon.h"
#include "VoiceMeeterRemote.h"
#include <stdio.h>
#include <shellapi.h>

#define WM_TRAY_DOUBLE_CLICK WM_USER + 1
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//static int audioSetting[8];
HINSTANCE vmlib;
HICON icon;
T_VBVMR_INTERFACE voicemeeter;
BOOL systemTray;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VOICEMEETERADDON, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VOICEMEETERADDON));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    if (voicemeeter.VBVMR_AudioCallbackUnregister != NULL) voicemeeter.VBVMR_AudioCallbackUnregister();
    if (voicemeeter.VBVMR_Logout != NULL) voicemeeter.VBVMR_Logout();
    FreeLibrary(vmlib);
    return (int) msg.wParam;
}

BOOL saveConfig(int* config, int size) {
    HANDLE configFile;
    char buf[64] = { 0, };

    if (size < sizeof(int) * 8) return false;

    // 파일 열기
    configFile = CreateFile(L"peach.cfg", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (configFile == INVALID_HANDLE_VALUE) return false;

    // 파일 쓰기
    sprintf_s(buf, "%d %d %d %d %d %d %d %d", config[0], config[1], config[2], config[3], config[4], config[5], config[6], config[7]);
    WriteFile(configFile, buf, sizeof(buf), NULL, NULL);

    CloseHandle(configFile);
    return true;
}

BOOL loadConfig(int* config, int size) {
    HANDLE configFile;
    char buf[64] = { 0, };

    if (size < sizeof(int) * 8) return false;

    // 파일 열기
    configFile = CreateFile(L"peach.cfg", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (configFile == INVALID_HANDLE_VALUE) return false;

    // 파일 읽기
    ReadFile(configFile, buf, sizeof(buf), NULL, NULL);
    sscanf_s(buf, "%d %d %d %d %d %d %d %d", config + 0, config + 1, config + 2, config + 3, config + 4, config + 5, config + 6, config + 7);

    CloseHandle(configFile);
    return true;
}

DWORD loadRegTray() {
    HKEY hkey;
    DWORD value, size = sizeof(DWORD);
    if (RegOpenKey(HKEY_CURRENT_USER, L"Software\\VoiceMeeterAddon\\peach", &hkey) != ERROR_SUCCESS) return 0xFF;
    if (RegGetValue(hkey, NULL, L"SystemTray", RRF_RT_DWORD | RRF_ZEROONFAILURE, NULL, &value, &size) != ERROR_SUCCESS) {
        RegCloseKey(hkey);
        return 0xFF;
    }
    RegCloseKey(hkey);
    return value;
}

void setRegTray(DWORD value) {
    HKEY hkey;
    // 키를 열어보고 없으면 생성
    if (RegOpenKey(HKEY_CURRENT_USER, L"Software\\VoiceMeeterAddon\\peach", &hkey) != ERROR_SUCCESS) RegCreateKey(HKEY_CURRENT_USER, L"Software\\VoiceMeeterAddon\\peach", &hkey);
    RegSetValueEx(hkey, L"SystemTray", 0, REG_DWORD, (LPBYTE)&value, sizeof(DWORD));
    RegCloseKey(hkey);
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); //IDI_VOICEMEETERADDON
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_VOICEMEETERADDON);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1)); //IDI_SMALL
    icon = wcex.hIcon;

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   //char a1[MAX_LOADSTRING], a2[MAX_LOADSTRING];
   //strcpy_s(a1, "VoiceMeeterAddonPeachShark");
   //strcpy_s(a2, "PeachShark");
   //HWND hWnd = CreateWindowA(a1, a2, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, 350, 255, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   if (systemTray == FALSE) ShowWindow(hWnd, nCmdShow);
   else ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);

   return TRUE;
}

long __stdcall audioCallback(void* lpUser, long nCommand, void* lpData, long nnn) {

    int* audst;
    float* in;
    float* inr;
    float* out;
    float* outr;
    VBVMR_LPT_AUDIOINFO info;
    VBVMR_LPT_AUDIOBUFFER buffer;

    switch (nCommand) {
    case VBVMR_CBCOMMAND_STARTING:
        info = (VBVMR_LPT_AUDIOINFO)lpData;
        break;
    case VBVMR_CBCOMMAND_ENDING:
        info = (VBVMR_LPT_AUDIOINFO)lpData;
        break;
    case VBVMR_CBCOMMAND_CHANGE:
        info = (VBVMR_LPT_AUDIOINFO)lpData;
        break;
    case VBVMR_CBCOMMAND_BUFFER_OUT:
        buffer = (VBVMR_LPT_AUDIOBUFFER)lpData;
        audst = (int*)lpUser;
        //audst = audioSetting;
        int t;
        /*for (int i = 0; i < buffer->audiobuffer_nbi; i++) {
            in = buffer->audiobuffer_r[i];
            out = buffer->audiobuffer_w[i];
            for (int j = 0; j < buffer->audiobuffer_nbs; j++) out[j] = in[j];
        }*/
        for (int i = 0; i < 4; i++) {
            // 그대로 통과
            if (audst == NULL) t = -1;
            else t = audst[i];
            if (t < -1 || t > 9) t = -1;
            if (buffer->audiobuffer_nbi == 16 && t >= 4) t = -1;
            if (t == -1) {
                in = buffer->audiobuffer_r[2 * i];
                inr = buffer->audiobuffer_r[2 * i + 1];
                out = buffer->audiobuffer_w[2 * i];
                outr = buffer->audiobuffer_w[2 * i + 1];
            } // 좌우 반전 없음
            else if (t % 2 == 0) {
                in = buffer->audiobuffer_r[8 * (t / 2)];
                inr = buffer->audiobuffer_r[8 * (t / 2) + 1];
                out = buffer->audiobuffer_w[2 * i];
                outr = buffer->audiobuffer_w[2 * i + 1];
            } // 좌우 반전 있음
            else {
                in = buffer->audiobuffer_r[8 * (t / 2)];
                inr = buffer->audiobuffer_r[8 * (t / 2) + 1];
                out = buffer->audiobuffer_w[2 * i + 1];
                outr = buffer->audiobuffer_w[2 * i];
            }
            for (int j = 0; j < buffer->audiobuffer_nbs; j++) { out[j] = in[j]; outr[j] = inr[j]; }
        }
        for (int i = 4; i < 8; i++) {
            if (buffer->audiobuffer_nbi == 16 && i > 4) break;
            if (audst == NULL) t = -1;
            else t = audst[i];
            if (t < -1 || t > 9) t = -1;
            if (buffer->audiobuffer_nbi == 16 && t >= 4) t = -1;
            if (t == -1) {
                in = buffer->audiobuffer_r[8 * (i - 3)];
                inr = buffer->audiobuffer_r[8 * (i - 3) + 1];
                out = buffer->audiobuffer_w[8 * (i - 3)];
                outr = buffer->audiobuffer_w[8 * (i - 3) + 1];
            } // 좌우 반전 없음
            else if (t % 2 == 0) {
                in = buffer->audiobuffer_r[8 * (t / 2)];
                inr = buffer->audiobuffer_r[8 * (t / 2) + 1];
                out = buffer->audiobuffer_w[8 * (i - 3)];
                outr = buffer->audiobuffer_w[8 * (i - 3) + 1];
            } // 좌우 반전 있음
            else {
                in = buffer->audiobuffer_r[8 * (t / 2)];
                inr = buffer->audiobuffer_r[8 * (t / 2) + 1];
                out = buffer->audiobuffer_w[8 * (i - 3) + 1];
                outr = buffer->audiobuffer_w[8 * (i - 3)];
            }
            for (int j = 0; j < buffer->audiobuffer_nbs; j++) { out[j] = in[j]; outr[j] = inr[j]; }
        }
        for (int i = 10; i < 16; i++) {
            in = buffer->audiobuffer_r[i];
            out = buffer->audiobuffer_w[i];
            for (int j = 0; j < buffer->audiobuffer_nbs; j++) out[j] = in[j];
        }
        if (buffer->audiobuffer_nbi == 16) break;
        for (int i = 18; i < 24; i++) {
            in = buffer->audiobuffer_r[i];
            out = buffer->audiobuffer_w[i];
            for (int j = 0; j < buffer->audiobuffer_nbs; j++) out[j] = in[j];
        }
        for (int i = 26; i < 32; i++) {
            in = buffer->audiobuffer_r[i];
            out = buffer->audiobuffer_w[i];
            for (int j = 0; j < buffer->audiobuffer_nbs; j++) out[j] = in[j];
        }
        for (int i = 34; i < 40; i++) {
            in = buffer->audiobuffer_r[i];
            out = buffer->audiobuffer_w[i];
            for (int j = 0; j < buffer->audiobuffer_nbs; j++) out[j] = in[j];
        }
        break;
    }

    return 0;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int audioSetting[8];
    static HWND rbutton[8][11];
    static NOTIFYICONDATA nid;
    static char str[256] = { 0, };
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            MENUITEMINFO menuItemInfo = { 0 };
            // 메뉴 선택을 구문 분석합니다:
            if (21000 <= wmId && wmId < 21200) {
                int i, j;
                i = wmId - 21000;
                if (i % 20 == 0) j = -1;
                else j = i % 10;
                i = i / 20;
                if (0 <= i && i < 8) audioSetting[i] = j;
                //sprintf_s(str, "%d %d     ", i, j);
                //InvalidateRect(hWnd, NULL, FALSE);
            }
            switch (wmId)
            {
            case 32773: // 시스템 트레이 메뉴
                menuItemInfo.cbSize = sizeof(MENUITEMINFO);
                menuItemInfo.fMask = MIIM_STATE;
                GetMenuItemInfo(GetMenu(hWnd), 32773, FALSE, &menuItemInfo);
                if (menuItemInfo.fState == MFS_CHECKED) {
                    CheckMenuItem(GetMenu(hWnd), 32773, MF_UNCHECKED);
                    setRegTray(0x00);
                }
                else if (menuItemInfo.fState == MFS_UNCHECKED) {
                    CheckMenuItem(GetMenu(hWnd), 32773, MF_CHECKED);
                    setRegTray(0x01);
                }
                //CheckMenuItem(GetMenu(hWnd), 32773, MF_CHECKED);
                break;
            case 32772: // 설정 저장 메뉴
                saveConfig(audioSetting, sizeof(audioSetting));
                break;
            case 32771: // 설정 불러오기 메뉴
                if (loadConfig(audioSetting, sizeof(audioSetting)) == false) break;
                //sprintf_s(str, "%d %d %d %d %d %d %d %d", audioSetting[0], audioSetting[1], audioSetting[2], audioSetting[3], audioSetting[4], audioSetting[5], audioSetting[6], audioSetting[7]);
                //InvalidateRect(hWnd, NULL, FALSE);
                for (int i = 0; i < 8; i++) CheckRadioButton(hWnd, 21000 + 20 * i, 21010 + 20 * i + 9, audioSetting[i] == -1 ? 21000 + 20 * i : 21010 + 20 * i + audioSetting[i]);
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE:
        {
            // audioSetting 초기화
            if(loadConfig(audioSetting, sizeof(audioSetting)) == false) for (int i = 0; i < 8; i++) audioSetting[i] = -1;
            for (int i = 0; i < 8; i++) if(audioSetting[i] < -1 || audioSetting[i] > 9) audioSetting[i] = -1;

            // 라디오 버튼 생성
            for (int i = 0; i < 8; i++) {
                rbutton[i][0] = CreateWindow(L"button", L"", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 60, 30 + 20 * i, 20, 20, hWnd, (HMENU) (21000 + 20 * i), hInst, NULL);
                for (int j = 0; j < 5; j++) {
                    rbutton[i][2 * j + 1] = CreateWindow(L"button", L"", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON ,  90 + 50 * j, 30 + 20 * i, 20, 20, hWnd, (HMENU) (21010 + 20 * i + 2 * j)    , hInst, NULL);
                    rbutton[i][2 * j + 2] = CreateWindow(L"button", L"", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON , 110 + 50 * j, 30 + 20 * i, 20, 20, hWnd, (HMENU) (21010 + 20 * i + 2 * j + 1), hInst, NULL);
                }
                //CheckRadioButton(hWnd, 21000 + 20 * i, 21010 + 20 * i + 9, 21000 + 20 * i);
                CheckRadioButton(hWnd, 21000 + 20 * i, 21010 + 20 * i + 9, audioSetting[i] == -1 ? 21000 + 20 * i : 21010 + 20 * i + audioSetting[i]);
            }

            // 레지스트리 읽기
            if (loadRegTray() != 0x00) {
                CheckMenuItem(GetMenu(hWnd), 32773, MF_CHECKED);
                systemTray = TRUE;
            }
            else systemTray = FALSE;

            // VoiceMeeter DLL 로드
            vmlib = LoadLibrary(L"C:\\Program Files (x86)\\VB\\Voicemeeter\\VoicemeeterRemote64.dll");
            //vmlib = LoadLibrary("VoicemeeterRemote64.dll"); //64비트
            //vmlib = LoadLibrary("VoicemeeterRemote.dll"); //32비트
            if (vmlib == NULL) {
                MessageBox(hWnd, L"VoicemeeterRemote64.dll 링크 실패", L"시작 실패", MB_ICONERROR | MB_OK);
                return -1;
            }

            // VoiceMeeterAPI 함수 가져오깅
            voicemeeter.VBVMR_Login = (T_VBVMR_Login)GetProcAddress(vmlib, "VBVMR_Login");
            voicemeeter.VBVMR_Logout = (T_VBVMR_Logout)GetProcAddress(vmlib, "VBVMR_Logout");
            if (voicemeeter.VBVMR_Login == NULL) { MessageBox(hWnd, L"실패", L"시작 실패", MB_ICONERROR | MB_OK); return -1; }
            if (voicemeeter.VBVMR_Logout == NULL) { MessageBox(hWnd, L"실패", L"시작 실패", MB_ICONERROR | MB_OK); return -1; }
            voicemeeter.VBVMR_IsParametersDirty = (T_VBVMR_IsParametersDirty)GetProcAddress(vmlib, "VBVMR_IsParametersDirty");
            if (voicemeeter.VBVMR_IsParametersDirty == NULL) { MessageBox(hWnd, L"실패", L"시작 실패", MB_ICONERROR | MB_OK); return -1; }
            voicemeeter.VBVMR_GetVoicemeeterType = (T_VBVMR_GetVoicemeeterType)GetProcAddress(vmlib, "VBVMR_GetVoicemeeterType");
            if (voicemeeter.VBVMR_GetVoicemeeterType == NULL) { MessageBox(hWnd, L"실패", L"시작 실패", MB_ICONERROR | MB_OK); return -1; }
            voicemeeter.VBVMR_AudioCallbackRegister = (T_VBVMR_AudioCallbackRegister)GetProcAddress(vmlib, "VBVMR_AudioCallbackRegister");
            voicemeeter.VBVMR_AudioCallbackStart = (T_VBVMR_AudioCallbackStart)GetProcAddress(vmlib, "VBVMR_AudioCallbackStart");
            voicemeeter.VBVMR_AudioCallbackStop = (T_VBVMR_AudioCallbackStop)GetProcAddress(vmlib, "VBVMR_AudioCallbackStop");
            voicemeeter.VBVMR_AudioCallbackUnregister = (T_VBVMR_AudioCallbackUnregister)GetProcAddress(vmlib, "VBVMR_AudioCallbackUnregister");
            if (voicemeeter.VBVMR_AudioCallbackRegister == NULL) { MessageBox(hWnd, L"실패", L"시작 실패", MB_ICONERROR | MB_OK); return -1; }
            if (voicemeeter.VBVMR_AudioCallbackStart == NULL) { MessageBox(hWnd, L"실패", L"시작 실패", MB_ICONERROR | MB_OK); return -1; }
            if (voicemeeter.VBVMR_AudioCallbackStop == NULL) { MessageBox(hWnd, L"실패", L"시작 실패", MB_ICONERROR | MB_OK); return -1; }
            if (voicemeeter.VBVMR_AudioCallbackUnregister == NULL) { MessageBox(hWnd, L"실패", L"시작 실패", MB_ICONERROR | MB_OK); return -1; }

            // VoiceMeeter Login
            if (voicemeeter.VBVMR_Login() < 0) {
                MessageBox(hWnd, L"Voicemeeter 로그인 실패", L"시작 실패", MB_ICONERROR | MB_OK);
                return -1;
            }

            // 뭔지 모름 없어도 될것 같기도 한데....
            voicemeeter.VBVMR_IsParametersDirty();
            
            // Voicemeeter 오디오 콜백 등록
            char aaaa[64] = "aaaa";
            if (voicemeeter.VBVMR_AudioCallbackRegister(VBVMR_AUDIOCALLBACK_OUT, audioCallback, audioSetting, aaaa) == 1) {
                MessageBox(hWnd, L"실패", L"시작 실패", MB_ICONERROR | MB_OK);
                return -1;
            }
            voicemeeter.VBVMR_AudioCallbackStart();

            // 트레이 아이콘
            ZeroMemory(&nid, sizeof(nid));
            nid.cbSize = sizeof(nid);
            nid.hWnd = hWnd;
            nid.uFlags = NIF_TIP | NIF_ICON | NIF_MESSAGE;
            nid.uCallbackMessage = WM_TRAY_DOUBLE_CLICK;
            nid.hIcon = icon;
            lstrcpy(nid.szTip, L"트레이아이콘");
            Shell_NotifyIcon(NIM_ADD, &nid);

            
        }
        break;
    case WM_TRAY_DOUBLE_CLICK:
        {
            if (lParam == WM_LBUTTONDBLCLK) {
                ShowWindow(hWnd, SW_SHOW);
                ShowWindow(hWnd, SW_RESTORE);
            }
        }
        break;
    case WM_SIZE:
        {
            if (wParam == SIZE_MINIMIZED) {
                ShowWindow(hWnd, SW_HIDE);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            TextOut(hdc, 20, 10, L"A1", 2);
            TextOut(hdc, 10, 30, L"ch 0,1", 6);
            TextOut(hdc, 10, 50, L"ch 2,3", 6);
            TextOut(hdc, 10, 70, L"ch 4,5", 6);
            TextOut(hdc, 10, 90, L"ch 6,7", 6);
            TextOut(hdc, 20, 110, L"A2", 2);
            TextOut(hdc, 20, 130, L"A3", 2);
            TextOut(hdc, 20, 150, L"B1", 2);
            TextOut(hdc, 20, 170, L"B2", 2);

            TextOut(hdc, 62, 10, L"X", 1);
            TextOut(hdc, 100, 10, L"A1", 2);
            TextOut(hdc, 150, 10, L"A2", 2);
            TextOut(hdc, 200, 10, L"A3", 2);
            TextOut(hdc, 250, 10, L"B1", 2);
            TextOut(hdc, 300, 10, L"B2", 2);

            //sprintf_s(str, "%d", loadRegTray());
            //TextOut(hdc, 220, 10, str, strlen(str));

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam;
            
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            SetBkMode(hdcStatic, TRANSPARENT);
            
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
