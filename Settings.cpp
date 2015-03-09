#include "StdAfx.h"
#include "Settings.h"


bool Settings::Move=true;
bool Settings::Resize=true;
int  Settings::ClientWidth=585;
int  Settings::WindowWidth=0;
POINT Settings::slots[32];

bool Settings::Fade=true;
bool Settings::Highlight=true;
bool Settings::Warning=true;

int Settings::Fold=-2;

int Settings::DefaultPreflop=1;
int Settings::DefaultPostflop=2;