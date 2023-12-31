#pragma once
#include "BufferReader.h"
#include "BufferWriter.h"

enum
{
	S_TEST = 1
};

template<typename T, typename C>
class PacketIterator
{
public:
	PacketIterator(C& container, uint16 index) : _container(container), _index(index) { }

	bool operator!=(const PacketIterator& other) const { return _index != other._index; }
	const T& operator*() const { return _container[_index]; }
	T& operator*() { return _container[_index]; }
	T* operator->() { return &_container[_index]; }
	PacketIterator& operator++() { ++_index; return *this; }
	PacketIterator operator++(int32) { PacketIterator ret = *this; ++_index; return ret; }

	PacketIterator& operator--() { --_index; return *this; }


private:
	C& _container;
	uint16 _index;
};

template<typename T>
class PacketList
{
public:
	PacketList() : _data(nullptr), _count(0) { }
	PacketList(T* data, uint16 count) : _data(data), _count(count) { }

	T& operator[](uint16 index)
	{
		ASSERT_CRASH(index < _count);
		return _data[index];
	}

	uint16 Count() { return _count; }

	PacketIterator<T, PacketList<T>> begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
	PacketIterator<T, PacketList<T>> end() { return PacketIterator<T, PacketList<T>>(*this, _count); }

private:
	T* _data;
	uint16 _count;
};

class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);
};

#pragma pack(1)
// 패킷 설계 TEMP
// [고정 데이터(PKT_S_TEST)][가변데이터(BuffsListItem)(BuffsListItem)(BuffsListItem)]
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 bufferId;
		float remainTime;

		uint16 victimsOffset;
		uint16 victimsCount;
	};

	uint16 packetSize; // 공용 헤더
	uint16 packetId; // 공용 헤더

	// 고정 데이터
	uint64 id;
	uint32 hp;
	uint16 attack;

	uint16 buffsOffeset;
	uint16 buffsCount;
};

// [고정 데이터(PKT_S_TEST)][가변데이터(BuffsListItem)(BuffsListItem)(BuffsListItem)]
class PKT_S_TEST_WRITE
{
public:
	using BuffsListItem = PKT_S_TEST::BuffsListItem;
	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	using BuffVictimsList = PacketList<uint64>;

	PKT_S_TEST_WRITE(uint64 id, uint32 hp, uint16 attack)
	{
		_sendBuffer = GSendBufferManager->Open(4096);
		_bw = BufferWriter(_sendBuffer->Buffer(), _sendBuffer->AllocSize());
		_pkt = _bw.Reserve<PKT_S_TEST>();
		_pkt->packetSize = 0; // To fill
		_pkt->packetId = S_TEST;
		_pkt->id = id;
		_pkt->hp = hp;
		_pkt->attack = attack;
		_pkt->buffsOffeset = 0; // To fill
		_pkt->buffsCount = 0; // To fill
	}

	BuffsList ReserveBuffsList(uint16 buffCount)
	{
		BuffsListItem* firstBuffsListItem = _bw.Reserve<BuffsListItem>(buffCount);
		_pkt->buffsOffeset = (uint64)firstBuffsListItem - (uint64)_pkt;
		_pkt->buffsCount = buffCount;
		return BuffsList(firstBuffsListItem, buffCount);
	}

	BuffVictimsList ReserveBuffVictimsList(BuffsListItem* buffsitem, uint16 victimCount)
	{
		uint64* firstVictimsListItem = _bw.Reserve<uint64>(victimCount);
		buffsitem->victimsOffset = (uint64)firstVictimsListItem - (uint64)_pkt;
		buffsitem->victimsCount = victimCount;
		return BuffVictimsList(firstVictimsListItem, victimCount);
	}

	SendBufferRef CloseAndReturn()
	{
		_pkt->packetSize = _bw.WriteSize();
		_sendBuffer->Close(_bw.WriteSize());
		return _sendBuffer;
	
	}

private:
	PKT_S_TEST* _pkt = nullptr;
	SendBufferRef _sendBuffer;
	BufferWriter _bw;
};
#pragma pack()