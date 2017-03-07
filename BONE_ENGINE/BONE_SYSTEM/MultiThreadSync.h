#pragma once
#include "Common.h"

namespace BONE_SYSTEM
{
	class CriticalSection
	{
	private:
		CRITICAL_SECTION sync;

	public:
		CriticalSection(VOID)
		{
			InitializeCriticalSection(&sync);
		}

		~CriticalSection(VOID)
		{
			DeleteCriticalSection(&sync);
		}

		inline VOID Enter(VOID)
		{
			EnterCriticalSection(&sync);
		}

		inline VOID Leave(VOID)
		{
			LeaveCriticalSection(&sync);
		}
	};

	template <class T>
	class MultiThreadSync {
		friend class ThreadSync;

	public:
		class ThreadSync
		{
		public:
            ThreadSync(VOID)
			{
				T::sync.Enter();
			}

			~ThreadSync(VOID)
			{
				T::sync.Leave();
			}
		};

	private:
		static CriticalSection sync;
	};

	template<class T>
	CriticalSection MultiThreadSync<T>::sync;
}

