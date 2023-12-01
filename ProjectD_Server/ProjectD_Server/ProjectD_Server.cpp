#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>


int32 buffer[10000][10000];

int main()
{
	memset(buffer, 0, sizeof(buffer));

	{
		uint64 start = GetTickCount64();

		int64 sum = 0;

		for (int32 i = 0; i < 10000; ++i)
			for (int32 j = 0; j < 10000; ++j)
				sum += buffer[i][j];

		// [i][j][j][j][j][j][j]... [i][j][j][j][j][j][j]... [i][j][j][j][j][j][j]... [i][j][j][j][j][j][j]... 
		// j가 연속적으로 증가하면서 데이터를 접근하므로 캐시철학에 맞게 캐시에 연속적으로 데이터를 올려놓는다.
		// 연속한 데이터가 캐시에 있는지 확인하고, 캐시에 있으면 빠르게 접근할 수 있다. == 캐시 힛

		uint64 end = GetTickCount64();
		cout << "Elapsed Time: " << end - start << "ms\n";
	}

	{
		uint64 start = GetTickCount64();

		int64 sum = 0;

		for (int32 i = 0; i < 10000; ++i)
			for (int32 j = 0; j < 10000; ++j)
				sum += buffer[j][i];

		// [i][j][j][j][j][j][j]... [i][j][j][j][j][j][j]... [i][j][j][j][j][j][j]... [i][j][j][j][j][j][j]... 
		// i가 증가하면서 데이터를 접근하므로 비연속적으로 데이터를 접근한다.
		// 연속하지 않은 데이터를 접근하므로 캐시 힛이 발생이 떨어진다.

		uint64 end = GetTickCount64();
		cout << "Elapsed Time: " << end - start << "ms\n";
	}
}