#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#ifdef _DEBUG
#pragma comment(lib, "Debug\\serverCore.lib")
#else
#pragma comment(lib, "Release\\serverCore.lib")
#endif // DEBUG

#include "CorePch.h"
