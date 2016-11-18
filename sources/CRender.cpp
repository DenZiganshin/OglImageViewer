#include "../headers/CRender.h"


CRender::CRender(){
	resetValues();
	resetTransform();
	_fileName = NULL;
}

void CRender::Draw(){
	//установка состояний
	glEnable(GL_TEXTURE_2D); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	//запись матрицы просмотра
	glPushMatrix();
	//трансформации
	glTranslatef(	(float)_imgZoomShift.x + _imgShift.x + _imgMove.x + _imgCenter.x, 
					(float)_imgZoomShift.y + _imgShift.y + _imgMove.y + _imgCenter.y, 0);
	glScalef(_imgZoom,_imgZoom, 1);

	//привязка нужной текстуры
	glBindTexture(GL_TEXTURE_2D, _texturesId[_frameCurrent]);

	// вывод текстуры на прямоугольнике
	glBegin(GL_QUADS);
	  glVertex2f(0,(float)_height); //1
	  glTexCoord2f(1.0f,	1.0f);

	  glVertex2f((float)_width,(float)_height); //2
	  glTexCoord2f(1.0f,	0.0f);

	  glVertex2f((float)_width,	0); //3
	  glTexCoord2f(0.0f,	0.0f);

	  glVertex2f(0, 0); //4 
	  glTexCoord2f(0.0f,	1.0f);
	glEnd();	

	//переключение на след кадр
	nextFrame();
	//восстановление матрицы просмотра
	glPopMatrix();

	//вывод имени файла
	drawName();
}

void CRender::nextFrame(){
	//изображение из одного кадра или изображение перемещается
	if( (_frameCount == 1) || (_isImgMoving) )
		return;
	_frameCurrent++;
	if(_frameCurrent >= _frameCount){
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
	//очистка
	cleanUp();

	//загрузка
	_width = img->width;
	_height = img->height;
	_frameCount = img->frameCount;
	_frameCurrent = 0;
	//--//имя файла
	_fileName = new std::string(img->fileName.begin(),img->fileName.end());
	
	//создание текстур
	makeTexture(img->data);

	//установки перед выводом
	centerImage();
}

void CRender::mainLoopDelay(){
	//плавное перемещение изображения
	if(_isImgMoving){
		Sleep(33); //~30 fps
		return;
	}

	//сорость обновления на основе количества кадров
	if(_frameCount == 1){
		Sleep(100);
	}else{
		Sleep( _delay );
	}
}

void CRender::resetTransform(){
	//очистка сдвига
	_imgShift = 0;
	_imgCenter = 0;
	_imgMove = 0;
	_imgZoomShift = 0;

	//очистка зума
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

void CRender::makeTexture(BYTE **data){

	glEnable(GL_TEXTURE_2D);
	//генерация id текстур
	_texturesId = (GLuint*)malloc(sizeof(GLuint) * _frameCount);
	glGenTextures(_frameCount,_texturesId);
	//заполнение текстур
	for(UINT i=0; i<_frameCount; i++){
		glBindTexture(GL_TEXTURE_2D, _texturesId[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); 
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, _width, _height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data[i]);
	}
}

void CRender::cleanUp(){
	//удаление текстур
	glEnable(GL_TEXTURE_2D);
	if(_frameCount == 0)
		return;
	glDeleteTextures(_frameCount, _texturesId);
	free(_texturesId);

	//сброс сдвигов
	resetTransform();
	//восстановление значений
	resetValues();
	//удаление имени файла
	if(_fileName)
		delete _fileName;
}

void CRender::resetValues(){
	_dtZoom = 0.02f;
	_imgZoom = 1.0f;
	_width = 0;
	_height = 0;
	_frameCount = 0;
	_frameCurrent = 0;
	_delay = 42; //24 fps
	_isImgMoving = false;
	_font = GLUT_BITMAP_TIMES_ROMAN_24;
}
void CRender::resizeWnd(UINT WindowWidth, UINT WindowHeight){
	_WindowHeight = WindowHeight;
	_WindowWidth = WindowWidth;

	centerImage();
}

void CRender::centerImage(){
	_imgCenter.x = (_WindowWidth - (_width * _imgZoom)) /2;
	_imgCenter.y = (_WindowHeight - (_height * _imgZoom)) /2;
}

void CRender::clearTransforms(){
	resetTransform();
	centerImage();
}

void CRender::drawName(){
	glRasterPos2f(0.0f, 24.0f); // 24 - font height
	for(UINT i=0; i<_fileName->length(); i++){
		glutBitmapCharacter(_font, _fileName->c_str()[i]);
	}
}