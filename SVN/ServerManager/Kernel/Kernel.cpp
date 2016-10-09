#include "Kernel.h"
//��ʼ��
bool  CKernel::OpenKernel( IUIToKernel *pUI  )
{
	if ( false == CloseKernel() )
	{
		return false; 
	}
	m_pUI = pUI;
	//��ʼ������(����Ӧ�����ⲿ����۲��߼���):
	m_pNet= new CUDPNet;
	//����۲��߼���:
	AddObserver(enum_udp_type,m_pNet);
	//���������߳�:
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
//����ʼ��
bool  CKernel::CloseKernel(  )
{
	Close();
}
bool  CKernel::Close(  )
{
	m_bRun =  false ;
	if ( WAIT_OBJECT_0 !=   WaitForSingleObject(m_quieHandle,10))
	{
		//û�еȵ��˳����ź�:
		Sleep(1);
	}
	//֪ͨ���еĹ۲��ߣ�����close����:
	MAP_OBSERVER::iterator ite = m_mp_observer.begin();
	while ( ite !=  m_mp_observer.end() )
	{
		ite->second->Close();
		ite++;
	}
}
void CKernel::DealProc(  )
{
	 //�Ӷ�����ȡ��һ������: 
	STRU_TASK *pTask ; 
	
	while( m_bRun )
	{
		if ( false == m_task_queue.Pop(&pTask))
		{
			//û������:
			Sleep(10 ); 
		}
		else
		{
			DealStatus(  pTask   );
		}
	}
	//�����˳��ź�:
	SetEvent(m_quieHandle);
}
void CKernel::DealStatus( STRU_TASK * pTask   )
{
	switch ( pTask  ->m_eStatusType )
	{
	case enum_status_init:
	{
		//���:
		char szSendBuf[DEF_MAX_RECV_BUF]  = {0} ; 
		long lSendLen =  pTask->pRq->Seriaze(szSendBuf,DEF_MAX_RECV_BUF);

		//TODO :
		//����������,����
		m_pNet->SendData(szSendBuf,lSendLen);
		//��״̬λ�޸�Ϊwait
		pTask->m_eStatusType = enum_status_wait;
		//�޸�ʱ��:
		pTask->m_ulTime = GetTickCount();
		//����Ͷ�뵽������ȥ:
		m_task_queue.Push( pTask ) ;
	}
		break ; 
	case enum_status_wait:
	{
		//TODO :
		if ( GetTickCount() -  pTask  ->m_ulTime > DEF_STATUS_WAIT_TIME )
		{
			
			//��״̬λ�޸�Ϊinit 
			pTask->m_eStatusType = enum_status_wait;
			//�޸�ʱ��:
			pTask->m_ulTime = GetTickCount();
           //���·�����е���
			m_task_queue.Push( pTask ) ;
		}
	}
		break ; 
	case enum_status_response:
	{
		//TODO :
		//���ö�Ӧ��RS������֪ͨUI
		
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
			//�޸�Ϊinit�������·���:
			if ( pTask->m_eStatusType !=  enum_status_complete)
			{
				pTask->m_eStatusType = enum_status_init ;
			}
		}
		else
		{
			pTask->m_eStatusType =enum_status_complete ;
		}
		// ����Ͷ�����:
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

