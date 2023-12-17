#pragma once
#include "Types.h"

// 컴파일 타임에 작업이 이루어지는 것을 이용한 타입 캐스팅
#pragma region TypeList
template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};

template<typename T, typename... U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = TypeList<U...>;
};
#pragma endregion

// 컴파일 타임에 작업이 이루어지는 것을 이용해 타입의 길이를 구하는 템플릿
#pragma region Length
template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum { value = 0 };
};

template<typename T, typename... U> 
struct Length<TypeList<T, U...>>
{
	enum { value = 1 + Length<TypeList<U...>>::value };
};
#pragma endregion

// 컴파일 타임에 작업이 이루어지는 것을 이용해 인덱스로 타입을 구하는 템플릿
#pragma region TypeAt
template<typename TL, int32 Index>
struct TypeAt;

// 헤드 테일 인덱스 받아서
// 인덱스가 0 이면 헤드를 리턴
template<typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};

// 헤드 테일 인데스 받아서
// 인덱스가 0 이 아니면 헤드를 날리고, 
// 테일과 인덱스 - 1 을 재귀적으로 호출
template<typename Head, typename... Tail, int32 Index>
struct TypeAt<TypeList<Head, Tail...>, Index>
{
	using Result = typename TypeAt<TypeList<Tail...>, Index - 1>::Result;
};
#pragma endregion

// 컴파일 타임에 작업이 이루어지는 것을 이용해 타입의 인덱스를 구하는 템플릿
#pragma region IndexOf
template<typename TL, typename T>
struct IndexOf;

template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum { value = 0 };
};

template<typename T>
struct IndexOf<TypeList<>, T>
{
	enum { value = -1 };
};

template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T>
{
private:
	enum { temp = IndexOf<TypeList<Tail...>, T>::value };

public:
	enum { value = temp == -1 ? -1 : temp + 1 };
};
#pragma endregion

// 컴파일 타임에 작업이 이루어지는 것을 이용해 타입을 변환이 가능한지 확인하는 템플릿
#pragma region Conversion
template<typename From, typename To>
class Conversion
{
private:
	using Small = __int8;
	using Big = __int32;

	static Small Test(const To&) { return 0; }
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }

public:
	enum 
	{ 
		exists = sizeof(Test(MakeFrom())) == sizeof(Small) 
	};
	
};;
#pragma endregion

#pragma region TypeCast
template<int32 v>
struct int2Type
{
	enum { value = v };
};

template<typename TL>
class TypeConversion
{
public:
	enum 
	{
		// TL 의 길이를 구한다.
		length = Length<TL>::value 
	};

	TypeConversion()
	{
		// 재귀를 통한 2중 for 문의 최초 호출
		MakeTable(int2Type<0>(), int2Type<0>());
	}

	template<int32 i, int32 j>
	static void MakeTable(int2Type<i>, int2Type<j>)
	{
		using FromType = typename TypeAt<TL, i>::Result;
		using ToType = typename TypeAt<TL, j>::Result;

		if (Conversion<FromType*, ToType*>::exists)
			s_convert[i][j] = true;
		else
			s_convert[i][j] = false;

		MakeTable(int2Type<i>(), int2Type<j + 1>());
	}

	// j 가 length 와 같아지면 아래 함수가 호출되고, i + 1, j 는 0 으로 초기화
	template<int32 i>
	static void MakeTable(int2Type<i>, int2Type<length>)
	{
		MakeTable(int2Type<i + 1>(), int2Type<0>());
	}

	// i 가 length 와 같아지면 재귀가 종료된다.
	template<int32 j>
	static void MakeTable(int2Type<length>, int2Type<j>)
	{
		
	}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static TypeConversion conversion;
		return s_convert[from][to];
	}

public:
	static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];

// To 는<> 안에 명시적으로 기록한 타입
// From 은 함수 인자로 받은 타입을 컴파일러가 추론해서 채워준다.
template<typename To, typename From>
To TypeCast(From* ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_cast<To>(ptr);

	return nullptr;
}

template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_pointer_cast<To>(ptr);

	return nullptr;
}

template<typename To, typename From>
bool CanCast(From* ptr)
{
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;

	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);

	return false;
}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;

	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);

	return false;
}
#pragma endregion

#define DECLARE_TL		using TL = TL; int32 _typeId;
#define INIT_TL(Type)	_typeId = IndexOf<TL, Type>::value;