#ifndef STRUCTSH
#define STRUCTSH

#include <gl/glut.h>

struct CShift{
	float x ,y;
	CShift(){
		x=0; y=0;
	}
	CShift(float a, float b){
		x=a; y=b;
	}
	CShift operator=(float a){
		x = a;
		y = a;
		return CShift(a,a);
	}
	CShift operator=(CShift a){
		return CShift(a.x,a.y);
	}
	CShift operator+=(CShift a){
		return CShift(x+a.x,y+a.y);
	}
};

struct CImage{
	BYTE **data;
	UINT width,height;
	UINT frameCount;
	std::wstring fileName;
	CImage(){
		data = NULL;
		width = 0; height = 0;
		frameCount = 0;
	}
	void cleanUp(){
		frameCount = 0;
		height = 0;
		width = 0;
		freeData();
	}
	void freeData(){
		if(data == NULL)
			return;
		for(UINT i=0; i<frameCount; i++){
			free(data[i]);
		}
		free(data);
		data = NULL;
	}
};


#endif