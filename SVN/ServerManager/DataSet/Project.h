#pragma once 
#ifndef ____INCLUDE__PROJECT__H____
#define ____INCLUDE__PROJECT__H____
#include "Common.h"
#include <set>
using namespace std;
typedef set<long long > PRO_USERLIST;  //��Ŀ�е�������û��б�
class CProject{
public :
	unsigned long m_lProjectID ; //�Ƿ��������͵�
	unsigned long m_lProLen; 
	char m_szProName[DEF_PRO_LEN];

	unsigned long m_lCreateTime ;
	unsigned short m_ePower ;
	PRO_USERLIST m_setUserList ;  //Ȩ��(��32)+ID(��32)
	CProject():m_lProjectID(0),m_lProLen(0),m_lCreateTime(0),m_ePower(0)
	{
		memset(m_szProName,0,sizeof(m_szProName));
	}
	bool SetProPower( unsigned long m_lID, unsigned short ePower )
	{
		PRO_USERLIST::iterator ite  = m_setUserList.begin();
		while( ite != m_setUserList.end() )
		{
			if ( (unsigned long )(*ite) == m_lID )
			{
				int  value =  ( unsigned long  )m_lID + ePower<<32;
				m_setUserList.erase(ite);
				m_setUserList.insert(value);
				return true; 
			}
			ite++;
		}
		return false;
	}
	void SetProName(char *pProName)
	{
		if( NULL == pProName )
		{
			return ;
		}
		strcpy(m_szProName,pProName);
	}
	bool AddUser(unsigned long m_lUserID, unsigned short ePower)
	{
		long long id =  (unsigned long )m_lUserID  + (ePower)<<32;
		PRO_USERLIST::iterator ite  ;
		if ( (ite = m_setUserList.find( id ) ) != m_setUserList.end() )
		{
			//�Ѿ���������û�
			return false; 
		}
		m_setUserList.insert(id);
		return true;
	}
	void SetProID( unsigned long lProID )
	{
		this->m_lProjectID = lProID;
	}
	bool AddUser( set<long long >set_user )
	{
		m_setUserList.insert(set_user.begin(),set_user.end());
		return true ; 
	}
	bool AddGroup(unsigned long m_GroupID, unsigned short ePower)
	{
		long long id =  (unsigned long )m_GroupID  + (ePower)<<32;
		PRO_USERLIST::iterator ite  ;
		if ( (ite = m_setUserList.find( id ) ) != m_setUserList.end() )
		{
			//�Ѿ����������
			return false; 
		}
		m_setUserList.insert(id);
		return true;
	}
	bool DeleteUser( unsigned long m_lUserID )
	{
		PRO_USERLIST::iterator ite  = m_setUserList.begin();
		while( ite != m_setUserList.end() )
		{
			if ( (unsigned long )(*ite) == m_lUserID )
			{
				m_setUserList.erase(ite);
				return true; 
			}
			ite++;
		}
		return false;
	}
	bool DeleteGroup( unsigned long m_lGroupID )
	{
		PRO_USERLIST::iterator ite  = m_setUserList.begin();
		while( ite != m_setUserList.end() )
		{
			if ( (unsigned long )(*ite) == m_lGroupID )
			{
				m_setUserList.erase(ite);
				return true; 
			}
			ite++;
		}
		return false;
	}
};


#endif //____INCLUDE__PROJECT__H____