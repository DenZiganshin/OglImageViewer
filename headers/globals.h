#ifndef GLOBALSH
#define GLOBALSH

#include "../headers/Structs.h"
#include "../headers/CFiles.h"
#include "../headers/CRender.h"

extern CFiles g_files;
extern CRender g_render;
//CWindow g_window;

extern UINT	g_wndWidth,
			g_wndHeight,
			g_initX,
			g_initY;

namespace Functions{
	void wndMouseMoveFunc(int x, int y);
	void wndKeybFunc(UINT key);
	void wndMouseFunc(int action, int x, int y, short param);
	void wndResizeFunc(int w, int h);
	bool initGl(HWND hwnd, HDC *hdc);
}

#endif