#include "bindings.h"

PYBIND11_MODULE(_cpputils, module) {

    module.doc() = R"pbdoc(
        cpputils
        -----------------------
        .. currentmodule:: cpputils

        .. autosummary::
           :toctree: _generate

            ini_load
            eval_type
            simple_hmac
            sha256
    )pbdoc";

    module.def("ini_load", [](const py::object& files,
                           const py::object& sections,
                           const py::object& keys,
                           const py::object& defaults
                           ) -> py::object {
                const auto m_files = string_operations::convert_to_map_str(files);
                const auto m_sections = string_operations::convert_to_map_str(sections);
                const auto m_keys = string_operations::convert_to_map_str(keys);
                const auto m_defaults = string_operations::convert_to_map_py(defaults);
                return ini::ini_load(m_files, m_sections, m_keys, m_defaults);
                },
            py::arg("files").none(true) = py::cast(NULL),
            py::arg("sections").none(true) = py::cast(NULL),
            py::arg("keys").none(true) = py::cast(NULL),
            py::arg("defaults").none(true) = py::cast(NULL),
            R"pbdoc(loads config-files into dictionary)pbdoc");

    module.def("eval_type", [](const py::str& value) -> py::object {
                   return string_operations::eval_type(value.cast<std::string>()); },
               py::arg("value").none(false),
               R"pbdoc(evaluate string into python object)pbdoc");

    module.def("simple_hmac", [](const std::vector<std::string>& data, const std::string &algo) -> py::object {
                   return py::cast(digest::simple_hmac({data.begin() + 1, data.end()}, data.at(0), algo)); },
               py::arg("data").none(false),
               py::arg("algo").none(true) = "SHA256",
               R"pbdoc(hashes multiple strings with key)pbdoc");

    module.def("simple_sha256", [](const py::object& data) -> py::object {
                   SHA256 sha256;
                   return py::cast(sha256(data.cast<std::string>())); },
               py::arg("data").none(false),
               R"pbdoc(hashes multiple strings with key)pbdoc");

    module.attr("__name__") = "_cpputils";
#ifdef VERSION_INFO
    module.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
