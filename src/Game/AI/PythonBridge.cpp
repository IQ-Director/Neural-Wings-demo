#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "AIEnvironment.h"

namespace py = pybind11;

PYBIND11_MODULE(nw_engine, m)
{
     py::class_<AIEnvironment>(m, "AIEnv")
         .def(py::init<int, int>())
         .def_static("initContext", &AIEnvironment::initContext)
         .def("init", &AIEnvironment::Init)
         .def("getTime", &AIEnvironment::GetTime)
         .def("reset", [](AIEnvironment &self)
              {
        StepResult res = self.Reset();
        
        std::vector<ssize_t> shape = { (ssize_t)self.GetHeight(), (ssize_t)self.GetWidth(), 4 };
        return py::array_t<float>(shape, res.image_data.data()); })
         .def("step", [](AIEnvironment &self, std::vector<float> actions)
              {
        StepResult res = self.Step(actions);
        std::vector<ssize_t> shape = { (ssize_t)self.GetHeight(), (ssize_t)self.GetWidth(), 4 };
        py::array_t<float> obs = py::array_t<float>(shape, res.image_data.data());
        return py::make_tuple(obs, res.reward, res.done); });
}