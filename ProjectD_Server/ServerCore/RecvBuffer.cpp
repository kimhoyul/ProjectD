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
		// dataSize �� 0�̸� _readPos, _writePos �� ���� ��ġ��, �׷��ٸ� ���� ����
		_readPos = _writePos = 0;
	}
	else
	{
		// ���������� ���� 1�� ũ�� �̸��̸�, �����͸� ������ �����.
		if (FreeSize() < _bufferSize)
		{
			// dataSize �� 0�� �ƴϸ� _readPos, _writePos �� �ٸ� ��ġ��, 
			// _readPos ��ġ���� dataSize ��ŭ�� �����͸� _buffer[0] ��ġ�� ����
			::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			// _readPos �� 0����
			_readPos = 0;
			// _writePos �� dataSize �� ����
			_writePos = dataSize;
		}
	}
}

// ���� �����Ͱ� ���� �о���� �����ͺ��� ũ�ٸ� false ��ȯ
bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;
	return true;
}

// �� �����Ͱ� ���� ����� ���� ũ�ٸ� false ��ȯ
bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	_writePos += numOfBytes;
	return true;
}
