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
	// ���� �����͸� �����ϴ� ����
	Vector<BYTE> _buffer;
	// ���� ���ۿ� ���� ������ ũ�� (�׻� ���� ���� ���� ũ��)
	int32 _writeSize = 0;
};

/*-------------------------------------------
			   SendBufferChunk

    ���۵��� ū ����� �Ҵ� �ް� �ɰ��� 
     ��� �� �� �ֵ��� ���� �ϴ� Ŭ����
-------------------------------------------*/

class SendBufferChunk
{
	
};

/*-------------------------------------------
			  SendBufferManager

 ������۸� �������� ����ϱ����� ���� Ŭ����
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