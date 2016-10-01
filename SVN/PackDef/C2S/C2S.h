#ifndef ____INCLUDE__C2S__H____
#define ____INCLUDE__C2S__H____
//�ͻ��˷�����Э��
#include "Common.h"
#define DEF_CS_START ( 2000) 
#define DEF_CS_LOGIN_RQ (DEF_CS_START+1)//��¼  
#define DEF_CS_LOGIN_RS  (DEF_CS_START+2)
#define DEF_CS_GET_PRO_LIST_RQ  (DEF_CS_START+3) //��ȡ������Ŀ�б�һ��ʼ�����Ŀ���б�
#define DEF_CS_GET_PRO_LIST_RS (DEF_CS_START+4)
#define DEF_CS_ADD_FILE_RQ  (DEF_CS_START+5)//����ļ�
#define DEF_CS_ADD_FILE_RS  (DEF_CS_START+6)
#define DEF_CS_ADD_FOLDER_RQ   (DEF_CS_START+7)//���Ŀ¼
#define DEF_CS_ADD_FOLDER_RS  (DEF_CS_START+8)
#define DEF_CS_DELETE_FILE_RQ   (DEF_CS_START+9)//ɾ���ļ�
#define DEF_CS_DELETE_FILE_RS  (DEF_CS_START+10)
#define DEF_CS_DELETE_FOLDER_RQ  (DEF_CS_START+11)//ɾ��Ŀ¼
#define DEF_CS_DELETE_FOLDER_RS (DEF_CS_START+12)
#define DEF_CS_SUBMIT_PRO_RQ (DEF_CS_START+13)//�ύ��Ŀ
#define DEF_CS_SUBMIT_PRO_RS  (DEF_CS_START+14)
#define DEF_CS_LOCK_PRO_RQ  (DEF_CS_START+15)//������Ŀ //ֻ�ܿ����������أ����ܶ�����Ȩ�޸�Ϊֻ��
#define DEF_CS_LOCK_PRO_RS (DEF_CS_START+16)
#define DEF_CS_UNLOCK_PRO_RQ  (DEF_CS_START+17)//������Ŀ
#define DEF_CS_UNLOCK_PRO_RS (DEF_CS_START+18)
#define DEF_CS_UPDATE_PRO_RQ (DEF_CS_START+19)//����  ʲôʱ����� ������ɾ���ı仯��
#define DEF_CS_UPDATE_PRO_RS  (DEF_CS_START+20)//����
#define DEF_CS_HISTORY_LOG_RQ (DEF_CS_START+21)//�鿴Ŀ¼�����ļ��е���ʷ��¼
#define DEF_CS_HISTORY_LOG_RS (DEF_CS_START+22)//�鿴Ŀ¼�����ļ��е���ʷ��¼
#define DEF_CS_COMPARE_RQ (DEF_CS_START+23)//�ļ��Ƚ�
#define DEF_CS_COMPARE_RS (DEF_CS_START+24)//�ļ��Ƚ�
#define DEF_CS_ROLLBACK_RQ (DEF_CS_START+25)//�ع�
#define DEF_CS_ROLLBACK_RS (DEF_CS_START+26)//�ع�
#define DEF_CS_SEARCH_BY_FILENAME_RQ  (DEF_CS_START+27)//���ļ����������� 
#define DEF_CS_SEARCH_BY_FILENAME_RS  (DEF_CS_START+28)
#define DEF_CS_LOGOUT_RQ  (DEF_CS_START+29)//�˳�
#define  DEF_CS_LOGOUT_RS  (DEF_CS_START+30)//�˳�
struct PACK_BASE{
int m_iType;
int m_iPackSize ;
PACK_BASE(int iType ):m_iType(iType ){}
virtual long Seriaze(char szbuf[],long lBuflen) =  0;
virtual bool UnSeriaze(char szbuf[],long lBuflen) = 0;
};
//////////////////////////�û���¼///////////////////////////////////////////////////////////////////////////////////////
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
	long m_lVeriftyCode; //���û�����ݽ���У��
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
//////////////////////////��ȡ��Ŀ�б�///////////////////////////////////////////////////////////////////////////////////////

struct STRU_CS_GET_PRO_RQ :public PACK_BASE{

	static int long m_lMinLength; 
	long m_lUserID;
	long m_lVerifyCode ;//�����֤��ʶ
	unsigned short m_wPathLen;
	char m_szPath[DEF_MAX_PATH_LEN];//ȡ��·���µ���ݱ�ʶ
	unsigned long m_ulSeqNum; // ���к������ڿͻ��˴洢ÿ������ṹ
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
//�����ļ���,��Ҫ��������.��STRU_CS_GET_PRO_RS �ṩSeriaze������ȥ����STRU_FILE_ITEM
struct STRU_FILE_ITEM{
	STRU_FILE_ITEM(){}
	static long m_lMinLength;
	unsigned short m_wFileLen; //�ļ�����
	unsigned short m_wFileType ;//�ļ�����
	unsigned long  m_ulFileSize ;//�ļ���С
	unsigned long m_ulTranTime ;//�ϴ�ʱ��
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
	unsigned long m_ulSeqNum; // ���к������ڿͻ��˴洢ÿ������ṹ
	unsigned short m_wItemCount ; //�ļ��ĸ���
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
//////////////////////////����ļ�///////////////////////////////////////////////////////////////////////////////////////



//////////////////////////���Ŀ¼///////////////////////////////////////////////////////////////////////////////////////










#endif //____INCLUDE__C2S__H____