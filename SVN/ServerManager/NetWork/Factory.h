#pragma once 
#ifndef ____INCLUDE__FACTORY__H____
#define ____INCLUDE__FACTORY__H____
#include "INet.h"
#include "TCPNet.h"
#include "UDPNet.h"
enum NET_TYPE{enum_tcp,enum_udp};
class CFactoryNet{
public :
	virtual INet * CreateNetObject ( int type  )
	{
		switch ( type )
		{
		case enum_tcp:
			return new CTCPNet;
			break;

		case enum_udp:
			return new CUDPNet;
			break;
			
		}
	}

};
#endif //____INCLUDE__FACTORY__H____