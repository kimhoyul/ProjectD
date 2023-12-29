#include "pch.h"
#include "BufferReader.h"

BufferReader::BufferReader()
{
	
}

BufferReader::BufferReader(BYTE* buffer, uint32 size, uint32 pos)
	: _buffer(buffer), _size(size), _pos(pos)
{
	
}

BufferReader::~BufferReader()
{
	
}

bool BufferReader::Peek(void* dest, uint32 len)
{
	if (FreeSize() < len)
		return false;

	// 받아온 dest의 주소에 있는 값에다가 _buffer의 _pos부터 len만큼 복사해준다.
	::memcpy(dest, &_buffer[_pos], len);
	return true;
}

bool BufferReader::Read(void* dest, uint32 len)
{
	if (Peek(dest, len) == false)
		return false;

	_pos += len;
	return true;
}
