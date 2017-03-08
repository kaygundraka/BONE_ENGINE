#include "Common.h"
#include "IShader.h"
#include "RenderManager.h"
#include "ResourceManager.h"

namespace BONE_GRAPHICS
{
	IShader::IShader()
	{
		SetTypeName("IShader");
		address = "";
	}

	IShader::~IShader() {}

	void IShader::SetAddress(std::string address)
	{
		this->address = address;
		ResourceMgr->LoadEffect(this->address);
	}

	std::string IShader::GetAddress()
	{
		return address;
	}

	void IShader::LoadContent()
	{
		ResourceMgr->LoadEffect(address);
	}

	LPD3DXEFFECT IShader::GetShader()
	{
		return ResourceMgr->LoadEffect(address);
	}
}