#include "TCPNet.h"

bool CTCPNet::InnerInitNet()
{
	m_listenSocket = socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in addr; 
	addr.sin_family  =AF_INET;
	addr.sin_addr.S_un.S_addr = GetHostIP();
	addr.sin_port = DEF_PORT;
	int ret; 
	ret = bind(m_listenSocket,( sockaddr * )&addr,sizeof(addr));
	if ( ret == SOCKET_ERROR )
	{
		return false ;
	}
	ret = listen(m_listenSocket,SOMAXCONN );
	if ( ret == SOCKET_ERROR )
	{
		return false ;

	}
	return true; 
}
bool CTCPNet::Init( INotify *pNotify )
{
	UnInit();
	if (false == InnerInitNet())
	{
		return false; 
	}
	m_pNotify = pNotify;
	//开启线程
	HANDLE handle =  ( HANDLE )_beginthreadex(NULL,0,AcceptProc,this,0,NULL);
	if ( handle != NULL )
	{
		CloseHandle ( handle );
		handle = NULL;
	}
	handle =  ( HANDLE )_beginthreadex(NULL,0,RecvProc,this,0,NULL); 
	if ( handle != NULL )
	{
		CloseHandle ( handle );
		handle = NULL;
	}
	return true ;
}
long CTCPNet::SendData( STRU_SESSION *pSession,char szBuf[],long lBuflen )
{
	//一般情况下，发送缓冲区不会满
	return send(pSession->m_sock,szBuf,lBuflen,0);
}

bool CTCPNet::UnInit()
{
	m_bRun  =false ;
	while ( m_iThreadCount != 0  )
	{
		Sleep(100);
	}
	//清空map:
	MAP_SESSION::iterator ite  =  m_mp_socket_session.begin();
	while( ite != m_mp_socket_session.end()  )
	{
		delete ite->second;
		ite++;
	}
	m_mp_socket_session.clear();
	closesocket(m_listenSocket);
	return true ;
}
 unsigned int _stdcall CTCPNet::AcceptProc( void * pParam ) 
{
	CTCPNet *pThis =  ( CTCPNet * )pParam;
	//设置accpet 为非阻塞的:
	u_long flag = 1; 
	InterlockedIncrement(( long * )&pThis->m_iThreadCount);
	ioctlsocket(pThis->m_listenSocket,FIONBIO,&flag );
	while ( pThis->m_bRun )
	{
		int ret  = accept(pThis->m_listenSocket,NULL,NULL);
		if ( ret ==SOCKET_ERROR )
		{
				if ( ret == WSAEWOULDBLOCK  )
			{
				continue ;
			}
		}
		else
		{
			STRU_SESSION *pSession;  
			pSession->m_sock =  ret ;
			pThis ->m_mp_socket_session[ ret ] =pSession;
		}
		
	
	}
	InterlockedDecrement(( long * )&pThis->m_iThreadCount);
}
 unsigned int _stdcall CTCPNet::RecvProc( void * pParam ) 
 {
	 CTCPNet *pThis =  ( CTCPNet * )pParam;
		//设置accpet 为非阻塞的:
		u_long flag = 1; 
		InterlockedIncrement(( long * )&pThis->m_iThreadCount);
		timeval time_val;
		time_val.tv_usec = 10;
		fd_set fd_read;
		int ret  =  0 ;  
		while ( pThis ->m_bRun )
		{

			FD_ZERO(&fd_read);
			//加入待检测的结合:
			bool IsHavaClient  = false; 
			MAP_SESSION::iterator ite   = pThis->m_mp_socket_session.begin();
			while ( ite != pThis->m_mp_socket_session.end() )
			{
				IsHavaClient = true  ;
				//加入集合
				FD_SET(ite->first,&fd_read);

				ite ++; 
			}
			if ( !IsHavaClient )
			{
				continue ;
			}
			 ret = select(0,&fd_read,NULL,NULL,&time_val);
			 if ( ret == 0)
			 {
				continue ;
			 }
			 else if ( ret ==SOCKET_ERROR )
			 {
				break;
			 }
			 else
			 {
					while ( ite != pThis->m_mp_socket_session.end() )
				{
				
					//检测集合是否发生了recv :
					if ( FD_ISSET(ite->first,&fd_read) )
					{
						int nRecv = recv(ite->first,pThis->m_szRecvBuf,sizeof( m_szRecvBuf ),0);
						if ( SOCKET_ERROR == nRecv || 0 == nRecv  )
						{
							//从map中移除::
							ite = pThis->m_mp_socket_session.erase(ite);
						}
						else 
						{
							if ( pThis->m_pNotify )
							{
								pThis->m_pNotify->NotiftyRecvData(ite->first,pThis->m_szRecvBuf,nRecv);
								memset(pThis ->m_szRecvBuf,0,DEF_MAX_RECV_BUF);
							}
						}
					}
						ite++;
				}
			 
			 }
		}

		InterlockedDecrement(( long * )&pThis->m_iThreadCount);
 }
