#pragma once
#include "ChildFrm.h"

// CBlueprintFrame frame

class CBlueprintFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CBlueprintFrame)
protected:
	CBlueprintFrame();           // protected constructor used by dynamic creation
	virtual ~CBlueprintFrame();

protected:
	DECLARE_MESSAGE_MAP()
};


