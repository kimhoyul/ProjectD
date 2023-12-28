#pragma once

/*-------------------------------------------
				  SendBuffer


-------------------------------------------*/
class SendBuffer : enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer();

	BYTE* Buffer() { return _buffer.data(); }
	int32 WriteSize() { return _writeSize; }
	int32 Capacity() { return static_cast<int32>(_buffer.size()); }

	void CopyData(void* data, int32 len);

private:
	// 실제 데이터를 보관하는 버퍼
	Vector<BYTE> _buffer;
	// 실제 버퍼에 쓰일 데이터 크기 (항상 버퍼 보다 작은 크기)
	int32 _writeSize = 0;
};

/*-------------------------------------------
			   SendBufferChunk

    버퍼들을 큰 덩어리로 할당 받고 쪼개서 
     사용 할 수 있도록 관리 하는 클래스
-------------------------------------------*/

class SendBufferChunk
{
	
};

/*-------------------------------------------
			  SendBufferManager

 샌드버퍼를 전역으로 사용하기위한 관리 클래스
-------------------------------------------*/
class SendBufferManager
{
public:
	SendBufferRef		Open(int32 size);

private:
	SendBufferChunkRef	Pop();
	void				Push(SendBufferChunkRef buffer);

	static void			PushGlobal(SendBufferChunk* buffer);

private:
	USE_LOCK;
	Vector<SendBufferChunkRef> _sendBufferChunks;
};