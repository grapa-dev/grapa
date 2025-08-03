// Minimal Python extension for Grapa
// Uses forward declarations to avoid Windows-specific header dependencies

// Static compilation definitions to avoid DLL dependencies
#ifdef _WIN32
#define UTF8PROC_STATIC
#define PCRE2_STATIC
#endif

#include <pybind11/pybind11.h>
#include <pybind11/eval.h>
#include <pybind11/embed.h>

namespace py = pybind11;
using namespace py::literals;

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

#include <iostream>
#include <string>

// Forward declarations to avoid including problematic headers
class GrapaCHAR;
class GrapaRuleEvent;
class GrapaScriptExec;
class GrapaNames;
class GrapaConsoleSend;
class GrapaConsoleResponse;
class GrapaLibraryEvent;
class GrapaLibraryRuleEvent;

// Minimal wrapper class that only exposes the necessary interface
class GrapaPyWrapper
{
private:
    void* mGrapaSystem; // Opaque pointer to avoid including headers

public:
    GrapaPyWrapper() : mGrapaSystem(nullptr) {
        // Initialize Grapa system without including headers
        // This will be handled by the static library
    }
    
    ~GrapaPyWrapper() {
        // Cleanup handled by static library
    }
    
    py::object eval(py::object cmdstr, py::object paramstr, std::string rulestr, std::string profilestr) {
        // This will be implemented by calling functions in the static library
        // For now, return a simple string to test the build
        return py::str("GrapaPy minimal wrapper - build test successful");
    }
    
    py::object compile(py::object cmdstr, std::string rulestr, std::string profilestr) {
        // This will be implemented by calling functions in the static library
        return py::str("GrapaPy minimal wrapper - compile test successful");
    }
};

py::object grapa_eval(py::object cmdstr, py::object paramstr, std::string rulestr, std::string profilestr)
{
    GrapaPyWrapper* wrapper = new GrapaPyWrapper();
    py::object o = wrapper->eval(cmdstr, paramstr, rulestr, profilestr);
    delete wrapper;
    return o;
}

PYBIND11_MODULE(grapapy, m)
{
    // Initialize Grapa system (this will be handled by the static library)
    // GrapaLink::Start() equivalent without including headers
    
    auto atexit = py::module_::import("atexit");
    atexit.attr("register")(py::cpp_function([]() {
        // perform cleanup here -- this function is called with the GIL held
        // GrapaLink::Stop() equivalent without including headers
    }));
    
    m.doc() = R"pbdoc(
        GrapaPy minimal extension
        -----------------------

        .. currentmodule:: grapapy

        .. autosummary::
           :toctree: _generate

           new - create an instance (state maintained between calls)
           eval - eval a string, return a string

           Pass in 2'nd parameter as string or bytes, available using @$ARG in the script. 
           
    )pbdoc";

    py::class_<GrapaPyWrapper>(m, "grapa")
        .def(py::init<>())
        .def("eval", &GrapaPyWrapper::eval, "", py::arg("s"), py::arg("a") = "", py::arg("r") = "", py::arg("p") = "", pybind11::call_guard<py::gil_scoped_release>())
        .def("compile", &GrapaPyWrapper::compile, "", py::arg("s"), py::arg("r") = "", py::arg("p") = "", pybind11::call_guard<py::gil_scoped_release>())
        ;
    
    m.def("eval", &grapa_eval, R"pbdoc(
        Evaluate a Grapa script
    )pbdoc",
        py::arg("s"), py::arg("a") = "", py::arg("r") = "", py::arg("p") = "", pybind11::call_guard<py::gil_scoped_release>());
    
    m.attr("__version__") = "0.0.114";
} 