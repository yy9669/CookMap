#pragma once

/*
 * A Load< T > represents a T that will (eventually) be loaded.
 *
 * This is useful for global-scope resources that need an OpenGL context:
 *
 * //at global scope:
 * Load< Mesh > main_mesh([]() -> const Mesh * {
 *     return &Meshes.get("Main");
 * });
 *
 * //later:
 * void GameMode::draw() {
 *     glBindVertexArray(main_mesh->vao);
 * }
 *
 * Load<> is built on the add_load_function() call that adds a function to one of several lists of functions that are called after the OpenGL canvas is initialized.
 *
 * These functions are grouped by 'tags', which allow some sequencing of calls.
 * (particularly, this is useful for loading large data blobs [e.g. Meshes] before looking up individual elements within them.)
 *
 */

#include <functional>
#include <stdexcept>

enum LoadTag : uint32_t {
	LoadTagEarly,
	LoadTagDefault,
	LoadTagLate,
	MaxLoadTag //<-- just used to track # of load tags
};

//Add a function to an internal list of loading functions:
// (only call *before* "call_load_functions()")
void add_load_function(LoadTag tag, std::function< void() > const &fn);

//Call all loading functions:
// (loading functions may throw exceptions if they fail.)
// (only call *once*)
void call_load_functions();


//work-around for MSVC not accepting this as a lambda:
template< typename T >
T const *new_T() { return new T; }

template< typename T >
struct Load {
	//Constructing a Load< T > adds the passed function to the list of functions to call:
	Load(LoadTag tag, const std::function< T const *() > &load_fn = new_T< T >) : value(nullptr) {
		add_load_function(tag, [this,load_fn](){
			this->value = load_fn();
			if (!(this->value)) {
				throw std::runtime_error("Loading failed.");
			}
		});
	}

	//Make a "Load< T >" behave like a "T const *":
	explicit operator bool() { return value != nullptr; }
	operator T const *() { return value; }
	T const &operator*() { return *value; }
	T const *operator->() { return value; }

	T const *value;
};


//Specialization:
//Load< void > just calls a function:
template< >
struct Load< void > {
	//Constructing a Load< T > adds the passed function to the list of functions to call:
	Load( LoadTag tag, const std::function< void() > &load_fn) {
		add_load_function(tag, load_fn);
	}
};


