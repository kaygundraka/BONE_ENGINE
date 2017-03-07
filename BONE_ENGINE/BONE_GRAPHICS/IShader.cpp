#include "Common.h"
#include "IShader.h"
#include "RenderManager.h"
#include "ResourceManager.h"

namespace BONE_GRAPHICS
{
	IShader::IShader()
	{
		ThreadSync sync;

		SetTypeName("IShader");
		address = "";
	}

	IShader::~IShader() { ThreadSync sync; }

	void IShader::SetAddress(string _address)
	{
		ThreadSync sync;

		address = _address;
		ResourceMgr->LoadEffect(address);
	}

	string IShader::GetAddress()
	{
		ThreadSync sync;

		return address;
	}

	void IShader::LoadContent()
	{
		ThreadSync sync;

		ResourceMgr->LoadEffect(address);
	}

	LPD3DXEFFECT IShader::GetShader()
	{
		ThreadSync sync;

		return ResourceMgr->LoadEffect(address);
	}
}