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

extern GrapaSystem* gSystem;
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
		if (gSystem->mGrammar.mLength)
			grresult = mConsoleSend.SendSync(gSystem->mGrammar);
	}
    ~GrapaStruct() 
	{ 
		mConsoleSend.Stop();
	}
	
    std::string eval(std::string cmdstr, std::string paramstr)
	{
		std::string r;
		if (cmdstr.length())
		{
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			/*
			GrapaRuleEvent* parameter = NULL;
			parameter = mScriptExec.vScriptState->SearchVariable(&mRuleVariables, GrapaCHAR("$ARGCIN"));
			if (parameter==NULL)
				mRuleVariables.GetNameQueue()->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$ARGCIN"), GrapaCHAR(paramstr.c_str(), paramstr.length())));
			else
			{
				parameter->CLEAR();
				parameter->mValue.FROM(GrapaCHAR(paramstr.c_str(), paramstr.length()));
				parameter->mValue.mToken = GrapaTokenType::STR;
			}
			*/
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return r; 
	}
	
    std::string eval(std::string cmdstr, py::bytes parambytes)
	{
		std::string r;
		if (cmdstr.length())
		{
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			/*
			std::string paramstr = py::str(parambytes);
			GrapaRuleEvent* parameter = NULL;
			parameter = mScriptExec.vScriptState->SearchVariable(&mRuleVariables, GrapaCHAR("$ARGCIN"));
			if (parameter==NULL)
				mRuleVariables.GetNameQueue()->PushTail(new GrapaRuleEvent(0, GrapaCHAR("$ARGCIN"), GrapaBYTE(paramstr.c_str(), paramstr.length())));
			else
			{
				parameter->CLEAR();
				parameter->mValue.FROM(GrapaCHAR(paramstr.c_str(), paramstr.length()));
				parameter->mValue.mToken = GrapaTokenType::RAW;
			}
			*/
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return r; 
	}
	
	py::bytes evalb(std::string cmdstr, std::string paramstr)
	{
		std::string r;
		if (cmdstr.length())
		{
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return py::bytes(r);
	}
	
	py::bytes evalb(std::string cmdstr, py::bytes parambytes)
	{
		std::string r;
		if (cmdstr.length())
		{
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return py::bytes(r);
	}
	
	std::string eval(py::bytes cmdbytes, std::string paramstr)
	{
		std::string r;
		std::string cmdstr = py::str(cmdbytes);
		if (cmdstr.length())
		{
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return r; 
	}

	std::string eval(py::bytes cmdbytes, py::bytes parambytes)
	{
		std::string r;
		std::string cmdstr = py::str(cmdbytes);
		if (cmdstr.length())
		{
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return r; 
	}

	py::bytes evalb(py::bytes cmdbytes, std::string paramstr)
	{
		std::string r;
		std::string cmdstr = py::str(cmdbytes);
		if (cmdstr.length())
		{
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return py::bytes(r);
	}

	py::bytes evalb(py::bytes cmdbytes, py::bytes parambytes)
	{
		std::string r;
		std::string cmdstr = py::str(cmdbytes);
		if (cmdstr.length())
		{
			GrapaCHAR runStr(cmdstr.c_str(), cmdstr.length());
			GrapaCHAR grresult = mConsoleSend.SendSync(runStr);
			r.assign((char*)grresult.mBytes, grresult.mLength);
		}
		return py::bytes(r);
	}

};

/*
std::string Grapaeval(std::string cmdstr)
{
	GrapaStruct gs;
	return gs.eval(cmdstr);
}

py::bytes Grapaevalb(std::string cmdstr)
{
	GrapaStruct gs;
	return gs.evalb(cmdstr);
}

std::string Grapaexec(py::bytes cmdbytes)
{
	GrapaStruct gs;
	return gs.exec(cmdbytes);
}

py::bytes Grapaexecb(py::bytes cmdbytes)
{
	GrapaStruct gs;
	return gs.execb(cmdbytes);
}
*/

PYBIND11_MODULE(grapapy, m)
{
	GrapaCHAR inStr, outStr, runStr;
	bool needExit = false, showConsole = false, showWidget = false;
	GrapaCHAR s = GrapaLink::Start(needExit, showConsole, showWidget, inStr, outStr, runStr);
	
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: cmake_example

        .. autosummary::
           :toctree: _generate

           new - create an instance (state maintained between calls)
		   eval - eval a string, return a string
		   evalb - eval a string, return bytes
		   
    )pbdoc";

	py::class_<GrapaStruct>(m, "new")
		.def(py::init<>())
		.def("eval", static_cast<std::string (GrapaStruct::*)(std::string,std::string)>(&GrapaStruct::eval),"",py::arg("c"),py::arg("p")="")
		.def("eval", static_cast<std::string (GrapaStruct::*)(std::string,py::bytes)>(&GrapaStruct::eval),"",py::arg("c"),py::arg("p")="")
		.def("eval", static_cast<std::string (GrapaStruct::*)(py::bytes,std::string)>(&GrapaStruct::eval),"",py::arg("c"),py::arg("p")="")
		.def("eval", static_cast<std::string (GrapaStruct::*)(py::bytes,py::bytes)>(&GrapaStruct::eval),"",py::arg("c"),py::arg("p")="")
		.def("evalb", static_cast<py::bytes (GrapaStruct::*)(std::string,std::string)>(&GrapaStruct::evalb),"",py::arg("c"),py::arg("p")="")
		.def("evalb", static_cast<py::bytes (GrapaStruct::*)(std::string,py::bytes)>(&GrapaStruct::evalb),"",py::arg("c"),py::arg("p")="")
		.def("evalb", static_cast<py::bytes (GrapaStruct::*)(py::bytes,std::string)>(&GrapaStruct::evalb),"",py::arg("c"),py::arg("p")="")
		.def("evalb", static_cast<py::bytes (GrapaStruct::*)(py::bytes,py::bytes)>(&GrapaStruct::evalb),"",py::arg("c"),py::arg("p")="")
		/*
		.def("eval", &GrapaStruct::eval,"",py::arg("s"))
		.def("evalb", &GrapaStruct::evalb,"",py::arg("s"))
		.def("exec", &GrapaStruct::exec, "", py::arg("b"))
		.def("execb", &GrapaStruct::execb, "", py::arg("b"))
		*/
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
	
    m.attr("__version__") = "0.0.8";
	
	// GrapaLink::Stop();
}
