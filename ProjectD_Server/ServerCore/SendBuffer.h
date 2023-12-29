#pragma once

class SendBufferChunk;

/*-------------------------------------------
				  SendBuffer


-------------------------------------------*/
class SendBuffer
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize);
	~SendBuffer();

	BYTE* Buffer() { return _buffer; }
	uint32 AllocSize() { return _allocSize; }
	uint32 WriteSize() { return _writeSize; }
	void Close(uint32 writeSize);

private:
	// 실제 데이터를 보관하는 버퍼
	BYTE* _buffer;
	uint32 _allocSize = 0;
	// 실제 버퍼에 쓰일 데이터 크기 (항상 버퍼 보다 작은 크기)
	uint32 _writeSize = 0;
	SendBufferChunkRef _owner;
};

/*-------------------------------------------
			   SendBufferChunk

    버퍼들을 큰 덩어리로 할당 받고 쪼개서 
     사용 할 수 있도록 관리 하는 클래스
-------------------------------------------*/
class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum 
	{
		SEND_BUFFER_CHUNK_SIZE = 6000,
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void Reset();
	SendBufferRef Open(uint32 allocSize);
	void Close(uint32 writeSize);

	bool IsOpen() { return _open; }
	BYTE* Buffer() { return &_buffer[_usedSize]; }
	uint32 FreeSize() {	return static_cast<uint32>(_buffer.size()) - _usedSize; }

private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE> _buffer = {};
	bool								_open = false;
	uint32								_usedSize = 0;
};

/*-------------------------------------------
			  SendBufferManager

 샌드버퍼를 전역으로 사용하기위한 관리 클래스
-------------------------------------------*/
class SendBufferManager
{
public:
	// 큰 덩이리에서 사용할 크기 만큼 뜯어 가는 함수
	SendBufferRef		Open(uint32 size);

private:
	SendBufferChunkRef	Pop();
	void				Push(SendBufferChunkRef buffer);

	static void			PushGlobal(SendBufferChunk* buffer);

private:
	USE_LOCK;
	Vector<SendBufferChunkRef> _sendBufferChunks;
};