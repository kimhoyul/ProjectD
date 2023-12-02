#include "pch.h"
#include "CoreTLS.h"

thread_local uint32 LThreadId = 0; // 직접적으로 관리할 쓰레드 ID