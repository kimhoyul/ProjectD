#pragma once
#include "Types.h"

// ������ Ÿ�ӿ� �۾��� �̷������ ���� �̿��� Ÿ�� ĳ����
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

// ������ Ÿ�ӿ� �۾��� �̷������ ���� �̿��� Ÿ���� ���̸� ���ϴ� ���ø�
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

// ������ Ÿ�ӿ� �۾��� �̷������ ���� �̿��� �ε����� Ÿ���� ���ϴ� ���ø�
#pragma region TypeAt
template<typename TL, int32 Index>
struct TypeAt;

// ��� ���� �ε��� �޾Ƽ�
// �ε����� 0 �̸� ��带 ����
template<typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};

// ��� ���� �ε��� �޾Ƽ�
// �ε����� 0 �� �ƴϸ� ��带 ������, 
// ���ϰ� �ε��� - 1 �� ��������� ȣ��
template<typename Head, typename... Tail, int32 Index>
struct TypeAt<TypeList<Head, Tail...>, Index>
{
	using Result = typename TypeAt<TypeList<Tail...>, Index - 1>::Result;
};
#pragma endregion

// ������ Ÿ�ӿ� �۾��� �̷������ ���� �̿��� Ÿ���� �ε����� ���ϴ� ���ø�
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

// ������ Ÿ�ӿ� �۾��� �̷������ ���� �̿��� Ÿ���� ��ȯ�� �������� Ȯ���ϴ� ���ø�
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
		// TL �� ���̸� ���Ѵ�.
		length = Length<TL>::value 
	};

	TypeConversion()
	{
		// ��͸� ���� 2�� for ���� ���� ȣ��
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

	// j �� length �� �������� �Ʒ� �Լ��� ȣ��ǰ�, i + 1, j �� 0 ���� �ʱ�ȭ
	template<int32 i>
	static void MakeTable(int2Type<i>, int2Type<length>)
	{
		MakeTable(int2Type<i + 1>(), int2Type<0>());
	}

	// i �� length �� �������� ��Ͱ� ����ȴ�.
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

// To ��<> �ȿ� ��������� ����� Ÿ��
// From �� �Լ� ���ڷ� ���� Ÿ���� �����Ϸ��� �߷��ؼ� ä���ش�.
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