#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include "RefCounting.h"

class Knight
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}
};

int main()
{	
	// 1) 이미 만들어진 클래스 대상으로 사용 불가 : RefCountable 을 상속 받아야 가능 (AddRef, ReleaseRef 라는 똑같은 인터페이스를 제공해주기 위해)
	// 2) 순환 참조가 발생할 수 있음 : 두 객체가 서로를 참조하는 경우 발생, 일반 shared_ptr 도 순환 참조가 발생할 수 있음
		
	// unique_ptr : 복사를 막아둔 스마트 포인터
	unique_ptr<Knight> knight2 = make_unique<Knight>();
	//unique_ptr<Knight> knight3 = knight2; // 복사 불가능
	unique_ptr<Knight> knight3 = std::move(knight2); // 이동만 가능
	
	// shared_ptr : 참조 카운팅을 통해 메모리를 관리하는 스마트 포인터
	shared_ptr<Knight> spr(new Knight());
	// shared_ptr 은 new로 생성될때 이렇게 [T*][RefCountBlock*] 두개의 객체를 생성한다.
	// [T*] : Knight 객체를 가리키는 포인터
	// [RefCountBlock*] : 참조 카운트를 관리하는 객체
	// [RefCountBlock*] 는 [useCount][weakCount] 를 가지고 있다.
	// [useCount] : 현재 참조 카운트
	// [weakCount] : weak_ptr 이 shared_ptr 을 참조 중인지 여부를 나타내는 카운트
	// 반면 TSharedPtr 은 [T*][RefCountBlock*] 두개의 객체를 생성하지 않고 [T*] 만 생성한다. 다만, [T*] 내부에 참조 카운트를 관리하는 객체를 포함하고 있다.

	shared_ptr<Knight> spr2 = make_shared<Knight>();
	// shared_ptr 은 make_shared로 생성될때 이렇게 [T* | RefCountBlock*] 영역을 한번에 할당받는다.

	// weak_ptr : shared_ptr 의 순환 참조 문제를 해결하기 위한 스마트 포인터
	weak_ptr<Knight> wpr = spr; // shared_ptr 을 weak_ptr 로 변환

	bool isExpired = wpr.expired(); // shared_ptr 이 유효한지 확인
	shared_ptr<Knight> spr3 = wpr.lock(); // shared_ptr 로 변환

	// weak_ptr 은 shard_ptr 와 짝을 이루어 사용해야 한다.
	// weak_ptr 은 shared_ptr 을 참조하고 있지만, 참조 카운트를 증가시키지 않는다.
}