#pragma once
#include "designer/editor/stdafx.h"
#include "third_party/quickjs/qjs.h"
#include "third_party/quickjs/weak_ptr.h"

class JsWindow : public nim_comp::WindowEx, public qjs::WeakObject<JsWindow>
{
public:
	JsWindow(qjs::Context* context, qjs::Value& this_obj);
	void InitWindow() override;
	static void RegisterWindow(qjs::Module* module);
	std::wstring GetSkinFile() override {
		return layout_;
	}
	std::wstring GetSkinFolder() override {
		return L"layout";
	}
	std::wstring GetWindowClassName() const override {
		return window_class_name_;
	}
	std::wstring GetWindowId() const override {
		return window_class_name_;
	}
	void SetLayout(const std::wstring & layout) {
		layout_ = layout;
	}
	void SetWindowClassName(const std::wstring & name) {
		window_class_name_ = name;
	}
private:
	std::wstring window_class_name_;
	std::wstring layout_;
	qjs::Context* context_;
	qjs::Value this_;
};
