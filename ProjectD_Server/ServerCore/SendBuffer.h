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
