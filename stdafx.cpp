
// stdafx.cpp : source file that includes just the standard includes
// ninja.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


void Log(const char *formatstr ... )
{
	char buffer[2048];

	va_list list; 
	va_start( list, formatstr );
	vsprintf_s( buffer,2048, formatstr, list );
	va_end( list );

	strcat(buffer,"\n");
	OutputDebugStringA(buffer);

}