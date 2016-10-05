#pragma once 
#ifndef ____INCLUDE__INET__H____
#define ____INCLUDE__INET__H____
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
struct STRU_SESSION{
	long long  m_sock ;
	long m_data ;
	//在UDP中 m_sock高32位存放IP，低16位存放端口
	//在TCP中,m_sock低32位存放套接字socket 
};
class INotify{
public :
	virtual void NotiftyRecvData(  STRU_SESSION  *pSession ,char szbuf[],long lBuflen ) = 0  ;

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