#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\serverCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\serverCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif // DEBUG

#include "CorePch.h"
