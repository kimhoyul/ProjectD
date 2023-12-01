#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>

void HelloThread() // Thread Entry Point
{
	cout << "Hello Thread" << endl;
}

void HelloThread_2(int32 num) // Thread Entry Point
{
	cout << num << endl;
}

int main()
{
	vector<std::thread> v; // Vector of Threads

	for (int i = 0; i < 10; ++i)
	{
		v.push_back(std::thread(HelloThread_2, i)); // Create Thread
	}

	for (int i = 0; i < 10; ++i)
	{
		if (v[i].joinable()) // Check if Thread is Joinable
			v[i].join(); // Wait for Thread
	}

	cout << "Hello Main" << endl;
}
