#include "TCPNet.h"

bool CTCPNet::InnerInitNet()
{
	WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	m_listenSocket = socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in addr; 
	addr.sin_family  =AF_INET;
	addr.sin_addr.S_un.S_addr = GetHostIP();
	addr.sin_port = htons( DEF_TCP_PORT ) ;
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
		Sleep(1);
	}
	//清空map:
	MAP_SESSION::iterator ite  =  m_mp_socket_session.begin();
	while( ite != m_mp_socket_session.end()  )
	{
		delete (STRU_SESSION * )ite->second;
		ite++;
	}
	m_mp_socket_session.clear();
	
	if ( m_listenSocket )
	{
		closesocket(m_listenSocket);
		m_listenSocket = NULL;
		WSACleanup();
	}
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
			pThis->m_lock.Lock();
			pThis ->m_mp_socket_session[ ret ] =pSession;
			pThis->m_lock.UnLock();
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
			MAP_SESSION::iterator ite_map ; 
			
			MAP_SESSION mp_copy; 
			pThis->m_lock.Lock();
			//拷贝一份map ,用空间去换减少锁的粒度
			mp_copy = pThis->m_mp_socket_session;
			pThis->m_lock.UnLock();
			while ( ite_map != mp_copy.end() )
			{
				IsHavaClient = true  ;
				//加入集合
				FD_SET(ite_map->first,&fd_read);

				ite_map ++; 
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
				 ite_map   = mp_copy.begin();
					while ( ite_map != mp_copy.end() )
				{
				
					//检测集合是否发生了recv :
					if ( FD_ISSET(ite_map->first,&fd_read) > 0 )
					{
						int nRecv = recv(ite_map->first,pThis->m_szRecvBuf,sizeof( m_szRecvBuf ),0);
						if ( SOCKET_ERROR == nRecv || 0 == nRecv  )
						{
							//从map中移除::	
							pThis->RemoveSession(ite_map->first);
							
						}
						else 
						{
							if ( pThis->m_pNotify )
							{
								pThis->m_pNotify->NotiftyRecvData(ite_map->second,pThis->m_szRecvBuf,nRecv,enum_tcp_type);
								memset(pThis ->m_szRecvBuf,0,DEF_MAX_RECV_BUF);
							}
							
						}
					}

						ite_map ++ ;	
				}
			 }
		}

		InterlockedDecrement(( long * )&pThis->m_iThreadCount);
		return 1L;
 }
 bool  CTCPNet::RemoveSession( SOCKET socket  )
 {
	 m_lock.Lock();
	 MAP_SESSION::iterator ite  = m_mp_socket_session.find(socket);
	 if( ite ==m_mp_socket_session.end()  )   
	 {
		m_lock.UnLock();
		return false ;
	 }
	 delete (STRU_SESSION * )ite->second;
	 ite = m_mp_socket_session.erase(ite);
	 m_lock.UnLock();
	 closesocket(socket);
	 return true ;
 }
  long CTCPNet::GetHostIP()
  {
	  unsigned long lValidIP =inet_addr("127.0.0.1"); 
	  char szHostName[255];
	  if ( 0 != gethostname(szHostName,sizeof( szHostName ) ) ) 
	  {
		 //无法获取到主机名:
		  return lValidIP;
	  }
	  hostent * pHostNet  = gethostbyname(szHostName);
	  if (pHostNet->h_addr_list [ 0 ] !=NULL && pHostNet->h_length == 4  )
	  {
		return  * ( long * )pHostNet->h_addr_list [ 0 ];
	  }
  }