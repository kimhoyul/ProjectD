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
		  
		  // �����͸� ���� ��ġ�� �ּ� ��ȯ
	BYTE* ReadPos() { return &_buffer[_readPos]; }
		  // �����͸� �� ��ġ�� �ּ� ��ȯ
	BYTE* WritePos() { return &_buffer[_writePos]; }
		  // �������� ũ�� ��ȯ = ���� ��ġ ���� �б� ��ġ�� ���� ���� ���� ���� �������� ũ�Ⱑ ����
	int32 DataSize() { return _writePos - _readPos; }
		  // ������ ũ�� ��ȯ = ������ ũ�⿡�� ���� ��ġ�� ���� ���ۿ� ���� ũ�Ⱑ ����
	int32 FreeSize() { return _capacity - _writePos; }

private:
	// ����ũ�� * ���۰��� : readPos, writePos �� ��ĥ Ȯ���� ����
	int32 _capacity = 0;
	// ������ ũ��	
	int32 _bufferSize = 0;
	// ������ �б� ��ġ�� �����ϴ� ����
	int32 _readPos = 0;
	// ������ ���� ��ġ�� �����ϴ� ����
	int32 _writePos = 0;
	Vector<BYTE> _buffer;
};

