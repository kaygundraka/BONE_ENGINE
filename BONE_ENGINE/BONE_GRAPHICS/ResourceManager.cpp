#include "Common.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "ConfigManager.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	void ResourceManager::InitializeMembers()
	{
		LogMgr->ShowMessage(LOG_MESSAGE, "ResourceManager is initialized");
	}

	void ResourceManager::ReleaseMembers()
	{
		ThreadSync sync;

		for (auto Iter = staticMeshList.begin(); Iter != staticMeshList.end();)
		{
			(*Iter).second.mesh->Release();
			(*Iter).second.buffer->Release();
			Iter = staticMeshList.erase(Iter);
		}
		
		for (auto Iter = textureList.begin(); Iter != textureList.end();)
		{
			(*Iter).second->Release();
			Iter = textureList.erase(Iter);
		}

		LogMgr->ShowMessage(LOG_MESSAGE, "ResourceManager Manager is Clear.");
	}

    bool ResourceManager::ExistFile(std::string name, std::string* full_path)
    {
        char path[MAX_PATH];

        int path_length = 0;
        std::vector<std::string> paths = ConfigMgr->GetPaths("info", "ResourcePaths");

        for (int i = 0; i < paths.size(); i++)
        {
            WIN32_FIND_DATA FindData;
            HANDLE FindHandle;

            strcpy_s(path, paths[i].c_str()); //".\\Resource\\b\\*"
            
            FindHandle = FindFirstFile((LPCSTR)path, &FindData);

            if (FindHandle == INVALID_HANDLE_VALUE)
                return false;
            do {
                if (!strcmp(FindData.cFileName, name.c_str()))
                {
                    paths[i].pop_back();
                    *full_path = paths[i] + name;

                    FindClose(FindHandle);
                    return true;
                }
            } while (FindNextFile(FindHandle, &FindData));
        }

        *full_path == "";

        return false;
    }

	MESH_INFO* ResourceManager::LoadMesh(string name)
	{
		ThreadSync sync;

		if (staticMeshList.find(name) == staticMeshList.end())
		{
			MESH_INFO Temp;
			
            std::string full_path = "";
            if (!ExistFile(name, &full_path))
                return nullptr;

			if (FAILED(D3DXLoadMeshFromX(full_path.c_str(), 0, RenderMgr->GetDevice(), 0, &Temp.buffer, 0, (DWORD*)&Temp.numMaterials, &Temp.mesh)))
				return nullptr;
			
			staticMeshList.insert(std::pair<string, MESH_INFO>(name, Temp));
			
			char log[MAX_PATH] = "";
			strcpy_s(log, name.c_str());
			strcat_s(log, "- is loaded.");
			LogMgr->ShowMessage(LOG_MESSAGE, log);

			return &(staticMeshList.find(name)->second);
		}
		else
			return &(staticMeshList.find(name)->second);
	}

	MESH_INFO* ResourceManager::FindMesh(string name)
	{
		ThreadSync sync;

        if (staticMeshList.find(name) != staticMeshList.end())
            return &(staticMeshList.find(name)->second);
        else
            return nullptr;
	}

	LPDIRECT3DTEXTURE9 ResourceManager::LoadTexture(string name)
	{
		ThreadSync sync;

		if (textureList.find(name) == textureList.end())
		{
			LPDIRECT3DTEXTURE9 Temp;

            std::string full_path = "";
            if (!ExistFile(name, &full_path))
                return nullptr;

            if (FAILED(D3DXCreateTextureFromFileEx(RenderMgr->GetDevice(), full_path.c_str(),
                D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                D3DX_DEFAULT, 0, 0, 0, 0, &Temp)))
                return nullptr;
			
			textureList.insert(std::pair<string, LPDIRECT3DTEXTURE9>(name, Temp));

			char log[MAX_PATH] = "";
			strcpy_s(log, name.c_str());
			strcat_s(log, "- is loaded.");
			LogMgr->ShowMessage(LOG_MESSAGE, log);

			return textureList.find(name)->second;
		}
		else
		{
			return textureList.find(name)->second;
		}
	}

	LPD3DXEFFECT ResourceManager::LoadEffect(string name)
	{
		ThreadSync sync;

		if (shaderList.find(name) == shaderList.end())
		{
			LPD3DXEFFECT ret = NULL;
			LPD3DXBUFFER pError = NULL;
			int dwShaderFlags = 0;

#if _DEBUG
			dwShaderFlags |= D3DXSHADER_DEBUG;
#endif
            std::string full_path = "";
            if (!ExistFile(name, &full_path))
                return nullptr;

			D3DXCreateEffectFromFile(RenderMgr->GetDevice(), full_path.c_str(),
				NULL, NULL, dwShaderFlags, NULL, &ret, &pError);

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

                return nullptr;
			}

			shaderList.insert(std::pair<string, LPD3DXEFFECT>(name, ret));

			return shaderList.find(name)->second;
		}
		else
			return shaderList.find(name)->second;
    }

	void ResourceManager::ReleaseMesh(string name)
	{
		ThreadSync sync;

		if (staticMeshList.find(name) != staticMeshList.end())
		{
			staticMeshList.erase(staticMeshList.find(name));
			staticMeshList.find(name)->second.mesh->Release();
			staticMeshList.find(name)->second.buffer->Release();
			staticMeshList.find(name)->second.buffer->Release();
		}
	}
	
	void ResourceManager::ReleaseTexture(string name)
	{
		ThreadSync sync;

		if (textureList.find(name) != textureList.end())
		{
			textureList.erase(textureList.find(name));
			textureList.find(name)->second->Release();
		}
	}

	void ResourceManager::ReleaseEffect(string name)
	{
		ThreadSync sync;
	}

	void ResourceManager::ReleaseMesh(MESH_INFO* meshInfo)
	{
		ThreadSync sync;

		for (auto Iter = staticMeshList.begin(); Iter != staticMeshList.end(); Iter++)
		{
			if ((*Iter).second.mesh == meshInfo->mesh)
			{
				staticMeshList.erase(Iter);
				meshInfo->mesh->Release();
				meshInfo->buffer->Release();
				break;
			}
		}
	}
	
	void ResourceManager::ReleaseTexture(LPDIRECT3DTEXTURE9 texture)
	{
		ThreadSync sync;
		
		for (auto Iter = textureList.begin(); Iter != textureList.end(); Iter++)
		{
			if ((*Iter).second == texture)
			{
				textureList.erase(Iter);
				texture->Release();
				break;
			}
		}
	}

	void ResourceManager::ReleaseEffect(LPD3DXEFFECT name)
	{
		ThreadSync sync;
	}
}