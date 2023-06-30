#pragma once
#define onConnectRequest	10
#define onMenuAction1		1
#define onMenuAction2		2
#define onExitSoftWare		4
#define onClearField		5
#define onReadColor			6
#define TextBufferSize		240
#define DigitIndexNumber	200
#define onFileSave			7
#define onFileLoad			8
#define onSerialRefresh		9
#define comPortAmount		50
#define ComSelectIndex		120
#define DigitIndexColorR	200
#define DigitIndexColorG	201
#define DigitIndexColorB	202


char Buffer[TextBufferSize];
int CharsRead;
unsigned num;
char FileName[260];

volatile bool isConnected = false;
volatile bool isThreading = true;
int selectedPort=10;
int targetBaudRate=9600;
int colorR, colorG, colorB;

HANDLE connectedPort;
HANDLE readThread;

HFONT fontReacnagle;
COLORREF fontColor;


HBRUSH brushRectangle;
RECT windowRectangle;
PAINTSTRUCT ps;


LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);

HWND hStaticControl;
HWND hEditControl;
HWND hDigControl;
HMENU ComPortSubMenu;
HMENU ComPortListMenu;

OPENFILENAMEA ofn;
void MainWndMenus(HWND hWnd);
void MainWndAddWidgets(HWND hWnd);
void SetWindowStatus(std::string status);
void SaveData(LPCSTR path);
void LoadData(LPCSTR path);
void SetFileParams(HWND hWnd);