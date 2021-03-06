#include "Kernel.h"
//初始化
//#define REGISTER_UI_FUN(TYPE,FUN_NAME,FUN_TYPE) FUN_NODE *pFunNode = new FUN_NODE ;\
//	pFunNode->eFunType = FUN_TYPE;\
//	pFunNode->pf_base =(_pf_base) &IUIToKernel::NotifyUI##FUN_NAME;\
//	m_mp_fun_type[DEF_SM2S_##TYPE##_RS]  = pFunNode;
//void CKernel::RegisterNofityUIFun()
//{
//	//IUIToKernel::NofityUIJoinProject
//	REGISTER_UI_FUN(ADD_USER,AddUser,pf_ul_sz_b)
//	REGISTER_UI_FUN(ADD_GROUP,AddGroup,pf_sz_b)
//	REGISTER_UI_FUN(ADD_PRO,AddProject,pf_sz_b)
//	REGISTER_UI_FUN(JOIN_GROUP,JoinGroup,pf_ul_ul_b)
//	REGISTER_UI_FUN(LEAVE_GROUP,LeaveGroup,pf_ul_ul_b)
//	REGISTER_UI_FUN(JOIN_PRO,JoinProject,pf_ul_ul_b)
//	REGISTER_UI_FUN(LEAVE_PRO,LeaveProject,pf_ul_ul_b)
//	REGISTER_UI_FUN(SET_POWER,SetPower,pf_ul_ul_b)
//	REGISTER_UI_FUN(DELETE_USER,DelUser,pf_ul_b)
//	REGISTER_UI_FUN(DELETE_GROUP,DelGroup,pf_ul_b)
//	REGISTER_UI_FUN(DELETE_PRO,DelProject,pf_ul_b)
//	REGISTER_UI_FUN(GET_USER_LIST,SetUserList,pf_list_user)
//	REGISTER_UI_FUN(GET_GROUP_LIST,SetGroupList,pf_list_group)
//	REGISTER_UI_FUN(GET_PRO_LIST,SetProList,pf_list_pro)
//}
bool  CKernel::OpenKernel( IUIToKernel *pUI  )
{
	if ( false == CloseKernel() )
	{
		return false; 
	}
	m_pUI = pUI;
	////注册对应的NotifyUI函数：
	//RegisterNofityUIFun();

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
		//取出index ,和类型:
	int iType =  * ( int * )szbuf ;
	szbuf+= sizeof ( int  );
	int iIndex  = *( int *)szbuf ; 
	//合成key
	long long i64Key =( ( long long  )iIndex)<<32+iIndex;
	//让工厂制造相应类型的包:
	PACK_BASE * pRs   = CFactoryPack::CreateObject(iType);
	//反序列化数据:
	if( false == pRs->UnSeriaze(szbuf,lBuflen) ) 
	{
		return  ;
	}

	 //将列表需改成response  
	MAP_STATUS::iterator ite ; 
	m_lock_mp_status.Lock();
	if((  ite = m_mp_staus.find( i64Key ) ) != m_mp_staus.end() ) 
	{
		ite->second->m_eStatusType = enum_status_response ;
		ite->second->pRs = pRs ;  
	}
	m_lock_mp_status.UnLock();

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
		//序列化包:
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

bool CKernel::NofityUI( STRU_TASK *pTask  )
{
	//根据任务中的Rs请求，调用对应的Notify函数:
	int iType = pTask->pRs->m_nPackType;
	switch (iType)
	{
			case DEF_SM2S_ADD_USER_RS:
		{
			STRU_ADD_USER_RS * pRs =  ( STRU_ADD_USER_RS * )pTask->pRs;
			if( m_pUI )
			{
				m_pUI->NotifyUIAddUser(pRs->m_lUserID,(( STRU_ADD_USER_RQ *)pTask->pRq)->m_szPassword,pRs->iResult==enum_success?true:false);
			}
		
		};
		break ;
			case DEF_SM2S_ADD_GROUP_RS:
		{
			STRU_ADD_GROUP_RS *pRs  = ( STRU_ADD_GROUP_RS * )pTask->pRs;
			
			m_pUI->NotifyUIAddGroup((( STRU_ADD_GROUP_RQ *)pTask->pRq)->m_szGroup, pRs->m_iResult==enum_success?true:false);
		};
		break ;
		case DEF_SM2S_ADD_PRO_RS:
		{
			STRU_ADD_PRO_RS *pRs  = ( STRU_ADD_PRO_RS * )pTask->pRs;
			m_pUI->NotifyUIAddProject((( STRU_ADD_PRO_RS *)pTask->pRq)->m_szProName, pRs->m_iResult==enum_success?true:false);
		};
		break ;
			case DEF_SM2S_JOIN_GROUP_RS:
		{
				STRU_JOIN_GROUP_RS *pRs  = ( STRU_JOIN_GROUP_RS * )pTask->pRs;
				m_pUI->NotifyUIJoinGroup(pRs->m_lUserID,pRs->m_lGroupID,pRs->m_iResult==enum_success?true:false);
		
		};
		break ;
			case DEF_SM2S_LEAVE_GROUP_RS:
		{
			STRU_LEAVE_GROUP_RS *pRs  = ( STRU_LEAVE_GROUP_RS * )pTask->pRs;
				m_pUI->NotifyUILeaveGroup(pRs->m_lUserID,pRs->m_lGroupID,pRs->m_iResult==enum_success?true:false);
		
		};
		break ;
			case DEF_SM2S_JOIN_PRO_RS:
		{
			STRU_JOIN_PRO_RS *pRs  = ( STRU_JOIN_PRO_RS * )pTask->pRs;
			long id  =  0 ; 
			if ( 0 == pRs->m_lUserID ) 
			{
				id = pRs->m_lGroupID;
			}
			m_pUI->NotifyUIJoinProject(id,pRs->m_lProID,pRs->m_ePower,pRs->m_iResult==enum_success?true:false);
		
		};
		break ;
			case DEF_SM2S_LEAVE_PRO_RS:
		{
			STRU_LEAVE_PRO_RS *pRs  = ( STRU_LEAVE_PRO_RS * )pTask->pRs;
			long id  =  0 ; 
			if ( 0 == pRs->m_lUserID ) 
			{
				id = pRs->m_lGroupID;
			}
			m_pUI->NotifyUILeaveProject(id,pRs->m_lProID,pRs->m_iResult==enum_success?true:false);
		
		};
		break ;
		case DEF_SM2S_SET_POWER_RS:
		{
			STRU_SET_POWER_RS *pRs  = ( STRU_SET_POWER_RS * )pTask->pRs;
			long id  =  0 ; 
			if ( 0 == pRs->m_lUserID ) 
			{
				id = pRs->m_lGroupID;
			}
			if( m_pUI )
			{
				m_pUI->NotifyUISetPower(id,pRs->m_lProID,pRs->m_ePower,pRs->m_iResult==enum_success?true:false);
			}
		};
		break ;
		case DEF_SM2S_GET_USER_LIST_RS:
		{
			STRU_GET_USER_LIST_RS *pRs  = ( STRU_GET_USER_LIST_RS * )pTask->pRs;
			list<CUser *>ls_user ;
			for( int i=0 ;pRs->m_iUserCount;i++)
			{
				CUser *pUser = new CUser;
				pUser->SetUserID(pRs->m_stru_userList[i].m_lUserID);
				pUser->SetPassword(pRs->m_stru_userList[i].m_szPassWord);
				ls_user.push_back(pUser);
			}
			if ( m_pUI )
			{
				m_pUI->NotifyUISetUserList(ls_user);
			}
			//如果返回了最大的用户个数，需要进行下一次的拉取，返回true 
			if ( pRs->m_iUserCount == DEF_MAX_GET_USER_COUNT )
			{
				return true ;
			}
		};
		break ;
		case DEF_SM2S_GET_GROUP_LIST_RS:
		{
			STRU_GET_GROUP_LIST_RS *pRs  = ( STRU_GET_GROUP_LIST_RS * )pTask->pRs;
			list<CGroup *>ls_group;
			for( int i=0 ;pRs->m_iGroupCount;i++)
			{
				CGroup *pGroup = new CGroup;
				pGroup->SetGroupName(pRs->m_stru_groupList[i].m_szGroup);
				pGroup->JoinGroup(pRs->m_stru_groupList[i].m_set_user);
				pGroup->SetGroupID = pRs->m_stru_groupList[i].m_lGroupID;
				/*pGroup->JoinGroup(pRs->m_stru_groupList[i].m_ls_user;
				pUser->SetUserID(pRs->m_stru_userList[i].m_lUserID);
				pUser->SetPassword(pRs->m_stru_userList[i].m_szPassWord);*/
				ls_group.push_back(pGroup);
			}
			if ( m_pUI )
			{
				m_pUI->NotifyUISetGroupList(ls_group);
			}
			//如果返回了最大的用户个数，需要进行下一次的拉取，返回true 
			if ( pRs->m_iGroupCount == DEF_MAX_GET_GROUP_COUNT )
			{
				return true ;
			}
		
		};
		break ;
		case DEF_SM2S_GET_PRO_LIST_RS:
		{
			STRU_GET_PRO_LIST_RS *pRs  = ( STRU_GET_PRO_LIST_RS * )pTask->pRs;
			list<CProject*>ls_project ;
			for( int i=0 ;pRs->m_iProCount;i++)
			{
				CProject  *pPro= new CProject;
				pPro->SetProID(pRs->m_stru_proList[i].m_lProID);
				pPro->AddUser( pRs->m_stru_proList[i].m_set_user);
				pPro->SetProName(pRs->m_stru_proList[i].m_szProName);
				pPro->m_lCreateTime = pRs->m_stru_proList[i].m_lCreateTime;
				ls_project.push_back(pPro);
			}
			if ( m_pUI )
			{
				m_pUI->NotifyUISetProList(ls_project);
			}
			//如果返回了最大的用户个数，需要进行下一次的拉取，返回true 
			if ( pRs->m_iProCount == DEF_MAX_GET_PROJECT_COUNT )
			{
				return true ;
			}
		
		};
		break ;
	}
		
}