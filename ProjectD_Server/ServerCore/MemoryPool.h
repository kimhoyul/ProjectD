#pragma once



/*-------------------------------------------
			     MemoryHeader

              [MemoryHeader][Data]
         �޸� ����� �ٿ��� �޸𸮸� �Ҵ�
-------------------------------------------*/
struct MemoryHeader
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
	// TODO : �ʿ��� �߰� ����
};

/*-------------------------------------------
				 MemoryPool

  [32byte][64byte][128byte][256byte][ ... ]
   ������ ũ���� ������ ���� ����ִ� �޸� Ǯ
-------------------------------------------*/
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void Push(MemoryHeader* ptr); // �޸� Ǯ�� �޸𸮸� ����
	MemoryHeader* Pop(); // �޸� Ǯ���� �޸𸮸� ������

private:
	int32 _allocSize = 0; // ��� ũ���� �޸� Ǯ ����
	atomic<int32> _allocCount = 0; // ��� �޸𸮰� �Ҵ�Ǿ�����

	USE_LOCK;
	queue<MemoryHeader*> _queue;
};

