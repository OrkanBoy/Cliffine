#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long i64;

typedef float f32;
typedef double f64;

#include <string>
typedef std::string str;

#include <vector>
template<typename T>
using vec = std::vector<T>;

#include <set>
using std::set;

#include <array>
template<typename T, size_t n>
using arr = std::array<T, n>;

#include <span>
using std::span;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Log.hpp"
#define CLF_CRITICAL(...) clf::Log::GetLogger()->critical(__VA_ARGS__)
#define CLF_ERROR(...)    clf::Log::GetLogger()->error(__VA_ARGS__)
#define CLF_WARN(...)     clf::Log::GetLogger()->warn(__VA_ARGS__)
#define CLF_INFO(...)     clf::Log::GetLogger()->info(__VA_ARGS__)
#define CLF_DEBUG(...)    clf::Log::GetLogger()->debug(__VA_ARGS__)
#define CLF_TRACE(...)    clf::Log::GetLogger()->trace(__VA_ARGS__)

#define CLF_ASSERT(expr, ...) if(!(expr)) CLF_ERROR("{0}\n\tLine {1} at {2}", __VA_ARGS__, __LINE__, __FILE__);