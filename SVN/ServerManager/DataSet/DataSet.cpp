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
//	m_pUser[iIndex].SetPower(ePower);
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
	//m_pGroup[iIndex].SetGroupPower(ePower);
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

CUser * CDataSet::GetUser( unsigned long m_lUserID)
{
	MAP_ID_USER::iterator ite  ;
	if ( ( ite = m_mp_user.find( m_lUserID ) ) ==m_mp_user.end() )
	{
		return NULL;
	}
	return ite->second; 
}
CProject * CDataSet:: GetProject( unsigned long m_lProID )
{
	MAP_ID_PRO::iterator ite  ;
	if ( ( ite = m_mp_pro.find( m_lProID ) ) ==m_mp_pro.end() )
	{
		return NULL;
	}
	return ite->second; 
}
CGroup *CDataSet:: GetGroup( unsigned long m_lGroupID )
{
		MAP_ID_GROUP::iterator ite  ;
	if ( ( ite = m_mp_group.find( m_lGroupID ) ) ==m_mp_group.end() )
	{
		return NULL;
	}
	return ite->second; 
}
//bool CDataSet::SetUserPower( unsigned long m_lUserID , unsigned short ePower  )
//{
//	 CUser * pUser =  GetUser(m_lUserID) ;
//	 if  ( NULL == pUser ) 
//	 {
//		return false ;
//	 }
//	 pUser->SetPower(ePower);
//	 return true; 
//}
//bool CDataSet::SetGroupPower( unsigned long m_lGroupID , unsigned short ePower  )
//{
//	 CGroup * pGroup =  GetGroup(m_lGroupID) ;
//	 if  ( NULL == pGroup ) 
//	 {
//		return false ;
//	 }
//	 pGroup->SetGroupPower(ePower);
//	 return true; 
//}
bool CDataSet::SetProPower(unsigned long  m_lProID, unsigned long m_lUserID , unsigned short ePower  )\
{
		//找到项目:
	CProject * pProject =  GetProject(m_lProID);
	if ( NULL == pProject )
	{
		return false;
	}
	return pProject->SetProPower(m_lUserID,ePower);
}
bool CDataSet::InitDataSet( int iMaxCount  )   //最大的内存池个数
{
	//初始化队列:
	if ( false == UnInitDataSet(  ))
	{
		return false; 
	}
	m_index_user.InitQueue( iMaxCount );
	m_index_group.InitQueue( iMaxCount ) ;
	m_index_pro.InitQueue( iMaxCount ) ;
	//建立内存池
	m_pUser  =new CUser[ iMaxCount ]; 
	m_pGroup  =new CGroup[ iMaxCount ]; 
	m_pPro  =new CProject[ iMaxCount ]; 
	//投入索引:
	for( int i=0;i<iMaxCount;i++ )
	{
		m_index_user.Push(i);
		m_index_group.Push(i);
		m_index_pro.Push(i);
	}

	return true;
}
bool CDataSet::UnInitDataSet(  )
{
	m_index_user.UnInitQueue(  );
	m_index_group.UnInitQueue(  ) ;
	m_index_pro.UnInitQueue(  ) ;

	delete [] m_pUser;
	m_pUser = NULL;
	delete [] m_pGroup;
	m_pGroup = NULL; 
	delete [] m_pPro;
	m_pPro  = NULL;
}