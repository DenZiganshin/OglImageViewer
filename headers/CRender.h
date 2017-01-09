#ifndef CRENDERH
#define CRENDERH

#include <Windows.h>
#include <gl/glut.h>
#include <algorithm>
#include <string>
#include "Structs.h"


class CRender{
	//CImage* _img;
	UINT _WindowWidth, _WindowHeight;
	UINT _width, _height;
	CShift _imgShift, _imgMove, _imgZoomShift, _imgCenter;
	bool _isImgMoving;
	float _imgZoom;
	float _dtZoom;
	UINT _frameCount, _frameCurrent;
	GLuint *_texturesId;
	int _delay;
	void* _font;

	DWORD _prevTime, _currentTime;
	int _FPS;

	std::string *_fileName;
public:
	CRender();
	void resizeWnd(UINT WindowWidth, UINT WindowHeight);
	void SetImg(CImage *img);	
	void Draw();
	void Zoom(UINT x, UINT y, short speed);
	//void ZoomOut(UINT x, UINT y, int speed);
	void MoveStart(int x, int y);
	void MoveEnd();	
	void mainLoopDelay();
	void clearTransforms();
	int modifySpeed(int count);
	void saveScreen(BYTE * outData);
private:
	void nextFrame();
	void makeTexture(BYTE **data);
	void cleanUp();
	void centerImage();
	void resetValues();
	void resetTransform();
	void drawName();
	void initFont();
};


#endif