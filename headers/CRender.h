#ifndef CRENDERH
#define CRENDERH

#include <gl/glut.h>
#include "Structs.h"


class CRender{
	CImage* _img;
	UINT _WindowWidth, _WindowHeight; //������� ����
	//UINT _width, _height; // ������� �����������
	CShift _imgShift, _imgMove, _imgZoomShift, _imgCenter; // ������
	bool _isImgMoving; // ���� : true - ����������� ������� ,false - ���
	float _imgZoom; // ���. ���� �������� -> ��������X = ��������Y
	float _dtZoom; //���������(�����) ���(+/-) ����
	UINT /*_frameCount,*/ _frameCurrent; //���������� ������, ������� ����
	GLuint *_texturesId; //������ �������� ��������� �������
	long _delay; //����� ����� ������� ������
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