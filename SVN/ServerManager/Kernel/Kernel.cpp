#include "Kernel.h"
//初始化
bool  CKernel::OpenKernel( IUIToKernel *pUI  )
{
	if ( false == CloseKernel() )
	{
		return false; 
	}
	m_pUI = pUI;
	//初始化网络(本来应该是外部加入观察者集合):
	m_pNet= new CUDPNet;
	//加入观察者集合:
	AddObserver(enum_udp_type,m_pNet);

	//初始化索引池:
	m_indexQueue_pack.InitQueue(DEF_MAX_PACK_COUNT);
	for( int i=0;i<DEF_MAX_PACK_COUNT ;i++  )
	{
		m_indexQueue_pack.Push( i ) ; 
	}
	//创建处理线程:
	m_bRun  = true ;
	HANDLE handle  = ( HANDLE )_beginthreadex(NULL,0,DealThreadProc,this,0,NULL);
	if (handle )
	{
		CloseHandle( handle );
	}
}
 bool  CKernel::AddObserver (int key , IObServer *  pObserver ) 
 {
	 if (m_mp_observer.find(  key  )  !=  m_mp_observer.end() )
	 {
		return false; 
	 }
	 m_mp_observer[ key ]  = pObserver;
	 return true ; 
}
bool CKernel:: RemObserver ( IObServer *  pObserver ) 
{
	MAP_OBSERVER::iterator ite = m_mp_observer.begin();
	while ( ite !=  m_mp_observer.end() )
	{
		if ( ite->second == pObserver ) 
		{
			ite = m_mp_observer.erase(ite ); 
			return true; 
		}
		ite++;
	}
	return true ; 
}
//反初始化
bool  CKernel::CloseKernel(  )
{
	Close();
}
bool  CKernel::Close(  )
{
	m_bRun =  false ;
	if ( WAIT_OBJECT_0 !=   WaitForSingleObject(m_quieHandle,10))
	{
		//没有等到退出的信号:
		Sleep(1);
	}
	//通知所有的观察者，调用close方法:
	MAP_OBSERVER::iterator ite = m_mp_observer.begin();
	while ( ite !=  m_mp_observer.end() )
	{
		ite->second->Close();
		ite++;
	}
	m_indexQueue_pack.UnInitQueue();
}

void CKernel::NotifyKernelAddUser( unsigned long lUserID,const char *pPassword )
{
	//判断参数:
	if ( 0 == lUserID || pPassword==NULL )
	{
		return ; 
	}
	//创建一个请求结构:
	STRU_ADD_USER_RQ *pRq  = new   STRU_ADD_USER_RQ;
	pRq->m_lUserID = lUserID;
	strcpy(pRq->m_szPassword,pPassword);
	pRq->m_lPassLen = strlen( pPassword ) + 1; 
	//请求一个索引，是为了同一个包类型包在map表中能够同时存在:
	int iIndex = 0  ; 
	if( false  == m_indexQueue_pack.Pop( & iIndex ) )
	{
		return ; 
	}
	pRq->m_iIndex = iIndex;
	//创建一个任务：
	STRU_TASK *pTask  =  new  STRU_TASK;
	pTask->pRq  = pRq; 
	//修改状态为init 
	pTask->m_eStatusType = enum_status_init;
	//设置时间为0 
	pTask->m_ulTime =  0 ; 
	//投入队列:
	if ( false == m_task_queue.Push( pTask) )
	{
		return  ; 
	}
	//放入map_status 状态机结构表
	//怕UI线程卡死，而放到处理线程去操作
}

void CKernel::NotiftyRecvData(  STRU_SESSION  *pSession ,char szbuf[],long lBuflen,unsigned short eNetType)
{
		//反序列化数据:

	    //
}
//void NotifyKernelAddGroup( const char * pGroupName ) ;
//void NotifyKernelAddProject ( const char * pProjectName ) ;
//void NotifyKernelUserJoinGroup( unsigned long lUserID, unsigned long lGroupID ) ;
//void NotifyKernelUserLeaveGroup( unsigned long lUserID, unsigned long  lGroupID)  ;
//void NotifyKernelUserJoinProject( unsigned long lUserID, unsigned long lProjectID )  ;
//void NotifyKernelUserLeaveProject( unsigned long lUserID, unsigned long lGroupID )  ;
//void NotifyKernelGroupJoinProject( unsigned long lGroupID,  unsigned long lProjectID)  ;
//void NotifyKernelGroupLeaveProject( unsigned long lGroupID, unsigned long lProjectID)  ;
//void NotifyKernelSetGroupPower (  unsigned long lGroupID,  unsigned short ePower  )  ;
//void NofityKernelSetUserPower( unsigned long lUserID, unsigned short ePower ) ; 
//void NofityKernelGetUserList(  )   ;
//void NofityKernelGetGroupList(  )   ;
//void NofityKernelGetProList( )  ;


void CKernel::DealProc(  )
{
	 //从队列中取出一个任务: 
	STRU_TASK *pTask ; 
	
	while( m_bRun )
	{
		if ( false == m_task_queue.Pop(&pTask))
		{
			//没有任务:
			Sleep(10 ); 
		}
		else
		{
			DealStatus(  pTask   );
		}
	}
	//设置退出信号:
	SetEvent(m_quieHandle);
}
void CKernel::DealStatus( STRU_TASK * pTask   )
{
	m_lock_status.Lock();
	unsigned short eStatus =  pTask  ->m_eStatusType;
	m_lock_status.UnLock();
	switch ( eStatus )
	{
	case enum_status_init:
	{
		//解包:
		char szSendBuf[DEF_MAX_RECV_BUF]  = {0} ; 
		long lSendLen =  pTask->pRq->Seriaze(szSendBuf,DEF_MAX_RECV_BUF);
		//重组map的key :
		long long i64Key =( ( long long ) pTask->pRq->m_iIndex)<<32+ pTask->pRq->m_nPackType;
		// 放入map结构:
		m_lock_mp_status.Lock();
		if ( m_mp_staus.find(i64Key ) != m_mp_staus.end() )
		{
			m_lock_mp_status.UnLock();
			//如果找到相同的处理请求，程序出错.
			break  ; 
		}
		m_mp_staus[ i64Key ]  = pTask ; 
		m_lock_mp_status.UnLock();
		//TODO :
		//调用网络类,发送
		m_pNet->SendData(szSendBuf,lSendLen);
		//将状态位修改为wait
		pTask->m_eStatusType = enum_status_wait;
		//修改时间:
		pTask->m_ulTime = GetTickCount();
		//重新投入到队列中去:
		m_task_queue.Push( pTask ) ;
	}
		break ; 
	case enum_status_wait:
	{
		//TODO :
		if ( GetTickCount() -  pTask  ->m_ulTime > DEF_STATUS_WAIT_TIME )
		{
			
			//将状态位修改为init 
			pTask->m_eStatusType = enum_status_init;
			//修改时间:
			pTask->m_ulTime = 0;
           //重新放入队列当中
			m_task_queue.Push( pTask ) ;
		}
	}
		break ; 
	case enum_status_response:
	{
		//TODO :
		//调用对应的RS处理函数通知UI
		
		if (NofityUI( pTask  )) 
		{
			PACK_BASE *pBase =  pTask->pRs;
			
			switch( pBase->m_nPackType )
			{
			case DEF_SM2S_GET_USER_LIST_RS:
				{
					 STRU_GET_USER_LIST_RS* pRs  = ( STRU_GET_USER_LIST_RS * )pBase ;
					 (( STRU_GET_USER_LIST_RQ * )pTask->pRq)->m_lastUserID =  pRs->m_stru_userList[ pRs->m_iUserCount-1 ].m_lUserID;
				}
				 break;
			case DEF_SM2S_GET_GROUP_LIST_RS:
				{
					STRU_GET_GROUP_LIST_RS * pRs  = ( STRU_GET_GROUP_LIST_RS * )pBase ;
					(( STRU_GET_GROUP_LIST_RQ * )pTask->pRq)->m_lastGroupID =  pRs->m_stru_groupList[ pRs->m_iGroupCount-1 ].m_lGroupID;
				}
				break;
			case DEF_SM2S_GET_PRO_LIST_RS:
				{			
					 STRU_GET_PRO_LIST_RS* pRs  = ( STRU_GET_PRO_LIST_RS * )pBase ;
					 (( STRU_GET_PRO_LIST_RQ * )pTask->pRq)->m_lastProID =  pRs->m_stru_proList[ pRs->m_iProCount-1 ].m_lProID;
				}
				break;
			default :
				pTask->m_eStatusType = enum_status_complete;
				break ; 
			}
			//修改为init进行重新发送:
			if ( pTask->m_eStatusType !=  enum_status_complete)
			{
				pTask->m_eStatusType = enum_status_init ;
			}
		}
		else
		{
			pTask->m_eStatusType =enum_status_complete ;
		}
		// 重新投入队列:
		m_task_queue.Push(pTask );
	}
		break ; 
	case enum_status_complete:
	{

		//从map结构中删除该结构:
		long long i64Key =( ( long long ) pTask->pRq->m_iIndex)<<32+ pTask->pRq->m_nPackType;
		m_lock_mp_status.Lock();
		MAP_STATUS::iterator ite = m_mp_staus.find(i64Key);
		if( ite !=  m_mp_staus.end() )
		{
			m_mp_staus.erase( ite ) ;
		}
		m_lock_mp_status.UnLock();
		//归还索引:
		m_indexQueue_pack.Push( pTask->pRq->m_iIndex);
		pTask ->Destory();
		delete pTask ;
		pTask  =  NULL;
	}
		break ; 
	default :
	{
		long long i64Key =( ( long long ) pTask->pRq->m_iIndex)<<32+ pTask->pRq->m_nPackType;
		m_lock_mp_status.Lock();
		MAP_STATUS::iterator ite = m_mp_staus.find(i64Key);
		if( ite !=  m_mp_staus.end() )
		{
			m_mp_staus.erase( ite ) ;
		}
		m_lock_mp_status.UnLock();
		//归还索引:
		m_indexQueue_pack.Push( pTask->pRq->m_iIndex);
		pTask ->Destory();
		delete pTask ;
		pTask  =  NULL;
	} //switch 
	}
}

