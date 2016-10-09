#pragma once 
#ifndef ____INCLUDE__KERNEL__H____
#define ____INCLUDE__KERNEL__H____

#include <string>
#include <list>
#include <map>
#include "Common.h"
#include "SM2S/SM2S.h"
#include "MyLock.h"
#include"LockQueue.h"
#include "User.h"
#include "Group.h"
#include "Project.h"
#include "UDPNet.h"
using namespace std ;
//状态机表:
typedef map<unsigned int ,IObServer *>MAP_OBSERVER;
struct STRU_TASK{
public :
	STRU_TASK(): pRq( NULL ) , pRs ( NULL ) ,m_ulTime ( 0 )
	{
	
	}
	PACK_BASE *pRq ; 
	PACK_BASE *pRs ;
	unsigned long m_ulTime; //每次等待的时间
	unsigned short  m_eStatusType; 
	void Destory()
	{
		if ( pRq  )
		{
			delete pRq;
			pRq  = NULL;
		}
		if( pRs  )
		{
			delete pRs ;
			pRs  =  NULL;
		}
	}

};

//UI回调Kernel的接口
class IKernelToUI{
public :
	virtual void NotifyKernelAddUser( unsigned long lUserID,const char *pPassword ) =  0;
	virtual void NotifyKernelAddGroup( const char * pGroupName ) = 0 ;
	virtual void NotifyKernelAddProject ( const char * pProjectName ) = 0  ;
	virtual void NotifyKernelUserJoinGroup( unsigned long lUserID, const char * pGroupName ) =  0 ;
	virtual void NotifyKernelUserLeaveGroup( unsigned long lUserID, const char * pGroupName ) =  0 ;
	virtual void NotifyKernelUserJoinProject( unsigned long lUserID, unsigned long lProjectID ) =  0 ;
	virtual void NotifyKernelUserLeaveProject( unsigned long lUserID, unsigned long lProjectID) =  0 ;
	virtual void NotifyKernelGroupJoinProject( unsigned long lGroupID,unsigned long lProjectID ) =  0 ;
	virtual void NotifyKernelGroupLeaveProject( unsigned long lGroupID, unsigned long lProjectID ) =  0 ;
	virtual void NotifyKernelSetGroupPower ( unsigned long lGroupID,  unsigned short ePower  ) =  0 ;
	virtual void NofityKernelSetUserPower( unsigned long lUserID, unsigned short ePower ) = 0 ; 
	virtual void NofityKernelGetUserList(  ) = 0  ;
	virtual void NofityKernelGetGroupList(  )  = 0 ;
	virtual void NofityKernelGetProList( )  = 0;
};

//Kernel通知给UI的接口
class IUIToKernel{
public :
	virtual void NotifyUIAddUser( unsigned long lUserID,const char *pPassword ,bool bResult) =  0;
	virtual void NotifyUIAddGroup( const char * pGroupName,bool bResult ) = 0 ;
	virtual void NotifyUIAddProject ( const char * pProjectName,bool bResult ) = 0  ;
	virtual void NotifyUIUserJoinGroup( unsigned long lUserID,  unsigned long lGroupID,bool bResult ) =  0 ;
	virtual void NotifyUIUserLeaveGroup( unsigned long lUserID, unsigned long lGroupID,bool bResult ) =  0 ;
	virtual void NotifyUIUserJoinProject( unsigned long lUserID, unsigned long lProjectID,bool bResult ) =  0 ;
	virtual void NotifyUIUserLeaveProject( unsigned long lUserID,unsigned long lProjectID,bool bResult) =  0 ;
	virtual void NotifyUIGroupJoinProject(  unsigned long lGroupID,unsigned long lProjectID,bool bResult ) =  0 ;
	virtual void NotifyUIGroupLeaveProject(  unsigned long lGroupID, unsigned long lProjectID ,bool bResult) =  0 ;
	virtual void NotifyUISetGroupPower ( unsigned long lGroupID,  unsigned short ePower ,bool bResult ) =  0 ;
	virtual void NofityUISetUserPower( unsigned long lUserID, unsigned short ePower,bool bResult ) = 0 ; 
	//删除接口
	virtual void NotifyUIDelUser( unsigned long lUserID ,  bool bResult )  =  0 ;
	virtual void NofityUIDelGroup( unsigned long lGroupID  ,bool bResult  )  = 0 ;
	virtual void NofityUIDelProject ( unsigned long lProjectID ,bool bResult  ) =  0 ; 
 	
	virtual void NofityUISetUserList( list < CUser *   > & ls_userID  ) = 0  ;
	virtual void NofityUISetGroupList( list<CGroup *  > & ls_groupList )  = 0 ;
	virtual void NofityUISetProList( list < CProject * > & ls_proList  )  = 0;
};

class IKernel{
public :
	virtual bool OpenKernel( IUIToKernel *m_pUI  )  = 0  ;
	virtual bool CloseKernel(  )  =  0 ;

};
class CKernel : public IObServerMember , public IKernelToUI , public IObServer,public IKernel{
public :
	//IKernel的接口:
	bool OpenKernel( IUIToKernel *m_pUI  ) ; 
	bool CloseKernel(  );
	//给ＵI的回调
	 void NotifyKernelAddUser( unsigned long lUserID,const char *pPassword ) =  0;
	 void NotifyKernelAddGroup( const char * pGroupName ) = 0 ;
	 void NotifyKernelAddProject ( const char * pProjectName ) = 0  ;
	 void NotifyKernelUserJoinGroup( unsigned long lUserID, unsigned long lGroupID ) ;
	 void NotifyKernelUserLeaveGroup( unsigned long lUserID, unsigned long  lGroupID)  ;
	 void NotifyKernelUserJoinProject( unsigned long lUserID, unsigned long lProjectID )  ;
	 void NotifyKernelUserLeaveProject( unsigned long lUserID, unsigned long lGroupID )  ;
	 void NotifyKernelGroupJoinProject( unsigned long lGroupID,  unsigned long lProjectID)  ;
	 void NotifyKernelGroupLeaveProject( unsigned long lGroupID, unsigned long lProjectID)  ;
	 void NotifyKernelSetGroupPower (  unsigned long lGroupID,  unsigned short ePower  )  ;
	 void NofityKernelSetUserPower( unsigned long lUserID, unsigned short ePower ) ; 
	 void NofityKernelGetUserList(  )   ;
	 void NofityKernelGetGroupList(  )   ;
	 void NofityKernelGetProList( )  ;
	//观察者的操作方法:
	  bool AddObserver ( int key , IObServer *  pObserver )   ;
	 bool RemObserver ( IObServer *  pObserver )   ;
	 //观察者统一的接口函数
	 bool Close()  ; 
	 MAP_OBSERVER m_mp_observer ;
private: 
	static unsigned int __stdcall DealThreadProc ( void * pParam  ) ; 
	void DealProc(  );  //线程中调用的函数
	
private :
	bool m_bRun ;
	HANDLE m_quieHandle ; //让线程强制退出的信号:
	IUIToKernel * m_pUI ; 
	INet * m_pNet ; 
private:
	//状态机定义:
	CLockQueue< STRU_TASK * > m_task_queue ;
	//MyLock m_lock_status ;// 访问状态机表的锁
   	void DealStatus( STRU_TASK * );//处理状态机的函数
	bool NofityUI( STRU_TASK *pTask  ); //返回true需要重新投入到队列中去




};
#endif //____INCLUDE__KERNEL__H____