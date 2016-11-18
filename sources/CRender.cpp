#include "../headers/CRender.h"


CRender::CRender(){
	resetValues();
	resetTransform();
}

void CRender::Draw(){
	glEnable(GL_TEXTURE_2D); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glPushMatrix();
	//�������������
	glTranslatef(	(float)_imgZoomShift.x + _imgShift.x + _imgMove.x + _imgCenter.x, 
					(float)_imgZoomShift.y + _imgShift.y + _imgMove.y + _imgCenter.y, 0);
	glScalef(_imgZoom,_imgZoom, 1);

	//�������� ������ ��������
	glBindTexture(GL_TEXTURE_2D, _texturesId[_img->frameCurrent]);

	// ����� �������� �� ��������������
	glBegin(GL_QUADS);
	  glVertex2f(0,	(float)_img->height); //1
	  glTexCoord2f(1.0f,	1.0f);

	  glVertex2f((float)_img->width,(float)_img->height); //2
	  glTexCoord2f(1.0f,	0.0f);

	  glVertex2f((float)_img->width,0); //3
	  glTexCoord2f(0.0f,	0.0f);

	  glVertex2f(0,				0); //4 
	  glTexCoord2f(0.0f,	1.0f);
	glEnd();
	
	
	nextFrame();

	glPopMatrix();
}

void CRender::nextFrame(){
	//����������� �� ������ ����� ��� ����������� ������������
	if( (_img->frameCount == 1) || (_isImgMoving) )
		return;
	_frameCurrent++;
	if(_frameCurrent >= _img->frameCount){
		_frameCurrent = 0;
	}
}


void CRender::MoveStart(int x, int y){
	_isImgMoving = true;
	_imgMove.x = (float)x;
	_imgMove.y = (float)y;
}

void CRender::MoveEnd(){
	_isImgMoving = false;
	_imgShift.x += _imgMove.x;
	_imgShift.y += _imgMove.y;
	//_imgZoomShift = 0;
	_imgMove = 0;
}

void CRender::SetImg(CImage *img){
	//�������
	cleanUp();

	//��������
	_img = img;
	_frameCurrent = 0;
	//�������� �������
	makeTexture(_img->data);

	//��������� ����� �������
	centerImage();
}

void CRender::mainLoopDelay(){
	//������� ����������� �����������
	if(_isImgMoving){
		Sleep(33); //~30 fps
		return;
	}

	//������� ���������� �� ������ ���������� ������
	if(_img->frameCount == 1){
		Sleep(100);
	}else{
		Sleep( _delay );
	}
}

void CRender::resetTransform(){
	//������� ������
	_imgShift = 0;
	_imgCenter = 0;
	_imgMove = 0;
	_imgZoomShift = 0;

	//������� ����
	_imgZoom = 1.0f;
}

void CRender::ZoomIn(UINT x, UINT y){
	if(_isImgMoving)
		return;
	_imgZoom += _dtZoom;
	_imgZoomShift.x -= ((x-(_imgShift.x+_imgCenter.x))*(1+_dtZoom)) - (x-(_imgShift.x+_imgCenter.x));
	_imgZoomShift.y -= ((y-(_imgShift.y+_imgCenter.y))*(1+_dtZoom)) - (y-(_imgShift.y+_imgCenter.y));
}

void CRender::ZoomOut(UINT x, UINT y){
	if(_isImgMoving)
		return;
	if(_imgZoom - _dtZoom < 0)
		return;
	_imgZoom -= _dtZoom;
	_imgZoomShift.x += ((x-(_imgShift.x+_imgCenter.x))*(1+_dtZoom)) - (x-(_imgShift.x+_imgCenter.x));
	_imgZoomShift.y += ((y-(_imgShift.y+_imgCenter.y))*(1+_dtZoom)) - (y-(_imgShift.y+_imgCenter.y));
}

void CRender::makeTexture(){

	glEnable(GL_TEXTURE_2D);
	//��������� id �������
	_texturesId = (GLuint*)malloc(sizeof(GLuint) * _img->frameCount);
	glGenTextures(_img->frameCount,_texturesId);
	//���������� �������
	for(UINT i=0; i<_img->frameCount; i++){
		glBindTexture(GL_TEXTURE_2D, _texturesId[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); 
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, _img->width, _img->height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, _img->data[i]);
	}
}

void CRender::cleanUp(){
	//�������� �������
	glEnable(GL_TEXTURE_2D);
	if(_img->frameCount == 0)
		return;
	glDeleteTextures(_img->frameCount, _texturesId);
	free(_texturesId);

	//����� �������
	resetTransform();
	//����� ����������
	resetValues();
	//�������� Img
	_img.cleanUp();
	delete _img;
}

void CRender::resetValues(){
	_dtZoom = 0.02f;
	_imgZoom = 1.0f;
	_frameCurrent = 0;
	_delay = 42; //24 fps
	_isImgMoving = false;
	_textFont = GLUT_BITMAP_TIMES_ROMAN_24;
}
void CRender::resizeWnd(UINT WindowWidth, UINT WindowHeight){
	_WindowHeight = WindowHeight;
	_WindowWidth = WindowWidth;

	centerImage();
}

void CRender::centerImage(){
	_imgCenter.x = (_WindowWidth - (_img->width * _imgZoom)) /2;
	_imgCenter.y = (_WindowHeight - (_img->height * _imgZoom)) /2;
}

void CRender::clearTransforms(){
	resetTransform();
	centerImage();
}

void CRender::drawName(){
	//������� ����� ����
	glRasterPos2f(0.0f, 0.0f);

	//�������������� wchar
	char *name = (char*) malloc(sizeof(char) * _img->fileName.length());

	//�����
	for (i=0; i<_img->fileName.size(); i++){
		glutBitmapCharacter(_textFont, );
	}

	free(name);
}