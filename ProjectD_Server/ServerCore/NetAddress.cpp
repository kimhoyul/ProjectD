#include "pch.h"
#include "NetAddress.h"

/*-------------------------------------------
				 NetAddress

			SOCKADDR_IN 헬퍼 클래스
  네트워크 주소를 편하게 사용하기 위해 만든 클래스
-------------------------------------------*/
NetAddress::NetAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr)
{
}

// ip와 port를 이용해서 SOCKADDR_IN 을 만드는 생성자
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
	// IPv4 또는 IPv6 ip 주소를 숫자 이진 형식(IN_ADDR)으로 변환
	::InetPtonW(AF_INET, ip, &address);
	return address;
}
