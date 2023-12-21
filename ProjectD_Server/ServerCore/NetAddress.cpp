#include "pch.h"
#include "NetAddress.h"

/*-------------------------------------------
				 NetAddress

			SOCKADDR_IN ���� Ŭ����
  ��Ʈ��ũ �ּҸ� ���ϰ� ����ϱ� ���� ���� Ŭ����
-------------------------------------------*/
NetAddress::NetAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr)
{
}

// ip�� port�� �̿��ؼ� SOCKADDR_IN �� ����� ������
NetAddress::NetAddress(wstring ip, uint16 port)
{
	::memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr = Ip2Address(ip.c_str());
	_sockAddr.sin_port = ::htons(port);
}

wstring NetAddress::GetIpAdress()
{
	WCHAR buffer[100];
	::InetNtopW(AF_INET, &_sockAddr.sin_addr, buffer, len32(buffer));
	return wstring(buffer);
}

IN_ADDR NetAddress::Ip2Address(const WCHAR* ip)
{
	IN_ADDR address;
	// IPv4 �Ǵ� IPv6 ip �ּҸ� ���� ���� ����(IN_ADDR)���� ��ȯ
	::InetPtonW(AF_INET, ip, &address);
	return address;
}
