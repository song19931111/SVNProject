#pragma once 
#ifndef ____INCLUDE__INET__H____
#define ____INCLUDE__INET__H____
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
struct STRU_SESSION{
	long m_data ;
	SOCKET m_sock ;
};
class INotify{
public :
virtual void NotiftyRecvData(  SOCKET socket ,char szbuf[],long lBuflen ) = 0  ;

};


class INet{
public :
	virtual long SendData( STRU_SESSION *pSession,char szBuf[],long lBuflen ) =  0;
	virtual bool Init( INotify *m_pNotify ) = 0;
	virtual bool UnInit() = 0;
public :
	INotify *m_pNotify; 


};
#endif //____INCLUDE__INET__H____