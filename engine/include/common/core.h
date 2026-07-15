#pragma once
#include <queue>
#include <vector>
#include <string>
#include <bitset>
#include <memory>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include <map>
#include <random>

// include spdlog
#ifndef FMT_HEADER_ONLY
    #define FMT_HEADER_ONLY
#endif

#ifndef SPDLOG_FMT_EXTERNAL
    #define SPDLOG_FMT_EXTERNAL
#endif

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


// include GLAD
#include <glad/gl.h>

// include glm
#ifndef GLM_ENABLE_EXPERIMENTAL
    #define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

// OS Check
#ifdef FLOW_EXPORT
    #ifdef _MSC_VER
    #define FLOW_API __declspec(dllexport)
    #else
    #define FLOW_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _MSC_VER
    #define FLOW_API __declspec(dllimport)
    #else
    #define FLOW_API
    #endif
#endif

// Runtime assertion to track where program execution fails
// (or block execution if conditions not met at runtime)
#define FLOW_ASSERT assert

// Compile asserts based on the compiler
#if defined(__clang__) || defined(__gcc__)
    #define FLOW_STATIC_ASSERT _Static_assert
#else
    #define FLOW_STATIC_ASSERT static_assert
#endif


// Inlining to force compiler to replace funcion call with definition copy
// Also needed to enforce ODR (One-Definition Rule)
#if defined(__clang__) || defined(__gcc__)
    #define FLOW_INLINE inline __attribute__((always_inline))
    #define FLOW_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
    #define FLOW_INLINE __forceinline
    #define FLOW_NOINLINE __declspec(noinline)
#else
    #define FLOW_INLINE inline
    #define FLOW_NOINLINE
#endif


// Meyer's Thread-Safe Singleton Console Logging System
#ifdef FLOW_ENABLE_LOG
    namespace flow{
        struct FLOW_API Logger {
            using SPDLog = std::shared_ptr<spdlog::logger>;

            FLOW_INLINE Logger() {
                m_SPD = spdlog::stdout_color_mt("stdout");
                spdlog::set_level(spdlog::level::trace);
                spdlog::set_pattern("%^[%T] [%L] [#%t] %v%$");
                // eg: [15:34:21] [I] [#14023] Hello!
            }

            FLOW_INLINE static SPDLog& Ref(){
                static Logger logger;
                return logger.m_SPD;
            }

            private:
            SPDLog m_SPD;
        };
    }

    // (...) takes a variable no of args and passes to the underlying logger
    #define FLOW_TRACE(...) flow::Logger::Ref()->trace(__VA_ARGS__)
    #define FLOW_DEBUG(...) flow::Logger::Ref()->debug(__VA_ARGS__)
    #define FLOW_INFO(...) flow::Logger::Ref()->info(__VA_ARGS__)
    #define FLOW_WARN(...) flow::Logger::Ref()->warn(__VA_ARGS__)
    #define FLOW_ERROR(...) flow::Logger::Ref()->error(__VA_ARGS__)
    #define FLOW_CRITICAL(...) flow::Logger::Ref()->critical(__VA_ARGS__)

#else
    #define FLOW_TRACE(...)
    #define FLOW_DEBUG(...)
    #define FLOW_INFO(...)
    #define FLOW_WARN(...)
    #define FLOW_ERROR(...)
    #define FLOW_CRITICAL(...)
#endif

// Generates hash ID for a given type using typeid
namespace flow{
    namespace internal {
        FLOW_INLINE uint32_t nextTypeID() {
            static uint32_t counter = 0;
            return counter++;
        }
    }

    template <typename T>
    FLOW_INLINE uint32_t TypeID() {
        static const uint32_t id = internal::nextTypeID();
        return id;
    }

    FLOW_INLINE uint64_t RandomU64() {
        static std::random_device device;
        static std::mt19937_64 generator(device());
        static std::uniform_int_distribution<uint64_t> distribution;
        return distribution(generator);
    }
}

#define FLOW_DELETE(ptr) do { if (ptr != nullptr){delete ptr; ptr = nullptr;} } while(0)
