#include "Kernel.h"
void CKernel::DealStatus( STRU_TASK * * pTask  )
{
	switch (  (*pTask ) ->m_eStatusType )
	{
	case enum_status_init:
	{
		//TODO :
		//����������,����

		//��״̬λ�޸�Ϊwait

		//�޸�ʱ��:
	
	}
		break ; 
	case enum_status_wait:
	{
		//TODO :
		if ( GetTickCount() -  ( *pTask ) ->m_ulTime > DEF_STATUS_WAIT_TIME )
		{
			//��״̬λ�޸�Ϊinit 
           //���·�����е���
		}
	}
		break ; 
	case enum_status_response:
	{
		//TODO :
		//���ö�Ӧ��RS������֪ͨUI
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