#include "Common.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "ConfigManager.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	void ResourceManager::InitializeMembers()
	{
		LogMgr->Info("ResourceManager is initialized");
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

		LogMgr->Info("ResourceManager Manager is Clear.");
	}

    bool ResourceManager::ExistFile(std::string name, std::string* full_path)
    {
        char path[MAX_PATH];

        int path_length = 0;
        std::vector<std::string> paths = ConfigMgr->GetPaths(".\\info", "ResourcePaths");

        for (int i = 0; i < paths.size(); i++)
        {
            WIN32_FIND_DATA FindData;
            HANDLE FindHandle;

            strcpy_s(path, paths[i].c_str()); //".\\Resource\\b\\*"
            
            FindHandle = FindFirstFile((LPCSTR)path, &FindData);

            if (FindHandle == INVALID_HANDLE_VALUE)
            {
                FindClose(FindHandle);
                return false;
            }
            do {
                if (!strcmp(FindData.cFileName, name.c_str()))
                {
                    paths[i].pop_back();
                    *full_path = paths[i] + name;

                    FindClose(FindHandle);
                    return true;
                }
            } while (FindNextFile(FindHandle, &FindData));

            FindClose(FindHandle);
        }

        *full_path == "";

        return false;
    }

    std::vector<std::string> ResourceManager::ExistFiles(std::string path)
    {
        char Path[MAX_PATH];
        strcpy(Path, path.c_str());

        std::vector<std::string> resultFiles;

        WIN32_FIND_DATA FindData;
        HANDLE FindHandle;

        FindHandle = FindFirstFile((LPCSTR)Path, &FindData);

        if (FindHandle == INVALID_HANDLE_VALUE)
            return resultFiles;
        do {
            if (!strcmp(FindData.cFileName, ".") || !strcmp(FindData.cFileName, ".."))
                continue;

            resultFiles.push_back(FindData.cFileName);

        } while (FindNextFile(FindHandle, &FindData));

        FindClose(FindHandle);

        return resultFiles;
    }

	MESH_INFO* ResourceManager::LoadMesh(std::string name)
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
			
            std::vector<DWORD> faces(Temp.mesh->GetNumFaces() * 3);
            Temp.mesh->GenerateAdjacency(0.0f, &faces[0]); // 읽어온다

            Temp.mesh->OptimizeInplace(
                D3DXMESHOPT_ATTRSORT | // 속성으로 삼각형을 정렬하고 속성 테이블을 생성한다.
                                       // 이것은 DrawSubset의 효율을 높여준다.
                D3DXMESHOPT_COMPACT | // 메쉬에서 이용되지 은 인덱스와 버텍스를 제거한다.
                D3DXMESHOPT_VERTEXCACHE, // 버텍스 캐시의 히트율을 높인다.
                &faces[0], // 최적화 되지 않은 메쉬의 인접 배열
                0, // 최적화된 메쉬의 인접배열 정보를 받을 것인가?
                0, // 면 리맵정보로 채워질 DWORD 배열로의 포인터 정보
                   // 면 리맵정보는 원래의 면이 어느 곳으로 이동했는지를 알려주는것
                0); // 버텍스 리맵정보

			staticMeshList.insert(std::pair<std::string, MESH_INFO>(name, Temp));
			
			char log[MAX_PATH] = "";
			strcpy_s(log, name.c_str());
			strcat_s(log, "- is loaded.");
			LogMgr->Info(log);

			return &(staticMeshList.find(name)->second);
		}
		else
			return &(staticMeshList.find(name)->second);
	}

	MESH_INFO* ResourceManager::FindMesh(std::string name)
	{
		ThreadSync sync;

        if (staticMeshList.find(name) != staticMeshList.end())
            return &(staticMeshList.find(name)->second);
        else
        {
            return LoadMesh(name);
        }

	}

	LPDIRECT3DTEXTURE9 ResourceManager::LoadTexture(std::string name)
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
			
			textureList.insert(std::pair<std::string, LPDIRECT3DTEXTURE9>(name, Temp));

			char log[MAX_PATH] = "";
			strcpy_s(log, name.c_str());
			strcat_s(log, "- is loaded.");
			LogMgr->Info(log);

			return textureList.find(name)->second;
		}
		else
		{
			return textureList.find(name)->second;
		}
	}

	LPD3DXEFFECT ResourceManager::LoadEffect(std::string name)
	{
		ThreadSync sync;

		if (shaderList.find(name) == shaderList.end())
		{
			LPD3DXEFFECT ret = NULL;
			LPD3DXBUFFER pError = NULL;
			int dwShaderFlags = 0;

			dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_NO_PRESHADER;

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

			shaderList.insert(std::pair<std::string, LPD3DXEFFECT>(name, ret));

			return shaderList.find(name)->second;
		}
		else
			return shaderList.find(name)->second;
    }

	void ResourceManager::ReleaseMesh(std::string name)
	{
		ThreadSync sync;

		if (staticMeshList.find(name) != staticMeshList.end())
		{
            auto info = staticMeshList.find(name)->second;
            info.mesh->Release();
            info.buffer->Release();
            staticMeshList.erase(staticMeshList.find(name));
		}
	}
	
	void ResourceManager::ReleaseTexture(std::string name)
	{
		ThreadSync sync;

		if (textureList.find(name) != textureList.end())
		{
            textureList.find(name)->second->Release();
			textureList.erase(textureList.find(name));
		}
	}

	void ResourceManager::ReleaseEffect(std::string name)
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