#pragma once 
#ifndef ____INCLUDE__TCPNET__H____
#define ____INCLUDE__TCPNET__H____
#include "INet.h"
#include <map>
#include <process.h>
using namespace std;
typedef map<SOCKET,STRU_SESSION *> MAP_SESSION;
#define DEF_MAX_RECV_BUF (2048) 
#define DEF_PORT (12345)
class CTCPNet : public INet{
	CTCPNet():m_bRun(true),m_iThreadCount(0)
	{
	
	}
	~CTCPNet()
	{
	
	}
	 long SendData( STRU_SESSION *pSession,char szBuf[],long lBuflen ) ;
	 bool Init( INotify *m_pNotify ) ;
	 bool UnInit() ;
	 long GetHostIP();
	 static unsigned int _stdcall AcceptProc( void * pParam ) ; 
	 static unsigned int _stdcall RecvProc( void * pParam ) ;
private :
	bool InnerInitNet(); 
	bool m_bRun ;
	int m_iThreadCount ; 
	SOCKET  m_listenSocket ;
	MAP_SESSION m_mp_socket_session;
	char m_szRecvBuf [DEF_MAX_RECV_BUF] ;
	
};
#endif //____INCLUDE__TCPNET__H____