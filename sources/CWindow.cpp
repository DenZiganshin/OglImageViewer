#include "../headers/CWindow.h"

CWindow::CWindow(){
	//init
	_hwnd = NULL;
	_isWindowCreated = false;
}

//начальное положение
void CWindow::initPosition(int x, int y){
	_x = x;
	_y = y;
}

//начальные размеры
void CWindow::initSize(int w, int h){
	_width = w;
	_height = h;
}

//создание нового потока и окна
void CWindow::createWindow(const WCHAR *title){
	_title.assign(title);
	_thrHandle = CreateThread(NULL, 0, wndThreadFunc, (void*)this , 0, 0);
}

//static функция потока, вызывает функцию класса
DWORD CWindow::wndThreadFunc(void *arg){
	((CWindow*)arg)->createW32Window();
	return 0;
}

//стандартный w32 обработчик сообщений для окна
LRESULT CALLBACK CWindow::msgWindowFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	//получение указ. на себя (т.к. static)
	CWindow *wnd = (CWindow*)GetWindowLong(hwnd, GWLP_USERDATA);

	switch(message){
		case WM_PAINT:			
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break; 
		case WM_KEYDOWN:
			wnd->keyboardCallback(wParam);
			break;
		case WM_KEYUP:
			break;
		case WM_SIZING: //sizing in progress
			break;
		case WM_SIZE: //sized
			break;
		case WM_MOVING:
			break;
		case WM_MOVE:
			break;
		//case: WM_MOUSE_SMTH
		default:                                   
			return DefWindowProc (hwnd,message,wParam,lParam);
	}                                                         
	return 0;
}

//функция создания окна
void CWindow::createW32Window(){
	MSG msg;
	LPCWSTR className = L"ResultWindow";          

	HINSTANCE hInstance;
	hInstance = GetModuleHandle(0);

	//заполнение wnd class
	WNDCLASS wc;                               
	ZeroMemory(&wc,sizeof(wc));  
	wc.hInstance = hInstance;         
	wc.lpszClassName = className;                        
	wc.lpfnWndProc = msgWindowFunc;                        
	wc.style=0;                                              
	wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);       
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);            
	wc.lpszMenuName=NULL;                       
	wc.cbClsExtra=0;                              
	wc.cbWndExtra=0;                               
	RegisterClass (&wc);

	//создание окна
	_hwnd = CreateWindow(
		className,_title.c_str(),
		WS_OVERLAPPEDWINDOW,
		_x,_y,
		_width,
		_height,
		NULL,NULL,hInstance,NULL);

	//передача параметров в msgWindowFunc (SetWindowLongPtr / GetWindowLongPtr)
	SetWindowLongPtr(_hwnd,GWL_USERDATA, (long)this);

	//отображение окна
	ShowWindow(_hwnd, SW_SHOWNORMAL);

	_isWindowCreated = true;

	//основной цикл окна
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CWindow::setCallbackIdle(void (*f)(void)){
	idleCallback = f;
}

void CWindow::setCallbackKeyboard(void (*f)(UINT)){
	keyboardCallback = f;
}

void CWindow::setCallbackMouse(void (*f)(int, int, int)){
	mouseCallback = f;
}

void CWindow::setCallbackResize(void (*f)(int, int)){
	resizeCallback = f;
}

void CWindow::mainCycle(){
	while(!_isWindowCreated); //костыли
	WaitForSingleObject(_thrHandle, INFINITE);
}