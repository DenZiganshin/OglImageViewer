#ifndef CWINDOWH
#define CWINDOWH

#include <Windows.h>
#include <string>

class CWindow{
	int _x,_y;
	int _width, _height;
	std::wstring _title;
	//callbacks
	void (*mouseCallback)(int key, int x, int y);
	void (*keyboardCallback)(int key);
	void (*resizeCallback)(int w, int h);
	void (*idleCallback)(void);
public:
	CWindow();
	void initPosition(int x, int y);
	void initSize(int w, int h);
	void setCallbackMouse(void (*f)(int key, int x, int y));
	void setCallbackKeyboard(void (*f)(int key));
	void setCallbackResize(void (*f)(int w, int h));
	void setCallbackIdle(void (*f)(void));
	void createWindow(const WCHAR *title);
private:
	static DWORD WINAPI wndThreadFunc(void *arg);
	static LRESULT CALLBACK msgWindowFunc(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam);
	void createW32Window();
};


#endif