#pragma once
#pragma warning(disable : 4996)
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <array>
#include <fstream>
#include <cstdint>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "PlatformDetection.h"

#ifdef CE_PLATFORM_WINDOWS
    #ifndef NOMINMAX
    # define NOMINMAX
    #endif
    #include <Windows.h>
#endif

#ifdef CE_DEBUG
	#if defined(CE_PLATFORM_WINDOWS)
		#define CE_DEBUGBREAK() __debugbreak()
	#elif defined(CE_PLATFORM_LINUX)
		#include <signal.h>
		#define CE_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define CE_ENABLE_ASSERTS
#else
	#define CE_DEBUGBREAK()
#endif

#define CE_EXPAND_MACRO(x) x
#define CE_STRINGIFY_MACRO(x) #x

// Define CE_API for import/export depending on platform and usage
#if defined(CE_PLATFORM_WINDOWS)
    #ifdef CE_EXPORT    
    #ifdef _MSC_VER
        #define CE_API __declspec(dllexport)
    #else
        #define CE_API __attribute__((visibility("default")))
    #endif
    #endif

    #ifdef CE_IMPORT           
    #ifdef _MSC_VER
        #define CE_API __declspec(dllimport)
    #else
        #define CE_API
    #endif
    #endif
#else
    // Linux/macOS: only need to set visibility when exporting
    #ifdef CE_EXPORT
        #define CE_API __attribute__((visibility("default")))
    #else
        #define CE_API
    #endif
#endif

namespace CE {

    //--------------------- Scope = unique pointer --------------------
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args){

        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    //--------------------- Ref = shared pointer -----------------------
    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args){

        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}
