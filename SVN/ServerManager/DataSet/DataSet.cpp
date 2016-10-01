#include "DataSet.h"

bool CDataSet::AddUser( unsigned long m_lUserID,char *pPassword,unsigned short ePower)
{
	if ( 0 ==m_lUserID || pPassword ==NULL ||  ePower == 0 )
	{
			return false; 
	}
	MAP_ID_USER::iterator ite;
	if(( ite = m_mp_user.find( m_lUserID )) != m_mp_user.end()  )
	{
		//已经存在这个用户 
		return false;
	}
	//从索引池里取出一个索引:
	int iIndex; 
	if (  false == m_index_user.Pop( &iIndex ) )
	{
		//池空:
		return false; 
	}
	m_pUser[iIndex].SetUserID(m_lUserID);
	m_pUser[iIndex].SetPower(ePower);
	m_pUser[iIndex].SetPassword(pPassword);
	m_mp_user[m_lUserID]  = & m_pUser[iIndex];
		
	return true;
}
bool  CDataSet::AddGroup( unsigned long m_lGroupID,char *pGroupName, unsigned short ePower)
{
	if ( 0 ==m_lGroupID || pGroupName ==NULL ||  ePower == 0 )
	{
			return false; 
	}
	MAP_ID_GROUP::iterator ite;
	if(( ite = m_mp_group.find( m_lGroupID )) != m_mp_group.end()  )
	{
		//已经存在这个用户 
		return false;
	}
	//从索引池里取出一个索引:
	int iIndex; 
	if (  false == m_index_group.Pop( &iIndex ) )
	{
		//池空:
		return false; 
	}
	m_pGroup[iIndex].SetGroupName(pGroupName);
	m_pGroup[iIndex].SetGroupPower(ePower);
	m_mp_group[m_lGroupID]  = & m_pGroup[iIndex];
	return true;
}	
	
bool  CDataSet::AddPro( unsigned long m_lProID,char *pProName, unsigned short ePower)
{
	if ( 0 ==m_lProID || pProName ==NULL ||  ePower == 0 )
	{
			return false; 
	}
	MAP_ID_PRO::iterator ite;
	if(( ite = m_mp_pro.find( m_lProID )) != m_mp_pro.end()  )
	{
		//已经存在这个用户 
		return false;
	}
	//从索引池里取出一个索引:
	int iIndex; 
	if (  false == m_index_pro.Pop( &iIndex ) )
	{
		//池空:
		return false; 
	}
	m_pPro[iIndex].SetProName(pProName);
	m_pPro[iIndex].SetProPower(ePower);
	m_mp_pro[m_lProID]  = & m_pPro[iIndex];
	return true;
}	

bool CDataSet::DelUser(unsigned int m_lUserID)
{
	MAP_ID_USER::iterator ite; 
	if ( ( ite =  m_mp_user.find (  m_lUserID)) != m_mp_user.end() )
	{
		m_mp_user.erase(ite);
		//算出索引:
		int iIndex   =  (ite->second -m_pUser)/sizeof(CUser) ; 
		//回收索引
		m_index_user.Push(iIndex);
		return true; 
	}
	return false; 
}
bool CDataSet::DelGroup(unsigned int m_lGroupID)
{
	MAP_ID_GROUP::iterator ite; 
	if ( ( ite =  m_mp_group.find (  m_lGroupID)) != m_mp_group.end() )
	{
		m_mp_group.erase(ite);
		//算出索引:
		int iIndex   =  (ite->second -m_pGroup)/sizeof(CGroup) ; 
		//回收索引
		m_index_group.Push(iIndex);
		return true; 
	}
	return false; 
}
bool CDataSet::DelPro(unsigned int m_lProID)
{
		MAP_ID_PRO::iterator ite; 
	if ( ( ite =  m_mp_pro.find (  m_lProID)) != m_mp_pro.end() )
	{
		m_mp_pro.erase(ite);
		//算出索引:
		int iIndex   =  (ite->second -m_pPro)/sizeof(CProject) ; 
		//回收索引
		m_index_pro.Push(iIndex);
		return true; 
	}
	return false; 
}

