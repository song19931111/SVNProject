#pragma once
#include "MyLock.h"
template<typename T>
class CLockQueue
{
public :
	bool m_bIsComplete ; //�Ƿ�����˶Զ��е�ʹ��
//��һ����־:
public:
	bool InitQueue(int);
	bool UnInitQueue();
	bool Push(T data);
	bool Pop(T* data);
	int GetSize();		//��ȡ��ǰ���еĴ�С
	CLockQueue(void);
	~CLockQueue(void);
private:
	int m_nHeadIndex;//ͷָ��
	int m_nEndIndex ;//βָ��
	int m_nQueueLength;//���еĳ���
	int m_nSize;
	T* m_pQueue;//���е�ͷָ��
	bool m_bInitFlag;
	MyLock m_lock;
};
template<typename T>
bool CLockQueue<T>::InitQueue(int nQueueLength)
{
	UnInitQueue();
	if(!m_bInitFlag)//���û�г�ʼ��
	{
		m_bIsComplete = false;
		m_nQueueLength  =nQueueLength;
		m_bInitFlag = true;//ֻ�ܳ�ʼ��һ��
		m_pQueue = new T[m_nQueueLength+1];
		m_nHeadIndex = m_nEndIndex = 0;//ͷβָ��ָ��0
		return true;
	}
	return false;
}
template<typename T>
bool  CLockQueue<T>::UnInitQueue()
{
	if(!m_bInitFlag)
	{
		return false; 
	}
	m_bIsComplete = true ;
	m_bInitFlag = false; //�ñ�־����Ϊ��
	Sleep(100);// ������Դ��ʱ�������������߳̿��ܻ��ڶԶ��еĲ���������,��˯100ms,�ȴ��߳����ڿ���false ��Ƕ���������
	if (m_pQueue!=NULL)
	{
		delete [] m_pQueue;
		m_pQueue=NULL;
		return true;
	}
	return false;
}
template<typename T>
bool  CLockQueue<T>::Push(T data)
{
	if(!m_bInitFlag)
	{
		return false;
	}
	if(m_nHeadIndex==(m_nEndIndex+1)%(m_nQueueLength+1))
	{
		//������
		return false; 
	}
	m_lock.Lock();
	m_pQueue[m_nEndIndex] = data;
	m_nEndIndex =(m_nEndIndex+1)%(m_nQueueLength+1);
	m_lock.UnLock();
	return true;
}
template<typename T>
int   CLockQueue<T>::GetSize()
{
	int nSize =  (m_nEndIndex-m_nHeadIndex+m_nQueueLength+1)%(m_nQueueLength+1);
	return nSize;
}

template<typename T>
bool  CLockQueue<T>::Pop(T* data)
{
	if (!m_bInitFlag)
	{
		return false;
	}
	if (m_nEndIndex==m_nHeadIndex)
	{
		//�ӿ�
		return false ;
	}
	m_lock.Lock();
	*data  = m_pQueue[m_nHeadIndex];
	m_nHeadIndex =(m_nHeadIndex+1)%(m_nQueueLength+1);
	m_lock.UnLock();
	return true;
	
}
template<typename T>
CLockQueue<T>::CLockQueue(void):m_nHeadIndex(0),m_nEndIndex(0),m_bInitFlag(false),m_nQueueLength(0),m_pQueue(NULL),m_nSize(0)
{
	m_bIsComplete = false;
}
template<typename T>
 CLockQueue<T>::~CLockQueue(void)
{
	
}
