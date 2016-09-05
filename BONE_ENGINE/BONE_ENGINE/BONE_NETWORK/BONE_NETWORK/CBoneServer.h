#pragma once

namespace BONE_NETWORK
{
	class CBoneServer {
	private:
	public:
		CBoneServer();
		~CBoneServer();
		
		bool Init();
		bool Run();
	};
}