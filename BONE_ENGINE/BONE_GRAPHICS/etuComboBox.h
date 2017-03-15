#pragma once
#include "Common.h"
#include "etuForm.h"

namespace BONE_GRAPHICS
{
	class etuComboBox : public etuForm
	{
	private:
		std::string m_sName;
		std::string m_sSelectItem;
		bool Is_SelectStatus;

		int m_rItemHeight;
		RECT m_rClicked;
		RECT m_rItem;
		RECT m_rSelectItem;
		RECT m_rItemLast;

		vector<std::string> m_pItems;

	public:
		bool SetInformaition(std::string _Name, D3DXVECTOR3 _vec3Position);

		bool AddItems(std::string _String);
		bool DeleteItems(std::string _String);

		std::string GetSelectItems();

		virtual bool IsClicked() override;
		virtual void Render() override;
	};
}