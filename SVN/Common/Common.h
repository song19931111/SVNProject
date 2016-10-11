#define DEF_MAX_PATH_LEN (100)
#define DEF_MAX_FILE_ITEM_COUNT (10)
#define DEF_PASS_LEN (100)
#define DEF_GROUP_LEN (200)
#define DEF_PRO_LEN (200)
#define DEF_TCP_PORT ( 12345 )
#define DEF_UDP_PORT ( 54321 )
#define DEF_SERVER_IP "127.0.0.1" 
#define DEF_MAX_RECV_BUF (2048) 
#define DEF_MAX_GET_USER_COUNT ( 10  )
#define DEF_MAX_GET_GROUP_COUNT ( 5  )
#define DEF_MAX_GET_PROJECT_COUNT ( 5  )
#define DEF_MAX_PACK_COUNT ( 1000 )  //同时存在的包数量
#define DEF_STATUS_WAIT_TIME ( 5000 )  //状态机等待的 时间
#include <iostream>
//权限
enum power_type{enum_only_read,enum_read_wirte,enum_not_accept};
//状态机:
enum enum_status { enum_status_init,enum_status_wait ,enum_status_response ,enum_status_complete} ;

//定义两个观察者的接口
class IObServer {
public :
virtual bool Close(  )  =  0 ;
}; 
class IObServerMember {

public :
	virtual bool AddObserver ( IObServer *  pObserver )  = 0 ;
	virtual bool RemObserver ( IObServer *  pObserver )  = 0 ;
};