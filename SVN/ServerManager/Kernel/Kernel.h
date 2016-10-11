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
//״̬����:
typedef map<unsigned int ,IObServer *>MAP_OBSERVER;
typedef map<long  long,STRU_TASK *>MAP_STATUS ;
//long long ����� + pTask �ṹ,Ϊ����һ�����͵İ���map�ظ����
struct STRU_TASK{
public :
	STRU_TASK *pNext ; 
public :
	STRU_TASK(): pRq( NULL ) , pRs ( NULL ) ,m_ulTime ( 0 ) ,pNext( NULL )
	{
	
	}
	PACK_BASE *pRq ; 
	PACK_BASE *pRs ;
	unsigned long m_ulTime; //ÿ�εȴ���ʱ��
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

//UI�ص�Kernel�Ľӿ�
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

//Kernel֪ͨ��UI�Ľӿ�
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
	//ɾ���ӿ�
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
class CKernel : public IObServerMember , public IKernelToUI , public IObServer,public IKernel,INotify{
public :
	//IKernel�Ľӿ�:
	bool OpenKernel( IUIToKernel *m_pUI  ) ; 
	bool CloseKernel(  );
private : 
	//����I�Ļص�
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
	//�۲��ߵĲ�������:
	  bool AddObserver ( int key , IObServer *  pObserver )   ;
	 bool RemObserver ( IObServer *  pObserver )   ;
	 //�۲���ͳһ�Ľӿں���
	 bool Close()  ; 
	 MAP_OBSERVER m_mp_observer ;
private: 
	static unsigned int __stdcall DealThreadProc ( void * pParam  ) ; 
	void DealProc(  );  //�߳��е��õĺ���
private :
	//INotify�Ľ��:
	void NotiftyRecvData(  STRU_SESSION  *pSession ,char szbuf[],long lBuflen,unsigned short eNetType);
private :
	bool m_bRun ;
	HANDLE m_quieHandle ; //���߳�ǿ���˳����ź�:
	IUIToKernel * m_pUI ; 
	INet * m_pNet ; 
private:
	//״̬������:
	//��עһ�����߶���������ȡ����ṹ�����ǿ��Դ��ڲ�������Ϊ�����߳�ֻ�ᵱ״̬��Ϊinit��״̬��Ϊwait
	//���߰�״̬��Ϊresponse״̬�ĳ�complete .���������߳�ֻ�ᵱwait��״̬��ʱ�򣬸�Ϊresponse ״̬��
	//����Ҫ���޸ĵ�״̬����ʱ�����,��Ϊ���ܴ������������ȥȡ״̬����ʱ�򣬲���response���߳������޸�
	//״̬����ָ����ptask->status��Ӧ���ڴ棩����ɵ�ͬ������
	MAP_STATUS m_mp_staus;  
	CLockQueue< STRU_TASK * > m_task_queue ;
	CLockQueue< int  > m_indexQueue_pack; //��������
	MyLock m_lock_mp_status ;// ����״̬�������
	MyLock m_lock_status ; //����״̬����
   	void DealStatus( STRU_TASK * );//����״̬���ĺ���
	bool NofityUI( STRU_TASK *pTask  ); //����true��Ҫ����Ͷ�뵽������ȥ
	



};
#endif //____INCLUDE__KERNEL__H____