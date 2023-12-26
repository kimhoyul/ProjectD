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

