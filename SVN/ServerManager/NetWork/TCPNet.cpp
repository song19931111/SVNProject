#include "TCPNet.h"

bool CTCPNet::InnerInitNet()
{
	WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	m_listenSocket = socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in addr; 
	addr.sin_family  =AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr ( DEF_SERVER_IP  );
	addr.sin_port = htons( DEF_TCP_PORT ) ;
	int ret = connect(m_listenSocket,( sockaddr_in * )&addr,sizeof( addr ) );
	if ( ret == SOCKET_ERROR )
	{
		return false ;

	}
	return true; 
}
bool CTCPNet::Init( INotify *pNotify )
{
	if( false == UnInit() )
	{
		return false ;
	}
	if (false == InnerInitNet())
	{
		return false; 
	}
	m_pNotify = pNotify;
	//开启线程
	m_bRun = true; 
	m_session.m_sock  = m_listenSocket;
	HANDLE handle =  ( HANDLE )_beginthreadex(NULL,0,RecvProc,this,0,NULL); 
	if ( handle != NULL )
	{
		CloseHandle ( handle );
		handle = NULL;
	}
	
	return true ;
}
long CTCPNet::SendData( char szBuf[],long lBuflen )
{
	//一般情况下，发送缓冲区不会满
	return send(m_listenSocket,szBuf,lBuflen,0);
}

bool CTCPNet::UnInit()
{
	m_bRun  =false ;
	while ( m_iThreadCount != 0  )
	{
		Sleep(1);
	}
	//清空map:

	if ( m_listenSocket )
	{
		closesocket(m_listenSocket);
		m_listenSocket = NULL;
		WSACleanup();
	}
	return true ;
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
			FD_SET(pThis->m_listenSocket,&fd_read);
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
					//检测集合是否发生了recv :
				 if ( FD_ISSET(pThis ->m_listenSocket,&fd_read) > 0 )
					{
						int nRecv = recv(pThis ->m_listenSocket,pThis->m_szRecvBuf,sizeof( m_szRecvBuf ),0);
						if ( SOCKET_ERROR == nRecv || 0 == nRecv  )
						{
							pThis->RemoveSession(pThis ->m_listenSocket);
						}
						else 
						{
							if ( pThis->m_pNotify )
							{
								pThis->m_pNotify->NotiftyRecvData(&pThis ->m_session,pThis->m_szRecvBuf,nRecv,enum_tcp_type);
								memset(pThis ->m_szRecvBuf,0,DEF_MAX_RECV_BUF);
							}
							
						}
					}

				}
			 }
		InterlockedDecrement(( long * )&pThis->m_iThreadCount);
		return 1L;
 }
 bool  CTCPNet::RemoveSession( SOCKET socket  )
 {
	 closesocket(socket);
	 return true ;
 }
