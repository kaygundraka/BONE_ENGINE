#pragma once
#include "Common.h"
#include "etuForm.h"

namespace BONE_GRAPHICS
{
	class etuComboBox : public etuForm
	{
	private:
		string m_sName;
		string m_sSelectItem;
		bool Is_SelectStatus;

		int m_rItemHeight;
		RECT m_rClicked;
		RECT m_rItem;
		RECT m_rSelectItem;
		RECT m_rItemLast;

		vector<string> m_pItems;

	public:
		bool SetInformaition(string _Name, D3DXVECTOR3 _vec3Position);

		bool AddItems(string _String);
		bool DeleteItems(string _String);

		string GetSelectItems();

		virtual bool IsClicked() override;
		virtual void Render() override;
	};
}