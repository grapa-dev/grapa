#include <pybind11/pybind11.h>

namespace py = pybind11;

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

#include <iostream>
#include <string>

#include "grapa/GrapaLink.h"
#include "grapa/GrapaValue.h"
#include "grapa/GrapaSystem.h"
#include "grapa/GrapaCompress.h"

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
			grresult = mConsoleSend.SendSync(gSystem->mGrammar);
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

    std::string eval(std::string cmdstr, std::string paramstr)
	{
		pybind11::gil_scoped_acquire acquire;
		std::string r;
		if (cmdstr.length())
		{
			setARG(paramstr);
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return r; 
	}
	
    std::string eval(std::string cmdstr, py::bytes parambytes)
	{
		pybind11::gil_scoped_acquire acquire;
		std::string r;
		if (cmdstr.length())
		{
			setARG(parambytes);
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return r; 
	}
	
	py::bytes evalb(std::string cmdstr, std::string paramstr)
	{
		pybind11::gil_scoped_acquire acquire;
		std::string r;
		if (cmdstr.length())
		{
			setARG(paramstr);
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			GrapaCHAR grresult = mConsoleSend.SendSyncRaw(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return py::bytes(r);
	}
	
	py::bytes evalb(std::string cmdstr, py::bytes parambytes)
	{
		pybind11::gil_scoped_acquire acquire;
		std::string r;
		if (cmdstr.length())
		{
			setARG(parambytes);
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			GrapaCHAR grresult = mConsoleSend.SendSyncRaw(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return py::bytes(r);
	}
	
	std::string exec(py::bytes cmdbytes, std::string paramstr)
	{
		pybind11::gil_scoped_acquire acquire;
		std::string r;
		py::buffer_info info(py::buffer(cmdbytes).request());
		const char* buffer = reinterpret_cast<const char*>(info.ptr);
		size_t length = static_cast<size_t>(info.size);
		if (length>0)
		{
			setARG(paramstr);
			GrapaCHAR runStr(buffer, length);
			runStr.mToken = GrapaTokenType::RAW;
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return r; 
	}

	std::string exec(py::bytes cmdbytes, py::bytes parambytes)
	{
		pybind11::gil_scoped_acquire acquire;
		std::string r;
		py::buffer_info info(py::buffer(cmdbytes).request());
		const char* buffer = reinterpret_cast<const char*>(info.ptr);
		size_t length = static_cast<size_t>(info.size);
		if (length > 0)
		{
			setARG(parambytes);
			GrapaCHAR runStr(buffer, length);
			runStr.mToken = GrapaTokenType::RAW;
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return r; 
	}

	py::bytes execb(py::bytes cmdbytes, std::string paramstr)
	{
		pybind11::gil_scoped_acquire acquire;
		std::string r;
		py::buffer_info info(py::buffer(cmdbytes).request());
		const char* buffer = reinterpret_cast<const char*>(info.ptr);
		size_t length = static_cast<size_t>(info.size);
		if (length > 0)
		{
			setARG(paramstr);
			GrapaCHAR runStr(buffer, length);
			runStr.mToken = GrapaTokenType::RAW;
			GrapaCHAR grresult = mConsoleSend.SendSyncRaw(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return py::bytes(r);
	}

	py::bytes execb(py::bytes cmdbytes, py::bytes parambytes)
	{
		pybind11::gil_scoped_acquire acquire;
		std::string r;
		py::buffer_info info(py::buffer(cmdbytes).request());
		const char* buffer = reinterpret_cast<const char*>(info.ptr);
		size_t length = static_cast<size_t>(info.size);
		if (length > 0)
		{
			setARG(parambytes);
			GrapaCHAR runStr(buffer, length);
			runStr.mToken = GrapaTokenType::RAW;
			GrapaCHAR grresult = mConsoleSend.SendSyncRaw(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return py::bytes(r);
	}

};

PYBIND11_MODULE(grapapy, m)
{
	GrapaCHAR inStr, outStr, runStr;
	bool needExit = false, showConsole = false, showWidget = false;
	GrapaCHAR s = GrapaLink::Start(needExit, showConsole, showWidget, inStr, outStr, runStr);

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
		.def("eval", static_cast<std::string(GrapaStruct::*)(std::string, py::bytes)>(&GrapaStruct::eval), "", py::arg("c"), py::arg("p"), pybind11::call_guard<py::gil_scoped_release>())
		.def("eval", static_cast<std::string (GrapaStruct::*)(std::string,std::string)>(&GrapaStruct::eval),"",py::arg("c"),py::arg("p")="", pybind11::call_guard<py::gil_scoped_release>())
		.def("evalb", static_cast<py::bytes(GrapaStruct::*)(std::string, py::bytes)>(&GrapaStruct::evalb), "", py::arg("c"), py::arg("p"), pybind11::call_guard<py::gil_scoped_release>())
		.def("evalb", static_cast<py::bytes(GrapaStruct::*)(std::string, std::string)>(&GrapaStruct::evalb), "", py::arg("c"), py::arg("p") = "", pybind11::call_guard<py::gil_scoped_release>())
		.def("exec", static_cast<std::string(GrapaStruct::*)(py::bytes, py::bytes)>(&GrapaStruct::exec), "", py::arg("c"), py::arg("p"), pybind11::call_guard<py::gil_scoped_release>())
		.def("exec", static_cast<std::string (GrapaStruct::*)(py::bytes,std::string)>(&GrapaStruct::exec),"",py::arg("c"),py::arg("p")="", pybind11::call_guard<py::gil_scoped_release>())
		.def("execb", static_cast<py::bytes(GrapaStruct::*)(py::bytes, py::bytes)>(&GrapaStruct::execb), "", py::arg("c"), py::arg("p"), pybind11::call_guard<py::gil_scoped_release>())
		.def("execb", static_cast<py::bytes (GrapaStruct::*)(py::bytes,std::string)>(&GrapaStruct::execb),"",py::arg("c"),py::arg("p")="", pybind11::call_guard<py::gil_scoped_release>())
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
	
    m.attr("__version__") = "0.0.11";
	
	// GrapaLink::Stop();
}
