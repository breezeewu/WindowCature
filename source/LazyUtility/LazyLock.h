#pragma once
#include <windows.h>
//Lock
class CCriSec
{
	public:
	CCriSec()
	{
		m_pcs = new RTL_CRITICAL_SECTION;

		//初始化临界区资源
		::InitializeCriticalSection(m_pcs);
	}

	~CCriSec()
	{
		//销毁临界锁
		if(m_pcs)
		{
			//离开临界区
			LeaveCriticalSection(m_pcs);
			//销毁临界区资源
			::DeleteCriticalSection(m_pcs);
			delete m_pcs;
			m_pcs = NULL;
		}
	}

	void Lock()
	{
		//进入临界区
		EnterCriticalSection(m_pcs);
	}

	void UnLock()
	{
		//离开临界区
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
