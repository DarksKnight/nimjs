#include "../stdafx.h"
#include "EditorForm.h"

const LPCTSTR EditorForm::kClassName = L"EditorForm";

std::wstring EditorForm::GetSkinFolder()
{
	return L"layout";
}

std::wstring EditorForm::GetSkinFile()
{
	return L"layout_editor.xml";
}

std::wstring EditorForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring EditorForm::GetWindowId() const
{
	return kClassName;
}

void EditorForm::InitWindow()
{
}