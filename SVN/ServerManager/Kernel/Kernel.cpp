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

	//��ʼ��������:
	m_indexQueue_pack.InitQueue(DEF_MAX_PACK_COUNT);
	for( int i=0;i<DEF_MAX_PACK_COUNT ;i++  )
	{
		m_indexQueue_pack.Push( i ) ; 
	}
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
	m_indexQueue_pack.UnInitQueue();
}

void CKernel::NotifyKernelAddUser( unsigned long lUserID,const char *pPassword )
{
	//�жϲ���:
	if ( 0 == lUserID || pPassword==NULL )
	{
		return ; 
	}
	//����һ������ṹ:
	STRU_ADD_USER_RQ *pRq  = new   STRU_ADD_USER_RQ;
	pRq->m_lUserID = lUserID;
	strcpy(pRq->m_szPassword,pPassword);
	pRq->m_lPassLen = strlen( pPassword ) + 1; 
	//����һ����������Ϊ��ͬһ�������Ͱ���map�����ܹ�ͬʱ����:
	int iIndex = 0  ; 
	if( false  == m_indexQueue_pack.Pop( & iIndex ) )
	{
		return ; 
	}
	pRq->m_iIndex = iIndex;
	//����һ������
	STRU_TASK *pTask  =  new  STRU_TASK;
	pTask->pRq  = pRq; 
	//�޸�״̬Ϊinit 
	pTask->m_eStatusType = enum_status_init;
	//����ʱ��Ϊ0 
	pTask->m_ulTime =  0 ; 
	//Ͷ�����:
	if ( false == m_task_queue.Push( pTask) )
	{
		return  ; 
	}
	//����map_status ״̬���ṹ��
	//��UI�߳̿��������ŵ������߳�ȥ����
}

void CKernel::NotiftyRecvData(  STRU_SESSION  *pSession ,char szbuf[],long lBuflen,unsigned short eNetType)
{
		//�����л�����:

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
	m_lock_status.Lock();
	unsigned short eStatus =  pTask  ->m_eStatusType;
	m_lock_status.UnLock();
	switch ( eStatus )
	{
	case enum_status_init:
	{
		//���:
		char szSendBuf[DEF_MAX_RECV_BUF]  = {0} ; 
		long lSendLen =  pTask->pRq->Seriaze(szSendBuf,DEF_MAX_RECV_BUF);
		//����map��key :
		long long i64Key =( ( long long ) pTask->pRq->m_iIndex)<<32+ pTask->pRq->m_nPackType;
		// ����map�ṹ:
		m_lock_mp_status.Lock();
		if ( m_mp_staus.find(i64Key ) != m_mp_staus.end() )
		{
			m_lock_mp_status.UnLock();
			//����ҵ���ͬ�Ĵ������󣬳������.
			break  ; 
		}
		m_mp_staus[ i64Key ]  = pTask ; 
		m_lock_mp_status.UnLock();
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
			pTask->m_eStatusType = enum_status_init;
			//�޸�ʱ��:
			pTask->m_ulTime = 0;
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

		//��map�ṹ��ɾ���ýṹ:
		long long i64Key =( ( long long ) pTask->pRq->m_iIndex)<<32+ pTask->pRq->m_nPackType;
		m_lock_mp_status.Lock();
		MAP_STATUS::iterator ite = m_mp_staus.find(i64Key);
		if( ite !=  m_mp_staus.end() )
		{
			m_mp_staus.erase( ite ) ;
		}
		m_lock_mp_status.UnLock();
		//�黹����:
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
		//�黹����:
		m_indexQueue_pack.Push( pTask->pRq->m_iIndex);
		pTask ->Destory();
		delete pTask ;
		pTask  =  NULL;
	} //switch 
	}
}

