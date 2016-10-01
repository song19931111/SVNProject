#ifndef ____INCLUDE__C2S__H____
#define ____INCLUDE__C2S__H____
//客户端服务器协议
#include "Common.h"
#define DEF_CS_START ( 2000) 
#define DEF_CS_LOGIN_RQ (DEF_CS_START+1)//登录  
#define DEF_CS_LOGIN_RS  (DEF_CS_START+2)
#define DEF_CS_GET_PRO_LIST_RQ  (DEF_CS_START+3) //获取单级项目列表，一开始获得项目的列表
#define DEF_CS_GET_PRO_LIST_RS (DEF_CS_START+4)
#define DEF_CS_ADD_FILE_RQ  (DEF_CS_START+5)//添加文件
#define DEF_CS_ADD_FILE_RS  (DEF_CS_START+6)
#define DEF_CS_ADD_FOLDER_RQ   (DEF_CS_START+7)//添加目录
#define DEF_CS_ADD_FOLDER_RS  (DEF_CS_START+8)
#define DEF_CS_DELETE_FILE_RQ   (DEF_CS_START+9)//删除文件
#define DEF_CS_DELETE_FILE_RS  (DEF_CS_START+10)
#define DEF_CS_DELETE_FOLDER_RQ  (DEF_CS_START+11)//删除目录
#define DEF_CS_DELETE_FOLDER_RS (DEF_CS_START+12)
#define DEF_CS_SUBMIT_PRO_RQ (DEF_CS_START+13)//提交项目
#define DEF_CS_SUBMIT_PRO_RS  (DEF_CS_START+14)
#define DEF_CS_LOCK_PRO_RQ  (DEF_CS_START+15)//锁定项目 //只能看，不能下载，不能读。改权限改为只读
#define DEF_CS_LOCK_PRO_RS (DEF_CS_START+16)
#define DEF_CS_UNLOCK_PRO_RQ  (DEF_CS_START+17)//解锁项目
#define DEF_CS_UNLOCK_PRO_RS (DEF_CS_START+18)
#define DEF_CS_UPDATE_PRO_RQ (DEF_CS_START+19)//更新  什么时间更新 新增的删除的变化的
#define DEF_CS_UPDATE_PRO_RS  (DEF_CS_START+20)//更新
#define DEF_CS_HISTORY_LOG_RQ (DEF_CS_START+21)//查看目录或者文件夹的历史记录
#define DEF_CS_HISTORY_LOG_RS (DEF_CS_START+22)//查看目录或者文件夹的历史记录
#define DEF_CS_COMPARE_RQ (DEF_CS_START+23)//文件比较
#define DEF_CS_COMPARE_RS (DEF_CS_START+24)//文件比较
#define DEF_CS_ROLLBACK_RQ (DEF_CS_START+25)//回滚
#define DEF_CS_ROLLBACK_RS (DEF_CS_START+26)//回滚
#define DEF_CS_SEARCH_BY_FILENAME_RQ  (DEF_CS_START+27)//按文件名搜索内容 
#define DEF_CS_SEARCH_BY_FILENAME_RS  (DEF_CS_START+28)
#define DEF_CS_LOGOUT_RQ  (DEF_CS_START+29)//退出
#define  DEF_CS_LOGOUT_RS  (DEF_CS_START+30)//退出
struct PACK_BASE{
int m_iType;
int m_iPackSize ;
PACK_BASE(int iType ):m_iType(iType ){}
virtual long Seriaze(char szbuf[],long lBuflen) =  0;
virtual bool UnSeriaze(char szbuf[],long lBuflen) = 0;
};
//////////////////////////用户登录///////////////////////////////////////////////////////////////////////////////////////
struct STRU_CS_LOGIN_RQ :public PACK_BASE{

	static int long m_lMinLength; 
	long m_lUserID;
	long m_lPassLen;
	char m_szPassLen[DEF_PASS_LEN];
	STRU_CS_LOGIN_RQ():PACK_BASE(DEF_CS_LOGIN_RQ){}
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_CS_LOGIN_RQ::m_lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{	
		return true;
	}

};
struct STRU_CS_LOGIN_RS :public PACK_BASE{

	static  long m_lMinLength; 
	long m_lUserID;
	long m_lVeriftyCode; //对用户的身份进行校验
	STRU_CS_LOGIN_RS():PACK_BASE(DEF_CS_LOGIN_RS){}
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_CS_LOGIN_RS::m_lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{
		return true ;
	}

};
//////////////////////////获取项目列表///////////////////////////////////////////////////////////////////////////////////////

struct STRU_CS_GET_PRO_RQ :public PACK_BASE{

	static int long m_lMinLength; 
	long m_lUserID;
	long m_lVerifyCode ;//身份验证标识
	unsigned short m_wPathLen;
	char m_szPath[DEF_MAX_PATH_LEN];//取该路径下的身份标识
	unsigned long m_ulSeqNum; // 序列号用于在客户端存储每个请求结构
	STRU_CS_GET_PRO_RQ():PACK_BASE(DEF_CS_GET_PRO_LIST_RQ){}
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_CS_GET_PRO_RQ::m_lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{	
		return true;
	}

};
//定义文件项,不要隔代管理.让STRU_CS_GET_PRO_RS 提供Seriaze方法，去管理STRU_FILE_ITEM
struct STRU_FILE_ITEM{
	STRU_FILE_ITEM(){}
	static long m_lMinLength;
	unsigned short m_wFileLen; //文件长度
	unsigned short m_wFileType ;//文件类型
	unsigned long  m_ulFileSize ;//文件大小
	unsigned long m_ulTranTime ;//上传时间
	char m_szFile[DEF_MAX_PATH_LEN];
	
	virtual long Seriaze(char* &szbuf,long & lBuflen)
	{
		return STRU_FILE_ITEM::m_lMinLength ;
	}
	virtual bool UnSeriaze(char * &szbuf,long &lBuflen)
	{
		return true ;
	}


};
struct STRU_CS_GET_PRO_RS :public PACK_BASE{

	static int long m_lMinLength; 
	long m_lUserID;
	unsigned long m_ulSeqNum; // 序列号用于在客户端存储每个请求结构
	unsigned short m_wItemCount ; //文件的个数
	STRU_FILE_ITEM m_stru_file_item[DEF_MAX_FILE_ITEM_COUNT];
	STRU_CS_GET_PRO_RS():PACK_BASE(DEF_CS_GET_PRO_LIST_RQ){}
	long Seriaze(char szbuf[],long lBuflen)
	{
		return STRU_CS_GET_PRO_RS::m_lMinLength;
	}
	bool UnSeriaze(char szbuf[],long lBuflen)
	{	
		return true;
	}

};
//////////////////////////添加文件///////////////////////////////////////////////////////////////////////////////////////



//////////////////////////添加目录///////////////////////////////////////////////////////////////////////////////////////










#endif //____INCLUDE__C2S__H____