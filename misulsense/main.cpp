#include <windows.h>
//#include <stdio.h>
#include <iostream>
#include <string.h>
#include <gdiplus.h>
#include <process.h>
#include <string>
#include <time.h>
#include <tuple>
#include <utility>
#include "serialcomm.h"
//#include "resource.h"

#define VEC_SIZE 10

using namespace std;
using namespace Gdiplus;
#pragma warning(disable: 4996);
#pragma comment(lib, "gdiplus")

const int WIDTH = 800;
const int HEIGHT = 600;

int hour, minu, seco, centi;
clock_t curr_clk, before_clk;
long double factor = 1.0;
HANDLE hThread, hMutex, hMutex2;
tuple<tuple<int, int, int, int>, int, int> record[VEC_SIZE];
int record_size = 0;
HFONT time_font, record_font, hangeul_font;
HBRUSH hBrush;

HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("GdiPlusStart");

unsigned WINAPI getArduino(void* args);
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	HWND     hWnd;
	MSG		 msg;
	WNDCLASS WndClass;

	g_hInst = hInstance;

	ULONG_PTR gpToken;
	GdiplusStartupInput gpsi;
	if (GdiplusStartup(&gpToken, &gpsi, NULL) != Ok)
	{
		MessageBox(NULL, TEXT("GDI+ 라이브러리를 초기화할 수 없습니다."), TEXT("알림"), MB_OK);
		return 0;
	}


	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = L"Misul Sen-se";
	RegisterClass(&WndClass);
	hWnd = CreateWindow(
		L"Misul Sen-se",
		L"Misul Sen-se",
		WS_OVERLAPPEDWINDOW,
		GetSystemMetrics(SM_CXFULLSCREEN) / 2 - WIDTH/2,
		GetSystemMetrics(SM_CYFULLSCREEN) / 2 - HEIGHT/2,
		WIDTH,
		HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
		);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, MemDC;
	PAINTSTRUCT ps;

	HBITMAP hBit, OldBit;
	RECT crt;

	RECT time_rt,rect, record_rt;
	WCHAR time_str[200]={}, record_str[40]={};
	HFONT OldFont;

	SYSTEMTIME st;


	switch (iMsg)
	{
	case WM_CREATE:
		SetTimer(hWnd, 1, 10, 0);
		hMutex = CreateMutex(NULL, FALSE, NULL);
		hMutex2 = CreateMutex(NULL, FALSE, NULL);
		time_font = CreateFont(90, 45, 0, 0, 3, 0,0,0, ANSI_CHARSET, 0,0,0, DEFAULT_PITCH, TEXT("굴림"));
		hangeul_font = CreateFont(26, 13, 0, 0, 3, 0,0,0, ANSI_CHARSET, 0,0,0, DEFAULT_PITCH, TEXT("굴림"));
		record_font = CreateFont(20, 10, 0, 0, 3, 0,0,0, ANSI_CHARSET, 0,0,0, DEFAULT_PITCH, TEXT("굴림"));
		hBrush = CreateSolidBrush(RGB(255, 255, 255));

		GetSystemTime(&st);
		hour = st.wHour + 9;
		minu = st.wMinute;
		seco = st.wSecond;
		centi = st.wMilliseconds/10;
		hThread=
			(HANDLE)_beginthreadex(NULL, 0, getArduino, NULL, 0, NULL);
		break;

	case WM_TIMER:
		InvalidateRect(hWnd, NULL, FALSE);
		
		WaitForSingleObject(hMutex, INFINITE);
		curr_clk = factor*clock();
		ReleaseMutex(hMutex);

		centi = (curr_clk/10)%100;
		if (curr_clk / CLOCKS_PER_SEC  - before_clk / CLOCKS_PER_SEC >= 1)
		{
			centi = 0;
			seco++;
		}
		if (seco >= 60)
		{
			seco = 0;
			minu++;
		}
		if(minu >= 60)
		{
			minu = 0;
			hour++;
		}
		if(hour >= 24)
		{
			hour = hour % 24;
		}
		before_clk = curr_clk;
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &crt);

		MemDC = CreateCompatibleDC(hdc);
		hBit = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
		OldBit = (HBITMAP)SelectObject(MemDC, hBit);
		
		//oldBrush = (HBRUSH)SelectObject(MemDC, hBrush);
		//hPen = CreatePen(PS_SOLID, 5, RGB(255, 255, 255));
		//oldPen = (HPEN)SelectObject(MemDC, hPen);

		FillRect(MemDC, &crt, hBrush);
		SetBkColor(MemDC, RGB(255, 255, 255));
		SetBkMode(MemDC, OPAQUE);
		SetTextColor(MemDC,RGB(0,0,0));

		GetWindowRect(hWnd, &rect);
		time_rt.left = (rect.right - rect.left)/2 - 300;
		time_rt.right = (rect.right - rect.left);
		time_rt.top = (rect.bottom - rect.top)/2 - 200;
		time_rt.bottom = (rect.bottom - rect.top);
		for(int i=0; i<200; i++)
		{
			time_str[i] = 0;
		}
		wsprintf(time_str, L"%02d:%02d:%02d:%02d", hour, minu, seco, centi);
		OldFont = (HFONT)SelectObject(MemDC, time_font);
		DrawText(MemDC, (LPWSTR)time_str, -1, &time_rt, DT_LEFT | DT_TOP );


		time_rt.left = (rect.right - rect.left)/2 - 190;
		time_rt.right = (rect.right - rect.left);
		time_rt.top = (rect.bottom - rect.top)/2 - 110;
		time_rt.bottom = (rect.bottom - rect.top);
		for(int i=0; i<200; i++)
		{
			time_str[i] = 0;
		}
		wsprintf(time_str, L"시            분            초            입니다.");
		OldFont = (HFONT)SelectObject(MemDC, hangeul_font);
		DrawText(MemDC, (LPWSTR)time_str, -1, &time_rt, DT_LEFT | DT_TOP );


		//SelectObject(MemDC, OldFont);
		//DeleteObject(time_font);
		
		OldFont = (HFONT)SelectObject(MemDC, record_font);
		for(int i=0; i<record_size; i++)
		{
			record_rt.left = (rect.right - rect.left)/2 - 100;
			record_rt.right = (rect.right - rect.left);
			record_rt.top = (rect.bottom - rect.top)/2 + i*25;
			record_rt.bottom = (rect.bottom - rect.top);

			for(int j=0; j<40; j++)
			{
				record_str[j] = 0;
			}

			if (get<1>(record[i]) > get<2>(record[i]))
			{
				wsprintf(record_str, L"%02d:%02d:%02d:%02d부터 시간이 느리게 갑니다.", 
					/*get<1>(record[i])/1000, get<1>(record[i])%1000,
					(get<1>(record[i])-get<2>(record[i]))/1000, (get<1>(record[i])-get<2>(record[i]))%1000,*/
					get<0>(get<0>(record[i])),
					get<1>(get<0>(record[i])),
					get<2>(get<0>(record[i])),
					get<3>(get<0>(record[i]))
				);
			}
			else /*if (get<1>(record[i]) < get<2>(record[i]))*/
			{
				wsprintf(record_str, L"%02d:%02d:%02d:%02d부터 시간이 빠르게 갑니다.", 
					/*get<1>(record[i])/1000, get<1>(record[i])%1000,
					(get<2>(record[i])-get<1>(record[i]))/1000, (get<2>(record[i])-get<1>(record[i]))%1000,*/
					get<0>(get<0>(record[i])),
					get<1>(get<0>(record[i])),
					get<2>(get<0>(record[i])),
					get<3>(get<0>(record[i]))
				);
			}
			/*else
			{
				wsprintf(record_str, L"%d.%03d(동일) @ %02d:%02d:%02d:%02d", 
					get<1>(record[i])/1000, get<1>(record[i])%1000,
					get<0>(get<0>(record[i])),
					get<1>(get<0>(record[i])),
					get<2>(get<0>(record[i])),
					get<3>(get<0>(record[i]))
				);
			}*/
	
			DrawText(MemDC, (LPWSTR)record_str, -1, &record_rt, DT_LEFT | DT_TOP );
		}

		//SelectObject(MemDC, OldFont);
		//DeleteObject(record_font);




		//OnPaint(MemDC, TITLE0, 0, 0);

		BitBlt(hdc, 0, 0, crt.right, crt.bottom, MemDC, 0, 0, SRCCOPY);
		SelectObject(MemDC, OldBit);
		DeleteDC(MemDC);
		//SelectObject(MemDC, oldPen);
		//DeleteObject(hPen);
		//SelectObject(MemDC, oldBrush);
		//DeleteObject(hBrush);
		DeleteObject(hBit);

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}


unsigned WINAPI getArduino(void* args)
{
	long double temp_factor = 1.0;
	long double before_factor = 1.0;
	int len;

	CSerialComm serialComm; //SerialComm 객체 생성
	BYTE* temp = (BYTE*)malloc(256);
	
	if(!serialComm.connect("COM3")) //COM3번의 포트를 오픈한다. 실패할 경우 -1을 반환한다.
	{
		cout << "connect faliled" << endl;
		return -1;
	}
	else
	{
		cout << "connect successed" << endl;
	}


	while(1)
	{
		for(int i=0; i<256; i++)
		{
			temp[i] = 0;
		}
		serialComm.readCommand((BYTE*)temp, 1);
		len = atoi((char*)temp);
		serialComm.readCommand((BYTE*)temp, len);
		temp_factor = atof((char*)temp);
		cout << "interval(ms): " << temp_factor/1000.0 << endl;
		cout << "temp_factor: " << 1/(temp_factor/1000.0) << endl;

		WaitForSingleObject(hMutex, INFINITE);
		before_factor = factor;
		factor = 1 / (temp_factor / 1000.0);
		ReleaseMutex(hMutex);

		WaitForSingleObject(hMutex2, INFINITE);
		for(int i=record_size; i>0; i--)
		{
			record[i] = record[i-1];
		}
		record[0] = make_tuple(make_tuple(hour,minu,seco,centi), temp_factor, (1/before_factor)*1000.0);
		if (record_size < VEC_SIZE)
			record_size++;
		ReleaseMutex(hMutex2);
	}

	serialComm.disconnect(); //작업이 끝나면 포트를 닫는다

	cout << "end connect" << endl;
	return 0;
}


/*#include <stdio.h>
#include <iostream>
#include <string>
#include "serialcomm.h"

using namespace std;

int main()
{
	char buffer; 
	CSerialComm serialComm; //SerialComm 객체 생성
	BYTE* temp = (BYTE*)malloc(256);


	if(!serialComm.connect("COM3")) //COM3번의 포트를 오픈한다. 실패할 경우 -1을 반환한다.
	{
		cout << "connect faliled" << endl;
		return -1;
	}
	else
	{
		cout << "connect successed" << endl;
	}


	while(1)  //오픈에 성공한 경우 sendCommand()를 통해 계속적으로 데이터를 전송한다. 전송에 실패 할 경우 failed 메시지를 출력한다.
	{
		cin >> buffer;

		if(!serialComm.sendCommand(buffer))
		{	
			cout << "send command failed"<< endl;
		}
		else
		{
			cout << "send Command success" << endl;
		}

		if(buffer == 'h')
		{
			for(int i=0; i<256; i++)
				temp[i] = 0;
			serialComm.readCommand((BYTE*)temp, 25);
			cout << "reply: " << temp << endl;
		}
	}


	serialComm.disconnect(); //작업이 끝나면 포트를 닫는다

	cout << "end connect" << endl;
	return 0;
}*/