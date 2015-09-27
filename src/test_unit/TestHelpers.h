#pragma once

// used to make phony shared_ptr<T> instances that don't call `free()`
template<typename T>
struct NonDeleter {
    void operator()(T* t) const {
    }
};

