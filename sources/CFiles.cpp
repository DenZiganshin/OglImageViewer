#include "../headers/CFiles.h"


	//Получение кодировщика для сохранения файла
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid){
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		using namespace Gdiplus;

		ImageCodecInfo* pImageCodecInfo = NULL;

		GetImageEncodersSize(&num, &size);
		if(size == 0)
		  return -1;  // Failure

		pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
		if(pImageCodecInfo == NULL)
		  return -1;  // Failure

		GetImageEncoders(num, size, pImageCodecInfo);

		for(UINT j = 0; j < num; ++j)
		{
		  if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		  {
			 *pClsid = pImageCodecInfo[j].Clsid;
			 free(pImageCodecInfo);
			 return j;  // Success
		  }
		}

		free(pImageCodecInfo);
		return -1;  // Failure
	}



	

void CFiles::getBitmapData(std::wstring filename){
	UINT imgWidth,imgHeight,frameCount;

	//Инициализация GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//Получение имени файла
	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(filename.c_str());

	//Получение количества кадров
	UINT dimensionCount;
	GUID *dimensionIds = NULL;
	dimensionCount = bitmap->GetFrameDimensionsCount();
	dimensionIds = (GUID*)malloc(dimensionCount * sizeof(GUID));
	bitmap->GetFrameDimensionsList(dimensionIds, dimensionCount);
	frameCount = bitmap->GetFrameCount( &dimensionIds[0] );

	//Получение задержек между кадрами
	int sizeOfFrameDelayTag = bitmap->GetPropertyItemSize(PropertyTagFrameDelay);
	Gdiplus::PropertyItem *propertyItem = (Gdiplus::PropertyItem*) malloc(sizeOfFrameDelayTag);
	bitmap->GetPropertyItem(PropertyTagFrameDelay, sizeOfFrameDelayTag, propertyItem);


	//Чтение bitmapData
	Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;
	imgWidth = bitmap->GetWidth();
	imgHeight = bitmap->GetHeight();
	Gdiplus::Rect rect(0, 0, imgWidth, imgHeight);

	BYTE** ImageBuffer = (BYTE**)malloc(sizeof(BYTE*) *frameCount);
	for(UINT i=0; i<frameCount; i++ ){
		ImageBuffer[i] = (BYTE*)malloc(sizeof(BYTE) * imgWidth*imgHeight*4);
	}


	//--//проход по всем кадрам
	for(UINT i=0; i<frameCount; i++){
		bitmap->SelectActiveFrame(&dimensionIds[0],i);
		bitmap->LockBits(
			&rect,
			Gdiplus::ImageLockModeRead,
			PixelFormat32bppARGB,
			bitmapData);

		BYTE* pixels = (BYTE*)bitmapData->Scan0;
		//заполнение временного буффера (а может не надо)
		memcpy(ImageBuffer[i], pixels, sizeof(BYTE)*4*imgHeight*imgWidth);

		bitmap->UnlockBits(bitmapData);
	}

	//заполнение структуры
	//--//очистка
	if(_img != NULL){
		_img->cleanUp();
		delete _img;
	}
	//--//выделение памяти
	//--//--//создание CImage
	_img = new CImage();
	//--//--//создание CImage::data
	_img->data = (BYTE**)malloc(sizeof(BYTE*)*frameCount);
	for(UINT i=0; i<frameCount; i++ ){
		_img->data[i] = (BYTE*)malloc(sizeof(BYTE)*imgWidth*imgHeight*4);
	}
	//--//заполнение
	for(UINT i=0; i<frameCount; i++ ){
		memcpy(_img->data[i], ImageBuffer[i], sizeof(BYTE)*4*imgHeight*imgWidth);
	}
	_img->width = imgWidth;
	_img->height = imgHeight;
	_img->frameCount = frameCount;
	_img->fileName = filename;


	/*
	// Сохранение нового файла
	Gdiplus::Bitmap* newBit = new Gdiplus::Bitmap(imgWidth, imgHeight*frameCount, imgWidth*4, PixelFormat32bppARGB, oneImageBuffer);
	std::cout << "sizes:";
	std::cout << newBit->GetWidth() << " ";
	std::cout << newBit->GetHeight() << std::endl;
	CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);
	newBit->Save(L"res.png",&pngClsid, NULL);
	delete newBit;
	*/

	//очистка
	free(dimensionIds);
	for(UINT i=0; i<frameCount; i++ ){
		free(ImageBuffer[i]);
	}
	free(ImageBuffer);

	delete bitmapData;
	delete bitmap;
	Gdiplus::GdiplusShutdown(gdiplusToken);
}



void CFiles::getFileList(std::wstring name){
	std::vector<std::wstring> masks;
	std::wstring currFileName;
	WIN32_FIND_DATA findData;

	masks.push_back(L"*.jpg");
	masks.push_back(L"*.jpeg");
	masks.push_back(L"*.bmp");
	masks.push_back(L"*.png");
	masks.push_back(L"*.gif");
	masks.push_back(L"*.tif");

	//Выделение рабочей папки и имени файла
	std::size_t found = name.find_last_of(L"/\\");
	if(found == std::string::npos){
		_folderPath = L"";
		currFileName = name;
	}else{
		_folderPath = name.substr(0,found+1);
		currFileName = name.substr(found+1, name.length());
	}

	//обход по всем маскам
	for(UINT i=0; i<masks.size(); i++){
		//путь + маска
		std::wstring path = _folderPath + masks[i];

		//первый файл по маске
		HANDLE hFind = FindFirstFile(path.c_str(), &findData);
		if(!hFind)
			return;
		_filesList.push_back(std::wstring(findData.cFileName));

		//остальные файлы
		while(FindNextFile(hFind, &findData)){
			_filesList.push_back(std::wstring(findData.cFileName));
		}
		FindClose(hFind);
	}

	for(UINT i=0; i<_filesList.size(); i++){
		if(_filesList[i].compare(currFileName) == 0){
			_positionInList = i;
		}
	}

}

void CFiles::loadNext(){
	//получение следующего имени файла
	if(_positionInList == _filesList.size()-1) //last file
		return;
	_positionInList++;
	std::wstring fullName = _folderPath + _filesList[_positionInList];

	//отчет о работе
	std::wcout << L"loading..." <<std::endl;
	std::wcout << L">" << fullName <<L" ["<< _positionInList <<L"/"<<_filesList.size()<<L"]"<<std::endl;

	//загрузка bitmap
	getBitmapData(fullName);
}
void CFiles::loadPrev(){
	//получение предыдущего имени файла
	if(_positionInList == 0) //first file
		return;
	_positionInList--;
	std::wstring fullName = _folderPath + _filesList[_positionInList];

	//отчет о работе
	std::wcout << L"loading..." <<std::endl;
	std::wcout << L">" << fullName <<L" ["<< _positionInList <<L"/"<<_filesList.size()<<L"]"<<std::endl;

	//загрузка bitmap
	getBitmapData(fullName);
}

int CFiles::loadFile(std::wstring name){
	//проверка существования файла
	if(_waccess(name.c_str(), 4) == -1){
		return 0;
	}

	//загрузка списка файлов 
	getFileList(name);

	//отчет о работе
	std::wcout << L"loading..." <<std::endl;
	std::wcout << L">" << name <<L" ["<< _positionInList <<L"/"<<_filesList.size()<<L"]"<<std::endl;

	//загрузка bitmap
	getBitmapData(name);

	return 1;
}


CImage* CFiles::getImage(){
	return _img;
}