#include <windows.h>
#include <gl/glut.h>
#include <iostream>
#include <stdio.h>


#include "../headers/globals.h"

CFiles g_files;
CRender g_render;

UINT	g_wndWidth,
		g_wndHeight,
		g_initX,
		g_initY;

#pragma comment(lib,"Gdiplus.lib")
#pragma comment(lib,"freeglut.lib")
#pragma comment(lib,"opengl32.lib")


void initGlobals(){
	g_wndWidth = 640;
	g_wndHeight = 480;
	g_initX = 100;
	g_initY = 100;
}

LRESULT CALLBACK msgWindowFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
bool initGl(HWND hwnd, HDC *hdc);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	HDC hdc;
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	LPCWSTR className = L"ResultWindow";    

	//установка глобальных переменных
	initGlobals();

	//получение аргументов командной строки
	LPTSTR line = GetCommandLine();
	int argc;
	LPWSTR *argv = CommandLineToArgvW(line, &argc);

	if(argc == 1){
		return 1;
	}

	//заполнение wnd class
	WNDCLASS wc;                               
	ZeroMemory(&wc,sizeof(wc));  
	wc.hInstance = hInstance;         
	wc.lpszClassName = className;                        
	wc.lpfnWndProc = msgWindowFunc;                        
	wc.style=CS_HREDRAW|CS_VREDRAW;                                              
	wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);       
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);            
	wc.lpszMenuName=NULL;                       
	wc.cbClsExtra=0;                              
	wc.cbWndExtra=0;                               
	if(!RegisterClass (&wc))
		return 1;


	//расчет требуемого размера окна
	RECT r;
	r.left = 0; r.top = 0;
	r.right = g_wndWidth, r.bottom = g_wndHeight;
	AdjustWindowRect(&r, windowStyle, false);

	//создание окна
	HWND hwnd = CreateWindow(
		className,
		L"ImgViewer",
		windowStyle,
		g_initX,g_initY,
		r.right - r.left,
		r.bottom - r.top,
		NULL,NULL,hInstance,NULL);

	//инициализация ogl
	if(!Functions::initGl(hwnd, &hdc)){
		return 1;
	}

	//отображение окна
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	//Загрузка первого изображения
	if(!g_files.loadFile(argv[1])){
		return 1;
	}
	g_render.SetImg(g_files.getImage());

	
	
	//main cycle
	while(true){
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
			if(msg.message == WM_CLOSE){
				break;
			}
			TranslateMessage(&msg);				
			DispatchMessage(&msg);				
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		g_render.Draw();
		g_render.mainLoopDelay();				
		SwapBuffers(hdc);
	}

	return msg.wParam;							
}


LRESULT CALLBACK msgWindowFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message){
		//управление
		case WM_PAINT:	
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break; 
		//клавиатура
		case WM_KEYDOWN:
			Functions::wndKeybFunc(wParam);
			break;
		case WM_KEYUP:
			break;
		//изменение параметров окна
		case WM_SIZING: //sizing in progress
			break;
		case WM_SIZE: //sized
			Functions::wndResizeFunc(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MOVING:
			break;
		case WM_MOVE:
			break;
		//сообщения мыши
		case WM_MOUSEMOVE:
			Functions::wndMouseMoveFunc(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_RBUTTONDOWN:
			Functions::wndMouseFunc(WM_RBUTTONDOWN,LOWORD(lParam), HIWORD(lParam), 0);
			break;
		case WM_RBUTTONUP:
			Functions::wndMouseFunc(WM_RBUTTONUP,LOWORD(lParam), HIWORD(lParam), 0);
			break;
		case WM_LBUTTONDOWN:
			Functions::wndMouseFunc(WM_LBUTTONDOWN,LOWORD(lParam), HIWORD(lParam), 0);
			break;
		case WM_LBUTTONUP:
			Functions::wndMouseFunc(WM_LBUTTONUP,LOWORD(lParam), HIWORD(lParam), 0);
			break;
		case WM_MOUSEWHEEL:
			Functions::wndMouseFunc(WM_MOUSEWHEEL, LOWORD(lParam), HIWORD(lParam), HIWORD(wParam));
			break;
		default:                                   
			return DefWindowProc (hwnd,message,wParam,lParam);
	}                                                         
	return 0;
}