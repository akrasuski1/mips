#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl_bind.h>
#include "../faiss/Index.h"

namespace py = pybind11;
using namespace py::literals;

template <typename T>
class STLVectorWrapper {

public:
    STLVectorWrapper(std::vector<T> &vec) : data_(vec) {
        base_ = py::capsule((void*) &data_,
                            [](void*){});
    };

    py::array_t<T> as_array() {
        return py::array_t<T>(data_.size(), data_.data(), base_);
    };

private:
    STLVectorWrapper() = delete;
    STLVectorWrapper(STLVectorWrapper& other) = delete;

    py::capsule base_;
    std::vector<T> &data_;
};


#define WRAP_INDEX_HELPER(ClassName, obj) {                                                                            \
    obj.def("add",                                                                                                     \
            [](ClassName& self, py::array_t<float, py::array::c_style | py::array::forcecast> data) {                  \
                auto n = data.request().shape[0];                                                            \
                auto data_ptr = (float*) data.request().ptr;                                                           \
                self.add(n, data_ptr);                                                                                 \
            },                                                                                                         \
            "data"_a,                                                                                           \
            "Add database vectors to this index");                                                                     \
                                                                                                                       \
    obj.def("search",                                                                                                  \
            [](ClassName& self, py::array_t<float, py::array::c_style | py::array::forcecast> data, long k) {          \
                auto n = data.request().shape[0];                                                                      \
                                                                                                                       \
                py::array_t<float> distances(n);                                                                       \
                py::array_t<long> labels(n);                                                                           \
                                                                                                                       \
                auto data_ptr = (float*) data.request().ptr;                                                           \
                auto distances_ptr = (float*) distances.request().ptr;                                                 \
                auto labels_ptr = (long*) labels.request().ptr;                                                        \
                                                                                                                       \
                self.search(n, data_ptr, k, distances_ptr, labels_ptr);                                                \
                                                                                                                       \
                return std::make_tuple(distances, labels);                                                             \
            },                                                                                                         \
            "data"_a, "k"_a,                                                                                    \
            "Search data vectors for k closest vectors stored in database");                                           \
                                                                                                                       \
    obj.def("reset",                                                                                                   \
            &ClassName::reset,                                                                                         \
            "Reset the state of the Index");                                                                           \
}
