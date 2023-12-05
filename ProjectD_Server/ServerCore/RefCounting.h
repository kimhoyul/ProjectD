#pragma once

/*-------------------------------------------
			    RefCountable

참조 카운팅을 통해 자동으로	메모리를 관리하는 클래스
-------------------------------------------*/
class RefCountable
{
public:
	RefCountable() : _refCount(1) {} // 기본적으로 생성하자마자 1로 초기화
	virtual ~RefCountable() {}

	int32 GetRefCount() { return _refCount; } // 현재 참조 카운트를 반환

	int32 AddRef() { return ++_refCount; } // 참조 카운트를 1 증가시킨 후 반환

	int32 ReleaseRef() // 참조 카운트를 1 감소시킨 후 반환
	{
		int32 refCount = --_refCount;
		if (refCount == 0)
			delete this; // 참조 카운트가 0이 되면 메모리를 해제한다. (소멸자 호출

		return refCount;
	}

private:
	atomic<int32> _refCount;
};

/*-------------------------------------------
				 TSharedPtr

 레퍼런스 카운팅을 자동으로 관리하는 스마트 포인터
-------------------------------------------*/
template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() { }
	
	TSharedPtr(T* ptr) { Set(ptr); } // 생성자

	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); } // 복사 생성자
	
	TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs._ptr; rhs._ptr = nullptr; } // 이동 생성자
	
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); } // 상속 관계일 때의 복사 생성자

	~TSharedPtr() { Release(); } // 소멸자

public:
	TSharedPtr& operator=(const TSharedPtr& rhs) // 복사 연산자
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	TSharedPtr& operator=(TSharedPtr&& rhs) // 이동 연산자
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}

	bool operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; } // 비교 연산자 : 스마트 포인터끼리 비교
	bool operator==(T* ptr) const { return _ptr == ptr; } // 비교 연산자 : 스마트 포인터와 원시 포인터 비교
	bool operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; } // 비교 연산자 : 스마트 포인터끼리 비교
	bool operator!=(T* ptr) const { return _ptr != ptr; } // 비교 연산자 : 스마트 포인터와 원시 포인터 비교

	bool operator<(const TSharedPtr& rhs) { return _ptr < rhs._ptr; } // 대소 관계 연산자

	T* operator*() { return _ptr; } // 역참조 연산자
	const T* operator*() const { return _ptr; } // 역참조 연산자

	operator T*() const { return _ptr; } // T*로의 암시적 변환 연산자

	T* operator->() { return _ptr; } // 멤버 접근 연산자
	const T* operator->() const { return _ptr; } // 멤버 접근 연산자


	bool IsNull() { return _ptr == nullptr; } // 포인터가 nullptr인지 확인

public:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
			ptr->AddRef();
	}

	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}

private:
	T* _ptr;
};