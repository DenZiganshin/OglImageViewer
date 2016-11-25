#include <windows.h>
#include <gl/glut.h>
#include <iostream>
#include <stdio.h>

#include "../headers/globals.h"

namespace Functions{

CPoint mouseOrig;
bool isMouseLeftDown = false;


void wndMouseMoveFunc(int x, int y){
	if(Functions::isMouseLeftDown){
		//сдвиг изображения относительно точки нажатия
		g_render.MoveStart(	x - mouseOrig.x, 
							y - mouseOrig.y);
	}
}

void wndKeybFunc(UINT key){
	switch(key){
	case VK_LEFT:
		g_files.loadPrev();
		g_render.SetImg(g_files.getImage());
		break;
	case VK_RIGHT:
		g_files.loadNext();
		g_render.SetImg(g_files.getImage());
		break;
	}
}

void wndMouseFunc(int action, int x, int y, short param){
	x = x<0?0:x;
	y = y<0?0:y;
	switch(action){
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
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

void wndResizeFunc(int w, int h){
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
	glOrtho(-w/2,w/2,h/2,-h/2, -1, 1); 
	g_wndWidth = w;
	g_wndHeight = h;

	glMatrixMode(GL_MODELVIEW);

	g_render.resizeWnd(w, h);
	//saveWindowSizeAndPosition();
}

void saveWindowSizeAndPosition(){
	FILE *f;
	int posX = glutGet(GLUT_WINDOW_X),
		posY = glutGet(GLUT_WINDOW_Y);
	//запись. заменить fopen
	f = fopen("config.txt", "wt");
	fprintf(f,"WindowWidth:%d\n",g_wndWidth);
	fprintf(f,"WindowHeight:%d\n",g_wndHeight);
	fprintf(f,"initX:%d\n",posX);
	fprintf(f,"initY:%d\n",posY);
	fclose(f);
}
std::string readLine(FILE *f){
	char ch;
	std::string st = "";
	while((ch = fgetc(f)) != '\n'){
		st += ch;
	}
	return st;
}
void loadWndConfig(){
	FILE *f = NULL;
	f = fopen("config.txt", "rt");
	if(f == NULL)
		return;
	//ищем 4 параметра в любом порядке
	for(int i=0; i<4; i++){
		int find;
		// читаем строку до \n
		std::string s = readLine(f);
		//поиск совпадений
		if((find = s.find("WindowWidth:")) != std::string::npos){
			std::string subs = s.substr(find + strlen("WindowWidth:"), s.length());
			g_wndWidth = atoi(subs.c_str());
			continue;
		}
		if((find = s.find("WindowHeight:")) != std::string::npos){
			std::string subs = s.substr(find + strlen("WindowHeight:"), s.length());
			g_wndHeight = atoi(subs.c_str());
			continue;
		}
		if((find = s.find("initX:")) != std::string::npos){
			std::string subs = s.substr(find + strlen("initX:"), s.length());
			g_initX = atoi(subs.c_str());
			continue;
		}
		if((find = s.find("initY:")) != std::string::npos){
			std::string subs = s.substr(find + strlen("initY:"), s.length());
			g_initY = atoi(subs.c_str());
			continue;
		}
	}
}


//инициализация openGl
bool initGl(HWND hwnd, HDC *hdc){
	HGLRC hRC;
	GLuint PixelFormat;
	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		16,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
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