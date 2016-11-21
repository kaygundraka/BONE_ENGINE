#pragma once

namespace BONE_GRAPHICS
{
	template <typename T>
	class SmartPointer
	{
	private:
		T* instance;

	public:
		SmartPointer(T* _instance)
		{
			instance = _instance;
		}

		virtual ~SmartPointer()
		{
			delete instance;
		}

		T* operator*()
		{
			return instance;
		}

		T* operator->() const
		{
			return instance;
		}
	};
}