#pragma once

class Settings
{
private:
	Settings(void) {}

public:
	static bool Move;
	static bool Resize;
	static int  ClientWidth;
	static int  WindowWidth;
	static POINT slots[32];

	static bool Fade;
	static bool Highlight;
	static bool Warning;

	static int Fold;

	static int DefaultPreflop;
	static int DefaultPostflop;

};

