#pragma once 
#ifndef ____INCLUDE__INET__H____
#define ____INCLUDE__INET__H____
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
struct STRU_SESSION{
	long long  m_sock ;
	long m_data ;
	//��UDP�� m_sock��32λ���IP����16λ��Ŷ˿�
	//��TCP��,m_sock��32λ����׽���socket 
};
enum enum_net_type{ enum_tcp_type, enum_udp_type} ;
class INotify{
public :
	virtual void NotiftyRecvData(  STRU_SESSION  *pSession ,char szbuf[],long lBuflen,unsigned short eNetType)= 0 ;

};


class INet : public IObServer{
public :
	virtual long SendData( char szBuf[],long lBuflen ) =  0;
	virtual bool Init( INotify *m_pNotify ) = 0;
	virtual bool UnInit() = 0;
	virtual bool Close()  =  0 ;
	virtual bool NofityNetDisconnet( STRU_SESSION *pSession  ) =  0 ;
public :
	INotify *m_pNotify; 


};
#endif //____INCLUDE__INET__H____