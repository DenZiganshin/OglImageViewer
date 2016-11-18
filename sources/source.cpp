#include <windows.h>
#include <gl/glut.h>
#include <iostream>
#include <stdio.h>

#include "../headers/CFiles.h"
#include "../headers/CRender.h"

#pragma comment(lib,"Gdiplus.lib")
#pragma comment(lib,"freeglut.lib")
#pragma comment(lib,"opengl32.lib")
//#pragma comment(lib,"glu32.lib")
//using namespace Gdiplus;


CFiles g_files;
CRender g_render;

UINT	g_wndWidth = 600,
		g_wndHeight = 400,
		g_initX = 100,
		g_initY = 100;
bool g_MouseLeftDown = false;

struct CPoint{
	int x,y;
}g_mouseOrig;


void glRender(void);

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
void glMouseMotionFunc(int x, int y){
	if(g_MouseLeftDown){
		//сдвиг изображения относительно точки нажатия
		g_render.MoveStart(x - g_mouseOrig.x, y - g_mouseOrig.y);
	}
}
void glKeybFunc(unsigned char key, int x, int y) {
}
void glSpecialKeyFunc(int key, int x, int y){
	switch(key){
	case GLUT_KEY_UP:
		//do something here
		break;	
	case GLUT_KEY_DOWN:
		//do something here
		break;
	case GLUT_KEY_LEFT:
		g_files.loadPrev();
		g_render.SetImg(g_files.getImage());
		break;
	case GLUT_KEY_RIGHT:
		g_files.loadNext();
		g_render.SetImg(g_files.getImage());
		break;
	}
}
void glMouseFunc(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON){
		if (state == GLUT_DOWN) {
			g_mouseOrig.x = x;
			g_mouseOrig.y = y;
			g_MouseLeftDown = true;
		}else if(state == GLUT_UP){
			g_MouseLeftDown = false;
			g_render.MoveEnd();
		}
	}
	if((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)){
		g_render.clearTransforms();
	}
	if(button == 3){//wheel up
		g_render.ZoomIn(x,y);
	}
	if(button == 4){//wheel down
		g_render.ZoomOut(x,y);
	}
}

void glRender(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	g_render.Draw();

	glutSwapBuffers();
}

void glIdle(){
	glRender();
	g_render.mainLoopDelay();
}

void glResize(int w, int h){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);
 
	//gluPerspective(45,ratio,1,1000);
	glOrtho(0,w,h,0, -1, 1);
	//gluOrtho2D(0,w,0,h);
 
	g_wndWidth = w;
	g_wndHeight = h;
	// вернуться к модели
	glMatrixMode(GL_MODELVIEW);

	g_render.resizeWnd(w, h);
	saveWindowSizeAndPosition();
}


int wmain(int argc, wchar_t *argv[]) {
	if(argc == 1){
		std::cout << "No image to draw. Use command line (or open with)" << std::endl;
		getchar();
		return 0;
	}
	//установка локали
	std::locale current_locale("");
	std::locale::global(current_locale);

	//Инициализация glut
	char fakeParam[] = "fake";
	char *fakeargv[] = { fakeParam, NULL };
	int fakeargc = 1;
	glutInit(&fakeargc, fakeargv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	//установка параметров окна
	loadWndConfig();
	glutInitWindowPosition(g_initX,g_initY);
	glutInitWindowSize(g_wndWidth,g_wndHeight);
	glutCreateWindow("Viewer");


	//Загрузка первого изображения
	g_files.loadFile(argv[1]);
	g_render.SetImg(g_files.getImage());
	
	//Установка обработчиков
	glutDisplayFunc(glRender);
	glutReshapeFunc(glResize);
	glutMouseFunc(glMouseFunc);
	glutMotionFunc(glMouseMotionFunc);
	glutKeyboardFunc(glKeybFunc);
	glutSpecialFunc(glSpecialKeyFunc);
	glutIdleFunc(glIdle);

	//Запуск основного цикла
	glutMainLoop();

	return 0;
}