#pragma once
#include "Common.h"

namespace BONE_GRAPHICS
{
	template <typename T>
	class ISingleton
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
			if (pInst == nullptr)
				pInst = new T;

			return pInst;
		}

		virtual void ReleaseMembers() = 0;

	};

	template <typename T>
	T* ISingleton<T>::pInst = nullptr;
}

