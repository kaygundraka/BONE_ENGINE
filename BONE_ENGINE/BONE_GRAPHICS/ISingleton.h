#pragma once
#include "stddef.h"
#include <assert.h>

namespace BONE_GRAPHICS
{
	template <typename T>
	class ISingleton abstract
	{
	private:
		static T* pInst;

	protected:
		bool Is_init;

		ISingleton()
		{}

		virtual ~ISingleton()
		{}

	public:
		static T* GetInstance()
		{
			if (pInst == NULL)
				pInst = new T;

			return pInst;
		}

		virtual void ReleaseMembers() = 0;

	};

	template <typename T>
	T* ISingleton<T>::pInst = NULL;
}

