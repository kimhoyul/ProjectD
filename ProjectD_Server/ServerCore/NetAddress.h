#pragma once

/*-------------------------------------------
		   	     NetAddress

		    SOCKADDR_IN ���� Ŭ����
  ��Ʈ��ũ �ּҸ� ���ϰ� ����ϱ� ���� ���� Ŭ����
-------------------------------------------*/
class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(wstring ip, uint16 port);
	
	SOCKADDR_IN&	GetSockAddr() { return _sockAddr; }
	wstring			GetIpAdress();
	uint16			GetPort() { return ntohs(_sockAddr.sin_port); }

public:
	static IN_ADDR Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN _sockAddr = {};
};

