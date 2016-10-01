#ifndef __INCLUDE__SM2S__H____
#include "Common.h"
#define DEF_SM2S_START (1000)
#define DEF_SM2S_COUNT (1000)

//添加用户
#define DEF_SM2S_ADD_USER_RQ		DEF_SM2S_START+1
#define DEF_SM2S_ADD_USER_RS		DEF_SM2S_START+2

//删除用户
#define DEF_SM2S_DELETE_USER_RQ		DEF_SM2S_START+3
#define DEF_SM2S_DELETE_USER_RS		DEF_SM2S_START+4

//添加组
#define DEF_SM2S_ADD_GROUP_RQ		DEF_SM2S_START+5
#define DEF_SM2S_ADD_GROUP_RS		DEF_SM2S_START+6

//加入组
#define DEF_SM2S_JOIN_GROUP_RQ		DEF_SM2S_START+7
#define DEF_SM2S_JOIN_GROUP_RS		DEF_SM2S_START+8

//删除组
#define DEF_SM2S_DELETE_GROUP_RQ		DEF_SM2S_START+9
#define DEF_SM2S_DELETE_GROUP_RS		DEF_SM2S_START+10

//离开组
#define DEF_SM2S_LEAVE_GROUP_RQ		DEF_SM2S_START+11
#define DEF_SM2S_LEAVE_GROUP_RS		DEF_SM2S_START+12


//添加项目
#define DEF_SM2S_ADD_PRO_RQ		DEF_SM2S_START+13
#define DEF_SM2S_ADD_PRO_RS		DEF_SM2S_START+14
//加入项目
#define DEF_SM2S_JOIN_PRO_RQ		DEF_SM2S_START+15
#define DEF_SM2S_JOIN_PRO_RS		DEF_SM2S_START+16

//离开项目
#define DEF_SM2S_LEAVE_PRO_RQ		DEF_SM2S_START+17
#define DEF_SM2S_LEAVE_PRO_RS		DEF_SM2S_START+18
//删除项目
#define DEF_SM2S_DELETE_PRO_RQ		DEF_SM2S_START+19
#define DEF_SM2S_DELETE_PRO_RS		DEF_SM2S_START+20
//设置权限
#define DEF_SM2S_SET_POWER_RQ		DEF_SM2S_START+21
#define DEF_SM2S_SET_POWER_RS		DEF_SM2S_START+22



#define DEF_SMS_END (1000) DEF_SMS_START+DEF_SMS_COUNT


struct PACK_BASE{
PACK_BASE(int nPackType):m_nPackSize(nPackType){}
unsigned int m_nPackSize;
unsigned int m_nPackType ;
virtual long Seriaze(char szbuf[],long lBuflen) = 0 ;
virtual bool UnSeriaze(char szbuf[],long lBuflen) = 0 ;
};
///////////////////////////////////////////////////////////////////////////////////////////////
struct STRU_ADD_USER_RQ :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lUserID;
	long m_lPassLen;
	char m_szPassword[DEF_PASS_LEN];
	STRU_ADD_USER_RQ():PACK_BASE(DEF_SM2S_ADD_USER_RQ){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_ADD_USER_RQ::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
enum add_user_result{enum_add_success,enum_add_failed};
struct STRU_ADD_USER_RS :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lUserID;
	unsigned short  iResult; //结果
	STRU_ADD_USER_RS():PACK_BASE(DEF_SM2S_ADD_USER_RS){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_ADD_USER_RS::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
/////////////////////////////////////删除用户/////////////////////////////////////////////////////////
enum delete_user_result{enum_delete_success,enum_delete_failed};
struct STRU_DELETE_USER_RQ :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lUserID;
	STRU_DELETE_USER_RQ():PACK_BASE(DEF_SM2S_DELETE_USER_RQ){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_DELETE_USER_RQ::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
struct STRU_DELETE_USER_RS :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lUserID;
	unsigned short m_iResult;
	STRU_DELETE_USER_RS():PACK_BASE(DEF_SM2S_DELETE_USER_RS){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_DELETE_USER_RS::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
/////////////////////////添加组///////////////////////////////////////////////////////////////////////

enum add_group_result{enum_add_group_success,enum_group_exist,enum_add_group_failed};
struct STRU_ADD_GROUP_RQ :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lGroupLen; 
	char m_szGroup[DEF_GROUP_LEN];
	STRU_ADD_GROUP_RQ():PACK_BASE(DEF_SM2S_ADD_GROUP_RQ){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_ADD_GROUP_RQ::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
struct STRU_ADD_GROUP_RS :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lGroupLen; 
	char m_szGroup[DEF_GROUP_LEN];
	unsigned short m_iResult;
	STRU_ADD_GROUP_RS():PACK_BASE(DEF_SM2S_ADD_GROUP_RS){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_ADD_GROUP_RS::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
/////////////////////////加入组///////////////////////////////////////////////////////////////////////
enum join_group_result{enum_join_group_success,enum_join_failed};
struct STRU_JOIN_GROUP_RQ :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_iUserID;
	long m_lGroupLen; 
	char m_szGroup[DEF_GROUP_LEN];
	
	STRU_JOIN_GROUP_RQ():PACK_BASE(DEF_SM2S_JOIN_GROUP_RQ){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_JOIN_GROUP_RQ::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
struct STRU_JOIN_GROUP_RS :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_iUserID;
	long m_lGroupLen; 
	unsigned short m_iResult ; 
	char m_szGroup[DEF_GROUP_LEN];
	
	STRU_JOIN_GROUP_RS():PACK_BASE(DEF_SM2S_JOIN_GROUP_RS){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_JOIN_GROUP_RS::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
/////////////////////////////////删除组/////////////////////////////////////////////////////////////////
enum delete_group_result{enum_delete_group_success,enum_delete_failed};
struct STRU_DELETE_GROUP_RQ :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lGroupLen; 
	char m_szGroup[DEF_GROUP_LEN];
	
	STRU_DELETE_GROUP_RQ():PACK_BASE(DEF_SM2S_DELETE_GROUP_RQ){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_DELETE_GROUP_RQ::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
struct STRU_DELETE_GROUP_RS :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lGroupLen; 
	unsigned short m_iResult ; 
	char m_szGroup[DEF_GROUP_LEN];
	
	STRU_DELETE_GROUP_RS():PACK_BASE(DEF_SM2S_DELETE_GROUP_RS){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_DELETE_GROUP_RS::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
/////////////////////////////////离开组/////////////////////////////////////////////////////////////////
enum leave_group_result{enum_leave_group_success,enum_leave_failed};
struct STRU_LEAVE_GROUP_RQ :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_iUserID;
	long m_lGroupLen; 
	char m_szGroup[DEF_GROUP_LEN];
	
	STRU_LEAVE_GROUP_RQ():PACK_BASE(DEF_SM2S_LEAVE_GROUP_RQ){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_LEAVE_GROUP_RQ::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
struct STRU_LEAVE_GROUP_RS :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lGroupLen; 
	long m_iUserID;
	int m_iResult ; 
	char m_szGroup[DEF_GROUP_LEN];
	
	STRU_LEAVE_GROUP_RS():PACK_BASE(DEF_SM2S_LEAVE_GROUP_RS){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_LEAVE_GROUP_RS::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};




/////////////////////////////////添加项目/////////////////////////////////////////////////////////////////
enum add_pro_result{enum_add_pro_success,enum_add_pro_failed};
struct STRU_ADD_PRO_RQ :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lProNameLen;
	char m_szProName[DEF_PRO_LEN];
	
	STRU_ADD_PRO_RQ():PACK_BASE(DEF_SM2S_ADD_PRO_RQ){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_ADD_PRO_RQ::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
struct STRU_ADD_PRO_RS :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lProNameLen;
	unsigned short m_iResult;
	char m_szProName[DEF_PRO_LEN];
	
	STRU_ADD_PRO_RS():PACK_BASE(DEF_SM2S_ADD_PRO_RS){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_ADD_PRO_RS::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};


/////////////////////////////////删除项目/////////////////////////////////////////////////////////////////
enum add_pro_result{enum_delete_pro_success,enum_delete_pro_failed};
struct STRU_DELETE_PRO_RQ :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lProNameLen;
	char m_szProName[DEF_PRO_LEN];
	
	STRU_DELETE_PRO_RQ():PACK_BASE(DEF_SM2S_DELETE_PRO_RQ){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_DELETE_PRO_RQ::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
struct STRU_DELETE_PRO_RS :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lProNameLen;
	unsigned short m_iResult;
	char m_szProName[DEF_PRO_LEN];
	
	STRU_DELETE_PRO_RS():PACK_BASE(DEF_SM2S_DELETE_PRO_RS){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_DELETE_PRO_RS::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
////////////////////////////////加入项目////////////////////////////////////////////////////////////////
enum join_pro_result{enum_join_pro_success,enum_join_pro_failed};
struct STRU_JOIN_PRO_RQ :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lUserID;
	long m_lProNameLen;
	char m_szProName[DEF_PRO_LEN];
	
	STRU_JOIN_PRO_RQ():PACK_BASE(DEF_SM2S_JOIN_PRO_RQ){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_JOIN_PRO_RQ::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
struct STRU_JOIN_PRO_RS :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lUserID;
	long m_lProNameLen;
	unsigned short m_iResult; 
	char m_szProName[DEF_PRO_LEN];
	
	STRU_JOIN_PRO_RS():PACK_BASE(DEF_SM2S_JOIN_PRO_RS){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_JOIN_PRO_RS::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
/////////////////////////////////离开项目/////////////////////////////////////////////////////////////////
enum leave_pro_result{enum_leave_pro_success,enum_leave_pro_failed};
struct STRU_LEAVE_PRO_RQ :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lUserID;
	long m_lProNameLen;
	char m_szProName[DEF_PRO_LEN];
	
	STRU_LEAVE_PRO_RQ():PACK_BASE(DEF_SM2S_LEAVE_PRO_RQ){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_LEAVE_PRO_RQ::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
struct STRU_LEAVE_PRO_RS :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lUserID;
	long m_lProNameLen;
	unsigned short m_iResult; 
	char m_szProName[DEF_PRO_LEN];
	
	STRU_LEAVE_PRO_RS():PACK_BASE(DEF_SM2S_LEAVE_PRO_RS){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_LEAVE_PRO_RS::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};


/////////////////////////////////设置权限项目/////////////////////////////////////////////////////////////////


struct STRU_SET_POWER_RQ :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lUserID; //用户ＩＤ为０的时候，设置组权限
	long m_lGroupLen;
	long m_lProNameLen;
	char m_szProName[DEF_PRO_LEN];
	char m_szGroup[DEF_GROUP_LEN];
	STRU_SET_POWER_RQ():PACK_BASE(DEF_SM2S_SET_POWER_RQ){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_SET_POWER_RQ::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
struct STRU_SET_POWER_RS :public PACK_BASE{
	static unsigned long lMinLength ; 
	long m_lUserID;
	long m_lGroupLen;
	long m_lProNameLen;
	unsigned short m_iResult; 
	char m_szProName[DEF_PRO_LEN];
	char m_szGroup[DEF_GROUP_LEN];
	STRU_SET_POWER_RS():PACK_BASE(DEF_SM2S_SET_POWER_RS){};
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_SET_POWER_RS::lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}
};
#define __INCLUDE__SM2S__H____
#endif //__INCLUDE__SM2S__H____