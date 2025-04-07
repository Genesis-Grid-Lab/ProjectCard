#pragma once

#include "Config.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace CE {

    class CE_API Log{
    public:
        static void Init();
        
        static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger;}
        static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger;}
    private:
        static Ref<spdlog::logger> s_CoreLogger;
        static Ref<spdlog::logger> s_ClientLogger;
    };
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternio)
{
	return os << glm::to_string(quaternio);
}

// Core log macros
#define CE_CORE_TRACE(...)    ::CE::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CE_CORE_INFO(...)     ::CE::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CE_CORE_WARN(...)     ::CE::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CE_CORE_ERROR(...)    ::CE::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CE_CORE_CRITICAL(...) ::CE::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define CE_TRACE(...)         ::CE::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CE_INFO(...)          ::CE::Log::GetClientLogger()->info(__VA_ARGS__)
#define CE_WARN(...)          ::CE::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CE_ERROR(...)         ::CE::Log::GetClientLogger()->error(__VA_ARGS__)
#define CE_CRITICAL(...)      ::CE::Log::GetClientLogger()->critical(__VA_ARGS__)