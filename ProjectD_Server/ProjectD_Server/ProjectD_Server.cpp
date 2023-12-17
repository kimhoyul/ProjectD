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
#include "Memory.h"
#include "Allocator.h"

using TL = TypeList<class Player, class Mage, class Knight, class Archer>;

class Player
{
public:
	Player()
	{
		INIT_TL(Player);
	}

	virtual ~Player() {}

	DECLARE_TL
};

class Knight : public Player
{
public:
	Knight() { INIT_TL(Knight); }

};

class Mage : public Player
{
public:
	Mage() { INIT_TL(Mage); }

};

class Archer : public Player
{
public:
	Archer() { INIT_TL(Archer); }

};

class Dog
{

};

int main()
{	
	//TypeList<Mage, Knight>::Head whoAMI;							// Mage
	////        ^      ^   
	////       Head   Tail
	//TypeList<Mage, Knight>::Tail whoAMI2;							// Knight
	////        ^      ^   
	////       Head   Tail
	//TypeList<Mage, TypeList<Knight, Archer>>::Head whoAMI3;			// Mage
	////        ^      ^        ^         ^
	////       Head   Tail  Tail::Head  Tail::Tail
	//TypeList<Mage, TypeList<Knight, Archer>>::Tail::Head whoAMI4;	// Knight
	////        ^      ^        ^         ^
	////       Head   Tail  Tail::Head  Tail::Tail
	//TypeList<Mage, TypeList<Knight, Archer>>::Tail::Tail whoAMI5;	// Archer
	////        ^      ^        ^         ^
	////       Head   Tail  Tail::Head  Tail::Tail

	//int32 len1 = Length<TypeList<Mage, Knight>>::value;				// 2
	//int32 len2 = Length<TypeList<Mage, Knight, Archer>>::value;		// 3

	//using TL = TypeList<Player, Mage, Knight, Archer>;
	//TypeAt<TL, 0>::Result whoAMI6;	
	//TypeAt<TL, 1>::Result whoAMI7;	
	//TypeAt<TL, 2>::Result whoAMI8;	

	//IndexOf<TL, Mage>::value;
	//IndexOf<TL, Knight>::value;
	//IndexOf<TL, Dog>::value;
	//
	//bool canConvert1 = Conversion<Knight, Player>::exists; 
	//bool canConvert2 = Conversion<Player, Knight>::exists; 
	//bool canConvert3 = Conversion<Knight, Mage>::exists;   
	//bool canConvert4 = Conversion<Dog, Player>::exists;   
	
	{ // 생포인터 예시
		Player* player = new Knight();

		// C# 의 is 와 같은 기능
		bool canCast = CanCast<Knight*>(player);
		
		// C# 의 as 와 같은 기능
		Knight* knight = TypeCast<Knight*>(player);

		delete player;
	}


	{ // 스마트 포인터 예시
		shared_ptr<Player> player = MakeShared<Knight>();

		shared_ptr<Archer> archer = TypeCast<Archer>(player);

		bool canCast = CanCast<Mage>(player);
	}

	for (int32 i = 0; i < 2; ++i)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					
				}
			}
		);
	}

	GThreadManager->Join();
}