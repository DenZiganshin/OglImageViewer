#ifndef CRENDERH
#define CRENDERH

#include <gl/glut.h>
#include "Structs.h"


class CRender{
	CImage* _img;
	UINT _WindowWidth, _WindowHeight; //размеры окна
	//UINT _width, _height; // размеры изображения
	CShift _imgShift, _imgMove, _imgZoomShift, _imgCenter; // сдвиги
	bool _isImgMoving; // флаг : true - изображение двигают ,false - нет
	float _imgZoom; // зум. один параметр -> маштабирX = маштабирY
	float _dtZoom; //константа(почти) шаг(+/-) зума
	UINT /*_frameCount,*/ _frameCurrent; //количество кадров, текущий кадр
	GLuint *_texturesId; //массив индексов созданных текстур
	long _delay; //пауза между сменами кадров
	std::wstring fileName;
public:
	CRender();
	void resizeWnd(UINT WindowWidth, UINT WindowHeight);
	void SetImg(CImage *img);	
	void Draw();
	void ZoomIn(UINT x, UINT y);
	void ZoomOut(UINT x, UINT y);
	void MoveStart(int x, int y);
	void MoveEnd();	
	void mainLoopDelay();
	void clearTransforms();
private:
	void nextFrame();
	void makeTexture(BYTE **data);
	void cleanUp();
	void centerImage();
	void resetValues();
	void resetTransform();
};


#endif