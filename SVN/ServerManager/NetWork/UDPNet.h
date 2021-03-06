#pragma once 
#ifndef ____INCLUDE__UDPNET__H____
#define ____INCLUDE__UDPNET__H____
#include "INet.h"
#include <map>
#include "Common.h"
#include "MyLock.h"
#include <process.h>
using namespace std; 
typedef map<long long ,STRU_SESSION *> MAP_UDP_SESSION;
class CUDPNet : public INet{
public :
	CUDPNet():m_iThreadCount(0),m_bRun(true),m_udpSocket(0)
	{
	
	}
	virtual long SendData( char szBuf[],long lBuflen ) ;

	virtual bool Init( INotify *m_pNotify );
	virtual bool UnInit() ;
	
	long GetHostIP();
	
	bool Close()
	{
		return  this->UnInit();
	}
private  :
	static unsigned int __stdcall RecvProc( void *param );
	void RecvFun();
	bool InnerInitNet();
	bool RemoveSession(STRU_SESSION *pSession); //提供给外部调用
	private :
		SOCKET m_udpSocket ;
	//	MAP_UDP_SESSION m_mp_session ;
		bool m_bRun ; 
		int  m_iThreadCount ;
		MyLock m_lock_mp_session; 
		STRU_SESSION m_Serversession ; //服务器的Session
	bool NofityNetDisconnet( STRU_SESSION *pSession  ) 
	{
		return this->RemoveSession( pSession);
	}
};

#endif //____INCLUDE__UDPNET__H____