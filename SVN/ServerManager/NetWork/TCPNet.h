#pragma once 
#ifndef ____INCLUDE__TCPNET__H____
#define ____INCLUDE__TCPNET__H____
#include "INet.h"
#include "MyLock.h"
#include <map>
#include <process.h>
#include "Common.h"
using namespace std;
#define DEF_MAX_RECV_BUF (2048) 


class CTCPNet : public INet{
public :
	CTCPNet():m_bRun(true),m_iThreadCount(0)
	{
	
	}
	~CTCPNet()
	{
	
	}
	 long SendData( char szBuf[],long lBuflen ) ;
	 bool Init( INotify *m_pNotify ) ;
	 bool UnInit() ;
	 static unsigned int _stdcall RecvProc( void * pParam ) ;
private :
	bool InnerInitNet(); 
	bool m_bRun ;
	int m_iThreadCount ; 
	STRU_SESSION  m_session ;
	SOCKET  m_listenSocket ;
	MyLock m_lock;
	char m_szRecvBuf [DEF_MAX_RECV_BUF] ;
	bool RemoveSession( SOCKET socket  );
	bool Close()
	{
		return this->UnInit();
	}
	
	bool NofityNetDisconnet( STRU_SESSION *pSession  ) 
	{
		return this->RemoveSession( pSession->m_sock);
	}
};
#endif //____INCLUDE__TCPNET__H____