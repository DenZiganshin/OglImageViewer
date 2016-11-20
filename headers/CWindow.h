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
	void (*keyboardCallback)(UINT key);
	void (*resizeCallback)(int w, int h);
	void (*idleCallback)(void);
	//handle
	HWND _hwnd;
	HANDLE _thrHandle;
	//is
	bool _isWindowCreated;
public:
	CWindow();
	void initPosition(int x, int y);
	void initSize(int w, int h);
	void setCallbackMouse(void (*f)(int key, int x, int y));
	void setCallbackKeyboard(void (*f)(UINT key));
	void setCallbackResize(void (*f)(int w, int h));
	void setCallbackIdle(void (*f)(void));
	void createWindow(const WCHAR *title);
	void mainCycle();
private:
	static DWORD WINAPI wndThreadFunc(void *arg);
	static LRESULT CALLBACK msgWindowFunc(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam);
	void createW32Window();
};


#endif