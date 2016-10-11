#pragma once 
#include "SM2S.h"
#define ADD_PROTOCOL(TYPE)\
	case DEF_SM2S_##TYPE##_RS: \
	return new STRU_##TYPE##_RS;\
			break ;
class CFactoryPack {

public :
	static  PACK_BASE * CreateObject( int type  )
	{
		switch (type)
		{
			ADD_PROTOCOL(ADD_USER)
			ADD_PROTOCOL(DELETE_USER)	
			ADD_PROTOCOL(ADD_GROUP)
			ADD_PROTOCOL(JOIN_GROUP)
			ADD_PROTOCOL(LEAVE_GROUP)
			ADD_PROTOCOL(DELETE_GROUP)
			ADD_PROTOCOL(ADD_PRO)
			ADD_PROTOCOL(JOIN_PRO)
			ADD_PROTOCOL(LEAVE_PRO)
			ADD_PROTOCOL(DELETE_PRO)
			ADD_PROTOCOL(SET_POWER)
			ADD_PROTOCOL(GET_USER_LIST)
			ADD_PROTOCOL(GET_PRO_LIST)
			ADD_PROTOCOL(GET_GROUP_LIST)
		default:
			return NULL;
			break ; 
		}
	}

};