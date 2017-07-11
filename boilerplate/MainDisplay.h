#pragma once

#include "SGB.h"

class MainDisplay :
	public SGB_Display
{
public:

	virtual SGB_DisplayInitInfo GetInitInfo() override;

	virtual void BeginDraw();
	virtual void EndDraw();
};

