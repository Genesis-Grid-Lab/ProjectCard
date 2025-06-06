#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <queue>
#include <algorithm>

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 1280

// namespace GA {

//     //--------------------- Scope = unique pointer --------------------
//     template<typename T>
//     using Scope = std::unique_ptr<T>;
//     template<typename T, typename ... Args>
//     constexpr Scope<T> CreateScope(Args&& ... args){

//         return std::make_unique<T>(std::forward<Args>(args)...);
//     }

//     //--------------------- Ref = shared pointer -----------------------
//     template<typename T>
//     using Ref = std::shared_ptr<T>;
//     template<typename T, typename ... Args>
//     constexpr Ref<T> CreateRef(Args&& ... args){

//         return std::make_shared<T>(std::forward<Args>(args)...);
//     }
// }
