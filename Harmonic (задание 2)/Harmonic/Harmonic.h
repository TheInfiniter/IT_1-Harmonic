
// Harmonic.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CHarmonicApp:
// Сведения о реализации этого класса: Harmonic.cpp
//

class CHarmonicApp : public CWinApp
{
public:
	CHarmonicApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CHarmonicApp theApp;
