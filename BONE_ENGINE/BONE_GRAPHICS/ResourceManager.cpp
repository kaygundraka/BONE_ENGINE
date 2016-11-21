#include "Common.h"
#include "ResourceManager.h"
#include "LogManager.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	void ResourceManager::InitializeMembers()
	{
		CThreadSync sync;

		LogMgr->ShowMessage(LOG_MESSAGE, "ResourceManager is initialized");
	}

	void ResourceManager::ReleaseMembers()
	{
		CThreadSync sync;

		for (auto Iter = staticMeshList.begin(); Iter != staticMeshList.end();)
		{
			(*Iter).second._mesh->Release();
			(*Iter).second._buffer->Release();
			Iter = staticMeshList.erase(Iter);
		}
		
		for (auto Iter = textureList.begin(); Iter != textureList.end();)
		{
			(*Iter).second->Release();
			Iter = textureList.erase(Iter);
		}

		LogMgr->ShowMessage(LOG_MESSAGE, "ResourceManager Manager is Clear.");
	}

	MeshInfo* ResourceManager::LoadMesh(string _address)
	{
		CThreadSync sync;

		if (staticMeshList.find(_address) == staticMeshList.end())
		{
			MeshInfo Temp;
			
			if (FAILED(D3DXLoadMeshFromX(_address.c_str(), NULL, RenderMgr->GetDevice(), NULL,
				&Temp._buffer, NULL, &Temp._numMaterials, &Temp._mesh)))
			{
				return NULL;
			}

			staticMeshList.insert(std::pair<string, MeshInfo>(_address, Temp));
			
			char log[MAX_PATH] = "";
			strcpy(log, _address.c_str());
			strcat(log, "- is loaded.");
			LogMgr->ShowMessage(LOG_MESSAGE, log);

			return &(staticMeshList.find(_address)->second);
		}
		else
		{
			return &(staticMeshList.find(_address)->second);
		}
	}

	MeshInfo* ResourceManager::FindMesh(string _address)
	{
		CThreadSync sync;

		if (staticMeshList.find(_address) != staticMeshList.end())
		{
			return &(staticMeshList.find(_address)->second);
		}
		else
			return NULL;
	}

	LPDIRECT3DTEXTURE9 ResourceManager::LoadTexture(string _address)
	{
		CThreadSync sync;

		if (textureList.find(_address) == textureList.end())
		{
			LPDIRECT3DTEXTURE9 Temp;

			if (FAILED(D3DXCreateTextureFromFileEx(RenderMgr->GetDevice(), _address.c_str(),
				D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
				D3DX_DEFAULT, NULL, NULL, NULL, NULL, &Temp)))
			{
				return NULL;
			}

			textureList.insert(std::pair<string, LPDIRECT3DTEXTURE9>(_address, Temp));

			char log[MAX_PATH] = "";
			strcpy(log, _address.c_str());
			strcat(log, "- is loaded.");
			LogMgr->ShowMessage(LOG_MESSAGE, log);

			return textureList.find(_address)->second;
		}
		else
		{
			return textureList.find(_address)->second;
		}
	}

	LPD3DXEFFECT ResourceManager::LoadEffect(string _address)
	{
		CThreadSync sync;

		if (shaderList.find(_address) == shaderList.end())
		{
			LPD3DXEFFECT ret = NULL;
			LPD3DXBUFFER pError = NULL;
			DWORD dwShaderFlags = 0;

#if _DEBUG
			dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

			D3DXCreateEffectFromFile(RenderMgr->GetDevice(), _address.c_str(), 
				NULL, NULL, dwShaderFlags, NULL, &ret, &pError);

			// 셰이더 로딩에 실패한 경우 Output 창에 셰이더
			// 컴파일 에러를 출력한다.
			if (!ret && pError)
			{
				int size = pError->GetBufferSize();
				void *ack = pError->GetBufferPointer();

				if (ack)
				{
					char* str = new char[size];
					strcpy(str, (const char*)ack);
					OutputDebugString(str);
					delete[] str;
				}

				return NULL;
			}

			shaderList.insert(std::pair<string, LPD3DXEFFECT>(_address, ret));

			return shaderList.find(_address)->second;
		}
		else
		{
			return shaderList.find(_address)->second;
		}
	}

	void ResourceManager::ReleaseMesh(string _address)
	{
		CThreadSync sync;

		if (staticMeshList.find(_address) != staticMeshList.end())
		{
			staticMeshList.erase(staticMeshList.find(_address));
			staticMeshList.find(_address)->second._mesh->Release();
			staticMeshList.find(_address)->second._buffer->Release();
			staticMeshList.find(_address)->second._buffer->Release();
		}
	}
	
	void ResourceManager::ReleaseTexture(string _address)
	{
		CThreadSync sync;

		if (textureList.find(_address) != textureList.end())
		{
			textureList.erase(textureList.find(_address));
			textureList.find(_address)->second->Release();
		}
	}

	void ResourceManager::ReleaseEffect(string _address)
	{
		CThreadSync sync;
	}

	void ResourceManager::ReleaseMesh(MeshInfo* _meshInfo)
	{
		CThreadSync sync;

		for (auto Iter = staticMeshList.begin(); Iter != staticMeshList.end(); Iter++)
		{
			if ((*Iter).second._mesh == _meshInfo->_mesh)
			{
				staticMeshList.erase(Iter);
				_meshInfo->_mesh->Release();
				_meshInfo->_buffer->Release();
				break;
			}
		}
	}
	
	void ResourceManager::ReleaseTexture(LPDIRECT3DTEXTURE9 _texture)
	{
		CThreadSync sync;
		
		for (auto Iter = textureList.begin(); Iter != textureList.end(); Iter++)
		{
			if ((*Iter).second == _texture)
			{
				textureList.erase(Iter);
				_texture->Release();
				break;
			}
		}
	}

	void ResourceManager::ReleaseEffect(LPD3DXEFFECT _address)
	{
		CThreadSync sync;
	}
}