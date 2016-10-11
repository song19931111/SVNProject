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
#include "SM2S/FactoryPack.h"
using namespace std ;

//



//状态机表:

struct STRU_TASK{
public :
	STRU_TASK *pNext ; 
public :
	STRU_TASK(): pRq( NULL ) , pRs ( NULL ) ,m_ulTime ( 0 ) ,pNext( NULL )
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
typedef map<unsigned int ,IObServer *>MAP_OBSERVER;
typedef map<long  long,STRU_TASK *>MAP_STATUS ;
//long long 是序号 + pTask 结构,为了让一个类型的包在map重复多个
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
//	virtual void NotifyKernelSetGroupPower ( unsigned long lGroupID,  unsigned short ePower  ) =  0 ;
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
	virtual void NotifyUIJoinGroup( unsigned long lUserID,  unsigned long lGroupID,bool bResult ) =  0 ;
	virtual void NotifyUILeaveGroup( unsigned long lUserID, unsigned long lGroupID,bool bResult ) =  0 ;
	virtual void NotifyUISetPower( unsigned long lUserID, unsigned long lProjectID,unsigned short ePower,bool bResult ) = 0 ; 
	
	virtual void NotifyUIJoinProject( unsigned long lUserID, unsigned long lProjectID,unsigned short ePower,bool bResult   ) =  0 ;
	virtual void NotifyUILeaveProject( unsigned long lUserID, unsigned long lProjectID,unsigned short ePower,bool bResult   ) =  0 ;
	//删除接口
	virtual void NotifyUIDelUser( unsigned long lUserID ,  bool bResult )  =  0 ;
	virtual void NotifyUIDelGroup( unsigned long lGroupID  ,bool bResult  )  = 0 ;
	virtual void NotifyUIDelProject ( unsigned long lProjectID ,bool bResult  ) =  0 ; 
 	//GetList 
	virtual void NotifyUISetUserList( list < CUser *   > & ls_userID  ) = 0  ;
	virtual void NotifyUISetGroupList( list<CGroup *  > & ls_groupList )  = 0 ;
	virtual void NotifyUISetProList( list < CProject * > & ls_proList  )  = 0;
};
/////////////////////////////应对函数是不同的参数////////////////////////////////////////////////////////////////
//注册对应的参数指针:
typedef void (IUIToKernel::*_pf_base)( );
typedef void (IUIToKernel::*_pf_ul_sz_b)( unsigned long ,char *,bool  );
typedef void (IUIToKernel::*_pf_sz_b)( const char * ,bool   );
typedef void (IUIToKernel::*_pf_ul_ul_b)( unsigned long ,  unsigned long ,bool );
typedef void (IUIToKernel::*_pf_ul_ul_ush_b)( unsigned long ,  unsigned long ,unsigned short ,bool );
typedef void (IUIToKernel::*_pf_ul_b)( unsigned long ,bool );
typedef void (IUIToKernel::*_pf_list_user)( list<CUser *> );
typedef void (IUIToKernel::*_pf_list_group)( list<CGroup *> );
typedef void (IUIToKernel::*_pf_list_pro)( list<CProject *> );
union u_fun_type{
	_pf_base pf_base ; 
	_pf_ul_b pf_ul_b;
	_pf_ul_sz_b   pf_ul_sz_b;
	_pf_sz_b pf_sz_b;
	_pf_ul_ul_b pf_ul_ul_b;
	_pf_ul_ul_ush_b pf_ul_ush_b;
	_pf_list_user pf_list_user;
	_pf_list_group pf_list_group;
	_pf_list_pro pf_list_pro;
};
enum enum_fun_type{pf_base,pf_ul_b,pf_ul_sz_b,pf_sz_b,pf_ul_ul_b,pf_ul_ush_b,pf_list_user,pf_list_group,pf_list_pro};
struct FUN_NODE{
	enum_fun_type eFunType ; //函数类型
	_pf_base pf_base ; 
} ;
map< int , FUN_NODE *> m_mp_fun_type; 
/////////////////////////////////////////////////////////////////////////////////////////////

class IKernel{
public :
	virtual bool OpenKernel( IUIToKernel *m_pUI  )  = 0  ;
	virtual bool CloseKernel(  )  =  0 ;

};
class CKernel : public IObServerMember , public IKernelToUI , public IObServer,public IKernel,INotify{
public :
	//IKernel的接口:
	bool OpenKernel( IUIToKernel *m_pUI  ) ; 
	bool CloseKernel(  );
private : 
	//给ＵI的回调
	 void NotifyKernelAddUser( unsigned long lUserID,const char *pPassword );
	 void NotifyKernelAddGroup( const char * pGroupName ) ;
	 void NotifyKernelAddProject ( const char * pProjectName ) ;
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
	//INotify的借口:
	void NotiftyRecvData(  STRU_SESSION  *pSession ,char szbuf[],long lBuflen,unsigned short eNetType);
private :
	bool m_bRun ;
	HANDLE m_quieHandle ; //让线程强制退出的信号:
	IUIToKernel * m_pUI ; 
	INet * m_pNet ; 
private:
	//状态机定义:
	//（注一下两者都是用来存取任务结构，但是可以存在并发，因为处理线程只会当状态机为init的状态改为wait
	//或者把状态机为response状态改成complete .而收数据线程只会当wait的状态的时候，改为response 状态）
	//但是要对修改的状态机的时候加锁,因为可能存在着任务队列去取状态机的时候，操作response的线程正在修改
	//状态机（指的是ptask->status对应的内存）而造成的同步问题
	MAP_STATUS m_mp_staus;  
	CLockQueue< STRU_TASK * > m_task_queue ;
	CLockQueue< int  > m_indexQueue_pack; //包的索引
	MyLock m_lock_mp_status ;// 访问状态机表的锁
	MyLock m_lock_status ; //访问状态的锁
   	void DealStatus( STRU_TASK * );//处理状态机的函数
	void RegisterNofityUIFun(); //将NofityUIXX注册进函数表中去
	bool NofityUI( STRU_TASK *pTask  ); //返回true需要重新投入到队列中去
	



};
#endif //____INCLUDE__KERNEL__H____