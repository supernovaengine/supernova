// https://github.com/kunitoki/LuaBridge3
// Copyright 2021, Lucio Asnaghi
// SPDX-License-Identifier: MIT

// clang-format off

#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

// Begin File: Source/LuaBridge/detail/Config.h


#if !(__cplusplus >= 201703L || (defined(_MSC_VER) && _HAS_CXX17))
#error LuaBridge 3 requires a compliant C++17 compiler, or C++17 has not been enabled !
#endif

#if defined(_MSC_VER)
#if _CPPUNWIND || _HAS_EXCEPTIONS
#define LUABRIDGE_HAS_EXCEPTIONS 1
#else
#define LUABRIDGE_HAS_EXCEPTIONS 0
#endif
#elif defined(__clang__)
#if __EXCEPTIONS && __has_feature(cxx_exceptions)
#define LUABRIDGE_HAS_EXCEPTIONS 1
#else
#define LUABRIDGE_HAS_EXCEPTIONS 0
#endif
#elif defined(__GNUC__)
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
#define LUABRIDGE_HAS_EXCEPTIONS 1
#else
#define LUABRIDGE_HAS_EXCEPTIONS 0
#endif
#endif

#if defined(LUAU_FASTMATH_BEGIN)
#define LUABRIDGE_ON_LUAU 1
#elif defined(LUAJIT_VERSION)
#define LUABRIDGE_ON_LUAJIT 1
#elif defined(LUA_VERSION_NUM)
#define LUABRIDGE_ON_LUA 1
#else
#error "Lua headers must be included prior to LuaBridge ones"
#endif

#if !defined(LUABRIDGE_SAFE_STACK_CHECKS)
#define LUABRIDGE_SAFE_STACK_CHECKS 1
#endif

// End File: Source/LuaBridge/detail/Config.h

// Begin File: Source/LuaBridge/detail/LuaHelpers.h


namespace luabridge {

/**
 * @brief Helper for unused vars.
 */
template <class... Args>
constexpr void unused(Args&&...)
{
}

// These functions and defines are for Luau.
#if LUABRIDGE_ON_LUAU
inline int luaL_ref(lua_State* L, int idx)
{
    assert(idx == LUA_REGISTRYINDEX);

    const int ref = lua_ref(L, -1);

    lua_pop(L, 1);

    return ref;
}

inline void luaL_unref(lua_State* L, int idx, int ref)
{
    unused(idx);

    lua_unref(L, ref);
}

template <class T>
inline void* lua_newuserdata_x(lua_State* L, size_t sz)
{
    return lua_newuserdatadtor(L, sz, [](void* x)
    {
        T* object = static_cast<T*>(x);
        object->~T();
    });
}

inline void lua_pushcfunction_x(lua_State *L, lua_CFunction fn)
{
    lua_pushcfunction(L, fn, "");
}

inline void lua_pushcclosure_x(lua_State* L, lua_CFunction fn, int n)
{
    lua_pushcclosure(L, fn, "", n);
}

#else
using ::luaL_ref;
using ::luaL_unref;

template <class T>
inline void* lua_newuserdata_x(lua_State* L, size_t sz)
{
    return lua_newuserdata(L, sz);
}

inline void lua_pushcfunction_x(lua_State *L, lua_CFunction fn)
{
    lua_pushcfunction(L, fn);
}

inline void lua_pushcclosure_x(lua_State* L, lua_CFunction fn, int n)
{
    lua_pushcclosure(L, fn, n);
}

#endif // LUABRIDGE_ON_LUAU

// These are for Lua versions prior to 5.3.0.
#if LUA_VERSION_NUM < 503
inline lua_Number to_numberx(lua_State* L, int idx, int* isnum)
{
    lua_Number n = lua_tonumber(L, idx);

    if (isnum)
        *isnum = (n != 0 || lua_isnumber(L, idx));

    return n;
}

inline lua_Integer to_integerx(lua_State* L, int idx, int* isnum)
{
    int ok = 0;
    lua_Number n = to_numberx(L, idx, &ok);

    if (ok)
    {
        const auto int_n = static_cast<lua_Integer>(n);
        if (n == static_cast<lua_Number>(int_n))
        {
            if (isnum)
                *isnum = 1;
            
            return int_n;
        }
    }

    if (isnum)
        *isnum = 0;
    
    return 0;
}

#endif // LUA_VERSION_NUM < 503

// These are for Lua versions prior to 5.2.0.
#if LUA_VERSION_NUM < 502
using lua_Unsigned = std::make_unsigned_t<lua_Integer>;

#if ! LUABRIDGE_ON_LUAU
inline int lua_absindex(lua_State* L, int idx)
{
    if (idx > LUA_REGISTRYINDEX && idx < 0)
        return lua_gettop(L) + idx + 1;
    else
        return idx;
}
#endif

inline void lua_rawgetp(lua_State* L, int idx, const void* p)
{
    idx = lua_absindex(L, idx);
    luaL_checkstack(L, 1, "not enough stack slots");
    lua_pushlightuserdata(L, const_cast<void*>(p));
    lua_rawget(L, idx);
}

inline void lua_rawsetp(lua_State* L, int idx, const void* p)
{
    idx = lua_absindex(L, idx);
    luaL_checkstack(L, 1, "not enough stack slots");
    lua_pushlightuserdata(L, const_cast<void*>(p));
    lua_insert(L, -2);
    lua_rawset(L, idx);
}

#define LUA_OPEQ 1
#define LUA_OPLT 2
#define LUA_OPLE 3

inline int lua_compare(lua_State* L, int idx1, int idx2, int op)
{
    switch (op)
    {
    case LUA_OPEQ:
        return lua_equal(L, idx1, idx2);

    case LUA_OPLT:
        return lua_lessthan(L, idx1, idx2);

    case LUA_OPLE:
        return lua_equal(L, idx1, idx2) || lua_lessthan(L, idx1, idx2);

    default:
        return 0;
    }
}

inline int get_length(lua_State* L, int idx)
{
    return static_cast<int>(lua_objlen(L, idx));
}

#else // LUA_VERSION_NUM >= 502
inline int get_length(lua_State* L, int idx)
{
    lua_len(L, idx);
    const int len = static_cast<int>(luaL_checknumber(L, -1));
    lua_pop(L, 1);
    return len;
}

#endif // LUA_VERSION_NUM < 502

#ifndef LUA_OK
#define LUABRIDGE_LUA_OK 0
#else
#define LUABRIDGE_LUA_OK LUA_OK
#endif

/**
 * @brief Helper to throw or return an error code.
 */
template <class T, class ErrorType>
std::error_code throw_or_error_code(ErrorType error)
{
#if LUABRIDGE_HAS_EXCEPTIONS
    throw T(makeErrorCode(error).message().c_str());
#else
    return makeErrorCode(error);
#endif
}

template <class T, class ErrorType>
std::error_code throw_or_error_code(lua_State* L, ErrorType error)
{
#if LUABRIDGE_HAS_EXCEPTIONS
    throw T(L, makeErrorCode(error));
#else
    return unused(L), makeErrorCode(error);
#endif
}

/**
 * @brief Helper to throw or assert.
 */
template <class T, class... Args>
void throw_or_assert(Args&&... args)
{
#if LUABRIDGE_HAS_EXCEPTIONS
    throw T(std::forward<Args>(args)...);
#else
    unused(std::forward<Args>(args)...);
    assert(false);
#endif
}

/**
 * @brief Helper to set unsigned.
 */
template <class T>
void pushunsigned(lua_State* L, T value)
{
    static_assert(std::is_unsigned_v<T>);

    lua_pushinteger(L, static_cast<lua_Integer>(value));
}

/**
 * @brief Helper to convert to integer.
 */
inline lua_Number tonumber(lua_State* L, int idx, int* isnum)
{
#if ! LUABRIDGE_ON_LUAU && LUA_VERSION_NUM > 502
    return lua_tonumberx(L, idx, isnum);
#else
    return to_numberx(L, idx, isnum);
#endif
}

/**
 * @brief Helper to convert to integer.
 */
inline lua_Integer tointeger(lua_State* L, int idx, int* isnum)
{
#if ! LUABRIDGE_ON_LUAU && LUA_VERSION_NUM > 502
    return lua_tointegerx(L, idx, isnum);
#else
    return to_integerx(L, idx, isnum);
#endif
}

/**
 * @brief Register main thread, only supported on 5.1.
 */
inline constexpr char main_thread_name[] = "__luabridge_main_thread";

inline void register_main_thread(lua_State* threadL)
{
#if LUA_VERSION_NUM < 502
    if (threadL == nullptr)
        lua_pushnil(threadL);
    else
        lua_pushthread(threadL);

    lua_setglobal(threadL, main_thread_name);
#else
    unused(threadL);
#endif
}

/**
 * @brief Get main thread, not supported on 5.1.
 */
inline lua_State* main_thread(lua_State* threadL)
{
#if LUA_VERSION_NUM < 502
    lua_getglobal(threadL, main_thread_name);
    if (lua_isthread(threadL, -1))
    {
        auto L = lua_tothread(threadL, -1);
        lua_pop(threadL, 1);
        return L;
    }
    assert(false); // Have you forgot to call luabridge::registerMainThread ?
    lua_pop(threadL, 1);
    return threadL;
#else
    lua_rawgeti(threadL, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
    lua_State* L = lua_tothread(threadL, -1);
    lua_pop(threadL, 1);
    return L;
#endif
}

/**
 * @brief Get a table value, bypassing metamethods.
 */
inline void rawgetfield(lua_State* L, int index, char const* key)
{
    assert(lua_istable(L, index));
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_rawget(L, index);
}

/**
 * @brief Set a table value, bypassing metamethods.
 */
inline void rawsetfield(lua_State* L, int index, char const* key)
{
    assert(lua_istable(L, index));
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_insert(L, -2);
    lua_rawset(L, index);
}

/**
 * @brief Returns true if the value is a full userdata (not light).
 */
[[nodiscard]] inline bool isfulluserdata(lua_State* L, int index)
{
    return lua_isuserdata(L, index) && !lua_islightuserdata(L, index);
}

/**
 * @brief Test lua_State objects for global equality.
 *
 * This can determine if two different lua_State objects really point
 * to the same global state, such as when using coroutines.
 *
 * @note This is used for assertions.
 */
[[nodiscard]] inline bool equalstates(lua_State* L1, lua_State* L2)
{
    return lua_topointer(L1, LUA_REGISTRYINDEX) == lua_topointer(L2, LUA_REGISTRYINDEX);
}

/**
 * @brief Return an aligned pointer of type T.
 */
template <class T>
[[nodiscard]] T* align(void* ptr) noexcept
{
    const auto address = reinterpret_cast<size_t>(ptr);

    const auto offset = address % alignof(T);
    const auto aligned_address = (offset == 0) ? address : (address + alignof(T) - offset);

    return reinterpret_cast<T*>(aligned_address);
}

/**
 * @brief Return the space needed to align the type T on an unaligned address.
 */
template <class T>
[[nodiscard]] constexpr size_t maximum_space_needed_to_align() noexcept
{
    return sizeof(T) + alignof(T) - 1;
}

/**
 * @brief Deallocate lua userdata taking into account alignment.
 */
template <class T>
int lua_deleteuserdata_aligned(lua_State* L)
{
    assert(isfulluserdata(L, 1));

    T* aligned = align<T>(lua_touserdata(L, 1));
    aligned->~T();

    return 0;
}

/**
 * @brief Allocate lua userdata taking into account alignment.
 *
 * Using this instead of lua_newuserdata directly prevents alignment warnings on 64bits platforms.
 */
template <class T, class... Args>
void* lua_newuserdata_aligned(lua_State* L, Args&&... args)
{
#if LUABRIDGE_ON_LUAU
    void* pointer = lua_newuserdatadtor(L, maximum_space_needed_to_align<T>(), [](void* x)
    {
        T* aligned = align<T>(x);
        aligned->~T();
    });
#else
    void* pointer = lua_newuserdata_x<T>(L, maximum_space_needed_to_align<T>());

    lua_newtable(L);
    lua_pushcfunction_x(L, &lua_deleteuserdata_aligned<T>);
    rawsetfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
#endif

    T* aligned = align<T>(pointer);

    new (aligned) T(std::forward<Args>(args)...);

    return pointer;
}

/**
 * @brief Safe error able to walk backwards for error reporting correctly.
 */
inline int raise_lua_error(lua_State *L, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);

    bool pushed_error = false;
    for (int level = 2; level > 0; --level)
    {
        lua_Debug ar;

#if LUABRIDGE_ON_LUAU
        if (lua_getinfo(L, level, "sl", &ar) == 0)
            continue;
#else
        if (lua_getstack(L, level, &ar) == 0 || lua_getinfo(L, "Sl", &ar) == 0)
            continue;
#endif

        if (ar.currentline <= 0)
            continue;

        lua_pushfstring(L, "%s:%d: ", ar.short_src, ar.currentline);
        pushed_error = true;

        break;
    }

    if (! pushed_error)
        lua_pushliteral(L, "");

    lua_pushvfstring(L, fmt, argp);
    va_end(argp);
    lua_concat(L, 2);

#if LUABRIDGE_ON_LUAU
    lua_error(L);
    return 1;
#else
    return lua_error(L);
#endif
}

/**
 * @brief Checks if the value on the stack is a number type and can fit into the corresponding c++ integral type..
 */
template <class U = lua_Integer, class T>
constexpr bool is_integral_representable_by(T value)
{
    constexpr bool same_signedness = (std::is_unsigned_v<T> && std::is_unsigned_v<U>)
        || (!std::is_unsigned_v<T> && !std::is_unsigned_v<U>);

    if constexpr (sizeof(T) == sizeof(U))
    {
        if constexpr (same_signedness)
            return true;

        if constexpr (std::is_unsigned_v<T>)
            return value <= static_cast<T>(std::numeric_limits<U>::max());
        
        return value >= static_cast<T>(std::numeric_limits<U>::min())
            && static_cast<U>(value) <= std::numeric_limits<U>::max();
    }

    if constexpr (sizeof(T) < sizeof(U))
    {
        return static_cast<U>(value) >= std::numeric_limits<U>::min()
            && static_cast<U>(value) <= std::numeric_limits<U>::max();
    }

    if constexpr (std::is_unsigned_v<T>)
        return value <= static_cast<T>(std::numeric_limits<U>::max());

    return value >= static_cast<T>(std::numeric_limits<U>::min())
        && value <= static_cast<T>(std::numeric_limits<U>::max());
}

template <class U = lua_Integer>
bool is_integral_representable_by(lua_State* L, int index)
{
    int isValid = 0;

    const auto value = tointeger(L, index, &isValid);

    return isValid ? is_integral_representable_by<U>(value) : false;
}

/**
 * @brief Checks if the value on the stack is a number type and can fit into the corresponding c++ numerical type..
 */
template <class U = lua_Number, class T>
constexpr bool is_floating_point_representable_by(T value)
{
    if constexpr (sizeof(T) == sizeof(U))
        return true;

    if constexpr (sizeof(T) < sizeof(U))
        return static_cast<U>(value) >= -std::numeric_limits<U>::max()
            && static_cast<U>(value) <= std::numeric_limits<U>::max();

    return value >= static_cast<T>(-std::numeric_limits<U>::max())
        && value <= static_cast<T>(std::numeric_limits<U>::max());
}

template <class U = lua_Number>
bool is_floating_point_representable_by(lua_State* L, int index)
{
    int isValid = 0;

    const auto value = tonumber(L, index, &isValid);

    return isValid ? is_floating_point_representable_by<U>(value) : false;
}

} // namespace luabridge

// End File: Source/LuaBridge/detail/LuaHelpers.h

// Begin File: Source/LuaBridge/detail/Errors.h


namespace luabridge {

//=================================================================================================
namespace detail {

static inline constexpr char error_lua_stack_overflow[] = "stack overflow";

} // namespace detail

//=================================================================================================
/**
 * @brief LuaBridge error codes.
 */
enum class ErrorCode
{
    ClassNotRegistered = 1,

    LuaStackOverflow,

    LuaFunctionCallFailed,

    IntegerDoesntFitIntoLuaInteger,
    
    FloatingPointDoesntFitIntoLuaNumber,
};

//=================================================================================================
namespace detail {
struct ErrorCategory : std::error_category
{
    const char* name() const noexcept override
    {
        return "luabridge";
    }

    std::string message(int ev) const override
    {
        switch (static_cast<ErrorCode>(ev))
        {
        case ErrorCode::ClassNotRegistered:
            return "The class is not registered in LuaBridge";

        case ErrorCode::LuaStackOverflow:
            return "The lua stack has overflow";

        case ErrorCode::LuaFunctionCallFailed:
            return "The lua function invocation raised an error";

        case ErrorCode::IntegerDoesntFitIntoLuaInteger:
            return "The native integer can't fit inside a lua integer";

        case ErrorCode::FloatingPointDoesntFitIntoLuaNumber:
            return "The native floating point can't fit inside a lua number";
                
        default:
            return "Unknown error";
        }
    }

    static const ErrorCategory& getInstance() noexcept
    {
        static ErrorCategory category;
        return category;
    }
};
} // namespace detail

//=================================================================================================
/**
 * @brief Construct an error code from the error enum.
 */
inline std::error_code makeErrorCode(ErrorCode e)
{
  return { static_cast<int>(e), detail::ErrorCategory::getInstance() };
}

} // namespace luabridge

namespace std {
template <> struct is_error_code_enum<luabridge::ErrorCode> : true_type {};
} // namespace std

// End File: Source/LuaBridge/detail/Errors.h

// Begin File: Source/LuaBridge/detail/LuaException.h


namespace luabridge {

//================================================================================================
class LuaException : public std::exception
{
public:
    //=============================================================================================
    /**
     * @brief Construct a LuaException after a lua_pcall().
     *
     * Assumes the error string is on top of the stack, but provides a generic error message otherwise.
     */
    LuaException(lua_State* L, std::error_code code)
        : m_L(L)
        , m_code(code)
    {
    }

    ~LuaException() noexcept override
    {
    }

    //=============================================================================================
    /**
     * @brief Return the error message.
     */
    const char* what() const noexcept override
    {
        return m_what.c_str();
    }

    //=============================================================================================
    /**
     * @brief Throw an exception or raises a luaerror when exceptions are disabled.
     */
    static void raise(lua_State* L, std::error_code code)
    {
        assert(areExceptionsEnabled());

#if LUABRIDGE_HAS_EXCEPTIONS
        throw LuaException(L, code, FromLua{});
#else
        unused(L, code);

        std::abort();
#endif
    }

    //=============================================================================================
    /**
     * @brief Check if exceptions are enabled.
     */
    static bool areExceptionsEnabled() noexcept
    {
        return exceptionsEnabled();
    }

    /**
     * @brief Initializes error handling.
     *
     * Subsequent Lua errors are translated to C++ exceptions, or logging and abort if exceptions are disabled.
     */
    static void enableExceptions(lua_State* L) noexcept
    {
        exceptionsEnabled() = true;

#if LUABRIDGE_HAS_EXCEPTIONS && LUABRIDGE_ON_LUAJIT
        lua_pushlightuserdata(L, (void*)luajitWrapperCallback);
        luaJIT_setmode(L, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);
        lua_pop(L, 1);
#endif

#if LUABRIDGE_ON_LUAU
        auto callbacks = lua_callbacks(L);
        callbacks->panic = +[](lua_State* L, int) { panicHandlerCallback(L); };
#else
        lua_atpanic(L, panicHandlerCallback);
#endif
    }

    //=============================================================================================
    /**
     * @brief Retrieve the lua_State associated with the exception.
     *
     * @return A Lua state.
     */
    lua_State* state() const { return m_L; }

private:
    struct FromLua {};

    LuaException(lua_State* L, std::error_code code, FromLua)
        : m_L(L)
        , m_code(code)
    {
        whatFromStack();
    }

    void whatFromStack()
    {
        std::stringstream ss;

        const char* errorText = nullptr;

        if (lua_gettop(m_L) > 0)
        {
            errorText = lua_tostring(m_L, -1);
            lua_pop(m_L, 1);
        }

        ss << (errorText ? errorText : "Unknown error") << " (code=" << m_code.message() << ")";

        m_what = std::move(ss).str();
    }

    static int panicHandlerCallback(lua_State* L)
    {
#if LUABRIDGE_HAS_EXCEPTIONS
        throw LuaException(L, makeErrorCode(ErrorCode::LuaFunctionCallFailed), FromLua{});
#else
        unused(L);

        std::abort();
#endif
    }

#if LUABRIDGE_HAS_EXCEPTIONS && LUABRIDGE_ON_LUAJIT
    static int luajitWrapperCallback(lua_State* L, lua_CFunction f)
    {
        try
        {
            return f(L);
        }
        catch (const std::exception& e)
        {
            lua_pushstring(L, e.what());
            return lua_error(L);
        }
    }
#endif

    static bool& exceptionsEnabled()
    {
        static bool areExceptionsEnabled = false;
        return areExceptionsEnabled;
    }

    lua_State* m_L = nullptr;
    std::error_code m_code;
    std::string m_what;
};

//=================================================================================================
/**
 * @brief Initializes error handling using C++ exceptions.
 *
 * Subsequent Lua errors are translated to C++ exceptions. It aborts the application if called when no exceptions.
 */
inline void enableExceptions(lua_State* L) noexcept
{
#if LUABRIDGE_HAS_EXCEPTIONS
    LuaException::enableExceptions(L);
#else
    unused(L);

    assert(false); // Never call this function when exceptions are not enabled.
#endif
}

} // namespace luabridge

// End File: Source/LuaBridge/detail/LuaException.h

// Begin File: Source/LuaBridge/detail/ClassInfo.h


namespace luabridge {
namespace detail {

//=================================================================================================
/**
 * @brief A unique key for a type name in a metatable.
 */
inline const void* getTypeKey() noexcept
{
    return reinterpret_cast<void*>(0x71);
}

//=================================================================================================
/**
 * @brief The key of a const table in another metatable.
 */
inline const void* getConstKey() noexcept
{
    return reinterpret_cast<void*>(0xc07);
}

//=================================================================================================
/**
 * @brief The key of a class table in another metatable.
 */
inline const void* getClassKey() noexcept
{
    return reinterpret_cast<void*>(0xc1a);
}

//=================================================================================================
/**
 * @brief The key of a propget table in another metatable.
 */
inline const void* getPropgetKey() noexcept
{
    return reinterpret_cast<void*>(0x6e7);
}

//=================================================================================================
/**
 * @brief The key of a propset table in another metatable.
 */
inline const void* getPropsetKey() noexcept
{
    return reinterpret_cast<void*>(0x5e7);
}

//=================================================================================================
/**
 * @brief The key of a static table in another metatable.
 */
inline const void* getStaticKey() noexcept
{
    return reinterpret_cast<void*>(0x57a);
}

//=================================================================================================
/**
 * @brief The key of a parent table in another metatable.
 */
inline const void* getParentKey() noexcept
{
    return reinterpret_cast<void*>(0xdad);
}

//=================================================================================================
/**
 * @brief Get the key for the static table in the Lua registry.
 *
 * The static table holds the static data members, static properties, and static member functions for a class.
 */
template <class T>
const void* getStaticRegistryKey() noexcept
{
    static char value;
    return std::addressof(value);
}

//=================================================================================================
/**
 * @brief Get the key for the class table in the Lua registry.
 *
 * The class table holds the data members, properties, and member functions of a class. Read-only data and properties, and const
 * member functions are also placed here (to save a lookup in the const table).
 */
template <class T>
const void* getClassRegistryKey() noexcept
{
    static char value;
    return std::addressof(value);
}

//=================================================================================================
/**
 * @brief Get the key for the const table in the Lua registry.
 *
 * The const table holds read-only data members and properties, and const member functions of a class.
 */
template <class T>
const void* getConstRegistryKey() noexcept
{
    static char value;
    return std::addressof(value);
}

} // namespace detail
} // namespace luabridge

// End File: Source/LuaBridge/detail/ClassInfo.h

// Begin File: Source/LuaBridge/detail/TypeTraits.h


namespace luabridge {

//=================================================================================================
/**
 * @brief Container traits.
 *
 * Unspecialized ContainerTraits has the isNotContainer typedef for SFINAE. All user defined containers must supply an appropriate
 * specialization for ContinerTraits (without the alias isNotContainer). The containers that come with LuaBridge also come with the
 * appropriate ContainerTraits specialization.
 *
 * @note See the corresponding declaration for details.
 *
 * A specialization of ContainerTraits for some generic type ContainerType looks like this:
 *
 * @code
 *
 *  template <class T>
 *  struct ContainerTraits<ContainerType<T>>
 *  {
 *    using Type = T;
 *
 *    static ContainerType<T> construct(T* c)
 *    {
 *      return c; // Implementation-dependent on ContainerType
 *    }
 *
 *    static T* get(const ContainerType<T>& c)
 *    {
 *      return c.get(); // Implementation-dependent on ContainerType
 *    }
 *  };
 *
 * @endcode
 */
template <class T>
struct ContainerTraits
{
    using IsNotContainer = bool;

    using Type = T;
};

/**
 * @brief Register shared_ptr support as container.
 *
 * @tparam T Class that is hold by the shared_ptr, must inherit from std::enable_shared_from_this.
 */
template <class T>
struct ContainerTraits<std::shared_ptr<T>>
{
    static_assert(std::is_base_of_v<std::enable_shared_from_this<T>, T>);
    
    using Type = T;

    static std::shared_ptr<T> construct(T* t)
    {
        return t->shared_from_this();
    }

    static T* get(const std::shared_ptr<T>& c)
    {
        return c.get();
    }
};

namespace detail {

//=================================================================================================
/**
 * @brief Determine if type T is a container.
 *
 * To be considered a container, there must be a specialization of ContainerTraits with the required fields.
 */
template <class T>
class IsContainer
{
private:
    typedef char yes[1]; // sizeof (yes) == 1
    typedef char no[2]; // sizeof (no)  == 2

    template <class C>
    static constexpr no& test(typename C::IsNotContainer*);

    template <class>
    static constexpr yes& test(...);

public:
    static constexpr bool value = sizeof(test<ContainerTraits<T>>(0)) == sizeof(yes);
};

} // namespace detail
} // namespace luabridge

// End File: Source/LuaBridge/detail/TypeTraits.h

// Begin File: Source/LuaBridge/detail/Userdata.h


namespace luabridge {
namespace detail {

//=================================================================================================
/**
 * @brief Return the identity pointer for our lightuserdata tokens.
 *
 * Because of Lua's dynamic typing and our improvised system of imposing C++ class structure, there is the possibility that executing
 * scripts may knowingly or unknowingly cause invalid data to get passed to the C functions created by LuaBridge.
 *
 * In particular, our security model addresses the following:
 *
 *   1. Scripts cannot create a userdata (ignoring the debug lib).
 *
 *   2. Scripts cannot create a lightuserdata (ignoring the debug lib).
 *
 *   3. Scripts cannot set the metatable on a userdata.
 */

/**
 * @brief Interface to a class pointer retrievable from a userdata.
 */
class Userdata
{
private:
    //=============================================================================================
    /**
     * @brief Validate and retrieve a Userdata on the stack.
     *
     * The Userdata must exactly match the corresponding class table or const table, or else a Lua error is raised. This is used for the
     * __gc metamethod.
     */
    static Userdata* getExactClass(lua_State* L, int index, const void* classKey)
    {
        return (void)classKey, static_cast<Userdata*>(lua_touserdata(L, lua_absindex(L, index)));
    }

    //=============================================================================================
    /**
     * @brief Validate and retrieve a Userdata on the stack.
     *
     * The Userdata must be derived from or the same as the given base class, identified by the key. If canBeConst is false, generates
     * an error if the resulting Userdata represents to a const object. We do the type check first so that the error message is informative.
     */
    static Userdata* getClass(lua_State* L,
                              int index,
                              const void* registryConstKey,
                              const void* registryClassKey,
                              bool canBeConst)
    {
        index = lua_absindex(L, index);

        lua_getmetatable(L, index); // Stack: object metatable (ot) | nil
        if (!lua_istable(L, -1))
        {
            lua_rawgetp(L, LUA_REGISTRYINDEX, registryClassKey); // Stack: registry metatable (rt) | nil
            return throwBadArg(L, index);
        }

        lua_rawgetp(L, -1, getConstKey()); // Stack: ot | nil, const table (co) | nil
        assert(lua_istable(L, -1) || lua_isnil(L, -1));

        // If const table is NOT present, object is const. Use non-const registry table
        // if object cannot be const, so constness validation is done automatically.
        // E.g. nonConstFn (constObj)
        // -> canBeConst = false, isConst = true
        // -> 'Class' registry table, 'const Class' object table
        // -> 'expected Class, got const Class'
        bool isConst = lua_isnil(L, -1); // Stack: ot | nil, nil, rt
        if (isConst && canBeConst)
        {
            lua_rawgetp(L, LUA_REGISTRYINDEX, registryConstKey); // Stack: ot, nil, rt
        }
        else
        {
            lua_rawgetp(L, LUA_REGISTRYINDEX, registryClassKey); // Stack: ot, co, rt
        }

        lua_insert(L, -3); // Stack: rt, ot, co | nil
        lua_pop(L, 1); // Stack: rt, ot

        for (;;)
        {
            if (lua_rawequal(L, -1, -2)) // Stack: rt, ot
            {
                lua_pop(L, 2); // Stack: -
                return static_cast<Userdata*>(lua_touserdata(L, index));
            }

            // Replace current metatable with it's base class.
            lua_rawgetp(L, -1, getParentKey()); // Stack: rt, ot, parent ot (pot) | nil

            if (lua_isnil(L, -1)) // Stack: rt, ot, nil
            {
                // Drop the object metatable because it may be some parent metatable
                lua_pop(L, 2); // Stack: rt
                return throwBadArg(L, index);
            }

            lua_remove(L, -2); // Stack: rt, pot
        }

        // no return
    }

    static bool isInstance(lua_State* L, int index, const void* registryClassKey)
    {
        index = lua_absindex(L, index);

        int result = lua_getmetatable(L, index); // Stack: object metatable (ot) | nothing
        if (result == 0)
            return false; // Nothing was pushed on the stack

        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1); // Stack: -
            return false;
        }

        lua_rawgetp(L, LUA_REGISTRYINDEX, registryClassKey); // Stack: ot, rt
        lua_insert(L, -2); // Stack: rt, ot

        for (;;)
        {
            if (lua_rawequal(L, -1, -2)) // Stack: rt, ot
            {
                lua_pop(L, 2); // Stack: -
                return true;
            }

            // Replace current metatable with it's base class.
            lua_rawgetp(L, -1, getParentKey()); // Stack: rt, ot, parent ot (pot) | nil

            if (lua_isnil(L, -1)) // Stack: rt, ot, nil
            {
                lua_pop(L, 3); // Stack: -
                return false;
            }

            lua_remove(L, -2); // Stack: rt, pot
        }
    }

    static Userdata* throwBadArg(lua_State* L, int index)
    {
        assert(lua_istable(L, -1) || lua_isnil(L, -1)); // Stack: rt | nil

        const char* expected = 0;
        if (lua_isnil(L, -1)) // Stack: nil
        {
            expected = "unregistered class";
        }
        else
        {
            lua_rawgetp(L, -1, getTypeKey()); // Stack: rt, registry type
            expected = lua_tostring(L, -1);
        }

        const char* got = 0;
        if (lua_isuserdata(L, index))
        {
            lua_getmetatable(L, index); // Stack: ..., ot | nil
            if (lua_istable(L, -1)) // Stack: ..., ot
            {
                lua_rawgetp(L, -1, getTypeKey()); // Stack: ..., ot, object type | nil
                if (lua_isstring(L, -1))
                {
                    got = lua_tostring(L, -1);
                }
            }
        }

        if (!got)
        {
            got = lua_typename(L, lua_type(L, index));
        }

        luaL_argerror(L, index, lua_pushfstring(L, "%s expected, got %s", expected, got));
        return nullptr;
    }

public:
    virtual ~Userdata() {}

    //=============================================================================================
    /**
     * @brief Returns the Userdata* if the class on the Lua stack matches.
     *
     * If the class does not match, a Lua error is raised.
     *
     * @tparam T  A registered user class.
     *
     * @param L A Lua state.
     * @param index The index of an item on the Lua stack.
     *
     * @return A userdata pointer if the class matches.
     */
    template <class T>
    static Userdata* getExact(lua_State* L, int index)
    {
        return getExactClass(L, index, detail::getClassRegistryKey<T>());
    }

    //=============================================================================================
    /**
     * @brief Get a pointer to the class from the Lua stack.
     *
     * If the object is not the class or a subclass, or it violates the const-ness, a Lua error is raised.
     *
     * @tparam T A registered user class.
     *
     * @param L A Lua state.
     * @param index The index of an item on the Lua stack.
     * @param canBeConst TBD
     *
     * @return A pointer if the class and constness match.
     */
    template <class T>
    static T* get(lua_State* L, int index, bool canBeConst)
    {
        if (lua_isnil(L, index))
            return nullptr;

        return static_cast<T*>(getClass(L,
                                        index,
                                        detail::getConstRegistryKey<T>(),
                                        detail::getClassRegistryKey<T>(),
                                        canBeConst)
                                   ->getPointer());
    }

    template <class T>
    static bool isInstance(lua_State* L, int index)
    {
        return isInstance(L, index, detail::getClassRegistryKey<T>());
    }

protected:
    Userdata() = default;

    /**
     * @brief Get an untyped pointer to the contained class.
     */
    void* getPointer() const noexcept
    {
        return m_p;
    }

    void* m_p = nullptr; // subclasses must set this
};

//=================================================================================================
/**
 * @brief Wraps a class object stored in a Lua userdata.
 *
 * The lifetime of the object is managed by Lua. The object is constructed inside the userdata using placement new.
 */
template <class T>
class UserdataValue : public Userdata
{
public:
    UserdataValue(const UserdataValue&) = delete;
    UserdataValue operator=(const UserdataValue&) = delete;

    ~UserdataValue()
    {
        if (getPointer() != nullptr)
        {
            getObject()->~T();
        }
    }

    /**
     * @brief Push a T via placement new.
     *
     * The caller is responsible for calling placement new using the returned uninitialized storage.
     *
     * @param L A Lua state.
     *
     * @return An object referring to the newly created userdata value.
     */
    static UserdataValue<T>* place(lua_State* L, std::error_code& ec)
    {
        auto* ud = new (lua_newuserdata_x<UserdataValue<T>>(L, sizeof(UserdataValue<T>))) UserdataValue<T>();

        lua_rawgetp(L, LUA_REGISTRYINDEX, detail::getClassRegistryKey<T>());

        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1); // possibly: a nil

            ud->~UserdataValue<T>();

            ec = throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);

            return nullptr;
        }

        lua_setmetatable(L, -2);

        return ud;
    }

    /**
     * @brief Push T via copy construction from U.
     *
     * @tparam U A container type.
     *
     * @param L A Lua state.
     * @param u A container object l-value reference.
     * @param ec Error code that will be set in case of failure to push on the lua stack.
     */
    template <class U>
    static bool push(lua_State* L, const U& u, std::error_code& ec)
    {
        auto* ud = place(L, ec);

        if (!ud)
            return false;

        new (ud->getObject()) U(u);

        ud->commit();

        return true;
    }

    /**
     * @brief Push T via move construction from U.
     *
     * @tparam U A container type.
     *
     * @param L A Lua state.
     * @param u A container object r-value reference.
     * @param ec Error code that will be set in case of failure to push on the lua stack.
     */
    template <class U>
    static bool push(lua_State* L, U&& u, std::error_code& ec)
    {
        auto* ud = place(L, ec);

        if (!ud)
            return false;

        new (ud->getObject()) U(std::move(u));

        ud->commit();

        return true;
    }

    /**
     * @brief Confirm object construction.
     */
    void commit() noexcept
    {
        m_p = getObject();
    }

    T* getObject() noexcept
    {
        // If this fails to compile it means you forgot to provide
        // a Container specialization for your container!
        return reinterpret_cast<T*>(&m_storage);
    }

private:
    /**
     * @brief Used for placement construction.
     */
    UserdataValue() noexcept
        : Userdata()
    {
    }

    std::aligned_storage_t<sizeof(T), alignof(T)> m_storage;
};

//=================================================================================================
/**
 * @brief Wraps a pointer to a class object inside a Lua userdata.
 *
 * The lifetime of the object is managed by C++.
 */
class UserdataPtr : public Userdata
{
public:
    UserdataPtr(const UserdataPtr&) = delete;
    UserdataPtr operator=(const UserdataPtr&) = delete;

    /**
     * @brief Push non-const pointer to object.
     *
     * @tparam T A user registered class.
     *
     * @param L A Lua state.
     * @param p A pointer to the user class instance.
     * @param ec Error code that will be set in case of failure to push on the lua stack.
     */
    template <class T>
    static bool push(lua_State* L, T* p, std::error_code& ec)
    {
        if (p)
            return push(L, p, getClassRegistryKey<T>(), ec);

        lua_pushnil(L);
        return true;
    }

    /**
     * @brief Push const pointer to object.
     *
     * @tparam T A user registered class.
     *
     * @param L A Lua state.
     * @param p A pointer to the user class instance.
     * @param ec Error code that will be set in case of failure to push on the lua stack.
     */
    template <class T>
    static bool push(lua_State* L, const T* p, std::error_code& ec)
    {
        if (p)
            return push(L, p, getConstRegistryKey<T>(), ec);

        lua_pushnil(L);
        return true;
    }

private:
    /**
     * @brief Push a pointer to object using metatable key.
     */
    static bool push(lua_State* L, const void* p, const void* key, std::error_code& ec)
    {
        auto* ptr = new (lua_newuserdata_x<UserdataPtr>(L, sizeof(UserdataPtr))) UserdataPtr(const_cast<void*>(p));

        lua_rawgetp(L, LUA_REGISTRYINDEX, key);

        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1); // possibly: a nil

            ptr->~UserdataPtr();

            ec = throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);

            return false;
        }

        lua_setmetatable(L, -2);

        return true;
    }

    explicit UserdataPtr(void* p)
    {
        // Can't construct with a null pointer!
        assert(p != nullptr);

        m_p = p;
    }
};

//============================================================================
/**
 * @brief Wraps a container that references a class object.
 *
 * The template argument C is the container type, ContainerTraits must be specialized on C or else a compile error will result.
 */
template <class C>
class UserdataShared : public Userdata
{
public:
    UserdataShared(const UserdataShared&) = delete;
    UserdataShared& operator=(const UserdataShared&) = delete;

    ~UserdataShared() = default;

    /**
     * @brief Construct from a container to the class or a derived class.
     *
     * @tparam U A container type.
     *
     * @param  u A container object reference.
     */
    template <class U>
    explicit UserdataShared(const U& u) : m_c(u)
    {
        m_p = const_cast<void*>(reinterpret_cast<const void*>((ContainerTraits<C>::get(m_c))));
    }

    /**
     * @brief Construct from a pointer to the class or a derived class.
     *
     * @tparam U A container type.
     *
     * @param u A container object pointer.
     */
    template <class U>
    explicit UserdataShared(U* u) : m_c(u)
    {
        m_p = const_cast<void*>(reinterpret_cast<const void*>((ContainerTraits<C>::get(m_c))));
    }

private:
    C m_c;
};

//=================================================================================================
/**
 * @brief SFINAE helper for non-const objects.
 */
template <class C, bool MakeObjectConst>
struct UserdataSharedHelper
{
    using T = std::remove_const_t<typename ContainerTraits<C>::Type>;

    static bool push(lua_State* L, const C& c, std::error_code& ec)
    {
        if (ContainerTraits<C>::get(c) != nullptr)
        {
            auto* us = new (lua_newuserdata_x<UserdataShared<C>>(L, sizeof(UserdataShared<C>))) UserdataShared<C>(c);

            lua_rawgetp(L, LUA_REGISTRYINDEX, getClassRegistryKey<T>());

            if (!lua_istable(L, -1))
            {
                lua_pop(L, 1); // possibly: a nil

                us->~UserdataShared<C>();

                ec = throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);

                return false;
            }

            lua_setmetatable(L, -2);
        }
        else
        {
            lua_pushnil(L);
        }

        return true;
    }

    static bool push(lua_State* L, T* t, std::error_code& ec)
    {
        if (t)
        {
            auto* us = new (lua_newuserdata_x<UserdataShared<C>>(L, sizeof(UserdataShared<C>))) UserdataShared<C>(t);

            lua_rawgetp(L, LUA_REGISTRYINDEX, getClassRegistryKey<T>());

            if (!lua_istable(L, -1))
            {
                lua_pop(L, 1); // possibly: a nil

                us->~UserdataShared<C>();

                ec = throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);

                return false;
            }

            lua_setmetatable(L, -2);
        }
        else
        {
            lua_pushnil(L);
        }

        return true;
    }
};

/**
 * @brief SFINAE helper for const objects.
 */
template <class C>
struct UserdataSharedHelper<C, true>
{
    using T = std::remove_const_t<typename ContainerTraits<C>::Type>;

    static bool push(lua_State* L, const C& c, std::error_code& ec)
    {
        if (ContainerTraits<C>::get(c) != nullptr)
        {
            auto* us = new (lua_newuserdata_x<UserdataShared<C>>(L, sizeof(UserdataShared<C>))) UserdataShared<C>(c);

            lua_rawgetp(L, LUA_REGISTRYINDEX, getConstRegistryKey<T>());

            if (!lua_istable(L, -1))
            {
                lua_pop(L, 1); // possibly: a nil

                us->~UserdataShared<C>();

                ec = throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);

                return false;
            }

            lua_setmetatable(L, -2);
        }
        else
        {
            lua_pushnil(L);
        }

        return true;
    }

    static bool push(lua_State* L, T* t, std::error_code& ec)
    {
        if (t)
        {
            auto* us = new (lua_newuserdata_x<UserdataShared<C>>(L, sizeof(UserdataShared<C>))) UserdataShared<C>(t);

            lua_rawgetp(L, LUA_REGISTRYINDEX, getConstRegistryKey<T>());

            if (!lua_istable(L, -1))
            {
                lua_pop(L, 1); // possibly: a nil

                us->~UserdataShared<C>();

                ec = throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);

                return false;
            }

            lua_setmetatable(L, -2);
        }
        else
        {
            lua_pushnil(L);
        }

        return true;
    }
};

//=================================================================================================
/**
 * @brief Pass by container.
 *
 * The container controls the object lifetime. Typically this will be a lifetime shared by C++ and Lua using a reference count. Because of type
 * erasure, containers like std::shared_ptr will not work, unless the type hold by them is derived from std::enable_shared_from_this.
 */
template <class T, bool ByContainer>
struct StackHelper
{
    using ReturnType = std::remove_const_t<typename ContainerTraits<T>::Type>;

    static bool push(lua_State* L, const T& t, std::error_code& ec)
    {
        return UserdataSharedHelper<T, std::is_const_v<typename ContainerTraits<T>::Type>>::push(L, t, ec);
    }

    static T get(lua_State* L, int index)
    {
        return ContainerTraits<T>::construct(Userdata::get<ReturnType>(L, index, true));
    }
};

/**
 * @brief Pass by value.
 *
 * Lifetime is managed by Lua. A C++ function which accesses a pointer or reference to an object outside the activation record in which it was
 * retrieved may result in undefined behavior if Lua garbage collected it.
 */
template <class T>
struct StackHelper<T, false>
{
    static bool push(lua_State* L, const T& t, std::error_code& ec)
    {
        return UserdataValue<T>::push(L, t, ec);
    }

    static bool push(lua_State* L, T&& t, std::error_code& ec)
    {
        return UserdataValue<T>::push(L, std::move(t), ec);
    }

    static T const& get(lua_State* L, int index)
    {
        return *Userdata::get<T>(L, index, true);
    }
};

//=================================================================================================
/**
 * @brief Lua stack conversions for pointers and references to class objects.
 *
 * Lifetime is managed by C++. Lua code which remembers a reference to the value may result in undefined behavior if C++ destroys the object.
 * The handling of the const and volatile qualifiers happens in UserdataPtr.
 */
template <class C, bool ByContainer>
struct RefStackHelper
{
    using ReturnType = C;
    using T = std::remove_const_t<typename ContainerTraits<C>::Type>;

    static bool push(lua_State* L, const C& t, std::error_code& ec)
    {
        return UserdataSharedHelper<C, std::is_const_v<typename ContainerTraits<C>::Type>>::push(L, t, ec);
    }

    static ReturnType get(lua_State* L, int index)
    {
        return ContainerTraits<C>::construct(Userdata::get<T>(L, index, true));
    }
};

template <class T>
struct RefStackHelper<T, false>
{
    using ReturnType = T&;

    static bool push(lua_State* L, const T& t, std::error_code& ec)
    {
        return UserdataPtr::push(L, &t, ec);
    }

    static ReturnType get(lua_State* L, int index)
    {
        T* t = Userdata::get<T>(L, index, true);

        if (!t)
            luaL_error(L, "nil passed to reference");

        return *t;
    }
};

//=================================================================================================
/**
 * @brief Trait class that selects whether to return a user registered class object by value or by reference.
 */
template <class T, class Enable = void>
struct UserdataGetter
{
    using ReturnType = T*;

    static ReturnType get(lua_State* L, int index)
    {
        return Userdata::get<T>(L, index, false);
    }
};

template <class T>
struct UserdataGetter<T, std::void_t<T (*)()>>
{
    using ReturnType = T;

    static ReturnType get(lua_State* L, int index)
    {
        return StackHelper<T, IsContainer<T>::value>::get(L, index);
    }
};

} // namespace detail

//=================================================================================================
/**
 * @brief Lua stack conversions for class objects passed by value.
 */
template <class T, class = void>
struct Stack
{
    using IsUserdata = void;

    using Getter = detail::UserdataGetter<T>;
    using ReturnType = typename Getter::ReturnType;

    [[nodiscard]] static bool push(lua_State* L, const T& value, std::error_code& ec)
    {
        return detail::StackHelper<T, detail::IsContainer<T>::value>::push(L, value, ec);
    }

    [[nodiscard]] static bool push(lua_State* L, T&& value, std::error_code& ec)
    {
        return detail::StackHelper<T, detail::IsContainer<T>::value>::push(L, std::move(value), ec);
    }

    [[nodiscard]] static ReturnType get(lua_State* L, int index)
    {
        return Getter::get(L, index);
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return detail::Userdata::isInstance<T>(L, index);
    }
};

namespace detail {

//=================================================================================================
/**
 * @brief Trait class indicating whether the parameter type must be a user registered class.
 *
 * The trait checks the existence of member type Stack::IsUserdata specialization for detection.
 */
template <class T, class Enable = void>
struct IsUserdata : std::false_type
{
};

template <class T>
struct IsUserdata<T, std::void_t<typename Stack<T>::IsUserdata>> : std::true_type
{
};

//=================================================================================================
/**
 * @brief Trait class that selects a specific push/get implementation for userdata.
 */
template <class T, bool IsUserdata>
struct StackOpSelector;

// pointer
template <class T>
struct StackOpSelector<T*, true>
{
    using ReturnType = T*;

    static bool push(lua_State* L, T* value, std::error_code& ec) { return UserdataPtr::push(L, value, ec); }

    static T* get(lua_State* L, int index) { return Userdata::get<T>(L, index, false); }

    static bool isInstance(lua_State* L, int index) { return Userdata::isInstance<T>(L, index); }
};

// pointer to const
template <class T>
struct StackOpSelector<const T*, true>
{
    using ReturnType = const T*;

    static bool push(lua_State* L, const T* value, std::error_code& ec) { return UserdataPtr::push(L, value, ec); }

    static const T* get(lua_State* L, int index) { return Userdata::get<T>(L, index, true); }

    static bool isInstance(lua_State* L, int index) { return Userdata::isInstance<T>(L, index); }
};

// l-value reference
template <class T>
struct StackOpSelector<T&, true>
{
    using Helper = RefStackHelper<T, IsContainer<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    static bool push(lua_State* L, T& value, std::error_code& ec) { return UserdataPtr::push(L, &value, ec); }

    static ReturnType get(lua_State* L, int index) { return Helper::get(L, index); }

    static bool isInstance(lua_State* L, int index) { return Userdata::isInstance<T>(L, index); }
};

// l-value reference to const
template <class T>
struct StackOpSelector<const T&, true>
{
    using Helper = RefStackHelper<T, IsContainer<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    static bool push(lua_State* L, const T& value, std::error_code& ec) { return Helper::push(L, value, ec); }

    static ReturnType get(lua_State* L, int index) { return Helper::get(L, index); }

    static bool isInstance(lua_State* L, int index) { return Userdata::isInstance<T>(L, index); }
};

} // namespace detail
} // namespace luabridge

// End File: Source/LuaBridge/detail/Userdata.h

// Begin File: Source/LuaBridge/detail/Stack.h


namespace luabridge {

//=================================================================================================
/**
 * @brief Lua stack traits for C++ types.
 *
 * @tparam T A C++ type.
 */
template <class T, class>
struct Stack;

//=================================================================================================
/**
 * @brief Specialization for void type.
 */
template <>
struct Stack<void>
{
    [[nodiscard]] static bool push(lua_State*, std::error_code&)
    {
        return true;
    }
};

//=================================================================================================
/**
 * @brief Specialization for nullptr_t.
 */
template <>
struct Stack<std::nullptr_t>
{
    [[nodiscard]] static bool push(lua_State* L, std::nullptr_t, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif
        
        lua_pushnil(L);
        return true;
    }

    [[nodiscard]] static std::nullptr_t get(lua_State*, int)
    {
        return nullptr;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_isnil(L, index);
    }
};

//=================================================================================================
/**
 * @brief Receive the lua_State* as an argument.
 */
template <>
struct Stack<lua_State*>
{
    [[nodiscard]] static lua_State* get(lua_State* L, int)
    {
        return L;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for a lua_CFunction.
 */
template <>
struct Stack<lua_CFunction>
{
    [[nodiscard]] static bool push(lua_State* L, lua_CFunction f, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        lua_pushcfunction_x(L, f);
        return true;
    }

    [[nodiscard]] static lua_CFunction get(lua_State* L, int index)
    {
        return lua_tocfunction(L, index);
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_iscfunction(L, index);
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `bool`.
 */
template <>
struct Stack<bool>
{
    [[nodiscard]] static bool push(lua_State* L, bool value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        lua_pushboolean(L, value ? 1 : 0);
        return true;
    }

    [[nodiscard]] static bool get(lua_State* L, int index)
    {
        return lua_toboolean(L, index) ? true : false;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_isboolean(L, index);
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `std::byte`.
 */
template <>
struct Stack<std::byte>
{
    static_assert(sizeof(std::byte) < sizeof(lua_Integer));

    [[nodiscard]] static bool push(lua_State* L, std::byte value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        pushunsigned(L, std::to_integer<std::make_unsigned_t<lua_Integer>>(value));
        return true;
    }

    [[nodiscard]] static std::byte get(lua_State* L, int index)
    {
        return static_cast<std::byte>(luaL_checkinteger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned char>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `char`.
 */
template <>
struct Stack<char>
{
    [[nodiscard]] static bool push(lua_State* L, char value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        lua_pushlstring(L, &value, 1);
        return true;
    }

    [[nodiscard]] static char get(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TSTRING)
        {
            std::size_t length = 0;
            const char* str = lua_tolstring(L, index, &length);

            if (str != nullptr && length >= 1)
                return str[0];
        }

        return char(0);
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TSTRING)
        {
            std::size_t len;
            luaL_checklstring(L, index, &len);
            return len == 1;
        }
        
        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `int8_t`.
 */
template <>
struct Stack<int8_t>
{
    static_assert(sizeof(int8_t) < sizeof(lua_Integer));

    [[nodiscard]] static bool push(lua_State* L, int8_t value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return true;
    }

    [[nodiscard]] static int8_t get(lua_State* L, int index)
    {
        return static_cast<int8_t>(luaL_checkinteger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<int8_t>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `unsigned char`.
 */
template <>
struct Stack<unsigned char>
{
    static_assert(sizeof(unsigned char) < sizeof(lua_Integer));

    [[nodiscard]] static bool push(lua_State* L, unsigned char value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        pushunsigned(L, value);
        return true;
    }

    [[nodiscard]] static unsigned char get(lua_State* L, int index)
    {
        return static_cast<unsigned char>(luaL_checkinteger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned char>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `short`.
 */
template <>
struct Stack<short>
{
    static_assert(sizeof(short) < sizeof(lua_Integer));

    [[nodiscard]] static bool push(lua_State* L, short value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return true;
    }

    [[nodiscard]] static short get(lua_State* L, int index)
    {
        return static_cast<short>(luaL_checkinteger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<short>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `unsigned short`.
 */
template <>
struct Stack<unsigned short>
{
    static_assert(sizeof(unsigned short) < sizeof(lua_Integer));
    
    [[nodiscard]] static bool push(lua_State* L, unsigned short value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        pushunsigned(L, value);
        return true;
    }

    [[nodiscard]] static unsigned short get(lua_State* L, int index)
    {
        return static_cast<unsigned short>(luaL_checkinteger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned short>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `int`.
 */
template <>
struct Stack<int>
{
    [[nodiscard]] static bool push(lua_State* L, int value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        if (! is_integral_representable_by(value))
        {
            ec = makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);
            return false;
        }

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return true;
    }

    [[nodiscard]] static int get(lua_State* L, int index)
    {
        return static_cast<int>(luaL_checkinteger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<int>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `unsigned int`.
 */
template <>
struct Stack<unsigned int>
{
    [[nodiscard]] static bool push(lua_State* L, unsigned int value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        if (! is_integral_representable_by(value))
        {
            ec = makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);
            return false;
        }
        
        pushunsigned(L, value);
        return true;
    }

    [[nodiscard]] static uint32_t get(lua_State* L, int index)
    {
        return static_cast<unsigned int>(luaL_checkinteger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned int>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `long`.
 */
template <>
struct Stack<long>
{
    [[nodiscard]] static bool push(lua_State* L, long value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        if (! is_integral_representable_by(value))
        {
            ec = makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);
            return false;
        }

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return true;
    }

    [[nodiscard]] static long get(lua_State* L, int index)
    {
        return static_cast<long>(luaL_checkinteger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<long>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `unsigned long`.
 */
template <>
struct Stack<unsigned long>
{
    [[nodiscard]] static bool push(lua_State* L, unsigned long value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        if (! is_integral_representable_by(value))
        {
            ec = makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);
            return false;
        }

        pushunsigned(L, value);
        return true;
    }

    [[nodiscard]] static unsigned long get(lua_State* L, int index)
    {
        return static_cast<unsigned long>(luaL_checkinteger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned long>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `long long`.
 */
template <>
struct Stack<long long>
{
    [[nodiscard]] static bool push(lua_State* L, long long value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        if (! is_integral_representable_by(value))
        {
            ec = makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);
            return false;
        }

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return true;
    }

    [[nodiscard]] static long long get(lua_State* L, int index)
    {
        return static_cast<long long>(luaL_checkinteger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<long long>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `unsigned long long`.
 */
template <>
struct Stack<unsigned long long>
{
    [[nodiscard]] static bool push(lua_State* L, unsigned long long value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        if (! is_integral_representable_by(value))
        {
            ec = makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);
            return false;
        }

        pushunsigned(L, value);
        return true;
    }

    [[nodiscard]] static unsigned long long get(lua_State* L, int index)
    {
        return static_cast<unsigned long long>(luaL_checkinteger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned long long>(L, index);
        
        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `float`.
 */
template <>
struct Stack<float>
{
    [[nodiscard]] static bool push(lua_State* L, float value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        if (! is_floating_point_representable_by(value))
        {
            ec = makeErrorCode(ErrorCode::FloatingPointDoesntFitIntoLuaNumber);
            return false;
        }

        lua_pushnumber(L, static_cast<lua_Number>(value));
        return true;
    }

    [[nodiscard]] static float get(lua_State* L, int index)
    {
        return static_cast<float>(luaL_checknumber(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_floating_point_representable_by<float>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `double`.
 */
template <>
struct Stack<double>
{
    [[nodiscard]] static bool push(lua_State* L, double value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        if (! is_floating_point_representable_by(value))
        {
            ec = makeErrorCode(ErrorCode::FloatingPointDoesntFitIntoLuaNumber);
            return false;
        }

        lua_pushnumber(L, static_cast<lua_Number>(value));
        return true;
    }

    [[nodiscard]] static double get(lua_State* L, int index)
    {
        return static_cast<double>(luaL_checknumber(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_floating_point_representable_by<double>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `long double`.
 */
template <>
struct Stack<long double>
{
    [[nodiscard]] static bool push(lua_State* L, long double value, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        if (! is_floating_point_representable_by(value))
        {
            ec = makeErrorCode(ErrorCode::FloatingPointDoesntFitIntoLuaNumber);
            return false;
        }

        lua_pushnumber(L, static_cast<lua_Number>(value));
        return true;
    }

    [[nodiscard]] static long double get(lua_State* L, int index)
    {
        return static_cast<long double>(luaL_checknumber(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_floating_point_representable_by<long double>(L, index);

        return false;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `const char*`.
 */
template <>
struct Stack<const char*>
{
    [[nodiscard]] static bool push(lua_State* L, const char* str, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        if (str != nullptr)
            lua_pushstring(L, str);
        else
            lua_pushlstring(L, "", 0);

        return true;
    }

    [[nodiscard]] static const char* get(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TSTRING)
        {
            std::size_t length = 0;
            const char* str = lua_tolstring(L, index, &length);

            if (str != nullptr)
                return str;
        }

        return "";
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TSTRING;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `std::string_view`.
 */
template <>
struct Stack<std::string_view>
{
    [[nodiscard]] static bool push(lua_State* L, std::string_view str, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        lua_pushlstring(L, str.data(), str.size());
        return true;
    }

    [[nodiscard]] static std::string_view get(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TSTRING)
        {
            std::size_t length = 0;
            const char* str = lua_tolstring(L, index, &length);

            if (str != nullptr)
                return { str, length };
        }

        return {};
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TSTRING;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `std::string`.
 */
template <>
struct Stack<std::string>
{
    [[nodiscard]] static bool push(lua_State* L, const std::string& str, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        lua_pushlstring(L, str.data(), str.size());
        return true;
    }

    [[nodiscard]] static std::string get(lua_State* L, int index)
    {
        std::size_t length = 0;

        if (lua_type(L, index) == LUA_TSTRING)
        {
            const char* str = lua_tolstring(L, index, &length);

            if (str != nullptr)
                return { str, length };
        }

        // Lua reference manual:
        // If the value is a number, then lua_tolstring also changes the actual value in the stack
        // to a string. (This change confuses lua_next when lua_tolstring is applied to keys during
        // a table traversal)
        lua_pushvalue(L, index);
        const char* str = lua_tolstring(L, -1, &length);
        lua_pop(L, 1);

        if (str != nullptr)
            return { str, length };

        return {};
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TSTRING;
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `std::optional`.
 */
template <class T>
struct Stack<std::optional<T>>
{
    using Type = std::optional<T>;
    
    [[nodiscard]] static bool push(lua_State* L, const Type& value, std::error_code& ec)
    {
        if (value)
            return Stack<T>::push(L, *value, ec);

#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        lua_pushnil(L);
        return true;
    }

    [[nodiscard]] static Type get(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNIL)
            return std::nullopt;
        
        return Stack<T>::get(L, index);
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_isnil(L, index) || Stack<T>::isInstance(L, index);
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `std::tuple`.
 */
template <class... Types>
struct Stack<std::tuple<Types...>>
{
    [[nodiscard]] static bool push(lua_State* L, const std::tuple<Types...>& t, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        lua_createtable(L, static_cast<int>(Size), 0);

        return push_element(L, t, ec);
    }

    [[nodiscard]] static std::tuple<Types...> get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            luaL_error(L, "#%d argment must be a table", index);

        if (get_length(L, index) != static_cast<int>(Size))
            luaL_error(L, "table size should be %d but is %d", static_cast<unsigned>(Size), get_length(L, index));

        std::tuple<Types...> value;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        pop_element(L, absIndex, value);

        return value;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TTABLE && get_length(L, index) == static_cast<int>(Size);
    }

private:
    static constexpr std::size_t Size = std::tuple_size_v<std::tuple<Types...>>;

    template <std::size_t Index = 0>
    static auto push_element(lua_State*, const std::tuple<Types...>&, std::error_code&)
        -> std::enable_if_t<Index == sizeof...(Types), bool>
    {
        return true;
    }

    template <std::size_t Index = 0>
    static auto push_element(lua_State* L, const std::tuple<Types...>& t, std::error_code& ec)
        -> std::enable_if_t<Index < sizeof...(Types), bool>
    {
        using T = std::tuple_element_t<Index, std::tuple<Types...>>;

        lua_pushinteger(L, static_cast<lua_Integer>(Index + 1));

        std::error_code push_ec;
        bool result = Stack<T>::push(L, std::get<Index>(t), push_ec);
        if (! result)
        {
            lua_pushnil(L);
            lua_settable(L, -3);
            ec = push_ec;
            return false;
        }

        lua_settable(L, -3);

        return push_element<Index + 1>(L, t, ec);
    }

    template <std::size_t Index = 0>
    static auto pop_element(lua_State*, int, std::tuple<Types...>&)
        -> std::enable_if_t<Index == sizeof...(Types)>
    {
    }

    template <std::size_t Index = 0>
    static auto pop_element(lua_State* L, int absIndex, std::tuple<Types...>& t)
        -> std::enable_if_t<Index < sizeof...(Types)>
    {
        using T = std::tuple_element_t<Index, std::tuple<Types...>>;

        if (lua_next(L, absIndex) == 0)
            return;

        std::get<Index>(t) = Stack<T>::get(L, -1);
        lua_pop(L, 1);

        pop_element<Index + 1>(L, absIndex, t);
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `T[N]`.
 */
template <class T, std::size_t N>
struct Stack<T[N]>
{
    static_assert(N > 0, "Unsupported zero sized array");

    [[nodiscard]] static bool push(lua_State* L, const T (&value)[N], std::error_code& ec)
    {
        if constexpr (std::is_same_v<T, char>)
        {
#if LUABRIDGE_SAFE_STACK_CHECKS
            if (! lua_checkstack(L, 1))
            {
                ec = makeErrorCode(ErrorCode::LuaStackOverflow);
                return false;
            }
#endif

            lua_pushlstring(L, value, N - 1);
            return true;
        }

#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 2))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        const int initialStackSize = lua_gettop(L);

        lua_createtable(L, static_cast<int>(N), 0);

        for (std::size_t i = 0; i < N; ++i)
        {
            lua_pushinteger(L, static_cast<lua_Integer>(i + 1));

            std::error_code push_ec;
            bool result = Stack<T>::push(L, value[i], push_ec);
            if (! result)
            {
                ec = push_ec;
                lua_pop(L, lua_gettop(L) - initialStackSize);
                return false;
            }

            lua_settable(L, -3);
        }

        return true;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TTABLE && get_length(L, index) == static_cast<int>(N);
    }
};

namespace detail {

template <class T>
struct StackOpSelector<T&, false>
{
    using ReturnType = T;

    static bool push(lua_State* L, T& value, std::error_code& ec) { return Stack<T>::push(L, value, ec); }

    static ReturnType get(lua_State* L, int index) { return Stack<T>::get(L, index); }

    static bool isInstance(lua_State* L, int index) { return Stack<T>::isInstance(L, index); }
};

template <class T>
struct StackOpSelector<const T&, false>
{
    using ReturnType = T;

    static bool push(lua_State* L, const T& value, std::error_code& ec) { return Stack<T>::push(L, value, ec); }

    static auto get(lua_State* L, int index) { return Stack<T>::get(L, index); }

    static bool isInstance(lua_State* L, int index) { return Stack<T>::isInstance(L, index); }
};

template <class T>
struct StackOpSelector<T*, false>
{
    using ReturnType = T;

    static bool push(lua_State* L, T* value, std::error_code& ec) { return Stack<T>::push(L, *value, ec); }

    static ReturnType get(lua_State* L, int index) { return Stack<T>::get(L, index); }

    static bool isInstance(lua_State* L, int index) { return Stack<T>::isInstance(L, index); }
};

template <class T>
struct StackOpSelector<const T*, false>
{
    using ReturnType = T;

    static bool push(lua_State* L, const T* value, std::error_code& ec) { return Stack<T>::push(L, *value, ec); }

    static ReturnType get(lua_State* L, int index) { return Stack<T>::get(L, index); }

    static bool isInstance(lua_State* L, int index) { return Stack<T>::isInstance(L, index); }
};

} // namespace detail

template <class T>
struct Stack<T&, std::enable_if_t<!std::is_array_v<T&>>>
{
    using Helper = detail::StackOpSelector<T&, detail::IsUserdata<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    [[nodiscard]] static bool push(lua_State* L, T& value, std::error_code& ec) { return Helper::push(L, value, ec); }

    [[nodiscard]] static ReturnType get(lua_State* L, int index) { return Helper::get(L, index); }

    [[nodiscard]] static bool isInstance(lua_State* L, int index) { return Helper::template isInstance<T>(L, index); }
};

template <class T>
struct Stack<const T&, std::enable_if_t<!std::is_array_v<const T&>>>
{
    using Helper = detail::StackOpSelector<const T&, detail::IsUserdata<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    [[nodiscard]] static bool push(lua_State* L, const T& value, std::error_code& ec) { return Helper::push(L, value, ec); }

    [[nodiscard]] static auto get(lua_State* L, int index) { return Helper::get(L, index); }

    [[nodiscard]] static bool isInstance(lua_State* L, int index) { return Helper::template isInstance<T>(L, index); }
};

template <class T>
struct Stack<T*>
{
    using Helper = detail::StackOpSelector<T*, detail::IsUserdata<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    [[nodiscard]] static bool push(lua_State* L, T* value, std::error_code& ec) { return Helper::push(L, value, ec); }

    [[nodiscard]] static ReturnType get(lua_State* L, int index) { return Helper::get(L, index); }

    [[nodiscard]] static bool isInstance(lua_State* L, int index) { return Helper::template isInstance<T>(L, index); }
};

template<class T>
struct Stack<const T*>
{
    using Helper = detail::StackOpSelector<const T*, detail::IsUserdata<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    [[nodiscard]] static bool push(lua_State* L, const T* value, std::error_code& ec) { return Helper::push(L, value, ec); }

    [[nodiscard]] static ReturnType get(lua_State* L, int index) { return Helper::get(L, index); }

    [[nodiscard]] static bool isInstance(lua_State* L, int index) { return Helper::template isInstance<T>(L, index); }
};

//=================================================================================================
/**
 * @brief Push an object onto the Lua stack.
 */
template <class T>
[[nodiscard]] bool push(lua_State* L, const T& t, std::error_code& ec)
{
    return Stack<T>::push(L, t, ec);
}

//=================================================================================================
/**
 * @brief Get an object from the Lua stack.
 */
template <class T>
[[nodiscard]] T get(lua_State* L, int index)
{
    return Stack<T>::get(L, index);
}

//=================================================================================================
/**
 * @brief Check whether an object on the Lua stack is of type T.
 */
template <class T>
[[nodiscard]] bool isInstance(lua_State* L, int index)
{
    return Stack<T>::isInstance(L, index);
}

//=================================================================================================
/**
 * @brief Stack restorer.
 */
class StackRestore final
{
public:
    StackRestore(lua_State* L)
        : m_L(L)
        , m_stackTop(lua_gettop(L))
    {
    }

    ~StackRestore()
    {
        lua_settop(m_L, m_stackTop);
    }

private:
    lua_State* const m_L = nullptr;
    int m_stackTop = 0;
};

} // namespace luabridge

// End File: Source/LuaBridge/detail/Stack.h

// Begin File: Source/LuaBridge/Array.h


namespace luabridge {

//=================================================================================================
/**
 * @brief Stack specialization for `std::array`.
 */
template <class T, std::size_t Size>
struct Stack<std::array<T, Size>>
{
    using Type = std::array<T, Size>;

    [[nodiscard]] static bool push(lua_State* L, const Type& array, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        const int initialStackSize = lua_gettop(L);
        
        lua_createtable(L, static_cast<int>(Size), 0);

        for (std::size_t i = 0; i < Size; ++i)
        {
            lua_pushinteger(L, static_cast<lua_Integer>(i + 1));

            std::error_code errorCode;
            bool result = Stack<T>::push(L, array[i], errorCode);
            if (!result)
            {
                ec = errorCode;
                lua_pop(L, lua_gettop(L) - initialStackSize);
                return false;
            }

            lua_settable(L, -3);
        }
        
        return true;
    }

    [[nodiscard]] static Type get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            luaL_error(L, "#%d argment must be a table", index);

        if (get_length(L, index) != Size)
            luaL_error(L, "table size should be %d but is %d", static_cast<int>(Size), get_length(L, index));

        Type array;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        int arrayIndex = 0;
        while (lua_next(L, absIndex) != 0)
        {
            array[arrayIndex++] = Stack<T>::get(L, -1);
            lua_pop(L, 1);
        }

        return array;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index) && get_length(L, index) == Size;
    }
};

} // namespace luabridge

// End File: Source/LuaBridge/Array.h

// Begin File: Source/LuaBridge/List.h


namespace luabridge {

//=================================================================================================
/**
 * @brief Stack specialization for `std::array`.
 */
template <class T>
struct Stack<std::list<T>>
{
    using Type = std::list<T>;
    
    [[nodiscard]] static bool push(lua_State* L, const Type& list, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        const int initialStackSize = lua_gettop(L);

        lua_createtable(L, static_cast<int>(list.size()), 0);

        auto it = list.cbegin();
        for (lua_Integer tableIndex = 1; it != list.cend(); ++tableIndex, ++it)
        {
            lua_pushinteger(L, tableIndex);

            std::error_code errorCode;
            if (! Stack<T>::push(L, *it, errorCode))
            {
                ec = errorCode;
                lua_pop(L, lua_gettop(L) - initialStackSize);
                return false;
            }

            lua_settable(L, -3);
        }
        
        return true;
    }

    [[nodiscard]] static Type get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            luaL_error(L, "#%d argument must be a table", index);

        Type list;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        while (lua_next(L, absIndex) != 0)
        {
            list.emplace_back(Stack<T>::get(L, -1));
            lua_pop(L, 1);
        }

        return list;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index);
    }
};

} // namespace luabridge

// End File: Source/LuaBridge/List.h

// Begin File: Source/LuaBridge/detail/FuncTraits.h


namespace luabridge {
namespace detail {

//=================================================================================================
/**
 * @brief Generic function traits.
 *
 * @tparam IsMember True if the function is a member function pointer.
 * @tparam IsConst True if the function is const.
 * @tparam R Return type of the function.
 * @tparam Args Arguments types as variadic parameter pack.
 */
template <bool IsMember, bool IsConst, class R, class... Args>
struct function_traits_base
{
    using result_type = R;

    using argument_types = std::tuple<Args...>;

    static constexpr auto arity = sizeof...(Args);

    static constexpr auto is_member = IsMember;

    static constexpr auto is_const = IsConst;
};

template <class...>
struct function_traits_impl;

template <class R, class... Args>
struct function_traits_impl<R(Args...)> : function_traits_base<false, false, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R (*)(Args...)> : function_traits_base<false, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (C::*)(Args...)> : function_traits_base<true, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (C::*)(Args...) const> : function_traits_base<true, true, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R(Args...) noexcept> : function_traits_base<false, false, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R (*)(Args...) noexcept> : function_traits_base<false, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (C::*)(Args...) noexcept> : function_traits_base<true, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (C::*)(Args...) const noexcept> : function_traits_base<true, true, R, Args...>
{
};

#if _MSC_VER && _M_IX86 // Windows: WINAPI (a.k.a. __stdcall) function pointers (32bit only).
template <class R, class... Args>
struct function_traits_impl<R __stdcall(Args...)> : function_traits_base<false, false, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R (__stdcall *)(Args...)> : function_traits_base<false, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__stdcall C::*)(Args...)> : function_traits_base<true, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__stdcall C::*)(Args...) const> : function_traits_base<true, true, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R __stdcall(Args...) noexcept> : function_traits_base<false, false, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R (__stdcall *)(Args...) noexcept> : function_traits_base<false, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__stdcall C::*)(Args...) noexcept> : function_traits_base<true, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__stdcall C::*)(Args...) const noexcept> : function_traits_base<true, true, R, Args...>
{
};
#endif

template <class F>
struct functor_traits_impl : function_traits_impl<decltype(&F::operator())>
{
};

//=================================================================================================
/**
 * @brief Traits class for callable objects (e.g. function pointers, lambdas)
 *
 * @tparam F Callable object.
 */
template <class F>
struct function_traits : std::conditional_t<std::is_class_v<F>,
                                            detail::functor_traits_impl<F>,
                                            detail::function_traits_impl<F>>
{
};

//=================================================================================================
/**
 * @brief Deduces the return type of a callble object.
 *
 * @tparam F Callable object.
 */
template <class F>
using function_result_t = typename function_traits<F>::result_type;

/**
 * @brief Deduces the argument type of a callble object.
 *
 * @tparam I Argument index.
 * @tparam F Callable object.
 */
template <std::size_t I, class F>
using function_argument_t = std::tuple_element_t<I, typename function_traits<F>::argument_types>;

/**
 * @brief Deduces the arguments type of a callble object.
 *
 * @tparam F Callable object.
 */
template <class F>
using function_arguments_t = typename function_traits<F>::argument_types;

/**
 * @brief An integral constant expression that gives the number of arguments accepted by the callable object.
 *
 * @tparam F Callable object.
 */
template <class F>
static constexpr std::size_t function_arity_v = function_traits<F>::arity;

/**
 * @brief An boolean constant expression that checks if the callable object is a member function.
 *
 * @tparam F Callable object.
 */
template <class F>
static constexpr bool function_is_member_v = function_traits<F>::is_member;

/**
 * @brief An boolean constant expression that checks if the callable object is const.
 *
 * @tparam F Callable object.
 */
template <class F>
static constexpr bool function_is_const_v = function_traits<F>::is_const;

//=================================================================================================
/**
 * @brief Detect if we are a `std::function`.
 *
 * @tparam F Callable object.
 */
template <class F, class...>
struct is_std_function : std::false_type
{
};

template <class ReturnType, class... Args>
struct is_std_function<std::function<ReturnType(Args...)>> : std::true_type
{
};

template <class Signature>
struct is_std_function<std::function<Signature>> : std::true_type
{
};

template <class F>
static constexpr bool is_std_function_v = is_std_function<F>::value;

//=================================================================================================
/**
 * @brief Reconstruct a function signature from return type and args.
 */
template <class, class...>
struct to_std_function_type
{
};

template <class ReturnType, class... Args>
struct to_std_function_type<ReturnType, std::tuple<Args...>>
{
    using type = std::function<ReturnType(Args...)>;
};

template <class ReturnType, class... Args>
using to_std_function_type_t = typename to_std_function_type<ReturnType, Args...>::type;

//=================================================================================================
/**
 * @brief Simple make_tuple alternative that doesn't decay the types.
 *
 * @tparam Types Argument types that will compose the tuple.
 */
template <class... Types>
constexpr auto tupleize(Types&&... types)
{
    return std::tuple<Types...>(std::forward<Types>(types)...);
}

//=================================================================================================
/**
 * @brief Make argument lists extracting them from the lua state, starting at a stack index.
 *
 * @tparam ArgsPack Arguments pack to extract from the lua stack.
 * @tparam Start Start index where stack variables are located in the lua stack.
 */
template <class ArgsPack, std::size_t Start, std::size_t... Indices>
auto make_arguments_list_impl(lua_State* L, std::index_sequence<Indices...>)
{
    return tupleize(Stack<std::tuple_element_t<Indices, ArgsPack>>::get(L, Start + Indices)...);
}

template <class ArgsPack, std::size_t Start>
auto make_arguments_list(lua_State* L)
{
    return make_arguments_list_impl<ArgsPack, Start>(L, std::make_index_sequence<std::tuple_size_v<ArgsPack>>());
}

//=================================================================================================
/**
 * @brief Helpers for iterating through tuple arguments, pushing each argument to the lua stack.
 */
template <std::size_t Index = 0, class... Types>
auto push_arguments(lua_State*, std::tuple<Types...>, std::error_code&)
    -> std::enable_if_t<Index == sizeof...(Types), std::size_t>
{
    return Index + 1;
}

template <std::size_t Index = 0, class... Types>
auto push_arguments(lua_State* L, std::tuple<Types...> t, std::error_code& ec)
    -> std::enable_if_t<Index < sizeof...(Types), std::size_t>
{
    using T = std::tuple_element_t<Index, std::tuple<Types...>>;

    std::error_code pec;
    bool result = Stack<T>::push(L, std::get<Index>(t), pec);
    if (! result)
    {
        ec = pec;
        return Index + 1;
    }

    return push_arguments<Index + 1, Types...>(L, std::move(t), ec);
}

//=================================================================================================
/**
 * @brief Helpers for iterating through tuple arguments, popping each argument from the lua stack.
 */
template <std::ptrdiff_t Start, std::ptrdiff_t Index = 0, class... Types>
auto pop_arguments(lua_State*, std::tuple<Types...>&)
    -> std::enable_if_t<Index == sizeof...(Types), std::size_t>
{
    return sizeof...(Types);
}

template <std::ptrdiff_t Start, std::ptrdiff_t Index = 0, class... Types>
auto pop_arguments(lua_State* L, std::tuple<Types...>& t)
    -> std::enable_if_t<Index < sizeof...(Types), std::size_t>
{
    using T = std::tuple_element_t<Index, std::tuple<Types...>>;

    std::get<Index>(t) = Stack<T>::get(L, Start - Index);

    return pop_arguments<Start, Index + 1, Types...>(L, t);
}

//=================================================================================================
/**
 * @brief Remove first type from tuple.
 */
template <class T>
struct remove_first_type
{
};

template <class T, class... Ts>
struct remove_first_type<std::tuple<T, Ts...>>
{
    using type = std::tuple<Ts...>;
};

template <class T>
using remove_first_type_t = typename remove_first_type<T>::type;

//=================================================================================================
/**
 * @brief Function generator.
 */
template <class ReturnType, class ArgsPack, std::size_t Start = 1u>
struct function
{
    template <class F>
    static int call(lua_State* L, F func)
    {
        std::error_code ec;
        bool result = false;

#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            result = Stack<ReturnType>::push(L, std::apply(func, make_arguments_list<ArgsPack, Start>(L)), ec);

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        if (! result)
            raise_lua_error(L, "%s", ec.message().c_str());

        return 1;
    }

    template <class T, class F>
    static int call(lua_State* L, T* ptr, F func)
    {
        std::error_code ec;
        bool result = false;

#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            auto f = [ptr, func](auto&&... args) -> ReturnType { return (ptr->*func)(std::forward<decltype(args)>(args)...); };

            result = Stack<ReturnType>::push(L, std::apply(f, make_arguments_list<ArgsPack, Start>(L)), ec);

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        if (! result)
            raise_lua_error(L, "%s", ec.message().c_str());

        return 1;
    }
};

template <class ArgsPack, std::size_t Start>
struct function<void, ArgsPack, Start>
{
    template <class F>
    static int call(lua_State* L, F func)
    {
#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            std::apply(func, make_arguments_list<ArgsPack, Start>(L));

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        return 0;
    }

    template <class T, class F>
    static int call(lua_State* L, T* ptr, F func)
    {
#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            auto f = [ptr, func](auto&&... args) { (ptr->*func)(std::forward<decltype(args)>(args)...); };

            std::apply(f, make_arguments_list<ArgsPack, Start>(L));

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        return 0;
    }
};

//=================================================================================================
/**
 * @brief Constructor generators.
 *
 * These templates call operator new with the contents of a type/value list passed to the constructor. Two versions of call() are provided.
 * One performs a regular new, the other performs a placement new.
 */
template <class T, class Args>
struct constructor;

template <class T>
struct constructor<T, void>
{
    using empty = std::tuple<>;

    static T* call(const empty&)
    {
        return new T;
    }

    static T* call(void* ptr, const empty&)
    {
        return new (ptr) T;
    }
};

template <class T, class Args>
struct constructor
{
    static T* call(const Args& args)
    {
        auto alloc = [](auto&&... args) { return new T{ std::forward<decltype(args)>(args)... }; };

        return std::apply(alloc, args);
    }

    static T* call(void* ptr, const Args& args)
    {
        auto alloc = [ptr](auto&&... args) { return new (ptr) T{ std::forward<decltype(args)>(args)... }; };

        return std::apply(alloc, args);
    }
};

//=================================================================================================
/**
 * @brief Factory generators.
 */
template <class T>
struct factory
{
    template <class F, class Args>
    static T* call(void* ptr, const F& func, const Args& args)
    {
        auto alloc = [ptr, &func](auto&&... args) { return func(ptr, std::forward<decltype(args)>(args)...); };

        return std::apply(alloc, args);
    }

    template <class F>
    static T* call(void* ptr, const F& func)
    {
        return func(ptr);
    }
};

} // namespace detail
} // namespace luabridge

// End File: Source/LuaBridge/detail/FuncTraits.h

// Begin File: Source/LuaBridge/detail/CFunctions.h


namespace luabridge {
namespace detail {

//=================================================================================================
/**
 * @brief __index metamethod for a namespace or class static and non-static members.
 *
 * Retrieves functions from metatables and properties from propget tables. Looks through the class hierarchy if inheritance is present.
 */
inline int index_metamethod(lua_State* L)
{
#if LUABRIDGE_SAFE_STACK_CHECKS
    luaL_checkstack(L, 3, detail::error_lua_stack_overflow);
#endif

    assert(lua_istable(L, 1) || lua_isuserdata(L, 1)); // Stack (further not shown): table | userdata, name

    lua_getmetatable(L, 1); // Stack: class/const table (mt)
    assert(lua_istable(L, -1));

    for (;;)
    {
        lua_pushvalue(L, 2); // Stack: mt, field name
        lua_rawget(L, -2); // Stack: mt, field | nil

        if (lua_iscfunction(L, -1)) // Stack: mt, field
        {
            lua_remove(L, -2); // Stack: field
            return 1;
        }

        assert(lua_isnil(L, -1)); // Stack: mt, nil
        lua_pop(L, 1); // Stack: mt

        lua_rawgetp(L, -1, getPropgetKey()); // Stack: mt, propget table (pg)
        assert(lua_istable(L, -1));

        lua_pushvalue(L, 2); // Stack: mt, pg, field name
        lua_rawget(L, -2); // Stack: mt, pg, getter | nil
        lua_remove(L, -2); // Stack: mt, getter | nil

        if (lua_iscfunction(L, -1)) // Stack: mt, getter
        {
            lua_remove(L, -2); // Stack: getter
            lua_pushvalue(L, 1); // Stack: getter, table | userdata
            lua_call(L, 1, 1); // Stack: value
            return 1;
        }

        assert(lua_isnil(L, -1)); // Stack: mt, nil
        lua_pop(L, 1); // Stack: mt

        // It may mean that the field may be in const table and it's constness violation.
        // Don't check that, just return nil

        // Repeat the lookup in the parent metafield,
        // or return nil if the field doesn't exist.
        lua_rawgetp(L, -1, getParentKey()); // Stack: mt, parent mt | nil

        if (lua_isnil(L, -1)) // Stack: mt, nil
        {
            lua_remove(L, -2); // Stack: nil
            return 1;
        }

        // Removethe  metatable and repeat the search in the parent one.
        assert(lua_istable(L, -1)); // Stack: mt, parent mt
        lua_remove(L, -2); // Stack: parent mt
    }

    // no return
}

//=================================================================================================
/**
 * @brief __newindex metamethod for non-static members.
 *
 * Retrieves properties from propset tables.
 */
inline int newindex_metamethod(lua_State* L, bool pushSelf)
{
#if LUABRIDGE_SAFE_STACK_CHECKS
    luaL_checkstack(L, 3, detail::error_lua_stack_overflow);
#endif

    assert(lua_istable(L, 1) || lua_isuserdata(L, 1)); // Stack (further not shown): table | userdata, name, new value

    lua_getmetatable(L, 1); // Stack: metatable (mt)
    assert(lua_istable(L, -1));

    for (;;)
    {
        lua_rawgetp(L, -1, getPropsetKey()); // Stack: mt, propset table (ps) | nil

        if (lua_isnil(L, -1)) // Stack: mt, nil
        {
            lua_pop(L, 2); // Stack: -
            luaL_error(L, "No member named '%s'", lua_tostring(L, 2));
        }

        assert(lua_istable(L, -1));

        lua_pushvalue(L, 2); // Stack: mt, ps, field name
        lua_rawget(L, -2); // Stack: mt, ps, setter | nil
        lua_remove(L, -2); // Stack: mt, setter | nil

        if (lua_iscfunction(L, -1)) // Stack: mt, setter
        {
            lua_remove(L, -2); // Stack: setter
            if (pushSelf)
                lua_pushvalue(L, 1); // Stack: setter, table | userdata
            lua_pushvalue(L, 3); // Stack: setter, table | userdata, new value
            lua_call(L, pushSelf ? 2 : 1, 0); // Stack: -
            return 0;
        }

        assert(lua_isnil(L, -1)); // Stack: mt, nil
        lua_pop(L, 1); // Stack: mt

        lua_rawgetp(L, -1, getParentKey()); // Stack: mt, parent mt | nil

        if (lua_isnil(L, -1)) // Stack: mt, nil
        {
            lua_pop(L, 1); // Stack: -
            luaL_error(L, "No writable member '%s'", lua_tostring(L, 2));
        }

        assert(lua_istable(L, -1)); // Stack: mt, parent mt
        lua_remove(L, -2); // Stack: parent mt
        // Repeat the search in the parent
    }

    return 0;
}

//=================================================================================================
/**
 * @brief __newindex metamethod for objects.
 */
inline int newindex_object_metamethod(lua_State* L)
{
    return newindex_metamethod(L, true);
}

//=================================================================================================
/**
 * @brief __newindex metamethod for namespace or class static members.
 *
 * Retrieves properties from propset tables.
 */
inline int newindex_static_metamethod(lua_State* L)
{
    return newindex_metamethod(L, false);
}

//=================================================================================================
/**
 * @brief lua_CFunction to report an error writing to a read-only value.
 *
 * The name of the variable is in the first upvalue.
 */
inline int read_only_error(lua_State* L)
{
    std::string s;

    s = s + "'" + lua_tostring(L, lua_upvalueindex(1)) + "' is read-only";

    luaL_error(L, "%s", s.c_str());

    return 0;
}

//=================================================================================================
/**
 * @brief __gc metamethod for a class.
 */
template <class C>
static int gc_metamethod(lua_State* L)
{
    Userdata* ud = Userdata::getExact<C>(L, 1);
    assert(ud);

    ud->~Userdata();

    return 0;
}

//=================================================================================================

template <class T, class C = void>
struct property_getter;

/**
 * @brief lua_CFunction to get a variable.
 *
 * This is used for global variables or class static data members. The pointer to the data is in the first upvalue.
 */
template <class T>
struct property_getter<T, void>
{
    static int call(lua_State* L)
    {
        assert(lua_islightuserdata(L, lua_upvalueindex(1)));

        T* ptr = static_cast<T*>(lua_touserdata(L, lua_upvalueindex(1)));
        assert(ptr != nullptr);

        std::error_code ec;
        if (! Stack<T&>::push(L, *ptr, ec))
            raise_lua_error(L, "%s", ec.message().c_str());

        return 1;
    }
};

#if 0
template <class T>
struct property_getter<std::reference_wrapper<T>, void>
{
    static int call(lua_State* L)
    {
        assert(lua_islightuserdata(L, lua_upvalueindex(1)));

        std::reference_wrapper<T>* ptr = static_cast<std::reference_wrapper<T>*>(lua_touserdata(L, lua_upvalueindex(1)));
        assert(ptr != nullptr);

        std::error_code ec;
        if (! Stack<T&>::push(L, ptr->get(), ec))
            luaL_error(L, "%s", ec.message().c_str());

        return 1;
    }
};
#endif

/**
 * @brief lua_CFunction to get a class data member.
 *
 * The pointer-to-member is in the first upvalue. The class userdata object is at the top of the Lua stack.
 */
template <class T, class C>
struct property_getter
{
    static int call(lua_State* L)
    {
        C* c = Userdata::get<C>(L, 1, true);

        T C::** mp = static_cast<T C::**>(lua_touserdata(L, lua_upvalueindex(1)));

        std::error_code ec;
        bool result = false;

#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            result = Stack<T&>::push(L, c->**mp, ec);

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        if (!result)
            raise_lua_error(L, "%s", ec.message().c_str());

        return 1;
    }
};

/**
 * @brief Helper function to push a property getter on a table at a specific index.
 */
inline void add_property_getter(lua_State* L, const char* name, int tableIndex)
{
#if LUABRIDGE_SAFE_STACK_CHECKS
    luaL_checkstack(L, 2, detail::error_lua_stack_overflow);
#endif

    assert(name != nullptr);
    assert(lua_istable(L, tableIndex));
    assert(lua_iscfunction(L, -1)); // Stack: getter

    lua_rawgetp(L, tableIndex, getPropgetKey()); // Stack: getter, propget table (pg)
    lua_pushvalue(L, -2); // Stack: getter, pg, getter
    rawsetfield(L, -2, name); // Stack: getter, pg
    lua_pop(L, 2); // Stack: -
}

//=================================================================================================

template <class T, class C = void>
struct property_setter;

/**
 * @brief lua_CFunction to set a variable.
 *
 * This is used for global variables or class static data members. The pointer to the data is in the first upvalue.
 */
template <class T>
struct property_setter<T, void>
{
    static int call(lua_State* L)
    {
        assert(lua_islightuserdata(L, lua_upvalueindex(1)));

        T* ptr = static_cast<T*>(lua_touserdata(L, lua_upvalueindex(1)));
        assert(ptr != nullptr);

        *ptr = Stack<T>::get(L, 1);

        return 0;
    }
};

#if 0
template <class T>
struct property_setter<std::reference_wrapper<T>, void>
{
    static int call(lua_State* L)
    {
        assert(lua_islightuserdata(L, lua_upvalueindex(1)));

        std::reference_wrapper<T>* ptr = static_cast<std::reference_wrapper<T>*>(lua_touserdata(L, lua_upvalueindex(1)));
        assert(ptr != nullptr);

        ptr->get() = Stack<T>::get(L, 1);

        return 0;
    }
};
#endif

/**
 * @brief lua_CFunction to set a class data member.
 *
 * The pointer-to-member is in the first upvalue. The class userdata object is at the top of the Lua stack.
 */
template <class T, class C>
struct property_setter
{
    static int call(lua_State* L)
    {
        C* c = Userdata::get<C>(L, 1, false);

        T C::** mp = static_cast<T C::**>(lua_touserdata(L, lua_upvalueindex(1)));

#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            c->** mp = Stack<T>::get(L, 2);

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        return 0;
    }
};

/**
 * @brief Helper function to push a property setter on a table at a specific index.
 */
inline void add_property_setter(lua_State* L, const char* name, int tableIndex)
{
#if LUABRIDGE_SAFE_STACK_CHECKS
    luaL_checkstack(L, 2, detail::error_lua_stack_overflow);
#endif

    assert(name != nullptr);
    assert(lua_istable(L, tableIndex));
    assert(lua_iscfunction(L, -1)); // Stack: setter

    lua_rawgetp(L, tableIndex, getPropsetKey()); // Stack: setter, propset table (ps)
    lua_pushvalue(L, -2); // Stack: setter, ps, setter
    rawsetfield(L, -2, name); // Stack: setter, ps
    lua_pop(L, 2); // Stack: -
}

//=================================================================================================
/**
 * @brief lua_CFunction to call a class member function with a return value.
 *
 * The member function pointer is in the first upvalue. The class userdata object is at the top of the Lua stack.
 */
template <class F, class T>
int invoke_member_function(lua_State* L)
{
    using FnTraits = detail::function_traits<F>;

    assert(isfulluserdata(L, lua_upvalueindex(1)));

    T* ptr = Userdata::get<T>(L, 1, false);

    const F& func = *static_cast<const F*>(lua_touserdata(L, lua_upvalueindex(1)));
    assert(func != nullptr);

    return function<typename FnTraits::result_type, typename FnTraits::argument_types, 2>::call(L, ptr, func);
}

template <class F, class T>
int invoke_const_member_function(lua_State* L)
{
    using FnTraits = detail::function_traits<F>;

    assert(isfulluserdata(L, lua_upvalueindex(1)));

    const T* ptr = Userdata::get<T>(L, 1, true);

    const F& func = *static_cast<const F*>(lua_touserdata(L, lua_upvalueindex(1)));
    assert(func != nullptr);

    return function<typename FnTraits::result_type, typename FnTraits::argument_types, 2>::call(L, ptr, func);
}

//=================================================================================================
/**
 * @brief lua_CFunction to call a class member lua_CFunction.
 *
 * The member function pointer is in the first upvalue. The object userdata ('this') value is at top ot the Lua stack.
 */
template <class T>
int invoke_member_cfunction(lua_State* L)
{
    using F = int (T::*)(lua_State * L);

    assert(isfulluserdata(L, lua_upvalueindex(1)));

    T* t = Userdata::get<T>(L, 1, false);

    const F& func = *static_cast<const F*>(lua_touserdata(L, lua_upvalueindex(1)));
    assert(func != nullptr);

    return (t->*func)(L);
}

template <class T>
int invoke_const_member_cfunction(lua_State* L)
{
    using F = int (T::*)(lua_State * L) const;

    assert(isfulluserdata(L, lua_upvalueindex(1)));

    const T* t = Userdata::get<T>(L, 1, true);

    const F& func = *static_cast<const F*>(lua_touserdata(L, lua_upvalueindex(1)));
    assert(func != nullptr);

    return (t->*func)(L);
}

//=================================================================================================
/**
 * @brief lua_CFunction to call on a object via function pointer.
 *
 * The proxy function pointer (lightuserdata) is in the first upvalue. The class userdata object is at the top of the Lua stack.
 */
template <class F>
int invoke_proxy_function(lua_State* L)
{
    using FnTraits = detail::function_traits<F>;

    assert(lua_islightuserdata(L, lua_upvalueindex(1)));

    auto func = reinterpret_cast<F>(lua_touserdata(L, lua_upvalueindex(1)));
    assert(func != nullptr);

    return function<typename FnTraits::result_type, typename FnTraits::argument_types, 1>::call(L, func);
}

//=================================================================================================
/**
 * @brief lua_CFunction to call on a object via functor (lambda wrapped in a std::function).
 *
 * The proxy std::function (lightuserdata) is in the first upvalue. The class userdata object is at the top of the Lua stack.
 */
template <class F>
int invoke_proxy_functor(lua_State* L)
{
    using FnTraits = detail::function_traits<F>;

    assert(isfulluserdata(L, lua_upvalueindex(1)));

    auto& func = *align<F>(lua_touserdata(L, lua_upvalueindex(1)));

    return function<typename FnTraits::result_type, typename FnTraits::argument_types, 1>::call(L, func);
}

} // namespace detail
} // namespace luabridge

// End File: Source/LuaBridge/detail/CFunctions.h

// Begin File: Source/LuaBridge/detail/LuaRef.h


namespace luabridge {

class LuaResult;

//=================================================================================================
/**
 * @brief Type tag for representing LUA_TNIL.
 *
 * Construct one of these using `LuaNil ()` to represent a Lua nil. This is faster than creating a reference in the registry to nil.
 * Example:
 *
 * @code
 *     LuaRef t (LuaRef::createTable (L));
 *     ...
 *     t ["k"] = LuaNil (); // assign nil
 * @endcode
 */
struct LuaNil
{
};

/**
 * @brief Stack specialization for LuaNil.
 */
template <>
struct Stack<LuaNil>
{
    [[nodiscard]] static bool push(lua_State* L, const LuaNil&, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        lua_pushnil(L);
        return true;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TNIL;
    }
};

//=================================================================================================
/**
 * @brief Base class for Lua variables and table item reference classes.
 */
template <class Impl, class LuaRef>
class LuaRefBase
{
protected:
    //=============================================================================================
    /**
     * @brief Pop the Lua stack.
     *
     * Pops the specified number of stack items on destruction. We use this when returning objects, to avoid an explicit temporary
     * variable, since the destructor executes after the return statement. For example:
     *
     * @code
     *     template <class U>
     *     U cast (lua_State* L)
     *     {
     *         StackPop p (L, 1);
     *         ...
     *         return U (); // Destructor called after this line
     *     }
     * @endcode
     *
     * @note The `StackPop` object must always be a named local variable.
     */
    class StackPop
    {
    public:
        /**
         * @brief Create a StackPop object.
         *
         * @param L  A Lua state.
         * @param count The number of stack entries to pop on destruction.
         */
        StackPop(lua_State* L, int count)
            : m_L(L)
            , m_count(count) {
        }

        /**
         * @brief Destroy a StackPop object.
         *
         * In case an exception is in flight before the destructor is called, stack is potentially cleared by lua. So we never pop more than
         * the actual size of the stack.
         */
        ~StackPop()
        {
            const int stackSize = lua_gettop(m_L);

            lua_pop(m_L, stackSize < m_count ? stackSize : m_count);
        }

        /**
         * @brief Set a new number to pop.
         *
         * @param newCount The new number of stack entries to pop on destruction.
         */
        void popCount(int newCount)
        {
            m_count = newCount;
        }

    private:
        lua_State* m_L = nullptr;
        int m_count = 0;
    };

    friend struct Stack<LuaRef>;

    //=============================================================================================
    /**
     * @brief Type tag for stack construction.
     */
    struct FromStack
    {
    };

    LuaRefBase(lua_State* L)
        : m_L(main_thread(L))
    {
    }

    //=============================================================================================
    /**
     * @brief Create a reference to this reference.
     *
     * @returns An index in the Lua registry.
     */
    int createRef() const
    {
        impl().push();

        return luaL_ref(m_L, LUA_REGISTRYINDEX);
    }

public:
    //=============================================================================================
    /**
     * @brief Convert to a string using lua_tostring function.
     *
     * @returns A string representation of the referred Lua value.
     */
    std::string tostring() const
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(m_L, 2))
            return {};
#endif

        StackPop p(m_L, 1);

        lua_getglobal(m_L, "tostring");

        impl().push();

        lua_call(m_L, 1, 1);

        const char* str = lua_tostring(m_L, -1);
        return str != nullptr ? str : "";
    }

    //=============================================================================================
    /**
     * @brief Print a text description of the value to a stream.
     *
     * This is used for diagnostics.
     *
     * @param os An output stream.
     */
    void print(std::ostream& os) const
    {
        switch (type())
        {
        case LUA_TNIL:
            os << "nil";
            break;

        case LUA_TNUMBER:
            os << cast<lua_Number>();
            break;

        case LUA_TBOOLEAN:
            os << (cast<bool>() ? "true" : "false");
            break;

        case LUA_TSTRING:
            os << '"' << cast<const char*>() << '"';
            break;

        case LUA_TTABLE:
            os << "table: " << tostring();
            break;

        case LUA_TFUNCTION:
            os << "function: " << tostring();
            break;

        case LUA_TUSERDATA:
            os << "userdata: " << tostring();
            break;

        case LUA_TTHREAD:
            os << "thread: " << tostring();
            break;

        case LUA_TLIGHTUSERDATA:
            os << "lightuserdata: " << tostring();
            break;

        default:
            os << "unknown";
            break;
        }
    }

    //=============================================================================================
    /**
     * @brief Insert a Lua value or table item reference to a stream.
     *
     * @param os  An output stream.
     * @param ref A Lua reference.
     *
     * @returns The output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const LuaRefBase& ref)
    {
        ref.print(os);
        return os;
    }

    //=============================================================================================
    /**
     * @brief Retrieve the lua_State associated with the reference.
     *
     * @returns A Lua state.
     */
    lua_State* state() const
    {
        return m_L;
    }

    //=============================================================================================
    /**
     * @brief Place the object onto the Lua stack.
     *
     * @param L A Lua state.
     */
    void push(lua_State* L) const
    {
        assert(equalstates(L, m_L));
        (void) L;

        impl().push();
    }

    //=============================================================================================
    /**
     * @brief Pop the top of Lua stack and assign it to the reference.
     *
     * @param L A Lua state.
     */
    void pop(lua_State* L)
    {
        assert(equalstates(L, m_L));
        (void) L;

        impl().pop();
    }

    //=============================================================================================
    /**
     * @brief Return the Lua type of the referred value.
     *
     * This invokes lua_type().
     *
     * @returns The type of the referred value.
     *
     * @see lua_type()
     */
    int type() const
    {
        StackPop p(m_L, 1);

        impl().push();

        const int refType = lua_type(m_L, -1);

        return refType;
    }

    /**
     * @brief Indicate whether it is a nil reference.
     *
     * @returns True if this is a nil reference, false otherwise.
     */
    bool isNil() const { return type() == LUA_TNIL; }

    /**
     * @brief Indicate whether it is a reference to a boolean.
     *
     * @returns True if it is a reference to a boolean, false otherwise.
     */
    bool isBool() const { return type() == LUA_TBOOLEAN; }

    /**
     * @brief Indicate whether it is a reference to a number.
     *
     * @returns True if it is a reference to a number, false otherwise.
     */
    bool isNumber() const { return type() == LUA_TNUMBER; }

    /**
     * @brief Indicate whether it is a reference to a string.
     *
     * @returns True if it is a reference to a string, false otherwise.
     */
    bool isString() const { return type() == LUA_TSTRING; }

    /**
     * @brief Indicate whether it is a reference to a table.
     *
     * @returns True if it is a reference to a table, false otherwise.
     */
    bool isTable() const { return type() == LUA_TTABLE; }

    /**
     * @brief Indicate whether it is a reference to a function.
     *
     * @returns True if it is a reference to a function, false otherwise.
     */
    bool isFunction() const { return type() == LUA_TFUNCTION; }

    /**
     * @brief Indicate whether it is a reference to a full userdata.
     *
     * @returns True if it is a reference to a full userdata, false otherwise.
     */
    bool isUserdata() const { return type() == LUA_TUSERDATA; }

    /**
     * @brief Indicate whether it is a reference to a lua thread (coroutine).
     *
     * @returns True if it is a reference to a lua thread, false otherwise.
     */
    bool isThread() const { return type() == LUA_TTHREAD; }

    /**
     * @brief Indicate whether it is a reference to a light userdata.
     *
     * @returns True if it is a reference to a light userdata, false otherwise.
     */
    bool isLightUserdata() const { return type() == LUA_TLIGHTUSERDATA; }

    /**
     * @brief Indicate whether it is a callable.
     *
     * @returns True if it is a callable, false otherwise.
     */
    bool isCallable() const
    {
        if (isFunction())
            return true;

        auto metatable = getMetatable();
        return metatable.isTable() && metatable["__call"].isFunction();
    }

    //=============================================================================================
    /**
     * @brief Perform an explicit conversion to the type T.
     *
     * @returns A value of the type T converted from this reference.
     */
    template <class T>
    T cast() const
    {
        StackPop p(m_L, 1);

        impl().push();

        if constexpr (std::is_enum_v<T>)
        {
            using U = std::underlying_type_t<T>;

            return static_cast<T>(Stack<U>::get(m_L, -1));
        }
        else
        {
            return Stack<T>::get(m_L, -1);
        }
    }

    //=============================================================================================
    /**
     * @brief Indicate if this reference is convertible to the type T.
     *
     * @returns True if the referred value is convertible to the type T, false otherwise.
     */
    template <class T>
    bool isInstance() const
    {
        StackPop p(m_L, 1);

        impl().push();

        if constexpr (std::is_enum_v<T>)
        {
            using U = std::underlying_type_t<T>;
            
            return Stack<U>::isInstance(m_L, -1);
        }
        else
        {
            return Stack<T>::isInstance(m_L, -1);
        }
    }

    //=============================================================================================
    /**
     * @brief Type cast operator.
     *
     * @returns A value of the type T converted from this reference.
     */
    template <class T>
    operator T() const
    {
        return cast<T>();
    }

    //=============================================================================================
    /**
     * @brief Get the metatable for the LuaRef.
     *
     * @returns A LuaRef holding the metatable of the lua object.
     */
    LuaRef getMetatable() const
    {
        if (isNil())
            return LuaRef(m_L);

        StackPop p(m_L, 2);

        impl().push();

        if (! lua_getmetatable(m_L, -1))
        {
            p.popCount(1);
            return LuaRef(m_L);
        }

        return LuaRef::fromStack(m_L);
    }

    //=============================================================================================
    /**
     * @brief Compare this reference with a specified value using lua_compare().
     *
     * This invokes metamethods.
     *
     * @param rhs A value to compare with.
     *
     * @returns True if the referred value is equal to the specified one.
     */
    template <class T>
    bool operator==(const T& rhs) const
    {
        StackPop p(m_L, 2);

        impl().push();

        std::error_code ec;
        if (! Stack<T>::push(m_L, rhs, ec))
        {
            p.popCount(1);
            return false;
        }

        return lua_compare(m_L, -2, -1, LUA_OPEQ) == 1;
    }

    /**
     * @brief Compare this reference with a specified value using lua_compare().
     *
     * This invokes metamethods.
     *
     * @param rhs A value to compare with.
     *
     * @returns True if the referred value is not equal to the specified one.
     */
    template <class T>
    bool operator!=(const T& rhs) const
    {
        return !(*this == rhs);
    }

    /**
     * @brief Compare this reference with a specified value using lua_compare().
     *
     * This invokes metamethods.
     *
     * @param rhs A value to compare with.
     *
     * @returns True if the referred value is less than the specified one.
     */
    template <class T>
    bool operator<(const T& rhs) const
    {
        StackPop p(m_L, 2);

        impl().push();

        std::error_code ec;
        if (! Stack<T>::push(m_L, rhs, ec))
        {
            p.popCount(1);
            return false;
        }

        const int lhsType = lua_type(m_L, -2);
        const int rhsType = lua_type(m_L, -1);
        if (lhsType != rhsType)
            return lhsType < rhsType;

        return lua_compare(m_L, -2, -1, LUA_OPLT) == 1;
    }

    /**
     * @brief Compare this reference with a specified value using lua_compare().
     *
     * This invokes metamethods.
     *
     * @param rhs A value to compare with.
     *
     * @returns True if the referred value is less than or equal to the specified one.
     */
    template <class T>
    bool operator<=(const T& rhs) const
    {
        StackPop p(m_L, 2);

        impl().push();

        std::error_code ec;
        if (! Stack<T>::push(m_L, rhs, ec))
        {
            p.popCount(1);
            return false;
        }

        const int lhsType = lua_type(m_L, -2);
        const int rhsType = lua_type(m_L, -1);
        if (lhsType != rhsType)
            return lhsType <= rhsType;

        return lua_compare(m_L, -2, -1, LUA_OPLE) == 1;
    }

    /**
     * @brief Compare this reference with a specified value using lua_compare().
     *
     * This invokes metamethods.
     *
     * @param rhs A value to compare with.
     *
     * @returns True if the referred value is greater than the specified one.
     */
    template <class T>
    bool operator>(const T& rhs) const
    {
        StackPop p(m_L, 2);

        impl().push();

        std::error_code ec;
        if (! Stack<T>::push(m_L, rhs, ec))
        {
            p.popCount(1);
            return false;
        }

        const int lhsType = lua_type(m_L, -2);
        const int rhsType = lua_type(m_L, -1);
        if (lhsType != rhsType)
            return lhsType > rhsType;

        return lua_compare(m_L, -1, -2, LUA_OPLT) == 1;
    }

    /**
     * @brief Compare this reference with a specified value using lua_compare().
     *
     * This invokes metamethods.
     *
     * @param rhs A value to compare with.
     *
     * @returns True if the referred value is greater than or equal to the specified one.
     */
    template <class T>
    bool operator>=(const T& rhs) const
    {
        StackPop p(m_L, 2);

        impl().push();

        std::error_code ec;
        if (! Stack<T>::push(m_L, rhs, ec))
        {
            p.popCount(1);
            return false;
        }

        const int lhsType = lua_type(m_L, -2);
        const int rhsType = lua_type(m_L, -1);
        if (lhsType != rhsType)
            return lhsType >= rhsType;

        return lua_compare(m_L, -1, -2, LUA_OPLE) == 1;
    }

    /**
     * @brief Compare this reference with a specified value using lua_compare().
     *
     * This does not invoke metamethods.
     *
     * @param rhs A value to compare with.
     *
     * @returns True if the referred value is equal to the specified one.
     */
    template <class T>
    bool rawequal(const T& v) const
    {
        StackPop p(m_L, 2);

        impl().push();

        std::error_code ec;
        if (! Stack<T>::push(m_L, v, ec))
        {
            p.popCount(1);
            return false;
        }

        return lua_rawequal(m_L, -1, -2) == 1;
    }

    //=============================================================================================
    /**
     * @brief Append a value to a referred table.
     *
     * If the table is a sequence this will add another element to it.
     *
     * @param v A value to append to the table.
     */
#if 0
    template <class T>
    void append(const T& v) const
    {
        StackPop p(m_L, 1);

        impl().push();

        std::error_code ec;
        if (! Stack<T>::push(m_L, v, ec))
            return;

        luaL_ref(m_L, -2);
    }
#endif
    
    //=============================================================================================
    /**
     * @brief Return the length of a referred array.
     *
     * This is identical to applying the Lua # operator.
     *
     * @returns The length of the referred array.
     */
    int length() const
    {
        StackPop p(m_L, 1);

        impl().push();

        return get_length(m_L, -1);
    }

    //=============================================================================================
    /**
     * @brief Call Lua code.
     *
     * The return value is provided as a LuaRef (which may be LUA_REFNIL).
     *
     * If an error occurs, a LuaException is thrown (only if exceptions are enabled).
     *
     * @returns A result of the call.
     */
    template <class... Args>
    LuaResult operator()(Args&&... args) const;

protected:
    lua_State* m_L = nullptr;

private:
    const Impl& impl() const { return static_cast<const Impl&>(*this); }

    Impl& impl() { return static_cast<Impl&>(*this); }
};

//=================================================================================================
/**
 * @brief Lightweight reference to a Lua object.
 *
 * The reference is maintained for the lifetime of the C++ object.
 */
class LuaRef : public LuaRefBase<LuaRef, LuaRef>
{
    //=============================================================================================
    /**
     * @brief A proxy for representing table values.
     */
    class TableItem : public LuaRefBase<TableItem, LuaRef>
    {
        friend class LuaRef;

    public:
        //=========================================================================================
        /**
         * @brief Construct a TableItem from a table value.
         *
         * The table is in the registry, and the key is at the top of the stack.
         * The key is popped off the stack.
         *
         * @param L A lua state.
         * @param tableRef The index of a table in the Lua registry.
         */
        TableItem(lua_State* L, int tableRef)
            : LuaRefBase(L)
            , m_keyRef(luaL_ref(L, LUA_REGISTRYINDEX))
        {
#if LUABRIDGE_SAFE_STACK_CHECKS
            luaL_checkstack(m_L, 1, detail::error_lua_stack_overflow);
#endif

            lua_rawgeti(m_L, LUA_REGISTRYINDEX, tableRef);
            m_tableRef = luaL_ref(L, LUA_REGISTRYINDEX);
        }

        //=========================================================================================
        /**
         * @brief Create a TableItem via copy constructor.
         *
         * It is best to avoid code paths that invoke this, because it creates an extra temporary Lua reference. Typically this is done by
         * passing the TableItem parameter as a `const` reference.
         *
         * @param other Another Lua table item reference.
         */
        TableItem(const TableItem& other)
            : LuaRefBase(other.m_L)
        {
#if LUABRIDGE_SAFE_STACK_CHECKS
            if (! lua_checkstack(m_L, 1))
                return;
#endif

            lua_rawgeti(m_L, LUA_REGISTRYINDEX, other.m_tableRef);
            m_tableRef = luaL_ref(m_L, LUA_REGISTRYINDEX);

            lua_rawgeti(m_L, LUA_REGISTRYINDEX, other.m_keyRef);
            m_keyRef = luaL_ref(m_L, LUA_REGISTRYINDEX);
        }

        //=========================================================================================
        /**
         * @brief Destroy the proxy.
         *
         * This does not destroy the table value.
         */
        ~TableItem()
        {
            if (m_keyRef != LUA_NOREF)
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_keyRef);

            if (m_tableRef != LUA_NOREF)
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_tableRef);
        }

        //=========================================================================================
        /**
         * @brief Assign a new value to this table key.
         *
         * This may invoke metamethods.
         *
         * @tparam T The type of a value to assing.
         *
         * @param v A value to assign.
         *
         * @returns This reference.
         */
        template <class T>
        TableItem& operator=(const T& v)
        {
#if LUABRIDGE_SAFE_STACK_CHECKS
            if (! lua_checkstack(m_L, 2))
                return *this;
#endif

            StackPop p(m_L, 1);

            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_tableRef);
            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_keyRef);

            std::error_code ec;
            if (! Stack<T>::push(m_L, v, ec))
                return *this;

            lua_settable(m_L, -3);
            return *this;
        }

        //=========================================================================================
        /**
         * @brief Assign a new value to this table key.
         *
         * The assignment is raw, no metamethods are invoked.
         *
         * @tparam T The type of a value to assing.
         *
         * @param v A value to assign.
         *
         * @returns This reference.
         */
        template <class T>
        TableItem& rawset(const T& v)
        {
#if LUABRIDGE_SAFE_STACK_CHECKS
            if (! lua_checkstack(m_L, 2))
                return *this;
#endif

            StackPop p(m_L, 1);

            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_tableRef);
            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_keyRef);

            std::error_code ec;
            if (! Stack<T>::push(m_L, v, ec))
                return *this;

            lua_rawset(m_L, -3);
            return *this;
        }

        //=========================================================================================
        /**
         * @brief Push the value onto the Lua stack.
         */
        using LuaRefBase::push;

        void push() const
        {
#if LUABRIDGE_SAFE_STACK_CHECKS
            if (! lua_checkstack(m_L, 3))
                return;
#endif

            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_tableRef);
            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_keyRef);
            lua_gettable(m_L, -2);
            lua_remove(m_L, -2); // remove the table
        }

        //=========================================================================================
        /**
         * @brief Access a table value using a key.
         *
         * This invokes metamethods.
         *
         * @tparam T The type of a key.
         *
         * @param key A key value.
         *
         * @returns A Lua table item reference.
         */
        template <class T>
        TableItem operator[](const T& key) const
        {
            return LuaRef(*this)[key];
        }

        //=========================================================================================
        /**
         * @brief Access a table value using a key.
         *
         * The operation is raw, metamethods are not invoked. The result is passed by value and may not be modified.
         *
         * @tparam T The type of a key.
         *
         * @param key A key value.
         *
         * @returns A Lua value reference.
         */
        template <class T>
        LuaRef rawget(const T& key) const
        {
            return LuaRef(*this).rawget(key);
        }

    private:
        int m_tableRef = LUA_NOREF;
        int m_keyRef = LUA_NOREF;
    };

    friend struct Stack<TableItem>;
    friend struct Stack<TableItem&>;

    //=========================================================================================
    /**
     * @brief Create a reference to an object at the top of the Lua stack and pop it.
     *
     * This constructor is private and not invoked directly. Instead, use the `fromStack` function.
     *
     * @param L A Lua state.
     *
     * @note The object is popped.
     */
    LuaRef(lua_State* L, FromStack)
        : LuaRefBase(L)
        , m_ref(luaL_ref(m_L, LUA_REGISTRYINDEX))
    {
    }

    //=========================================================================================
    /**
     * @brief Create a reference to an object on the Lua stack.
     *
     * This constructor is private and not invoked directly. Instead, use the `fromStack` function.
     *
     * @param L A Lua state.
     *
     * @param index The index of the value on the Lua stack.
     *
     * @note The object is not popped.
     */
    LuaRef(lua_State* L, int index, FromStack)
        : LuaRefBase(L)
        , m_ref(LUA_NOREF)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(m_L, 1))
            return;
#endif

        lua_pushvalue(m_L, index);
        m_ref = luaL_ref(m_L, LUA_REGISTRYINDEX);
    }

public:
    //=============================================================================================
    /**
     * @brief Create an invalid reference that will be treated as nil.
     *
     * The Lua reference may be assigned later.
     *
     * @param L A Lua state.
     */
    LuaRef(lua_State* L)
        : LuaRefBase(L)
        , m_ref(LUA_NOREF)
    {
    }

    //=============================================================================================
    /**
     * @brief Push a value onto a Lua stack and return a reference to it.
     *
     * @param L A Lua state.
     * @param v A value to push.
     */
    template <class T>
    LuaRef(lua_State* L, const T& v)
        : LuaRefBase(L)
        , m_ref(LUA_NOREF)
    {
        std::error_code ec;
        if (! Stack<T>::push(m_L, v, ec))
            return;

        m_ref = luaL_ref(m_L, LUA_REGISTRYINDEX);
    }

    //=============================================================================================
    /**
     * @brief Create a reference to a table item.
     *
     * @param v A table item reference.
     */
    LuaRef(const TableItem& v)
        : LuaRefBase(v.state())
        , m_ref(v.createRef())
    {
    }

    //=============================================================================================
    /**
     * @brief Create a new reference to an existing Lua value.
     *
     * @param other An existing reference.
     */
    LuaRef(const LuaRef& other)
        : LuaRefBase(other.m_L)
        , m_ref(other.createRef())
    {
    }

    //=============================================================================================
    /**
     * @brief Move a reference to an existing Lua value.
     *
     * @param other An existing reference.
     */
    LuaRef(LuaRef&& other)
        : LuaRefBase(other.m_L)
        , m_ref(std::exchange(other.m_ref, LUA_NOREF))
    {
    }

    //=============================================================================================
    /**
     * @brief Destroy a reference.
     *
     * The corresponding Lua registry reference will be released.
     *
     * @note If the state refers to a thread, it is the responsibility of the caller to ensure that the thread still exists when the LuaRef is destroyed.
     */
    ~LuaRef()
    {
        if (m_ref != LUA_NOREF)
            luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
    }

    //=============================================================================================
    /**
     * @brief Return a reference to a top Lua stack item.
     *
     * The stack item is not popped.
     *
     * @param L A Lua state.
     *
     * @returns A reference to a value on the top of a Lua stack.
     */
    static LuaRef fromStack(lua_State* L)
    {
        return LuaRef(L, FromStack());
    }

    //=============================================================================================
    /**
     * @brief Return a reference to a Lua stack item with a specified index.
     *
     * The stack item is not removed.
     *
     * @param L     A Lua state.
     * @param index An index in the Lua stack.
     *
     * @returns A reference to a value in a Lua stack.
     */
    static LuaRef fromStack(lua_State* L, int index)
    {
        return LuaRef(L, index, FromStack());
    }

    //=============================================================================================
    /**
     * @brief Create a new empty table on the top of a Lua stack and return a reference to it.
     *
     * @param L A Lua state.
     *
     * @returns A reference to the newly created table.
     *
     * @see luabridge::newTable()
     */
    static LuaRef newTable(lua_State* L)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return { L };
#endif

        lua_newtable(L);
        return LuaRef(L, FromStack());
    }

    //=============================================================================================
    /**
     * @brief Return a reference to a named global Lua variable.
     *
     * @param L    A Lua state.
     * @param name The name of a global variable.
     *
     * @returns A reference to the Lua variable.
     *
     * @see luabridge::getGlobal()
     */
    static LuaRef getGlobal(lua_State* L, const char* name)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return { L };
#endif

        lua_getglobal(L, name);
        return LuaRef(L, FromStack());
    }

    //=============================================================================================
    /**
     * @brief Indicate whether it is an invalid reference.
     *
     * @returns True if this is an invalid reference, false otherwise.
     */
    bool isValid() const { return m_ref != LUA_NOREF; }

    //=============================================================================================
    /**
     * @brief Assign another LuaRef to this LuaRef.
     *
     * @param rhs A reference to assign from.
     *
     * @returns This reference.
     */
    LuaRef& operator=(const LuaRef& rhs)
    {
        LuaRef ref(rhs);
        swap(ref);
        return *this;
    }

    //=============================================================================================
    /**
     * @brief Move assign another LuaRef to this LuaRef.
     *
     * @param rhs A reference to assign from.
     *
     * @returns This reference.
     */
    LuaRef& operator=(LuaRef&& rhs)
    {
        if (m_ref != LUA_NOREF)
            luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);

        m_L = rhs.m_L;
        m_ref = std::exchange(rhs.m_ref, LUA_NOREF);
        
        return *this;
    }
    
    //=============================================================================================
    /**
     * @brief Assign a table item reference.
     *
     * @param rhs A table item reference.
     *
     * @returns This reference.
     */
    LuaRef& operator=(const LuaRef::TableItem& rhs)
    {
        LuaRef ref(rhs);
        swap(ref);
        return *this;
    }

    //=============================================================================================
    /**
     * @brief Assign nil to this reference.
     *
     * @returns This reference.
     */
    LuaRef& operator=(const LuaNil&)
    {
        LuaRef ref(m_L);
        swap(ref);
        return *this;
    }

    //=============================================================================================
    /**
     * @brief Assign a different value to this reference.
     *
     * @param rhs A value to assign.
     *
     * @returns This reference.
     */
    template <class T>
    LuaRef& operator=(const T& rhs)
    {
        LuaRef ref(m_L, rhs);
        swap(ref);
        return *this;
    }

    //=============================================================================================
    /**
     * @brief Place the object onto the Lua stack.
     */
    using LuaRefBase::push;

    void push() const
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(m_L, 1))
            return;
#endif

        lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
    }

    //=============================================================================================
    /**
     * @brief Pop the top of Lua stack and assign the ref to m_ref
     */
    void pop()
    {
        if (m_ref != LUA_NOREF)
            luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
        
        m_ref = luaL_ref(m_L, LUA_REGISTRYINDEX);
    }

    //=============================================================================================
    /**
     * @brief Access a table value using a key.
     *
     * This invokes metamethods.
     *
     * @param key A key in the table.
     *
     * @returns A reference to the table item.
     */
    template <class T>
    TableItem operator[](const T& key) const
    {
        std::error_code ec;
        if (! Stack<T>::push(m_L, key, ec))
            return TableItem(m_L, m_ref);

        return TableItem(m_L, m_ref);
    }

    //=============================================================================================
    /**
     * @brief Access a table value using a key.
     *
     * The operation is raw, metamethods are not invoked. The result is passed by value and may not be modified.
     *
     * @param key A key in the table.
     *
     * @returns A reference to the table item.
     */
    template <class T>
    LuaRef rawget(const T& key) const
    {
        StackPop(m_L, 1);

        push(m_L);

        std::error_code ec;
        if (! Stack<T>::push(m_L, key, ec))
            return LuaRef(m_L);

        lua_rawget(m_L, -2);
        return LuaRef(m_L, FromStack());
    }

private:
    void swap(LuaRef& other)
    {
        using std::swap;

        swap(m_L, other.m_L);
        swap(m_ref, other.m_ref);
    }

    int m_ref = LUA_NOREF;
};

//=================================================================================================
/**
 * @brief Stack specialization for `LuaRef`.
 */
template <>
struct Stack<LuaRef>
{
    [[nodiscard]] static bool push(lua_State* L, const LuaRef& v, std::error_code&)
    {
        return v.push(L), true;
    }

    [[nodiscard]] static LuaRef get(lua_State* L, int index)
    {
        return LuaRef::fromStack(L, index);
    }
};

//=================================================================================================
/**
 * @brief Stack specialization for `TableItem`.
 */
template <>
struct Stack<LuaRef::TableItem>
{
    [[nodiscard]] static bool push(lua_State* L, const LuaRef::TableItem& v, std::error_code&)
    {
        return v.push(L), true;
    }
};

//=================================================================================================
/**
 * @brief Create a reference to a new, empty table.
 *
 * This is a syntactic abbreviation for LuaRef::newTable ().
 */
[[nodiscard]] inline LuaRef newTable(lua_State* L)
{
    return LuaRef::newTable(L);
}

//=================================================================================================
/**
 * @brief Create a reference to a value in the global table.
 *
 * This is a syntactic abbreviation for LuaRef::getGlobal ().
 */
[[nodiscard]] inline LuaRef getGlobal(lua_State* L, const char* name)
{
    return LuaRef::getGlobal(L, name);
}

//=================================================================================================
/**
 * @brief C++ like cast syntax.
 */
template <class T>
[[nodiscard]] T cast(const LuaRef& ref)
{
    return ref.cast<T>();
}

} // namespace luabridge

// End File: Source/LuaBridge/detail/LuaRef.h

// Begin File: Source/LuaBridge/detail/Invoke.h


namespace luabridge {

//=================================================================================================
/**
 * @brief Result of a lua invocation.
 */
class LuaResult
{
public:
    /**
     * @brief Get if the result was ok and didn't raise a lua error.
     */
    explicit operator bool() const noexcept
    {
        return !m_ec;
    }

    /**
     * @brief Return if the invocation was ok and didn't raise a lua error.
     */
    bool wasOk() const noexcept
    {
        return !m_ec;
    }

    /**
     * @brief Return if the invocation did raise a lua error.
     */
    bool hasFailed() const noexcept
    {
        return !!m_ec;
    }

    /**
     * @brief Return the error code, if any.
     *
     * In case the invcation didn't raise any lua error, the value returned equals to a
     * default constructed std::error_code.
     */
    std::error_code errorCode() const noexcept
    {
        return m_ec;
    }

    /**
     * @brief Return the error message, if any.
     */
    std::string errorMessage() const noexcept
    {
        if (std::holds_alternative<std::string>(m_data))
        {
            const auto& message = std::get<std::string>(m_data);
            return message.empty() ? m_ec.message() : message;
        }

        return {};
    }

    /**
     * @brief Return the number of return values.
     */
    std::size_t size() const noexcept
    {
        if (std::holds_alternative<std::vector<LuaRef>>(m_data))
            return std::get<std::vector<LuaRef>>(m_data).size();

        return 0;
    }

    /**
     * @brief Get a return value at a specific index.
     */
    LuaRef operator[](std::size_t index) const
    {
        assert(m_ec == std::error_code());

        if (std::holds_alternative<std::vector<LuaRef>>(m_data))
        {
            const auto& values = std::get<std::vector<LuaRef>>(m_data);

            assert(index < values.size());
            return values[index];
        }

        return LuaRef(m_L);
    }

private:
    template <class... Args>
    friend LuaResult call(const LuaRef&, Args&&...);

    static LuaResult errorFromStack(lua_State* L, std::error_code ec)
    {
        auto errorString = lua_tostring(L, -1);
        lua_pop(L, 1);

        return LuaResult(L, ec, errorString ? errorString : ec.message());
    }

    static LuaResult valuesFromStack(lua_State* L, int stackTop)
    {
        std::vector<LuaRef> values;

        const int numReturnedValues = lua_gettop(L) - stackTop;
        if (numReturnedValues > 0)
        {
            values.reserve(numReturnedValues);

            for (int index = numReturnedValues; index > 0; --index)
                values.emplace_back(LuaRef::fromStack(L, -index));

            lua_pop(L, numReturnedValues);
        }

        return LuaResult(L, std::move(values));
    }

    LuaResult(lua_State* L, std::error_code ec, std::string_view errorString)
        : m_L(L)
        , m_ec(ec)
        , m_data(std::string(errorString))
    {
    }

    explicit LuaResult(lua_State* L, std::vector<LuaRef> values) noexcept
        : m_L(L)
        , m_data(std::move(values))
    {
    }

    lua_State* m_L = nullptr;
    std::error_code m_ec;
    std::variant<std::vector<LuaRef>, std::string> m_data;
};

//=================================================================================================
/**
 * @brief Safely call Lua code.
 *
 * These overloads allow Lua code to be called throught lua_pcall.  The return value is provided as
 * a LuaResult which will hold the return values or an error if the call failed.
 *
 * If an error occurs, a LuaException is thrown or if exceptions are disabled the FunctionResult will
 * contain a error code and evaluate false.
 *
 * @note The function might throw a LuaException if the application is compiled with exceptions on
 * and they are enabled globally by calling `enableExceptions` in two cases:
 * - one of the arguments passed cannot be pushed in the stack, for example a unregistered C++ class
 * - the lua invaction calls the panic handler, which is converted to a C++ exception
 *
 * @return A result object.
*/
template <class... Args>
LuaResult call(const LuaRef& object, Args&&... args)
{
    lua_State* L = object.state();
    const int stackTop = lua_gettop(L);

    object.push();

    {
        std::error_code ec;
        auto pushedArgs = detail::push_arguments(L, std::forward_as_tuple(args...), ec);
        if (ec)
        {
            lua_pop(L, static_cast<int>(pushedArgs) + 1);
            return LuaResult(L, ec, ec.message());
        }
    }

    int code = lua_pcall(L, sizeof...(Args), LUA_MULTRET, 0);
    if (code != LUABRIDGE_LUA_OK)
    {
        auto ec = makeErrorCode(ErrorCode::LuaFunctionCallFailed);

#if LUABRIDGE_HAS_EXCEPTIONS
        if (LuaException::areExceptionsEnabled())
            LuaException::raise(L, ec);
#else
        return LuaResult::errorFromStack(L, ec);
#endif
    }

    return LuaResult::valuesFromStack(L, stackTop);
}

//=============================================================================================
/**
 * @brief Wrapper for lua_pcall that throws if exceptions are enabled.
 */
inline int pcall(lua_State* L, int nargs = 0, int nresults = 0, int msgh = 0)
{
    const int code = lua_pcall(L, nargs, nresults, msgh);

#if LUABRIDGE_HAS_EXCEPTIONS
    if (code != LUABRIDGE_LUA_OK && LuaException::areExceptionsEnabled())
        LuaException::raise(L, makeErrorCode(ErrorCode::LuaFunctionCallFailed));
#endif

    return code;
}

//=============================================================================================
template <class Impl, class LuaRef>
template <class... Args>
LuaResult LuaRefBase<Impl, LuaRef>::operator()(Args&&... args) const
{
    return call(*this, std::forward<Args>(args)...);
}

} // namespace luabridge

// End File: Source/LuaBridge/detail/Invoke.h

// Begin File: Source/LuaBridge/detail/Iterator.h


namespace luabridge {

//=================================================================================================
/**
 * @brief Iterator class to allow table iteration.
 *
 * @see Range class.
 */
class Iterator
{
public:
    explicit Iterator(const LuaRef& table, bool isEnd = false)
        : m_L(table.state())
        , m_table(table)
        , m_key(table.state()) // m_key is nil
        , m_value(table.state()) // m_value is nil
    {
        if (! isEnd)
        {
            next(); // get the first (key, value) pair from table
        }
    }

    /**
     * @brief Return an associated Lua state.
     *
     * @return A Lua state.
     */
    lua_State* state() const noexcept
    {
        return m_L;
    }

    /**
     * @brief Dereference the iterator.
     *
     * @return A key-value pair for a current table entry.
     */
    std::pair<LuaRef, LuaRef> operator*() const
    {
        return std::make_pair(m_key, m_value);
    }

    /**
     * @brief Return the value referred by the iterator.
     *
     * @return A value for the current table entry.
     */
    LuaRef operator->() const
    {
        return m_value;
    }

    /**
     * @brief Compare two iterators.
     *
     * @param rhs Another iterator.
     *
     * @return True if iterators point to the same entry of the same table, false otherwise.
     */
    bool operator!=(const Iterator& rhs) const
    {
        assert(m_L == rhs.m_L);

        return ! m_table.rawequal(rhs.m_table) || ! m_key.rawequal(rhs.m_key);
    }

    /**
     * @brief Move the iterator to the next table entry.
     *
     * @return This iterator.
     */
    Iterator& operator++()
    {
        if (isNil())
        {
            // if the iterator reaches the end, do nothing
            return *this;
        }
        else
        {
            next();
            return *this;
        }
    }

    /**
     * @brief Check if the iterator points after the last table entry.
     *
     * @return True if there are no more table entries to iterate, false otherwise.
     */
    bool isNil() const noexcept
    {
        return m_key.isNil();
    }

    /**
     * @brief Return the key for the current table entry.
     *
     * @return A reference to the entry key.
     */
    LuaRef key() const
    {
        return m_key;
    }

    /**
     * @brief Return the key for the current table entry.
     *
     * @return A reference to the entry value.
     */
    LuaRef value() const
    {
        return m_value;
    }

private:
    // Don't use postfix increment, it is less efficient
    Iterator operator++(int);

    void next()
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(m_L, 2))
        {
            m_key = LuaNil();
            m_value = LuaNil();
            return;
        }
#endif

        m_table.push();
        m_key.push();

        if (lua_next(m_L, -2))
        {
            m_value.pop();
            m_key.pop();
        }
        else
        {
            m_key = LuaNil();
            m_value = LuaNil();
        }

        lua_pop(m_L, 1);
    }

    lua_State* m_L = nullptr;
    LuaRef m_table;
    LuaRef m_key;
    LuaRef m_value;
};

//=================================================================================================
/**
 * @brief Range class taking two table iterators.
 */
class Range
{
public:
    Range(const Iterator& begin, const Iterator& end)
        : m_begin(begin)
        , m_end(end)
    {
    }

    const Iterator& begin() const noexcept
    {
        return m_begin;
    }

    const Iterator& end() const noexcept
    {
        return m_end;
    }

private:
    Iterator m_begin;
    Iterator m_end;
};

//=================================================================================================
/**
 * @brief Return a range for the Lua table reference.
 *
 * @return A range suitable for range-based for statement.
 */
inline Range pairs(const LuaRef& table)
{
    return Range{ Iterator(table, false), Iterator(table, true) };
}

} // namespace luabridge

// End File: Source/LuaBridge/detail/Iterator.h

// Begin File: Source/LuaBridge/detail/Security.h


namespace luabridge {

//=================================================================================================
/**
 * @brief Security options.
 */
class Security
{
public:
    static bool hideMetatables() noexcept
    {
        return getSettings().hideMetatables;
    }

    static void setHideMetatables(bool shouldHide) noexcept
    {
        getSettings().hideMetatables = shouldHide;
    }

private:
    struct Settings
    {
        Settings() noexcept
            : hideMetatables(true)
        {
        }

        bool hideMetatables;
    };

    static Settings& getSettings() noexcept
    {
        static Settings settings;
        return settings;
    }
};

//=================================================================================================
/**
 * @brief Get a global value from the lua_State.
 *
 * @note This works on any type specialized by `Stack`, including `LuaRef` and its table proxies.
*/
template <class T>
T getGlobal(lua_State* L, const char* name)
{
    lua_getglobal(L, name);

    auto result = luabridge::Stack<T>::get(L, -1);
    
    lua_pop(L, 1);
    
    return result;
}

//=================================================================================================
/**
 * @brief Set a global value in the lua_State.
 *
 * @note This works on any type specialized by `Stack`, including `LuaRef` and its table proxies.
*/
template <class T>
bool setGlobal(lua_State* L, T&& t, const char* name)
{
    std::error_code ec;
    if (push(L, std::forward<T>(t), ec))
    {
        lua_setglobal(L, name);
        return true;
    }

    return false;
}

//=================================================================================================
/**
 * @brief Change whether or not metatables are hidden (on by default).
 */
inline void setHideMetatables(bool shouldHide) noexcept
{
    Security::setHideMetatables(shouldHide);
}

} // namespace luabridge

// End File: Source/LuaBridge/detail/Security.h

// Begin File: Source/LuaBridge/detail/Namespace.h


namespace luabridge {
namespace detail {

//=================================================================================================
/**
 * @brief Base for class and namespace registration.
 *
 * Maintains Lua stack in the proper state. Once beginNamespace, beginClass or deriveClass is called the parent object upon its destruction
 * may no longer clear the Lua stack.
 *
 * Then endNamespace or endClass is called, a new parent is created and the child transfers the responsibility for clearing stack to it.
 *
 * So there can be maximum one "active" registrar object.
 */
class Registrar
{
protected:
    Registrar(lua_State* L)
        : L(L)
        , m_stackSize(0)
    {
    }

    Registrar(lua_State* L, int skipStackPops)
        : L(L)
        , m_stackSize(0)
        , m_skipStackPops(skipStackPops)
    {
    }

    Registrar(const Registrar& rhs)
        : L(rhs.L)
        , m_stackSize(std::exchange(rhs.m_stackSize, 0))
        , m_skipStackPops(std::exchange(rhs.m_skipStackPops, 0))
    {
    }

    Registrar& operator=(const Registrar& rhs)
    {
        m_stackSize = rhs.m_stackSize;
        m_skipStackPops = rhs.m_skipStackPops;

        return *this;
    }

    ~Registrar()
    {
        const int popsCount = m_stackSize - m_skipStackPops;
        if (popsCount > 0)
        {
            assert(popsCount <= lua_gettop(L));

            lua_pop(L, popsCount);
        }
    }

    void assertIsActive() const
    {
        if (m_stackSize == 0)
        {
            throw_or_assert<std::logic_error>("Unable to continue registration");
        }
    }

    lua_State* const L = nullptr;
    int mutable m_stackSize = 0;
    int mutable m_skipStackPops = 0;
};

} // namespace detail

//=================================================================================================
/**
 * @brief Provides C++ to Lua registration capabilities.
 *
 * This class is not instantiated directly, call `getGlobalNamespace` to start the registration process.
 */
class Namespace : public detail::Registrar
{
    //=============================================================================================
#if 0
    /**
     * @brief Error reporting.
     *
     * This function looks handy, why aren't we using it?
     */
    static int luaError(lua_State* L, std::string message)
    {
        assert(lua_isstring(L, lua_upvalueindex(1)));
        std::string s;

        // Get information on the caller's caller to format the message,
        // so the error appears to originate from the Lua source.
        lua_Debug ar;
    
        int result = lua_getstack(L, 2, &ar);
        if (result != 0)
        {
            lua_getinfo(L, "Sl", &ar);
            s = ar.short_src;
            if (ar.currentline != -1)
            {
                // poor mans int to string to avoid <strstrream>.
                lua_pushnumber(L, ar.currentline);
                s = s + ":" + lua_tostring(L, -1) + ": ";
                lua_pop(L, 1);
            }
        }

        s = s + message;

        luaL_error(L, "%s", s.c_str());

        return 0;
    }
#endif

    //=============================================================================================
    /**
     * @brief Factored base to reduce template instantiations.
     */
    class ClassBase : public detail::Registrar
    {
    public:
        explicit ClassBase(Namespace& parent)
            : Registrar(parent)
        {
        }

        using Registrar::operator=;

    protected:
        //=========================================================================================
        /**
         * @brief Create the const table.
         */
        void createConstTable(const char* name, bool trueConst = true)
        {
            assert(name != nullptr);

            std::string type_name = std::string(trueConst ? "const " : "") + name;

            // Stack: namespace table (ns)
            lua_newtable(L); // Stack: ns, const table (co)
            lua_pushvalue(L, -1); // Stack: ns, co, co
            lua_setmetatable(L, -2); // co.__metatable = co. Stack: ns, co

            lua_pushstring(L, type_name.c_str());
            lua_rawsetp(L, -2, detail::getTypeKey()); // co [typeKey] = name. Stack: ns, co

            lua_pushcfunction_x(L, &detail::index_metamethod);
            rawsetfield(L, -2, "__index");

            lua_pushcfunction_x(L, &detail::newindex_object_metamethod);
            rawsetfield(L, -2, "__newindex");

            lua_newtable(L);
            lua_rawsetp(L, -2, detail::getPropgetKey());

            if (Security::hideMetatables())
            {
                lua_pushnil(L);
                rawsetfield(L, -2, "__metatable");
            }
        }

        //=========================================================================================
        /**
         * @brief Create the class table.
         *
         * The Lua stack should have the const table on top.
         */
        void createClassTable(const char* name)
        {
            assert(name != nullptr);

            // Stack: namespace table (ns), const table (co)

            // Class table is the same as const table except the propset table
            createConstTable(name, false); // Stack: ns, co, cl

            lua_newtable(L); // Stack: ns, co, cl, propset table (ps)
            lua_rawsetp(L, -2, detail::getPropsetKey()); // cl [propsetKey] = ps. Stack: ns, co, cl

            lua_pushvalue(L, -2); // Stack: ns, co, cl, co
            lua_rawsetp(L, -2, detail::getConstKey()); // cl [constKey] = co. Stack: ns, co, cl

            lua_pushvalue(L, -1); // Stack: ns, co, cl, cl
            lua_rawsetp(L, -3, detail::getClassKey()); // co [classKey] = cl. Stack: ns, co, cl
        }

        //=========================================================================================
        /**
         * @brief Create the static table.
         */
        void createStaticTable(const char* name)
        {
            assert(name != nullptr);

            // Stack: namespace table (ns), const table (co), class table (cl)
            lua_newtable(L); // Stack: ns, co, cl, visible static table (vst)
            lua_newtable(L); // Stack: ns, co, cl, st, static metatable (st)
            lua_pushvalue(L, -1); // Stack: ns, co, cl, vst, st, st
            lua_setmetatable(L, -3); // st.__metatable = mt. Stack: ns, co, cl, vst, st
            lua_insert(L, -2); // Stack: ns, co, cl, st, vst
            rawsetfield(L, -5, name); // ns [name] = vst. Stack: ns, co, cl, st

#if 0
            lua_pushlightuserdata(L, this);
            lua_pushcclosure_x(L, &tostringMetaMethod, 1);
            rawsetfield(L, -2, "__tostring");
#endif

            lua_pushcfunction_x(L, &detail::index_metamethod);
            rawsetfield(L, -2, "__index");

            lua_pushcfunction_x(L, &detail::newindex_static_metamethod);
            rawsetfield(L, -2, "__newindex");

            lua_newtable(L); // Stack: ns, co, cl, st, proget table (pg)
            lua_rawsetp(L, -2, detail::getPropgetKey()); // st [propgetKey] = pg. Stack: ns, co, cl, st

            lua_newtable(L); // Stack: ns, co, cl, st, propset table (ps)
            lua_rawsetp(L, -2, detail::getPropsetKey()); // st [propsetKey] = pg. Stack: ns, co, cl, st

            lua_pushvalue(L, -2); // Stack: ns, co, cl, st, cl
            lua_rawsetp(L, -2, detail::getClassKey()); // st [classKey] = cl. Stack: ns, co, cl, st

            if (Security::hideMetatables())
            {
                lua_pushnil(L);
                rawsetfield(L, -2, "__metatable");
            }
        }

        //=========================================================================================
        /**
         * @brief lua_CFunction to construct a class object wrapped in a container.
         */
        template <class Args, class C>
        static int ctorContainerProxy(lua_State* L)
        {
            using T = typename ContainerTraits<C>::Type;

            T* object = detail::constructor<T, Args>::call(detail::make_arguments_list<Args, 2>(L));

            std::error_code ec;
            if (! detail::UserdataSharedHelper<C, false>::push(L, object, ec))
                luaL_error(L, "%s", ec.message().c_str());

            return 1;
        }

        //=========================================================================================
        /**
         * @brief lua_CFunction to construct a class object in-place in the userdata.
         */
        template <class Args, class T>
        static int ctorPlacementProxy(lua_State* L)
        {
            std::error_code ec;
            detail::UserdataValue<T>* value = detail::UserdataValue<T>::place(L, ec);
            if (! value)
                luaL_error(L, "%s", ec.message().c_str());

            detail::constructor<T, Args>::call(value->getObject(), detail::make_arguments_list<Args, 2>(L));

            value->commit();

            return 1;
        }

        //=========================================================================================
        /**
         * @brief Asserts on stack state.
         */
        void assertStackState() const
        {
            // Stack: const table (co), class table (cl), static table (st)
            assert(lua_istable(L, -3));
            assert(lua_istable(L, -2));
            assert(lua_istable(L, -1));
        }
    };

    //=============================================================================================
    /**
     * @brief Provides a class registration in a lua_State.
     *
     * After construction the Lua stack holds these objects:
     *   -1 static table
     *   -2 class table
     *   -3 const table
     *   -4 enclosing namespace table
     */
    template <class T>
    class Class : public ClassBase
    {
    public:
        //=========================================================================================

        /**
         * @brief Register a new class or add to an existing class registration.
         *
         * @param name   The new class name.
         * @param parent A parent namespace object.
         */
        Class(const char* name, Namespace& parent)
            : ClassBase(parent)
        {
            assert(name != nullptr);
            assert(lua_istable(L, -1)); // Stack: namespace table (ns)

            rawgetfield(L, -1, name); // Stack: ns, static table (st) | nil

            if (lua_isnil(L, -1)) // Stack: ns, nil
            {
                lua_pop(L, 1); // Stack: ns

                createConstTable(name); // Stack: ns, const table (co)
#if !defined(LUABRIDGE_ON_LUAU)
                lua_pushcfunction_x(L, &detail::gc_metamethod<T>); // Stack: ns, co, function
                rawsetfield(L, -2, "__gc"); // co ["__gc"] = function. Stack: ns, co
#endif
                ++m_stackSize;

                createClassTable(name); // Stack: ns, co, class table (cl)
#if !defined(LUABRIDGE_ON_LUAU)
                lua_pushcfunction_x(L, &detail::gc_metamethod<T>); // Stack: ns, co, cl, function
                rawsetfield(L, -2, "__gc"); // cl ["__gc"] = function. Stack: ns, co, cl
#endif
                ++m_stackSize;

                createStaticTable(name); // Stack: ns, co, cl, st
                ++m_stackSize;

                // Map T back to its tables.
                lua_pushvalue(L, -1); // Stack: ns, co, cl, st, st
                lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getStaticRegistryKey<T>()); // Stack: ns, co, cl, st
                lua_pushvalue(L, -2); // Stack: ns, co, cl, st, cl
                lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getClassRegistryKey<T>()); // Stack: ns, co, cl, st
                lua_pushvalue(L, -3); // Stack: ns, co, cl, st, co
                lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getConstRegistryKey<T>()); // Stack: ns, co, cl, st
            }
            else
            {
                assert(lua_istable(L, -1)); // Stack: ns, st
                ++m_stackSize;

                // Map T back from its stored tables

                lua_rawgetp(L, LUA_REGISTRYINDEX, detail::getConstRegistryKey<T>()); // Stack: ns, st, co
                lua_insert(L, -2); // Stack: ns, co, st
                ++m_stackSize;

                lua_rawgetp(L, LUA_REGISTRYINDEX, detail::getClassRegistryKey<T>()); // Stack: ns, co, st, cl
                lua_insert(L, -2); // Stack: ns, co, cl, st
                ++m_stackSize;
            }
        }

        //=========================================================================================
        /**
         * @brief Derive a new class.
         *
         * @param name The class name.
         * @param parent A parent namespace object.
         * @param staticKey Key where the class is stored.
        */
        Class(const char* name, Namespace& parent, void const* const staticKey)
            : ClassBase(parent)
        {
            assert(name != nullptr);
            assert(lua_istable(L, -1)); // Stack: namespace table (ns)

            createConstTable(name); // Stack: ns, const table (co)
#if !defined(LUABRIDGE_ON_LUAU)
            lua_pushcfunction_x(L, &detail::gc_metamethod<T>); // Stack: ns, co, function
            rawsetfield(L, -2, "__gc"); // co ["__gc"] = function. Stack: ns, co
#endif
            ++m_stackSize;

            createClassTable(name); // Stack: ns, co, class table (cl)
#if !defined(LUABRIDGE_ON_LUAU)
            lua_pushcfunction_x(L, &detail::gc_metamethod<T>); // Stack: ns, co, cl, function
            rawsetfield(L, -2, "__gc"); // cl ["__gc"] = function. Stack: ns, co, cl
#endif
            ++m_stackSize;

            createStaticTable(name); // Stack: ns, co, cl, st
            ++m_stackSize;

            lua_rawgetp(L, LUA_REGISTRYINDEX, staticKey); // Stack: ns, co, cl, st, parent st (pst) | nil
            if (lua_isnil(L, -1)) // Stack: ns, co, cl, st, nil
            {
                lua_pop(L, 1);

                throw_or_assert<std::logic_error>("Base class is not registered");
                return;
            }

            assert(lua_istable(L, -1)); // Stack: ns, co, cl, st, pst

            lua_rawgetp(L, -1, detail::getClassKey()); // Stack: ns, co, cl, st, pst, parent cl (pcl)
            assert(lua_istable(L, -1));

            lua_rawgetp(L, -1, detail::getConstKey()); // Stack: ns, co, cl, st, pst, pcl, parent co (pco)
            assert(lua_istable(L, -1));

            lua_rawsetp(L, -6, detail::getParentKey()); // co [parentKey] = pco. Stack: ns, co, cl, st, pst, pcl
            lua_rawsetp(L, -4, detail::getParentKey()); // cl [parentKey] = pcl. Stack: ns, co, cl, st, pst
            lua_rawsetp(L, -2, detail::getParentKey()); // st [parentKey] = pst. Stack: ns, co, cl, st

            lua_pushvalue(L, -1); // Stack: ns, co, cl, st, st
            lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getStaticRegistryKey<T>()); // Stack: ns, co, cl, st
            lua_pushvalue(L, -2); // Stack: ns, co, cl, st, cl
            lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getClassRegistryKey<T>()); // Stack: ns, co, cl, st
            lua_pushvalue(L, -3); // Stack: ns, co, cl, st, co
            lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getConstRegistryKey<T>()); // Stack: ns, co, cl, st
        }

        //=========================================================================================
        /**
         * @brief Continue registration in the enclosing namespace.
         *
         * @returns A parent registration object.
         */
        Namespace endClass()
        {
            assert(m_stackSize > 3);

            m_stackSize -= 3;
            lua_pop(L, 3);
            return Namespace(*this);
        }

        //=========================================================================================
        /**
         * @brief Add or replace a static property.
         *
         * @tparam U The type of the property.
         *
         * @param name The property name.
         * @param value A property value pointer.
         * @param isWritable True for a read-write, false for read-only property.
         *
         * @returns This class registration object.
         */
        template <class U, typename = std::enable_if_t<!std::is_invocable_v<U>>>
        Class<T>& addStaticProperty(const char* name, const U* value)
        {
            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushlightuserdata(L, const_cast<U*>(value)); // Stack: co, cl, st, pointer
            lua_pushcclosure_x(L, &detail::property_getter<U>::call, 1); // Stack: co, cl, st, getter
            detail::add_property_getter(L, name, -2); // Stack: co, cl, st

            lua_pushstring(L, name); // Stack: co, cl, st, name
            lua_pushcclosure_x(L, &detail::read_only_error, 1); // Stack: co, cl, st, function

            detail::add_property_setter(L, name, -2); // Stack: co, cl, st

            return *this;
        }

        /**
         * @brief Add or replace a static property.
         *
         * @tparam U The type of the property.
         *
         * @param name The property name.
         * @param value A property value pointer.
         * @param isWritable True for a read-write, false for read-only property.
         *
         * @returns This class registration object.
         */
        template <class U, typename = std::enable_if_t<!std::is_invocable_v<U>>>
        Class<T>& addStaticProperty(const char* name, U* value, bool isWritable = true)
        {
            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushlightuserdata(L, value); // Stack: co, cl, st, pointer
            lua_pushcclosure_x(L, &detail::property_getter<U>::call, 1); // Stack: co, cl, st, getter
            detail::add_property_getter(L, name, -2); // Stack: co, cl, st

            if (isWritable)
            {
                lua_pushlightuserdata(L, value); // Stack: co, cl, st, ps, pointer
                lua_pushcclosure_x(L, &detail::property_setter<U>::call, 1); // Stack: co, cl, st, ps, setter
            }
            else
            {
                lua_pushstring(L, name); // Stack: co, cl, st, name
                lua_pushcclosure_x(L, &detail::read_only_error, 1); // Stack: co, cl, st, function
            }

            detail::add_property_setter(L, name, -2); // Stack: co, cl, st

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a static property member.
         *
         * @tparam U The type of the property.
         *
         * @param name The property name.
         * @param get A property getter function pointer.
         * @param set A property setter function pointer, optional, nullable. Omit or pass nullptr for a read-only property.
         *
         * @returns This class registration object.
         */
        template <class U>
        Class<T>& addStaticProperty(const char* name, U (*get)(), void (*set)(U) = nullptr)
        {
            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushlightuserdata(L, reinterpret_cast<void*>(get)); // Stack: co, cl, st, function ptr
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<U (*)()>, 1); // Stack: co, cl, st, getter
            detail::add_property_getter(L, name, -2); // Stack: co, cl, st

            if (set != nullptr)
            {
                lua_pushlightuserdata(L, reinterpret_cast<void*>(set)); // Stack: co, cl, st, function ptr
                lua_pushcclosure_x(L, &detail::invoke_proxy_function<void (*)(U)>, 1); // Stack: co, cl, st, setter
            }
            else
            {
                lua_pushstring(L, name); // Stack: co, cl, st, ps, name
                lua_pushcclosure_x(L, &detail::read_only_error, 1); // Stack: co, cl, st, function
            }

            detail::add_property_setter(L, name, -2); // Stack: co, cl, st

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a static property, by constructible by std::function.
         */
        template <class Getter, typename = std::enable_if_t<!std::is_pointer_v<Getter>>>
        Class<T> addStaticProperty(const char* name, Getter get)
        {
            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            using GetType = decltype(get);

            lua_newuserdata_aligned<GetType>(L, std::move(get)); // Stack: co, cl, st, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<GetType>, 1); // Stack: co, cl, st, function
            detail::add_property_getter(L, name, -2); // Stack: co, cl, st

            return *this;
        }

        template <class Getter, class Setter, typename = std::enable_if_t<!std::is_pointer_v<Getter> && !std::is_pointer_v<Setter>>>
        Class<T> addStaticProperty(const char* name, Getter get, Setter set)
        {
            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            using GetType = decltype(get);
            using SetType = decltype(set);

            lua_newuserdata_aligned<GetType>(L, std::move(get)); // Stack: co, cl, st, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<GetType>, 1); // Stack: co, cl, st, function
            detail::add_property_getter(L, name, -2); // Stack: co, cl, st

            lua_newuserdata_aligned<SetType>(L, std::move(set)); // Stack: co, cl, st, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<SetType>, 1); // Stack: co, cl, st, function
            detail::add_property_setter(L, name, -2); // Stack: co, cl, st

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a static member function.
         */
        template <class Function, typename = std::enable_if_t<std::is_pointer_v<Function>>>
        Class<T>& addStaticFunction(const char* name, Function fp)
        {
            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushlightuserdata(L, reinterpret_cast<void*>(fp)); // Stack: co, cl, st, function ptr
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<Function>, 1); // co, cl, st, function
            rawsetfield(L, -2, name); // co, cl, st

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a static member function for constructible by std::function.
         */
        template <class Function, typename = std::enable_if_t<!std::is_pointer_v<Function>>>
        Class<T> addStaticFunction(const char* name, Function function)
        {
            using FnType = decltype(function);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_newuserdata_aligned<FnType>(L, std::move(function)); // Stack: co, cl, st, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<FnType>, 1); // Stack: co, cl, st, function
            rawsetfield(L, -2, name); // Stack: co, cl, st

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a lua_CFunction.
         *
         * @param name The name of the function.
         * @param fp   A C-function pointer.
         *
         * @returns This class registration object.
         */
        Class<T>& addStaticFunction(const char* name, lua_CFunction fp)
        {
            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushcfunction_x(L, fp); // co, cl, st, function
            rawsetfield(L, -2, name); // co, cl, st

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a property member.
         */
        template <class U, class V>
        Class<T>& addProperty(const char* name, U V::*mp, bool isWritable = true)
        {
            static_assert(std::is_base_of_v<V, T>);

            using MemberPtrType = decltype(mp);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            new (lua_newuserdata_x<MemberPtrType>(L, sizeof(MemberPtrType))) MemberPtrType(mp); // Stack: co, cl, st, field ptr
            lua_pushcclosure_x(L, &detail::property_getter<U, T>::call, 1); // Stack: co, cl, st, getter
            lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
            detail::add_property_getter(L, name, -5); // Stack: co, cl, st, getter
            detail::add_property_getter(L, name, -3); // Stack: co, cl, st

            if (isWritable)
            {
                new (lua_newuserdata_x<MemberPtrType>(L, sizeof(MemberPtrType))) MemberPtrType(mp); // Stack: co, cl, st, field ptr
                lua_pushcclosure_x(L, &detail::property_setter<U, T>::call, 1); // Stack: co, cl, st, setter
                detail::add_property_setter(L, name, -3); // Stack: co, cl, st
            }

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a property member.
         */
        template <class TG, class TS = TG>
        Class<T>& addProperty(const char* name, TG (T::*get)() const, void (T::*set)(TS) = nullptr)
        {
            using GetType = TG (T::*)() const;
            using SetType = void (T::*)(TS);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            new (lua_newuserdata_x<GetType>(L, sizeof(GetType))) GetType(get); // Stack: co, cl, st, funcion ptr
            lua_pushcclosure_x(L, &detail::invoke_const_member_function<GetType, T>, 1); // Stack: co, cl, st, getter
            lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
            detail::add_property_getter(L, name, -5); // Stack: co, cl, st, getter
            detail::add_property_getter(L, name, -3); // Stack: co, cl, st

            if (set != nullptr)
            {
                new (lua_newuserdata_x<SetType>(L, sizeof(SetType))) SetType(set); // Stack: co, cl, st, function ptr
                lua_pushcclosure_x(L, &detail::invoke_member_function<SetType, T>, 1); // Stack: co, cl, st, setter
                detail::add_property_setter(L, name, -3); // Stack: co, cl, st
            }

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a property member.
         */
        template <class TG, class TS = TG>
        Class<T>& addProperty(const char* name, TG (T::*get)(lua_State*) const, void (T::*set)(TS, lua_State*) = nullptr)
        {
            using GetType = TG (T::*)(lua_State*) const;
            using SetType = void (T::*)(TS, lua_State*);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            new (lua_newuserdata_x<GetType>(L, sizeof(GetType))) GetType(get); // Stack: co, cl, st, funcion ptr
            lua_pushcclosure_x(L, &detail::invoke_const_member_function<GetType, T>, 1); // Stack: co, cl, st, getter
            lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
            detail::add_property_getter(L, name, -5); // Stack: co, cl, st, getter
            detail::add_property_getter(L, name, -3); // Stack: co, cl, st

            if (set != nullptr)
            {
                new (lua_newuserdata_x<SetType>(L, sizeof(SetType))) SetType(set); // Stack: co, cl, st, function ptr
                lua_pushcclosure_x(L, &detail::invoke_member_function<SetType, T>, 1); // Stack: co, cl, st, setter
                detail::add_property_setter(L, name, -3); // Stack: co, cl, st
            }

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a property member, by proxy.
         *
         * When a class is closed for modification and does not provide (or cannot provide) the function signatures necessary to implement
         * get or set for a property, this will allow non-member functions act as proxies.
         *
         * Both the get and the set functions require a T const* and T* in the first argument respectively.
         */
        template <class TG, class TS = TG>
        Class<T>& addProperty(const char* name, TG (*get)(T const*), void (*set)(T*, TS) = nullptr)
        {
            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushlightuserdata(L, reinterpret_cast<void*>(get)); // Stack: co, cl, st, function ptr
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<TG (*)(const T*)>, 1); // Stack: co, cl, st, getter
            lua_pushvalue(L, -1); // Stack: co, cl, st,, getter, getter
            detail::add_property_getter(L, name, -5); // Stack: co, cl, st, getter
            detail::add_property_getter(L, name, -3); // Stack: co, cl, st

            if (set != nullptr)
            {
                lua_pushlightuserdata( L, reinterpret_cast<void*>(set)); // Stack: co, cl, st, function ptr
                lua_pushcclosure_x(L, &detail::invoke_proxy_function<void (*)(T*, TS)>, 1); // Stack: co, cl, st, setter
                detail::add_property_setter(L, name, -3); // Stack: co, cl, st
            }

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a property member, by proxy C-function.
         *
         * When a class is closed for modification and does not provide (or cannot provide) the function signatures necessary to implement
         * get or set for a property, this will allow non-member functions act as proxies.
         *
         * The object userdata ('this') value is at the index 1.
         * The new value for set function is at the index 2.
         */
        Class<T>& addProperty(const char* name, lua_CFunction get, lua_CFunction set = nullptr)
        {
            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushcfunction_x(L, get);
            lua_pushvalue(L, -1); // Stack: co, cl, st,, getter, getter
            detail::add_property_getter(L, name, -5); // Stack: co, cl, st,, getter
            detail::add_property_getter(L, name, -3); // Stack: co, cl, st,

            if (set != nullptr)
            {
                lua_pushcfunction_x(L, set);
                detail::add_property_setter(L, name, -3); // Stack: co, cl, st,
            }

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a property member, by constructible by std::function.
         */
        template <class Getter, typename = std::enable_if_t<!std::is_pointer_v<Getter>>>
        Class<T>& addProperty(const char* name, Getter get)
        {
            using FirstArg = detail::function_argument_t<0, Getter>;
            static_assert(std::is_same_v<std::decay_t<std::remove_pointer_t<FirstArg>>, T>);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            using GetType = decltype(get);

            lua_newuserdata_aligned<GetType>(L, std::move(get)); // Stack: co, cl, st, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<GetType>, 1); // Stack: co, cl, st, getter
            lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
            detail::add_property_getter(L, name, -4); // Stack: co, cl, st, getter
            detail::add_property_getter(L, name, -4); // Stack: co, cl, st

            return *this;
        }

        template <class Getter, class Setter, typename = std::enable_if_t<!std::is_pointer_v<Getter> && !std::is_pointer_v<Setter>>>
        Class<T>& addProperty(const char* name, Getter get, Setter set)
        {
            addProperty<Getter>(name, std::move(get));

            using FirstArg = detail::function_argument_t<0, Setter>;
            static_assert(std::is_same_v<std::decay_t<std::remove_pointer_t<FirstArg>>, T>);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            using SetType = decltype(set);

            lua_newuserdata_aligned<SetType>(L, std::move(set)); // Stack: co, cl, st, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<SetType>, 1); // Stack: co, cl, st, setter
            detail::add_property_setter(L, name, -3); // Stack: co, cl, st

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a namespace function by convertible to std::function (capturing lambdas).
         */
        template <class Function, typename = std::enable_if_t<detail::function_arity_v<Function> != 0>>
        Class<T> addFunction(const char* name, Function function)
        {
            using FnType = decltype(function);

            using FirstArg = detail::function_argument_t<0, Function>;
            static_assert(std::is_same_v<std::decay_t<std::remove_pointer_t<FirstArg>>, T>);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            if (name == std::string_view("__gc"))
            {
                throw_or_assert<std::logic_error>("__gc metamethod registration is forbidden");
                return *this;
            }

            lua_newuserdata_aligned<FnType>(L, std::move(function)); // Stack: co, cl, st, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<FnType>, 1); // Stack: co, cl, st, function

            if constexpr (! std::is_const_v<std::remove_reference_t<std::remove_pointer_t<FirstArg>>>)
            {
                rawsetfield(L, -3, name); // Stack: co, cl, st
            }
            else
            {
                lua_pushvalue(L, -1); // Stack: co, cl, st, function, function
                rawsetfield(L, -4, name); // Stack: co, cl, st, function
                rawsetfield(L, -4, name); // Stack: co, cl, st
            }

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a member function.
         */
        template <class U, class ReturnType, class... Params>
        Class<T>& addFunction(const char* name, ReturnType (U::*mf)(Params...))
        {
            static_assert(std::is_base_of_v<U, T>);

            using MemFn = decltype(mf);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            if (name == std::string_view("__gc"))
            {
                throw_or_assert<std::logic_error>("__gc metamethod registration is forbidden");
                return *this;
            }

            new (lua_newuserdata_x<MemFn>(L, sizeof(MemFn))) MemFn(mf);
            lua_pushcclosure_x(L, &detail::invoke_member_function<MemFn, T>, 1);
            rawsetfield(L, -3, name); // class table

            return *this;
        }

        template <class U, class ReturnType, class... Params>
        Class<T>& addFunction(const char* name, ReturnType (U::*mf)(Params...) const)
        {
            static_assert(std::is_base_of_v<U, T>);

            using MemFn = decltype(mf);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            if (name == std::string_view("__gc"))
            {
                throw_or_assert<std::logic_error>("__gc metamethod registration is forbidden");
                return *this;
            }

            new (lua_newuserdata_x<MemFn>(L, sizeof(MemFn))) MemFn(mf);
            lua_pushcclosure_x(L, &detail::invoke_const_member_function<MemFn, T>, 1);
            lua_pushvalue(L, -1);
            rawsetfield(L, -5, name); // const table
            rawsetfield(L, -3, name); // class table

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a proxy function.
         */
        template <class ReturnType, class... Params>
        Class<T>& addFunction(const char* name, ReturnType (*proxyFn)(T* object, Params...))
        {
            using FnType = decltype(proxyFn);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            if (name == std::string_view("__gc"))
            {
                throw_or_assert<std::logic_error>("__gc metamethod registration is forbidden");
                return *this;
            }

            lua_pushlightuserdata(L, reinterpret_cast<void*>(proxyFn)); // Stack: co, cl, st, function ptr
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<FnType>, 1); // Stack: co, cl, st, function
            rawsetfield(L, -3, name); // Stack: co, cl, st

            return *this;
        }

        template <class ReturnType, class... Params>
        Class<T>& addFunction(const char* name, ReturnType (*proxyFn)(const T* object, Params...))
        {
            using FnType = decltype(proxyFn);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            if (name == std::string_view("__gc"))
            {
                throw_or_assert<std::logic_error>("__gc metamethod registration is forbidden");
                return *this;
            }

            lua_pushlightuserdata(L, reinterpret_cast<void*>(proxyFn)); // Stack: co, cl, st, function ptr
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<FnType>, 1); // Stack: co, cl, st, function
            lua_pushvalue(L, -1); // Stack: co, cl, st, function, function
            rawsetfield(L, -4, name); // Stack: co, cl, st, function
            rawsetfield(L, -4, name); // Stack: co, cl, st

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a member lua_CFunction.
         */
        template <class U>
        Class<T>& addFunction(const char* name, int (U::*mfp)(lua_State*))
        {
            static_assert(std::is_base_of_v<U, T>);

            using F = decltype(mfp);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            if (name == std::string_view("__gc"))
            {
                throw_or_assert<std::logic_error>("__gc metamethod registration is forbidden");
                return *this;
            }

            new (lua_newuserdata_x<F>(L, sizeof(mfp))) F(mfp); // Stack: co, cl, st, function ptr
            lua_pushcclosure_x(L, &detail::invoke_member_cfunction<T>, 1); // Stack: co, cl, st, function
            rawsetfield(L, -3, name); // Stack: co, cl, st

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a const member lua_CFunction.
         */
        template <class U>
        Class<T>& addFunction(const char* name, int (U::*mfp)(lua_State*) const)
        {
            static_assert(std::is_base_of_v<U, T>);

            using F = decltype(mfp);

            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            if (name == std::string_view("__gc"))
            {
                throw_or_assert<std::logic_error>("__gc metamethod registration is forbidden");
                return *this;
            }

            new (lua_newuserdata_x<F>(L, sizeof(mfp))) F(mfp);
            lua_pushcclosure_x(L, &detail::invoke_const_member_cfunction<T>, 1);
            lua_pushvalue(L, -1); // Stack: co, cl, st, function, function
            rawsetfield(L, -4, name); // Stack: co, cl, st, function
            rawsetfield(L, -4, name); // Stack: co, cl, st

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a free lua_CFunction that works as a member.
         *
         * This object is at top of the stack, then all other arguments.
         */
        Class<T>& addFunction(const char* name, lua_CFunction fp)
        {
            assert(name != nullptr);
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            if (name == std::string_view("__gc"))
            {
                throw_or_assert<std::logic_error>("__gc metamethod registration is forbidden");
                return *this;
            }

            lua_pushcfunction_x(L, fp); // Stack: co, cl, st, function
            rawsetfield(L, -3, name); // Stack: co, cl, st

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a primary Constructor.
         *
         * The primary Constructor is invoked when calling the class type table like a function.
         *
         * The template parameter should be a function pointer type that matches the desired Constructor (since you can't take the
         * address of a Constructor and pass it as an argument).
         */
        template <class MemFn, class C>
        Class<T>& addConstructor()
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushcclosure_x(L, &ctorContainerProxy<detail::function_arguments_t<MemFn>, C>, 0);
            rawsetfield(L, -2, "__call");

            return *this;
        }

        template <class MemFn>
        Class<T>& addConstructor()
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushcclosure_x(L, &ctorPlacementProxy<detail::function_arguments_t<MemFn>, T>, 0);
            rawsetfield(L, -2, "__call");

            return *this;
        }

        //=========================================================================================
        /**
         * @brief Add or replace a factory.
         *
         * The primary Constructor is invoked when calling the class type table like a function.
         *
         * The template parameter should be a function pointer type that matches the desired Constructor (since you can't take the
         * address of a Constructor and pass it as an argument).
         */
        template <class Function>
        Class<T> addConstructor(Function function)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            auto factory = [function = std::move(function)](lua_State* L) -> T*
            {
                std::error_code ec;
                detail::UserdataValue<T>* value = detail::UserdataValue<T>::place(L, ec);
                if (! value)
                    luaL_error(L, "%s", ec.message().c_str());

                using FnTraits = detail::function_traits<Function>;
                using FnArgs = detail::remove_first_type_t<typename FnTraits::argument_types>;

                T* obj = detail::factory<T>::call(value->getObject(), function, detail::make_arguments_list<FnArgs, 2>(L));

                value->commit();

                return obj;
            };

            using FactoryFnType = decltype(factory);

            lua_newuserdata_aligned<FactoryFnType>(L, std::move(factory)); // Stack: co, cl, st, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<FactoryFnType>, 1); // Stack: co, cl, st, function
            rawsetfield(L, -2, "__call"); // Stack: co, cl, st

            return *this;
        }
    };

    class Table : public detail::Registrar
    {
    public:
        explicit Table(const char* name, Namespace& parent)
            : Registrar(parent)
        {
            lua_newtable(L); // Stack: ns, table (tb)
            lua_pushvalue(L, -1); // Stack: ns, tb, tb
            rawsetfield(L, -3, name);
            ++m_stackSize;

            lua_newtable(L); // Stack: ns, table (tb)
            lua_pushvalue(L, -1); // Stack: ns, tb, tb
            lua_setmetatable(L, -3); // tb.__metatable = tb. Stack: ns, tb
            ++m_stackSize;
        }

        using Registrar::operator=;

        template <class Function>
        Table& addFunction(const char* name, Function function)
        {
            using FnType = decltype(function);

            assert(name != nullptr);
            assert(lua_istable(L, -1)); // Stack: namespace table (ns)

            lua_newuserdata_aligned<FnType>(L, std::move(function)); // Stack: ns, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<FnType>, 1); // Stack: ns, function
            rawsetfield(L, -3, name); // Stack: ns

            return *this;
        }

        template <class Function>
        Table& addMetaFunction(const char* name, Function function)
        {
            using FnType = decltype(function);

            assert(name != nullptr);
            assert(lua_istable(L, -1)); // Stack: namespace table (ns)

            lua_newuserdata_aligned<FnType>(L, std::move(function)); // Stack: ns, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<FnType>, 1); // Stack: ns, function
            rawsetfield(L, -2, name); // Stack: ns

            return *this;
        }

        Namespace endTable()
        {
            assert(m_stackSize > 2);

            m_stackSize -= 2;
            lua_pop(L, 2);
            return Namespace(*this);
        }
    };
    
private:
    struct FromStack {};

    //=============================================================================================
    /**
     * @brief Open the global namespace for registrations.
     *
     * @param L A Lua state.
     */
    explicit Namespace(lua_State* L)
        : Registrar(L)
    {
        lua_getglobal(L, "_G");

        ++m_stackSize;
    }

    //=============================================================================================
    /**
     * @brief Open the a namespace for registrations from a table on top of the stack.
     *
     * @param L A Lua state.
     */
    Namespace(lua_State* L, FromStack)
        : Registrar(L, 1)
    {
        assert(lua_istable(L, -1));

        {
            lua_pushvalue(L, -1); // Stack: ns, mt

            // ns.__metatable = ns
            lua_setmetatable(L, -2); // Stack: ns, mt

            // ns.__index = index_metamethod
            lua_pushcfunction_x(L, &detail::index_metamethod);
            rawsetfield(L, -2, "__index"); // Stack: ns

            lua_newtable(L); // Stack: ns, mt, propget table (pg)
            lua_rawsetp(L, -2, detail::getPropgetKey()); // ns [propgetKey] = pg. Stack: ns

            lua_newtable(L); // Stack: ns, mt, propset table (ps)
            lua_rawsetp(L, -2, detail::getPropsetKey()); // ns [propsetKey] = ps. Stack: ns
        }

        ++m_stackSize;
    }

    //=============================================================================================
    /**
     * @brief Open a namespace for registrations.
     *
     * The namespace is created if it doesn't already exist.
     *
     * @param name The namespace name.
     * @param parent The parent namespace object.
     *
     * @pre The parent namespace is at the top of the Lua stack.
     */
    Namespace(const char* name, Namespace& parent)
        : Registrar(parent)
    {
        assert(name != nullptr);
        assert(lua_istable(L, -1)); // Stack: parent namespace (pns)

        rawgetfield(L, -1, name); // Stack: pns, namespace (ns) | nil

        if (lua_isnil(L, -1)) // Stack: pns, nil
        {
            lua_pop(L, 1); // Stack: pns

            lua_newtable(L); // Stack: pns, ns
            lua_pushvalue(L, -1); // Stack: pns, ns, mt

            // ns.__metatable = ns
            lua_setmetatable(L, -2); // Stack: pns, ns

            // ns.__index = index_metamethod
            lua_pushcfunction_x(L, &detail::index_metamethod);
            rawsetfield(L, -2, "__index"); // Stack: pns, ns

            // ns.__newindex = newindex_static_metamethod
            lua_pushcfunction_x(L, &detail::newindex_static_metamethod);
            rawsetfield(L, -2, "__newindex"); // Stack: pns, ns

            lua_newtable(L); // Stack: pns, ns, propget table (pg)
            lua_rawsetp(L, -2, detail::getPropgetKey()); // ns [propgetKey] = pg. Stack: pns, ns

            lua_newtable(L); // Stack: pns, ns, propset table (ps)
            lua_rawsetp(L, -2, detail::getPropsetKey()); // ns [propsetKey] = ps. Stack: pns, ns

            // pns [name] = ns
            lua_pushvalue(L, -1); // Stack: pns, ns, ns
            rawsetfield(L, -3, name); // Stack: pns, ns
        }

        ++m_stackSize;
    }

    //=============================================================================================
    /**
     * @brief Close the class and continue the namespace registrations.
     *
     * @param child A child class registration object.
     */
    explicit Namespace(ClassBase& child)
        : Registrar(child)
    {
    }

    explicit Namespace(Table& child)
        : Registrar(child)
    {
    }

    using Registrar::operator=;

public:
    //=============================================================================================
    /**
     * @brief Retrieve the global namespace.
     *
     * It is recommended to put your namespace inside the global namespace, and then add your classes and functions to it, rather than
     * adding many classes and functions directly to the global namespace.
     *
     * @param L A Lua state.
     *
     * @returns A namespace registration object.
     */
    static Namespace getGlobalNamespace(lua_State* L)
    {
        return Namespace(L);
    }

    /**
     * @brief Retrieve the namespace on top of the stack.
     *
     * You should have a table on top of the stack before calling this function. It will then use the table there as destination for registrations.
     *
     * @param L A Lua state.
     *
     * @returns A namespace registration object.
     */
    static Namespace getNamespaceFromStack(lua_State* L)
    {
        return Namespace(L, FromStack{});
    }

    //=============================================================================================
    /**
     * @brief Open a new or existing namespace for registrations.
     *
     * @param name The namespace name.
     *
     * @returns A namespace registration object.
     */
    Namespace beginNamespace(const char* name)
    {
        assertIsActive();
        return Namespace(name, *this);
    }

    //=============================================================================================
    /**
     * @brief Continue namespace registration in the parent.
     *
     * Do not use this on the global namespace.
     *
     * @returns A parent namespace registration object.
     */
    Namespace endNamespace()
    {
        if (m_stackSize == 1)
        {
            throw_or_assert<std::logic_error>("endNamespace() called on global namespace");

            return Namespace(*this);
        }

        assert(m_stackSize > 1);
        --m_stackSize;
        lua_pop(L, 1);
        return Namespace(*this);
    }

    //=============================================================================================
    /**
     * @brief Add or replace a variable.
     *
     * @param name The property name.
     * @param value A value pointer.
     *
     * @returns This namespace registration object.
     */
    template <class T>
    Namespace& addVariable(const char* name, const T& value)
    {
        if (m_stackSize == 1)
        {
            throw_or_assert<std::logic_error>("addVariable() called on global namespace");

            return *this;
        }

        assert(name != nullptr);
        assert(lua_istable(L, -1)); // Stack: namespace table (ns)

        std::error_code ec;
        if constexpr (std::is_enum_v<T>)
        {
            using U = std::underlying_type_t<T>;
            
            if (! Stack<U>::push(L, static_cast<U>(value), ec))
                luaL_error(L, "%s", ec.message().c_str());
        }
        else
        {
            if (! Stack<T>::push(L, value, ec))
                luaL_error(L, "%s", ec.message().c_str());
        }

        rawsetfield(L, -2, name); // Stack: ns

        return *this;
    }

    //=============================================================================================
    /**
     * @brief Add or replace a property.
     *
     * @param name The property name.
     * @param value A value pointer.
     * @param isWritable True for a read-write, false for read-only property.
     *
     * @returns This namespace registration object.
     */
    template <class T>
    Namespace& addProperty(const char* name, T* value, bool isWritable = true)
    {
        if (m_stackSize == 1)
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        assert(name != nullptr);
        assert(lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushlightuserdata(L, value); // Stack: ns, pointer
        lua_pushcclosure_x(L, &detail::property_getter<T>::call, 1); // Stack: ns, getter
        detail::add_property_getter(L, name, -2); // Stack: ns

        if (isWritable)
        {
            lua_pushlightuserdata(L, value); // Stack: ns, pointer
            lua_pushcclosure_x(L, &detail::property_setter<T>::call, 1); // Stack: ns, setter
        }
        else
        {
            lua_pushstring(L, name); // Stack: ns, ps, name
            lua_pushcclosure_x(L, &detail::read_only_error, 1); // Stack: ns, function
        }

        detail::add_property_setter(L, name, -2); // Stack: ns

        return *this;
    }

    //=============================================================================================
    /**
     * @brief Add or replace a property.
     *
     * If the set function is omitted or null, the property is read-only.
     *
     * @param name The property name.
     * @param get  A pointer to a property getter function.
     * @param set  A pointer to a property setter function, optional.
     *
     * @returns This namespace registration object.
     */
    template <class TG, class TS = TG>
    Namespace& addProperty(const char* name, TG (*get)(), void (*set)(TS) = nullptr)
    {
        if (m_stackSize == 1)
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        assert(name != nullptr);
        assert(lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushlightuserdata(L, reinterpret_cast<void*>(get)); // Stack: ns, function ptr
        lua_pushcclosure_x(L, &detail::invoke_proxy_function<TG (*)()>, 1); // Stack: ns, getter
        detail::add_property_getter(L, name, -2);

        if (set != nullptr)
        {
            lua_pushlightuserdata(L, reinterpret_cast<void*>(set)); // Stack: ns, function ptr
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<void (*)(TS)>, 1);
        }
        else
        {
            lua_pushstring(L, name);
            lua_pushcclosure_x(L, &detail::read_only_error, 1);
        }

        detail::add_property_setter(L, name, -2);

        return *this;
    }

    //=============================================================================================
    /**
     * @brief Add or replace a readonly property.
     *
     * @param name The property name.
     * @param get  A pointer to a property getter function.
     *
     * @returns This namespace registration object.
     */
    template <class Getter>
    Namespace& addProperty(const char* name, Getter get)
    {
        assert(name != nullptr);
        assert(lua_istable(L, -1)); // Stack: namespace table (ns)

        if constexpr (std::is_enum_v<Getter>)
        {
            using U = std::underlying_type_t<Getter>;

            auto enumGet = [get = std::move(get)] { return static_cast<U>(get); };

            using GetType = decltype(enumGet);
            lua_newuserdata_aligned<GetType>(L, std::move(enumGet)); // Stack: ns, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<GetType>, 1); // Stack: ns, ud, getter
        }
        else
        {
            using GetType = decltype(get);
            lua_newuserdata_aligned<GetType>(L, std::move(get)); // Stack: ns, function userdata (ud)
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<GetType>, 1); // Stack: ns, ud, getter
        }

        detail::add_property_getter(L, name, -2); // Stack: ns, ud, getter

        lua_pushstring(L, name); // Stack: ns, name
        lua_pushcclosure_x(L, &detail::read_only_error, 1); // Stack: ns, name, function
        detail::add_property_setter(L, name, -2); // Stack: ns

        return *this;
    }

    /**
     * @brief Add or replace a mutable property.
     *
     * @param name The property name.
     * @param get  A pointer to a property getter function.
     * @param set  A pointer to a property setter function.
     *
     * @returns This namespace registration object.
     */
    template <class Getter, class Setter>
    Namespace& addProperty(const char* name, Getter get, Setter set)
    {
        assert(name != nullptr);
        assert(lua_istable(L, -1)); // Stack: namespace table (ns)

        addProperty<Getter>(name, std::move(get));

        using SetType = decltype(set);

        lua_newuserdata_aligned<SetType>(L, std::move(set)); // Stack: ns, function userdata (ud)
        lua_pushcclosure_x(L, &detail::invoke_proxy_functor<SetType>, 1); // Stack: ns, ud, getter
        detail::add_property_setter(L, name, -2); // Stack: ns, ud, getter

        return *this;
    }

    //=============================================================================================
    /**
     * @brief Add or replace a property.
     *
     * If the set function is omitted or null, the property is read-only.
     *
     * @param name The property name.
     * @param get  A pointer to a property getter function.
     * @param set  A pointer to a property setter function, optional.
     *
     * @returns This namespace registration object.
     */
    Namespace& addProperty(const char* name, lua_CFunction get, lua_CFunction set = nullptr)
    {
        assert(name != nullptr);
        assert(lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushcfunction_x(L, get); // Stack: ns, getter
        detail::add_property_getter(L, name, -2); // Stack: ns

        if (set != nullptr)
        {
            lua_pushcfunction_x(L, set); // Stack: ns, setter
            detail::add_property_setter(L, name, -2); // Stack: ns
        }
        else
        {
            lua_pushstring(L, name); // Stack: ns, name
            lua_pushcclosure_x(L, &detail::read_only_error, 1); // Stack: ns, name, function
            detail::add_property_setter(L, name, -2); // Stack: ns
        }

        return *this;
    }

    //=============================================================================================
    /**
     * @brief Add or replace a namespace function by convertible to std::function.
     */
    template <class Function>
    Namespace& addFunction(const char* name, Function function)
    {
        using FnType = decltype(function);

        assert(name != nullptr);
        assert(lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_newuserdata_aligned<FnType>(L, std::move(function)); // Stack: ns, function userdata (ud)
        lua_pushcclosure_x(L, &detail::invoke_proxy_functor<FnType>, 1); // Stack: ns, function
        rawsetfield(L, -2, name); // Stack: ns

        return *this;
    }

    //=============================================================================================
    /**
     * @brief Add or replace a free function.
     */
    template <class ReturnType, class... Params>
    Namespace& addFunction(const char* name, ReturnType (*fp)(Params...))
    {
        using FnType = decltype(fp);

        assert(name != nullptr);
        assert(lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushlightuserdata(L, reinterpret_cast<void*>(fp)); // Stack: ns, function ptr
        lua_pushcclosure_x(L, &detail::invoke_proxy_function<FnType>, 1); // Stack: ns, function
        rawsetfield(L, -2, name); // Stack: ns

        return *this;
    }

    //=============================================================================================
    /**
     * @brief Add or replace a lua_CFunction.
     *
     * @param name The function name.
     * @param fp   A C-function pointer.
     *
     * @returns This namespace registration object.
     */
    Namespace& addFunction(const char* name, lua_CFunction fp)
    {
        assert(name != nullptr);
        assert(lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushcfunction_x(L, fp); // Stack: ns, function
        rawsetfield(L, -2, name); // Stack: ns

        return *this;
    }

    //=============================================================================================
    Table beginTable(const char* name)
    {
        assertIsActive();
        return Table(name, *this);
    }

    //=============================================================================================
    /**
     * @brief Open a new or existing class for registrations.
     *
     * @param name The class name.
     *
     * @returns A class registration object.
     */
    template <class T>
    Class<T> beginClass(const char* name)
    {
        assertIsActive();
        return Class<T>(name, *this);
    }

    //=============================================================================================
    /**
     * @brief Derive a new class for registrations.
     *
     * Call deriveClass() only once. To continue registrations for the class later, use beginClass().
     *
     * @param name The class name.
     *
     * @returns A class registration object.
     */
    template <class Derived, class Base>
    Class<Derived> deriveClass(const char* name)
    {
        assertIsActive();
        return Class<Derived>(name, *this, detail::getStaticRegistryKey<Base>());
    }
};

//=================================================================================================
/**
 * @brief Retrieve the global namespace.
 *
 * It is recommended to put your namespace inside the global namespace, and then add your classes and functions to it, rather than adding
 * many classes and functions directly to the global namespace.
 *
 * @param L A Lua state.
 *
 * @returns A namespace registration object.
 */
inline Namespace getGlobalNamespace(lua_State* L)
{
    return Namespace::getGlobalNamespace(L);
}

//=================================================================================================
/**
 * @brief Retrieve the namespace on top of the stack.
 *
 * You should have a table on top of the stack before calling this function. It will then use the table there as destination for registrations.
 *
 * @param L A Lua state.
 *
 * @returns A namespace registration object.
 */
inline Namespace getNamespaceFromStack(lua_State* L)
{
    return Namespace::getNamespaceFromStack(L);
}

//=================================================================================================
/**
 * @brief Registers main thread.
 *
 * This is a backward compatibility mitigation for lua 5.1 not supporting LUA_RIDX_MAINTHREAD.
 *
 * @param L The main Lua state that will be regstered as main thread.
 *
 * @returns A namespace registration object.
 */
inline void registerMainThread(lua_State* L)
{
    register_main_thread(L);
}

} // namespace luabridge

// End File: Source/LuaBridge/detail/Namespace.h

// Begin File: Source/LuaBridge/LuaBridge.h


// All #include dependencies are listed here
// instead of in the individual header files.

#define LUABRIDGE_MAJOR_VERSION 3
#define LUABRIDGE_MINOR_VERSION 1
#define LUABRIDGE_VERSION 301


// End File: Source/LuaBridge/LuaBridge.h

// Begin File: Source/LuaBridge/Map.h


namespace luabridge {

//=================================================================================================
/**
 * @brief Stack specialization for `std::map`.
 */
template <class K, class V>
struct Stack<std::map<K, V>>
{
    using Type = std::map<K, V>;

    [[nodiscard]] static bool push(lua_State* L, const Type& map, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        const int initialStackSize = lua_gettop(L);

        lua_createtable(L, 0, static_cast<int>(map.size()));

        for (auto it = map.begin(); it != map.end(); ++it)
        {
            std::error_code errorCodeKey;
            if (! Stack<K>::push(L, it->first, errorCodeKey))
            {
                ec = errorCodeKey;
                lua_pop(L, lua_gettop(L) - initialStackSize);
                return false;
            }

            std::error_code errorCodeValue;
            if (! Stack<V>::push(L, it->second, errorCodeValue))
            {
                ec = errorCodeValue;
                lua_pop(L, lua_gettop(L) - initialStackSize);
                return false;
            }

            lua_settable(L, -3);
        }
        
        return true;
    }

    [[nodiscard]] static Type get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            luaL_error(L, "#%d argument must be a table", index);

        Type map;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        while (lua_next(L, absIndex) != 0)
        {
            map.emplace(Stack<K>::get(L, -2), Stack<V>::get(L, -1));
            lua_pop(L, 1);
        }

        return map;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index);
    }
};

} // namespace luabridge

// End File: Source/LuaBridge/Map.h

// Begin File: Source/LuaBridge/RefCountedObject.h


//==============================================================================
/*
  This is a derivative work used by permission from part of
  JUCE, available at http://www.rawaterialsoftware.com

  License: The MIT License (http://www.opensource.org/licenses/mit-license.php)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  This file incorporates work covered by the following copyright and
  permission notice:

    This file is part of the JUCE library - "Jules' Utility Class Extensions"
    Copyright 2004-11 by Raw Material Software Ltd.
*/
//==============================================================================

namespace luabridge {

//==============================================================================
/**
  Adds reference-counting to an object.

  To add reference-counting to a class, derive it from this class, and
  use the RefCountedObjectPtr class to point to it.

  e.g. @code
  class MyClass : public RefCountedObjectType
  {
      void foo();

      // This is a neat way of declaring a typedef for a pointer class,
      // rather than typing out the full templated name each time..
      typedef RefCountedObjectPtr<MyClass> Ptr;
  };

  MyClass::Ptr p = new MyClass();
  MyClass::Ptr p2 = p;
  p = 0;
  p2->foo();
  @endcode

  Once a new RefCountedObjectType has been assigned to a pointer, be
  careful not to delete the object manually.
*/
template<class CounterType>
class RefCountedObjectType
{
public:
    //==============================================================================
    /** Increments the object's reference count.

        This is done automatically by the smart pointer, but is public just
        in case it's needed for nefarious purposes.
    */
    inline void incReferenceCount() const { ++refCount; }

    /** Decreases the object's reference count.

        If the count gets to zero, the object will be deleted.
    */
    inline void decReferenceCount() const
    {
        assert(getReferenceCount() > 0);

        if (--refCount == 0)
            delete this;
    }

    /** Returns the object's current reference count.
     * @returns The reference count.
     */
    inline int getReferenceCount() const { return static_cast<int>(refCount); }

protected:
    //==============================================================================
    /** Creates the reference-counted object (with an initial ref count of zero). */
    RefCountedObjectType() : refCount() {}

    /** Destructor. */
    virtual ~RefCountedObjectType()
    {
        // it's dangerous to delete an object that's still referenced by something else!
        assert(getReferenceCount() == 0);
    }

private:
    //==============================================================================
    CounterType mutable refCount;
};

//==============================================================================

/** Non thread-safe reference counted object.

    This creates a RefCountedObjectType that uses a non-atomic integer
    as the counter.
*/
typedef RefCountedObjectType<int> RefCountedObject;

//==============================================================================
/**
    A smart-pointer class which points to a reference-counted object.

    The template parameter specifies the class of the object you want to point
    to - the easiest way to make a class reference-countable is to simply make
    it inherit from RefCountedObjectType, but if you need to, you could roll
    your own reference-countable class by implementing a pair of methods called
    incReferenceCount() and decReferenceCount().

    When using this class, you'll probably want to create a typedef to
    abbreviate the full templated name - e.g.

    @code

    typedef RefCountedObjectPtr <MyClass> MyClassPtr;

    @endcode
*/
template<class ReferenceCountedObjectClass>
class RefCountedObjectPtr
{
public:
    /** The class being referenced by this pointer. */
    typedef ReferenceCountedObjectClass ReferencedType;

    //==============================================================================
    /** Creates a pointer to a null object. */
    inline RefCountedObjectPtr() : referencedObject(0) {}

    /** Creates a pointer to an object.
        This will increment the object's reference-count if it is non-null.

        @param refCountedObject A reference counted object to own.
    */
    inline RefCountedObjectPtr(ReferenceCountedObjectClass* const refCountedObject)
        : referencedObject(refCountedObject)
    {
        if (refCountedObject != 0)
            refCountedObject->incReferenceCount();
    }

    /** Copies another pointer.
        This will increment the object's reference-count (if it is non-null).

        @param other Another pointer.
    */
    inline RefCountedObjectPtr(const RefCountedObjectPtr& other)
        : referencedObject(other.referencedObject)
    {
        if (referencedObject != 0)
            referencedObject->incReferenceCount();
    }

    /**
      Takes-over the object from another pointer.

      @param other Another pointer.
    */
    inline RefCountedObjectPtr(RefCountedObjectPtr&& other)
        : referencedObject(other.referencedObject)
    {
        other.referencedObject = 0;
    }

    /** Copies another pointer.
        This will increment the object's reference-count (if it is non-null).

        @param other Another pointer.
    */
    template<class DerivedClass>
    inline RefCountedObjectPtr(const RefCountedObjectPtr<DerivedClass>& other)
        : referencedObject(static_cast<ReferenceCountedObjectClass*>(other.getObject()))
    {
        if (referencedObject != 0)
            referencedObject->incReferenceCount();
    }

    /** Changes this pointer to point at a different object.

        The reference count of the old object is decremented, and it might be
        deleted if it hits zero. The new object's count is incremented.

        @param other A pointer to assign from.
        @returns This pointer.
    */
    RefCountedObjectPtr& operator=(const RefCountedObjectPtr& other)
    {
        return operator=(other.referencedObject);
    }

    /** Changes this pointer to point at a different object.
        The reference count of the old object is decremented, and it might be
        deleted if it hits zero. The new object's count is incremented.

        @param other A pointer to assign from.
        @returns This pointer.
    */
    template<class DerivedClass>
    RefCountedObjectPtr& operator=(const RefCountedObjectPtr<DerivedClass>& other)
    {
        return operator=(static_cast<ReferenceCountedObjectClass*>(other.getObject()));
    }

    /**
      Takes-over the object from another pointer.

      @param other A pointer to assign from.
      @returns This pointer.
     */
    RefCountedObjectPtr& operator=(RefCountedObjectPtr&& other)
    {
        using std::swap;

        swap(referencedObject, other.referencedObject);

        return *this;
    }

    /** Changes this pointer to point at a different object.
        The reference count of the old object is decremented, and it might be
        deleted if it hits zero. The new object's count is incremented.

        @param newObject A reference counted object to own.
        @returns This pointer.
    */
    RefCountedObjectPtr& operator=(ReferenceCountedObjectClass* const newObject)
    {
        if (referencedObject != newObject)
        {
            if (newObject != 0)
                newObject->incReferenceCount();

            ReferenceCountedObjectClass* const oldObject = referencedObject;
            referencedObject = newObject;

            if (oldObject != 0)
                oldObject->decReferenceCount();
        }

        return *this;
    }

    /** Destructor.
        This will decrement the object's reference-count, and may delete it if it
        gets to zero.
    */
    ~RefCountedObjectPtr()
    {
        if (referencedObject != 0)
            referencedObject->decReferenceCount();
    }

    /** Returns the object that this pointer references.
        The returned pointer may be null.

        @returns The pointee.
    */
    operator ReferenceCountedObjectClass*() const { return referencedObject; }

    /** Returns the object that this pointer references.
        The returned pointer may be null.

        @returns The pointee.
    */
    ReferenceCountedObjectClass* operator->() const { return referencedObject; }

    /** Returns the object that this pointer references.
        The returned pointer may be null.

        @returns The pointee.
    */
    ReferenceCountedObjectClass* getObject() const { return referencedObject; }

private:
    //==============================================================================
    ReferenceCountedObjectClass* referencedObject;
};

/** Compares two ReferenceCountedObjectPointers. */
template<class ReferenceCountedObjectClass>
bool operator==(const RefCountedObjectPtr<ReferenceCountedObjectClass>& object1,
                ReferenceCountedObjectClass* const object2)
{
    return object1.getObject() == object2;
}

/** Compares two ReferenceCountedObjectPointers. */
template<class ReferenceCountedObjectClass>
bool operator==(const RefCountedObjectPtr<ReferenceCountedObjectClass>& object1,
                const RefCountedObjectPtr<ReferenceCountedObjectClass>& object2)
{
    return object1.getObject() == object2.getObject();
}

/** Compares two ReferenceCountedObjectPointers. */
template<class ReferenceCountedObjectClass>
bool operator==(ReferenceCountedObjectClass* object1,
                RefCountedObjectPtr<ReferenceCountedObjectClass>& object2)
{
    return object1 == object2.getObject();
}

/** Compares two ReferenceCountedObjectPointers. */
template<class ReferenceCountedObjectClass>
bool operator!=(const RefCountedObjectPtr<ReferenceCountedObjectClass>& object1,
                const ReferenceCountedObjectClass* object2)
{
    return object1.getObject() != object2;
}

/** Compares two ReferenceCountedObjectPointers. */
template<class ReferenceCountedObjectClass>
bool operator!=(const RefCountedObjectPtr<ReferenceCountedObjectClass>& object1,
                RefCountedObjectPtr<ReferenceCountedObjectClass>& object2)
{
    return object1.getObject() != object2.getObject();
}

/** Compares two ReferenceCountedObjectPointers. */
template<class ReferenceCountedObjectClass>
bool operator!=(ReferenceCountedObjectClass* object1,
                RefCountedObjectPtr<ReferenceCountedObjectClass>& object2)
{
    return object1 != object2.getObject();
}

//==============================================================================

template<class T>
struct ContainerTraits<RefCountedObjectPtr<T>>
{
    using Type = T;

    static RefCountedObjectPtr<T> construct(T* c) { return c; }

    static T* get(RefCountedObjectPtr<T> const& c) { return c.getObject(); }
};

//==============================================================================

} // namespace luabridge

// End File: Source/LuaBridge/RefCountedObject.h

// Begin File: Source/LuaBridge/Set.h


namespace luabridge {

//=================================================================================================
/**
 * @brief Stack specialization for `std::set`.
 */
template <class K, class V>
struct Stack<std::set<K, V>>
{
    using Type = std::set<K, V>;
    
    [[nodiscard]] static bool push(lua_State* L, const Type& set, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        const int initialStackSize = lua_gettop(L);
        
        lua_createtable(L, 0, static_cast<int>(set.size()));

        for (auto it = set.begin(); it != set.end(); ++it)
        {
            std::error_code errorCodeKey;
            if (! Stack<K>::push(L, it->first, errorCodeKey))
            {
                ec = errorCodeKey;
                lua_pop(L, lua_gettop(L) - initialStackSize);
                return false;
            }

            std::error_code errorCodeValue;
            if (! Stack<V>::push(L, it->second, errorCodeValue))
            {
                ec = errorCodeValue;
                lua_pop(L, lua_gettop(L) - initialStackSize);
                return false;
            }

            lua_settable(L, -3);
        }
        
        return true;
    }

    [[nodiscard]] static Type get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            luaL_error(L, "#%d argument must be a table", index);

        Type set;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        while (lua_next(L, absIndex) != 0)
        {
            set.emplace(Stack<K>::get(L, -2), Stack<V>::get(L, -1));
            lua_pop(L, 1);
        }

        return set;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index);
    }
};

} // namespace luabridge

// End File: Source/LuaBridge/Set.h

// Begin File: Source/LuaBridge/UnorderedMap.h


namespace luabridge {

//=================================================================================================
/**
 * @brief Stack specialization for `std::unordered_map`.
 */
template <class K, class V>
struct Stack<std::unordered_map<K, V>>
{
    using Type = std::unordered_map<K, V>;

    [[nodiscard]] static bool push(lua_State* L, const Type& map, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        const int initialStackSize = lua_gettop(L);
        
        lua_createtable(L, 0, static_cast<int>(map.size()));

        for (auto it = map.begin(); it != map.end(); ++it)
        {
            std::error_code errorCodeKey;
            if (! Stack<K>::push(L, it->first, errorCodeKey))
            {
                ec = errorCodeKey;
                lua_pop(L, lua_gettop(L) - initialStackSize);
                return false;
            }
            
            std::error_code errorCodeValue;
            if (! Stack<V>::push(L, it->second, errorCodeValue))
            {
                ec = errorCodeValue;
                lua_pop(L, lua_gettop(L) - initialStackSize);
                return false;
            }

            lua_settable(L, -3);
        }
        
        return true;
    }

    [[nodiscard]] static Type get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            luaL_error(L, "#%d argument must be a table", index);

        Type map;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        while (lua_next(L, absIndex) != 0)
        {
            map.emplace(Stack<K>::get(L, -2), Stack<V>::get(L, -1));
            lua_pop(L, 1);
        }

        return map;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index);
    }
};

} // namespace luabridge

// End File: Source/LuaBridge/UnorderedMap.h

// Begin File: Source/LuaBridge/Vector.h


namespace luabridge {

//=================================================================================================
/**
 * @brief Stack specialization for `std::vector`.
 */
template <class T>
struct Stack<std::vector<T>>
{
    using Type = std::vector<T>;

    [[nodiscard]] static bool push(lua_State* L, const Type& vector, std::error_code& ec)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
        {
            ec = makeErrorCode(ErrorCode::LuaStackOverflow);
            return false;
        }
#endif

        const int initialStackSize = lua_gettop(L);
        
        lua_createtable(L, static_cast<int>(vector.size()), 0);

        for (std::size_t i = 0; i < vector.size(); ++i)
        {
            lua_pushinteger(L, static_cast<lua_Integer>(i + 1));
            
            std::error_code errorCode;
            if (! Stack<T>::push(L, vector[i], errorCode))
            {
                ec = errorCode;
                lua_pop(L, lua_gettop(L) - initialStackSize);
                return false;
            }
            
            lua_settable(L, -3);
        }
        
        return true;
    }

    [[nodiscard]] static Type get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            luaL_error(L, "#%d argument must be a table", index);

        Type vector;
        vector.reserve(static_cast<std::size_t>(get_length(L, index)));

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        while (lua_next(L, absIndex) != 0)
        {
            vector.emplace_back(Stack<T>::get(L, -1));
            lua_pop(L, 1);
        }

        return vector;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index);
    }
};

} // namespace luabridge

// End File: Source/LuaBridge/Vector.h

// Begin File: Source/LuaBridge/detail/Dump.h


namespace luabridge {
namespace debug {

inline void putIndent(std::ostream& stream, unsigned level)
{
    for (unsigned i = 0; i < level; ++i)
    {
        stream << "  ";
    }
}

inline void dumpTable(lua_State* L, int index, std::ostream& stream, unsigned level);

inline void dumpValue(lua_State* L, int index, std::ostream& stream, unsigned level = 0)
{
    const int type = lua_type(L, index);
    switch (type)
    {
    case LUA_TNIL:
        stream << "nil";
        break;

    case LUA_TBOOLEAN:
        stream << (lua_toboolean(L, index) ? "true" : "false");
        break;

    case LUA_TNUMBER:
        stream << lua_tonumber(L, index);
        break;

    case LUA_TSTRING:
        stream << '"' << lua_tostring(L, index) << '"';
        break;

    case LUA_TFUNCTION:
        if (lua_iscfunction(L, index))
        {
            stream << "cfunction@" << lua_topointer(L, index);
        }
        else
        {
            stream << "function@" << lua_topointer(L, index);
        }
        break;

    case LUA_TTHREAD:
        stream << "thread@" << lua_tothread(L, index);
        break;

    case LUA_TLIGHTUSERDATA:
        stream << "lightuserdata@" << lua_touserdata(L, index);
        break;

    case LUA_TTABLE:
        dumpTable(L, index, stream, level);
        break;

    case LUA_TUSERDATA:
        stream << "userdata@" << lua_touserdata(L, index);
        break;

    default:
        stream << lua_typename(L, type);
        ;
        break;
    }
}

inline void dumpTable(lua_State* L, int index, std::ostream& stream, unsigned level = 0)
{
    stream << "table@" << lua_topointer(L, index);

    if (level > 0)
    {
        return;
    }

    index = lua_absindex(L, index);
    stream << " {";
    lua_pushnil(L); // Initial key
    while (lua_next(L, index))
    {
        stream << "\n";
        putIndent(stream, level + 1);
        dumpValue(L, -2, stream, level + 1); // Key
        stream << ": ";
        dumpValue(L, -1, stream, level + 1); // Value
        lua_pop(L, 1); // Value
    }
    putIndent(stream, level);
    stream << "\n}";
}

inline void dumpState(lua_State* L, std::ostream& stream = std::cerr)
{
    int top = lua_gettop(L);
    for (int i = 1; i <= top; ++i)
    {
        stream << "stack #" << i << ": ";
        dumpValue(L, i, stream, 0);
        stream << "\n";
    }
}

} // namespace debug
} // namespace luabridge

// End File: Source/LuaBridge/detail/Dump.h

// clang-format on

