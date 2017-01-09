#ifndef CFILESH
#define CFILESH

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <gl/glut.h>
#include <io.h>
#include "Structs.h"

/*
#pragma comment(lib,"glut.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"Gdiplus.lib")
*/

class CFiles{
CImage *_img;
GLuint _textureId;
std::wstring _folderPath;
std::vector<std::wstring> _filesList;
UINT _positionInList;
public:
	CFiles(){
		_textureId = 0;
		_img = NULL;
	}
	int loadFile(std::wstring);
	int saveFile(std::wstring name, BYTE *data, int width, int height);
	bool loadNext();
	bool loadPrev();
	CImage* getImage();
private:
	void getBitmapData(std::wstring filename);
	void getFileList(std::wstring name);
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
};

#endif