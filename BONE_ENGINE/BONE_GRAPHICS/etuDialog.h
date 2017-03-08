#pragma once
#include "Common.h"
#include "etuMessageHandler.h"
#include "etuForm.h"
#include "etuTextBox.h"
#include "etuButton.h"
#include "etuCheckBox.h"
#include "etuImage.h"
#include "etuComboBox.h"
#include "InputManager.h"
#include "etuStaticText.h"
#include "etuInputText.h"
#include <map>

namespace BONE_GRAPHICS
{
	template <typename T>
	class etuDialog : public etuForm
	{
	private:
		std::string m_sWindowName;
		map<std::string, etuTextBox> m_mTextBoxMap;
		map<std::string, etuButton> m_mButtonMap;
		map<std::string, etuCheckBox> m_mCheckBoxMap;
		map<std::string, etuImage> m_mImageMap;
		map<std::string, etuComboBox> m_mComboBoxMap;
		map<std::string, etuStaticText> m_mStaticTextMap;
		map<std::string, etuInputText> m_mInputTextMap;
		etuMessageHandler<T> *MessageHandler;

		RECT m_rWindowBarRect;
		RECT m_rFrameRect;

	public:
		bool SetInformaition(std::string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, RECT* _Rect = nullptr, RECT* _Margin = nullptr);

		void SetMessageHandler(etuMessageHandler<T> *MessageHandler)
		{
			this->MessageHandler = MessageHandler;
			this->MessageHandler->AddMessage(etuUIMessage(this, MS_CREATE));
		}

		virtual void SetPosition(D3DXVECTOR3& _vec3Position);

		bool AddTextBox(std::string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, std::string _Text, RECT* _Rect = nullptr, RECT* _Margin = nullptr);
		bool AddButton(std::string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, std::string _Text, RECT* _Rect = nullptr, RECT* _Margin = nullptr);
		bool AddCheckBox(std::string _Name, D3DXVECTOR3 _vec3Position, std::string _Text, RECT* _Rect = nullptr, RECT* _Margin = nullptr);
		bool AddImage(std::string _Name, std::string _ImageAddress, D3DXVECTOR3 _vec3Position, int _Width, int _Height, RECT* _Rect, RECT* _Margin = nullptr);
		bool AddComboBox(std::string _Name, D3DXVECTOR3 _vec3Position);
		bool AddStaticText(std::string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, std::string _Text);
		bool AddInputText(std::string _Name, D3DXVECTOR3 _vec3Position, int _Width, std::string _Text);

		auto GetWindow(std::string _Name);
		etuComboBox* GetComboBox(std::string _Name);
		etuInputText* GetInputText(std::string _Name);
		etuCheckBox* GetCheckBox(std::string _Name);
		virtual void Render() override;

		void Update(float _fTimeDelta);

		virtual void Release();
	};

	template <typename T>
	auto etuDialog<T>::GetWindow(std::string _Name)
	{
		auto CheckBox = &(m_mCheckBoxMap.find(p_sName)->second);
		if (CheckBox != nullptr)
			return CheckBox;

		auto ScreenImage = &(m_mImage.find(p_sName)->second);
		if (ScreenImage != nullptr)
			return ScreenImage;

		auto TextBox = &(m_mTextBoxMap.find(p_sName)->second);
		if (TextBox != nullptr)
			return TextBox;

		auto Button = &(m_mButtonMap.find(p_sName)->second);
		if (Button != nullptr)
			return Button;

		auto ComboBox = &(m_mComboBoxMap.find(p_sName)->second);
		if (ComboBox != nullptr)
			return ComboBox;

		return nullptr;
	}

	template <typename T>
	etuInputText* etuDialog<T>::GetInputText(std::string _Name)
	{
		etuInputText* InputText = &(m_mInputTextMap.find(p_sName)->second);
		if (InputText != nullptr)
			return InputText;

		return nullptr;
	}

	template <typename T>
	etuCheckBox* etuDialog<T>::GetCheckBox(std::string _Name)
	{
		etuCheckBox* CheckBox = &(m_mCheckBoxMap.find(p_sName)->second);
		if (CheckBox != nullptr)
			return CheckBox;

		return nullptr;
	}

	template <typename T>
	etuComboBox* etuDialog<T>::GetComboBox(std::string _Name)
	{
		etuComboBox* ComboBox = &(m_mComboBoxMap.find(p_sName)->second);
		if (ComboBox != nullptr)
			return ComboBox;

		return nullptr;
	}

	template <typename T>
	bool etuDialog<T>::SetInformaition(std::string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, RECT* _Rect, RECT* _Margin)
	{
		m_sWindowName = _Name;

		if (p_pRect == nullptr)
		{
			m_rImageRect = { 495, 933, 817, 1156, };
			m_rWindowBarRect = { 495, 933, 817, 979, };
			m_rFrameRect = { 495, 979, 817, 1156, };
		}
		else
			m_rImageRect = *p_pRect;

		if (!BasicSetInformation(p_sName, _vec3Position, _Width, _Height, _Margin))
			return false;

		m_rMargin.top += m_rWindowBarRect.bottom - m_rWindowBarRect.top;

		Is_Show = true;

		return true;
	}

	template <typename T>
	void etuDialog<T>::SetPosition(D3DXVECTOR3& _vec3Position)
	{
		m_vec3Position = _vec3Position;

		auto ComboBoxIterator = m_mComboBoxMap.begin();
		for (int i = 0; i < m_mComboBoxMap.size(); i++)
		{
			ComboBoxIterator->second.SetParentPosition(m_vec3Position);

			ComboBoxIterator++;
		}

		auto TextBoxIterator = m_mTextBoxMap.begin();
		for (int i = 0; i < m_mTextBoxMap.size(); i++)
		{
			TextBoxIterator->second.SetParentPosition(m_vec3Position);

			TextBoxIterator++;
		}

		auto ButtonIterator = m_mButtonMap.begin();
		for (int i = 0; i < m_mButtonMap.size(); i++)
		{
			ButtonIterator->second.SetParentPosition(m_vec3Position);

			ButtonIterator++;
		}

		auto CheckBoxIterator = m_mCheckBoxMap.begin();
		for (int i = 0; i < m_mCheckBoxMap.size(); i++)
		{
			CheckBoxIterator->second.SetParentPosition(m_vec3Position);

			CheckBoxIterator++;
		}

		auto ImageIterator = m_mImageMap.begin();
		for (int i = 0; i < m_mImageMap.size(); i++)
		{
			ImageIterator->second.SetParentPosition(m_vec3Position);

			ImageIterator++;
		}

		auto StaticTextIterator = m_mStaticTextMap.begin();
		for (int i = 0; i < m_mStaticTextMap.size(); i++)
		{
			StaticTextIterator->second.SetParentPosition(m_vec3Position);

			StaticTextIterator++;
		}

		auto InputTextIterator = m_mInputTextMap.begin();
		for (int i = 0; i < m_mInputTextMap.size(); i++)
		{
			InputTextIterator->second.SetParentPosition(m_vec3Position);

			InputTextIterator++;
		}
	}

	template <typename T>
	void etuDialog<T>::Update(float _fTimeDelta)
	{
		if (Is_Show == true)
		{
			if (this->etuForm::IsClicked() && MessageHandler != nullptr &&
				InputMgr->GetMousePosition().y <= m_vec3ParentPosition.y + m_vec3Position.y + (m_rWindowBarRect.bottom - m_rWindowBarRect.top))
			{
				MessageHandler->AddMessage(etuUIMessage(this, MS_LBDOWN));
			}
			else if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBUP)
			{
				MessageHandler->AddMessage(etuUIMessage(this, MS_LBUP));
			}

			auto ComboBoxIterator = m_mComboBoxMap.begin();
			for (int i = 0; i < m_mComboBoxMap.size(); i++)
			{
				if (ComboBoxIterator->second.IsClicked() && MessageHandler != nullptr)
					MessageHandler->AddMessage(etuUIMessage(&(ComboBoxIterator->second), MS_LBDOWN));
				else if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBUP)
					MessageHandler->AddMessage(etuUIMessage(&(ComboBoxIterator->second), MS_LBUP));

				ComboBoxIterator++;
			}

			auto CheckBoxIterator = m_mCheckBoxMap.begin();
			for (int i = 0; i < m_mCheckBoxMap.size(); i++)
			{
				if (CheckBoxIterator->second.IsClicked() && MessageHandler != nullptr)
					MessageHandler->AddMessage(etuUIMessage(&(CheckBoxIterator->second), MS_LBDOWN));
				else if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBUP)
					MessageHandler->AddMessage(etuUIMessage(&(CheckBoxIterator->second), MS_LBUP));

				CheckBoxIterator++;
			}

			auto ImageIterator = m_mImageMap.begin();
			for (int i = 0; i < m_mImageMap.size(); i++)
			{
				if (ImageIterator->second.IsClicked() && MessageHandler != nullptr)
					MessageHandler->AddMessage(etuUIMessage(&(ImageIterator->second), MS_LBDOWN));
				else if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBUP)
					MessageHandler->AddMessage(etuUIMessage(&(ImageIterator->second), MS_LBUP));

				ImageIterator++;
			}

			auto TextBoxIterator = m_mTextBoxMap.begin();
			for (int i = 0; i < m_mTextBoxMap.size(); i++)
			{

				if (TextBoxIterator->second.IsClicked() && MessageHandler != nullptr)
					MessageHandler->AddMessage(etuUIMessage(&(TextBoxIterator->second), MS_LBDOWN));
				else if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBUP)
					MessageHandler->AddMessage(etuUIMessage(&(TextBoxIterator->second), MS_LBUP));

				TextBoxIterator++;
			}

			auto ButtonIterator = m_mButtonMap.begin();
			for (int i = 0; i < m_mButtonMap.size(); i++)
			{
				if (ButtonIterator->second.IsClicked() && MessageHandler != nullptr)
					MessageHandler->AddMessage(etuUIMessage(&(ButtonIterator->second), MS_LBDOWN));
				else if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBUP)
					MessageHandler->AddMessage(etuUIMessage(&(ButtonIterator->second), MS_LBUP));

				ButtonIterator++;
			}

			auto InputTextIterator = m_mInputTextMap.begin();
			for (int i = 0; i < m_mInputTextMap.size(); i++)
			{
				if (InputTextIterator->second.IsClicked() && MessageHandler != nullptr)
					MessageHandler->AddMessage(etuUIMessage(&(InputTextIterator->second), MS_LBDOWN));
				else if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBUP)
					MessageHandler->AddMessage(etuUIMessage(&(InputTextIterator->second), MS_LBUP));

				InputTextIterator->second.Update(p_fTimeDelta);
				InputTextIterator++;
			}
			
			MessageHandler->ProcessMessage();
		}
	}

	template <typename T>
	void etuDialog<T>::Render()
	{
		if (Is_Show)
		{
			m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
			D3DXMatrixIdentity(&m_TransformMatrix);
			D3DXMatrixTransformation2D(&m_TransformMatrix, &D3DXVECTOR2(m_vec3Position.x, m_vec3Position.y),
				nullptr, &D3DXVECTOR2(m_fScaleX, 1), nullptr, 0.0f, &D3DXVECTOR2(0, 0));
			m_pSprite->SetTransform(&m_TransformMatrix);
			m_pSprite->Draw(m_pTextures, &m_rWindowBarRect, nullptr, &m_vec3Position, 0xffffffff);


			D3DXMatrixIdentity(&m_TransformMatrix);
			D3DXMatrixTransformation2D(&m_TransformMatrix, &D3DXVECTOR2(m_vec3Position.x, m_vec3Position.y + (m_rWindowBarRect.bottom - m_rWindowBarRect.top)),
				nullptr, &D3DXVECTOR2(m_fScaleX, m_fScaleY), nullptr, 0.0f, &D3DXVECTOR2(0, 0));
			m_pSprite->SetTransform(&m_TransformMatrix);
			m_pSprite->Draw(m_pTextures, &m_rFrameRect, nullptr,
				&D3DXVECTOR3(m_vec3Position.x, m_vec3Position.y + (m_rWindowBarRect.bottom - m_rWindowBarRect.top),
					m_vec3Position.z), 0xffffffff);

			m_pSprite->End();

			static SIZE size;

			GetTextExtentPoint32(m_pFont->GetDC(), m_sWindowName.c_str(), m_sWindowName.length(), &size);

			RECT titlePosition = {
				m_vec3Position.x + m_iWidth * m_fScaleX / 2 - size.cx / 2,
				m_vec3Position.y + size.cy ,
				m_vec3Position.x + m_iWidth * m_fScaleX,
				m_vec3Position.y + size.cy * 2 ,
			};

			m_pFont->DrawText(nullptr, m_sWindowName.c_str(), -1, &titlePosition, 0, 0xFFFFFFFF);

			auto TextBoxIterator = m_mTextBoxMap.begin();
			for (int i = 0; i < m_mTextBoxMap.size(); i++)
			{
				TextBoxIterator->second.Render();
				TextBoxIterator++;
			}

			auto ButtonIterator = m_mButtonMap.begin();
			for (int i = 0; i < m_mButtonMap.size(); i++)
			{
				ButtonIterator->second.Render();
				ButtonIterator++;
			}

			auto ImageIterator = m_mImageMap.begin();
			for (int i = 0; i < m_mImageMap.size(); i++)
			{
				ImageIterator->second.Render();
				ImageIterator++;
			}
			
			auto CheckBoxIterator = m_mCheckBoxMap.begin();
			for (int i = 0; i < m_mCheckBoxMap.size(); i++)
			{
				CheckBoxIterator->second.Render();
				CheckBoxIterator++;
			}

			auto ComboBoxIterator = m_mComboBoxMap.begin();
			for (int i = 0; i < m_mComboBoxMap.size(); i++)
			{
				ComboBoxIterator->second.Render();
				ComboBoxIterator++;
			}

			auto StaticTextIterator = m_mStaticTextMap.begin();
			for (int i = 0; i < m_mStaticTextMap.size(); i++)
			{
				StaticTextIterator->second.Render();
				StaticTextIterator++;
			}

			auto InputTextIterator = m_mInputTextMap.begin();
			for (int i = 0; i < m_mInputTextMap.size(); i++)
			{
				InputTextIterator->second.Render();
				InputTextIterator++;
			}
		}
	}

	template <typename T>
	bool etuDialog<T>::AddButton(std::string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, std::string _Text, RECT* _Rect, RECT* _Margin)
	{
		if (0 != m_mButtonMap.count(p_sName))
			return false;

		etuButton Temp;

		p_vec3Position.x += m_rMargin.left;
		p_vec3Position.y += m_rMargin.top;

		if (p_vec3Position.x > m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right)
			p_vec3Position.x = m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right;

		if (p_vec3Position.y > m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom)
			p_vec3Position.y = m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom;

		if (p_vec3Position.x + _Width > m_iWidth * m_fScaleX - m_rMargin.right)
			p_iWidth = m_iWidth * m_fScaleX - m_rMargin.right - _vec3Position.x - m_rMargin.left;

		if (p_vec3Position.y + _Height >  m_iHeight * m_fScaleY - m_rMargin.bottom)
			p_iHeight = m_iHeight * m_fScaleY - m_rMargin.bottom - _vec3Position.y - m_rMargin.top;

		Temp.SetInformaition(p_sName, _vec3Position, _Width, _Height, _Text, _Rect, _Margin);
		Temp.SetParentPosition(m_vec3Position);

		m_mButtonMap[p_sName] = Temp;

		MessageHandler->AddMessage(etuUIMessage((&m_mButtonMap[p_sName]), MS_CREATE));

		return true;
	}

	template <typename T>
	bool etuDialog<T>::AddTextBox(std::string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, std::string _Text, RECT* _Rect, RECT* _Margin)
	{
		if (0 != m_mTextBoxMap.count(p_sName))
			return false;

		etuTextBox Temp;

		p_vec3Position.x += m_rMargin.left;
		p_vec3Position.y += m_rMargin.top;

		if (p_vec3Position.x > m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right)
			p_vec3Position.x = m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right;

		if (p_vec3Position.y > m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom)
			p_vec3Position.y = m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom;

		if (p_vec3Position.x + _Width > m_iWidth * m_fScaleX - m_rMargin.right)
			p_iWidth = m_iWidth * m_fScaleX - m_rMargin.right - _vec3Position.x - m_rMargin.left;

		if (p_vec3Position.y + _Height >  m_iHeight * m_fScaleY - m_rMargin.bottom)
			p_iHeight = m_iHeight * m_fScaleY - m_rMargin.bottom - _vec3Position.y - m_rMargin.top;

		Temp.SetInformaition(p_sName, _vec3Position, _Width, _Height, _Text, _Rect, _Margin);
		Temp.SetParentPosition(m_vec3Position);

		m_mTextBoxMap[p_sName] = Temp;

		MessageHandler->AddMessage(etuUIMessage((&m_mTextBoxMap[p_sName]), MS_CREATE));

		return true;
	}

	template <typename T>
	bool etuDialog<T>::AddCheckBox(std::string _Name, D3DXVECTOR3 _vec3Position, std::string _Text, RECT* _Rect = nullptr, RECT* _Margin = nullptr)
	{
		if (0 != m_mCheckBoxMap.count(p_sName))
			return false;

		etuCheckBox Temp;

		p_vec3Position.x += m_rMargin.left;
		p_vec3Position.y += m_rMargin.top;

		if (p_vec3Position.x > m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right)
			p_vec3Position.x = m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right;

		if (p_vec3Position.y > m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom)
			p_vec3Position.y = m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom;

		Temp.SetInformaition(p_sName, _vec3Position, _Text, _Rect, _Margin);
		Temp.SetParentPosition(m_vec3Position);

		m_mCheckBoxMap[p_sName] = Temp;

		MessageHandler->AddMessage(etuUIMessage((&m_mCheckBoxMap[p_sName]), MS_CREATE));
	}

	template <typename T>
	bool etuDialog<T>::AddImage(std::string _Name, std::string _ImageAddress, D3DXVECTOR3 _vec3Position, int _Width, int _Height, RECT* _Rect, RECT* _Margin = nullptr)
	{
		if (0 != m_mImageMap.count(p_sName))
			return false;

		etuImage Temp;

		p_vec3Position.x += m_rMargin.left;
		p_vec3Position.y += m_rMargin.top;

		if (p_vec3Position.x > m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right)
			p_vec3Position.x = m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right;

		if (p_vec3Position.y > m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom)
			p_vec3Position.y = m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom;

		if (p_vec3Position.x + _Width > m_iWidth * m_fScaleX - m_rMargin.right)
			p_iWidth = m_iWidth * m_fScaleX - m_rMargin.right - _vec3Position.x - m_rMargin.left;

		if (p_vec3Position.y + _Height >  m_iHeight * m_fScaleY - m_rMargin.bottom)
			p_iHeight = m_iHeight * m_fScaleY - m_rMargin.bottom - _vec3Position.y - m_rMargin.top;

		Temp.SetInformaition(p_sName, _ImageAddress, _vec3Position, _Width, _Height, _Text, _Rect, _Margin);
		Temp.SetParentPosition(m_vec3Position);

		m_mImageMap[p_sName] = Temp;

		MessageHandler->AddMessage(etuUIMessage((&m_mImageMap[p_sName]), MS_CREATE));
	}

	template <typename T>
	bool etuDialog<T>::AddComboBox(std::string _Name, D3DXVECTOR3 _vec3Position)
	{
		if (0 != m_mComboBoxMap.count(p_sName))
			return false;

		etuComboBox Temp;

		p_vec3Position.x += m_rMargin.left;
		p_vec3Position.y += m_rMargin.top;

		if (p_vec3Position.x > m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right)
			p_vec3Position.x = m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right;

		if (p_vec3Position.y > m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom)
			p_vec3Position.y = m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom;

		Temp.SetInformaition(p_sName, _vec3Position);
		Temp.SetParentPosition(m_vec3Position);

		m_mComboBoxMap[p_sName] = Temp;

		MessageHandler->AddMessage(etuUIMessage((&m_mComboBoxMap[p_sName]), MS_CREATE));
	}

	template <typename T>
	bool etuDialog<T>::AddStaticText(std::string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, std::string _Text)
	{
		if (0 != m_mStaticTextMap.count(p_sName))
			return false;

		etuStaticText Temp;

		p_vec3Position.x += m_rMargin.left;
		p_vec3Position.y += m_rMargin.top;

		if (p_vec3Position.x > m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right)
			p_vec3Position.x = m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right;

		if (p_vec3Position.y > m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom)
			p_vec3Position.y = m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom;

		Temp.SetInformaition(p_sName, _vec3Position, _Width, _Height, _Text);
		Temp.SetParentPosition(m_vec3Position);

		m_mStaticTextMap[p_sName] = Temp;

		MessageHandler->AddMessage(etuUIMessage((&m_mStaticTextMap[p_sName]), MS_CREATE));
	}

	template <typename T>
	bool etuDialog<T>::AddInputText(std::string _Name, D3DXVECTOR3 _vec3Position, int _Width, std::string _Text)
	{
		if (0 != m_mInputTextMap.count(p_sName))
			return false;

		etuInputText Temp;

		p_vec3Position.x += m_rMargin.left;
		p_vec3Position.y += m_rMargin.top;

		if (p_vec3Position.x > m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right)
			p_vec3Position.x = m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right;

		if (p_vec3Position.y > m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom)
			p_vec3Position.y = m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom;

		Temp.SetInformaition(p_sName, _vec3Position, _Width, _Text);
		Temp.SetParentPosition(m_vec3Position);

		m_mInputTextMap[p_sName] = Temp;

		MessageHandler->AddMessage(etuUIMessage((&m_mInputTextMap[p_sName]), MS_CREATE));
	}

	template <typename T>
	void etuDialog<T>::Release()
	{
		auto ComboBoxIterator = m_mComboBoxMap.begin();
		for (int i = 0; i < m_mComboBoxMap.size(); i++)
		{
			ComboBoxIterator->second.Release();
			ComboBoxIterator++;
		}

		auto CheckBoxIterator = m_mCheckBoxMap.begin();
		for (int i = 0; i < m_mCheckBoxMap.size(); i++)
		{
			CheckBoxIterator->second.Release();
			CheckBoxIterator++;
		}

		auto ImageIterator = m_mImageMap.begin();
		for (int i = 0; i < m_mImageMap.size(); i++)
		{
			ImageIterator->second.Release();
			ImageIterator++;
		}

		auto TextBoxIterator = m_mTextBoxMap.begin();
		for (int i = 0; i < m_mTextBoxMap.size(); i++)
		{
			TextBoxIterator->second.Release();
			TextBoxIterator++;
		}

		auto ButtonIterator = m_mButtonMap.begin();
		for (int i = 0; i < m_mButtonMap.size(); i++)
		{
			ButtonIterator->second.Release();
			ButtonIterator++;
		}

		auto StaticTextIterator = m_mStaticTextMap.begin();
		for (int i = 0; i < m_mStaticTextMap.size(); i++)
		{
			StaticTextIterator->second.Release();
			StaticTextIterator++;
		}

		auto InputTextIterator = m_mInputTextMap.begin();
		for (int i = 0; i < m_mInputTextMap.size(); i++)
		{
			InputTextIterator->second.Release();
			InputTextIterator++;
		}

		etuForm::Release();
	}
}