// BlueprintFrame.cpp : implementation file
//

#include "stdafx.h"
#include "Fractal Generator.h"
#include "BlueprintFrame.h"


// CBlueprintFrame

IMPLEMENT_DYNCREATE(CBlueprintFrame, CMDIChildWnd)

CBlueprintFrame::CBlueprintFrame() /*: CChildFrame()*/
{
	bBlueprintWnd = true;
}

CBlueprintFrame::~CBlueprintFrame()
{
}


BEGIN_MESSAGE_MAP(CBlueprintFrame, CMDIChildWnd)
END_MESSAGE_MAP()


// CBlueprintFrame message handlers
