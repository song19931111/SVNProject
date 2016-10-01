#ifndef ____INCLUDE__GROUP__H____
#define ____INCLUDE__GROUP__H____
#include "Group.h"
#include "Common.h"
#include <set>
using namespace std ;
typedef  set<unsigned long > USERSET;
class CGroup{
public  :
	unsigned long m_lGroupLen ;
	char m_szGroupName[DEF_GROUP_LEN];
	unsigned short m_ePower ;
	unsigned long m_lGroupID;
	USERSET m_userSet ;

public :
	
	CGroup()
	{
		m_lGroupLen = 0 ;
		memset(m_szGroupName,0,sizeof(  m_szGroupName ));
		m_ePower =  0;
	}
	
	bool  JoinGroup( unsigned long m_lUserID )
	{
			//加入组
		if ( m_userSet.find( m_lUserID ) != m_userSet.end())
		{
			//用户已经存储在组了
			return false ;
		}
		m_userSet.insert(m_lUserID);
		return true ;
	}
	bool LeaveGroup( unsigned long m_lUserID )
	{
		USERSET::iterator ite ;
		//离开组
		if ( (ite  = m_userSet.find(m_lUserID))  != m_userSet.end() )
		{
			m_userSet.erase(ite);
			return true; 
		}
		return false ;
	}
	void SetGroupName( char *pGroupName )
	{
		if( NULL ==  pGroupName )
		{
			return;
		}
		strcpy(m_szGroupName,pGroupName);
	}
	void SetGroupPower( unsigned short ePower )
	{
		m_ePower  =ePower;
	}
	
};
#endif  //____INCLUDE__GROUP__H____