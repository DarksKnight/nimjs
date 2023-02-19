#pragma once

class EditorForm : public nim_comp::WindowEx
{
public:
	EditorForm() {}
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual void InitWindow() override;
	static const LPCTSTR kClassName;
};