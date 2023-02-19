#pragma once
#include "third_party/quickjs/qjs.h"

class JsEngine
{
public:
	bool Init();
	bool Excute(const std::string & input, const std::string & fn);
	void RunLoop();
private:
	qjs::Runtime* runtime_;
	qjs::Context* context_;
};