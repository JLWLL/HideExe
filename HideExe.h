
// HideExe.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CHideExeApp:
// 有关此类的实现，请参阅 HideExe.cpp
//

class CHideExeApp : public CWinApp
{
public:
	CHideExeApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CHideExeApp theApp;