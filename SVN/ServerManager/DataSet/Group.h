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
	
	void SetPower(power_type ePower)
	{
		m_ePower = ePower;
	}
	bool  JoinGroup( unsigned long m_lUserID )
	{
			//������
		if ( m_userSet.find( m_lUserID ) != m_userSet.end())
		{
			//�û��Ѿ��洢������
			return false ;
		}
		m_userSet.insert(m_lUserID);
		return true ;
	}
	bool LeaveGroup( unsigned long m_lUserID )
	{
		USERSET::iterator ite ;
		//�뿪��
		if ( (ite  = m_userSet.find(m_lUserID))  != m_userSet.end() )
		{
			m_userSet.erase(ite);
			return true; 
		}
		return false ;
	}
};
#endif  //____INCLUDE__GROUP__H____