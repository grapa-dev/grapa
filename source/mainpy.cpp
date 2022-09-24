#include <pybind11/pybind11.h>
#include "pybind11/include/pybind11/pybind11.h"
#include "pybind11/include/pybind11/eval.h"

namespace py = pybind11;

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
	static GrapaRuleEvent* ToGrapa(py::object& o)
	{
		GrapaRuleEvent* result = NULL;
		GrapaCHAR pStr;
		if (PyUnicode_Check(o.ptr()))
		{
			std::string retvalue = o.cast<std::string>();
			pStr.FROM(retvalue.c_str(), retvalue.length());
			result = new GrapaRuleEvent(0, GrapaCHAR("result"), pStr);
		}
		else if (PyLong_Check(o.ptr()))
		{
			long retvalue = PyLong_AsLong(o.ptr());
			pStr = GrapaInt(retvalue).getBytes();
			result = new GrapaRuleEvent(0, GrapaCHAR("result"), pStr);
		}
		else if (PyBool_Check(o.ptr()))
		{
			bool retvalue = o.cast<bool>();
			pStr.SetBool(retvalue);
			result = new GrapaRuleEvent(0, GrapaCHAR("result"), pStr);
		}
		else if (PyFloat_Check(o.ptr()))
		{
			double retvalue = PyFloat_AS_DOUBLE(o.ptr());
			pStr = GrapaFloat(retvalue).getBytes();
			result = new GrapaRuleEvent(0, GrapaCHAR("result"), pStr);
		}
		else if (PyTuple_Check(o.ptr()))
		{
			result = new GrapaRuleEvent(0, GrapaCHAR("result"), GrapaCHAR());
			result->vQueue = new GrapaRuleQueue();
			result->mValue.mToken = GrapaTokenType::ARRAY;
			for (int i = 0; i < PyTuple_Size(o.ptr()); ++i)
			{
				PyObject* item = PyTuple_GetItem(o.ptr(), i);
				if (PyUnicode_Check(item))
				{
					Py_ssize_t size;
					char* ptr = (char*)PyUnicode_AsUTF8AndSize(item, &size);
					result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR(ptr, size)));
				}
				else if (PyFloat_Check(item))
				{
					double retvalue = PyFloat_AS_DOUBLE(item);
					pStr = GrapaFloat(retvalue).getBytes();
					result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), pStr));
				}
				else if (PyLong_Check(item))
				{
					long retvalue = PyLong_AsLong(item);
					pStr = GrapaInt(retvalue).getBytes();
					result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), pStr));
				}
				else
				{
					result->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR()));
				}
			}
		}
		else if (PyList_Check(o.ptr()))
		{
			for (int i = 0; i < PyList_Size(o.ptr()); ++i)
			{
				PyObject* item = PyList_GetItem(o.ptr(), i);

			}
		}
		else if (PyDict_Check(o.ptr()))
		{
			result = new GrapaRuleEvent(0, GrapaCHAR("result"), GrapaCHAR());
			result->vQueue = new GrapaRuleQueue();
			result->mValue.mToken = GrapaTokenType::LIST;
			PyObject* key, * item;
			Py_ssize_t pos = 0;
			while (PyDict_Next(o.ptr(), &pos, &key, &item))
			{
				Py_ssize_t nsize;
				char* nptr = (char*)PyUnicode_AsUTF8AndSize(key, &nsize);
				GrapaCHAR kname(nptr, nsize);
				if (PyUnicode_Check(item))
				{
					Py_ssize_t size;
					char* ptr = (char*)PyUnicode_AsUTF8AndSize(item, &size);
					result->vQueue->PushTail(new GrapaRuleEvent(0, kname, GrapaCHAR(ptr, size)));
				}
				else if (PyFloat_Check(item))
				{
					double retvalue = PyFloat_AS_DOUBLE(item);
					pStr = GrapaFloat(retvalue).getBytes();
					result->vQueue->PushTail(new GrapaRuleEvent(0, kname, pStr));
				}
				else if (PyLong_Check(item))
				{
					long retvalue = PyLong_AsLong(item);
					pStr = GrapaInt(retvalue).getBytes();
					result->vQueue->PushTail(new GrapaRuleEvent(0, kname, pStr));
				}
				else
				{
					result->vQueue->PushTail(new GrapaRuleEvent(0, kname, GrapaCHAR()));
				}
			}
		}
		return result;
	}
	
	static void FromGrapa(GrapaRuleEvent* e, py::object* o)
	{
		if (e == NULL) return;
		GrapaInt a;
		GrapaInt f;
		GrapaCHAR s;
		switch (e->mValue.mToken)
		{
		case GrapaTokenType::STR:
			*o = py::str((char*)e->mValue.mBytes, e->mValue.mLength);
			break;
		case GrapaTokenType::RAW:
			*o = py::bytes((char*)e->mValue.mBytes, e->mValue.mLength);
			break;
		case GrapaTokenType::INT:
			a.FromBytes(e->mValue);
			*o = py::int_(a.LongValue());
			break;
		case GrapaTokenType::FLOAT:
			f.FromBytes(e->mValue);
			s = f.ToString();
			*o = py::float_(std::stod((char*)s.mBytes));
			break;
		default:
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
		GrapaLibraryParam import_param(vScriptExec, pNameSpace, pInput ? pInput->Head(1) : NULL);
		GrapaLibraryParam attr_param(vScriptExec, pNameSpace, pInput ? pInput->Head(2) : NULL);
		if (script_param.vVal && script_param.vVal->mValue.mLength)
		{
			std::string sript_str;
			GrapaCHAR type_str(""), import_str("__main__"), attr_str("__dict__");
			sript_str.assign((char*)script_param.vVal->mValue.mBytes, script_param.vVal->mValue.mLength);
			if (import_param.vVal && import_param.vVal->mValue.mLength) import_str.FROM(import_param.vVal->mValue);
			if (attr_param.vVal && attr_param.vVal->mValue.mLength) attr_str.FROM(attr_param.vVal->mValue);
			py::object scope = py::module_::import((char*)import_str.mBytes).attr((char*)attr_str.mBytes);
			GrapaCHAR pStr;
			py::object o = py::eval(sript_str, scope);
			result = GrapaPyObject::ToGrapa(o);
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
	GrapaStruct()
	{
		mConsoleSend.mScriptState.vScriptExec = &mScriptExec;
		mScriptExec.vScriptState = &mConsoleSend.mScriptState;
		mConsoleSend.mScriptState.SetNameSpace(&mRuleVariables);
		mRuleVariables.SetResponse(&mConsoleResponse);
		mConsoleSend.Start();
		GrapaCHAR grresult;
		GrapaSystem* gSystem = GrapaLink::GetGrapaSystem();
		if (gSystem->mGrammar.mLength)
			grresult = mConsoleSend.SendSync(gSystem->mGrammar, NULL, 0, GrapaCHAR());
		GrapaCHAR runStr("$global[\"$py\"] = class {eval = op(script,import=\"\",attr=\"\"){@<py,eval,{@<var,{script}>,@<var,{import}>,@<var,{attr}>}>();};};");
		grresult = mConsoleSend.SendSync(runStr,NULL,0,GrapaCHAR());
	}
    ~GrapaStruct() 
	{ 
		mConsoleSend.Stop();
	}
	
	void setARG(GrapaCHAR& pStr)
	{
		GrapaRuleEvent* e = mRuleVariables.GetNameQueue()->Tail();
		s64 idx = 0;
		GrapaRuleEvent* v = e->vQueue->Search("$ARG", idx);
		if (v)
		{
			v->CLEAR();
			v->mValue.FROM(pStr);
		}
		else
		{
			mConsoleSend.mScriptState.AddRawParameter(e, GrapaCHAR("$ARG"), pStr);
		}
	}

	void setARG(std::string paramstr)
	{
		GrapaCHAR pStr(paramstr.c_str(), paramstr.length());
		setARG(pStr);
	}

	void setARG(py::bytes parambytes)
	{
		py::buffer_info info(py::buffer(parambytes).request());
		const char* buffer = reinterpret_cast<const char*>(info.ptr);
		size_t length = static_cast<size_t>(info.size);
		GrapaCHAR pStr(buffer, length);
		pStr.mToken = GrapaTokenType::RAW;
		setARG(pStr);
	}

	py::object eval(std::string cmdstr, std::string paramstr, std::string rulestr, std::string profilestr)
	{
		GrapaCHAR runStr;
		GrapaRuleEvent* rulexx = NULL;
		GrapaCHAR profStr;
		py::object o;
		GrapaRuleEvent* grresult = NULL;
		if (true)
		{
			pybind11::gil_scoped_acquire acquire;
			o = py::object(py::cast(NULL));
			if (cmdstr.length())
			{
				setARG(paramstr);
				runStr.FROM(cmdstr.c_str(), cmdstr.length());
			}
			if (rulestr.length())
			{
				GrapaCHAR rStr(rulestr.c_str(), rulestr.length());
				rulexx = mConsoleSend.mScriptState.SearchVariable(mConsoleSend.mScriptState.GetNameSpace(), rStr);
			}
			profStr.FROM(profilestr.c_str(), profilestr.length());
		}
		if (cmdstr.length())
		{
			grresult = mConsoleSend.SendSyncResult(runStr, rulexx, 0, profStr);
		}
		if (grresult)
		{
			pybind11::gil_scoped_acquire acquire;
			GrapaRuleEvent* echo = grresult;
			while (echo && echo->mValue.mToken == GrapaTokenType::PTR)
				echo = echo->vRulePointer;
			GrapaPyObject::FromGrapa(echo,&o);
			grresult->CLEAR();
			delete grresult;
		}
		return o;
	}

	py::object eval(std::string cmdstr, py::bytes parambytes, std::string rulestr, std::string profilestr)
	{
		GrapaCHAR runStr;
		GrapaRuleEvent* rulexx = NULL;
		GrapaCHAR profStr;
		py::object o;
		GrapaRuleEvent* grresult = NULL;
		if (true)
		{
			pybind11::gil_scoped_acquire acquire;
			if (cmdstr.length())
			{
				setARG(parambytes);
				runStr.FROM(cmdstr.c_str(), cmdstr.length());
			}
			if (rulestr.length())
			{
				GrapaCHAR rStr(rulestr.c_str(), rulestr.length());
				rulexx = mConsoleSend.mScriptState.SearchVariable(mConsoleSend.mScriptState.GetNameSpace(), rStr);
			}
			profStr.FROM(profilestr.c_str(), profilestr.length());
		}
		if (cmdstr.length())
		{
			grresult = mConsoleSend.SendSyncResult(runStr, rulexx, 0, profStr);
		}
		if (grresult)
		{
			pybind11::gil_scoped_acquire acquire;
			GrapaRuleEvent* echo = grresult;
			while (echo && echo->mValue.mToken == GrapaTokenType::PTR)
				echo = echo->vRulePointer;
			GrapaPyObject::FromGrapa(echo, &o);
			grresult->CLEAR();
			delete grresult;
		}
		return o;
	}
	
	py::object exec(py::bytes cmdbytes, std::string paramstr)
	{
		size_t length = 0;
		GrapaCHAR runStr;
		GrapaRuleEvent* rulexx = NULL;
		GrapaCHAR profStr;
		py::object o;
		GrapaRuleEvent* grresult = NULL;
		if (true)
		{
			pybind11::gil_scoped_acquire acquire;
			py::buffer_info info(py::buffer(cmdbytes).request());
			const char* buffer = reinterpret_cast<const char*>(info.ptr);
			length = static_cast<size_t>(info.size);
			if (length > 0)
			{
				setARG(paramstr);
				runStr.FROM(buffer, length);
				runStr.mToken = GrapaTokenType::RAW;
			}
		}
		if (length)
		{
			grresult = mConsoleSend.SendSyncResult(runStr, rulexx, 0, profStr);
		}
		if (grresult)
		{
			pybind11::gil_scoped_acquire acquire;
			GrapaRuleEvent* echo = grresult;
			while (echo && echo->mValue.mToken == GrapaTokenType::PTR)
				echo = echo->vRulePointer;
			GrapaPyObject::FromGrapa(echo, &o);
			grresult->CLEAR();
			delete grresult;
		}
		return o;
	}

	py::object exec(py::bytes cmdbytes, py::bytes parambytes)
	{
		size_t length = 0;
		GrapaCHAR runStr;
		GrapaRuleEvent* rulexx = NULL;
		GrapaCHAR profStr;
		py::object o;
		GrapaRuleEvent* grresult = NULL;
		if (true)
		{
			pybind11::gil_scoped_acquire acquire;
			py::buffer_info info(py::buffer(cmdbytes).request());
			const char* buffer = reinterpret_cast<const char*>(info.ptr);
			length = static_cast<size_t>(info.size);
			if (length > 0)
			{
				setARG(parambytes);
				runStr.FROM(buffer, length);
				runStr.mToken = GrapaTokenType::RAW;
			}
		}
		if (length)
		{
			grresult = mConsoleSend.SendSyncResult(runStr, rulexx, 0, profStr);
		}
		if (grresult)
		{
			pybind11::gil_scoped_acquire acquire;
			GrapaRuleEvent* echo = grresult;
			while (echo && echo->mValue.mToken == GrapaTokenType::PTR)
				echo = echo->vRulePointer;
			GrapaPyObject::FromGrapa(echo, &o);
			grresult->CLEAR();
			delete grresult;
		}
		return o;
	}

};

PYBIND11_MODULE(grapapy, m)
{
	GrapaCHAR inStr, outStr, runStr;
	bool needExit = false, showConsole = false, showWidget = false;
	GrapaCHAR s = GrapaLink::Start(needExit, showConsole, showWidget, inStr, outStr, runStr);
	GrapaLink::GetGrapaSystem()->mLibraryQueue.PushTail(new GrapaPyRuleEvent(GrapaCHAR("py")));

    m.doc() = R"pbdoc(
        GrapaPy extention
        -----------------------

        .. currentmodule:: grapapy

        .. autosummary::
           :toctree: _generate

           new - create an instance (state maintained between calls)
		   eval - eval a string, return a string
		   evalb - eval a string, return bytes
		   exec - exec compiled script, return a string
		   execb - exec compiled script, return bytes

		   Pass in 2'nd parameter as string or bytes, available using @$ARG in the script. 
		   
    )pbdoc";

	py::class_<GrapaStruct>(m, "new")
		.def(py::init<>())
		.def("eval", static_cast<py::object(GrapaStruct::*)(std::string, py::bytes, std::string, std::string)>(&GrapaStruct::eval), "", py::arg("c"), py::arg("p"), py::arg("r") = "", py::arg("f") = "", pybind11::call_guard<py::gil_scoped_release>())
		.def("eval", static_cast<py::object(GrapaStruct::*)(std::string,std::string, std::string, std::string)>(&GrapaStruct::eval),"",py::arg("c"),py::arg("p") = "", py::arg("r") = "", py::arg("f") = "", pybind11::call_guard<py::gil_scoped_release>())
		.def("exec", static_cast<py::object(GrapaStruct::*)(py::bytes, py::bytes)>(&GrapaStruct::exec), "", py::arg("c"), py::arg("p"), pybind11::call_guard<py::gil_scoped_release>())
		.def("exec", static_cast<py::object(GrapaStruct::*)(py::bytes,std::string)>(&GrapaStruct::exec),"",py::arg("c"),py::arg("p")="", pybind11::call_guard<py::gil_scoped_release>())
		;
	
    /*
	m.def("eval", &Grapaeval, R"pbdoc(
        Evaluate a Grapa script
    )pbdoc",
		py::arg("s"));

    m.def("evalb", &Grapaevalb, R"pbdoc(
        Evaluate a Grapa script
    )pbdoc",
		py::arg("s"));

    m.def("exec", &Grapaexec, R"pbdoc(
        Evaluate a Grapa script
    )pbdoc",
		py::arg("b"));
	
    m.def("execb", &Grapaexecb, R"pbdoc(
        Evaluate a Grapa script
    )pbdoc",
		py::arg("b"));
	*/
	
    m.attr("__version__") = "0.0.16";
	
	// GrapaLink::Stop();
}
