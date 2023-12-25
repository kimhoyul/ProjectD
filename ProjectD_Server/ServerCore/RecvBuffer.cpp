#include "pch.h"
#include "RecvBuffer.h"

/*-------------------------------------------
				  RecvBuffer


-------------------------------------------*/

RecvBuffer::RecvBuffer(int32 bufferSize) : _bufferSize(bufferSize)
{
	_capacity = _bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		// dataSize 가 0이면 _readPos, _writePos 가 같은 위치고, 그렇다면 리셋 가능
		_readPos = _writePos = 0;
	}
	else
	{
		// 여유공간이 버퍼 1개 크기 미만이면, 데이터를 앞으로 땅긴다.
		if (FreeSize() < _bufferSize)
		{
			// dataSize 가 0이 아니면 _readPos, _writePos 가 다른 위치고, 
			// _readPos 위치부터 dataSize 만큼의 데이터를 _buffer[0] 위치로 복사
			::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			// _readPos 는 0으로
			_readPos = 0;
			// _writePos 는 dataSize 로 설정
			_writePos = dataSize;
		}
	}
}

// 읽은 데이터가 현재 읽어야할 데이터보다 크다면 false 반환
bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;
	return true;
}

// 쓴 데이터가 현재 빈공간 보다 크다면 false 반환
bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	_writePos += numOfBytes;
	return true;
}
