#include "JsEngine.h"

#include "JsWindow.h"
#include "designer/editor/stdafx.h"

static int has_suffix(const char* str, const char* suffix)
{
	size_t len = strlen(str);
	size_t slen = strlen(suffix);
	return (len >= slen && !memcmp(str + len - slen, suffix, slen));
}


typedef JSModuleDef* (JSInitModuleFunc)(JSContext* ctx,
	const char* module_name);

static JSModuleDef* js_module_loader_dll(JSContext* ctx,
	const char* module_name)
{
	JSModuleDef* m;
	HMODULE hd;
	JSInitModuleFunc* init;
	char* filename;

	if (!strchr(module_name, '/')) {
		/* must add a '/' so that the DLL is not searched in the
		   system library paths */
		filename = (char*)js_malloc(ctx, strlen(module_name) + 2 + 1);
		if (!filename)
			return NULL;
		strcpy(filename, "./");
		strcpy(filename + 2, module_name);
	}
	else {
		filename = (char*)module_name;
	}

	/* C module */
	hd = LoadLibraryA(filename);
	if (filename != module_name)
		js_free(ctx, filename);

	if (!hd) {
		JS_ThrowReferenceError(ctx, "could not load module filename '%s' as shared library",
			module_name);
		goto fail;
	}

	init = (JSInitModuleFunc*)GetProcAddress(hd, "js_init_module");
	if (!init) {
		JS_ThrowReferenceError(ctx, "could not load module filename '%s': js_init_module not found",
			module_name);
		goto fail;
	}

	m = init(ctx, module_name);
	if (!m) {
		JS_ThrowReferenceError(ctx, "could not load module filename '%s': initialization error",
			module_name);
	fail:
		if (hd)
			FreeLibrary(hd);
		return NULL;
	}
	return m;
}

BYTE* ReadFileToBuffer(const wchar_t* filename, size_t& size) {
	HANDLE hFile = ::CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return nullptr;
	DWORD dwSize = ::GetFileSize(hFile, NULL);
	if (dwSize == 0) return nullptr;
	if (dwSize > 4096 * 1024) return nullptr;

	DWORD dwRead = 0;
	BYTE* pByte = new BYTE[dwSize + 1];
	::ReadFile(hFile, pByte, dwSize, &dwRead, NULL);
	::CloseHandle(hFile);
	if (dwRead != dwSize) {
		delete[] pByte;
		pByte = nullptr;
	}

	pByte[dwSize] = '\0';
	size = dwSize;
	return pByte;
}

JSModuleDef* jsModuleLoader(JSContext* ctx,
	const char* module_name, void* opaque)
{
	JSModuleDef* m = nullptr;

	if (has_suffix(module_name, ".dll")) {
		m = js_module_loader_dll(ctx, module_name);
	}
	else {
		size_t buf_len;
		JSValue func_val;
		BYTE* buf;
		TCHAR errorMsg[64];
		// TCHAR* name = a2w(module_name, CP_UTF8);
		std::string fn = module_name;
		std::wstring theme_dir = nbase::win32::GetCurrentModuleDirectory();
		buf = ReadFileToBuffer((theme_dir + L"resources\\js\\skin\\" + nbase::UTF8ToUTF16(fn)).c_str(), buf_len);
		// delete[] name;
		if (!buf) {
			JS_ThrowReferenceError(ctx, "could not load module filename '%s'",
				module_name);
			return NULL;
		}
		
		if (has_suffix(module_name, "jsc")) {
			//º”‘ÿjs bytecode
		
			func_val = JS_ReadObject(ctx, buf, buf_len, JS_READ_OBJ_BYTECODE);
		
			delete[] buf;
		
			if (JS_IsException(func_val))
				return NULL;
			js_module_set_import_meta(ctx, func_val, TRUE, FALSE);
		}
		else {
			/* compile the module */
			func_val = JS_Eval(ctx, (char*)buf, buf_len, module_name,
				JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
		
			delete[] buf;
		
			if (JS_IsException(func_val))
				return NULL;
			/* XXX: could propagate the exception */
			js_module_set_import_meta(ctx, func_val, TRUE, FALSE);
		}
		
		/* the module is already referenced, so we must free it */
		m = (JSModuleDef*)JS_VALUE_GET_PTR(func_val);
		JS_FreeValue(ctx, func_val);
	}
	return m;
}

extern void RegisterWindow(qjs::Module* module);
extern void RegisterControl(qjs::Module* module);
extern void RegisterLabel(qjs::Module* module);

// static qjs::Value jsxFunc(qjs::Context& context, qjs::ArgList& args) {
// 	auto tag = args[0].ToString();
//
// 	const auto control = new ui::Control;
// 	return qjs::WeakClass<DuiLib::CControlUI>::ToJsById(ctx, control->get_weak_ptr<DuiLib::CControlUI>(), control->GetClassId());
// }

bool JsEngine::Init()
{
	runtime_ = new qjs::Runtime;
	context_ = new qjs::Context(runtime_);
	context_->SetUserData(this);
	JS_SetModuleLoaderFunc(runtime_->runtime(), NULL, jsModuleLoader, NULL);
	// context_->Global().SetProperty("JSX", context_->NewFunction<jsxFunc>("JSX"));
	auto module = context_->NewModule("DuiLib");
	RegisterWindow(module);
	RegisterControl(module);
	RegisterLabel(module);

	context_->SetLogFunc([this](const std::string& msg) {
		int a = 0;
		});
	return true;
}

bool JsEngine::Excute(const std::string& input, const std::string& fn)
{
	auto value = context_->Excute(input.c_str(), strlen(input.c_str()), fn.c_str());
	if (value.IsException()) {
		context_->DumpError();
		return false;
	}
	return true;
}

void JsEngine::RunLoop()
{
	MSG msg = { 0 };
	while (::GetMessage(&msg, NULL, 0, 0)) {
		::TranslateMessage(&msg);
		context_->ExecuteJobs();
	}
}