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

extern std::wstring g_path;

namespace Functions{
	void toggleConsole();
	void wndMouseMoveFunc(UINT x, UINT y);
	void wndKeybFunc(UINT key);
	void wndMouseFunc(UINT action, UINT x, UINT y, short param);
	void wndPosChangedFunc(WINDOWPOS *p);
	bool initGl(HWND hwnd, HDC *hdc);
	void loadWndConfig();
}

#endif