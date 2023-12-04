#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

int number = 1000000;
int a[10000001];

void primeNumberSieve() {

	// 1. 배열을 생성하여 초기화한다.
	for (int i = 2; i <= number; i++) {
		a[i] = i;
	}

	// 2. 2부터 시작해서 특정 수의 배수에 해당하는 수를 모두 지운다.
	// (지울 때 자기자신은 지우지 않고, 이미 지워진 수는 건너뛴다.)
	for (int i = 2; i <= number; i++) {
		if (a[i] == 0) continue; // 이미 지워진 수라면 건너뛰기

		// 이미 지워진 숫자가 아니라면, 그 배수부터 출발하여, 가능한 모든 숫자 지우기
		for (int j = 2 * i; j <= number; j += i) {
			a[j] = 0;
		}
	}

	int res =0;

	// 3. 2부터 시작하여 남아있는 수를 모두 출력한다.
	for (int i = 2; i <= number; i++) {
		if (a[i] != 0)
		{
			res++;
		}
	}

	printf("%d ", res);
}

int main(void) {
	primeNumberSieve();
	return 0;
}