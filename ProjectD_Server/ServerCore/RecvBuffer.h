#pragma once

/*-------------------------------------------
				  RecvBuffer


-------------------------------------------*/
class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	void Clean();
	bool OnRead(int32 numOfBytes);
	bool OnWrite(int32 numOfBytes);
		  
		  // 데이터를 읽을 위치의 주소 반환
	BYTE* ReadPos() { return &_buffer[_readPos]; }
		  // 데이터를 쓸 위치의 주소 반환
	BYTE* WritePos() { return &_buffer[_writePos]; }
		  // 데이터의 크기 반환 = 쓰기 위치 에서 읽기 위치를 빼면 아직 읽지 않은 데이터의 크기가 나옴
	int32 DataSize() { return _writePos - _readPos; }
		  // 버퍼의 크기 반환 = 버퍼의 크기에서 쓰기 위치를 빼면 버퍼에 남은 크기가 나옴
	int32 FreeSize() { return _capacity - _writePos; }

private:
	// 버퍼크기 * 버퍼개수 : readPos, writePos 가 겹칠 확률을 높임
	int32 _capacity = 0;
	// 버퍼의 크기	
	int32 _bufferSize = 0;
	// 데이터 읽기 위치를 저장하는 변수
	int32 _readPos = 0;
	// 데이터 쓰기 위치를 저장하는 변수
	int32 _writePos = 0;
	Vector<BYTE> _buffer;
};

