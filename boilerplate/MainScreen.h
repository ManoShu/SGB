#pragma once

#include "SGB.h"

class MainScreen :
	public SGB_Screen
{
public:

	virtual void LoadScreen() override;
	virtual void Update() override;
	virtual void ScreenShow() override;
};

