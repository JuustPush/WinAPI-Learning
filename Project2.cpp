//#pragma comment(linker,"\"/manifestdependency:type='win32' \
//name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
//processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <Windows.h>
#include <string>
#include "resource.h"
#include "SoftwareDefinition.h"
#include "SoftwareColors.h"
#include "SoftWareCommuncation.h"


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) 
{

	fontReacnagle = CreateFontA(
		80, 30, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, 
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DECORATIVE, "MyFont"); //Создание шрифта
	WNDCLASS SoftwareMainClass=NewWindowClass((HBRUSH)COLOR_WINDOW,LoadCursor(NULL,IDC_HAND),hInst,LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON1)),L"MainWndClass",SoftwareMainProcedure);
	if (!RegisterClassW(&SoftwareMainClass)) { return -1; }
	MSG SoftwareMainMessege = { 0 };
	CreateWindow(L"MainWndClass", L"First C++ window", WS_OVERLAPPEDWINDOW | WS_VISIBLE , 600, 200, 520, 400, NULL, NULL, NULL, NULL);
	while (GetMessage(&SoftwareMainMessege,NULL,NULL,NULL))
	{
		TranslateMessage(&SoftwareMainMessege);
		DispatchMessage(&SoftwareMainMessege);
	}
	TerminateThread(readThread, 0);
	return 0;
}

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure)
{
	WNDCLASS NWC = { 0 };
	NWC.hIcon = Icon;
	NWC.hCursor = Cursor;
	NWC.hInstance = hInst;
	NWC.lpszClassName = Name;
	NWC.hbrBackground = BGColor;
	NWC.lpfnWndProc = Procedure;
	return NWC;
}

void ExitSoftware(void)
{
	isConnected = false;
	isThreading = false;
	CloseHandle(connectedPort);
	CloseHandle(readThread);
	ExitThread(0);
	PostQuitMessage(0);
}

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_COMMAND:
		if ((wp >= ComSelectIndex) && (wp < ComSelectIndex + comPortAmount)) {
			selectedPort = wp - ComSelectIndex;
			SetWindowStatus("PORT: " + std::to_string(selectedPort));
			SerialUpdate();
			break;
		}
			switch (wp){
		case onConnectRequest:
			ConnectRequest();
			break;
		case onMenuAction1:
			MessageBox(hWnd, L"Menu 1 was clicked", L"Menu worked", MB_OK);
			break;
		case onMenuAction2:
			MessageBox(hWnd, L"Menu 2 was clicked", L"Menu worked", MB_OK);
			break;

		case onClearField:
			SetWindowTextA(hEditControl, "");
			break;
		case onReadColor:
			colorR = GetDlgItemInt(hWnd, DigitIndexColorR, 0, 0);
			colorG = GetDlgItemInt(hWnd, DigitIndexColorG, 0, 0);
			colorB = GetDlgItemInt(hWnd, DigitIndexColorB, 0, 0);



			brushRectangle= CreateSolidBrush(
				RGB(colorR,colorG,colorB));
			fontColor = RGB(255 - colorR, 255 - colorG, 255 - colorB);

			RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);

			break;
		case onFileLoad:
			if (GetSaveFileNameA(&ofn)) { LoadData(FileName); }
			break;
		case onFileSave:
			if (GetSaveFileNameA(&ofn)) { SaveData(FileName); }
			break;
		case onSerialRefresh:
			SerialUpdate();
			break;
		case onExitSoftWare:
			PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
		BeginPaint(hWnd, &ps);

		//FillRect(ps.hdc, &windowRectangle, brushRectangle);
		GradientRect(ps.hdc, &windowRectangle, Color(200, 20, 20), Color(250, 100, 200),Color(20,250,20),Color(0,20,20));

		SetBkMode(ps.hdc, TRANSPARENT);
		SetTextColor(ps.hdc, fontColor);
		SelectObject(ps.hdc, fontReacnagle);
		DrawTextA(ps.hdc, "Rect text", -1, &windowRectangle, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOCLIP);
		EndPaint(hWnd, &ps);
		break;
	case WM_CREATE:
		MainWndMenus(hWnd);
		MainWndAddWidgets(hWnd);
		SetFileParams(hWnd);
		SerialUpdate();
		readThread = CreateThread(NULL, 0, SerialRead,NULL, 0, NULL);
		break;
	case WM_DESTROY:
		ExitSoftware();
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
}
void MainWndMenus(HWND hWnd)
{
	HMENU RootMenu = CreateMenu();
	HMENU SubMenu = CreateMenu();
	HMENU SubActionMenu = CreateMenu();

	ComPortSubMenu = CreateMenu();
	ComPortListMenu = CreateMenu();

	AppendMenu(SubActionMenu, MF_STRING, onMenuAction1, L"Menu 1");
	AppendMenu(SubActionMenu, MF_STRING, onMenuAction2, L"Menu 2");
	AppendMenu(SubMenu, MF_POPUP, (UINT_PTR)SubActionMenu, L"Action");
	AppendMenu(SubMenu, MF_STRING, onClearField, L"Clear");
	AppendMenu(SubMenu, MF_STRING, onFileSave, L"Save");
	AppendMenu(SubMenu, MF_STRING, onFileLoad, L"Load");
	AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(SubMenu, MF_STRING, onExitSoftWare, L"Exit");
	AppendMenu(ComPortSubMenu, MF_STRING, onConnectRequest, L"Connect");
	AppendMenu(ComPortSubMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(ComPortSubMenu, MF_STRING, onSerialRefresh, L"Refresh Ports");
	AppendMenu(ComPortSubMenu, MF_POPUP, (UINT_PTR)ComPortListMenu, L"Selected port");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"File");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)ComPortSubMenu, L"Connection");
	SetMenu(hWnd, RootMenu);
}

void MainWndAddWidgets(HWND hWnd)
{
	//hStaticControl = CreateWindowA("static", "WINDOWS GRADIENTS", WS_VISIBLE | WS_CHILD | ES_CENTER, 200, 5, 220, 30, hWnd, NULL, NULL, NULL);
	//hEditControl = CreateWindowA("edit", "This is EDIT control", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 5, 40, 470, 120, hWnd, NULL, NULL, NULL);
	windowRectangle = { 5 + 470, 40,5,40 + 120 };

	CreateWindowA("edit", "0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 5, 170, 100, 30, hWnd, (HMENU)DigitIndexColorR, NULL, NULL);
	CreateWindowA("edit", "0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 110, 170, 100, 30, hWnd, (HMENU)DigitIndexColorG, NULL, NULL);
	CreateWindowA("edit", "0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 215, 170, 100, 30, hWnd, (HMENU)DigitIndexColorB, NULL, NULL);

	CreateWindowA("button", "Clear", WS_VISIBLE | WS_CHILD | ES_CENTER, 5, 5, 120, 30, hWnd, (HMENU)onClearField, NULL, NULL);
	CreateWindowA("button", "Set color", WS_VISIBLE | WS_CHILD | ES_CENTER, 130, 5, 65, 30, hWnd, (HMENU)onReadColor, NULL, NULL);
}

void SaveData(LPCSTR path)
{
	HANDLE FileToSave = CreateFileA(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	int SaveLenght = GetWindowTextLength(hEditControl)+1;
	char* data = new char[SaveLenght];
	SaveLenght = GetWindowTextA(hEditControl, data, SaveLenght);
	DWORD bytesIterated;
	WriteFile(FileToSave, data, SaveLenght, &bytesIterated, NULL);
	CloseHandle(FileToSave);
	delete[] data;
}
void LoadData(LPCSTR path)
{
	HANDLE FileToRead = CreateFileA(path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD bytesIterated;
	ReadFile(FileToRead, Buffer, TextBufferSize, &bytesIterated, NULL);
	SetWindowTextA(hEditControl, Buffer);
	CloseHandle(FileToRead);
}
void SetFileParams(HWND hWnd)
{
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = FileName;
	ofn.nMaxFile = sizeof(FileName);
	ofn.lpstrFilter = ".txt";
	ofn.lpstrFileTitle = NULL; 
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = "E:\\visual studio\\repos\\Project2\\x64\\Debug";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

void SetWindowStatus(std::string status) {
	SetWindowTextA(hStaticControl, status.c_str());
}