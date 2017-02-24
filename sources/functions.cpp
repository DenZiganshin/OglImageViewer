#include <windows.h>
#include <gl/glut.h>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>

#include "../headers/globals.h"

namespace Functions{

CPoint mouseOrig;
bool isMouseLeftDown = false;

bool isConsole = false;
FILE *_stdoutFile = NULL;
int _stdoutHandle = 0;
FILE _oldStdOut = *stdout;


void toggleConsole(){
	if(!isConsole){
		//show console
		AllocConsole();
		//redirecting stdout
		HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_stdoutHandle = _open_osfhandle((long)handle_out, _O_WTEXT);
		_stdoutFile = _wfdopen( _stdoutHandle, L"w");
		*stdout = *_stdoutFile;
		_setmode(_fileno(stdout), _O_U8TEXT);

		isConsole = true;
	}else{
		FreeConsole();

		isConsole = false;
	}
}


void wndMouseMoveFunc(UINT x, UINT y){
	if(Functions::isMouseLeftDown){
		//сдвиг изображения относительно точки нажатия
		g_render.MoveStart(	x - mouseOrig.x, 
							y - mouseOrig.y);
	}
}


void saveCurrentScreen(){
	BYTE *data = NULL;
	
	data = (BYTE*)malloc(3 * g_wndWidth * g_wndHeight);

	g_render.saveScreen(data);
	g_files.saveFile(L"test.png",data,g_wndWidth,g_wndHeight);

	free(data);
}

void wndKeybFunc(UINT key){
	switch(key){
	case VK_LEFT:
		if(g_files.loadPrev())
			g_render.SetImg(g_files.getImage());
		break;
	case VK_RIGHT:
		if(g_files.loadNext())
			g_render.SetImg(g_files.getImage());
		break;
	case 187:
		wprintf(L"FPS:%d\n",g_render.modifySpeed(1));
		break;
	case 189:
		wprintf(L"FPS:%d\n",g_render.modifySpeed(-1));
		break;
	case 67: //C key
		toggleConsole();
		break;
	case 80:
		saveCurrentScreen();
		break;
	}
}

void wndMouseFunc(UINT action, UINT x, UINT y, short param){
	x = x<0?0:x;
	y = y<0?0:y;
	switch(action){
		case WM_LBUTTONDOWN:
			mouseOrig.x = x;
			mouseOrig.y = y;
			isMouseLeftDown = true;
			break;
		case WM_LBUTTONUP:
			isMouseLeftDown = false;
			g_render.MoveEnd();
			break;
		//перемещение изображения (up/down)
		case WM_RBUTTONDOWN:
			mouseOrig.x = x;
			mouseOrig.y = y;
			isMouseLeftDown = true;
			break;
		case WM_RBUTTONUP:
			isMouseLeftDown = false;
			g_render.MoveEnd();
			break;
		//события от колеса мыши
		case WM_MOUSEWHEEL:
			g_render.Zoom(x,y,param);			
			break;
		case WM_MOUSELEAVE:
			if(isMouseLeftDown){
				isMouseLeftDown = false;
				g_render.MoveEnd();
			}
			break;
		case WM_LBUTTONDBLCLK:
			if( x > g_wndWidth/2){
				g_files.loadNext();
				g_render.SetImg(g_files.getImage());
			}else{
				g_files.loadPrev();
				g_render.SetImg(g_files.getImage());
			}
			break;
		default:
			break;
	}

	/*
	if((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)){
		g_render.clearTransforms();
	}
	if(button == 3){//wheel up
		g_render.ZoomIn(x,y);
	}
	if(button == 4){//wheel down
		g_render.ZoomOut(x,y);
	}
	*/
}

void saveWindowSizeAndPosition();

void resizeWindow(UINT w, UINT h){
	//вычисление положения рабочей области
	/*
	RECT cr,wr;
	GetClientRect(hwnd, &cr);
	GetWindowRect(hwnd, &wr);
	int diffx = (int)(((wr.right - wr.left) - cr.right) / 2);
	int diffy = (int)(((wr.bottom - wr.top) - cr.bottom) / 2);

	int w = cr.right,
		h = cr.bottom;
	*/

	//расчет матриц для opengl
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h); 
	glOrtho(-(int)(w/2),w/2,h/2,-(int)(h/2), -1, 1); 
	g_wndWidth = w;
	g_wndHeight = h;

	glMatrixMode(GL_MODELVIEW);

	g_render.resizeWnd(w, h);
	saveWindowSizeAndPosition();
}
void wndPosChangedFunc(WINDOWPOS *p){
	g_initX = p->x;
	g_initY = p->y;
	resizeWindow(p->cx, p->cy);
}

void saveWindowSizeAndPosition(){
	
	std::wstring name = g_path + L"config.txt";
	std::fstream fs;

	fs.open(name.c_str(), std::fstream::out);
	fs << "WindowWidth: " << g_wndWidth << std::endl;
	fs << "WindowHeight: " << g_wndHeight << std::endl;
	fs << "initX: " << g_initX << std::endl;
	fs << "initY: " << g_initY << std::endl;
	fs.close();
}

void loadWndConfig(){
	std::wstring name = g_path + L"config.txt";

	//проверка существования файла
	if(_waccess(g_path.c_str(), 4) == -1){
		return;
	}

	//загрузка
	std::fstream fs;

	std::string param;

	fs.open(name.c_str(), std::fstream::in);

	for(int i=0; i<4; i++){
		fs >> param;

		if(param == "WindowWidth:"){
			fs >> g_wndWidth;
		}
		if(param == "WindowHeight:"){
			fs >> g_wndHeight;
		}
		if(param == "initX:"){
			fs >> g_initX;
		}
		if(param == "initY:"){
			fs >> g_initY;
		}
	}

	fs.close();
}


//инициализация openGl
bool initGl(HWND hwnd, HDC *hdc){
	HGLRC hRC;
	GLuint PixelFormat;
	PIXELFORMATDESCRIPTOR pfd=				
	{
		sizeof(PIXELFORMATDESCRIPTOR),				
		1,											
		PFD_DRAW_TO_WINDOW |						
		PFD_SUPPORT_OPENGL |						
		PFD_DOUBLEBUFFER,							
		PFD_TYPE_RGBA,								
		16,										
		0, 0, 0, 0, 0, 0,							
		0,											
		0,											
		0,											
		0, 0, 0, 0,									
		16,											
		0,										
		0,											
		PFD_MAIN_PLANE,								
		0,											
		0, 0, 0										
	};

	if (!(*hdc=GetDC(hwnd)))							
	{
		printf("Can't Create A GL Device Context.\n");
		return false;					
	}

	if (!(PixelFormat=ChoosePixelFormat(*hdc,&pfd)))	
	{
		printf("Can't Find A Suitable PixelFormat.\n");
		return false;							
	}

	if(!SetPixelFormat(*hdc,PixelFormat,&pfd))		
	{
		printf("Can't Set The PixelFormat.\n");
		return false;								
	}

	if (!(hRC=wglCreateContext(*hdc)))				
	{
		printf("Can't Create A GL Rendering Context.\n");
		return false;								
	}

	if(!wglMakeCurrent(*hdc,hRC))					
	{
		printf("Can't Activate The GL Rendering Context.\n");
		return false;								
	}


	glShadeModel( GL_SMOOTH );
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	return true;
}

}// namespace