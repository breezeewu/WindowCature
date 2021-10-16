#pragma once
#include <windows.h>
//Lock
class CCriSec
{
	public:
	CCriSec()
	{
		m_pcs = new RTL_CRITICAL_SECTION;

		//��ʼ���ٽ�����Դ
		::InitializeCriticalSection(m_pcs);
	}

	~CCriSec()
	{
		//�����ٽ���
		if(m_pcs)
		{
			//�뿪�ٽ���
			LeaveCriticalSection(m_pcs);
			//�����ٽ�����Դ
			::DeleteCriticalSection(m_pcs);
			delete m_pcs;
			m_pcs = NULL;
		}
	}

	void Lock()
	{
		//�����ٽ���
		EnterCriticalSection(m_pcs);
	}

	void UnLock()
	{
		//�뿪�ٽ���
		LeaveCriticalSection(m_pcs);
	}


protected:
	RTL_CRITICAL_SECTION*			m_pcs;
};

class CLazyLock
{
public:
	CLazyLock(CCriSec& cirsec):m_Crisec(cirsec)
	{
		m_Crisec.Lock();
	}

	~CLazyLock()
	{
		m_Crisec.UnLock();
	}
private:
	CCriSec&	m_Crisec;
};
