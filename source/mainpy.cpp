// Static compilation definitions to avoid DLL dependencies
#ifdef _WIN32
#define UTF8PROC_STATIC
#define PCRE2_STATIC
#endif

#include <pybind11/pybind11.h>
#include "pybind11/include/pybind11/pybind11.h"
#include "pybind11/include/pybind11/eval.h"
#include "pybind11/include/pybind11/embed.h"

namespace py = pybind11;
using namespace py::literals;

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

#include <iostream>
#include <string>

#include "grapa/GrapaLink.h"
#include "grapa/GrapaValue.h"
#include "grapa/GrapaSystem.h"
#include "grapa/GrapaCompress.h"
#include "grapa/GrapaLibRule.h"
#include "grapa/GrapaFloat.h"
#include "grep/grapa_grep_unicode.hpp"

#define gGrapaUseWidget false

extern bool gGrapaWidgetMainThread;

class GrapaMainResponse : public GrapaConsoleResponse
{
public:
    virtual void SendCommand(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const void* sendbuf, u64 sendbuflen)
    {
    };
    virtual void SendPrompt(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, const GrapaBYTE& sendbuf)
    {
    };
    virtual void SendEnd(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pValue)
    {
    };
};

class GrapaPyObject
{
public:
	static GrapaRuleEvent* ToGrapa(PyObject* o, GrapaCHAR pname)
	{
		GrapaRuleEvent* result = NULL;
		GrapaCHAR pStr;
		if (PyBool_Check(o))
		{
			long retvalue = PyLong_AsLong(o);
			if (retvalue) pStr.FROM("\1");
			else pStr.FROM("\0");
			pStr.mToken = GrapaTokenType::BOOL;
			result = new GrapaRuleEvent(0, pname, pStr);
		}
		else if (Py_None==o)
		{
			result = new GrapaRuleEvent(0, pname, pStr);
			result->SetNull();
		}
		else if (PyBytes_Check(o))
		{
			const char* buffer = PyBytes_AS_STRING(o);
			Py_ssize_t length = PyBytes_GET_SIZE(o);
			pStr.FROM(buffer, length);
			pStr.mToken = GrapaTokenType::RAW;
			result = new GrapaRuleEvent(0, pname, pStr);
		}
		else if (PyUnicode_Check(o))
		{
			Py_ssize_t size=0;
			char* ptr = (char*)PyUnicode_AsUTF8AndSize(o, &size);
			result = new GrapaRuleEvent(0, pname, GrapaCHAR(ptr, size));
		}
		else if (PyLong_Check(o))
		{
			long long retvalue = PyLong_AsLongLong(o);
			pStr = GrapaInt(retvalue).getBytes();
			result = new GrapaRuleEvent(0, pname, pStr);
		}
		else if (PyFloat_Check(o))
		{
			double retvalue = PyFloat_AS_DOUBLE(o);
			pStr = GrapaFloat(retvalue).getBytes();
			result = new GrapaRuleEvent(0, pname, pStr);
		}
		else if (PyTuple_Check(o))
		{
			result = new GrapaRuleEvent(0, pname, GrapaCHAR());
			result->vQueue = new GrapaRuleQueue();
			result->mValue.mToken = GrapaTokenType::TUPLE;
			for (int i = 0; i < PyTuple_Size(o); ++i)
			{
				PyObject* item = PyTuple_GetItem(o, i);
				result->vQueue->PushTail(ToGrapa(item, GrapaCHAR()));
			}
		}
		else if (PyList_Check(o))
		{
			result = new GrapaRuleEvent(0, pname, GrapaCHAR());
			result->vQueue = new GrapaRuleQueue();
			result->mValue.mToken = GrapaTokenType::LIST;
			for (int i = 0; i < PyList_Size(o); ++i)
			{
				PyObject* item = PyList_GetItem(o, i);
				result->vQueue->PushTail(ToGrapa(item, GrapaCHAR()));
			}
		}
		else if (PyDict_Check(o))
		{
			result = new GrapaRuleEvent(0, pname, GrapaCHAR());
			result->vQueue = new GrapaRuleQueue();
			result->mValue.mToken = GrapaTokenType::GOBJ;
			PyObject* key, * item;
			Py_ssize_t pos = 0;
			while (PyDict_Next(o, &pos, &key, &item))
			{
				Py_ssize_t nsize;
				char* nptr = (char*)PyUnicode_AsUTF8AndSize(key, &nsize);
				GrapaCHAR kname(nptr, nsize);
				result->vQueue->PushTail(ToGrapa(item, kname));
			}
		}
		else
		{
			result = new GrapaRuleEvent(0, pname, GrapaCHAR());
			result->SetNull();
		}
		return result;
	}
	GrapaScriptExec mScriptExec;

	static void FromGrapa(GrapaScriptExec* vScriptExec, GrapaRuleEvent* e, py::object* o)
	{
		if (e == NULL)
		{
			*o = py::none();
			return;
		}
		GrapaInt a;
		GrapaFloat f;
		GrapaCHAR s;
		bool isTrueA = false;
		bool isNegA = false;
		bool isNull = false;
		Py_ssize_t pos = 0;
		switch (e->mValue.mToken)
		{
		case GrapaTokenType::PTR:
			FromGrapa(vScriptExec, e->vRulePointer, o);
			break;
		case GrapaTokenType::STR:
			*o = py::str((char*)e->mValue.mBytes, e->mValue.mLength);
			break;
		case GrapaTokenType::INT:
			a.FromBytes(e->mValue);
			*o = py::int_(a.LongValue());
			break;
		case GrapaTokenType::ID:
		case GrapaTokenType::SYSID:
			if (e->IsNull())
				*o = py::none();
			else
				*o = py::str((char*)e->mValue.mBytes, e->mValue.mLength);
			break;
		case GrapaTokenType::RAW:
			*o = py::bytes((char*)e->mValue.mBytes, e->mValue.mLength);
			break;
		case GrapaTokenType::FLOAT:
			f.FromBytes(e->mValue);
			s = f.ToString();
			*o = py::float_(std::stod((char*)s.mBytes));
			break;
		case GrapaTokenType::BOOL:
			if (e) isTrueA = !e->IsNullIsNegIsZero(isNegA, isNull);
			*o = py::bool_(isTrueA);
			break;
		case GrapaTokenType::LIST:
			*o = py::list();
			e = e->vQueue->Head();
			while (e)
			{
				GrapaRuleEvent* e2 = e;
				while (e2 && e2->mValue.mToken == GrapaTokenType::PTR) e2 = e2->vRulePointer;
				py::object o2;
				FromGrapa(vScriptExec, e2, &o2);
				((py::list*)o)->append(o2);
				e = e->Next();
			}
			break;
		case GrapaTokenType::TUPLE:
			*o = py::tuple(e->vQueue->mCount);
			e = e->vQueue->Head();
			while (e)
			{
				GrapaRuleEvent* e2 = e;
				while (e2 && e2->mValue.mToken == GrapaTokenType::PTR) e2 = e2->vRulePointer;
				py::object o2;
				FromGrapa(vScriptExec, e2, &o2);
				PyTuple_SET_ITEM(o->ptr(), pos, o2.ptr());
				o2.inc_ref();
				pos++;
				e = e->Next();
			}
			break;
		case GrapaTokenType::GOBJ:
			*o = py::dict();
			e = e->vQueue->Head();
			while (e)
			{
				GrapaRuleEvent* e2 = e;
				while (e2 && e2->mValue.mToken == GrapaTokenType::PTR) e2 = e2->vRulePointer;
				py::object o2;
				FromGrapa(vScriptExec, e2, &o2);
				py::str nm((char*)e2->mName.mBytes, e2->mName.mLength);
				PyDict_SetItem(o->ptr(), nm.ptr(), o2.ptr());
				e = e->Next();
			}
			break;
		case GrapaTokenType::ERR:
		case GrapaTokenType::INPUT:
		case GrapaTokenType::XML:
		case GrapaTokenType::SYM:
		case GrapaTokenType::SYSSYM:
		case GrapaTokenType::SYSSTR:
		case GrapaTokenType::SYSINT:
		case GrapaTokenType::TIME:
		case GrapaTokenType::EL:
		case GrapaTokenType::TAG:
		case GrapaTokenType::TABLE:
		case GrapaTokenType::OP:
		case GrapaTokenType::CODE:
		case GrapaTokenType::VECTOR:
		case GrapaTokenType::CLASS:
		case GrapaTokenType::OBJ:
		case GrapaTokenType::REF:
		case GrapaTokenType::RULE:
		case GrapaTokenType::TOKEN:
		case GrapaTokenType::WIDGET:
			if (e)
			{
				GrapaSystemSend send;
				send.isActive = false;
				if (e)
				{
					if (e->vQueue) vScriptExec->EchoList(&send, e, false, false, false);
					else vScriptExec->EchoValue(&send, e, false, false, false);
				}
				send.GetStr(s);
				*o = py::str((char*)s.mBytes, s.mLength);
			}
			break;
		default:
			*o = py::none();
			break;
		}
	}
};

class GrapaLibraryRulePyEvalEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePyEvalEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
	{
		pybind11::gil_scoped_acquire acquire;
		GrapaRuleEvent* result = NULL;
		GrapaLibraryParam script_param(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
		GrapaLibraryParam locals_param(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
		GrapaLibraryParam import_param(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
		GrapaLibraryParam attr_param(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
		if (script_param.vVal && script_param.vVal->mValue.mLength)
		{
			std::string sript_str;
			GrapaCHAR import_str("__main__"), attr_str("__dict__");
			sript_str.assign((char*)script_param.vVal->mValue.mBytes, script_param.vVal->mValue.mLength);
			if (import_param.vVal && import_param.vVal->mValue.mLength) import_str.FROM(import_param.vVal->mValue);
			if (attr_param.vVal && attr_param.vVal->mValue.mLength) attr_str.FROM(attr_param.vVal->mValue);
			py::object scope = py::module_::import((char*)import_str.mBytes).attr((char*)attr_str.mBytes);
			GrapaCHAR pStr;
			py::object locals;
			GrapaRuleEvent* cp = vScriptExec->CopyItem(locals_param.vVal);
			GrapaPyObject::FromGrapa(vScriptExec, cp, &locals);
			if (cp)
			{
				cp->CLEAR();
				delete cp;
			}
			//auto locals = py::dict("a"_a=5);
			py::object o = py::eval(sript_str, scope, locals);
			result = GrapaPyObject::ToGrapa(o.ptr(), GrapaCHAR());
		}
		return result;
	}
};

class GrapaLibraryRulePyExecEvent : public GrapaLibraryEvent
{
public:
	GrapaLibraryRulePyExecEvent(GrapaCHAR& pName) { mName.FROM(pName); };
	virtual GrapaRuleEvent* Run(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace, GrapaRuleEvent* pOperation, GrapaRuleQueue* pInput)
	{
		pybind11::gil_scoped_acquire acquire;
		GrapaRuleEvent* result = NULL;
		GrapaLibraryParam script_param(vScriptExec, pNameSpace, pInput ? pInput->Head(0) : NULL);
		GrapaLibraryParam locals_param(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
		GrapaLibraryParam import_param(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
		GrapaLibraryParam attr_param(vScriptExec, pNameSpace, pInput ? pInput->Head(3) : NULL);
		if (script_param.vVal && script_param.vVal->mValue.mLength)
		{
			std::string sript_str;
			sript_str.assign((char*)script_param.vVal->mValue.mBytes, script_param.vVal->mValue.mLength);
			GrapaCHAR import_str("__main__"), attr_str("__dict__");
			if (import_param.vVal && import_param.vVal->mValue.mLength) import_str.FROM(import_param.vVal->mValue);
			if (attr_param.vVal && attr_param.vVal->mValue.mLength) attr_str.FROM(attr_param.vVal->mValue);
			py::object scope = py::module_::import((char*)import_str.mBytes).attr((char*)attr_str.mBytes);
			py::object locals;
			GrapaRuleEvent* cp = vScriptExec->CopyItem(locals_param.vVal);
			GrapaPyObject::FromGrapa(vScriptExec, locals_param.vVal, &locals);
			if (cp)
			{
				cp->CLEAR();
				delete cp;
			}
			py::exec(sript_str, scope, locals);
		}
		return result;
	}
};

class GrapaPyRuleEvent : public GrapaLibraryRuleEvent
{
public:
	GrapaPyRuleEvent(GrapaCHAR pName) { mName.FROM(pName); };
	virtual GrapaLibraryEvent* LoadLib(GrapaScriptExec* vScriptExec, GrapaRuleEvent* pLib, GrapaCHAR& pName)
	{
		GrapaLibraryEvent* lib = NULL;
		if (pName.Cmp("eval") == 0) lib = new GrapaLibraryRulePyEvalEvent(pName);
		else if (pName.Cmp("exec") == 0) lib = new GrapaLibraryRulePyExecEvent(pName);
		return(lib);
	}
};

template <typename... Args>
using overload_cast_ = pybind11::detail::overload_cast_impl<Args...>;

/* https://pybind11.readthedocs.io/en/stable/classes.html
py::class_<Pet>(m, "Pet")
    .def("set", overload_cast_<int>()(&Pet::set), "Set the pet's age")
    .def("set", overload_cast_<const std::string &>()(&Pet::set), "Set the pet's name");
*/

class  GrapaStruct 
{
public:
    GrapaScriptExec mScriptExec;
    GrapaConsoleSend mConsoleSend;
    GrapaMainResponse mConsoleResponse;
    GrapaNames mRuleVariables;
	GrapaStruct(py::object cmdstr)
	{
		mConsoleSend.mScriptState.vScriptExec = &mScriptExec;
		mScriptExec.vScriptState = &mConsoleSend.mScriptState;
		mConsoleSend.mScriptState.SetNameSpace(&mRuleVariables);
		mRuleVariables.SetResponse(&mConsoleResponse);
		mConsoleSend.Start();
		GrapaCHAR runStr;
		if (cmdstr.ptr() == Py_None)
		{
			GrapaSystem* gSystem = GrapaLink::GetGrapaSystem();
			GrapaCHAR grresult;
			if (gSystem->mGrammar.mLength)
				grresult = mConsoleSend.SendSync(gSystem->mGrammar, NULL, 0, GrapaCHAR());
			GrapaCHAR configName(gSystem->mHomeDir);
			configName.Append("/.grapa/config");
			GrapaLink::RunFile(mConsoleSend,configName);
			GrapaCHAR runStr("$global['$py'] = class {eval = op(script,locals={},import='',attr=''){@<'py','eval',{@<var,{script}>,@<var,{locals}>,@<var,{import}>,@<var,{attr}>}>();};exec = op(script,locals={},import='',attr=''){@<'py','exec',{@<var,{script}>,@<var,{locals}>,@<var,{import}>,@<var,{attr}>}>();};};");
			grresult = mConsoleSend.SendSync(runStr, NULL, 0, GrapaCHAR());
		}
		else if (PyUnicode_Check(cmdstr.ptr()))
		{
			Py_ssize_t size = 0;
			const char* buffer = (char*)PyUnicode_AsUTF8AndSize(cmdstr.ptr(), &size);
			size_t length = size;
			if (length > 0)
			{
				runStr.FROM(buffer, length);
				GrapaCHAR grresult;
				grresult = mConsoleSend.SendSync(runStr, NULL, 0, GrapaCHAR());
			}
		}
		else if (PyBytes_Check(cmdstr.ptr()))
		{
			const char* buffer = PyBytes_AS_STRING(cmdstr.ptr());
			size_t length = PyBytes_GET_SIZE(cmdstr.ptr());
			if (length > 0)
			{
				runStr.FROM(buffer, length);
				runStr.mToken = GrapaTokenType::RAW;
				GrapaCHAR grresult;
				grresult = mConsoleSend.SendSync(runStr, NULL, 0, GrapaCHAR());
			}
		}

	}
    ~GrapaStruct() 
	{ 
		mConsoleSend.Stop();
	}

	py::object eval(py::object cmdstr, py::object paramstr, std::string rulestr, std::string profilestr)
	{
		size_t length = 0;
		GrapaCHAR runStr;
		GrapaRuleEvent* rulexx = NULL;
		GrapaCHAR profStr;
		py::object o;
		if (true)
		{
			pybind11::gil_scoped_acquire acquire;
			o = py::none();
			if (PyUnicode_Check(cmdstr.ptr()))
			{
				Py_ssize_t size = 0;
				const char* buffer = (char*)PyUnicode_AsUTF8AndSize(cmdstr.ptr(), &size);
				length = size;
				if (length > 0)
				{
					runStr.FROM(buffer, length);
				}
			}
			else if (PyBytes_Check(cmdstr.ptr()))
			{
				const char* buffer = PyBytes_AS_STRING(cmdstr.ptr());
				length = PyBytes_GET_SIZE(cmdstr.ptr());
				if (length > 0)
				{
					runStr.FROM(buffer, length);
					runStr.mToken = GrapaTokenType::RAW;
				}
			}
			if (rulestr.length() > 0)
			{
				GrapaCHAR rStr(rulestr.c_str(), rulestr.length());
				rulexx = mConsoleSend.mScriptState.SearchVariable(mConsoleSend.mScriptState.GetNameSpace(), rStr);
			}
			profStr.FROM(profilestr.c_str(), profilestr.length());
		}
		//if (GrapaRuleEvent* operation = mConsoleSend.mScriptState.AddRuleOperation(mConsoleSend.mScriptState.GetNameSpace()->GetNameQueue(), "", ""))
		//{
			//operation->mValue.mToken = GrapaTokenType::OBJ;
			GrapaRuleEvent* grresult = NULL;
			GrapaRuleEvent* vLocals = NULL;
			if (length > 0)
			{

				pybind11::gil_scoped_acquire acquire;
				GrapaRuleEvent* e = GrapaPyObject::ToGrapa(paramstr.ptr(), GrapaCHAR());
				if (e && e->vQueue)
				{
					vLocals = new GrapaRuleEvent();
					vLocals->mOpLocal = true;
					vLocals->mValue.mToken = GrapaTokenType::GOBJ;
					vLocals->vQueue = new GrapaRuleQueue();
					mConsoleSend.mScriptState.GetNameSpace()->GetNameQueue()->PushTail(vLocals);
					while (e->vQueue->Head())
						vLocals->vQueue->PushTail(e->vQueue->PopHead());
				}
				if (e)
				{
					e->CLEAR();
					delete e;
				}
			}

			if (length > 0)
			{
				grresult = mConsoleSend.SendSyncResult(runStr, rulexx, 0, profStr);
			}
			if (grresult)
			{
				pybind11::gil_scoped_acquire acquire;
				GrapaRuleEvent* echo = grresult;
				while (echo && echo->mValue.mToken == GrapaTokenType::PTR)
					echo = echo->vRulePointer;
				GrapaPyObject::FromGrapa(&mScriptExec, echo, &o);
				grresult->CLEAR();
				delete grresult;
			}
			if (vLocals)
			{
				mConsoleSend.mScriptState.GetNameSpace()->GetNameQueue()->PopEvent(vLocals);
				vLocals->CLEAR();
				delete vLocals;
			}
			//if (mConsoleSend.mScriptState.GetNameSpace()->GetNameQueue()->PopEvent(operation))
			//{
			//	operation->CLEAR();
			//	delete operation;
			//	operation = NULL;
			//}
		//}
		return o;
	}


	py::object compile(py::object cmdstr, std::string rulestr, std::string profilestr)
	{
		size_t length = 0;
		GrapaCHAR runStr;
		GrapaRuleEvent* rulexx = NULL;
		GrapaCHAR profStr;
		py::object o;
		if (true)
		{
			pybind11::gil_scoped_acquire acquire;
			o = py::none();
			Py_ssize_t size = 0;
			const char* buffer = (char*)PyUnicode_AsUTF8AndSize(cmdstr.ptr(), &size);
			length = size;
			if (length > 0)
			{
				runStr.FROM(buffer, length);
			}
			if (rulestr.length() > 0)
			{
				GrapaCHAR rStr(rulestr.c_str(), rulestr.length());
				rulexx = mConsoleSend.mScriptState.SearchVariable(mConsoleSend.mScriptState.GetNameSpace(), rStr);
			}
			profStr.FROM(profilestr.c_str(), profilestr.length());
		}
		GrapaRuleEvent* grresult = NULL;
		if (length > 0)
		{
			GrapaRuleEvent* opEvent = mScriptExec.Plan(mConsoleSend.mScriptState.GetNameSpace(), runStr, rulexx, 0, profStr);
			opEvent->mName.FROM("op");
			GrapaRuleEvent* verEvent = new GrapaRuleEvent(0, GrapaCHAR("version"), GrapaLink::GetGrapaSystem()->mVersion);

			GrapaRuleEvent* e = new GrapaRuleEvent();
			e->mValue.mToken = GrapaTokenType::GOBJ;
			e->vQueue = new GrapaRuleQueue();
			e->vQueue->PushTail(opEvent);
			e->vQueue->PushTail(verEvent);

			grresult = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
			GrapaCHAR s;
			e->TO(s);
			GrapaCompress::Compress(s, grresult->mValue);
			grresult->mValue.mToken = GrapaTokenType::RAW;

			e->CLEAR();
			delete e;
		}
		if (grresult)
		{
			pybind11::gil_scoped_acquire acquire;
			GrapaRuleEvent* echo = grresult;
			while (echo && echo->mValue.mToken == GrapaTokenType::PTR)
				echo = echo->vRulePointer;
			GrapaPyObject::FromGrapa(&mScriptExec, echo, &o);
			grresult->CLEAR();
			delete grresult;
		}

		return o;
	}

	py::object grep(py::object inputstr, py::object patternstr, std::string optionsstr, std::string delimstr, std::string normstr, std::string procstr, int num_workers)
	{
		py::object o;
		{
			pybind11::gil_scoped_acquire acquire;
			o = py::none();
		}

		// Convert Python strings to C++ strings
		std::string input, pattern;
		{
			pybind11::gil_scoped_acquire acquire;
			Py_ssize_t size = 0;
			const char* buffer = (char*)PyUnicode_AsUTF8AndSize(inputstr.ptr(), &size);
			if (size > 0) {
				input = std::string(buffer, size);
			}
			buffer = (char*)PyUnicode_AsUTF8AndSize(patternstr.ptr(), &size);
			if (size > 0) {
				pattern = std::string(buffer, size);
			}
		}

		if (input.empty() || pattern.empty()) {
			return o; // Return None for empty input
		}

		try {
			// Convert normalization string to enum
			GrapaUnicode::NormalizationForm normalization = GrapaUnicode::NormalizationForm::NONE;
			std::string normstr_upper = normstr;
			std::transform(normstr_upper.begin(), normstr_upper.end(), normstr_upper.begin(), ::toupper);

			if (normstr_upper.empty() || normstr_upper == "NONE") {
				normalization = GrapaUnicode::NormalizationForm::NONE;
			}
			else if (normstr_upper == "NFC") {
				normalization = GrapaUnicode::NormalizationForm::NFC;
			}
			else if (normstr_upper == "NFD") {
				normalization = GrapaUnicode::NormalizationForm::NFD;
			}
			else if (normstr_upper == "NFKC") {
				normalization = GrapaUnicode::NormalizationForm::NFKC;
			}
			else if (normstr_upper == "NFKD") {
				normalization = GrapaUnicode::NormalizationForm::NFKD;
			}

			// Convert processing mode string to enum
			GrapaUnicode::ProcessingMode mode = GrapaUnicode::ProcessingMode::UNICODE_MODE;
			std::string procstr_upper = procstr;
			std::transform(procstr_upper.begin(), procstr_upper.end(), procstr_upper.begin(), ::toupper);

			if (procstr_upper.empty() || procstr_upper == "UNICODE") {
				mode = GrapaUnicode::ProcessingMode::UNICODE_MODE;
			}
			else if (procstr_upper == "BINARY") {
				mode = GrapaUnicode::ProcessingMode::BINARY_MODE;
			}

			// Call the direct C++ grep function
			auto matches = grep_extract_matches_unicode(input, pattern, optionsstr, delimstr, normalization, mode, static_cast<size_t>(num_workers));

			// Convert C++ vector<string> to Python list
			{
				pybind11::gil_scoped_acquire acquire;
				py::list result;
				for (const auto& match : matches) {
					result.append(py::str(match));
				}
				o = result;
			}
		}
		catch (const std::exception& e) {
			// Handle any exceptions and return error
			{
				pybind11::gil_scoped_acquire acquire;
				py::dict error_dict;
				error_dict["error"] = py::str(e.what());
				o = error_dict;
			}
		}

		return o;
	}

};

py::object grapa_eval(py::object cmdstr, py::object paramstr, std::string rulestr, std::string profilestr)
{
	GrapaStruct* gs = new GrapaStruct(py::none());
	py::object o = gs->eval(cmdstr, paramstr, rulestr, profilestr);
	delete gs;
	return o;
}

py::object grapa_grep(py::object inputstr, py::object patternstr, std::string optionsstr, std::string delimstr, std::string normstr, std::string procstr, int num_workers)
{
	GrapaStruct* gs = new GrapaStruct(py::none());
	py::object o = gs->grep(inputstr, patternstr, optionsstr, delimstr, normstr, procstr, num_workers);
	delete gs;
	return o;
}

PYBIND11_MODULE(grapapy, m)
{
	GrapaCHAR inStr, outStr, runStr;
	bool needExit = false, showConsole = false;
	GrapaLink::Start(needExit, showConsole, outStr, runStr);
	GrapaLink::GetGrapaSystem()->mLibraryQueue.PushTail(new GrapaPyRuleEvent(GrapaCHAR("py")));

	auto atexit = py::module_::import("atexit");
	atexit.attr("register")(py::cpp_function([]() {
		// perform cleanup here -- this function is called with the GIL held
		GrapaLink::Stop();
		}));
	
	m.doc() = R"pbdoc(
        GrapaPy extention
        -----------------------

        .. currentmodule:: grapapy

        .. autosummary::
           :toctree: _generate

           new - create an instance (state maintained between calls)
		   eval - eval a string, return a string
		   compile - compile a string into a Grapa operation object
		   grep - search for patterns in text using Grapa's grep functionality

		   Pass in 2'nd parameter as string or bytes, available using @$ARG in the script. 
		   
    )pbdoc";

	py::class_<GrapaStruct>(m, "grapa")
		.def(py::init<py::object>(), py::arg("s") = py::none())
		.def("eval", static_cast<py::object(GrapaStruct::*)(py::object, py::object, std::string, std::string)>(&GrapaStruct::eval), "", py::arg("s"), py::arg("a") = "", py::arg("r") = "", py::arg("p") = "", pybind11::call_guard<py::gil_scoped_release>())
		.def("compile", static_cast<py::object(GrapaStruct::*)(py::object, std::string, std::string)>(&GrapaStruct::compile), "", py::arg("s"), py::arg("r") = "", py::arg("p") = "", pybind11::call_guard<py::gil_scoped_release>())
		.def("grep", static_cast<py::object(GrapaStruct::*)(py::object, py::object, std::string, std::string, std::string, std::string, int)>(&GrapaStruct::grep), "", py::arg("input"), py::arg("pattern"), py::arg("options") = "", py::arg("delim") = "", py::arg("normstr") = "", py::arg("procstr") = "", py::arg("num_workers") = 0, pybind11::call_guard<py::gil_scoped_release>())
		;
	
	m.def("eval", &grapa_eval, R"pbdoc(
        Evaluate a Grapa script
    )pbdoc",
		py::arg("s"), py::arg("a") = "", py::arg("r") = "", py::arg("p") = "", pybind11::call_guard<py::gil_scoped_release>());
	
	m.def("grep", &grapa_grep, R"pbdoc(
        Search for patterns in text using Grapa's grep functionality
    )pbdoc",
		py::arg("input"), py::arg("pattern"), py::arg("options") = "", py::arg("delim") = "", py::arg("normstr") = "", py::arg("procstr") = "", py::arg("num_workers") = 0, pybind11::call_guard<py::gil_scoped_release>());
	
    m.attr("__version__") = "0.1.56";

}
