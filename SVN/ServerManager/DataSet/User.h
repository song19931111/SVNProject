#pragma once 
#ifndef ____INCLUDE__USER__H____
#define ____INCLUDE__USER__H____
#include "Common.h"
class CUser{
private :
	unsigned long m_lUserID; 
private :
	unsigned long m_lPassLen;
	char m_szPassword[DEF_PASS_LEN];
	unsigned short m_ePower ;
public :
	void SetPower( unsigned short  eType )
	{
		m_ePower = eType;
	}
	void SetUserID(unsigned long lUserID)
	{
		m_lUserID = lUserID;
	}
	unsigned long  GetUSerID(   )
	{
		return m_lUserID;
	}
	void SetPassword(char *pPassword)
	{
		if ( NULL == pPassword )
		{
			return; 
		}
		strcpy(m_szPassword,pPassword);
	}
};
#endif 