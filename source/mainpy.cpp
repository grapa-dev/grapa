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

std::string Grapaeval(std::string cmdstr)
{
    GrapaCHAR inStr, outStr, runStr;
    bool needExit = false, showConsole = false, showWidget = false;
    GrapaCHAR s = GrapaLink::Start(needExit, showConsole, showWidget, inStr, outStr, runStr);

    GrapaScriptExec mScriptExec;
    GrapaConsoleSend mConsoleSend;
    GrapaMainResponse mConsoleResponse;
    GrapaNames mRuleVariables;
    mConsoleSend.mScriptState.vScriptExec = &mScriptExec;
    mScriptExec.vScriptState = &mConsoleSend.mScriptState;
    mConsoleSend.mScriptState.SetNameSpace(&mRuleVariables);
    mRuleVariables.SetResponse(&mConsoleResponse);
    mConsoleSend.Start();
    GrapaCHAR grresult;
    if (gSystem->mGrammar.mLength)
        grresult = mConsoleSend.SendSync(gSystem->mGrammar);
    grresult.SetLength(0);
    std::string r;
    if (cmdstr.length())
    {
        GrapaCHAR runStr(cmdstr.c_str());
        grresult = mConsoleSend.SendSync(runStr);
        r.assign((char*)grresult.mBytes, grresult.mLength);
    }

    GrapaLink::Stop();
    return r;
}

int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(grapapy, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: cmake_example

        .. autosummary::
           :toctree: _generate

           add
           subtract
           eval
    )pbdoc";

    m.def("add", &add, R"pbdoc(
        Add two numbers

        Some other explanation about the add function.
    )pbdoc");

    m.def("subtract", [](int i, int j) { return i - j; }, R"pbdoc(
        Subtract two numbers

        Some other explanation about the subtract function.
    )pbdoc");

    m.def("eval", &Grapaeval, R"pbdoc(
        Evaluate a Grapa script

        Some other explanation about the Grapaeval function.
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
