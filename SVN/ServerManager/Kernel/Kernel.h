#pragma once 
#ifndef ____INCLUDE__KERNEL__H____
#define ____INCLUDE__KERNEL__H____

#include <string>
#include <list>
#include <map>
#include "Common.h"
#include "SM2S/SM2S.h"
#include "MyLock.h"
using namespace std ;
//״̬����:

struct STRU_TASK{
public :
	STRU_TASK(): pRq( NULL ) , pRs ( NULL ) ,m_ulTime ( 0 )
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
//״̬����: 
typedef map<int ,  STRU_TASK *>MAP_STATUS;
//UI�ص�Kernel�Ľӿ�
class IKernelToUI{
public :
	virtual void NotifyKernelAddUser( unsigned long lUserID,const char *pPassword ) =  0;
	virtual void NotifyKernelAddGroup( const char * pGroupName ) = 0 ;
	virtual void NotifyKernelAddProject ( const char * pProjectName ) = 0  ;
	virtual void NotifyKernelUserJoinGroup( unsigned long lUserID, const char * pGroupName ) =  0 ;
	virtual void NotifyKernelUserLeaveGroup( unsigned long lUserID, const char * pGroupName ) =  0 ;
	virtual void NotifyKernelUserJoinProject( unsigned long lUserID, const char * pProName ) =  0 ;
	virtual void NotifyKernelUserLeaveProject( unsigned long lUserID, const char * pProName ) =  0 ;
	virtual void NotifyKernelGroupJoinProject( const char *pGroupName, const char * pProName ) =  0 ;
	virtual void NotifyKernelGroupLeaveProject( const char *pGroupName, const char * pProName ) =  0 ;
	virtual void NotifyKernelSetGroupPower (  const char * pGroupName,  unsigned short ePower  ) =  0 ;
	virtual void NofityKernelSetUserPower( unsigned long lUserID, unsigned short ePower ) = 0 ; 
	virtual void NofityKernelGetUserList(  ) = 0  ;
	virtual void NofityKernelGetGroupList(  )  = 0 ;
	virtual void NofityKernelGetProList( )  = 0;
};

//Kernel֪ͨ��UI�Ľӿ�
class IUIToKernel{
public :
	virtual void NotifyUIAddUser( unsigned long lUserID,const char *pPassword ) =  0;
	virtual void NotifyUIAddGroup( const char * pGroupName ) = 0 ;
	virtual void NotifyUIAddProject ( const char * pProjectName ) = 0  ;
	virtual void NotifyUIUserJoinGroup( unsigned long lUserID, const char * pGroupName ) =  0 ;
	virtual void NotifyUIUserLeaveGroup( unsigned long lUserID, const char * pGroupName ) =  0 ;
	virtual void NotifyUIUserJoinProject( unsigned long lUserID, const char * pProName ) =  0 ;
	virtual void NotifyUIUserLeaveProject( unsigned long lUserID, const char * pProName ) =  0 ;
	virtual void NotifyUIGroupJoinProject( const char *pGroupName, const char * pProName ) =  0 ;
	virtual void NotifyUIGroupLeaveProject( const char *pGroupName, const char * pProName ) =  0 ;
	virtual void NotifyUISetGroupPower (  const char * pGroupName,  unsigned short ePower  ) =  0 ;
	virtual void NofityUISetUserPower( unsigned long lUserID, unsigned short ePower ) = 0 ; 
	virtual void NofityUISetUserList( list < unsigned long  > & ls_userID  ) = 0  ;
	virtual void NofityUISetGroupList( list< string > & ls_groupList )  = 0 ;
	virtual void NofityUISetProList( list < string > & ls_proList  )  = 0;
};


class CKernel : public IObServerMember , public IKernelToUI , public IObServer{
public :
	//����I�Ļص�
	 void NotifyKernelAddUser( unsigned long lUserID,const char *pPassword ) =  0;
	 void NotifyKernelAddGroup( const char * pGroupName ) = 0 ;
	 void NotifyKernelAddProject ( const char * pProjectName ) = 0  ;
	 void NotifyKernelUserJoinGroup( unsigned long lUserID, const char * pGroupName ) ;
	 void NotifyKernelUserLeaveGroup( unsigned long lUserID, const char * pGroupName )  ;
	 void NotifyKernelUserJoinProject( unsigned long lUserID, const char * pProName )  ;
	 void NotifyKernelUserLeaveProject( unsigned long lUserID, const char * pProName )  ;
	 void NotifyKernelGroupJoinProject( const char *pGroupName, const char * pProName )  ;
	 void NotifyKernelGroupLeaveProject( const char *pGroupName, const char * pProName )  ;
	 void NotifyKernelSetGroupPower (  const char * pGroupName,  unsigned short ePower  )  ;
	 void NofityKernelSetUserPower( unsigned long lUserID, unsigned short ePower ) ; 
	 void NofityKernelGetUserList(  )   ;
	 void NofityKernelGetGroupList(  )   ;
	 void NofityKernelGetProList( )  ;
	//�۲��ߵĲ�������:
	  bool AddObserver ( IObServer *  pObserver )   ;
	 bool RemObserver ( IObServer *  pObserver )   ;
	 //�۲���ͳһ�Ľӿں���
	 bool Close()  ; 
private: 
	static unsigned int __stdcall DealThreadProc ( void * pParam  ) ; 
	void DealProc(  );  //�߳��е��õĺ���
	
private :
	bool m_bRun ;
	HANDLE m_quieHandle ; //���߳�ǿ���˳����ź�:
private:
	//״̬������:
	MAP_STATUS m_mp_status ;//״̬����
	MyLock m_lock_status ;// ����״̬�������
	void DealStatus( STRU_TASK ** );//����״̬���ĺ���





};
#endif //____INCLUDE__KERNEL__H____