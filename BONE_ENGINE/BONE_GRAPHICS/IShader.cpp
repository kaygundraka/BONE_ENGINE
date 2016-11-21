#include "Common.h"
#include "IShader.h"
#include "RenderManager.h"
#include "ResourceManager.h"

namespace BONE_GRAPHICS
{
	IShader::IShader()
	{
		CThreadSync sync;

		SetTypeName("IShader");
		address = "";
	}

	IShader::~IShader() { CThreadSync sync; }

	void IShader::SetAddress(string _address)
	{
		CThreadSync sync;

		address = _address;
		ResourceMgr->LoadEffect(address);
	}

	string IShader::GetAddress()
	{
		CThreadSync sync;

		return address;
	}

	void IShader::LoadContent()
	{
		CThreadSync sync;

		ResourceMgr->LoadEffect(address);
	}

	LPD3DXEFFECT IShader::GetShader()
	{
		CThreadSync sync;

		return ResourceMgr->LoadEffect(address);
	}
}