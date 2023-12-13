#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

/*-------------------------------------------
			     MemoryHeader

              [MemoryHeader][Data]
         메모리 헤더를 붙여서 메모리를 할당
-------------------------------------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size); //placement new
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
	// TODO : 필요한 추가 정보
};

/*-------------------------------------------
				 MemoryPool

  [32byte][64byte][128byte][256byte][ ... ]
   동일한 크기의 데이터 끼리 모아주는 메모리 풀
-------------------------------------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void Push(MemoryHeader* ptr); // 메모리 풀에 메모리를 넣음
	MemoryHeader* Pop(); // 메모리 풀에서 메모리를 꺼내옴

private:
	SLIST_HEADER _header; // 메모리 풀의 헤더
	int32 _allocSize = 0; // 어떠한 크기의 메모리 풀 인지
	atomic<int32> _allocCount = 0; // 몇개의 메모리가 할당되었는지
};

