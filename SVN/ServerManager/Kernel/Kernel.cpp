#include "Kernel.h"
void CKernel::DealStatus( STRU_TASK * * pTask  )
{
	switch (  (*pTask ) ->m_eStatusType )
	{
	case enum_status_init:
	{
		//TODO :
		//调用网络类,发送

		//将状态位修改为wait

		//修改时间:
	
	}
		break ; 
	case enum_status_wait:
	{
		//TODO :
		if ( GetTickCount() -  ( *pTask ) ->m_ulTime > DEF_STATUS_WAIT_TIME )
		{
			//将状态位修改为init 
           //重新放入队列当中
		}
	}
		break ; 
	case enum_status_response:
	{
		//TODO :
		//调用对应的RS处理函数通知UI
	}
		break ; 
	case enum_status_complete:
	{
		

	}
		break ; 
		default :
			{
					( *pTask )->Destory();
				delete ( *pTask );
				( *pTask ) =  NULL;
			}
	}
}