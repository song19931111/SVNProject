#pragma once 
#ifndef ____INCLUDE__DATASET__H____
#include "User.h"
#include "Group.h"
#include "Project.h"
#include "LockQueue.h"
#include<map>
#include <string>
typedef  map<unsigned long,CUser *> MAP_ID_USER ;
typedef  map<unsigned long,CGroup *> MAP_ID_GROUP ;
typedef  map<unsigned long,CProject *> MAP_ID_PRO ;
class CDataSet{
public :
	MAP_ID_USER m_mp_user ; //用户的集合
	MAP_ID_GROUP m_mp_group ;//组的集合
	MAP_ID_PRO m_mp_pro; //项目的集合
public:
	 //用户,组,项目的内存池+索引池:
	CUser *m_pUser ;
	CGroup *m_pGroup;
	CProject *m_pPro;
	CLockQueue<int>m_index_user;
	CLockQueue<int>m_index_group;
	CLockQueue<int>m_index_pro;

public :
	bool AddUser( unsigned long m_lUserID,char *pPassword,unsigned short ePower);
	bool AddGroup( unsigned long m_lGroupID,char *pGroupName, unsigned short ePower);
	bool AddPro( unsigned long m_lProID,char *pProName, unsigned short ePower);
	bool DelUser(unsigned int m_lUserID);
	bool DelPro(unsigned int m_lGroupID);
	bool DelGroup(unsigned int m_lGroupID);
	CUser * GetUser( unsigned long m_lUserID);
	CProject *  GetProject( unsigned long m_lProID );
	CGroup * GetGroup( unsigned long m_lGroup );
	//bool SetUserPower( unsigned long m_lUserID , unsigned short ePower  );
	//bool SetGroupPower( unsigned long m_lGroupID , unsigned short ePower  );
	bool SetProPower(unsigned long  m_lProID, unsigned long m_lUserID , unsigned short ePower  );
	bool InitDataSet( int iMaxCount  )  ; //最大的内存池个数
	bool UnInitDataSet(  );

};


#define ____INCLUDE__DATASET__H____
#endif  //____INCLUDE__DATASET__H____