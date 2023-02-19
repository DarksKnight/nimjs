#include "designer/editor/stdafx.h"
#include "JsWindow.h"
#include "third_party/quickjs/qjs.h"
#include "third_party/quickjs/weak_class.h"
#include "designer/editor/form/EditorForm.h"

JsWindow::JsWindow(qjs::Context* context, qjs::Value& this_obj) :context_(context), this_(this_obj)
{
	qjs::Value mgr = qjs::WeakClass<WindowEx>::ToJs(*context, get_weak_ptr<WindowEx>());
	this_.SetProperty("manager", mgr);
}

void JsWindow::InitWindow()
{
	if (this_.HasProperty("initWindow")) {
		qjs::Value result = this_.Invoke("initWindow");
		if (result.IsException()) {
			context_->DumpError();
		}
	}
}

static qjs::Value createWindow(JsWindow* pThis, qjs::Context& context, qjs::ArgList& args) {
	const std::wstring layout = nbase::UTF8ToUTF16(args[0].ToStdString());
	const std::wstring name = nbase::UTF8ToUTF16(args[1].ToStdString());
	pThis->SetLayout(layout);
	pThis->SetWindowClassName(name);
	pThis->Create(nullptr, name.c_str(), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
	// window->CenterWindow();
	return qjs::undefined_value;
}

static qjs::WeakPtr<JsWindow> newWindow(qjs::Context& context, qjs::Value& this_obj, qjs::ArgList& args) {
	JsWindow* wnd = new JsWindow(&context, this_obj);
	return wnd->get_weak_ptr<JsWindow>();
}

static qjs::Value showWindow(JsWindow* pThis, qjs::Context& context, qjs::ArgList& args) {
	pThis->ShowWindow();
	return qjs::undefined_value;
}

static qjs::Value centerWindow(JsWindow* pThis, qjs::Context& context, qjs::ArgList& args) {
	pThis->CenterWindow();
	return qjs::undefined_value;
}

static qjs::Value findControl(JsWindow* pThis, qjs::Context& context, qjs::ArgList& args) {
	const std::wstring name = nbase::UTF8ToUTF16(args[0].ToStdString());
	const auto control = pThis->FindControl(name);
	return qjs::WeakClass<ui::Control>::ToJsById(context, control->get_weak_ptr<ui::Control>(), control->GetClassId());
}

static qjs::Value setText(ui::Label* pThis, qjs::Context& context, qjs::ArgList& args) {
	const std::wstring str = nbase::UTF8ToUTF16(args[0].ToStdString());
	pThis->SetText(str);
	return qjs::undefined_value;
}

template<class T>
static qjs::WeakPtr<T> createControl(qjs::Context& context, qjs::ArgList& args) {
	T* ctrl = new T();
	return ctrl->get_weak_ptr<T>();
}

void RegisterWindow(qjs::Module* module)
{
	auto window = module->ExportWeakClass<JsWindow>("Window");
	window.Init();
	window.AddCtor2<newWindow>();
	window.AddFunc<createWindow>("create");
	window.AddFunc<showWindow>("showWindow");
	window.AddFunc<centerWindow>("centerWindow");
	window.AddFunc<findControl>("findControl");
}

void RegisterControl(qjs::Module* module) 
{
	auto control = module->ExportWeakClass<ui::Control>("Control");
	control.Init();
	control.AddCtor<createControl>();
}

void RegisterLabel(qjs::Module* module)
{
	auto label = module->ExportWeakClass<ui::Label>("Label");
	label.Init(qjs::WeakClass<ui::Control>::class_id());
	label.AddCtor<createControl>();
	label.AddFunc<setText>("setText");
}