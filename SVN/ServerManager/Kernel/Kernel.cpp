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
}
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
	switch ( pTask  ->m_eStatusType )
	{
	case enum_status_init:
	{
		//解包:
		char szSendBuf[DEF_MAX_RECV_BUF]  = {0} ; 
		long lSendLen =  pTask->pRq->Seriaze(szSendBuf,DEF_MAX_RECV_BUF);

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
			pTask->m_eStatusType = enum_status_wait;
			//修改时间:
			pTask->m_ulTime = GetTickCount();
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
		pTask ->Destory();
		delete pTask ;
		pTask  =  NULL;
	}
		break ; 
	default :
	{
		pTask ->Destory();
		delete pTask ;
		pTask  =  NULL;
	}
	}
}

