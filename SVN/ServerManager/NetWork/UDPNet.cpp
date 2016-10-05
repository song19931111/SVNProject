#include "UDPNet.h"

long CUDPNet::GetHostIP()
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
bool CUDPNet::InnerInitNet()
{
	WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	m_udpSocket = socket(AF_INET,SOCK_DGRAM,0);
	sockaddr_in addr; 
	addr.sin_family  =AF_INET;
	addr.sin_addr.S_un.S_addr = GetHostIP();
	addr.sin_port = DEF_UDP_PORT;
	int ret; 
	//为了广播，必须绑定一个端口
	ret = bind(m_udpSocket,( sockaddr * )&addr,sizeof(addr));
	if ( ret == SOCKET_ERROR )
	{
		return false ;
	}
	//设置socket具有广播属性:
	int iFlag = 1; 
	setsockopt(m_udpSocket,SOL_SOCKET,SO_BROADCAST,(char * )iFlag,sizeof( iFlag ));
	return true; 
}
 long CUDPNet::SendData( STRU_SESSION *pSession,char szBuf[],long lBuflen ) 
 {
	//构造addr
	 sockaddr_in addr; 
	 addr.sin_family = AF_INET;
	 addr.sin_addr.S_un.S_addr = (pSession->m_sock&((unsigned long )0))>>32;
	 addr.sin_port = ( unsigned short  )pSession->m_sock;
	 if (! m_udpSocket )
	 {
		return false ;
	 }
	 return sendto(m_udpSocket,szBuf,lBuflen,0,( sockaddr * )&addr,sizeof( addr ) );
 }

bool CUDPNet::Init( INotify *pNotify )
{
	
	if ( false == UnInit() )
	{
		return false ;
	}
	m_pNotify  = pNotify;

	if( false == InnerInitNet() )
	{
		return false; 
	}
	m_bRun = true;
	HANDLE handle = ( HANDLE )_beginthreadex(NULL,0,RecvProc,this,0,NULL);
	if (handle)
	{
		CloseHandle(handle);
	}

}
bool CUDPNet::UnInit() 
{
	 m_bRun = false ;
	 while ( m_iThreadCount != 0  )
	 {
		 Sleep(1);
	 }
	 MAP_UDP_SESSION ::iterator ite= m_mp_session.begin();
	 while( ite !=  m_mp_session.end() )
	 {
		 delete (STRU_SESSION *)ite->second;
		ite++;
	 }
	 m_pNotify  =NULL;
	 if ( m_udpSocket )
	 {
		 closesocket(m_udpSocket);
		 m_udpSocket = NULL;
		 WSACleanup();
	 }

	 m_mp_session.clear();
	 return true;
}
 unsigned int __stdcall CUDPNet::RecvProc( void *param )
 {
	 CUDPNet   * pThis =  ( CUDPNet *  ) param; 
	 pThis->RecvFun();
	 return 1L;
 }
void CUDPNet::RecvFun()
{
	while( m_bRun )
	{
			//设置accpet 为非阻塞的:
			u_long flag = 1; 
			InterlockedIncrement(( long * )&m_iThreadCount);
			timeval time_val;
			time_val.tv_usec = 10;
			fd_set fd_read;
			int ret  =  0 ;  
			char szRecvBuf[DEF_MAX_RECV_BUF];
			while ( m_bRun )
			{

				FD_ZERO(&fd_read);
				//加入待检测的结合:
				FD_SET(m_udpSocket,&fd_read);
				 ret = select(0,&fd_read,NULL,NULL,&time_val);
				 if ( ret == 0)
				 {
					 //超时
					continue ;
				 }
				 else if ( ret ==SOCKET_ERROR )
				 {
					break;
				 }
				 else
				 {				 		
						//检测集合是否发生了recv :
					 if ( FD_ISSET(m_udpSocket,&fd_read) > 0 )
					{
						sockaddr_in client_addr ;
						int iLen = sizeof( client_addr );
						int iRet  =recvfrom(m_udpSocket,szRecvBuf,sizeof( szRecvBuf ),0,(sockaddr *)&client_addr,&iLen);
						long long i64Data ;
						i64Data = ( ( long long  )client_addr.sin_addr.S_un.S_addr)<<32 +( unsigned short )client_addr.sin_port;
						if ( iRet >  0  )
						{
							//重组数据:高32位为IP，低16位为端口(高字节)
						
							MAP_UDP_SESSION ::iterator ite ; 
							STRU_SESSION * pRemSession  = NULL;
							m_lock_mp_session.Lock();
							if ( ( ite = m_mp_session.find( i64Data  ) )  != m_mp_session.end()  )
							{
								pRemSession = ite->second;
								m_lock_mp_session.UnLock();
								//找到了这个信息结构
								if ( m_pNotify )
								{
									m_pNotify->NotiftyRecvData(pRemSession,szRecvBuf,iRet);
								}
							
							}
							else
							{
								//加入到map客户端表中，向相当于TCP的accpet 
								STRU_SESSION *pSession = new STRU_SESSION;
								pSession->m_sock = i64Data;
								m_lock_mp_session.Lock();
								m_mp_session[ i64Data ] = pSession;
								m_lock_mp_session.UnLock();
							}
						}
					 }
				}
			}
			InterlockedDecrement(( long * )& m_iThreadCount);
	}
}
bool CUDPNet::RemoveSession(STRU_SESSION *pSession)
{
	MAP_UDP_SESSION::iterator ite ;
	m_lock_mp_session.Lock();
	if( (ite =m_mp_session.find(pSession->m_sock) ) == m_mp_session.end() )
	{
		m_lock_mp_session.UnLock();
		return false; 
	}
	delete (STRU_SESSION *)ite->second;
	ite = m_mp_session.erase(ite);
	m_lock_mp_session.UnLock();
	return true; 
}