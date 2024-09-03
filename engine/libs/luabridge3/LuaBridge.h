// https://github.com/kunitoki/LuaBridge3
// Copyright 2024, Lucio Asnaghi
// SPDX-License-Identifier: MIT

// clang-format off

#pragma once

#include <algorithm>
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

#if !defined(LUABRIDGE_HAS_EXCEPTIONS)
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
#endif

#if LUABRIDGE_HAS_EXCEPTIONS
#define LUABRIDGE_IF_EXCEPTIONS(...) __VA_ARGS__
#define LUABRIDGE_IF_NO_EXCEPTIONS(...)
#else
#define LUABRIDGE_IF_EXCEPTIONS(...)
#define LUABRIDGE_IF_NO_EXCEPTIONS(...) __VA_ARGS__
#endif

#if defined(LUAU_FASTMATH_BEGIN)
#define LUABRIDGE_ON_LUAU 1
#elif defined(LUAJIT_VERSION)
#define LUABRIDGE_ON_LUAJIT 1
#elif defined(RAVI_OPTION_STRING2)
#define LUABRIDGE_ON_RAVI 1
#elif defined(LUA_VERSION_NUM)
#define LUABRIDGE_ON_LUA 1
#else
#error "Lua headers must be included prior to LuaBridge ones"
#endif

#if defined(__OBJC__)
#define LUABRIDGE_ON_OBJECTIVE_C 1
#endif

#if !defined(LUABRIDGE_SAFE_STACK_CHECKS)
#define LUABRIDGE_SAFE_STACK_CHECKS 1
#endif

#if !defined(LUABRIDGE_SAFE_LUA_C_EXCEPTION_HANDLING)
#define LUABRIDGE_SAFE_LUA_C_EXCEPTION_HANDLING 0
#endif

#if !defined(LUABRIDGE_RAISE_UNREGISTERED_CLASS_USAGE)
#if LUABRIDGE_HAS_EXCEPTIONS
#define LUABRIDGE_RAISE_UNREGISTERED_CLASS_USAGE 1
#else
#define LUABRIDGE_RAISE_UNREGISTERED_CLASS_USAGE 0
#endif
#endif

#if !defined(LUABRIDGE_ASSERT)
#if defined(NDEBUG) && !defined(LUABRIDGE_FORCE_ASSERT_RELEASE)
#define LUABRIDGE_ASSERT(expr) ((void)(expr))
#else
#define LUABRIDGE_ASSERT(expr) assert(expr)
#endif
#endif


// End File: Source/LuaBridge/detail/Config.h

// Begin File: Source/LuaBridge/detail/LuaHelpers.h

namespace luabridge {

template <class... Args>
constexpr void unused(Args&&...)
{
}

#if LUABRIDGE_ON_LUAU
inline int luaL_ref(lua_State* L, int idx)
{
    LUABRIDGE_ASSERT(idx == LUA_REGISTRYINDEX);

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

inline void lua_pushcfunction_x(lua_State *L, lua_CFunction fn, const char* debugname)
{
    lua_pushcfunction(L, fn, debugname);
}

inline void lua_pushcclosure_x(lua_State* L, lua_CFunction fn, const char* debugname, int n)
{
    lua_pushcclosure(L, fn, debugname, n);
}

inline int lua_error_x(lua_State* L)
{
    lua_error(L);
    return 0;
}

inline int lua_getstack_x(lua_State* L, int level, lua_Debug* ar)
{
    return lua_getinfo(L, level, "nlS", ar);
}

inline int lua_getstack_info_x(lua_State* L, int level, const char* what, lua_Debug* ar)
{
    return lua_getinfo(L, level, what, ar);
}

#else
using ::luaL_ref;
using ::luaL_unref;

template <class T>
inline void* lua_newuserdata_x(lua_State* L, size_t sz)
{
    return lua_newuserdata(L, sz);
}

inline void lua_pushcfunction_x(lua_State *L, lua_CFunction fn, const char* debugname)
{
    unused(debugname);

    lua_pushcfunction(L, fn);
}

inline void lua_pushcclosure_x(lua_State* L, lua_CFunction fn, const char* debugname, int n)
{
    unused(debugname);

    lua_pushcclosure(L, fn, n);
}

inline int lua_error_x(lua_State* L)
{
    return lua_error(L);
}

inline int lua_getstack_x(lua_State* L, int level, lua_Debug* ar)
{
    return lua_getstack(L, level, ar);
}

inline int lua_getstack_info_x(lua_State* L, int level, const char* what, lua_Debug* ar)
{
    lua_getstack(L, level, ar);
    return lua_getinfo(L, what, ar);
}

#endif 

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

#endif 

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

inline int lua_rawgetp(lua_State* L, int idx, const void* p)
{
    idx = lua_absindex(L, idx);
    luaL_checkstack(L, 1, "not enough stack slots");
    lua_pushlightuserdata(L, const_cast<void*>(p));
    lua_rawget(L, idx);
    return lua_type(L, -1);
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

#if ! LUABRIDGE_ON_LUAJIT
inline void* luaL_testudata(lua_State* L, int ud, const char* tname)
{
    void* p = lua_touserdata(L, ud);
    if (p == nullptr)
        return nullptr;

    if (! lua_getmetatable(L, ud))
        return nullptr;

    luaL_getmetatable(L, tname);
    if (! lua_rawequal(L, -1, -2))
        p = nullptr;

    lua_pop(L, 2);
    return p;
}
#endif

inline int get_length(lua_State* L, int idx)
{
    return static_cast<int>(lua_objlen(L, idx));
}

#else 
inline int get_length(lua_State* L, int idx)
{
    lua_len(L, idx);
    const int len = static_cast<int>(luaL_checknumber(L, -1));
    lua_pop(L, 1);
    return len;
}

#endif 

#ifndef LUA_OK
#define LUABRIDGE_LUA_OK 0
#else
#define LUABRIDGE_LUA_OK LUA_OK
#endif

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

template <class T, class... Args>
void throw_or_assert(Args&&... args)
{
#if LUABRIDGE_HAS_EXCEPTIONS
    throw T(std::forward<Args>(args)...);
#else
    unused(std::forward<Args>(args)...);
    LUABRIDGE_ASSERT(false);
#endif
}

template <class T>
void pushunsigned(lua_State* L, T value)
{
    static_assert(std::is_unsigned_v<T>);

    lua_pushinteger(L, static_cast<lua_Integer>(value));
}

inline lua_Number tonumber(lua_State* L, int idx, int* isnum)
{
#if ! LUABRIDGE_ON_LUAU && LUA_VERSION_NUM > 502
    return lua_tonumberx(L, idx, isnum);
#else
    return to_numberx(L, idx, isnum);
#endif
}

inline lua_Integer tointeger(lua_State* L, int idx, int* isnum)
{
#if ! LUABRIDGE_ON_LUAU && LUA_VERSION_NUM > 502
    return lua_tointegerx(L, idx, isnum);
#else
    return to_integerx(L, idx, isnum);
#endif
}

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
    LUABRIDGE_ASSERT(false); 
    lua_pop(threadL, 1);
    return threadL;
#else
    lua_rawgeti(threadL, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
    lua_State* L = lua_tothread(threadL, -1);
    lua_pop(threadL, 1);
    return L;
#endif
}

inline int rawgetfield(lua_State* L, int index, const char* key)
{
    LUABRIDGE_ASSERT(lua_istable(L, index));
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
#if LUA_VERSION_NUM <= 502
    lua_rawget(L, index);
    return lua_type(L, -1);
#else
    return lua_rawget(L, index);
#endif
}

inline void rawsetfield(lua_State* L, int index, const char* key)
{
    LUABRIDGE_ASSERT(lua_istable(L, index));
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_insert(L, -2);
    lua_rawset(L, index);
}

[[nodiscard]] inline bool isfulluserdata(lua_State* L, int index)
{
    return lua_isuserdata(L, index) && !lua_islightuserdata(L, index);
}

[[nodiscard]] inline bool equalstates(lua_State* L1, lua_State* L2)
{
    return lua_topointer(L1, LUA_REGISTRYINDEX) == lua_topointer(L2, LUA_REGISTRYINDEX);
}

[[nodiscard]] inline int table_length(lua_State* L, int index)
{
    LUABRIDGE_ASSERT(lua_istable(L, index));

    int items_count = 0;

    lua_pushnil(L);
    while (lua_next(L, index) != 0)
    {
        ++items_count;

        lua_pop(L, 1);
    }

    return items_count;
}

template <class T>
[[nodiscard]] T* align(void* ptr) noexcept
{
    const auto address = reinterpret_cast<size_t>(ptr);

    const auto offset = address % alignof(T);
    const auto aligned_address = (offset == 0) ? address : (address + alignof(T) - offset);

    return reinterpret_cast<T*>(aligned_address);
}

template <std::size_t Alignment, class T, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
[[nodiscard]] bool is_aligned(T address) noexcept
{
    static_assert(Alignment > 0u);

    return (reinterpret_cast<std::uintptr_t>(address) & (Alignment - 1u)) == 0u;
}

template <class T>
[[nodiscard]] constexpr size_t maximum_space_needed_to_align() noexcept
{
    return sizeof(T) + alignof(T) - 1;
}

template <class T>
int lua_deleteuserdata_aligned(lua_State* L)
{
    LUABRIDGE_ASSERT(isfulluserdata(L, 1));

    T* aligned = align<T>(lua_touserdata(L, 1));
    aligned->~T();

    return 0;
}

template <class T, class... Args>
void* lua_newuserdata_aligned(lua_State* L, Args&&... args)
{
    using U = std::remove_reference_t<T>;

#if LUABRIDGE_ON_LUAU
    void* pointer = lua_newuserdatadtor(L, maximum_space_needed_to_align<U>(), [](void* x)
    {
        U* aligned = align<U>(x);
        aligned->~U();
    });
#else
    void* pointer = lua_newuserdata_x<U>(L, maximum_space_needed_to_align<U>());

    lua_newtable(L);
    lua_pushcfunction_x(L, &lua_deleteuserdata_aligned<U>, "");
    rawsetfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
#endif

    U* aligned = align<U>(pointer);

    new (aligned) U(std::forward<Args>(args)...);

    return pointer;
}

inline int raise_lua_error(lua_State* L, const char* fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    lua_pushvfstring(L, fmt, argp);
    va_end(argp);

    const char* message = lua_tostring(L, -1);
    if (message != nullptr)
    {
        if (auto str = std::string_view(message); !str.empty() && str[0] == '[')
            return lua_error_x(L);
    }

    bool pushed_error = false;
    for (int level = 1; level <= 2; ++level)
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

    lua_pushvalue(L, -2);
    lua_remove(L, -3);
    lua_concat(L, 2);

    return lua_error_x(L);
}

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
            return value <= static_cast<T>((std::numeric_limits<U>::max)());

        return value >= static_cast<T>((std::numeric_limits<U>::min)())
            && static_cast<U>(value) <= (std::numeric_limits<U>::max)();
    }

    if constexpr (sizeof(T) < sizeof(U))
    {
        return static_cast<U>(value) >= (std::numeric_limits<U>::min)()
            && static_cast<U>(value) <= (std::numeric_limits<U>::max)();
    }

    if constexpr (std::is_unsigned_v<T>)
        return value <= static_cast<T>((std::numeric_limits<U>::max)());

    return value >= static_cast<T>((std::numeric_limits<U>::min)())
        && value <= static_cast<T>((std::numeric_limits<U>::max)());
}

template <class U = lua_Integer>
bool is_integral_representable_by(lua_State* L, int index)
{
    int isValid = 0;

    const auto value = tointeger(L, index, &isValid);

    return isValid ? is_integral_representable_by<U>(value) : false;
}

template <class U = lua_Number, class T>
constexpr bool is_floating_point_representable_by(T value)
{
    if constexpr (sizeof(T) == sizeof(U))
        return true;

    if constexpr (sizeof(T) < sizeof(U))
        return static_cast<U>(value) >= -(std::numeric_limits<U>::max)()
            && static_cast<U>(value) <= (std::numeric_limits<U>::max)();

    return value >= static_cast<T>(-(std::numeric_limits<U>::max)())
        && value <= static_cast<T>((std::numeric_limits<U>::max)());
}

template <class U = lua_Number>
bool is_floating_point_representable_by(lua_State* L, int index)
{
    int isValid = 0;

    const auto value = tonumber(L, index, &isValid);

    return isValid ? is_floating_point_representable_by<U>(value) : false;
}

} 


// End File: Source/LuaBridge/detail/LuaHelpers.h

// Begin File: Source/LuaBridge/detail/Errors.h

namespace luabridge {

namespace detail {

static inline constexpr char error_lua_stack_overflow[] = "stack overflow";

} 

enum class ErrorCode
{
    ClassNotRegistered = 1,

    LuaStackOverflow,

    LuaFunctionCallFailed,

    IntegerDoesntFitIntoLuaInteger,
    
    FloatingPointDoesntFitIntoLuaNumber,

    InvalidTypeCast,

    InvalidTableSizeInCast
};

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

        case ErrorCode::InvalidTypeCast:
            return "The lua object can't be cast to desired type";

        case ErrorCode::InvalidTableSizeInCast:
            return "The lua table has different size than expected";

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
} 

inline std::error_code makeErrorCode(ErrorCode e)
{
    return { static_cast<int>(e), detail::ErrorCategory::getInstance() };
}

inline std::error_code make_error_code(ErrorCode e)
{
    return { static_cast<int>(e), detail::ErrorCategory::getInstance() };
}
} 

namespace std {
template <> struct is_error_code_enum<luabridge::ErrorCode> : true_type {};
} 


// End File: Source/LuaBridge/detail/Errors.h

// Begin File: Source/LuaBridge/detail/Expected.h

#if LUABRIDGE_HAS_EXCEPTIONS
#endif

namespace luabridge {
namespace detail {
using std::swap;

template <class T, class... Args>
T* construct_at(T* ptr, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
{
    return static_cast<T*>(::new (const_cast<void*>(static_cast<const void*>(ptr))) T(std::forward<Args>(args)...));
}

template <class T, class U, class = void>
struct is_swappable_with_impl : std::false_type
{
};

template <class T, class U>
struct is_swappable_with_impl<T, U, std::void_t<decltype(swap(std::declval<T>(), std::declval<U>()))>>
    : std::true_type
{
};

template <class T, class U>
struct is_nothrow_swappable_with_impl
{
    static constexpr bool value = noexcept(swap(std::declval<T>(), std::declval<U>())) && noexcept(swap(std::declval<U>(), std::declval<T>()));

    using type = std::bool_constant<value>;
};

template <class T, class U>
struct is_swappable_with
    : std::conjunction<
          is_swappable_with_impl<std::add_lvalue_reference_t<T>, std::add_lvalue_reference_t<U>>,
          is_swappable_with_impl<std::add_lvalue_reference_t<U>, std::add_lvalue_reference_t<T>>>::type
{
};

template <class T, class U>
struct is_nothrow_swappable_with
    : std::conjunction<is_swappable_with<T, U>, is_nothrow_swappable_with_impl<T, U>>::type
{
};

template <class T>
struct is_nothrow_swappable
    : std::is_nothrow_swappable_with<std::add_lvalue_reference_t<T>, std::add_lvalue_reference_t<T>>
{
};

template <class T, class = void>
struct has_member_message : std::false_type
{
};

template <class T>
struct has_member_message<T, std::void_t<decltype(std::declval<T>().message())>> : std::true_type
{
};

template <class T>
inline static constexpr bool has_member_message_v = has_member_message<T>::value;
} 

template <class T, class E>
class Expected;

struct UnexpectType
{
    constexpr UnexpectType() = default;
};

static constexpr auto unexpect = UnexpectType();

namespace detail {
template <class T, class E, bool = std::is_default_constructible_v<T>, bool = (std::is_void_v<T> || std::is_trivial_v<T>) && std::is_trivial_v<E>>
union expected_storage
{
public:
    template <class U = T, class = std::enable_if_t<std::is_default_constructible_v<U>>>
    constexpr expected_storage() noexcept
        : value_()
    {
    }

    template <class... Args>
    constexpr explicit expected_storage(std::in_place_t, Args&&... args) noexcept
        : value_(std::forward<Args>(args)...)
    {
    }

    template <class... Args>
    constexpr explicit expected_storage(UnexpectType, Args&&... args) noexcept
        : error_(std::forward<Args>(args)...)
    {
    }

    ~expected_storage() = default;

    constexpr const T& value() const noexcept
    {
        return value_;
    }

    constexpr T& value() noexcept
    {
        return value_;
    }

    constexpr const E& error() const noexcept
    {
        return error_;
    }

    constexpr E& error() noexcept
    {
        return error_;
    }

private:
    T value_;
    E error_;
};

template <class E>
union expected_storage<void, E, true, true>
{
public:
    constexpr expected_storage() noexcept
        : dummy_(0)
    {
    }

    template <class... Args>
    constexpr explicit expected_storage(UnexpectType, Args&&... args) noexcept
        : error_(std::forward<Args>(args)...)
    {
    }

    ~expected_storage() = default;

    constexpr const E& error() const noexcept
    {
        return error_;
    }

    constexpr E& error() noexcept
    {
        return error_;
    }

private:
    char dummy_;
    E error_;
};

template <class T, class E>
union expected_storage<T, E, true, false>
{
public:
    constexpr expected_storage() noexcept(std::is_nothrow_default_constructible_v<T>)
        : value_()
    {
    }

    template <class... Args>
    constexpr explicit expected_storage(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : value_(std::forward<Args>(args)...)
    {
    }

    template <class... Args>
    constexpr explicit expected_storage(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : error_(std::forward<Args>(args)...)
    {
    }

    ~expected_storage()
    {
    }

    constexpr const T& value() const noexcept
    {
        return value_;
    }

    constexpr T& value() noexcept
    {
        return value_;
    }

    constexpr const E& error() const noexcept
    {
        return error_;
    }

    constexpr E& error() noexcept
    {
        return error_;
    }

private:
    T value_;
    E error_;
};

template <class T, class E>
union expected_storage<T, E, false, false>
{
public:
    constexpr explicit expected_storage() noexcept
        : dummy_(0)
    {
    }

    template <class... Args>
    constexpr explicit expected_storage(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : value_(std::forward<Args>(args)...)
    {
    }

    template <class... Args>
    constexpr explicit expected_storage(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : error_(std::forward<Args>(args)...)
    {
    }

    ~expected_storage()
    {
    }

    constexpr const T& value() const noexcept
    {
        return value_;
    }

    constexpr T& value() noexcept
    {
        return value_;
    }

    constexpr const E& error() const noexcept
    {
        return error_;
    }

    constexpr E& error() noexcept
    {
        return error_;
    }

private:
    char dummy_;
    T value_;
    E error_;
};

template <class E>
union expected_storage<void, E, true, false>
{
public:
    constexpr expected_storage() noexcept
        : dummy_(0)
    {
    }

    template <class... Args>
    constexpr explicit expected_storage(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : error_(std::forward<Args>(args)...)
    {
    }

    ~expected_storage() = default;

    constexpr const E& error() const noexcept
    {
        return error_;
    }

    constexpr E& error() noexcept
    {
        return error_;
    }

private:
    char dummy_;
    E error_;
};

template <class T, class E, bool IsCopyConstructible, bool IsMoveConstructible>
class expected_base_trivial
{
    using this_type = expected_base_trivial<T, E, IsCopyConstructible, IsMoveConstructible>;

protected:
    using storage_type = expected_storage<T, E>;

    constexpr expected_base_trivial() noexcept
        : valid_(true)
    {
    }

    template <class... Args>
    constexpr expected_base_trivial(std::in_place_t, Args&&... args) noexcept
        : storage_(std::in_place, std::forward<Args>(args)...)
        , valid_(true)
    {
    }

    template <class... Args>
    constexpr expected_base_trivial(UnexpectType, Args&&... args) noexcept
        : storage_(unexpect, std::forward<Args>(args)...)
        , valid_(false)
    {
    }

    expected_base_trivial(const expected_base_trivial& other) noexcept
    {
        if (other.valid_)
        {
            construct(std::in_place, other.value());
        }
        else
        {
            construct(unexpect, other.error());
        }
    }

    expected_base_trivial(expected_base_trivial&& other) noexcept
    {
        if (other.valid_)
        {
            construct(std::in_place, std::move(other.value()));
        }
        else
        {
            construct(unexpect, std::move(other.error()));
        }
    }

    ~expected_base_trivial() noexcept = default;

    constexpr const T& value() const noexcept
    {
        return storage_.value();
    }

    constexpr T& value() noexcept
    {
        return storage_.value();
    }

    constexpr const E& error() const noexcept
    {
        return storage_.error();
    }

    constexpr E& error() noexcept
    {
        return storage_.error();
    }

    constexpr const T* valuePtr() const noexcept
    {
        return std::addressof(value());
    }

    constexpr T* valuePtr() noexcept
    {
        return std::addressof(value());
    }

    constexpr const E* errorPtr() const noexcept
    {
        return std::addressof(error());
    }

    constexpr E* errorPtr() noexcept
    {
        return std::addressof(error());
    }

    constexpr bool valid() const noexcept
    {
        return valid_;
    }

    template <class... Args>
    inline T& construct(std::in_place_t, Args&&... args) noexcept
    {
        valid_ = true;
        return *detail::construct_at(valuePtr(), std::forward<Args>(args)...);
    }

    template <class... Args>
    inline E& construct(UnexpectType, Args&&... args) noexcept
    {
        valid_ = false;
        return *detail::construct_at(errorPtr(), std::forward<Args>(args)...);
    }

    inline void destroy() noexcept
    {
    }

private:
    storage_type storage_;
    bool valid_;
};

template <class T, class E, bool IsCopyConstructible, bool IsMoveConstructible>
class expected_base_non_trivial
{
    using this_type = expected_base_non_trivial<T, E, IsCopyConstructible, IsMoveConstructible>;

protected:
    using storage_type = expected_storage<T, E>;

    constexpr expected_base_non_trivial() noexcept(std::is_nothrow_default_constructible_v<storage_type>)
        : valid_(true)
    {
    }

    template <class... Args>
    constexpr expected_base_non_trivial(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<storage_type, std::in_place_t, Args...>)
        : storage_(std::in_place, std::forward<Args>(args)...)
        , valid_(true)
    {
    }

    template <class... Args>
    constexpr expected_base_non_trivial(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<storage_type, UnexpectType, Args...>)
        : storage_(unexpect, std::forward<Args>(args)...)
        , valid_(false)
    {
    }

    expected_base_non_trivial(const expected_base_non_trivial& other)
    {
        if (other.valid_)
        {
            construct(std::in_place, other.value());
        }
        else
        {
            construct(unexpect, other.error());
        }
    }

    expected_base_non_trivial(expected_base_non_trivial&& other) noexcept
    {
        if (other.valid_)
        {
            construct(std::in_place, std::move(other.value()));
        }
        else
        {
            construct(unexpect, std::move(other.error()));
        }
    }

    ~expected_base_non_trivial()
    {
        destroy();
    }

    constexpr const T& value() const noexcept
    {
        return storage_.value();
    }

    constexpr T& value() noexcept
    {
        return storage_.value();
    }

    constexpr const E& error() const noexcept
    {
        return storage_.error();
    }

    constexpr E& error() noexcept
    {
        return storage_.error();
    }

    constexpr const T* valuePtr() const noexcept
    {
        return std::addressof(value());
    }

    constexpr T* valuePtr() noexcept
    {
        return std::addressof(value());
    }

    constexpr const E* errorPtr() const noexcept
    {
        return std::addressof(error());
    }

    constexpr E* errorPtr() noexcept
    {
        return std::addressof(error());
    }

    constexpr bool valid() const noexcept
    {
        return valid_;
    }

    template <class... Args>
    inline T& construct(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        valid_ = true;
        return *detail::construct_at(valuePtr(), std::forward<Args>(args)...);
    }

    template <class... Args>
    inline E& construct(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
    {
        valid_ = false;
        return *detail::construct_at(errorPtr(), std::forward<Args>(args)...);
    }

    inline void destroy() noexcept(std::is_nothrow_destructible_v<T>&& std::is_nothrow_destructible_v<E>)
    {
        if (valid_)
        {
            std::destroy_at(valuePtr());
        }
        else
        {
            std::destroy_at(errorPtr());
        }
    }

private:
    storage_type storage_;
    bool valid_;
};

template <class T, class E, bool IsMoveConstructible>
class expected_base_non_trivial<T, E, false, IsMoveConstructible>
{
    using this_type = expected_base_non_trivial<T, E, false, IsMoveConstructible>;

protected:
    using storage_type = expected_storage<T, E>;

    constexpr expected_base_non_trivial() noexcept(std::is_nothrow_default_constructible_v<storage_type>)
        : valid_(true)
    {
    }

    template <class... Args>
    constexpr expected_base_non_trivial(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<storage_type, std::in_place_t, Args...>)
        : storage_(std::in_place, std::forward<Args>(args)...)
        , valid_(true)
    {
    }

    template <class... Args>
    constexpr expected_base_non_trivial(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<storage_type, UnexpectType, Args...>)
        : storage_(unexpect, std::forward<Args>(args)...)
        , valid_(false)
    {
    }

    expected_base_non_trivial(const expected_base_non_trivial& other) = delete;

    expected_base_non_trivial(expected_base_non_trivial&& other) noexcept
    {
        if (other.valid_)
        {
            construct(std::in_place, std::move(other.value()));
        }
        else
        {
            construct(unexpect, std::move(other.error()));
        }
    }

    ~expected_base_non_trivial()
    {
        destroy();
    }

    constexpr const T& value() const noexcept
    {
        return storage_.value();
    }

    constexpr T& value() noexcept
    {
        return storage_.value();
    }

    constexpr const E& error() const noexcept
    {
        return storage_.error();
    }

    constexpr E& error() noexcept
    {
        return storage_.error();
    }

    constexpr const T* valuePtr() const noexcept
    {
        return std::addressof(value());
    }

    constexpr T* valuePtr() noexcept
    {
        return std::addressof(value());
    }

    constexpr const E* errorPtr() const noexcept
    {
        return std::addressof(error());
    }

    constexpr E* errorPtr() noexcept
    {
        return std::addressof(error());
    }

    constexpr bool valid() const noexcept
    {
        return valid_;
    }

    template <class... Args>
    inline T& construct(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        valid_ = true;
        return *detail::construct_at(valuePtr(), std::forward<Args>(args)...);
    }

    template <class... Args>
    inline E& construct(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
    {
        valid_ = false;
        return *detail::construct_at(errorPtr(), std::forward<Args>(args)...);
    }

    inline void destroy() noexcept(std::is_nothrow_destructible_v<T>&& std::is_nothrow_destructible_v<E>)
    {
        if (valid_)
        {
            std::destroy_at(valuePtr());
        }
        else
        {
            std::destroy_at(errorPtr());
        }
    }

private:
    storage_type storage_;
    bool valid_;
};

template <class T, class E, bool IsCopyConstructible>
class expected_base_non_trivial<T, E, IsCopyConstructible, false>
{
    using this_type = expected_base_non_trivial<T, E, IsCopyConstructible, false>;

protected:
    using storage_type = expected_storage<T, E>;

    template <class U = storage_type, class = std::enable_if_t<std::is_default_constructible_v<U>>>
    constexpr expected_base_non_trivial() noexcept(std::is_nothrow_default_constructible_v<storage_type>)
        : valid_(true)
    {
    }

    template <class... Args>
    constexpr expected_base_non_trivial(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<storage_type, std::in_place_t, Args...>)
        : storage_(std::in_place, std::forward<Args>(args)...)
        , valid_(true)
    {
    }

    template <class... Args>
    constexpr expected_base_non_trivial(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<storage_type, UnexpectType, Args...>)
        : storage_(unexpect, std::forward<Args>(args)...)
        , valid_(false)
    {
    }

    expected_base_non_trivial(const expected_base_non_trivial& other)
    {
        if (other.valid_)
        {
            construct(std::in_place, other.value());
        }
        else
        {
            construct(unexpect, other.error());
        }
    }

    expected_base_non_trivial(expected_base_non_trivial&& other) = delete;

    ~expected_base_non_trivial()
    {
        destroy();
    }

    constexpr const T& value() const noexcept
    {
        return storage_.value();
    }

    constexpr T& value() noexcept
    {
        return storage_.value();
    }

    constexpr const E& error() const noexcept
    {
        return storage_.error();
    }

    constexpr E& error() noexcept
    {
        return storage_.error();
    }

    constexpr const T* valuePtr() const noexcept
    {
        return std::addressof(value());
    }

    constexpr T* valuePtr() noexcept
    {
        return std::addressof(value());
    }

    constexpr const E* errorPtr() const noexcept
    {
        return std::addressof(error());
    }

    constexpr E* errorPtr() noexcept
    {
        return std::addressof(error());
    }

    constexpr bool valid() const noexcept
    {
        return valid_;
    }

    template <class... Args>
    inline T& construct(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        valid_ = true;
        return *detail::construct_at(valuePtr(), std::forward<Args>(args)...);
    }

    template <class... Args>
    inline E& construct(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
    {
        valid_ = false;
        return *detail::construct_at(errorPtr(), std::forward<Args>(args)...);
    }

    inline void destroy() noexcept(std::is_nothrow_destructible_v<T>&& std::is_nothrow_destructible_v<E>)
    {
        if (valid_)
        {
            std::destroy_at(valuePtr());
        }
        else
        {
            std::destroy_at(errorPtr());
        }
    }

private:
    storage_type storage_;
    bool valid_;
};

template <class T, class E>
class expected_base_non_trivial<T, E, false, false>
{
    using this_type = expected_base_non_trivial<T, E, false, false>;

protected:
    using storage_type = expected_storage<T, E>;

    template <class U = storage_type, class = std::enable_if_t<std::is_default_constructible_v<U>>>
    constexpr expected_base_non_trivial() noexcept(std::is_nothrow_default_constructible_v<storage_type>)
        : valid_(true)
    {
    }

    template <class... Args>
    constexpr expected_base_non_trivial(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<storage_type, std::in_place_t, Args...>)
        : storage_(std::in_place, std::forward<Args>(args)...)
        , valid_(true)
    {
    }

    template <class... Args>
    constexpr expected_base_non_trivial(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<storage_type, UnexpectType, Args...>)
        : storage_(unexpect, std::forward<Args>(args)...)
        , valid_(false)
    {
    }

    expected_base_non_trivial(const expected_base_non_trivial& other) = delete;

    expected_base_non_trivial(expected_base_non_trivial&& other) = delete;

    ~expected_base_non_trivial()
    {
        destroy();
    }

    constexpr const T& value() const noexcept
    {
        return storage_.value();
    }

    constexpr T& value() noexcept
    {
        return storage_.value();
    }

    constexpr const E& error() const noexcept
    {
        return storage_.error();
    }

    constexpr E& error() noexcept
    {
        return storage_.error();
    }

    constexpr const T* valuePtr() const noexcept
    {
        return std::addressof(value());
    }

    constexpr T* valuePtr() noexcept
    {
        return std::addressof(value());
    }

    constexpr const E* errorPtr() const noexcept
    {
        return std::addressof(error());
    }

    constexpr E* errorPtr() noexcept
    {
        return std::addressof(error());
    }

    constexpr bool valid() const noexcept
    {
        return valid_;
    }

    template <class... Args>
    inline T& construct(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        valid_ = true;
        return *detail::construct_at(valuePtr(), std::forward<Args>(args)...);
    }

    template <class... Args>
    inline E& construct(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
    {
        valid_ = false;
        return *detail::construct_at(errorPtr(), std::forward<Args>(args)...);
    }

    inline void destroy() noexcept(std::is_nothrow_destructible_v<T>&& std::is_nothrow_destructible_v<E>)
    {
        if (valid_)
        {
            std::destroy_at(valuePtr());
        }
        else
        {
            std::destroy_at(errorPtr());
        }
    }

private:
    storage_type storage_;
    bool valid_;
};

template <class T, class E, bool IsCopyConstructible, bool IsMoveConstructible>
using expected_base = std::conditional_t<
    (std::is_void_v<T> || std::is_trivially_destructible_v<T>) && std::is_trivially_destructible_v<E>,
    expected_base_trivial<T, E, IsCopyConstructible, IsMoveConstructible>,
    expected_base_non_trivial<T, E, IsCopyConstructible, IsMoveConstructible>>;

}  

template <class E>
class Unexpected
{
    static_assert(!std::is_reference_v<E> && !std::is_void_v<E>, "Unexpected type can't be a reference or void");

public:
    Unexpected() = delete;

    constexpr explicit Unexpected(E&& e) noexcept(std::is_nothrow_move_constructible_v<E>)
        : error_(std::move(e))
    {
    }

    constexpr explicit Unexpected(const E& e) noexcept(std::is_nothrow_copy_constructible_v<E>)
        : error_(e)
    {
    }

    constexpr const E& value() const& noexcept
    {
        return error_;
    }

    constexpr E& value() & noexcept
    {
        return error_;
    }

    constexpr const E&& value() const&& noexcept
    {
        return std::move(error_);
    }

    constexpr E&& value() && noexcept
    {
        return std::move(error_);
    }

private:
    E error_;
};

template <class E>
constexpr bool operator==(const Unexpected<E>& lhs, const Unexpected<E>& rhs) noexcept
{
    return lhs.value() == rhs.value();
}

template <class E>
constexpr bool operator!=(const Unexpected<E>& lhs, const Unexpected<E>& rhs) noexcept
{
    return lhs.value() != rhs.value();
}

template <class E>
constexpr inline Unexpected<std::decay_t<E>> makeUnexpected(E&& error) noexcept(std::is_nothrow_constructible_v<Unexpected<std::decay_t<E>>, E>)
{
    return Unexpected<std::decay_t<E>>{ std::forward<E>(error) };
}

#if LUABRIDGE_HAS_EXCEPTIONS
template <class E>
class BadExpectedAccess;

template <>
class BadExpectedAccess<void> : public std::runtime_error
{
    template <class T>
    friend class BadExpectedAccess;

    BadExpectedAccess(std::in_place_t) noexcept
        : std::runtime_error("Bad access to expected value")
    {
    }

public:
    BadExpectedAccess() noexcept
        : BadExpectedAccess(std::in_place)
    {
    }

    explicit BadExpectedAccess(const std::string& message) noexcept
        : std::runtime_error(message)
    {
    }
};

template <class E>
class BadExpectedAccess : public std::runtime_error
{
public:
    explicit BadExpectedAccess(E error) noexcept(std::is_nothrow_constructible_v<E, E&&>)
        : std::runtime_error([](const E& error)
            {
                if constexpr (detail::has_member_message_v<E>)
                    return error.message();
                else
                    return "Bad access to expected value";
            }(error))
        , error_(std::move(error))
    {
    }

    const E& error() const& noexcept
    {
        return error_;
    }

    E& error() & noexcept
    {
        return error_;
    }

    E&& error() && noexcept
    {
        return std::move(error_);
    }

private:
    E error_;
};
#endif

template <class T>
struct is_expected : std::false_type
{
};

template <class T, class E>
struct is_expected<Expected<T, E>> : std::true_type
{
};
template <class T>
struct is_unexpected : std::false_type
{
};

template <class E>
struct is_unexpected<Unexpected<E>> : std::true_type
{
};

template <class T, class E>
class Expected : public detail::expected_base<T, E, std::is_copy_constructible_v<T>, std::is_move_constructible_v<T>>
{
    static_assert(!std::is_reference_v<E> && !std::is_void_v<E>, "Unexpected type can't be a reference or void");

    using base_type = detail::expected_base<T, E, std::is_copy_constructible_v<T>, std::is_move_constructible_v<T>>;
    using this_type = Expected<T, E>;

public:
    using value_type = T;

    using error_type = E;

    using unexpected_type = Unexpected<E>;

    template <class U>
    struct rebind
    {
        using type = Expected<U, error_type>;
    };

    template <class U = T, class = std::enable_if_t<std::is_default_constructible_v<U>>>
    constexpr Expected() noexcept(std::is_nothrow_default_constructible_v<base_type>)
        : base_type()
    {
    }

    constexpr Expected(const Expected& other) noexcept(std::is_nothrow_copy_constructible_v<base_type>) = default;

    constexpr Expected(Expected&& other) noexcept(std::is_nothrow_move_constructible_v<base_type>) = default;

    template <class U, class G>
    Expected(const Expected<U, G>& other)
    {
        if (other.hasValue())
        {
            this->construct(std::in_place, other.value());
        }
        else
        {
            this->construct(unexpect, other.error());
        }
    }

    template <class U, class G>
    Expected(Expected<U, G>&& other)
    {
        if (other.hasValue())
        {
            this->construct(std::in_place, std::move(other.value()));
        }
        else
        {
            this->construct(unexpect, std::move(other.error()));
        }
    }

    template <class U = T, std::enable_if_t<!std::is_void_v<T> && std::is_constructible_v<T, U&&> && !std::is_same_v<std::decay_t<U>, std::in_place_t> && !is_expected<std::decay_t<U>>::value && !is_unexpected<std::decay_t<U>>::value, int> = 0>
    constexpr Expected(U&& value) noexcept(std::is_nothrow_constructible_v<base_type, std::in_place_t, U>)
        : base_type(std::in_place, std::forward<U>(value))
    {
    }

    template <class... Args>
    constexpr explicit Expected(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<base_type, std::in_place_t, Args...>)
        : base_type(std::in_place, std::forward<Args>(args)...)
    {
    }

    template <class U, class... Args>
    constexpr explicit Expected(std::in_place_t, std::initializer_list<U> ilist, Args&&... args) noexcept(std::is_nothrow_constructible_v<base_type, std::in_place_t, std::initializer_list<U>, Args...>)
        : base_type(std::in_place, ilist, std::forward<Args>(args)...)
    {
    }

    template <class G = E>
    constexpr Expected(const Unexpected<G>& u) noexcept(std::is_nothrow_constructible_v<base_type, UnexpectType, const G&>)
        : base_type(unexpect, u.value())
    {
    }

    template <class G = E>
    constexpr Expected(Unexpected<G>&& u) noexcept(std::is_nothrow_constructible_v<base_type, UnexpectType, G&&>)
        : base_type(unexpect, std::move(u.value()))
    {
    }

    template <class... Args>
    constexpr explicit Expected(UnexpectType, Args&&... args) noexcept(std::is_nothrow_constructible_v<base_type, UnexpectType, Args...>)
        : base_type(unexpect, std::forward<Args>(args)...)
    {
    }

    template <class U, class... Args>
    constexpr explicit Expected(UnexpectType, std::initializer_list<U> ilist, Args&&... args) noexcept(std::is_nothrow_constructible_v<base_type, UnexpectType, std::initializer_list<U>, Args...>)
        : base_type(unexpect, ilist, std::forward<Args>(args)...)
    {
    }

    Expected& operator=(const Expected& other)
    {
        if (other.hasValue())
        {
            assign(std::in_place, other.value());
        }
        else
        {
            assign(unexpect, other.error());
        }

        return *this;
    }

    Expected& operator=(Expected&& other) noexcept
    {
        if (other.hasValue())
        {
            assign(std::in_place, std::move(other.value()));
        }
        else
        {
            assign(unexpect, std::move(other.error()));
        }

        return *this;
    }

    template <class U = T, std::enable_if_t<!is_expected<std::decay_t<U>>::value && !is_unexpected<std::decay_t<U>>::value, int> = 0>
    Expected& operator=(U&& value)
    {
        assign(std::in_place, std::forward<U>(value));
        return *this;
    }

    template <class G = E>
    Expected& operator=(const Unexpected<G>& u)
    {
        assign(unexpect, u.value());
        return *this;
    }

    template <class G = E>
    Expected& operator=(Unexpected<G>&& u)
    {
        assign(unexpect, std::move(u.value()));
        return *this;
    }

    template <class... Args>
    T& emplace(Args&&... args) noexcept(noexcept(std::declval<this_type>().assign(std::in_place, std::forward<Args>(args)...)))
    {
        return assign(std::in_place, std::forward<Args>(args)...);
    }

    template <class U, class... Args>
    T& emplace(std::initializer_list<U> ilist, Args&&... args) noexcept(noexcept(std::declval<this_type>().assign(std::in_place, ilist, std::forward<Args>(args)...)))
    {
        return assign(std::in_place, ilist, std::forward<Args>(args)...);
    }

    void swap(Expected& other) noexcept(detail::is_nothrow_swappable<value_type>::value && detail::is_nothrow_swappable<error_type>::value)
    {
        using std::swap;

        if (hasValue())
        {
            if (other.hasValue())
            {
                swap(value(), other.value());
            }
            else
            {
                E error = std::move(other.error());
                other.assign(std::in_place, std::move(value()));
                assign(unexpect, std::move(error));
            }
        }
        else
        {
            if (other.hasValue())
            {
                other.swap(*this);
            }
            else
            {
                swap(error(), other.error());
            }
        }
    }

    constexpr const T* operator->() const
    {
        return base_type::valuePtr();
    }

    constexpr T* operator->()
    {
        return base_type::valuePtr();
    }

    constexpr const T& operator*() const&
    {
        return value();
    }

    constexpr T& operator*() &
    {
        return value();
    }

    constexpr const T&& operator*() const&&
    {
        return std::move(value());
    }

    constexpr T&& operator*() &&
    {
        return std::move(value());
    }

    constexpr explicit operator bool() const noexcept
    {
        return hasValue();
    }

    constexpr bool hasValue() const noexcept
    {
        return base_type::valid();
    }

    constexpr const T& value() const& LUABRIDGE_IF_NO_EXCEPTIONS(noexcept)
    {
#if LUABRIDGE_HAS_EXCEPTIONS
        if (!hasValue())
            throw BadExpectedAccess<E>(error());
#endif

        return base_type::value();
    }

    constexpr T& value() & LUABRIDGE_IF_NO_EXCEPTIONS(noexcept)
    {
#if LUABRIDGE_HAS_EXCEPTIONS
        if (!hasValue())
            throw BadExpectedAccess<E>(error());
#endif

        return base_type::value();
    }

    constexpr const T&& value() const&& LUABRIDGE_IF_NO_EXCEPTIONS(noexcept)
    {
#if LUABRIDGE_HAS_EXCEPTIONS
        if (!hasValue())
            throw BadExpectedAccess<E>(error());
#endif

        return std::move(base_type::value());
    }

    constexpr T&& value() && LUABRIDGE_IF_NO_EXCEPTIONS(noexcept)
    {
#if LUABRIDGE_HAS_EXCEPTIONS
        if (!hasValue())
            throw BadExpectedAccess<E>(error());
#endif
        return std::move(base_type::value());
    }

    constexpr const E& error() const& noexcept
    {
        return base_type::error();
    }

    constexpr E& error() & noexcept
    {
        return base_type::error();
    }

    constexpr const E&& error() const&& noexcept
    {
        return std::move(base_type::error());
    }

    constexpr E&& error() && noexcept
    {
        return std::move(base_type::error());
    }

    template <class U>
    constexpr T valueOr(U&& defaultValue) const&
    {
        return hasValue() ? value() : static_cast<T>(std::forward<U>(defaultValue));
    }

    template <class U>
    T valueOr(U&& defaultValue) &&
    {
        return hasValue() ? std::move(value()) : static_cast<T>(std::forward<U>(defaultValue));
    }

private:
    template <class Tag, class... Args>
    auto assign(Tag tag, Args&&... args) noexcept(noexcept(std::declval<this_type>().destroy()) && noexcept(std::declval<this_type>().construct(tag, std::forward<Args>(args)...)))
        -> decltype(std::declval<this_type>().construct(tag, std::forward<Args>(args)...))
    {
        this->destroy();

        return this->construct(tag, std::forward<Args>(args)...);
    }
};

template <class E>
class Expected<void, E> : public detail::expected_base<void, E, std::is_copy_constructible_v<E>, std::is_move_constructible_v<E>>
{
    static_assert(!std::is_reference_v<E> && !std::is_void_v<E>, "Unexpected type can't be a reference or void");

    using base_type = detail::expected_base<void, E, std::is_copy_constructible_v<E>, std::is_move_constructible_v<E>>;
    using this_type = Expected<void, E>;

public:
    using value_type = void;

    using error_type = E;

    using unexpected_type = Unexpected<E>;

    template <class U>
    struct rebind
    {
        using type = Expected<U, error_type>;
    };

    constexpr Expected() = default;

    constexpr Expected(const Expected& other) = default;

    constexpr Expected(Expected&& other) = default;

    template <class G>
    Expected(const Expected<void, G>& other)
    {
        if (other.hasValue())
        {
            this->valid_ = true;
        }
        else
        {
            this->construct(unexpect, other.error());
        }
    }

    template <class G>
    Expected(Expected<void, G>&& other)
    {
        if (other.hasValue())
        {
            this->valid_ = true;
        }
        else
        {
            this->construct(unexpect, std::move(other.error()));
        }
    }

    template <class G = E>
    constexpr Expected(const Unexpected<G>& u)
        : base_type(unexpect, u.value())
    {
    }

    template <class G = E>
    constexpr Expected(Unexpected<G>&& u)
        : base_type(unexpect, std::move(u.value()))
    {
    }

    template <class... Args>
    constexpr explicit Expected(UnexpectType, Args&&... args)
        : base_type(unexpect, std::forward<Args>(args)...)
    {
    }

    template <class U, class... Args>
    constexpr explicit Expected(UnexpectType, std::initializer_list<U> ilist, Args&&... args)
        : base_type(unexpect, ilist, std::forward<Args>(args)...)
    {
    }

    Expected& operator=(const Expected& other)
    {
        if (other.hasValue())
        {
            assign(std::in_place);
        }
        else
        {
            assign(unexpect, other.error());
        }

        return *this;
    }

    Expected& operator=(Expected&& other)
    {
        if (other.hasValue())
        {
            assign(std::in_place);
        }
        else
        {
            assign(unexpect, std::move(other.error()));
        }

        return *this;
    }

    template <class G = E>
    Expected& operator=(const Unexpected<G>& u)
    {
        assign(unexpect, u.value());
        return *this;
    }

    template <class G = E>
    Expected& operator=(Unexpected<G>&& u)
    {
        assign(unexpect, std::move(u.value()));
        return *this;
    }

    void swap(Expected& other) noexcept(detail::is_nothrow_swappable<error_type>::value)
    {
        using std::swap;

        if (hasValue())
        {
            if (!other.hasValue())
            {
                assign(unexpect, std::move(other.error()));
                other.assign(std::in_place);
            }
        }
        else
        {
            if (other.hasValue())
            {
                other.swap(*this);
            }
            else
            {
                swap(error(), other.error());
            }
        }
    }

    constexpr explicit operator bool() const noexcept
    {
        return hasValue();
    }

    constexpr bool hasValue() const noexcept
    {
        return base_type::valid();
    }

    constexpr const E& error() const& noexcept
    {
        return base_type::error();
    }

    constexpr E& error() & noexcept
    {
        return base_type::error();
    }

    constexpr const E&& error() const&& noexcept
    {
        return std::move(base_type::error());
    }

    constexpr E&& error() && noexcept
    {
        return std::move(base_type::error());
    }

private:
    template <class Tag, class... Args>
    void assign(Tag tag, Args&&... args) noexcept(noexcept(std::declval<this_type>().destroy()) && noexcept(std::declval<this_type>().construct(tag, std::forward<Args>(args)...)))
    {
        this->destroy();
        this->construct(tag, std::forward<Args>(args)...);
    }
};

template <class T, class E>
constexpr bool operator==(const Expected<T, E>& lhs, const Expected<T, E>& rhs)
{
    return (lhs && rhs) ? *lhs == *rhs : ((!lhs && !rhs) ? lhs.error() == rhs.error() : false);
}

template <class E>
constexpr bool operator==(const Expected<void, E>& lhs, const Expected<void, E>& rhs)
{
    return (lhs && rhs) ? true : ((!lhs && !rhs) ? lhs.error() == rhs.error() : false);
}

template <class T, class E>
constexpr bool operator!=(const Expected<T, E>& lhs, const Expected<T, E>& rhs)
{
    return !(lhs == rhs);
}

template <class T, class E>
constexpr bool operator==(const Expected<T, E>& lhs, const T& rhs)
{
    return lhs ? *lhs == rhs : false;
}

template <class T, class E>
constexpr bool operator==(const T& lhs, const Expected<T, E>& rhs)
{
    return rhs == lhs;
}

template <class T, class E>
constexpr bool operator!=(const Expected<T, E>& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

template <class T, class E>
constexpr bool operator!=(const T& lhs, const Expected<T, E>& rhs)
{
    return rhs != lhs;
}

template <class T, class E>
constexpr bool operator==(const Expected<T, E>& lhs, const Unexpected<E>& rhs)
{
    return lhs ? false : lhs.error() == rhs.value();
}

template <class T, class E>
constexpr bool operator==(const Unexpected<E>& lhs, const Expected<T, E>& rhs)
{
    return rhs == lhs;
}

template <class T, class E>
constexpr bool operator!=(const Expected<T, E>& lhs, const Unexpected<E>& rhs)
{
    return !(lhs == rhs);
}

template <class T, class E>
constexpr bool operator!=(const Unexpected<E>& lhs, const Expected<T, E>& rhs)
{
    return rhs != lhs;
}
} 


// End File: Source/LuaBridge/detail/Expected.h

// Begin File: Source/LuaBridge/detail/Result.h

namespace luabridge {

struct Result
{
    Result() noexcept = default;

    Result(std::error_code ec) noexcept
        : m_ec(ec)
    {
    }

    Result(const Result&) noexcept = default;
    Result(Result&&) noexcept = default;
    Result& operator=(const Result&) noexcept = default;
    Result& operator=(Result&&) noexcept = default;

    explicit operator bool() const noexcept
    {
        return !m_ec;
    }

    std::error_code error() const noexcept
    {
        return m_ec;
    }

    operator std::error_code() const noexcept
    {
        return m_ec;
    }

    std::string message() const
    {
        return m_ec.message();
    }

#if LUABRIDGE_HAS_EXCEPTIONS
    void throw_on_error() const
    {
        if (m_ec)
            throw std::system_error(m_ec);
    }
#endif

private:
    std::error_code m_ec;
};

template <class T>
struct TypeResult
{
    TypeResult() noexcept = default;

    template <class U, class = std::enable_if_t<std::is_convertible_v<U, T> && !std::is_same_v<std::decay_t<U>, std::error_code>>>
    TypeResult(U&& value) noexcept
        : m_value(std::in_place, std::forward<U>(value))
    {
    }

    TypeResult(std::error_code ec) noexcept
        : m_value(makeUnexpected(ec))
    {
    }

    TypeResult(const TypeResult&) = default;
    TypeResult(TypeResult&&) = default;
    TypeResult& operator=(const TypeResult&) = default;
    TypeResult& operator=(TypeResult&&) = default;

    explicit operator bool() const noexcept
    {
        return m_value.hasValue();
    }

    const T& value() const
    {
        return m_value.value();
    }

    T& operator*() &
    {
        return m_value.value();
    }

    T operator*() &&
    {
        return std::move(m_value.value());
    }

    const T& operator*() const&
    {
        return m_value.value();
    }

    T operator*() const&&
    {
        return std::move(m_value.value());
    }

    template <class U>
    T valueOr(U&& defaultValue) const&
    {
        return m_value.valueOr(std::forward<U>(defaultValue));
    }

    template <class U>
    T valueOr(U&& defaultValue) &&
    {
        return m_value.valueOr(std::forward<U>(defaultValue));
    }

    std::error_code error() const
    {
        return m_value.error();
    }

    operator std::error_code() const
    {
        return m_value.error();
    }

    std::string message() const
    {
        return m_value.error().message();
    }

#if LUABRIDGE_HAS_EXCEPTIONS
    void throw_on_error() const
    {
        if (! m_value.hasValue())
            throw std::system_error(m_value.error());
    }
#endif

private:
    Expected<T, std::error_code> m_value;
};

template <class U>
inline bool operator==(const TypeResult<U>& lhs, const U& rhs) noexcept
{
    return lhs ? *lhs == rhs : false;
}

template <class U>
inline bool operator==(const U& lhs, const TypeResult<U>& rhs) noexcept
{
    return rhs == lhs;
}

template <class U>
inline bool operator!=(const TypeResult<U>& lhs, const U& rhs) noexcept
{
    return !(lhs == rhs);
}

template <class U>
inline bool operator!=(const U& lhs, const TypeResult<U>& rhs) noexcept
{
    return !(rhs == lhs);
}

} 


// End File: Source/LuaBridge/detail/Result.h

// Begin File: Source/LuaBridge/detail/ClassInfo.h

#if defined __clang__ || defined __GNUC__
#define LUABRIDGE_PRETTY_FUNCTION __PRETTY_FUNCTION__
#define LUABRIDGE_PRETTY_FUNCTION_PREFIX '='
#define LUABRIDGE_PRETTY_FUNCTION_SUFFIX ']'
#elif defined _MSC_VER
#define LUABRIDGE_PRETTY_FUNCTION __FUNCSIG__
#define LUABRIDGE_PRETTY_FUNCTION_PREFIX '<'
#define LUABRIDGE_PRETTY_FUNCTION_SUFFIX '>'
#endif

namespace luabridge {
namespace detail {

[[nodiscard]] constexpr auto fnv1a(const char* s, std::size_t count) noexcept
{
    uint32_t seed = 2166136261u;

    for (std::size_t i = 0; i < count; ++i)
        seed = static_cast<uint32_t>(static_cast<uint32_t>(seed ^ static_cast<uint8_t>(*s++)) * 16777619u);

    if constexpr (sizeof(void*) == 8)
        return static_cast<uint64_t>(seed);
    else
        return seed;
}

template <class T>
[[nodiscard]] static constexpr auto typeName(T* = nullptr) noexcept
{
    constexpr std::string_view prettyName{ LUABRIDGE_PRETTY_FUNCTION };

    constexpr auto first = prettyName.find_first_not_of(' ', prettyName.find_first_of(LUABRIDGE_PRETTY_FUNCTION_PREFIX) + 1);

    return prettyName.substr(first, prettyName.find_last_of(LUABRIDGE_PRETTY_FUNCTION_SUFFIX) - first);
}

template <class T, auto = typeName<T>().find_first_of('.')>
[[nodiscard]] static constexpr auto typeHash(T* = nullptr) noexcept
{
    constexpr auto stripped = typeName<T>();

    return fnv1a(stripped.data(), stripped.size());
}

[[nodiscard]] inline void* getExceptionsKey() noexcept
{
    return reinterpret_cast<void*>(0xc7);
}

[[nodiscard]] inline const void* getTypeKey() noexcept
{
    return reinterpret_cast<void*>(0x71);
}

[[nodiscard]] inline const void* getConstKey() noexcept
{
    return reinterpret_cast<void*>(0xc07);
}

[[nodiscard]] inline const void* getClassKey() noexcept
{
    return reinterpret_cast<void*>(0xc1a);
}

[[nodiscard]] inline const void* getClassOptionsKey() noexcept
{
    return reinterpret_cast<void*>(0xc2b);
}

[[nodiscard]] inline const void* getPropgetKey() noexcept
{
    return reinterpret_cast<void*>(0x6e7);
}

[[nodiscard]] inline const void* getPropsetKey() noexcept
{
    return reinterpret_cast<void*>(0x5e7);
}

[[nodiscard]] inline const void* getStaticKey() noexcept
{
    return reinterpret_cast<void*>(0x57a);
}

[[nodiscard]] inline const void* getParentKey() noexcept
{
    return reinterpret_cast<void*>(0xdad);
}

[[nodiscard]] inline const void* getIndexFallbackKey()
{
    return reinterpret_cast<void*>(0x81ca);
}

[[nodiscard]] inline const void* getIndexExtensibleKey()
{
    return reinterpret_cast<void*>(0x81cb);
}

[[nodiscard]] inline const void* getNewIndexFallbackKey()
{
    return reinterpret_cast<void*>(0x8107);
}

[[nodiscard]] inline const void* getNewIndexExtensibleKey()
{
    return reinterpret_cast<void*>(0x8108);
}

template <class T>
[[nodiscard]] const void* getStaticRegistryKey() noexcept
{
    static auto value = typeHash<T>();

    return reinterpret_cast<void*>(value);
}

template <class T>
[[nodiscard]] const void* getClassRegistryKey() noexcept
{
    static auto value = typeHash<T>() ^ 1;

    return reinterpret_cast<void*>(value);
}

template <class T>
[[nodiscard]] const void* getConstRegistryKey() noexcept
{
    static auto value = typeHash<T>() ^ 2;

    return reinterpret_cast<void*>(value);
}
} 
} 


// End File: Source/LuaBridge/detail/ClassInfo.h

// Begin File: Source/LuaBridge/detail/LuaException.h

namespace luabridge {

class LuaException : public std::exception
{
public:
    
    LuaException(lua_State* L, std::error_code code)
        : m_L(L)
        , m_code(code)
    {
    }

    ~LuaException() noexcept override
    {
    }

    const char* what() const noexcept override
    {
        return m_what.c_str();
    }

    static void raise(lua_State* L, std::error_code code)
    {
        LUABRIDGE_ASSERT(areExceptionsEnabled(L));

#if LUABRIDGE_HAS_EXCEPTIONS
        throw LuaException(L, code, FromLua{});
#else
        unused(L, code);

        std::abort();
#endif
    }

    static bool areExceptionsEnabled(lua_State* L) noexcept
    {
        lua_pushlightuserdata(L, detail::getExceptionsKey());
        lua_gettable(L, LUA_REGISTRYINDEX);

        const bool enabled = lua_isboolean(L, -1) ? static_cast<bool>(lua_toboolean(L, -1)) : false;
        lua_pop(L, 1);

        return enabled;
    }

    static void enableExceptions(lua_State* L) noexcept
    {
        lua_pushlightuserdata(L, detail::getExceptionsKey());
        lua_pushboolean(L, true);
        lua_settable(L, LUA_REGISTRYINDEX);

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
            return lua_error_x(L);
        }
    }
#endif

    lua_State* m_L = nullptr;
    std::error_code m_code;
    std::string m_what;
};

inline void enableExceptions(lua_State* L) noexcept
{
#if LUABRIDGE_HAS_EXCEPTIONS
    LuaException::enableExceptions(L);
#else
    unused(L);

    LUABRIDGE_ASSERT(false); 
#endif
}

} 


// End File: Source/LuaBridge/detail/LuaException.h

// Begin File: Source/LuaBridge/detail/TypeTraits.h

namespace luabridge {
namespace detail {
template <template <class...> class C, class... Ts>
std::true_type is_base_of_template_impl(const C<Ts...>*);

template <template <class...> class C>
std::false_type is_base_of_template_impl(...);

template <class T, template <class...> class C>
using is_base_of_template = decltype(is_base_of_template_impl<C>(std::declval<T*>()));

template <class T, template <class...> class C>
static inline constexpr bool is_base_of_template_v = is_base_of_template<T, C>::value;

template <class... Args>
constexpr bool dependent_false = false;

} 

template <class T>
struct ContainerTraits
{
    using IsNotContainer = bool;

    using Type = T;
};

template <class T>
struct ContainerTraits<std::shared_ptr<T>>
{
    using Type = T;

    template <class U = T>
    static std::shared_ptr<U> construct(U* t)
    {
        if constexpr (detail::is_base_of_template_v<U, std::enable_shared_from_this>)
        {
            return std::static_pointer_cast<U>(t->shared_from_this());
        }
        else
        {
            static_assert(detail::dependent_false<U>,
                "Failed reconstructing the reference count of the object instance, class must inherit from std::enable_shared_from_this");
        }
    }

    static T* get(const std::shared_ptr<T>& c)
    {
        return c.get();
    }
};

namespace detail {

template <class T>
class IsContainer
{
private:
    typedef char yes[1]; 
    typedef char no[2]; 

    template <class C>
    static constexpr no& test(typename C::IsNotContainer*);

    template <class>
    static constexpr yes& test(...);

public:
    static constexpr bool value = sizeof(test<ContainerTraits<T>>(nullptr)) == sizeof(yes);
};

} 
} 


// End File: Source/LuaBridge/detail/TypeTraits.h

// Begin File: Source/LuaBridge/detail/Userdata.h

namespace luabridge {
namespace detail {

class Userdata
{
private:
    
    static Userdata* getExactClass(lua_State* L, int index, const void* classKey)
    {
        return (void)classKey, static_cast<Userdata*>(lua_touserdata(L, lua_absindex(L, index)));
    }

    static Userdata* getClass(lua_State* L,
                              int index,
                              const void* registryConstKey,
                              const void* registryClassKey,
                              bool canBeConst)
    {
        const int result = lua_getmetatable(L, index); 
        if (result == 0 || !lua_istable(L, -1))
        {
            lua_rawgetp(L, LUA_REGISTRYINDEX, registryClassKey); 
            return throwBadArg(L, index);
        }

        lua_rawgetp(L, -1, getConstKey()); 
        LUABRIDGE_ASSERT(lua_istable(L, -1) || lua_isnil(L, -1));

        const bool isConst = lua_isnil(L, -1); 
        lua_rawgetp(L, LUA_REGISTRYINDEX, (isConst && canBeConst)
            ? registryConstKey
            : registryClassKey); 

        lua_insert(L, -3); 
        lua_pop(L, 1); 

        for (;;)
        {
            if (lua_rawequal(L, -1, -2)) 
            {
                lua_pop(L, 2); 
                return static_cast<Userdata*>(lua_touserdata(L, index));
            }

            lua_rawgetp(L, -1, getParentKey()); 

            if (lua_isnil(L, -1)) 
            {
                
                lua_pop(L, 2); 
                return throwBadArg(L, index);
            }

            lua_remove(L, -2); 
        }

    }

    static bool isInstance(lua_State* L, int index, const void* registryKey)
    {
        const auto result = lua_getmetatable(L, index); 
        if (result == 0)
            return false;

        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1); 
            return false;
        }

        lua_rawgetp(L, LUA_REGISTRYINDEX, registryKey); 
        lua_insert(L, -2); 

        for (;;)
        {
            if (lua_rawequal(L, -1, -2)) 
            {
                lua_pop(L, 2); 
                return true;
            }

            lua_rawgetp(L, -1, getParentKey()); 

            if (lua_isnil(L, -1)) 
            {
                
                lua_pop(L, 3); 
                return false;
            }

            lua_remove(L, -2); 
        }

    }

    static Userdata* throwBadArg(lua_State* L, int index)
    {
        LUABRIDGE_ASSERT(lua_istable(L, -1) || lua_isnil(L, -1)); 

        const char* expected = 0;
        if (lua_isnil(L, -1)) 
        {
            expected = "unregistered class";
        }
        else
        {
            lua_rawgetp(L, -1, getTypeKey()); 
            expected = lua_tostring(L, -1);
            lua_pop(L, 1); 
        }

        const char* got = nullptr;
        if (lua_isuserdata(L, index))
        {
            lua_getmetatable(L, index); 
            if (lua_istable(L, -1)) 
            {
                lua_rawgetp(L, -1, getTypeKey()); 
                if (lua_isstring(L, -1))
                    got = lua_tostring(L, -1);

                lua_pop(L, 1); 
            }

            lua_pop(L, 1); 
        }

        if (!got)
        {
            lua_pop(L, 1); 
            got = lua_typename(L, lua_type(L, index));
        }

        luaL_argerror(L, index, lua_pushfstring(L, "%s expected, got %s", expected, got));
        return nullptr;
    }

public:
    virtual ~Userdata() {}

    template <class T>
    static Userdata* getExact(lua_State* L, int index)
    {
        return getExactClass(L, index, detail::getClassRegistryKey<T>());
    }

    template <class T>
    static T* get(lua_State* L, int index, bool canBeConst)
    {
        if (lua_isnil(L, index))
            return nullptr;

        auto* clazz = getClass(L, index, detail::getConstRegistryKey<T>(), detail::getClassRegistryKey<T>(), canBeConst);
        if (! clazz)
            return nullptr;

        return static_cast<T*>(clazz->getPointer());
    }

    template <class T>
    static bool isInstance(lua_State* L, int index)
    {
        return isInstance(L, index, detail::getClassRegistryKey<T>())
            || isInstance(L, index, detail::getConstRegistryKey<T>());
    }

protected:
    Userdata() = default;

    void* getPointer() const noexcept
    {
        return m_p;
    }

    void* m_p = nullptr; 
};

template <class T>
class UserdataValue : public Userdata
{
    using AlignType = typename std::conditional_t<alignof(T) <= alignof(double), T, void*>;

    static constexpr int MaxPadding = alignof(T) <= alignof(AlignType) ? 0 : alignof(T) - alignof(AlignType) + 1;

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

    static UserdataValue<T>* place(lua_State* L, std::error_code& ec)
    {
        auto* ud = new (lua_newuserdata_x<UserdataValue<T>>(L, sizeof(UserdataValue<T>))) UserdataValue<T>();

        lua_rawgetp(L, LUA_REGISTRYINDEX, detail::getClassRegistryKey<T>());

        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1); 

            ud->~UserdataValue<T>();

#if LUABRIDGE_RAISE_UNREGISTERED_CLASS_USAGE
            ec = throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);
#else
            ec = makeErrorCode(ErrorCode::ClassNotRegistered);
#endif

            return nullptr;
        }

        lua_setmetatable(L, -2);

        return ud;
    }

    template <class U>
    static auto push(lua_State* L, const U& u) -> std::enable_if_t<std::is_copy_constructible_v<U>, Result>
    {
        std::error_code ec;
        auto* ud = place(L, ec);

        if (!ud)
            return ec;

        new (ud->getObject()) U(u);

        ud->commit();

        return {};
    }

    template <class U>
    static auto push(lua_State* L, U&& u) -> std::enable_if_t<std::is_move_constructible_v<U>, Result>
    {
        std::error_code ec;
        auto* ud = place(L, ec);

        if (!ud)
            return ec;

        new (ud->getObject()) U(std::move(u));

        ud->commit();

        return {};
    }

    void commit() noexcept
    {
        m_p = getObject();
    }

    T* getObject() noexcept
    {
        
        if constexpr (MaxPadding == 0)
            return reinterpret_cast<T*>(&m_storage[0]);
        else
            return reinterpret_cast<T*>(&m_storage[0] + m_storage[sizeof(m_storage) - 1]);
    }

private:
    
    UserdataValue() noexcept
        : Userdata()
    {
        if constexpr (MaxPadding > 0)
        {
            uintptr_t offset = reinterpret_cast<uintptr_t>(&m_storage[0]) % alignof(T);
            if (offset > 0)
                offset = alignof(T) - offset;

            assert(offset < MaxPadding);
            m_storage[sizeof(m_storage) - 1] = static_cast<unsigned char>(offset);
        }
    }

    alignas(AlignType) unsigned char m_storage[sizeof(T) + MaxPadding];
};

class UserdataPtr : public Userdata
{
public:
    UserdataPtr(const UserdataPtr&) = delete;
    UserdataPtr operator=(const UserdataPtr&) = delete;

    template <class T>
    static Result push(lua_State* L, T* ptr)
    {
        if (ptr)
            return push(L, ptr, getClassRegistryKey<T>());

        lua_pushnil(L);
        return {};
    }

    template <class T>
    static Result push(lua_State* L, const T* ptr)
    {
        if (ptr)
            return push(L, ptr, getConstRegistryKey<T>());

        lua_pushnil(L);
        return {};
    }

private:
    
    static Result push(lua_State* L, const void* ptr, const void* key)
    {
        auto* udptr = new (lua_newuserdata_x<UserdataPtr>(L, sizeof(UserdataPtr))) UserdataPtr(const_cast<void*>(ptr));

        lua_rawgetp(L, LUA_REGISTRYINDEX, key);

        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1); 

            udptr->~UserdataPtr();

#if LUABRIDGE_RAISE_UNREGISTERED_CLASS_USAGE
            return throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);
#else
            return makeErrorCode(ErrorCode::ClassNotRegistered);
#endif
        }

        lua_setmetatable(L, -2);

        return {};
    }

    explicit UserdataPtr(void* ptr)
    {
        
        LUABRIDGE_ASSERT(ptr != nullptr);
        m_p = ptr;
    }
};

template <class T>
class UserdataValueExternal : public Userdata
{
public:
    UserdataValueExternal(const UserdataValueExternal&) = delete;
    UserdataValueExternal operator=(const UserdataValueExternal&) = delete;

    ~UserdataValueExternal()
    {
        if (getObject() != nullptr)
            m_dealloc(getObject());
    }

    template <class Dealloc>
    static UserdataValueExternal<T>* place(lua_State* L, T* obj, Dealloc dealloc, std::error_code& ec)
    {
        auto* ud = new (lua_newuserdata_x<UserdataValueExternal<T>>(L, sizeof(UserdataValueExternal<T>))) UserdataValueExternal<T>(obj, dealloc);

        lua_rawgetp(L, LUA_REGISTRYINDEX, detail::getClassRegistryKey<T>());

        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1); 

            ud->~UserdataValueExternal<T>();

#if LUABRIDGE_RAISE_UNREGISTERED_CLASS_USAGE
            ec = throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);
#else
            ec = makeErrorCode(ErrorCode::ClassNotRegistered);
#endif

            return nullptr;
        }

        lua_setmetatable(L, -2);

        return ud;
    }

    T* getObject() noexcept
    {
        return static_cast<T*>(m_p);
    }

private:
    UserdataValueExternal(void* ptr, void (*dealloc)(T*)) noexcept
    {
        
        LUABRIDGE_ASSERT(ptr != nullptr);
        m_p = ptr;

        LUABRIDGE_ASSERT(dealloc != nullptr);
        m_dealloc = dealloc;
    }

    void (*m_dealloc)(T*) = nullptr;
};

template <class C>
class UserdataShared : public Userdata
{
public:
    UserdataShared(const UserdataShared&) = delete;
    UserdataShared& operator=(const UserdataShared&) = delete;

    ~UserdataShared() = default;

    template <class U>
    explicit UserdataShared(const U& u) : m_c(u)
    {
        m_p = const_cast<void*>(reinterpret_cast<const void*>((ContainerTraits<C>::get(m_c))));
    }

    template <class U>
    explicit UserdataShared(U* u) : m_c(u)
    {
        m_p = const_cast<void*>(reinterpret_cast<const void*>((ContainerTraits<C>::get(m_c))));
    }

private:
    C m_c;
};

template <class C, bool MakeObjectConst>
struct UserdataSharedHelper
{
    using T = std::remove_const_t<typename ContainerTraits<C>::Type>;

    static Result push(lua_State* L, const C& c)
    {
        if (ContainerTraits<C>::get(c) != nullptr)
        {
            auto* us = new (lua_newuserdata_x<UserdataShared<C>>(L, sizeof(UserdataShared<C>))) UserdataShared<C>(c);

            lua_rawgetp(L, LUA_REGISTRYINDEX, getClassRegistryKey<T>());

            if (!lua_istable(L, -1))
            {
                lua_pop(L, 1); 

                us->~UserdataShared<C>();

#if LUABRIDGE_RAISE_UNREGISTERED_CLASS_USAGE
                return throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);
#else
                return makeErrorCode(ErrorCode::ClassNotRegistered);
#endif
            }

            lua_setmetatable(L, -2);
        }
        else
        {
            lua_pushnil(L);
        }

        return {};
    }

    static Result push(lua_State* L, T* t)
    {
        if (t)
        {
            auto* us = new (lua_newuserdata_x<UserdataShared<C>>(L, sizeof(UserdataShared<C>))) UserdataShared<C>(t);

            lua_rawgetp(L, LUA_REGISTRYINDEX, getClassRegistryKey<T>());

            if (!lua_istable(L, -1))
            {
                lua_pop(L, 1); 

                us->~UserdataShared<C>();

#if LUABRIDGE_RAISE_UNREGISTERED_CLASS_USAGE
                return throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);
#else
                return makeErrorCode(ErrorCode::ClassNotRegistered);
#endif
            }

            lua_setmetatable(L, -2);
        }
        else
        {
            lua_pushnil(L);
        }

        return {};
    }
};

template <class C>
struct UserdataSharedHelper<C, true>
{
    using T = std::remove_const_t<typename ContainerTraits<C>::Type>;

    static Result push(lua_State* L, const C& c)
    {
        if (ContainerTraits<C>::get(c) != nullptr)
        {
            auto* us = new (lua_newuserdata_x<UserdataShared<C>>(L, sizeof(UserdataShared<C>))) UserdataShared<C>(c);

            lua_rawgetp(L, LUA_REGISTRYINDEX, getConstRegistryKey<T>());

            if (!lua_istable(L, -1))
            {
                lua_pop(L, 1); 

                us->~UserdataShared<C>();

#if LUABRIDGE_RAISE_UNREGISTERED_CLASS_USAGE
                return throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);
#else
                return makeErrorCode(ErrorCode::ClassNotRegistered);
#endif
            }

            lua_setmetatable(L, -2);
        }
        else
        {
            lua_pushnil(L);
        }

        return {};
    }

    static Result push(lua_State* L, T* t)
    {
        if (t)
        {
            auto* us = new (lua_newuserdata_x<UserdataShared<C>>(L, sizeof(UserdataShared<C>))) UserdataShared<C>(t);

            lua_rawgetp(L, LUA_REGISTRYINDEX, getConstRegistryKey<T>());

            if (!lua_istable(L, -1))
            {
                lua_pop(L, 1); 

                us->~UserdataShared<C>();

#if LUABRIDGE_RAISE_UNREGISTERED_CLASS_USAGE
                return throw_or_error_code<LuaException>(L, ErrorCode::ClassNotRegistered);
#else
                return makeErrorCode(ErrorCode::ClassNotRegistered);
#endif
            }

            lua_setmetatable(L, -2);
        }
        else
        {
            lua_pushnil(L);
        }

        return {};
    }
};

template <class T, bool ByContainer>
struct StackHelper
{
    using ReturnType = TypeResult<T>;

    static Result push(lua_State* L, const T& t)
    {
        return UserdataSharedHelper<T, std::is_const_v<typename ContainerTraits<T>::Type>>::push(L, t);
    }

    static ReturnType get(lua_State* L, int index)
    {
        using CastType = std::remove_const_t<typename ContainerTraits<T>::Type>;

        auto* result = Userdata::get<CastType>(L, index, true);
        if (! result)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        return ContainerTraits<T>::construct(result);
    }
};

template <class T>
struct StackHelper<T, false>
{
    static Result push(lua_State* L, const T& t)
    {
        return UserdataValue<T>::push(L, t);
    }

    static Result push(lua_State* L, T&& t)
    {
        return UserdataValue<T>::push(L, std::move(t));
    }

    static TypeResult<std::reference_wrapper<const T>> get(lua_State* L, int index)
    {
        auto* result = Userdata::get<T>(L, index, true);
        if (! result)
            return makeErrorCode(ErrorCode::InvalidTypeCast); 

        return std::cref(*result);
    }
};

template <class C, bool ByContainer>
struct RefStackHelper
{
    using ReturnType = TypeResult<C>;
    using T = std::remove_const_t<typename ContainerTraits<C>::Type>;

    static Result push(lua_State* L, const C& t)
    {
        return UserdataSharedHelper<C, std::is_const_v<typename ContainerTraits<C>::Type>>::push(L, t);
    }

    static ReturnType get(lua_State* L, int index)
    {
        auto* result = Userdata::get<T>(L, index, true);
        if (! result)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        return ContainerTraits<C>::construct(result);
    }
};

template <class T>
struct RefStackHelper<T, false>
{
    using ReturnType = TypeResult<std::reference_wrapper<T>>;

    static Result push(lua_State* L, T& t)
    {
        return UserdataPtr::push(L, std::addressof(t));
    }

    static Result push(lua_State* L, const T& t)
    {
        return UserdataPtr::push(L, std::addressof(t));
    }

    static ReturnType get(lua_State* L, int index)
    {
        auto* result = Userdata::get<T>(L, index, true);
        if (! result)
            return makeErrorCode(ErrorCode::InvalidTypeCast); 

        return std::ref(*result);
    }
};

template <class T, class Enable = void>
struct UserdataGetter
{
    using ReturnType = TypeResult<T*>;

    static ReturnType get(lua_State* L, int index)
    {
        auto* result = Userdata::get<T>(L, index, true);
        if (! result)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        return result;
    }
};

template <class T>
struct UserdataGetter<T, std::void_t<T (*)()>>
{
    using ReturnType = TypeResult<T>;

    static ReturnType get(lua_State* L, int index)
    {
        auto result = StackHelper<T, IsContainer<T>::value>::get(L, index);
        if (! result)
            return result.error();

        return *result;
    }
};

} 

template <class T, class = void>
struct Stack
{
    using IsUserdata = void;

    using Getter = detail::UserdataGetter<T>;
    using ReturnType = typename Getter::ReturnType;

    [[nodiscard]] static Result push(lua_State* L, const T& value)
    {
        return detail::StackHelper<T, detail::IsContainer<T>::value>::push(L, value);
    }

    [[nodiscard]] static Result push(lua_State* L, T&& value)
    {
        return detail::StackHelper<T, detail::IsContainer<T>::value>::push(L, std::move(value));
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

template <class T, class Enable = void>
struct IsUserdata : std::false_type
{
};

template <class T>
struct IsUserdata<T, std::void_t<typename Stack<T>::IsUserdata>> : std::true_type
{
};

template <class T, bool IsUserdata>
struct StackOpSelector;

template <class T>
struct StackOpSelector<T*, true>
{
    using ReturnType = TypeResult<T*>;

    static Result push(lua_State* L, T* value) { return UserdataPtr::push(L, value); }

    static ReturnType get(lua_State* L, int index) { return Userdata::get<T>(L, index, false); }

    template <class U = T>
    static bool isInstance(lua_State* L, int index) { return Userdata::isInstance<U>(L, index); }
};

template <class T>
struct StackOpSelector<const T*, true>
{
    using ReturnType = TypeResult<const T*>;

    static Result push(lua_State* L, const T* value) { return UserdataPtr::push(L, value); }

    static ReturnType get(lua_State* L, int index) { return Userdata::get<T>(L, index, true); }

    template <class U = T>
    static bool isInstance(lua_State* L, int index) { return Userdata::isInstance<U>(L, index); }
};

template <class T>
struct StackOpSelector<T&, true>
{
    using Helper = RefStackHelper<T, IsContainer<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    static Result push(lua_State* L, T& value) { return Helper::push(L, value); }

    static ReturnType get(lua_State* L, int index) { return Helper::get(L, index); }

    template <class U = T>
    static bool isInstance(lua_State* L, int index) { return Userdata::isInstance<U>(L, index); }
};

template <class T>
struct StackOpSelector<const T&, true>
{
    using Helper = RefStackHelper<T, IsContainer<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    static Result push(lua_State* L, const T& value) { return Helper::push(L, value); }

    static ReturnType get(lua_State* L, int index) { return Helper::get(L, index); }

    template <class U = T>
    static bool isInstance(lua_State* L, int index) { return Userdata::isInstance<U>(L, index); }
};

} 
} 


// End File: Source/LuaBridge/detail/Userdata.h

// Begin File: Source/LuaBridge/detail/Stack.h

namespace luabridge {

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
        if (m_doRestoreStack)
            lua_settop(m_L, m_stackTop);
    }

    void reset()
    {
        m_doRestoreStack = false;
    }

private:
    lua_State* const m_L = nullptr;
    int m_stackTop = 0;
    bool m_doRestoreStack = true;
};

template <class T, class>
struct Stack;

template <>
struct Stack<void>
{
    [[nodiscard]] static Result push(lua_State*)
    {
        return {};
    }
};

template <>
struct Stack<std::nullptr_t>
{
    [[nodiscard]] static Result push(lua_State* L, std::nullptr_t)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushnil(L);
        return {};
    }

    [[nodiscard]] static TypeResult<std::nullptr_t> get(lua_State* L, int index)
    {
        if (! lua_isnil(L, index))
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        return nullptr;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_isnil(L, index);
    }
};

template <>
struct Stack<lua_State*>
{
    [[nodiscard]] static TypeResult<lua_State*> get(lua_State* L, int)
    {
        return L;
    }
};

template <>
struct Stack<lua_CFunction>
{
    [[nodiscard]] static Result push(lua_State* L, lua_CFunction f)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushcfunction_x(L, f, "");
        return {};
    }

    [[nodiscard]] static TypeResult<lua_CFunction> get(lua_State* L, int index)
    {
        if (! lua_iscfunction(L, index))
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        return lua_tocfunction(L, index);
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_iscfunction(L, index) != 0;
    }
};

template <>
struct Stack<bool>
{
    [[nodiscard]] static Result push(lua_State* L, bool value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushboolean(L, value ? 1 : 0);
        return {};
    }

    [[nodiscard]] static TypeResult<bool> get(lua_State* L, int index)
    {
        return lua_toboolean(L, index) ? true : false;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_isboolean(L, index);
    }
};

template <>
struct Stack<std::byte>
{
    static_assert(sizeof(std::byte) < sizeof(lua_Integer));

    [[nodiscard]] static Result push(lua_State* L, std::byte value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        pushunsigned(L, std::to_integer<std::make_unsigned_t<lua_Integer>>(value));
        return {};
    }

    [[nodiscard]] static TypeResult<std::byte> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<unsigned char>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<std::byte>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned char>(L, index);

        return false;
    }
};

template <>
struct Stack<char>
{
    [[nodiscard]] static Result push(lua_State* L, char value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushlstring(L, &value, 1);
        return {};
    }

    [[nodiscard]] static TypeResult<char> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TSTRING)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        std::size_t length = 0;
        const char* str = lua_tolstring(L, index, &length);

        if (str == nullptr || length != 1)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        return str[0];
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

#if !defined(__BORLANDC__)
template <>
struct Stack<int8_t>
{
    static_assert(sizeof(int8_t) < sizeof(lua_Integer));

    [[nodiscard]] static Result push(lua_State* L, int8_t value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return {};
    }

    [[nodiscard]] static TypeResult<int8_t> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<int8_t>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<int8_t>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<int8_t>(L, index);

        return false;
    }
};
#endif

template <>
struct Stack<unsigned char>
{
    static_assert(sizeof(unsigned char) < sizeof(lua_Integer));

    [[nodiscard]] static Result push(lua_State* L, unsigned char value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        pushunsigned(L, value);
        return {};
    }

    [[nodiscard]] static TypeResult<unsigned char> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<unsigned char>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<unsigned char>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned char>(L, index);

        return false;
    }
};

template <>
struct Stack<short>
{
    static_assert(sizeof(short) < sizeof(lua_Integer));

    [[nodiscard]] static Result push(lua_State* L, short value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return {};
    }

    [[nodiscard]] static TypeResult<short> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<short>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<short>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<short>(L, index);

        return false;
    }
};

template <>
struct Stack<unsigned short>
{
    static_assert(sizeof(unsigned short) < sizeof(lua_Integer));

    [[nodiscard]] static Result push(lua_State* L, unsigned short value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        pushunsigned(L, value);
        return {};
    }

    [[nodiscard]] static TypeResult<unsigned short> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<unsigned short>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<unsigned short>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned short>(L, index);

        return false;
    }
};

template <>
struct Stack<int>
{
    [[nodiscard]] static Result push(lua_State* L, int value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (! is_integral_representable_by(value))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return {};
    }

    [[nodiscard]] static TypeResult<int> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<int>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<int>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<int>(L, index);

        return false;
    }
};

template <>
struct Stack<unsigned int>
{
    [[nodiscard]] static Result push(lua_State* L, unsigned int value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (! is_integral_representable_by(value))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        pushunsigned(L, value);
        return {};
    }

    [[nodiscard]] static TypeResult<unsigned int> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<unsigned int>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<unsigned int>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned int>(L, index);

        return false;
    }
};

template <>
struct Stack<long>
{
    [[nodiscard]] static Result push(lua_State* L, long value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (! is_integral_representable_by(value))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return {};
    }

    [[nodiscard]] static TypeResult<long> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<long>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<long>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<long>(L, index);

        return false;
    }
};

template <>
struct Stack<unsigned long>
{
    [[nodiscard]] static Result push(lua_State* L, unsigned long value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (! is_integral_representable_by(value))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        pushunsigned(L, value);
        return {};
    }

    [[nodiscard]] static TypeResult<unsigned long> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<unsigned long>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<unsigned long>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned long>(L, index);

        return false;
    }
};

template <>
struct Stack<long long>
{
    [[nodiscard]] static Result push(lua_State* L, long long value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (! is_integral_representable_by(value))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return {};
    }

    [[nodiscard]] static TypeResult<long long> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<long long>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<long long>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<long long>(L, index);

        return false;
    }
};

template <>
struct Stack<unsigned long long>
{
    [[nodiscard]] static Result push(lua_State* L, unsigned long long value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (! is_integral_representable_by(value))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        pushunsigned(L, value);
        return {};
    }

    [[nodiscard]] static TypeResult<unsigned long long> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<unsigned long long>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<unsigned long long>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<unsigned long long>(L, index);

        return false;
    }
};

#if 0 

template <>
struct Stack<__int128_t>
{
    [[nodiscard]] static Result push(lua_State* L, __int128_t value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (! is_integral_representable_by(value))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return {};
    }

    [[nodiscard]] static TypeResult<__int128_t> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<__int128_t>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<__int128_t>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<__int128_t>(L, index);

        return false;
    }
};

template <>
struct Stack<__uint128_t>
{
    [[nodiscard]] static Result push(lua_State* L, __uint128_t value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (! is_integral_representable_by(value))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        lua_pushinteger(L, static_cast<lua_Integer>(value));
        return {};
    }

    [[nodiscard]] static TypeResult<__uint128_t> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_integral_representable_by<__uint128_t>(L, index))
            return makeErrorCode(ErrorCode::IntegerDoesntFitIntoLuaInteger);

        return static_cast<__uint128_t>(lua_tointeger(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_integral_representable_by<__uint128_t>(L, index);

        return false;
    }
};
#endif

template <>
struct Stack<float>
{
    [[nodiscard]] static Result push(lua_State* L, float value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (! is_floating_point_representable_by(value))
            return makeErrorCode(ErrorCode::FloatingPointDoesntFitIntoLuaNumber);

        lua_pushnumber(L, static_cast<lua_Number>(value));
        return {};
    }

    [[nodiscard]] static TypeResult<float> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_floating_point_representable_by<float>(L, index))
            return makeErrorCode(ErrorCode::FloatingPointDoesntFitIntoLuaNumber);

        return static_cast<float>(lua_tonumber(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_floating_point_representable_by<float>(L, index);

        return false;
    }
};

template <>
struct Stack<double>
{
    [[nodiscard]] static Result push(lua_State* L, double value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (! is_floating_point_representable_by(value))
            return makeErrorCode(ErrorCode::FloatingPointDoesntFitIntoLuaNumber);

        lua_pushnumber(L, static_cast<lua_Number>(value));
        return {};
    }

    [[nodiscard]] static TypeResult<double> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_floating_point_representable_by<double>(L, index))
            return makeErrorCode(ErrorCode::FloatingPointDoesntFitIntoLuaNumber);

        return static_cast<double>(lua_tonumber(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_floating_point_representable_by<double>(L, index);

        return false;
    }
};

template <>
struct Stack<long double>
{
    [[nodiscard]] static Result push(lua_State* L, long double value)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (! is_floating_point_representable_by(value))
            return makeErrorCode(ErrorCode::FloatingPointDoesntFitIntoLuaNumber);

        lua_pushnumber(L, static_cast<lua_Number>(value));
        return {};
    }

    [[nodiscard]] static TypeResult<long double> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TNUMBER)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (! is_floating_point_representable_by<long double>(L, index))
            return makeErrorCode(ErrorCode::FloatingPointDoesntFitIntoLuaNumber);

        return static_cast<long double>(lua_tonumber(L, index));
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        if (lua_type(L, index) == LUA_TNUMBER)
            return is_floating_point_representable_by<long double>(L, index);

        return false;
    }
};

template <>
struct Stack<const char*>
{
    [[nodiscard]] static Result push(lua_State* L, const char* str)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        if (str != nullptr)
            lua_pushstring(L, str);
        else
            lua_pushnil(L);

        return {};
    }

    [[nodiscard]] static TypeResult<const char*> get(lua_State* L, int index)
    {
        const bool isNil = lua_isnil(L, index);

        if (!isNil && lua_type(L, index) != LUA_TSTRING)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (isNil)
            return nullptr;

        std::size_t length = 0;
        const char* str = lua_tolstring(L, index, &length);
        if (str == nullptr)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        return str;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_isnil(L, index) || lua_type(L, index) == LUA_TSTRING;
    }
};

template <>
struct Stack<std::string_view>
{
    [[nodiscard]] static Result push(lua_State* L, std::string_view str)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushlstring(L, str.data(), str.size());
        return {};
    }

    [[nodiscard]] static TypeResult<std::string_view> get(lua_State* L, int index)
    {
        if (lua_type(L, index) != LUA_TSTRING)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        std::size_t length = 0;
        const char* str = lua_tolstring(L, index, &length);
        if (str == nullptr)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        return std::string_view{ str, length };
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TSTRING;
    }
};

template <>
struct Stack<std::string>
{
    [[nodiscard]] static Result push(lua_State* L, const std::string& str)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushlstring(L, str.data(), str.size());
        return {};
    }

    [[nodiscard]] static TypeResult<std::string> get(lua_State* L, int index)
    {
        std::size_t length = 0;
        const char* str = nullptr;

        if (lua_type(L, index) == LUA_TSTRING)
        {
            str = lua_tolstring(L, index, &length);
        }
        else
        {
#if LUABRIDGE_SAFE_STACK_CHECKS
            if (! lua_checkstack(L, 1))
                return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

            lua_pushvalue(L, index);
            str = lua_tolstring(L, -1, &length);
            lua_pop(L, 1);
        }

        if (str == nullptr)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        return std::string{ str, length };
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TSTRING;
    }
};

template <class T>
struct Stack<std::optional<T>>
{
    using Type = std::optional<T>;

    [[nodiscard]] static Result push(lua_State* L, const Type& value)
    {
        if (value)
        {
            StackRestore stackRestore(L);

            auto result = Stack<T>::push(L, *value);
            if (! result)
                return result;

            stackRestore.reset();
            return {};
        }

#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushnil(L);
        return {};
    }

    [[nodiscard]] static TypeResult<Type> get(lua_State* L, int index)
    {
        const auto type = lua_type(L, index);
        if (type == LUA_TNIL || type == LUA_TNONE)
            return std::nullopt;

        auto result = Stack<T>::get(L, index);
        if (! result)
            return result.error();

        return *result;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        const auto type = lua_type(L, index);
        return (type == LUA_TNIL || type == LUA_TNONE) || Stack<T>::isInstance(L, index);
    }
};

template <class T1, class T2>
struct Stack<std::pair<T1, T2>>
{
    [[nodiscard]] static Result push(lua_State* L, const std::pair<T1, T2>& t)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        StackRestore stackRestore(L);

        lua_createtable(L, 2, 0);

        auto result1 = push_element<0>(L, t);
        if (! result1)
            return result1;

        auto result2 = push_element<1>(L, t);
        if (! result2)
            return result2;

        stackRestore.reset();
        return {};
    }

    [[nodiscard]] static TypeResult<std::pair<T1, T2>> get(lua_State* L, int index)
    {
        const StackRestore stackRestore(L);

        if (!lua_istable(L, index))
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (get_length(L, index) != 2)
            return makeErrorCode(ErrorCode::InvalidTableSizeInCast);

        std::pair<T1, T2> value;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        auto result1 = pop_element<0>(L, absIndex, value);
        if (! result1)
            return result1.error();

        auto result2 = pop_element<1>(L, absIndex, value);
        if (! result2)
            return result2.error();

        return value;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TTABLE && get_length(L, index) == 2;
    }

private:
    template <std::size_t Index>
    static Result push_element(lua_State* L, const std::pair<T1, T2>& p)
    {
        static_assert(Index < 2);

        using T = std::tuple_element_t<Index, std::pair<T1, T2>>;

        lua_pushinteger(L, static_cast<lua_Integer>(Index + 1));

        auto result = Stack<T>::push(L, std::get<Index>(p));
        if (! result)
            return result;

        lua_settable(L, -3);

        return {};
    }

    template <std::size_t Index>
    static Result pop_element(lua_State* L, int absIndex, std::pair<T1, T2>& p)
    {
        static_assert(Index < 2);

        using T = std::tuple_element_t<Index, std::pair<T1, T2>>;

        if (lua_next(L, absIndex) == 0)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        auto result = Stack<T>::get(L, -1);
        if (! result)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        std::get<Index>(p) = std::move(*result);
        lua_pop(L, 1);

        return {};
    }
};

template <class... Types>
struct Stack<std::tuple<Types...>>
{
    [[nodiscard]] static Result push(lua_State* L, const std::tuple<Types...>& t)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        StackRestore stackRestore(L);

        lua_createtable(L, static_cast<int>(Size), 0);

        auto result = push_element(L, t);
        if (! result)
            return result;

        stackRestore.reset();
        return {};
    }

    [[nodiscard]] static TypeResult<std::tuple<Types...>> get(lua_State* L, int index)
    {
        const StackRestore stackRestore(L);

        if (!lua_istable(L, index))
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (get_length(L, index) != static_cast<int>(Size))
            return makeErrorCode(ErrorCode::InvalidTableSizeInCast);

        std::tuple<Types...> value;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        auto result = pop_element(L, absIndex, value);
        if (! result)
            return result.error();

        return value;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TTABLE && get_length(L, index) == static_cast<int>(Size);
    }

private:
    static constexpr std::size_t Size = std::tuple_size_v<std::tuple<Types...>>;

    template <std::size_t Index = 0>
    static auto push_element(lua_State*, const std::tuple<Types...>&)
        -> std::enable_if_t<Index == sizeof...(Types), Result>
    {
        return {};
    }

    template <std::size_t Index = 0>
    static auto push_element(lua_State* L, const std::tuple<Types...>& t)
        -> std::enable_if_t<Index < sizeof...(Types), Result>
    {
        using T = std::tuple_element_t<Index, std::tuple<Types...>>;

        lua_pushinteger(L, static_cast<lua_Integer>(Index + 1));

        auto result = Stack<T>::push(L, std::get<Index>(t));
        if (! result)
            return result;

        lua_settable(L, -3);

        return push_element<Index + 1>(L, t);
    }

    template <std::size_t Index = 0>
    static auto pop_element(lua_State*, int, std::tuple<Types...>&)
        -> std::enable_if_t<Index == sizeof...(Types), Result>
    {
        return {};
    }

    template <std::size_t Index = 0>
    static auto pop_element(lua_State* L, int absIndex, std::tuple<Types...>& t)
        -> std::enable_if_t<Index < sizeof...(Types), Result>
    {
        using T = std::tuple_element_t<Index, std::tuple<Types...>>;

        if (lua_next(L, absIndex) == 0)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        auto result = Stack<T>::get(L, -1);
        if (! result)
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        std::get<Index>(t) = std::move(*result);
        lua_pop(L, 1);

        return pop_element<Index + 1>(L, absIndex, t);
    }
};

template <class T, std::size_t N>
struct Stack<T[N]>
{
    static_assert(N > 0, "Unsupported zero sized array");

    [[nodiscard]] static Result push(lua_State* L, const T (&value)[N])
    {
        if constexpr (std::is_same_v<T, char>)
        {
#if LUABRIDGE_SAFE_STACK_CHECKS
            if (! lua_checkstack(L, 1))
                return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

            lua_pushlstring(L, value, N - 1);
            return {};
        }

#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 2))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        StackRestore stackRestore(L);

        lua_createtable(L, static_cast<int>(N), 0);

        for (std::size_t i = 0; i < N; ++i)
        {
            lua_pushinteger(L, static_cast<lua_Integer>(i + 1));

            auto result = Stack<T>::push(L, value[i]);
            if (! result)
                return result;

            lua_settable(L, -3);
        }

        stackRestore.reset();
        return {};
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TTABLE && get_length(L, index) == static_cast<int>(N);
    }
};

template <>
struct Stack<void*>
{
    [[nodiscard]] static Result push(lua_State* L, void* ptr)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushlightuserdata(L, ptr);
        return {};
    }

    [[nodiscard]] static TypeResult<void*> get(lua_State* L, int index)
    {
        if (lua_isnil(L, index))
            return nullptr;

        if (lua_islightuserdata(L, index))
            return lua_touserdata(L, index);

        return makeErrorCode(ErrorCode::InvalidTypeCast);
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_islightuserdata(L, index) || lua_isnil(L, index);
    }
};

template <>
struct Stack<const void*>
{
    [[nodiscard]] static Result push(lua_State* L, const void* ptr)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushlightuserdata(L, const_cast<void*>(ptr));
        return {};
    }

    [[nodiscard]] static TypeResult<const void*> get(lua_State* L, int index)
    {
        if (lua_isnil(L, index))
            return nullptr;

        if (lua_islightuserdata(L, index))
            return lua_touserdata(L, index);

        return makeErrorCode(ErrorCode::InvalidTypeCast);
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_islightuserdata(L, index) || lua_isnil(L, index);
    }
};

namespace detail {

template <class T>
struct StackOpSelector<T&, false>
{
    using ReturnType = TypeResult<T>;

    static Result push(lua_State* L, T& value) { return Stack<T>::push(L, value); }

    static ReturnType get(lua_State* L, int index) { return Stack<T>::get(L, index); }

    static bool isInstance(lua_State* L, int index) { return Stack<T>::isInstance(L, index); }
};

template <class T>
struct StackOpSelector<const T&, false>
{
    using ReturnType = TypeResult<T>;

    static Result push(lua_State* L, const T& value) { return Stack<T>::push(L, value); }

    static ReturnType get(lua_State* L, int index) { return Stack<T>::get(L, index); }

    static bool isInstance(lua_State* L, int index) { return Stack<T>::isInstance(L, index); }
};

template <class T>
struct StackOpSelector<T*, false>
{
    using ReturnType = TypeResult<T>;

    static Result push(lua_State* L, T* value)
    {
        return value ? Stack<T>::push(L, *value) : Stack<std::nullptr_t>::push(L, nullptr);
    }

    static ReturnType get(lua_State* L, int index) { return Stack<T>::get(L, index); }

    static bool isInstance(lua_State* L, int index) { return Stack<T>::isInstance(L, index); }
};

template <class T>
struct StackOpSelector<const T*, false>
{
    using ReturnType = TypeResult<T>;

    static Result push(lua_State* L, const T* value)
    {
        return value ? Stack<T>::push(L, *value) : Stack<std::nullptr_t>::push(L, nullptr);
    }

    static ReturnType get(lua_State* L, int index) { return Stack<T>::get(L, index); }

    static bool isInstance(lua_State* L, int index) { return Stack<T>::isInstance(L, index); }
};

} 

template <class T>
struct Stack<T&, std::enable_if_t<!std::is_array_v<T&>>>
{
    using Helper = detail::StackOpSelector<T&, detail::IsUserdata<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    [[nodiscard]] static Result push(lua_State* L, T& value) { return Helper::push(L, value); }

    [[nodiscard]] static ReturnType get(lua_State* L, int index) { return Helper::get(L, index); }

    [[nodiscard]] static bool isInstance(lua_State* L, int index) { return Helper::template isInstance<T>(L, index); }
};

template <class T>
struct Stack<const T&, std::enable_if_t<!std::is_array_v<const T&>>>
{
    using Helper = detail::StackOpSelector<const T&, detail::IsUserdata<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    [[nodiscard]] static Result push(lua_State* L, const T& value) { return Helper::push(L, value); }

    [[nodiscard]] static ReturnType get(lua_State* L, int index) { return Helper::get(L, index); }

    [[nodiscard]] static bool isInstance(lua_State* L, int index) { return Helper::template isInstance<T>(L, index); }
};

template <class T>
struct Stack<T*>
{
    using Helper = detail::StackOpSelector<T*, detail::IsUserdata<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    [[nodiscard]] static Result push(lua_State* L, T* value) { return Helper::push(L, value); }

    [[nodiscard]] static ReturnType get(lua_State* L, int index) { return Helper::get(L, index); }

    [[nodiscard]] static bool isInstance(lua_State* L, int index) { return Helper::template isInstance<T>(L, index); }
};

template<class T>
struct Stack<const T*>
{
    using Helper = detail::StackOpSelector<const T*, detail::IsUserdata<T>::value>;
    using ReturnType = typename Helper::ReturnType;

    [[nodiscard]] static Result push(lua_State* L, const T* value) { return Helper::push(L, value); }

    [[nodiscard]] static ReturnType get(lua_State* L, int index) { return Helper::get(L, index); }

    [[nodiscard]] static bool isInstance(lua_State* L, int index) { return Helper::template isInstance<T>(L, index); }
};

template <class T>
[[nodiscard]] Result push(lua_State* L, const T& t)
{
    return Stack<T>::push(L, t);
}

template <class T>
[[nodiscard]] TypeResult<T> get(lua_State* L, int index)
{
    return Stack<T>::get(L, index);
}

template <class T>
[[nodiscard]] bool isInstance(lua_State* L, int index)
{
    return Stack<T>::isInstance(L, index);
}

} 


// End File: Source/LuaBridge/detail/Stack.h

// Begin File: Source/LuaBridge/Array.h

namespace luabridge {

template <class T, std::size_t Size>
struct Stack<std::array<T, Size>>
{
    using Type = std::array<T, Size>;

    [[nodiscard]] static Result push(lua_State* L, const Type& array)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        StackRestore stackRestore(L);

        lua_createtable(L, static_cast<int>(Size), 0);

        for (std::size_t i = 0; i < Size; ++i)
        {
            lua_pushinteger(L, static_cast<lua_Integer>(i + 1));

            auto result = Stack<T>::push(L, array[i]);
            if (! result)
                return result;

            lua_settable(L, -3);
        }
        
        stackRestore.reset();
        return {};
    }

    [[nodiscard]] static TypeResult<Type> get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        if (get_length(L, index) != Size)
            return makeErrorCode(ErrorCode::InvalidTableSizeInCast);

        const StackRestore stackRestore(L);

        Type array;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        int arrayIndex = 0;
        while (lua_next(L, absIndex) != 0)
        {
            auto item = Stack<T>::get(L, -1);
            if (!item)
                return makeErrorCode(ErrorCode::InvalidTypeCast);

            array[arrayIndex++] = *item;
            lua_pop(L, 1);
        }

        return array;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index) && get_length(L, index) == Size;
    }
};

} 


// End File: Source/LuaBridge/Array.h

// Begin File: Source/LuaBridge/Dump.h

namespace luabridge {
namespace detail {
inline void putIndent(std::ostream& stream, unsigned level)
{
    for (unsigned i = 0; i < level; ++i)
        stream << "    ";
}
} 

inline void dumpTable(lua_State* L, int index, unsigned maxDepth = 1, unsigned level = 0, bool newLine = true, std::ostream& stream = std::cerr);

inline void dumpValue(lua_State* L, int index, unsigned maxDepth = 1, unsigned level = 0, bool newLine = true, std::ostream& stream = std::cerr)
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
            stream << "cfunction@" << lua_topointer(L, index);
        else
            stream << "function@" << lua_topointer(L, index);
        break;

    case LUA_TTHREAD:
        stream << "thread@" << lua_tothread(L, index);
        break;

    case LUA_TLIGHTUSERDATA:
        stream << "lightuserdata@" << lua_touserdata(L, index);
        break;

    case LUA_TTABLE:
        dumpTable(L, index, maxDepth, level, false, stream);
        break;

    case LUA_TUSERDATA:
        stream << "userdata@" << lua_touserdata(L, index);
        break;

    default:
        stream << lua_typename(L, type);
        break;
    }

    if (newLine)
        stream << '\n';
}

inline void dumpTable(lua_State* L, int index, unsigned maxDepth, unsigned level, bool newLine, std::ostream& stream)
{
    stream << "table@" << lua_topointer(L, index);

    if (level > maxDepth)
    {
        if (newLine)
            stream << '\n';

        return;
    }

    index = lua_absindex(L, index);

    stream << " {";

    int valuesCount = 0;

    lua_pushnil(L); 
    while (lua_next(L, index))
    {
        stream << '\n';
        detail::putIndent(stream, level + 1);

        dumpValue(L, -2, maxDepth, level + 1, false, stream); 
        stream << ": ";
        dumpValue(L, -1, maxDepth, level + 1, false, stream); 
        stream << ",";

        lua_pop(L, 1); 

        ++valuesCount;
    }

    if (valuesCount > 0)
    {
        stream << '\n';
        detail::putIndent(stream, level);
    }

    stream << "}";

    if (newLine)
        stream << '\n';
}

inline void dumpState(lua_State* L, unsigned maxDepth = 1, std::ostream& stream = std::cerr)
{
    stream << "----------------------------------------------" << '\n';

    int top = lua_gettop(L);
    for (int i = 1; i <= top; ++i)
    {
        stream << "stack #" << i << " (" << -(top - i + 1) << "): ";

        dumpValue(L, i, maxDepth, 0, true, stream);
    }
}

} 


// End File: Source/LuaBridge/Dump.h

// Begin File: Source/LuaBridge/List.h

namespace luabridge {

template <class T>
struct Stack<std::list<T>>
{
    using Type = std::list<T>;
    
    [[nodiscard]] static Result push(lua_State* L, const Type& list)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        StackRestore stackRestore(L);

        lua_createtable(L, static_cast<int>(list.size()), 0);

        auto it = list.cbegin();
        for (lua_Integer tableIndex = 1; it != list.cend(); ++tableIndex, ++it)
        {
            lua_pushinteger(L, tableIndex);

            auto result = Stack<T>::push(L, *it);
            if (! result)
                return result;

            lua_settable(L, -3);
        }

        stackRestore.reset();
        return {};
    }

    [[nodiscard]] static TypeResult<Type> get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        const StackRestore stackRestore(L);

        Type list;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        while (lua_next(L, absIndex) != 0)
        {
            auto item = Stack<T>::get(L, -1);
            if (! item)
                return makeErrorCode(ErrorCode::InvalidTypeCast);

            list.emplace_back(*item);
            lua_pop(L, 1);
        }

        return list;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index);
    }
};

} 


// End File: Source/LuaBridge/List.h

// Begin File: Source/LuaBridge/detail/FuncTraits.h

namespace luabridge {
namespace detail {

[[noreturn]] inline void unreachable()
{
#if defined(__GNUC__) 
    __builtin_unreachable();
#elif defined(_MSC_VER) 
    __assume(false);
#endif
}

template< class T >
struct remove_cvref
{
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <bool IsMember, bool IsConst, class R, class... Args>
struct function_traits_base
{
    using result_type = R;

    using argument_types = std::tuple<Args...>;

    static constexpr auto arity = sizeof...(Args);

    static constexpr auto is_member = IsMember;

    static constexpr auto is_const = IsConst;
};

template <class, bool Enable>
struct function_traits_impl;

template <class R, class... Args>
struct function_traits_impl<R(Args...), true> : function_traits_base<false, false, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R (*)(Args...), true> : function_traits_base<false, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (C::*)(Args...), true> : function_traits_base<true, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (C::*)(Args...) const, true> : function_traits_base<true, true, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R(Args...) noexcept, true> : function_traits_base<false, false, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R (*)(Args...) noexcept, true> : function_traits_base<false, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (C::*)(Args...) noexcept, true> : function_traits_base<true, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (C::*)(Args...) const noexcept, true> : function_traits_base<true, true, R, Args...>
{
};

#if defined(_MSC_VER) && defined(_M_IX86) 
inline static constexpr bool is_stdcall_default_calling_convention = std::is_same_v<void __stdcall(), void()>;
inline static constexpr bool is_fastcall_default_calling_convention = std::is_same_v<void __fastcall(), void()>;

template <class R, class... Args>
struct function_traits_impl<R __stdcall(Args...), !is_stdcall_default_calling_convention> : function_traits_base<false, false, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R (__stdcall *)(Args...), !is_stdcall_default_calling_convention> : function_traits_base<false, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__stdcall C::*)(Args...), true> : function_traits_base<true, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__stdcall C::*)(Args...) const, true> : function_traits_base<true, true, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R __stdcall(Args...) noexcept, !is_stdcall_default_calling_convention> : function_traits_base<false, false, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R (__stdcall *)(Args...) noexcept, !is_stdcall_default_calling_convention> : function_traits_base<false, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__stdcall C::*)(Args...) noexcept, true> : function_traits_base<true, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__stdcall C::*)(Args...) const noexcept, true> : function_traits_base<true, true, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R __fastcall(Args...), !is_fastcall_default_calling_convention> : function_traits_base<false, false, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R (__fastcall *)(Args...), !is_fastcall_default_calling_convention> : function_traits_base<false, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__fastcall C::*)(Args...), true> : function_traits_base<true, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__fastcall C::*)(Args...) const, true> : function_traits_base<true, true, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R __fastcall(Args...) noexcept, !is_fastcall_default_calling_convention> : function_traits_base<false, false, R, Args...>
{
};

template <class R, class... Args>
struct function_traits_impl<R (__fastcall *)(Args...) noexcept, !is_fastcall_default_calling_convention> : function_traits_base<false, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__fastcall C::*)(Args...) noexcept, true> : function_traits_base<true, false, R, Args...>
{
};

template <class C, class R, class... Args>
struct function_traits_impl<R (__fastcall C::*)(Args...) const noexcept, true> : function_traits_base<true, true, R, Args...>
{
};
#endif

template <class F>
struct functor_traits_impl : function_traits_impl<decltype(&F::operator()), true>
{
};

template <class F>
struct function_traits : std::conditional_t<std::is_class_v<F>,
                                            detail::functor_traits_impl<F>,
                                            detail::function_traits_impl<F, true>>
{
};

template <std::size_t I, class F, class = void>
struct function_argument_or_void
{
    using type = void;
};

template <std::size_t I, class F>
struct function_argument_or_void<I, F, std::enable_if_t<I < std::tuple_size_v<typename function_traits<F>::argument_types>>>
{
    using type = std::tuple_element_t<I, typename function_traits<F>::argument_types>;
};

template <std::size_t I, class F>
using function_argument_or_void_t = typename function_argument_or_void<I, F>::type;

template <class F>
using function_result_t = typename function_traits<F>::result_type;

template <std::size_t I, class F>
using function_argument_t = std::tuple_element_t<I, typename function_traits<F>::argument_types>;

template <class F>
using function_arguments_t = typename function_traits<F>::argument_types;

template <class F>
static constexpr std::size_t function_arity_v = function_traits<F>::arity;

template <class F>
static constexpr bool function_is_member_v = function_traits<F>::is_member;

template <class F>
static constexpr bool function_is_const_v = function_traits<F>::is_const;

template <class T, class = void>
struct is_callable
{
    static constexpr bool value = false;
};

template <class T>
struct is_callable<T, std::void_t<decltype(&T::operator())>>
{
    static constexpr bool value = true;
};

template <class T>
struct is_callable<T, std::enable_if_t<std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>>>
{
    static constexpr bool value = true;
};

template <class T>
struct is_callable<T, std::enable_if_t<std::is_member_function_pointer_v<T>>>
{
    static constexpr bool value = true;
};

template <class T>
inline static constexpr bool is_callable_v = is_callable<T>::value;

template <class T>
struct is_const_member_function_pointer
{
    static constexpr bool value = false;
};

template <class T, class R, class... Args>
struct is_const_member_function_pointer<R (T::*)(Args...)>
{
    static constexpr bool value = false;
};

template <class T, class R, class... Args>
struct is_const_member_function_pointer<R (T::*)(Args...) const>
{
    static constexpr bool value = true;
};

template <class T, class R, class... Args>
struct is_const_member_function_pointer<R (T::*)(Args...) noexcept>
{
    static constexpr bool value = false;
};

template <class T, class R, class... Args>
struct is_const_member_function_pointer<R (T::*)(Args...) const noexcept>
{
    static constexpr bool value = true;
};

template <class T>
inline static constexpr bool is_const_member_function_pointer_v = is_const_member_function_pointer<T>::value;

template <class T>
struct is_cfunction_pointer
{
    static constexpr bool value = false;
};

template <>
struct is_cfunction_pointer<int (*)(lua_State*)>
{
    static constexpr bool value = true;
};

template <class T>
inline static constexpr bool is_cfunction_pointer_v = is_cfunction_pointer<T>::value;

template <class T>
struct is_member_cfunction_pointer
{
    static constexpr bool value = false;
};

template <class T>
struct is_member_cfunction_pointer<int (T::*)(lua_State*)>
{
    static constexpr bool value = true;
};

template <class T>
struct is_member_cfunction_pointer<int (T::*)(lua_State*) const>
{
    static constexpr bool value = true;
};

template <class T>
inline static constexpr bool is_member_cfunction_pointer_v = is_member_cfunction_pointer<T>::value;

template <class T>
struct is_const_member_cfunction_pointer
{
    static constexpr bool value = false;
};

template <class T>
struct is_const_member_cfunction_pointer<int (T::*)(lua_State*)>
{
    static constexpr bool value = false;
};

template <class T>
struct is_const_member_cfunction_pointer<int (T::*)(lua_State*) const>
{
    static constexpr bool value = true;
};

template <class T>
inline static constexpr bool is_const_member_cfunction_pointer_v = is_const_member_cfunction_pointer<T>::value;

template <class T>
inline static constexpr bool is_any_cfunction_pointer_v = is_cfunction_pointer_v<T> || is_member_cfunction_pointer_v<T>;

template <class T, class F>
inline static constexpr bool is_proxy_member_function_v =
    !std::is_member_function_pointer_v<F> &&
    std::is_same_v<T, remove_cvref_t<std::remove_pointer_t<function_argument_or_void_t<0, F>>>>;

template <class T, class F>
inline static constexpr bool is_const_proxy_function_v =
    is_proxy_member_function_v<T, F> &&
    std::is_const_v<std::remove_reference_t<std::remove_pointer_t<function_argument_or_void_t<0, F>>>>;

template <class, class>
struct function_arity_excluding
{
};

template < class... Ts, class ExclusionType>
struct function_arity_excluding<std::tuple<Ts...>, ExclusionType>
    : std::integral_constant<std::size_t, (0 + ... + (std::is_same_v<std::decay_t<Ts>, ExclusionType> ? 0 : 1))>
{
};

template <class F, class ExclusionType>
inline static constexpr std::size_t function_arity_excluding_v = function_arity_excluding<function_arguments_t<F>, ExclusionType>::value;

template <class, class, class, class, class = void>
struct member_function_arity_excluding
{
};

template <class T, class F, class... Ts, class ExclusionType>
struct member_function_arity_excluding<T, F, std::tuple<Ts...>, ExclusionType, std::enable_if_t<!is_proxy_member_function_v<T, F>>>
    : std::integral_constant<std::size_t, (0 + ... + (std::is_same_v<std::decay_t<Ts>, ExclusionType> ? 0 : 1))>
{
};

template <class T, class F, class... Ts, class ExclusionType>
struct member_function_arity_excluding<T, F, std::tuple<Ts...>, ExclusionType, std::enable_if_t<is_proxy_member_function_v<T, F>>>
    : std::integral_constant<std::size_t, (0 + ... + (std::is_same_v<std::decay_t<Ts>, ExclusionType> ? 0 : 1)) - 1>
{
};

template <class T, class F, class ExclusionType>
inline static constexpr std::size_t member_function_arity_excluding_v = member_function_arity_excluding<T, F, function_arguments_t<F>, ExclusionType>::value;

template <class T, class F>
static constexpr bool is_const_function =
    detail::is_const_member_function_pointer_v<F> ||
        (detail::function_arity_v<F> > 0 && detail::is_const_proxy_function_v<T, F>);

template <class T, class... Fs>
inline static constexpr std::size_t const_functions_count = (0 + ... + (is_const_function<T, Fs> ? 1 : 0));

template <class T, class... Fs>
inline static constexpr std::size_t non_const_functions_count = (0 + ... + (is_const_function<T, Fs> ? 0 : 1));

template <class... Types>
constexpr auto tupleize(Types&&... types)
{
    return std::tuple<Types...>(std::forward<Types>(types)...);
}

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

} 
} 


// End File: Source/LuaBridge/detail/FuncTraits.h

// Begin File: Source/LuaBridge/detail/FlagSet.h

namespace luabridge {

template <class T, class... Ts>
class FlagSet
{
    static_assert(std::is_integral_v<T>);

public:
    constexpr FlagSet() noexcept = default;

    constexpr void set(FlagSet other) noexcept
    {
        flags |= other.flags;
    }

    constexpr FlagSet withSet(FlagSet other) const noexcept
    {
        FlagSet result { flags };
        result.flags |= other.flags;
        return result;
    }

    constexpr void unset(FlagSet other) noexcept
    {
        flags &= ~other.flags;
    }

    constexpr FlagSet withUnset(FlagSet other) const noexcept
    {
        FlagSet result { flags };
        result.flags &= ~other.flags;
        return result;
    }

    constexpr bool test(FlagSet other) const noexcept
    {
        return (flags & other.flags) != 0;
    }

    constexpr FlagSet operator|(FlagSet other) const noexcept
    {
        return FlagSet(flags | other.flags);
    }

    constexpr FlagSet operator&(FlagSet other) const noexcept
    {
        return FlagSet(flags & other.flags);
    }

    constexpr FlagSet operator~() const noexcept
    {
        return FlagSet(~flags);
    }

    constexpr T toUnderlying() const noexcept
    {
        return flags;
    }

    std::string toString() const
    {
        std::string result;
        result.reserve(sizeof(T) * std::numeric_limits<uint8_t>::digits);

        (result.append((mask<Ts>() & flags) ? "1" : "0"), ...);

        for (std::size_t i = sizeof...(Ts); i < sizeof(T) * std::numeric_limits<uint8_t>::digits; ++i)
            result.append("0");

        std::reverse(result.begin(), result.end());

        return result;
    }

    template <class... Us>
    static constexpr FlagSet Value() noexcept
    {
        return FlagSet{ mask<Us...>() };
    }

    template <class U>
    static constexpr auto fromUnderlying(U newFlags) noexcept
        -> std::enable_if_t<std::is_integral_v<U> && std::is_convertible_v<U, T>, FlagSet>
    {
        return { static_cast<T>(newFlags) };
    }

private:
    template <class U, class V, class... Us>
    static constexpr T indexOf() noexcept
    {
        if constexpr (std::is_same_v<U, V>)
            return static_cast<T>(0);
        else
            return static_cast<T>(1) + indexOf<U, Us...>();
    }

    template <class... Us>
    static constexpr T mask() noexcept
    {
        return ((static_cast<T>(1) << indexOf<Us, Ts...>()) | ...);
    }

    constexpr FlagSet(T flags) noexcept
        : flags(flags)
    {
    }

    T flags = 0;
};

} 


// End File: Source/LuaBridge/detail/FlagSet.h

// Begin File: Source/LuaBridge/detail/Options.h

namespace luabridge {

namespace detail {
struct OptionExtensibleClass;
struct OptionAllowOverridingMethods;
struct OptionVisibleMetatables;
} 

using Options = FlagSet<uint32_t,
    detail::OptionExtensibleClass,
    detail::OptionAllowOverridingMethods,
    detail::OptionVisibleMetatables>;

static inline constexpr Options defaultOptions = Options();

static inline constexpr Options extensibleClass = Options::Value<detail::OptionExtensibleClass>();

static inline constexpr Options allowOverridingMethods = Options::Value<detail::OptionAllowOverridingMethods>();

static inline constexpr Options visibleMetatables = Options::Value<detail::OptionVisibleMetatables>();

} 


// End File: Source/LuaBridge/detail/Options.h

// Begin File: Source/LuaBridge/detail/CFunctions.h

namespace luabridge {
namespace detail {

template <class T>
auto unwrap_argument_or_error(lua_State* L, std::size_t index, std::size_t start)
{
    auto result = Stack<T>::get(L, static_cast<int>(index + start));
    if (! result)
        raise_lua_error(L, "Error decoding argument #%d: %s", static_cast<int>(index + 1), result.message().c_str());

    return std::move(*result);
}

template <class ArgsPack, std::size_t Start, std::size_t... Indices>
auto make_arguments_list_impl(lua_State* L, std::index_sequence<Indices...>)
{
    return tupleize(unwrap_argument_or_error<std::tuple_element_t<Indices, ArgsPack>>(L, Indices, Start)...);
}

template <class ArgsPack, std::size_t Start>
auto make_arguments_list(lua_State* L)
{
    return make_arguments_list_impl<ArgsPack, Start>(L, std::make_index_sequence<std::tuple_size_v<ArgsPack>>());
}

template <std::size_t Index = 0, class... Types>
auto push_arguments(lua_State*, std::tuple<Types...>)
    -> std::enable_if_t<Index == sizeof...(Types), std::tuple<Result, std::size_t>>
{
    return std::make_tuple(Result(), Index + 1);
}

template <std::size_t Index = 0, class... Types>
auto push_arguments(lua_State* L, std::tuple<Types...> t)
    -> std::enable_if_t<Index < sizeof...(Types), std::tuple<Result, std::size_t>>
{
    using T = std::tuple_element_t<Index, std::tuple<Types...>>;

    auto result = Stack<T>::push(L, std::get<Index>(t));
    if (! result)
        return std::make_tuple(result, Index + 1);

    return push_arguments<Index + 1, Types...>(L, std::move(t));
}

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

template <class T = void, class... Args>
constexpr auto make_array(Args&&... args)
{
    if constexpr (std::is_same_v<void, T>)
        return std::array<std::common_type_t<std::decay_t<Args>...>, sizeof...(Args)>{{ std::forward<Args>(args)... }};
    else
        return std::array<T, sizeof...(Args)>{{ std::forward<Args>(args)... }};
}

inline bool is_metamethod(std::string_view method_name)
{
    static constexpr auto metamethods = make_array<std::string_view>(
        "__add",
        "__band",
        "__bnot",
        "__bor",
        "__bxor",
        "__call",
        "__close",
        "__concat",
        "__div",
        "__eq",
        "__gc",
        "__idiv",
        "__index",
        "__ipairs",
        "__le",
        "__len",
        "__lt",
        "__metatable",
        "__mod",
        "__mode",
        "__mul",
        "__name",
        "__newindex",
        "__pairs",
        "__pow",
        "__shl",
        "__shr",
        "__sub",
        "__tostring",
        "__unm"
    );

    if (method_name.size() <= 2 || method_name[0] != '_' || method_name[1] != '_')
        return false;

    auto result = std::lower_bound(metamethods.begin(), metamethods.end(), method_name);
    return result != metamethods.end() && *result == method_name;
}

inline std::string make_super_method_name(const char* name)
{
    LUABRIDGE_ASSERT(name != nullptr);

    return (std::string_view(name).find("_") == 0u)
        ? (std::string("super") + name)
        : (std::string("super_") + name);
}

inline Options get_class_options(lua_State* L, int index)
{
    LUABRIDGE_ASSERT(lua_istable(L, index)); 

    Options options = defaultOptions;

    lua_rawgetp(L, index, getClassOptionsKey()); 
    if (lua_isnumber(L, -1))
        options = Options::fromUnderlying(lua_tointeger(L, -1));

    lua_pop(L, 1);

    return options;
}

inline void push_class_or_const_table(lua_State* L, int index)
{
    LUABRIDGE_ASSERT(lua_istable(L, index)); 

    lua_rawgetp(L, index, getClassKey()); 
    if (! lua_istable(L, -1)) 
    {
        lua_pop(L, 1); 

        lua_rawgetp(L, index, getConstKey()); 
        if (! lua_istable(L, -1)) 
            return;
    }
}

inline std::optional<int> try_call_index_fallback(lua_State* L)
{
    LUABRIDGE_ASSERT(lua_istable(L, -1)); 

    lua_rawgetp(L, -1, getIndexFallbackKey()); 
    if (! lua_iscfunction(L, -1))
    {
        lua_pop(L, 1); 
        return std::nullopt;
    }

    lua_pushvalue(L, 1); 
    lua_pushvalue(L, 2); 
    lua_call(L, 2, 1); 

    if (! lua_isnoneornil(L, -1))
    {
        lua_remove(L, -2); 
        return 1;
    }

    lua_pop(L, 1); 
    return std::nullopt;
}

template <bool IsObject>
inline std::optional<int> try_call_index_extensible(lua_State* L, const char* key)
{
    LUABRIDGE_ASSERT(lua_istable(L, -1)); 

    if constexpr (IsObject)
        push_class_or_const_table(L, -1); 
    else
        lua_rawgetp(L, -1, getStaticKey()); 

    LUABRIDGE_ASSERT(lua_istable(L, -1)); 
    rawgetfield(L, -1, key); 

    if (! lua_isnoneornil(L, -1)) 
    {
        lua_remove(L, -2); 
        lua_remove(L, -2); 
        return 1;
    }

    lua_pop(L, 2); 
    return std::nullopt;
}

template <bool IsObject>
inline int index_metamethod(lua_State* L)
{
#if LUABRIDGE_SAFE_STACK_CHECKS
    luaL_checkstack(L, 3, detail::error_lua_stack_overflow);
#endif

    LUABRIDGE_ASSERT(lua_istable(L, 1) || lua_isuserdata(L, 1)); 

    lua_getmetatable(L, 1); 
    LUABRIDGE_ASSERT(lua_istable(L, -1));

    const char* key = lua_tostring(L, 2);
    if (key != nullptr && is_metamethod(key))
    {
        lua_pushnil(L);
        return 1;
    }

    for (;;)
    {
        if constexpr (IsObject)
        {
            
            if (auto result = try_call_index_fallback(L))
                return *result;
        }

        if (lua_istable(L, 1))
        {
            if constexpr (IsObject)
                lua_pushvalue(L, 1); 
            else
                push_class_or_const_table(L, -1); 

            if (lua_istable(L, -1))
            {
                lua_pushvalue(L, 2); 
                lua_rawget(L, -2); 
                lua_remove(L, -2); 
                if (! lua_isnil(L, -1)) 
                {
                    lua_remove(L, -2); 
                    return 1;
                }
            }

            lua_pop(L, 1); 
        }

        lua_pushvalue(L, 2); 
        lua_rawget(L, -2); 
        if (! lua_isnil(L, -1)) 
        {
            lua_remove(L, -2); 
            return 1;
        }

        LUABRIDGE_ASSERT(lua_isnil(L, -1)); 
        lua_pop(L, 1); 

        const Options options = get_class_options(L, -1); 
        if (options.test(extensibleClass | allowOverridingMethods))
        {
            if (auto result = try_call_index_extensible<IsObject>(L, key))
                return *result;
        }

        lua_rawgetp(L, -1, getPropgetKey()); 
        LUABRIDGE_ASSERT(lua_istable(L, -1));

        lua_pushvalue(L, 2); 
        lua_rawget(L, -2); 
        lua_remove(L, -2); 

        if (lua_iscfunction(L, -1)) 
        {
            lua_remove(L, -2); 
            lua_pushvalue(L, 1); 
            lua_call(L, 1, 1); 
            return 1;
        }

        LUABRIDGE_ASSERT(lua_isnil(L, -1)); 
        lua_pop(L, 1); 

        lua_rawgetp(L, -1, getParentKey()); 
        if (lua_isnil(L, -1)) 
        {
            lua_pop(L, 2); 
            break;
        }

        LUABRIDGE_ASSERT(lua_istable(L, -1)); 
        lua_remove(L, -2); 
    }

    lua_getmetatable(L, 1); 
    LUABRIDGE_ASSERT(lua_istable(L, -1));

    for (;;)
    {
        const Options options = get_class_options(L, -1); 

        if (options.test(extensibleClass | ~allowOverridingMethods))
        {
            if (auto result = try_call_index_extensible<IsObject>(L, key))
                return *result;
        }

        lua_rawgetp(L, -1, getParentKey()); 
        if (lua_isnil(L, -1)) 
        {
            lua_remove(L, -2); 
            return 1;
        }

        LUABRIDGE_ASSERT(lua_istable(L, -1)); 
        lua_remove(L, -2); 
    }

}

inline std::optional<int> try_call_newindex_fallback(lua_State* L)
{
    LUABRIDGE_ASSERT(lua_istable(L, -1)); 

    lua_rawgetp(L, -1, getNewIndexFallbackKey()); 
    if (! lua_iscfunction(L, -1))
    {
        lua_pop(L, 1); 
        return std::nullopt;
    }

    lua_pushvalue(L, 1); 
    lua_pushvalue(L, 2); 
    lua_pushvalue(L, 3); 
    lua_call(L, 3, 0); 

    return 0;
}

inline std::optional<int> try_call_newindex_extensible(lua_State* L, const char* key)
{
    LUABRIDGE_ASSERT(key != nullptr);
    LUABRIDGE_ASSERT(lua_istable(L, -1)); 

    lua_pushvalue(L, -1); 

    for (;;)
    {
        push_class_or_const_table(L, -1); 
        if (! lua_istable(L, -1)) 
        {
            lua_pop(L, 2); 
            return std::nullopt;
        }

        lua_pushvalue(L, 2); 
        lua_rawget(L, -2); 

        if (! lua_isnil(L, -1)) 
        {
            if (! lua_iscfunction(L, -1))
            {
                lua_pop(L, 1);
                break;
            }

            const Options options = get_class_options(L, -2); 
            if (! options.test(allowOverridingMethods))
                luaL_error(L, "immutable member '%s'", key);

            rawsetfield(L, -2, make_super_method_name(key).c_str()); 
            break;
        }

        lua_pop(L, 1); 

        lua_rawgetp(L, -2, getParentKey()); 
        if (lua_isnil(L, -1)) 
        {
            lua_pop(L, 1); 
            break;
        }

        LUABRIDGE_ASSERT(lua_istable(L, -1)); 
        lua_remove(L, -2); 
        lua_remove(L, -2); 
    }

    lua_remove(L, -2); 
    lua_getmetatable(L, -1); 
    lua_pushvalue(L, 3); 
    rawsetfield(L, -2, key); 

    lua_pop(L, 2); 

    return 0;
}

template <bool IsObject>
inline int newindex_metamethod(lua_State* L)
{
#if LUABRIDGE_SAFE_STACK_CHECKS
    luaL_checkstack(L, 3, detail::error_lua_stack_overflow);
#endif

    LUABRIDGE_ASSERT(lua_istable(L, 1) || lua_isuserdata(L, 1)); 

    lua_getmetatable(L, 1); 
    LUABRIDGE_ASSERT(lua_istable(L, -1));

    const char* key = lua_tostring(L, 2);

    for (;;)
    {
        const Options options = get_class_options(L, -1);

        lua_rawgetp(L, -1, getPropsetKey()); 
        if (lua_isnil(L, -1)) 
            luaL_error(L, "no member named '%s'", key);

        LUABRIDGE_ASSERT(lua_istable(L, -1));

        lua_pushvalue(L, 2); 
        lua_rawget(L, -2); 
        lua_remove(L, -2); 

        if (lua_iscfunction(L, -1)) 
        {
            lua_remove(L, -2); 
            if constexpr (IsObject)
                lua_pushvalue(L, 1); 
            lua_pushvalue(L, 3); 
            lua_call(L, IsObject ? 2 : 1, 0); 
            return 0;
        }

        LUABRIDGE_ASSERT(lua_isnil(L, -1)); 
        lua_pop(L, 1); 

        if constexpr (IsObject)
        {
            
            if (auto result = try_call_newindex_fallback(L))
                return *result;
        }
        else
        {
            
            if (options.test(extensibleClass))
            {
                if (auto result = try_call_newindex_extensible(L, key))
                    return *result;
            }
        }

        lua_rawgetp(L, -1, getParentKey()); 
        if (lua_isnil(L, -1)) 
            luaL_error(L, "no writable member '%s'", key);

        LUABRIDGE_ASSERT(lua_istable(L, -1)); 
        lua_remove(L, -2); 

    }

    return 0;
}

inline int read_only_error(lua_State* L)
{
    std::stringstream ss;

    ss << "'" << lua_tostring(L, lua_upvalueindex(1)) << "' is read-only";

    raise_lua_error(L, "%s", ss.str().c_str());

    return 0;
}

template <class C>
static int tostring_metamethod(lua_State* L)
{
    const void* ptr = lua_topointer(L, 1);

    lua_getmetatable(L, -1); 
    lua_rawgetp(L, -1, getTypeKey()); 
    lua_remove(L, -2); 

    std::stringstream ss;
    ss << ": 0x" << std::hex << reinterpret_cast<std::uintptr_t>(const_cast<void*>(ptr));
    lua_pushstring(L, ss.str().c_str()); 
    lua_concat(L, 2); 

    return 1;
}

template <class C>
static int gc_metamethod(lua_State* L)
{
    Userdata* ud = Userdata::getExact<C>(L, 1);
    LUABRIDGE_ASSERT(ud);

    ud->~Userdata();

    return 0;
}

template <class T, class C = void>
struct property_getter;

template <class T>
struct property_getter<T, void>
{
    static int call(lua_State* L)
    {
        LUABRIDGE_ASSERT(lua_islightuserdata(L, lua_upvalueindex(1)));

        T* ptr = static_cast<T*>(lua_touserdata(L, lua_upvalueindex(1)));
        LUABRIDGE_ASSERT(ptr != nullptr);

        auto result = Stack<T&>::push(L, *ptr);
        if (! result)
            raise_lua_error(L, "%s", result.message().c_str());

        return 1;
    }
};

template <class T, class C>
struct property_getter
{
    static int call(lua_State* L)
    {
        C* c = Userdata::get<C>(L, 1, true);

        T C::** mp = static_cast<T C::**>(lua_touserdata(L, lua_upvalueindex(1)));

        Result result;

#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            result = Stack<T&>::push(L, c->**mp);

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        if (! result)
            raise_lua_error(L, "%s", result.message().c_str());

        return 1;
    }
};

inline void add_property_getter(lua_State* L, const char* name, int tableIndex)
{
#if LUABRIDGE_SAFE_STACK_CHECKS
    luaL_checkstack(L, 2, detail::error_lua_stack_overflow);
#endif

    LUABRIDGE_ASSERT(name != nullptr);
    LUABRIDGE_ASSERT(lua_istable(L, tableIndex));
    LUABRIDGE_ASSERT(lua_iscfunction(L, -1)); 

    lua_rawgetp(L, tableIndex, getPropgetKey()); 
    lua_pushvalue(L, -2); 
    rawsetfield(L, -2, name); 
    lua_pop(L, 2); 
}

template <class T, class C = void>
struct property_setter;

template <class T>
struct property_setter<T, void>
{
    static int call(lua_State* L)
    {
        LUABRIDGE_ASSERT(lua_islightuserdata(L, lua_upvalueindex(1)));

        T* ptr = static_cast<T*>(lua_touserdata(L, lua_upvalueindex(1)));
        LUABRIDGE_ASSERT(ptr != nullptr);

        auto result = Stack<T>::get(L, 1);
        if (! result)
            raise_lua_error(L, "%s", result.error().message().c_str());

        *ptr = std::move(*result);

        return 0;
    }
};

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
            auto result = Stack<T>::get(L, 2);
            if (! result)
                raise_lua_error(L, "%s", result.error().message().c_str());

            c->** mp = std::move(*result);

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

inline void add_property_setter(lua_State* L, const char* name, int tableIndex)
{
#if LUABRIDGE_SAFE_STACK_CHECKS
    luaL_checkstack(L, 2, detail::error_lua_stack_overflow);
#endif

    LUABRIDGE_ASSERT(name != nullptr);
    LUABRIDGE_ASSERT(lua_istable(L, tableIndex));
    LUABRIDGE_ASSERT(lua_iscfunction(L, -1)); 

    lua_rawgetp(L, tableIndex, getPropsetKey()); 
    lua_pushvalue(L, -2); 
    rawsetfield(L, -2, name); 
    lua_pop(L, 2); 
}

template <class ReturnType, class ArgsPack, std::size_t Start = 1u>
struct function
{
    template <class F>
    static int call(lua_State* L, F&& func)
    {
        Result result;

#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            result = Stack<ReturnType>::push(L, std::apply(std::forward<F>(func), make_arguments_list<ArgsPack, Start>(L)));

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        if (! result)
            raise_lua_error(L, "%s", result.message().c_str());

        return 1;
    }

    template <class T, class F>
    static int call(lua_State* L, T* ptr, F&& func)
    {
        Result result;

#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            auto f = [ptr, func = std::forward<F>(func)](auto&&... args) -> ReturnType { return (ptr->*func)(std::forward<decltype(args)>(args)...); };

            result = Stack<ReturnType>::push(L, std::apply(f, make_arguments_list<ArgsPack, Start>(L)));

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        if (! result)
            raise_lua_error(L, "%s", result.message().c_str());

        return 1;
    }
};

template <class ArgsPack, std::size_t Start>
struct function<void, ArgsPack, Start>
{
    template <class F>
    static int call(lua_State* L, F&& func)
    {
#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            std::apply(std::forward<F>(func), make_arguments_list<ArgsPack, Start>(L));

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
    static int call(lua_State* L, T* ptr, F&& func)
    {
#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            auto f = [ptr, func = std::forward<F>(func)](auto&&... args) { (ptr->*func)(std::forward<decltype(args)>(args)...); };

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

template <class F, class T>
int invoke_member_function(lua_State* L)
{
    using FnTraits = function_traits<F>;

    LUABRIDGE_ASSERT(isfulluserdata(L, lua_upvalueindex(1)));

    T* ptr = Userdata::get<T>(L, 1, false);

    const F& func = *static_cast<const F*>(lua_touserdata(L, lua_upvalueindex(1)));
    LUABRIDGE_ASSERT(func != nullptr);

    return function<typename FnTraits::result_type, typename FnTraits::argument_types, 2>::call(L, ptr, func);
}

template <class F, class T>
int invoke_const_member_function(lua_State* L)
{
    using FnTraits = function_traits<F>;

    LUABRIDGE_ASSERT(isfulluserdata(L, lua_upvalueindex(1)));

    const T* ptr = Userdata::get<T>(L, 1, true);

    const F& func = *static_cast<const F*>(lua_touserdata(L, lua_upvalueindex(1)));
    LUABRIDGE_ASSERT(func != nullptr);

    return function<typename FnTraits::result_type, typename FnTraits::argument_types, 2>::call(L, ptr, func);
}

template <class T>
int invoke_member_cfunction(lua_State* L)
{
    using F = int (T::*)(lua_State* L);

    LUABRIDGE_ASSERT(isfulluserdata(L, lua_upvalueindex(1)));

    T* t = Userdata::get<T>(L, 1, false);

    const F& func = *static_cast<const F*>(lua_touserdata(L, lua_upvalueindex(1)));
    LUABRIDGE_ASSERT(func != nullptr);

#if LUABRIDGE_HAS_EXCEPTIONS
    try
    {
#endif
        return (t->*func)(L);

#if LUABRIDGE_HAS_EXCEPTIONS
    }
    catch (const std::exception& e)
    {
        raise_lua_error(L, "%s", e.what());
    }
    
    return 0;
#endif
}

template <class T>
int invoke_const_member_cfunction(lua_State* L)
{
    using F = int (T::*)(lua_State * L) const;

    LUABRIDGE_ASSERT(isfulluserdata(L, lua_upvalueindex(1)));

    const T* t = Userdata::get<T>(L, 1, true);

    const F& func = *static_cast<const F*>(lua_touserdata(L, lua_upvalueindex(1)));
    LUABRIDGE_ASSERT(func != nullptr);

#if LUABRIDGE_HAS_EXCEPTIONS
    try
    {
#endif
        return (t->*func)(L);

#if LUABRIDGE_HAS_EXCEPTIONS
    }
    catch (const std::exception& e)
    {
        raise_lua_error(L, "%s", e.what());
    }
    
    return 0;
#endif
}

template <class F>
int invoke_proxy_function(lua_State* L)
{
    using FnTraits = function_traits<F>;

    LUABRIDGE_ASSERT(lua_islightuserdata(L, lua_upvalueindex(1)));

    auto func = reinterpret_cast<F>(lua_touserdata(L, lua_upvalueindex(1)));
    LUABRIDGE_ASSERT(func != nullptr);

    return function<typename FnTraits::result_type, typename FnTraits::argument_types, 1>::call(L, func);
}

template <class F>
int invoke_proxy_functor(lua_State* L)
{
    using FnTraits = function_traits<std::remove_reference_t<F>>;

    LUABRIDGE_ASSERT(isfulluserdata(L, lua_upvalueindex(1)));

    auto& func = *align<std::remove_reference_t<F>>(lua_touserdata(L, lua_upvalueindex(1)));

    return function<typename FnTraits::result_type, typename FnTraits::argument_types, 1>::call(L, func);
}

#if LUABRIDGE_SAFE_LUA_C_EXCEPTION_HANDLING && LUABRIDGE_HAS_EXCEPTIONS
inline int invoke_safe_cfunction(lua_State* L)
{
    LUABRIDGE_ASSERT(lua_iscfunction(L, lua_upvalueindex(1)));

    auto func = lua_tocfunction(L, lua_upvalueindex(1));

    try
    {
        return func(L);
    }
    catch (const std::exception& e)
    {
        raise_lua_error(L, "%s", e.what());
    }

    return 0;
}
#endif

template <class F>
int invoke_proxy_constructor(lua_State* L)
{
    using FnTraits = function_traits<F>;

    LUABRIDGE_ASSERT(isfulluserdata(L, lua_upvalueindex(1)));

    auto& func = *align<F>(lua_touserdata(L, lua_upvalueindex(1)));

    function<void, typename FnTraits::argument_types, 1>::call(L, func);

    return 1;
}

template <bool Member>
inline int try_overload_functions(lua_State* L)
{
    const int nargs = lua_gettop(L);
    const int effective_args = nargs - (Member ? 1 : 0);

    lua_pushvalue(L, lua_upvalueindex(1));
    LUABRIDGE_ASSERT(lua_istable(L, -1));
    const int idx_overloads = nargs + 1;
    const int num_overloads = get_length(L, idx_overloads);

    lua_createtable(L, num_overloads, 0);
    const int idx_errors = nargs + 2;
    int nerrors = 0;

    lua_pushnil(L);  
    while (lua_next(L, idx_overloads) != 0)
    {
        LUABRIDGE_ASSERT(lua_istable(L, -1));

        lua_rawgeti(L, -1, 1);
        LUABRIDGE_ASSERT(lua_isnumber(L, -1));

        const int overload_arity = static_cast<int>(lua_tointeger(L, -1));
        if (overload_arity >= 0 && overload_arity != effective_args)
        {
            
            lua_pushfstring(L, "Skipped overload #%d with unmatched arity of %d instead of %d", nerrors, overload_arity, effective_args);
            lua_rawseti(L, idx_errors, ++nerrors);

            lua_pop(L, 2); 
            continue;
        }

        lua_pop(L, 1); 

        lua_pushnumber(L, 2);
        lua_gettable(L, -2);
        LUABRIDGE_ASSERT(lua_isfunction(L, -1));

        for (int i = 1; i <= nargs; ++i)
            lua_pushvalue(L, i);

        const int err = lua_pcall(L, nargs, LUA_MULTRET, 0);
        if (err == LUABRIDGE_LUA_OK)
        {
            
            return lua_gettop(L) - nargs - 4; 
        }
        else if (err == LUA_ERRRUN)
        {
            
            lua_rawseti(L, idx_errors, ++nerrors);
        }
        else
        {
            return lua_error_x(L);  
        }

        lua_pop(L, 1); 
    }

    lua_Debug debug;
    lua_getstack_info_x(L, 0, "n", &debug);
    lua_pushfstring(L, "All %d overloads of %s returned an error:", nerrors, debug.name);

    for (int i = 1; i <= nerrors; ++i)
    {
        lua_pushfstring(L, "\n%d: ", i);
        lua_rawgeti(L, idx_errors, i);
    }
    lua_concat(L, nerrors * 2 + 1);

    return lua_error_x(L); 
}

inline void push_function(lua_State* L, lua_CFunction fp, const char* debugname)
{
#if LUABRIDGE_SAFE_LUA_C_EXCEPTION_HANDLING && LUABRIDGE_HAS_EXCEPTIONS
    lua_pushcfunction_x(L, fp, debugname);
    lua_pushcclosure_x(L, invoke_safe_cfunction, debugname, 1);
#else
    lua_pushcfunction_x(L, fp, debugname);
#endif
}

template <class ReturnType, class... Params>
inline void push_function(lua_State* L, ReturnType (*fp)(Params...), const char* debugname)
{
    using FnType = decltype(fp);

    lua_pushlightuserdata(L, reinterpret_cast<void*>(fp));
    lua_pushcclosure_x(L, &invoke_proxy_function<FnType>, debugname, 1);
}

template <class ReturnType, class... Params>
inline void push_function(lua_State* L, ReturnType (*fp)(Params...) noexcept, const char* debugname)
{
    using FnType = decltype(fp);

    lua_pushlightuserdata(L, reinterpret_cast<void*>(fp));
    lua_pushcclosure_x(L, &invoke_proxy_function<FnType>, debugname, 1);
}

template <class F, class = std::enable_if<is_callable_v<F> && !std::is_pointer_v<F> && !std::is_member_function_pointer_v<F>>>
inline void push_function(lua_State* L, F&& f, const char* debugname)
{
    lua_newuserdata_aligned<F>(L, std::forward<F>(f));
    lua_pushcclosure_x(L, &invoke_proxy_functor<F>, debugname, 1);
}

template <class T>
void push_member_function(lua_State* L, lua_CFunction fp, const char* debugname)
{
#if LUABRIDGE_SAFE_LUA_C_EXCEPTION_HANDLING && LUABRIDGE_HAS_EXCEPTIONS
    lua_pushcfunction_x(L, fp, debugname);
    lua_pushcclosure_x(L, invoke_safe_cfunction, debugname, 1);
#else    
    lua_pushcfunction_x(L, fp, debugname);
#endif
}

template <class T, class ReturnType, class... Params>
void push_member_function(lua_State* L, ReturnType (*fp)(T*, Params...), const char* debugname)
{
    using FnType = decltype(fp);

    lua_pushlightuserdata(L, reinterpret_cast<void*>(fp));
    lua_pushcclosure_x(L, &invoke_proxy_function<FnType>, debugname, 1);
}

template <class T, class ReturnType, class... Params>
void push_member_function(lua_State* L, ReturnType (*fp)(T*, Params...) noexcept, const char* debugname)
{
    using FnType = decltype(fp);

    lua_pushlightuserdata(L, reinterpret_cast<void*>(fp));
    lua_pushcclosure_x(L, &invoke_proxy_function<FnType>, debugname, 1);
}

template <class T, class ReturnType, class... Params>
void push_member_function(lua_State* L, ReturnType (*fp)(const T*, Params...), const char* debugname)
{
    using FnType = decltype(fp);

    lua_pushlightuserdata(L, reinterpret_cast<void*>(fp));
    lua_pushcclosure_x(L, &invoke_proxy_function<FnType>, debugname, 1);
}

template <class T, class ReturnType, class... Params>
void push_member_function(lua_State* L, ReturnType (*fp)(const T*, Params...) noexcept, const char* debugname)
{
    using FnType = decltype(fp);

    lua_pushlightuserdata(L, reinterpret_cast<void*>(fp));
    lua_pushcclosure_x(L, &invoke_proxy_function<FnType>, debugname, 1);
}

template <class T, class F, class = std::enable_if<
    is_callable_v<F> &&
        std::is_object_v<F> &&
        !std::is_pointer_v<F> &&
        !std::is_member_function_pointer_v<F>>>
void push_member_function(lua_State* L, F&& f, const char* debugname)
{
    static_assert(std::is_same_v<T, remove_cvref_t<std::remove_pointer_t<function_argument_or_void_t<0, F>>>>);

    lua_newuserdata_aligned<F>(L, std::forward<F>(f));
    lua_pushcclosure_x(L, &invoke_proxy_functor<F>, debugname, 1);
}

template <class T, class U, class ReturnType, class... Params>
void push_member_function(lua_State* L, ReturnType (U::*mfp)(Params...), const char* debugname)
{
    static_assert(std::is_same_v<T, U> || std::is_base_of_v<U, T>);

    using F = decltype(mfp);

    new (lua_newuserdata_x<F>(L, sizeof(F))) F(mfp);
    lua_pushcclosure_x(L, &invoke_member_function<F, T>, debugname, 1);
}

template <class T, class U, class ReturnType, class... Params>
void push_member_function(lua_State* L, ReturnType (U::*mfp)(Params...) noexcept, const char* debugname)
{
    static_assert(std::is_same_v<T, U> || std::is_base_of_v<U, T>);

    using F = decltype(mfp);

    new (lua_newuserdata_x<F>(L, sizeof(F))) F(mfp);
    lua_pushcclosure_x(L, &invoke_member_function<F, T>, debugname, 1);
}

template <class T, class U, class ReturnType, class... Params>
void push_member_function(lua_State* L, ReturnType (U::*mfp)(Params...) const, const char* debugname)
{
    static_assert(std::is_same_v<T, U> || std::is_base_of_v<U, T>);

    using F = decltype(mfp);

    new (lua_newuserdata_x<F>(L, sizeof(F))) F(mfp);
    lua_pushcclosure_x(L, &detail::invoke_const_member_function<F, T>, debugname, 1);
}

template <class T, class U, class ReturnType, class... Params>
void push_member_function(lua_State* L, ReturnType (U::*mfp)(Params...) const noexcept, const char* debugname)
{
    static_assert(std::is_same_v<T, U> || std::is_base_of_v<U, T>);

    using F = decltype(mfp);

    new (lua_newuserdata_x<F>(L, sizeof(F))) F(mfp);
    lua_pushcclosure_x(L, &detail::invoke_const_member_function<F, T>, debugname, 1);
}

template <class T, class U = T>
void push_member_function(lua_State* L, int (U::*mfp)(lua_State*), const char* debugname)
{
    static_assert(std::is_same_v<T, U> || std::is_base_of_v<U, T>);

    using F = decltype(mfp);

    new (lua_newuserdata_x<F>(L, sizeof(F))) F(mfp);
    lua_pushcclosure_x(L, &invoke_member_cfunction<T>, debugname, 1);
}

template <class T, class U = T>
void push_member_function(lua_State* L, int (U::*mfp)(lua_State*) const, const char* debugname)
{
    static_assert(std::is_same_v<T, U> || std::is_base_of_v<U, T>);

    using F = decltype(mfp);

    new (lua_newuserdata_x<F>(L, sizeof(F))) F(mfp);
    lua_pushcclosure_x(L, &invoke_const_member_cfunction<T>, debugname, 1);
}

template <class T, class Args>
struct constructor
{
    static T* construct(const Args& args)
    {
        auto alloc = [](auto&&... args) { return new T(std::forward<decltype(args)>(args)...); };

        return std::apply(alloc, args);
    }

    static T* construct(void* ptr, const Args& args)
    {
        auto alloc = [ptr](auto&&... args) { return new (ptr) T(std::forward<decltype(args)>(args)...); };

        return std::apply(alloc, args);
    }
};

template <class T>
struct placement_constructor
{
    template <class F, class Args>
    static T* construct(void* ptr, F&& func, const Args& args)
    {
        auto alloc = [ptr, func = std::forward<F>(func)](auto&&... args) { return func(ptr, std::forward<decltype(args)>(args)...); };

        return std::apply(alloc, args);
    }
};

template <class C>
struct container_constructor
{
    template <class F, class Args>
    static C construct(F&& func, const Args& args)
    {
        auto alloc = [func = std::forward<F>(func)](auto&&... args) { return func(std::forward<decltype(args)>(args)...); };

        return std::apply(alloc, args);
    }
};

template <class T>
struct external_constructor
{
    template <class F, class Args>
    static T* construct(F&& func, const Args& args)
    {
        auto alloc = [func = std::forward<F>(func)](auto&&... args) { return func(std::forward<decltype(args)>(args)...); };

        return std::apply(alloc, args);
    }
};

template <class C, class Args>
int constructor_container_proxy(lua_State* L)
{
    using T = typename ContainerTraits<C>::Type;

    T* object = nullptr;
    
#if LUABRIDGE_HAS_EXCEPTIONS
    try
    {
#endif
        object = constructor<T, Args>::construct(detail::make_arguments_list<Args, 2>(L));

#if LUABRIDGE_HAS_EXCEPTIONS
    }
    catch (const std::exception& e)
    {
        raise_lua_error(L, "%s", e.what());
    }
#endif

    auto result = UserdataSharedHelper<C, false>::push(L, object);
    if (! result)
        raise_lua_error(L, "%s", result.message().c_str());

    return 1;
}

template <class T, class Args>
int constructor_placement_proxy(lua_State* L)
{
    auto args = make_arguments_list<Args, 2>(L);

    std::error_code ec;
    auto* value = UserdataValue<T>::place(L, ec);
    if (! value)
        raise_lua_error(L, "%s", ec.message().c_str());

#if LUABRIDGE_HAS_EXCEPTIONS
    try
    {
#endif

        constructor<T, Args>::construct(value->getObject(), std::move(args));

#if LUABRIDGE_HAS_EXCEPTIONS
    }
    catch (const std::exception& e)
    {
        raise_lua_error(L, "%s", e.what());
    }
#endif
        
    value->commit();

    return 1;
}

template <class T, class F>
struct constructor_forwarder
{
    explicit constructor_forwarder(F f)
        : m_func(std::move(f))
    {
    }

    T* operator()(lua_State* L)
    {
        using FnTraits = function_traits<F>;
        using FnArgs = remove_first_type_t<typename FnTraits::argument_types>;

        auto args = make_arguments_list<FnArgs, 2>(L);

        std::error_code ec;
        auto* value = UserdataValue<T>::place(L, ec);
        if (! value)
            raise_lua_error(L, "%s", ec.message().c_str());

        T* object = nullptr;
        
#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            object = placement_constructor<T>::construct(value->getObject(), m_func, std::move(args));

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        value->commit();

        return object;
    }

private:
    F m_func;
};

template <class T, class Alloc, class Dealloc>
struct factory_forwarder
{
    explicit factory_forwarder(Alloc alloc, Dealloc dealloc)
        : m_alloc(std::move(alloc))
        , m_dealloc(std::move(dealloc))
    {
    }

    T* operator()(lua_State* L)
    {
        using FnTraits = function_traits<Alloc>;
        using FnArgs = typename FnTraits::argument_types;

        T* object = nullptr;
        
#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            object = external_constructor<T>::construct(m_alloc, make_arguments_list<FnArgs, 0>(L));

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        std::error_code ec;
        auto* value = UserdataValueExternal<T>::place(L, object, m_dealloc, ec);
        if (! value)
            raise_lua_error(L, "%s", ec.message().c_str());

        return object;
    }

private:
    Alloc m_alloc;
    Dealloc m_dealloc;
};

template <class C, class F>
struct container_forwarder
{
    explicit container_forwarder(F f)
        : m_func(std::move(f))
    {
    }

    C operator()(lua_State* L)
    {
        using FnTraits = function_traits<F>;
        using FnArgs = typename FnTraits::argument_types;

        C object;
        
#if LUABRIDGE_HAS_EXCEPTIONS
        try
        {
#endif
            object = container_constructor<C>::construct(m_func, make_arguments_list<FnArgs, 2>(L));

#if LUABRIDGE_HAS_EXCEPTIONS
        }
        catch (const std::exception& e)
        {
            raise_lua_error(L, "%s", e.what());
        }
#endif

        auto result = UserdataSharedHelper<C, false>::push(L, object);
        if (! result)
            raise_lua_error(L, "%s", result.message().c_str());

        return object;
    }

private:
    F m_func;
};

} 
} 


// End File: Source/LuaBridge/detail/CFunctions.h

// Begin File: Source/LuaBridge/detail/Enum.h

namespace luabridge {

template <class T, T... Values>
struct Enum
{
    static_assert(std::is_enum_v<T>);

    using Type = std::underlying_type_t<T>;

    [[nodiscard]] static Result push(lua_State* L, T value)
    {
        return Stack<Type>::push(L, static_cast<Type>(value));
    }

    [[nodiscard]] static TypeResult<T> get(lua_State* L, int index)
    {
        const auto result = Stack<Type>::get(L, index);
        if (! result)
            return result.error();

        if constexpr (sizeof...(Values) > 0)
        {
            constexpr Type values[] = { static_cast<Type>(Values)... };
            for (std::size_t i = 0; i < sizeof...(Values); ++i)
            {
                if (values[i] == *result)
                    return static_cast<T>(*result);
            }

            return makeErrorCode(ErrorCode::InvalidTypeCast);
        }
        else
        {
            return static_cast<T>(*result);
        }
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TNUMBER;
    }
};

} 


// End File: Source/LuaBridge/detail/Enum.h

// Begin File: Source/LuaBridge/detail/Globals.h

namespace luabridge {

template <class T>
TypeResult<T> getGlobal(lua_State* L, const char* name)
{
    lua_getglobal(L, name);

    auto result = luabridge::Stack<T>::get(L, -1);

    lua_pop(L, 1);

    return result;
}

template <class T>
bool setGlobal(lua_State* L, T&& t, const char* name)
{
    if (auto result = push(L, std::forward<T>(t)))
    {
        lua_setglobal(L, name);
        return true;
    }

    return false;
}

} 


// End File: Source/LuaBridge/detail/Globals.h

// Begin File: Source/LuaBridge/detail/LuaRef.h

namespace luabridge {

class LuaResult;

struct LuaNil
{
};

template <>
struct Stack<LuaNil>
{
    [[nodiscard]] static Result push(lua_State* L, const LuaNil&)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        lua_pushnil(L);
        return {};
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_type(L, index) == LUA_TNIL;
    }
};

template <class Impl, class LuaRef>
class LuaRefBase
{
protected:
    friend struct Stack<LuaRef>;

    struct FromStack
    {
    };

    LuaRefBase(lua_State* L) noexcept
        : m_L(L)
    {
        LUABRIDGE_ASSERT(L != nullptr);
    }

    int createRef() const
    {
        impl().push(m_L);

        return luaL_ref(m_L, LUA_REGISTRYINDEX);
    }

public:
    
    std::string tostring() const
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(m_L, 2))
            return {};
#endif

        const StackRestore stackRestore(m_L);

        lua_getglobal(m_L, "tostring");

        impl().push(m_L);

        lua_call(m_L, 1, 1);

        const char* str = lua_tostring(m_L, -1);
        return str != nullptr ? str : "";
    }

    void print(std::ostream& os) const
    {
        switch (type())
        {
        case LUA_TNONE:
        case LUA_TNIL:
            os << "nil";
            break;

        case LUA_TNUMBER:
            os << unsafe_cast<lua_Number>();
            break;

        case LUA_TBOOLEAN:
            os << (unsafe_cast<bool>() ? "true" : "false");
            break;

        case LUA_TSTRING:
            os << '"' << unsafe_cast<const char*>() << '"';
            break;

        case LUA_TTABLE:
        case LUA_TFUNCTION:
        case LUA_TTHREAD:
        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
            os << tostring();
            break;

        default:
            os << "unknown";
            break;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const LuaRefBase& ref)
    {
        ref.print(os);
        return os;
    }

    lua_State* state() const
    {
        return m_L;
    }

    int type() const
    {
        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        return lua_type(m_L, -1);
    }

    bool isNil() const { return type() == LUA_TNIL; }

    bool isBool() const { return type() == LUA_TBOOLEAN; }

    bool isNumber() const { return type() == LUA_TNUMBER; }

    bool isString() const { return type() == LUA_TSTRING; }

    bool isTable() const { return type() == LUA_TTABLE; }

    bool isFunction() const { return type() == LUA_TFUNCTION; }

    bool isUserdata() const { return type() == LUA_TUSERDATA; }

    bool isThread() const { return type() == LUA_TTHREAD; }

    bool isLightUserdata() const { return type() == LUA_TLIGHTUSERDATA; }

    bool isCallable() const
    {
        if (isFunction())
            return true;

        auto metatable = getMetatable();
        return metatable.isTable() && metatable["__call"].isFunction();
    }

    std::optional<std::string> getClassName()
    {
        if (! isUserdata())
            return std::nullopt;

        const StackRestore stackRestore(m_L);

        impl().push(m_L);
        if (! lua_getmetatable(m_L, -1))
            return std::nullopt;

        lua_rawgetp(m_L, -1, detail::getTypeKey());
        if (lua_isstring(m_L, -1))
            return lua_tostring(m_L, -1);

        return std::nullopt;
    }

    template <class T>
    TypeResult<T> cast() const
    {
        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        return Stack<T>::get(m_L, -1);
    }

    template <class T>
    T unsafe_cast() const
    {
        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        return *Stack<T>::get(m_L, -1);
    }

    template <class T>
    bool isInstance() const
    {
        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        return Stack<T>::isInstance(m_L, -1);
    }

    template <class T>
    operator T() const
    {
        return cast<T>().value();
    }

    LuaRef getMetatable() const
    {
        if (isNil())
            return LuaRef(m_L);

        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        if (! lua_getmetatable(m_L, -1))
            return LuaRef(m_L);

        return LuaRef::fromStack(m_L);
    }

    template <class T>
    bool operator==(const T& rhs) const
    {
        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        if (! Stack<T>::push(m_L, rhs))
            return false;

        return lua_compare(m_L, -2, -1, LUA_OPEQ) == 1;
    }

    template <class T>
    bool operator!=(const T& rhs) const
    {
        return !(*this == rhs);
    }

    template <class T>
    bool operator<(const T& rhs) const
    {
        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        if (! Stack<T>::push(m_L, rhs))
            return false;

        const int lhsType = lua_type(m_L, -2);
        const int rhsType = lua_type(m_L, -1);
        if (lhsType != rhsType)
            return lhsType < rhsType;

        return lua_compare(m_L, -2, -1, LUA_OPLT) == 1;
    }

    template <class T>
    bool operator<=(const T& rhs) const
    {
        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        if (! Stack<T>::push(m_L, rhs))
            return false;

        const int lhsType = lua_type(m_L, -2);
        const int rhsType = lua_type(m_L, -1);
        if (lhsType != rhsType)
            return lhsType <= rhsType;

        return lua_compare(m_L, -2, -1, LUA_OPLE) == 1;
    }

    template <class T>
    bool operator>(const T& rhs) const
    {
        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        if (! Stack<T>::push(m_L, rhs))
            return false;

        const int lhsType = lua_type(m_L, -2);
        const int rhsType = lua_type(m_L, -1);
        if (lhsType != rhsType)
            return lhsType > rhsType;

        return lua_compare(m_L, -1, -2, LUA_OPLT) == 1;
    }

    template <class T>
    bool operator>=(const T& rhs) const
    {
        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        if (! Stack<T>::push(m_L, rhs))
            return false;

        const int lhsType = lua_type(m_L, -2);
        const int rhsType = lua_type(m_L, -1);
        if (lhsType != rhsType)
            return lhsType >= rhsType;

        return lua_compare(m_L, -1, -2, LUA_OPLE) == 1;
    }

    template <class T>
    bool rawequal(const T& v) const
    {
        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        if (! Stack<T>::push(m_L, v))
            return false;

        return lua_rawequal(m_L, -1, -2) == 1;
    }

    int length() const
    {
        const StackRestore stackRestore(m_L);

        impl().push(m_L);

        return get_length(m_L, -1);
    }

    template <class... Args>
    LuaResult operator()(Args&&... args) const;

    template <class... Args>
    LuaResult call(Args&&... args) const;

    template <class F, class... Args>
    LuaResult callWithHandler(F&& errorHandler, Args&&... args) const;

protected:
    lua_State* m_L = nullptr;

private:
    const Impl& impl() const { return static_cast<const Impl&>(*this); }

    Impl& impl() { return static_cast<Impl&>(*this); }
};

class LuaRef : public LuaRefBase<LuaRef, LuaRef>
{
    
    class TableItem : public LuaRefBase<TableItem, LuaRef>
    {
        friend class LuaRef;

    public:
        
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

        ~TableItem()
        {
            if (m_keyRef != LUA_NOREF)
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_keyRef);

            if (m_tableRef != LUA_NOREF)
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_tableRef);
        }

        template <class T>
        TableItem& operator=(const T& v)
        {
#if LUABRIDGE_SAFE_STACK_CHECKS
            if (! lua_checkstack(m_L, 2))
                return *this;
#endif

            const StackRestore stackRestore(m_L);

            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_tableRef);
            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_keyRef);

            if (! Stack<T>::push(m_L, v))
                return *this;

            lua_settable(m_L, -3);
            return *this;
        }

        template <class T>
        TableItem& rawset(const T& v)
        {
#if LUABRIDGE_SAFE_STACK_CHECKS
            if (! lua_checkstack(m_L, 2))
                return *this;
#endif

            const StackRestore stackRestore(m_L);

            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_tableRef);
            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_keyRef);

            if (! Stack<T>::push(m_L, v))
                return *this;

            lua_rawset(m_L, -3);
            return *this;
        }

        void push() const
        {
            push(m_L);
        }

        void push(lua_State* L) const
        {
            LUABRIDGE_ASSERT(equalstates(L, m_L));

#if LUABRIDGE_SAFE_STACK_CHECKS
            if (! lua_checkstack(L, 3))
                return;
#endif

            lua_rawgeti(L, LUA_REGISTRYINDEX, m_tableRef);
            lua_rawgeti(L, LUA_REGISTRYINDEX, m_keyRef);
            lua_gettable(L, -2);
            lua_remove(L, -2); 
        }

        template <class T>
        TableItem operator[](const T& key) const
        {
            return LuaRef(*this)[key];
        }

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

    LuaRef(lua_State* L, FromStack) noexcept
        : LuaRefBase(L)
        , m_ref(luaL_ref(m_L, LUA_REGISTRYINDEX))
    {
    }

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

    LuaRef(std::nullptr_t) noexcept = delete;

public:
    
    LuaRef(lua_State* L) noexcept
        : LuaRefBase(L)
        , m_ref(LUA_NOREF)
    {
    }

    template <class T>
    LuaRef(lua_State* L, const T& v)
        : LuaRefBase(L)
        , m_ref(LUA_NOREF)
    {
        if (! Stack<T>::push(m_L, v))
            return;

        m_ref = luaL_ref(m_L, LUA_REGISTRYINDEX);
    }

    LuaRef(const TableItem& v)
        : LuaRefBase(v.state())
        , m_ref(v.createRef())
    {
    }

    LuaRef(const LuaRef& other)
        : LuaRefBase(other.m_L)
        , m_ref(other.createRef())
    {
    }

    LuaRef(LuaRef&& other) noexcept
        : LuaRefBase(other.m_L)
        , m_ref(std::exchange(other.m_ref, LUA_NOREF))
    {
    }

    ~LuaRef()
    {
        if (m_ref != LUA_NOREF)
            luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
    }

    static LuaRef fromStack(lua_State* L)
    {
        return LuaRef(L, FromStack());
    }

    static LuaRef fromStack(lua_State* L, int index)
    {
        return LuaRef(L, index, FromStack());
    }

    static LuaRef newTable(lua_State* L)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return { L };
#endif

        lua_newtable(L);
        return LuaRef(L, FromStack());
    }
    
    template <class F>
    static LuaRef newFunction(lua_State* L, F&& func, const char* debugname = "")
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return { L };
#endif

        detail::push_function(L, std::forward<F>(func), debugname);
        return LuaRef(L, FromStack());
    }

    static LuaRef getGlobal(lua_State* L, const char* name)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return { L };
#endif

        lua_getglobal(L, name);
        return LuaRef(L, FromStack());
    }

    bool isValid() const { return m_ref != LUA_NOREF; }

    LuaRef& operator=(const LuaRef& rhs)
    {
        LuaRef ref(rhs);
        swap(ref);
        return *this;
    }

    LuaRef& operator=(LuaRef&& rhs) noexcept
    {
        if (m_ref != LUA_NOREF)
            luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);

        m_L = rhs.m_L;
        m_ref = std::exchange(rhs.m_ref, LUA_NOREF);

        return *this;
    }

    LuaRef& operator=(const LuaRef::TableItem& rhs)
    {
        LuaRef ref(rhs);
        swap(ref);
        return *this;
    }

    LuaRef& operator=(const LuaNil&)
    {
        LuaRef ref(m_L);
        swap(ref);
        return *this;
    }

    template <class T>
    LuaRef& operator=(const T& rhs)
    {
        LuaRef ref(m_L, rhs);
        swap(ref);
        return *this;
    }

    void push() const
    {
        push(m_L);
    }

    void push(lua_State* L) const
    {
        LUABRIDGE_ASSERT(equalstates(L, m_L));

#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 1))
            return;
#endif

        lua_rawgeti(L, LUA_REGISTRYINDEX, m_ref);
    }

    void pop()
    {
        pop(m_L);
    }

    void pop(lua_State* L)
    {
        LUABRIDGE_ASSERT(equalstates(L, m_L));

        if (m_ref != LUA_NOREF)
            luaL_unref(L, LUA_REGISTRYINDEX, m_ref);

        m_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    void moveTo(lua_State* newL)
    {
        push();

        lua_xmove(m_L, newL, 1);

        m_L = newL;
    }

    template <class T>
    TableItem operator[](const T& key) const
    {
        if (! Stack<T>::push(m_L, key))
            return TableItem(m_L, m_ref);

        return TableItem(m_L, m_ref);
    }

    template <class T>
    LuaRef rawget(const T& key) const
    {
        const StackRestore stackRestore(m_L);

        push(m_L);

        if (! Stack<T>::push(m_L, key))
            return LuaRef(m_L);

        lua_rawget(m_L, -2);
        return LuaRef(m_L, FromStack());
    }

    std::size_t hash() const
    {
        std::size_t value;
        switch (type())
        {
        case LUA_TNONE:
            value = std::hash<std::nullptr_t>{}(nullptr);
            break;

        case LUA_TBOOLEAN:
            value = std::hash<bool>{}(unsafe_cast<bool>());
            break;

        case LUA_TNUMBER:
            value = std::hash<lua_Number>{}(unsafe_cast<lua_Number>());
            break;

        case LUA_TSTRING:
            value = std::hash<const char*>{}(unsafe_cast<const char*>());
            break;

        case LUA_TNIL:
        case LUA_TTABLE:
        case LUA_TFUNCTION:
        case LUA_TTHREAD:
        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
        default:
            value = static_cast<std::size_t>(m_ref);
            break;
        }

        const std::size_t seed = std::hash<int>{}(type());
        return value + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    }

private:
    void swap(LuaRef& other) noexcept
    {
        using std::swap;

        swap(m_L, other.m_L);
        swap(m_ref, other.m_ref);
    }

    int m_ref = LUA_NOREF;
};

template <class T>
auto operator==(const T& lhs, const LuaRef& rhs)
    -> std::enable_if_t<!std::is_same_v<T, LuaRef> && !std::is_same_v<T, LuaRefBase<LuaRef, LuaRef>>, bool>
{
    return rhs == lhs;
}

template <class T>
auto operator!=(const T& lhs, const LuaRef& rhs)
    -> std::enable_if_t<!std::is_same_v<T, LuaRef> && !std::is_same_v<T, LuaRefBase<LuaRef, LuaRef>>, bool>
{
    return !(rhs == lhs);
}

template <class T>
auto operator<(const T& lhs, const LuaRef& rhs)
    -> std::enable_if_t<!std::is_same_v<T, LuaRef> && !std::is_same_v<T, LuaRefBase<LuaRef, LuaRef>>, bool>
{
    return !(rhs >= lhs);
}

template <class T>
auto operator<=(const T& lhs, const LuaRef& rhs)
    -> std::enable_if_t<!std::is_same_v<T, LuaRef> && !std::is_same_v<T, LuaRefBase<LuaRef, LuaRef>>, bool>
{
    return !(rhs > lhs);
}

template <class T>
auto operator>(const T& lhs, const LuaRef& rhs)
    -> std::enable_if_t<!std::is_same_v<T, LuaRef> && !std::is_same_v<T, LuaRefBase<LuaRef, LuaRef>>, bool>
{
    return rhs <= lhs;
}

template <class T>
auto operator>=(const T& lhs, const LuaRef& rhs)
    -> std::enable_if_t<!std::is_same_v<T, LuaRef> && !std::is_same_v<T, LuaRefBase<LuaRef, LuaRef>>, bool>
{
    return !(rhs > lhs);
}

template <>
struct Stack<LuaRef>
{
    [[nodiscard]] static Result push(lua_State* L, const LuaRef& v)
    {
        v.push(L);

        return {};
    }

    [[nodiscard]] static TypeResult<LuaRef> get(lua_State* L, int index)
    {
        return LuaRef::fromStack(L, index);
    }
};

template <>
struct Stack<LuaRef::TableItem>
{
    [[nodiscard]] static Result push(lua_State* L, const LuaRef::TableItem& v)
    {
        v.push(L);

        return {};
    }
};

[[nodiscard]] inline LuaRef newTable(lua_State* L)
{
    return LuaRef::newTable(L);
}

template <class F>
[[nodiscard]] inline LuaRef newFunction(lua_State* L, F&& func)
{
    return LuaRef::newFunction(L, std::forward<F>(func));
}

[[nodiscard]] inline LuaRef getGlobal(lua_State* L, const char* name)
{
    return LuaRef::getGlobal(L, name);
}

template <class T>
[[nodiscard]] TypeResult<T> cast(const LuaRef& ref)
{
    return ref.cast<T>();
}

template <class T>
[[nodiscard]] T unsafe_cast(const LuaRef& ref)
{
    return ref.unsafe_cast<T>();
}
} 

namespace std {
template <>
struct hash<luabridge::LuaRef>
{
    std::size_t operator()(const luabridge::LuaRef& x) const
    {
        return x.hash();
    }
};
} 


// End File: Source/LuaBridge/detail/LuaRef.h

// Begin File: Source/LuaBridge/detail/Invoke.h

namespace luabridge {

class LuaResult
{
public:
    
    explicit operator bool() const noexcept
    {
        return !m_ec;
    }

    bool wasOk() const noexcept
    {
        return !m_ec;
    }

    bool hasFailed() const noexcept
    {
        return !!m_ec;
    }

    std::error_code errorCode() const noexcept
    {
        return m_ec;
    }

    std::string errorMessage() const noexcept
    {
        if (std::holds_alternative<std::string>(m_data))
        {
            const auto& message = std::get<std::string>(m_data);
            return message.empty() ? m_ec.message() : message;
        }

        return {};
    }

    std::size_t size() const noexcept
    {
        if (std::holds_alternative<std::vector<LuaRef>>(m_data))
            return std::get<std::vector<LuaRef>>(m_data).size();

        return 0;
    }

    LuaRef operator[](std::size_t index) const
    {
        LUABRIDGE_ASSERT(m_ec == std::error_code());

        if (std::holds_alternative<std::vector<LuaRef>>(m_data))
        {
            const auto& values = std::get<std::vector<LuaRef>>(m_data);

            LUABRIDGE_ASSERT(index < values.size());
            return values[index];
        }

        return LuaRef(m_L);
    }

private:
    template <class... Args>
    friend LuaResult call(const LuaRef&, Args&&...);

    template <class F, class... Args>
    friend LuaResult callWithHandler(const LuaRef&, F&&, Args&&...);

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

template <class F, class... Args>
LuaResult callWithHandler(const LuaRef& object, F&& errorHandler, Args&&... args)
{
    static constexpr bool isValidHandler = !std::is_same_v<detail::remove_cvref_t<F>, detail::remove_cvref_t<decltype(std::ignore)>>;

    lua_State* L = object.state();
    const int stackTop = lua_gettop(L);

    if constexpr (isValidHandler)
        detail::push_function(L, std::forward<F>(errorHandler), ""); 

    object.push(); 

    {
        const auto [result, index] = detail::push_arguments(L, std::forward_as_tuple(args...)); 
        if (! result)
        {
            lua_pop(L, static_cast<int>(index) + 1);
            return LuaResult(L, result, result.message());
        }
    }

    const int code = lua_pcall(L, sizeof...(Args), LUA_MULTRET, isValidHandler ? (-static_cast<int>(sizeof...(Args)) - 2) : 0);
    if (code != LUABRIDGE_LUA_OK)
    {
        auto ec = makeErrorCode(ErrorCode::LuaFunctionCallFailed);

#if LUABRIDGE_HAS_EXCEPTIONS
        if constexpr (! isValidHandler)
        {
            if (LuaException::areExceptionsEnabled(L))
                LuaException::raise(L, ec);
        }
#endif

        return LuaResult::errorFromStack(L, ec);
    }

    return LuaResult::valuesFromStack(L, stackTop);
}

template <class... Args>
LuaResult call(const LuaRef& object, Args&&... args)
{
    return callWithHandler(object, std::ignore, std::forward<Args>(args)...);
}

inline int pcall(lua_State* L, int nargs = 0, int nresults = 0, int msgh = 0)
{
    const int code = lua_pcall(L, nargs, nresults, msgh);

#if LUABRIDGE_HAS_EXCEPTIONS
    if (code != LUABRIDGE_LUA_OK && LuaException::areExceptionsEnabled(L))
        LuaException::raise(L, makeErrorCode(ErrorCode::LuaFunctionCallFailed));
#endif

    return code;
}

template <class Impl, class LuaRef>
template <class... Args>
LuaResult LuaRefBase<Impl, LuaRef>::operator()(Args&&... args) const
{
    return luabridge::call(*this, std::forward<Args>(args)...);
}

template <class Impl, class LuaRef>
template <class... Args>
LuaResult LuaRefBase<Impl, LuaRef>::call(Args&&... args) const
{
    return luabridge::call(*this, std::forward<Args>(args)...);
}

template <class Impl, class LuaRef>
template <class F, class... Args>
LuaResult LuaRefBase<Impl, LuaRef>::callWithHandler(F&& errorHandler, Args&&... args) const
{
    return luabridge::callWithHandler(*this, std::forward<F>(errorHandler), std::forward<Args>(args)...);
}

} 


// End File: Source/LuaBridge/detail/Invoke.h

// Begin File: Source/LuaBridge/detail/Iterator.h

namespace luabridge {

class Iterator
{
public:
    explicit Iterator(const LuaRef& table, bool isEnd = false)
        : m_L(table.state())
        , m_table(table)
        , m_key(table.state()) 
        , m_value(table.state()) 
    {
        if (! isEnd)
        {
            next(); 
        }
    }

    lua_State* state() const noexcept
    {
        return m_L;
    }

    std::pair<LuaRef, LuaRef> operator*() const
    {
        return std::make_pair(m_key, m_value);
    }

    LuaRef operator->() const
    {
        return m_value;
    }

    bool operator!=(const Iterator& rhs) const
    {
        LUABRIDGE_ASSERT(m_L == rhs.m_L);

        return ! m_table.rawequal(rhs.m_table) || ! m_key.rawequal(rhs.m_key);
    }

    Iterator& operator++()
    {
        if (isNil())
        {
            
            return *this;
        }
        else
        {
            next();
            return *this;
        }
    }

    bool isNil() const noexcept
    {
        return m_key.isNil();
    }

    LuaRef key() const
    {
        return m_key;
    }

    LuaRef value() const
    {
        return m_value;
    }

private:
    
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

inline Range pairs(const LuaRef& table)
{
    return Range{ Iterator(table, false), Iterator(table, true) };
}

} 


// End File: Source/LuaBridge/detail/Iterator.h

// Begin File: Source/LuaBridge/detail/Namespace.h

namespace luabridge {

namespace detail {

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

    Registrar(const Registrar& rhs) = delete;

    Registrar(Registrar&& rhs)
        : L(rhs.L)
        , m_stackSize(std::exchange(rhs.m_stackSize, 0))
        , m_skipStackPops(std::exchange(rhs.m_skipStackPops, 0))
    {
    }

    Registrar& operator=(const Registrar& rhs) = delete;

    Registrar& operator=(Registrar&& rhs)
    {
        m_stackSize = std::exchange(rhs.m_stackSize, 0);
        m_skipStackPops = std::exchange(rhs.m_skipStackPops, 0);

        return *this;
    }

    ~Registrar()
    {
        const int popsCount = m_stackSize - m_skipStackPops;
        if (popsCount > 0)
        {
            LUABRIDGE_ASSERT(popsCount <= lua_gettop(L));

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
    int m_stackSize = 0;
    int m_skipStackPops = 0;
};

} 

class Namespace : public detail::Registrar
{
    
#if 0
    
    static int luaError(lua_State* L, std::string message)
    {
        LUABRIDGE_ASSERT(lua_isstring(L, lua_upvalueindex(1)));
        std::string s;

        lua_Debug ar;

        int result = lua_getstack(L, 2, &ar);
        if (result != 0)
        {
            lua_getinfo(L, "Sl", &ar);
            s = ar.short_src;
            if (ar.currentline != -1)
            {
                
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

    class ClassBase : public detail::Registrar
    {
    public:
        explicit ClassBase(const char* name, Namespace parent)
            : Registrar(std::move(parent))
            , className(name == nullptr ? "" : name)
        {
        }

        using Registrar::operator=;

    protected:
        
        void createConstTable(const char* name, bool trueConst, Options options)
        {
            LUABRIDGE_ASSERT(name != nullptr);

            std::string type_name = std::string(trueConst ? "const " : "") + name;

            lua_newtable(L); 
            lua_pushvalue(L, -1); 
            lua_setmetatable(L, -2); 

            pushunsigned(L, options.toUnderlying()); 
            lua_rawsetp(L, -2, detail::getClassOptionsKey()); 

            lua_pushstring(L, type_name.c_str()); 
            lua_rawsetp(L, -2, detail::getTypeKey()); 

            lua_pushcfunction_x(L, &detail::index_metamethod<true>, "__index"); 
            rawsetfield(L, -2, "__index"); 

            lua_pushcfunction_x(L, &detail::newindex_metamethod<true>, "__newindex"); 
            rawsetfield(L, -2, "__newindex"); 

            lua_newtable(L); 
            lua_rawsetp(L, -2, detail::getPropgetKey()); 

            if (! options.test(visibleMetatables))
            {
                lua_pushboolean(L, 0);
                rawsetfield(L, -2, "__metatable");
            }
        }

        void createClassTable(const char* name, Options options)
        {
            LUABRIDGE_ASSERT(name != nullptr);

            createConstTable(name, false, options); 

            lua_newtable(L); 
            lua_rawsetp(L, -2, detail::getPropsetKey()); 

            lua_pushvalue(L, -2); 
            lua_rawsetp(L, -2, detail::getConstKey()); 

            lua_pushvalue(L, -1); 
            lua_rawsetp(L, -3, detail::getClassKey()); 
        }

        void createStaticTable(const char* name, Options options)
        {
            LUABRIDGE_ASSERT(name != nullptr);

            lua_newtable(L); 
            lua_newtable(L); 
            lua_pushvalue(L, -1); 
            lua_setmetatable(L, -3); 
            lua_insert(L, -2); 
            rawsetfield(L, -5, name); 

            pushunsigned(L, options.toUnderlying()); 
            lua_rawsetp(L, -2, detail::getClassOptionsKey()); 

            lua_pushcfunction_x(L, &detail::index_metamethod<false>, "__index");
            rawsetfield(L, -2, "__index");

            lua_pushcfunction_x(L, &detail::newindex_metamethod<false>, "__newindex");
            rawsetfield(L, -2, "__newindex");

            lua_newtable(L); 
            lua_rawsetp(L, -2, detail::getPropgetKey()); 

            lua_newtable(L); 
            lua_rawsetp(L, -2, detail::getPropsetKey()); 

            lua_pushvalue(L, -2); 
            lua_rawsetp(L, -2, detail::getClassKey()); 

            if (! options.test(visibleMetatables))
            {
                lua_pushboolean(L, 0);
                rawsetfield(L, -2, "__metatable");
            }
        }

        void assertStackState() const
        {
            
            LUABRIDGE_ASSERT(lua_istable(L, -3));
            LUABRIDGE_ASSERT(lua_istable(L, -2));
            LUABRIDGE_ASSERT(lua_istable(L, -1));
        }

        const char* className = "";
    };

    template <class T>
    class Class : public ClassBase
    {
    public:
        
        Class(const char* name, Namespace parent, Options options)
            : ClassBase(name, std::move(parent))
        {
            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(lua_istable(L, -1)); 

            rawgetfield(L, -1, name); 

            if (lua_isnil(L, -1)) 
            {
                lua_pop(L, 1); 

                createConstTable(name, true, options); 
                ++m_stackSize;
#if !defined(LUABRIDGE_ON_LUAU)
                lua_pushcfunction_x(L, &detail::gc_metamethod<T>, "__gc"); 
                rawsetfield(L, -2, "__gc"); 
#endif
                lua_pushcfunction_x(L, &detail::tostring_metamethod<T>, "__tostring");
                rawsetfield(L, -2, "__tostring");

                createClassTable(name, options); 
                ++m_stackSize;
#if !defined(LUABRIDGE_ON_LUAU)
                lua_pushcfunction_x(L, &detail::gc_metamethod<T>, "__gc"); 
                rawsetfield(L, -2, "__gc"); 
#endif

                lua_pushcfunction_x(L, &detail::tostring_metamethod<T>, "__tostring");
                rawsetfield(L, -2, "__tostring");

                createStaticTable(name, options); 
                ++m_stackSize;

                lua_pushvalue(L, -1); 
                lua_rawsetp(L, -2, detail::getStaticKey()); 
                lua_pushvalue(L, -1); 
                lua_rawsetp(L, -3, detail::getStaticKey()); 

                lua_pushvalue(L, -1); 
                lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getStaticRegistryKey<T>()); 
                lua_pushvalue(L, -2); 
                lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getClassRegistryKey<T>()); 
                lua_pushvalue(L, -3); 
                lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getConstRegistryKey<T>()); 
            }
            else
            {
                LUABRIDGE_ASSERT(lua_istable(L, -1)); 
                ++m_stackSize;

                lua_getmetatable(L, -1); 
                lua_insert(L, -2); 
                lua_pop(L, 1); 

                lua_rawgetp(L, LUA_REGISTRYINDEX, detail::getConstRegistryKey<T>()); 
                lua_insert(L, -2); 
                ++m_stackSize;

                lua_rawgetp(L, LUA_REGISTRYINDEX, detail::getClassRegistryKey<T>()); 
                lua_insert(L, -2); 
                ++m_stackSize;
            }
        }

        Class(const char* name, Namespace parent, const void* const staticKey, Options options)
            : ClassBase(name, std::move(parent))
        {
            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(lua_istable(L, -1)); 

            createConstTable(name, true, options); 
            ++m_stackSize;
#if !defined(LUABRIDGE_ON_LUAU)
            lua_pushcfunction_x(L, &detail::gc_metamethod<T>, "__gc"); 
            rawsetfield(L, -2, "__gc"); 
#endif
            lua_pushcfunction_x(L, &detail::tostring_metamethod<T>, "__tostring");
            rawsetfield(L, -2, "__tostring");

            createClassTable(name, options); 
            ++m_stackSize;
#if !defined(LUABRIDGE_ON_LUAU)
            lua_pushcfunction_x(L, &detail::gc_metamethod<T>, "__gc"); 
            rawsetfield(L, -2, "__gc"); 
#endif
            lua_pushcfunction_x(L, &detail::tostring_metamethod<T>, "__tostring");
            rawsetfield(L, -2, "__tostring");

            createStaticTable(name, options); 
            ++m_stackSize;

            lua_pushvalue(L, -1); 
            lua_rawsetp(L, -2, detail::getStaticKey()); 
            lua_pushvalue(L, -1); 
            lua_rawsetp(L, -3, detail::getStaticKey()); 

            lua_rawgetp(L, LUA_REGISTRYINDEX, staticKey); 
            if (lua_isnil(L, -1)) 
            {
                lua_pop(L, 1);

                throw_or_assert<std::logic_error>("Base class is not registered");
                return;
            }

            LUABRIDGE_ASSERT(lua_istable(L, -1)); 

            lua_rawgetp(L, -1, detail::getClassKey()); 
            LUABRIDGE_ASSERT(lua_istable(L, -1));

            lua_rawgetp(L, -1, detail::getConstKey()); 
            LUABRIDGE_ASSERT(lua_istable(L, -1));

            lua_rawsetp(L, -6, detail::getParentKey()); 
            lua_rawsetp(L, -4, detail::getParentKey()); 
            lua_rawsetp(L, -2, detail::getParentKey()); 

            lua_pushvalue(L, -1); 
            lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getStaticRegistryKey<T>()); 
            lua_pushvalue(L, -2); 
            lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getClassRegistryKey<T>()); 
            lua_pushvalue(L, -3); 
            lua_rawsetp(L, LUA_REGISTRYINDEX, detail::getConstRegistryKey<T>()); 
        }

        Namespace endClass()
        {
            LUABRIDGE_ASSERT(m_stackSize > 3);

            m_stackSize -= 3;
            lua_pop(L, 3);

            return Namespace(std::move(*this));
        }

        template <class U, class = std::enable_if_t<std::is_base_of_v<U, LuaRef> || !std::is_invocable_v<U>>>
        Class<T>& addStaticProperty(const char* name, const U* value)
        {
            LUABRIDGE_ASSERT(name != nullptr);
            assertStackState(); 

            lua_pushlightuserdata(L, const_cast<U*>(value)); 
            lua_pushcclosure_x(L, &detail::property_getter<U>::call, name, 1); 
            detail::add_property_getter(L, name, -2); 

            lua_pushstring(L, name); 
            lua_pushcclosure_x(L, &detail::read_only_error, name, 1); 

            detail::add_property_setter(L, name, -2); 

            return *this;
        }

        template <class U, class = std::enable_if_t<std::is_base_of_v<U, LuaRef> || !std::is_invocable_v<U>>>
        Class<T>& addStaticProperty(const char* name, U* value, bool isWritable = true)
        {
            LUABRIDGE_ASSERT(name != nullptr);
            assertStackState(); 

            lua_pushlightuserdata(L, value); 
            lua_pushcclosure_x(L, &detail::property_getter<U>::call, name, 1); 
            detail::add_property_getter(L, name, -2); 

            if (isWritable)
            {
                lua_pushlightuserdata(L, value); 
                lua_pushcclosure_x(L, &detail::property_setter<U>::call, name, 1); 
            }
            else
            {
                lua_pushstring(L, name); 
                lua_pushcclosure_x(L, &detail::read_only_error, name, 1); 
            }

            detail::add_property_setter(L, name, -2); 

            return *this;
        }

        template <class U>
        Class<T>& addStaticProperty(const char* name, U (*get)(), void (*set)(U) = nullptr)
        {
            LUABRIDGE_ASSERT(name != nullptr);
            assertStackState(); 

            lua_pushlightuserdata(L, reinterpret_cast<void*>(get)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<U (*)()>, name, 1); 
            detail::add_property_getter(L, name, -2); 

            if (set != nullptr)
            {
                lua_pushlightuserdata(L, reinterpret_cast<void*>(set)); 
                lua_pushcclosure_x(L, &detail::invoke_proxy_function<void (*)(U)>, name, 1); 
            }
            else
            {
                lua_pushstring(L, name); 
                lua_pushcclosure_x(L, &detail::read_only_error, name, 1); 
            }

            detail::add_property_setter(L, name, -2); 

            return *this;
        }

        template <class U>
        Class<T>& addStaticProperty(const char* name, U (*get)() noexcept, void (*set)(U) noexcept = nullptr)
        {
            LUABRIDGE_ASSERT(name != nullptr);
            assertStackState(); 

            lua_pushlightuserdata(L, reinterpret_cast<void*>(get)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<U (*)() noexcept>, name, 1); 
            detail::add_property_getter(L, name, -2); 

            if (set != nullptr)
            {
                lua_pushlightuserdata(L, reinterpret_cast<void*>(set)); 
                lua_pushcclosure_x(L, &detail::invoke_proxy_function<void (*)(U) noexcept>, name, 1); 
            }
            else
            {
                lua_pushstring(L, name); 
                lua_pushcclosure_x(L, &detail::read_only_error, name, 1); 
            }

            detail::add_property_setter(L, name, -2); 

            return *this;
        }

        template <class Getter, class = std::enable_if_t<!std::is_pointer_v<Getter>>>
        Class<T>& addStaticProperty(const char* name, Getter get)
        {
            LUABRIDGE_ASSERT(name != nullptr);
            assertStackState(); 

            using GetType = decltype(get);

            lua_newuserdata_aligned<GetType>(L, std::move(get)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<GetType>, name, 1); 
            detail::add_property_getter(L, name, -2); 

            return *this;
        }

        template <class Getter, class Setter, class = std::enable_if_t<!std::is_pointer_v<Getter> && !std::is_pointer_v<Setter>>>
        Class<T>& addStaticProperty(const char* name, Getter get, Setter set)
        {
            LUABRIDGE_ASSERT(name != nullptr);
            assertStackState(); 

            using GetType = decltype(get);
            using SetType = decltype(set);

            lua_newuserdata_aligned<GetType>(L, std::move(get)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<GetType>, name, 1); 
            detail::add_property_getter(L, name, -2); 

            lua_newuserdata_aligned<SetType>(L, std::move(set)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<SetType>, name, 1); 
            detail::add_property_setter(L, name, -2); 

            return *this;
        }

        template <class... Functions>
        auto addStaticFunction(const char* name, Functions... functions)
            -> std::enable_if_t<(detail::is_callable_v<Functions> && ...) && (sizeof...(Functions) > 0), Class<T>&>
        {
            LUABRIDGE_ASSERT(name != nullptr);
            assertStackState(); 

            if constexpr (sizeof...(Functions) == 1)
            {
                ([&]
                {
                    detail::push_function(L, std::move(functions), name);

                } (), ...);
            }
            else
            {
                
                lua_createtable(L, static_cast<int>(sizeof...(Functions)), 0); 

                int idx = 1;

                ([&]
                {
                    lua_createtable(L, 2, 0); 
                    lua_pushinteger(L, 1);
                    if constexpr (detail::is_any_cfunction_pointer_v<Functions>)
                        lua_pushinteger(L, -1);
                    else
                        lua_pushinteger(L, static_cast<int>(detail::function_arity_excluding_v<Functions, lua_State*>));
                    lua_settable(L, -3);
                    lua_pushinteger(L, 2);
                    detail::push_function(L, std::move(functions), name);
                    lua_settable(L, -3);

                    lua_rawseti(L, -2, idx);
                    ++idx;

                } (), ...);

                lua_pushcclosure_x(L, &detail::try_overload_functions<false>, name, 1);
            }

            rawsetfield(L, -2, name);

            return *this;
        }

        template <class U, class V>
        Class<T>& addProperty(const char* name, U V::*mp, bool isWritable = true)
        {
            static_assert(std::is_base_of_v<V, T>);

            using MemberPtrType = decltype(mp);

            LUABRIDGE_ASSERT(name != nullptr);
            assertStackState(); 

            new (lua_newuserdata_x<MemberPtrType>(L, sizeof(MemberPtrType))) MemberPtrType(mp); 
            lua_pushcclosure_x(L, &detail::property_getter<U, T>::call, name, 1); 
            lua_pushvalue(L, -1); 
            detail::add_property_getter(L, name, -5); 
            detail::add_property_getter(L, name, -3); 

            if (isWritable)
            {
                new (lua_newuserdata_x<MemberPtrType>(L, sizeof(MemberPtrType))) MemberPtrType(mp); 
                lua_pushcclosure_x(L, &detail::property_setter<U, T>::call, name, 1); 
                detail::add_property_setter(L, name, -3); 
            }

            return *this;
        }

        template <class TG>
        Class<T>& addProperty(const char* name, TG (T::*get)() const)
        {
            using GetType = TG (T::*)() const;

            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            assertStackState(); 

            new (lua_newuserdata_x<GetType>(L, sizeof(GetType))) GetType(get); 
            lua_pushcclosure_x(L, &detail::invoke_const_member_function<GetType, T>, name, 1); 
            lua_pushvalue(L, -1); 
            detail::add_property_getter(L, name, -5); 
            detail::add_property_getter(L, name, -3); 

            return *this;
        }

        template <class TG, class TS = TG>
        Class<T>& addProperty(const char* name, TG (T::*get)() const, void (T::*set)(TS))
        {
            using SetType = void (T::*)(TS);

            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            LUABRIDGE_ASSERT(set != nullptr);
            assertStackState(); 

            addProperty(name, get);

            new (lua_newuserdata_x<SetType>(L, sizeof(SetType))) SetType(set); 
            lua_pushcclosure_x(L, &detail::invoke_member_function<SetType, T>, name, 1); 
            detail::add_property_setter(L, name, -3); 

            return *this;
        }

        template <class TG>
        Class<T>& addProperty(const char* name, TG (T::*get)() const noexcept)
        {
            using GetType = TG (T::*)() const noexcept;

            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            assertStackState(); 

            new (lua_newuserdata_x<GetType>(L, sizeof(GetType))) GetType(get); 
            lua_pushcclosure_x(L, &detail::invoke_const_member_function<GetType, T>, name, 1); 
            lua_pushvalue(L, -1); 
            detail::add_property_getter(L, name, -5); 
            detail::add_property_getter(L, name, -3); 

            return *this;
        }

        template <class TG, class TS = TG>
        Class<T>& addProperty(const char* name, TG (T::*get)() const noexcept, void (T::*set)(TS) noexcept)
        {
            using SetType = void (T::*)(TS) noexcept;

            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            LUABRIDGE_ASSERT(set != nullptr);
            assertStackState(); 

            addProperty(name, get);

            new (lua_newuserdata_x<SetType>(L, sizeof(SetType))) SetType(set); 
            lua_pushcclosure_x(L, &detail::invoke_member_function<SetType, T>, name, 1); 
            detail::add_property_setter(L, name, -3); 

            return *this;
        }

        template <class TG>
        Class<T>& addProperty(const char* name, TG (T::*get)(lua_State*) const)
        {
            using GetType = TG (T::*)(lua_State*) const;

            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            assertStackState(); 

            new (lua_newuserdata_x<GetType>(L, sizeof(GetType))) GetType(get); 
            lua_pushcclosure_x(L, &detail::invoke_const_member_function<GetType, T>, name, 1); 
            lua_pushvalue(L, -1); 
            detail::add_property_getter(L, name, -5); 
            detail::add_property_getter(L, name, -3); 

            return *this;
        }

        template <class TG, class TS = TG>
        Class<T>& addProperty(const char* name, TG (T::*get)(lua_State*) const, void (T::*set)(TS, lua_State*))
        {
            using SetType = void (T::*)(TS, lua_State*);

            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            LUABRIDGE_ASSERT(set != nullptr);
            assertStackState(); 

            addProperty(name, get);

            new (lua_newuserdata_x<SetType>(L, sizeof(SetType))) SetType(set); 
            lua_pushcclosure_x(L, &detail::invoke_member_function<SetType, T>, name, 1); 
            detail::add_property_setter(L, name, -3); 

            return *this;
        }

        template <class TG>
        Class<T>& addProperty(const char* name, TG (T::*get)(lua_State*) const noexcept)
        {
            using GetType = TG (T::*)(lua_State*) const noexcept;

            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            assertStackState(); 

            new (lua_newuserdata_x<GetType>(L, sizeof(GetType))) GetType(get); 
            lua_pushcclosure_x(L, &detail::invoke_const_member_function<GetType, T>, name, 1); 
            lua_pushvalue(L, -1); 
            detail::add_property_getter(L, name, -5); 
            detail::add_property_getter(L, name, -3); 

            return *this;
        }

        template <class TG, class TS = TG>
        Class<T>& addProperty(const char* name, TG (T::*get)(lua_State*) const noexcept, void (T::*set)(TS, lua_State*) noexcept)
        {
            using SetType = void (T::*)(TS, lua_State*) noexcept;

            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            LUABRIDGE_ASSERT(set != nullptr);
            assertStackState(); 

            addProperty(name, get);

            new (lua_newuserdata_x<SetType>(L, sizeof(SetType))) SetType(set); 
            lua_pushcclosure_x(L, &detail::invoke_member_function<SetType, T>, name, 1); 
            detail::add_property_setter(L, name, -3); 

            return *this;
        }

        template <class TG>
        Class<T>& addProperty(const char* name, TG (*get)(const T*))
        {
            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            assertStackState(); 

            lua_pushlightuserdata(L, reinterpret_cast<void*>(get)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<TG (*)(const T*)>, name, 1); 
            lua_pushvalue(L, -1); 
            detail::add_property_getter(L, name, -5); 
            detail::add_property_getter(L, name, -3); 

            return *this;
        }

        template <class TG, class TS = TG>
        Class<T>& addProperty(const char* name, TG (*get)(const T*), void (*set)(T*, TS))
        {
            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            LUABRIDGE_ASSERT(set != nullptr);
            assertStackState(); 

            addProperty(name, get);

            lua_pushlightuserdata( L, reinterpret_cast<void*>(set)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<void (*)(T*, TS)>, name, 1); 
            detail::add_property_setter(L, name, -3); 

            return *this;
        }

        template <class TG>
        Class<T>& addProperty(const char* name, TG (*get)(const T*) noexcept)
        {
            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            assertStackState(); 

            lua_pushlightuserdata(L, reinterpret_cast<void*>(get)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<TG (*)(const T*) noexcept>, name, 1); 
            lua_pushvalue(L, -1); 
            detail::add_property_getter(L, name, -5); 
            detail::add_property_getter(L, name, -3); 

            return *this;
        }

        template <class TG, class TS = TG>
        Class<T>& addProperty(const char* name, TG (*get)(const T*) noexcept, void (*set)(T*, TS) noexcept)
        {
            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            LUABRIDGE_ASSERT(set != nullptr);
            assertStackState(); 

            addProperty(name, get);

            lua_pushlightuserdata( L, reinterpret_cast<void*>(set)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<void (*)(T*, TS) noexcept>, name, 1); 
            detail::add_property_setter(L, name, -3); 

            return *this;
        }

        Class<T>& addProperty(const char* name, lua_CFunction get)
        {
            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            assertStackState(); 

            lua_pushcfunction_x(L, get, name);
            lua_pushvalue(L, -1); 
            detail::add_property_getter(L, name, -5); 
            detail::add_property_getter(L, name, -3); 

            return *this;
        }

        Class<T>& addProperty(const char* name, lua_CFunction get, lua_CFunction set)
        {
            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(get != nullptr);
            LUABRIDGE_ASSERT(set != nullptr);
            assertStackState(); 

            addProperty(name, get);

            lua_pushcfunction_x(L, set, name);
            detail::add_property_setter(L, name, -3); 

            return *this;
        }

        template <class Getter, class = std::enable_if_t<!std::is_pointer_v<Getter>>>
        Class<T>& addProperty(const char* name, Getter get)
        {
            using FirstArg = detail::function_argument_t<0, Getter>;
            static_assert(std::is_same_v<std::decay_t<std::remove_pointer_t<FirstArg>>, T>);

            LUABRIDGE_ASSERT(name != nullptr);
            assertStackState(); 

            using GetType = decltype(get);

            lua_newuserdata_aligned<GetType>(L, std::move(get)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<GetType>, name, 1); 
            lua_pushvalue(L, -1); 
            detail::add_property_getter(L, name, -4); 
            detail::add_property_getter(L, name, -4); 

            return *this;
        }

        template <class Getter, class Setter, class = std::enable_if_t<!std::is_pointer_v<Getter> && !std::is_pointer_v<Setter>>>
        Class<T>& addProperty(const char* name, Getter get, Setter set)
        {
            addProperty<Getter>(name, std::move(get));

            using FirstArg = detail::function_argument_t<0, Setter>;
            static_assert(std::is_same_v<std::decay_t<std::remove_pointer_t<FirstArg>>, T>);

            LUABRIDGE_ASSERT(name != nullptr);
            assertStackState(); 

            using SetType = decltype(set);

            lua_newuserdata_aligned<SetType>(L, std::move(set)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<SetType>, name, 1); 
            detail::add_property_setter(L, name, -3); 

            return *this;
        }

        template <class... Functions>
        auto addFunction(const char* name, Functions... functions)
            -> std::enable_if_t<(detail::is_callable_v<Functions> && ...) && (sizeof...(Functions) > 0), Class<T>&>
        {
            LUABRIDGE_ASSERT(name != nullptr);
            assertStackState(); 

            if (name == std::string_view("__gc"))
            {
                throw_or_assert<std::logic_error>("__gc metamethod registration is forbidden");
                return *this;
            }

            if constexpr (sizeof...(Functions) == 1)
            {
                ([&]
                {
                    detail::push_member_function<T>(L, std::move(functions), name);

                } (), ...);

                if constexpr (detail::const_functions_count<T, Functions...> == 1)
                {
                    lua_pushvalue(L, -1); 
                    rawsetfield(L, -4, name); 
                    rawsetfield(L, -4, name); 
                }
                else
                {
                    rawsetfield(L, -3, name); 
                }
            }
            else
            {
                
                if constexpr (detail::const_functions_count<T, Functions...> > 0)
                {
                    lua_createtable(L, static_cast<int>(detail::const_functions_count<T, Functions...>), 0);

                    int idx = 1;

                    ([&]
                    {
                        if (!detail::is_const_function<T, Functions>)
                            return;

                        lua_createtable(L, 2, 0); 
                        lua_pushinteger(L, 1);
                        if constexpr (detail::is_any_cfunction_pointer_v<Functions>)
                            lua_pushinteger(L, -1);
                        else
                            lua_pushinteger(L, static_cast<int>(detail::member_function_arity_excluding_v<T, Functions, lua_State*>));
                        lua_settable(L, -3);
                        lua_pushinteger(L, 2);
                        detail::push_member_function<T>(L, std::move(functions), name);
                        lua_settable(L, -3);

                        lua_rawseti(L, -2, idx);
                        ++idx;

                    } (), ...);

                    LUABRIDGE_ASSERT(idx > 1);

                    lua_pushcclosure_x(L, &detail::try_overload_functions<true>, name, 1);
                    lua_pushvalue(L, -1); 
                    rawsetfield(L, -4, name); 
                    rawsetfield(L, -4, name); 
                }

                if constexpr (detail::non_const_functions_count<T, Functions...> > 0)
                {
                    lua_createtable(L, static_cast<int>(detail::non_const_functions_count<T, Functions...>), 0);

                    int idx = 1;

                    ([&]
                    {
                        if (detail::is_const_function<T, Functions>)
                            return;

                        lua_createtable(L, 2, 0); 
                        lua_pushinteger(L, 1);
                        if constexpr (detail::is_any_cfunction_pointer_v<Functions>)
                            lua_pushinteger(L, -1);
                        else
                            lua_pushinteger(L, static_cast<int>(detail::member_function_arity_excluding_v<T, Functions, lua_State*>));
                        lua_settable(L, -3);
                        lua_pushinteger(L, 2);
                        detail::push_member_function<T>(L, std::move(functions), name);
                        lua_settable(L, -3);

                        lua_rawseti(L, -2, idx);
                        ++idx;

                    } (), ...);

                    LUABRIDGE_ASSERT(idx > 1);

                    lua_pushcclosure_x(L, &detail::try_overload_functions<true>, name, 1);
                    rawsetfield(L, -3, name); 
                }
            }

            return *this;
        }

        template <class... Functions>
        auto addConstructor()
            -> std::enable_if_t<(sizeof...(Functions) > 0), Class<T>&>
        {
            assertStackState(); 

            if constexpr (sizeof...(Functions) == 1)
            {
                ([&]
                {
                    lua_pushcclosure_x(L, &detail::constructor_placement_proxy<T, detail::function_arguments_t<Functions>>, className, 0);

                } (), ...);
            }
            else
            {
                
                lua_createtable(L, static_cast<int>(sizeof...(Functions)), 0); 

                int idx = 1;

                ([&]
                {
                    lua_createtable(L, 2, 0); 
                    lua_pushinteger(L, 1);
                    lua_pushinteger(L, static_cast<int>(detail::function_arity_excluding_v<Functions, lua_State*>));
                    lua_settable(L, -3);
                    lua_pushinteger(L, 2);
                    lua_pushcclosure_x(L, &detail::constructor_placement_proxy<T, detail::function_arguments_t<Functions>>, className, 0);
                    lua_settable(L, -3);
                    lua_rawseti(L, -2, idx);
                    ++idx;

                } (), ...);

                lua_pushcclosure_x(L, &detail::try_overload_functions<true>, className, 1);
            }

            rawsetfield(L, -2, "__call");

            return *this;
        }

        template <class... Functions>
        auto addConstructor(Functions... functions)
            -> std::enable_if_t<(detail::is_callable_v<Functions> && ...) && (sizeof...(Functions) > 0), Class<T>&>
        {
            static_assert(((detail::function_arity_excluding_v<Functions, lua_State*> >= 1) && ...));
            static_assert(((std::is_same_v<detail::function_argument_t<0, Functions>, void*>) && ...));

            assertStackState(); 

            if constexpr (sizeof...(Functions) == 1)
            {
                ([&]
                {
                    using F = detail::constructor_forwarder<T, Functions>;

                    lua_newuserdata_aligned<F>(L, F(std::move(functions))); 
                    lua_pushcclosure_x(L, &detail::invoke_proxy_constructor<F>, className, 1); 

                } (), ...);
            }
            else
            {
                
                lua_createtable(L, static_cast<int>(sizeof...(Functions)), 0); 

                int idx = 1;

                ([&]
                {
                    using F = detail::constructor_forwarder<T, Functions>;

                    lua_createtable(L, 2, 0); 
                    lua_pushinteger(L, 1);
                    if constexpr (detail::is_any_cfunction_pointer_v<Functions>)
                        lua_pushinteger(L, -1);
                    else
                        lua_pushinteger(L, static_cast<int>(detail::function_arity_excluding_v<Functions, lua_State*>) - 1); 
                    lua_settable(L, -3);
                    lua_pushinteger(L, 2);
                    lua_newuserdata_aligned<F>(L, F(std::move(functions)));
                    lua_pushcclosure_x(L, &detail::invoke_proxy_constructor<F>, className, 1);
                    lua_settable(L, -3);
                    lua_rawseti(L, -2, idx);
                    ++idx;

                } (), ...);

                lua_pushcclosure_x(L, &detail::try_overload_functions<true>, className, 1);
            }

            rawsetfield(L, -2, "__call"); 

            return *this;
        }

        template <class C, class... Functions>
        auto addConstructorFrom()
            -> std::enable_if_t<(sizeof...(Functions) > 0), Class<T>&>
        {
            assertStackState(); 

            if constexpr (sizeof...(Functions) == 1)
            {
                ([&]
                {
                    lua_pushcclosure_x(L, &detail::constructor_container_proxy<C, detail::function_arguments_t<Functions>>, className, 0);

                } (), ...);
            }
            else
            {
                
                lua_createtable(L, static_cast<int>(sizeof...(Functions)), 0); 

                int idx = 1;

                ([&]
                {
                    lua_createtable(L, 2, 0); 
                    lua_pushinteger(L, 1);
                    lua_pushinteger(L, static_cast<int>(detail::function_arity_excluding_v<Functions, lua_State*>));
                    lua_settable(L, -3);
                    lua_pushinteger(L, 2);
                    lua_pushcclosure_x(L, &detail::constructor_container_proxy<C, detail::function_arguments_t<Functions>>, className, 0);
                    lua_settable(L, -3);
                    lua_rawseti(L, -2, idx);
                    ++idx;

                } (), ...);

                lua_pushcclosure_x(L, &detail::try_overload_functions<true>, className, 1);
            }

            rawsetfield(L, -2, "__call");

            return *this;
        }

        template <class C, class... Functions>
        auto addConstructorFrom(Functions... functions)
            -> std::enable_if_t<(detail::is_callable_v<Functions> && ...) && (sizeof...(Functions) > 0), Class<T>&>
        {
            static_assert(((std::is_same_v<detail::function_result_t<Functions>, C>) && ...));

            assertStackState(); 

            if constexpr (sizeof...(Functions) == 1)
            {
                ([&]
                {
                    using F = detail::container_forwarder<C, Functions>;

                    lua_newuserdata_aligned<F>(L, F(std::move(functions))); 
                    lua_pushcclosure_x(L, &detail::invoke_proxy_constructor<F>, className, 1); 

                } (), ...);
            }
            else
            {
                
                lua_createtable(L, static_cast<int>(sizeof...(Functions)), 0); 

                int idx = 1;

                ([&]
                {
                    using F = detail::container_forwarder<C, Functions>;

                    lua_createtable(L, 2, 0); 
                    lua_pushinteger(L, 1);
                    if constexpr (detail::is_any_cfunction_pointer_v<Functions>)
                        lua_pushinteger(L, -1);
                    else
                        lua_pushinteger(L, static_cast<int>(detail::function_arity_excluding_v<Functions, lua_State*>));
                    lua_settable(L, -3);
                    lua_pushinteger(L, 2);
                    lua_newuserdata_aligned<F>(L, F(std::move(functions)));
                    lua_pushcclosure_x(L, &detail::invoke_proxy_constructor<F>, className, 1);
                    lua_settable(L, -3);
                    lua_rawseti(L, -2, idx);
                    ++idx;

                } (), ...);

                lua_pushcclosure_x(L, &detail::try_overload_functions<true>, className, 1);
            }

            rawsetfield(L, -2, "__call"); 

            return *this;
        }

        template <class Allocator, class Deallocator>
        Class<T>& addFactory(Allocator allocator, Deallocator deallocator)
        {
            assertStackState(); 

            using F = detail::factory_forwarder<T, Allocator, Deallocator>;

            lua_newuserdata_aligned<F>(L, F(std::move(allocator), std::move(deallocator))); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_constructor<F>, className, 1); 
            rawsetfield(L, -2, "__call"); 

            return *this;
        }

        template <class Function>
        auto addIndexMetaMethod(Function function)
            -> std::enable_if_t<!std::is_pointer_v<Function>
                && std::is_invocable_v<Function, T&, const LuaRef&, lua_State*>, Class<T>&>
        {
            using FnType = decltype(function);

            assertStackState(); 

            lua_newuserdata_aligned<FnType>(L, std::move(function)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<FnType>, "__index", 1); 
            lua_rawsetp(L, -3, detail::getIndexFallbackKey());

            return *this;
        }

        Class<T>& addIndexMetaMethod(LuaRef (*idxf)(T&, const LuaRef&, lua_State*))
        {
            using FnType = decltype(idxf);

            assertStackState(); 

            lua_pushlightuserdata(L, reinterpret_cast<void*>(idxf)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<FnType>, "__index", 1); 
            lua_rawsetp(L, -3, detail::getIndexFallbackKey());

            return *this;
        }

        Class<T>& addIndexMetaMethod(LuaRef (T::* idxf)(const LuaRef&, lua_State*))
        {
            using MemFnPtr = decltype(idxf);

            assertStackState(); 

            new (lua_newuserdata_x<MemFnPtr>(L, sizeof(MemFnPtr))) MemFnPtr(idxf);
            lua_pushcclosure_x(L, &detail::invoke_member_function<MemFnPtr, T>, "__index", 1);
            lua_rawsetp(L, -3, detail::getIndexFallbackKey());

            return *this;
        }

        template <class Function>
        auto addNewIndexMetaMethod(Function function)
            -> std::enable_if_t<!std::is_pointer_v<Function>
                && std::is_invocable_v<Function, T&, const LuaRef&, const LuaRef&, lua_State*>, Class<T>&>
        {
            using FnType = decltype(function);

            assertStackState(); 

            lua_newuserdata_aligned<FnType>(L, std::move(function)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<FnType>, "__newindex", 1); 
            lua_rawsetp(L, -3, detail::getNewIndexFallbackKey());

            return *this;
        }

        Class<T>& addNewIndexMetaMethod(LuaRef (*idxf)(T&, const LuaRef&, const LuaRef&, lua_State*))
        {
            using FnType = decltype(idxf);

            assertStackState(); 

            lua_pushlightuserdata(L, reinterpret_cast<void*>(idxf)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_function<FnType>, "__newindex", 1); 
            lua_rawsetp(L, -3, detail::getNewIndexFallbackKey());

            return *this;
        }

        Class<T>& addNewIndexMetaMethod(LuaRef (T::* idxf)(const LuaRef&, const LuaRef&, lua_State*))
        {
            using MemFnPtr = decltype(idxf);

            assertStackState(); 

            new (lua_newuserdata_x<MemFnPtr>(L, sizeof(MemFnPtr))) MemFnPtr(idxf);
            lua_pushcclosure_x(L, &detail::invoke_member_function<MemFnPtr, T>, "__newindex", 1);
            lua_rawsetp(L, -3, detail::getNewIndexFallbackKey());

            return *this;
        }
    };

    class Table : public detail::Registrar
    {
    public:
        explicit Table(const char* name, Namespace parent)
            : Registrar(std::move(parent))
        {
            lua_newtable(L); 
            lua_pushvalue(L, -1); 
            rawsetfield(L, -3, name);
            ++m_stackSize;

            lua_newtable(L); 
            lua_pushvalue(L, -1); 
            lua_setmetatable(L, -3); 
            ++m_stackSize;
        }

        using Registrar::operator=;

        template <class Function>
        Table& addFunction(const char* name, Function function)
        {
            using FnType = decltype(function);

            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(lua_istable(L, -1)); 

            lua_newuserdata_aligned<FnType>(L, std::move(function)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<FnType>, name, 1); 
            rawsetfield(L, -3, name); 

            return *this;
        }

        template <class Function>
        Table& addMetaFunction(const char* name, Function function)
        {
            using FnType = decltype(function);

            LUABRIDGE_ASSERT(name != nullptr);
            LUABRIDGE_ASSERT(lua_istable(L, -1)); 

            lua_newuserdata_aligned<FnType>(L, std::move(function)); 
            lua_pushcclosure_x(L, &detail::invoke_proxy_functor<FnType>, name, 1); 
            rawsetfield(L, -2, name); 

            return *this;
        }

        Namespace endTable()
        {
            LUABRIDGE_ASSERT(m_stackSize > 2);

            m_stackSize -= 2;
            lua_pop(L, 2);

            return Namespace(std::move(*this));
        }
    };

private:
    struct FromStack {};

    explicit Namespace(lua_State* L)
        : Registrar(L)
    {
        lua_getglobal(L, "_G");

        ++m_stackSize;
    }

    Namespace(lua_State* L, Options options, FromStack)
        : Registrar(L, 1)
    {
        LUABRIDGE_ASSERT(lua_istable(L, -1));

        lua_pushvalue(L, -1); 

        lua_setmetatable(L, -2); 

        lua_pushcfunction_x(L, &detail::index_metamethod<false>, "__index");
        rawsetfield(L, -2, "__index"); 

        lua_newtable(L); 
        lua_rawsetp(L, -2, detail::getPropgetKey()); 

        lua_newtable(L); 
        lua_rawsetp(L, -2, detail::getPropsetKey()); 

        if (! options.test(visibleMetatables))
        {
            lua_pushboolean(L, 0);
            rawsetfield(L, -2, "__metatable");
        }

        ++m_stackSize;
    }

    Namespace(const char* name, Namespace parent, Options options)
        : Registrar(std::move(parent))
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        rawgetfield(L, -1, name); 

        if (lua_isnil(L, -1)) 
        {
            lua_pop(L, 1); 

            lua_newtable(L); 
            lua_pushvalue(L, -1); 

            lua_setmetatable(L, -2); 

            lua_pushcfunction_x(L, &detail::index_metamethod<false>, "__index");
            rawsetfield(L, -2, "__index"); 

            lua_pushcfunction_x(L, &detail::newindex_metamethod<false>, "__newindex");
            rawsetfield(L, -2, "__newindex"); 

            lua_newtable(L); 
            lua_rawsetp(L, -2, detail::getPropgetKey()); 

            lua_newtable(L); 
            lua_rawsetp(L, -2, detail::getPropsetKey()); 

            if (! options.test(visibleMetatables))
            {
                lua_pushboolean(L, 0);
                rawsetfield(L, -2, "__metatable");
            }

            lua_pushvalue(L, -1); 
            rawsetfield(L, -3, name); 
        }

        ++m_stackSize;
    }

    explicit Namespace(ClassBase child)
        : Registrar(std::move(child))
    {
    }

    explicit Namespace(Table child)
        : Registrar(std::move(child))
    {
    }

    using Registrar::operator=;

public:
    
    static Namespace getGlobalNamespace(lua_State* L)
    {
        return Namespace(L);
    }

    static Namespace getNamespaceFromStack(lua_State* L, Options options = defaultOptions)
    {
        return Namespace(L, options, FromStack{});
    }

    Namespace beginNamespace(const char* name, Options options = defaultOptions)
    {
        assertIsActive();
        return Namespace(name, std::move(*this), options);
    }

    Namespace endNamespace()
    {
        if (m_stackSize == 1)
        {
            throw_or_assert<std::logic_error>("endNamespace() called on global namespace");

            return Namespace(std::move(*this));
        }

        LUABRIDGE_ASSERT(m_stackSize > 1);
        --m_stackSize;
        lua_pop(L, 1);

        return Namespace(std::move(*this));
    }

    template <class T>
    Namespace& addVariable(const char* name, const T& value)
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if constexpr (std::is_enum_v<T>)
        {
            using U = std::underlying_type_t<T>;

            if (auto result = Stack<U>::push(L, static_cast<U>(value)); ! result)
                luaL_error(L, "%s", result.message().c_str());
        }
        else
        {
            if (auto result = Stack<T>::push(L, value); ! result)
                luaL_error(L, "%s", result.message().c_str());
        }

        rawsetfield(L, -2, name); 

        return *this;
    }

    template <class T, class = std::enable_if_t<std::is_base_of_v<T, LuaRef> || !std::is_invocable_v<T>>>
    Namespace& addProperty(const char* name, T* value, bool isWritable = true)
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if (! checkTableHasPropertyGetter())
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        lua_pushlightuserdata(L, value); 
        lua_pushcclosure_x(L, &detail::property_getter<T>::call, name, 1); 
        detail::add_property_getter(L, name, -2); 

        if (isWritable)
        {
            lua_pushlightuserdata(L, value); 
            lua_pushcclosure_x(L, &detail::property_setter<T>::call, name, 1); 
        }
        else
        {
            lua_pushstring(L, name); 
            lua_pushcclosure_x(L, &detail::read_only_error, name, 1); 
        }

        detail::add_property_setter(L, name, -2); 

        return *this;
    }

    template <class T, class = std::enable_if_t<std::is_base_of_v<T, LuaRef> || !std::is_invocable_v<T>>>
    Namespace& addProperty(const char* name, const T* value)
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if (! checkTableHasPropertyGetter())
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        lua_pushlightuserdata(L, const_cast<T*>(value)); 
        lua_pushcclosure_x(L, &detail::property_getter<T>::call, name, 1); 
        detail::add_property_getter(L, name, -2); 

        lua_pushstring(L, name); 
        lua_pushcclosure_x(L, &detail::read_only_error, name, 1); 
        detail::add_property_setter(L, name, -2); 

        return *this;
    }

    template <class TG>
    Namespace& addProperty(const char* name, TG (*get)())
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(get != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if (! checkTableHasPropertyGetter())
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        lua_pushlightuserdata(L, reinterpret_cast<void*>(get)); 
        lua_pushcclosure_x(L, &detail::invoke_proxy_function<TG (*)()>, name, 1); 
        detail::add_property_getter(L, name, -2);

        lua_pushstring(L, name);
        lua_pushcclosure_x(L, &detail::read_only_error, name, 1);
        detail::add_property_setter(L, name, -2);

        return *this;
    }

    template <class TG, class TS = TG>
    Namespace& addProperty(const char* name, TG (*get)(), void (*set)(TS))
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(get != nullptr);
        LUABRIDGE_ASSERT(set != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if (! checkTableHasPropertyGetter())
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        lua_pushlightuserdata(L, reinterpret_cast<void*>(get)); 
        lua_pushcclosure_x(L, &detail::invoke_proxy_function<TG (*)()>, name, 1); 
        detail::add_property_getter(L, name, -2);

        lua_pushlightuserdata(L, reinterpret_cast<void*>(set)); 
        lua_pushcclosure_x(L, &detail::invoke_proxy_function<void (*)(TS)>, name, 1);
        detail::add_property_setter(L, name, -2);

        return *this;
    }

    template <class TG>
    Namespace& addProperty(const char* name, TG (*get)() noexcept)
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(get != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if (! checkTableHasPropertyGetter())
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        lua_pushlightuserdata(L, reinterpret_cast<void*>(get)); 
        lua_pushcclosure_x(L, &detail::invoke_proxy_function<TG (*)() noexcept>, name, 1); 
        detail::add_property_getter(L, name, -2);

        lua_pushstring(L, name);
        lua_pushcclosure_x(L, &detail::read_only_error, name, 1);
        detail::add_property_setter(L, name, -2);

        return *this;
    }

    template <class TG, class TS = TG>
    Namespace& addProperty(const char* name, TG (*get)() noexcept, void (*set)(TS) noexcept)
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(get != nullptr);
        LUABRIDGE_ASSERT(set != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if (! checkTableHasPropertyGetter())
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        lua_pushlightuserdata(L, reinterpret_cast<void*>(get)); 
        lua_pushcclosure_x(L, &detail::invoke_proxy_function<TG (*)() noexcept>, name, 1); 
        detail::add_property_getter(L, name, -2);

        lua_pushlightuserdata(L, reinterpret_cast<void*>(set)); 
        lua_pushcclosure_x(L, &detail::invoke_proxy_function<void (*)(TS) noexcept>, name, 1);
        detail::add_property_setter(L, name, -2);

        return *this;
    }

    template <class Getter, class = std::enable_if_t<!std::is_pointer_v<Getter> && std::is_invocable_v<Getter>>>
    Namespace& addProperty(const char* name, Getter get)
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if (! checkTableHasPropertyGetter())
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        using GetType = decltype(get);

        lua_newuserdata_aligned<GetType>(L, std::move(get)); 
        lua_pushcclosure_x(L, &detail::invoke_proxy_functor<GetType>, name, 1); 
        detail::add_property_getter(L, name, -2); 

        lua_pushstring(L, name); 
        lua_pushcclosure_x(L, &detail::read_only_error, name, 1); 
        detail::add_property_setter(L, name, -2); 

        return *this;
    }

    template <class Getter, class Setter, class = std::enable_if_t<
        !std::is_pointer_v<Getter>
            && std::is_invocable_v<Getter>
            && !std::is_pointer_v<Setter>
            && std::is_invocable_v<Setter, std::invoke_result_t<Getter>>>>
    Namespace& addProperty(const char* name, Getter get, Setter set)
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if (! checkTableHasPropertyGetter())
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        addProperty<Getter>(name, std::move(get));

        using SetType = decltype(set);

        lua_newuserdata_aligned<SetType>(L, std::move(set)); 
        lua_pushcclosure_x(L, &detail::invoke_proxy_functor<SetType>, name, 1); 
        detail::add_property_setter(L, name, -2); 

        return *this;
    }

    Namespace& addProperty(const char* name, lua_CFunction get)
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(get != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if (! checkTableHasPropertyGetter())
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        lua_pushcfunction_x(L, get, name); 
        detail::add_property_getter(L, name, -2); 

        lua_pushstring(L, name); 
        lua_pushcclosure_x(L, &detail::read_only_error, name, 1); 
        detail::add_property_setter(L, name, -2); 

        return *this;
    }

    Namespace& addProperty(const char* name, lua_CFunction get, lua_CFunction set)
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(get != nullptr);
        LUABRIDGE_ASSERT(set != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if (! checkTableHasPropertyGetter())
        {
            throw_or_assert<std::logic_error>("addProperty() called on global namespace");

            return *this;
        }

        lua_pushcfunction_x(L, get, name); 
        detail::add_property_getter(L, name, -2); 

        lua_pushcfunction_x(L, set, name); 
        detail::add_property_setter(L, name, -2); 

        return *this;
    }

    template <class... Functions>
    auto addFunction(const char* name, Functions... functions)
        -> std::enable_if_t<(detail::is_callable_v<Functions> && ...) && (sizeof...(Functions) > 0), Namespace&>
    {
        LUABRIDGE_ASSERT(name != nullptr);
        LUABRIDGE_ASSERT(lua_istable(L, -1)); 

        if constexpr (sizeof...(Functions) == 1)
        {
            ([&]
            {
                detail::push_function(L, std::move(functions), name);

            } (), ...);
        }
        else
        {
            
            lua_createtable(L, static_cast<int>(sizeof...(Functions)), 0); 

            int idx = 1;

            ([&]
            {
                lua_createtable(L, 2, 0); 
                lua_pushinteger(L, 1);
                if constexpr (detail::is_any_cfunction_pointer_v<Functions>)
                    lua_pushinteger(L, -1);
                else
                    lua_pushinteger(L, static_cast<int>(detail::function_arity_excluding_v<Functions, lua_State*>));
                lua_settable(L, -3);
                lua_pushinteger(L, 2);
                detail::push_function(L, std::move(functions), name);
                lua_settable(L, -3);

                lua_rawseti(L, -2, idx);
                ++idx;

            } (), ...);

            lua_pushcclosure_x(L, &detail::try_overload_functions<false>, name, 1);
        }

        rawsetfield(L, -2, name);

        return *this;
    }

    Table beginTable(const char* name)
    {
        assertIsActive();
        return Table(name, std::move(*this));
    }

    template <class T>
    Class<T> beginClass(const char* name, Options options = defaultOptions)
    {
        assertIsActive();
        return Class<T>(name, std::move(*this), options);
    }

    template <class Derived, class Base>
    Class<Derived> deriveClass(const char* name, Options options = defaultOptions)
    {
        assertIsActive();
        return Class<Derived>(name, std::move(*this), detail::getStaticRegistryKey<Base>(), options);
    }
    
private:
    
    bool checkTableHasPropertyGetter() const
    {
        if (m_stackSize == 1 && lua_istable(L, -1))
        {
            lua_rawgetp(L, -1, detail::getPropgetKey());
            const bool propertyGetterTableIsValid = lua_istable(L, -1);
            lua_pop(L, 1);

            return propertyGetterTableIsValid;
        }
        
        return true;
    }
};

inline Namespace getGlobalNamespace(lua_State* L)
{
    return Namespace::getGlobalNamespace(L);
}

inline Namespace getNamespaceFromStack(lua_State* L)
{
    return Namespace::getNamespaceFromStack(L);
}

inline void registerMainThread(lua_State* L)
{
    register_main_thread(L);
}

} 


// End File: Source/LuaBridge/detail/Namespace.h

// Begin File: Source/LuaBridge/detail/Overload.h

namespace luabridge {

template <class... Args>
struct NonConstOverload
{
    template <class R, class T>
    constexpr auto operator()(R (T::*ptr)(Args...)) const noexcept -> decltype(ptr)
    {
        return ptr;
    }

    template <class R, class T>
    static constexpr auto with(R (T::*ptr)(Args...)) noexcept -> decltype(ptr)
    {
        return ptr;
    }
};

template <class... Args>
struct ConstOverload
{
    template <class R, class T>
    constexpr auto operator()(R (T::*ptr)(Args...) const) const noexcept -> decltype(ptr)
    {
        return ptr;
    }

    template <class R, class T>
    static constexpr auto with(R (T::*ptr)(Args...) const) noexcept -> decltype(ptr)
    {
        return ptr;
    }
};

template <class... Args>
struct Overload : ConstOverload<Args...>, NonConstOverload<Args...>
{
    using ConstOverload<Args...>::operator();
    using NonConstOverload<Args...>::operator();

    template <class R>
    constexpr auto operator()(R (*ptr)(Args...)) const noexcept -> decltype(ptr)
    {
        return ptr;
    }

    template <class R, class T>
    static constexpr auto with(R (T::*ptr)(Args...)) noexcept -> decltype(ptr)
    {
        return ptr;
    }
};

template <class... Args> [[maybe_unused]] constexpr Overload<Args...> overload = {};
template <class... Args> [[maybe_unused]] constexpr ConstOverload<Args...> constOverload = {};
template <class... Args> [[maybe_unused]] constexpr NonConstOverload<Args...> nonConstOverload = {};

} 


// End File: Source/LuaBridge/detail/Overload.h

// Begin File: Source/LuaBridge/detail/ScopeGuard.h

namespace luabridge::detail {

template <class F>
class ScopeGuard
{
public:
    template <class V>
    explicit ScopeGuard(V&& v)
        : m_func(std::forward<V>(v))
        , m_shouldRun(true)
    {
    }

    ~ScopeGuard()
    {
        if (m_shouldRun)
            m_func();
    }

    void reset() noexcept
    {
        m_shouldRun = false;
    }

private:
    F m_func;
    bool m_shouldRun;
};

template <class F>
ScopeGuard(F&&) -> ScopeGuard<F>;

} 


// End File: Source/LuaBridge/detail/ScopeGuard.h

// Begin File: Source/LuaBridge/LuaBridge.h

#define LUABRIDGE_MAJOR_VERSION 3
#define LUABRIDGE_MINOR_VERSION 1
#define LUABRIDGE_VERSION 301


// End File: Source/LuaBridge/LuaBridge.h

// Begin File: Source/LuaBridge/Map.h

namespace luabridge {

template <class K, class V>
struct Stack<std::map<K, V>>
{
    using Type = std::map<K, V>;

    [[nodiscard]] static Result push(lua_State* L, const Type& map)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        StackRestore stackRestore(L);

        lua_createtable(L, 0, static_cast<int>(map.size()));

        for (auto it = map.begin(); it != map.end(); ++it)
        {
            auto result = Stack<K>::push(L, it->first);
            if (! result)
                return result;

            result = Stack<V>::push(L, it->second);
            if (! result)
                return result;

            lua_settable(L, -3);
        }
        
        stackRestore.reset();
        return {};
    }

    [[nodiscard]] static TypeResult<Type> get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        const StackRestore stackRestore(L);

        Type map;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        while (lua_next(L, absIndex) != 0)
        {
            auto value = Stack<V>::get(L, -1);
            if (! value)
                return makeErrorCode(ErrorCode::InvalidTypeCast);

            auto key = Stack<K>::get(L, -2);
            if (! key)
                return makeErrorCode(ErrorCode::InvalidTypeCast);

            map.emplace(*key, *value);
            lua_pop(L, 1);
        }

        return map;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index);
    }
};

} 


// End File: Source/LuaBridge/Map.h

// Begin File: Source/LuaBridge/Set.h

namespace luabridge {

template <class K>
struct Stack<std::set<K>>
{
    using Type = std::set<K>;
    
    [[nodiscard]] static Result push(lua_State* L, const Type& set)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        StackRestore stackRestore(L);

        lua_createtable(L, 0, static_cast<int>(set.size()));

        auto it = set.cbegin();
        for (lua_Integer tableIndex = 1; it != set.cend(); ++tableIndex, ++it)
        {
            lua_pushinteger(L, tableIndex);

            auto result = Stack<K>::push(L, *it);
            if (! result)
                return result;

            lua_settable(L, -3);
        }

        stackRestore.reset();
        return {};
    }

    [[nodiscard]] static TypeResult<Type> get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        const StackRestore stackRestore(L);

        Type set;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        while (lua_next(L, absIndex) != 0)
        {
            auto item = Stack<K>::get(L, -1);
            if (! item)
                return makeErrorCode(ErrorCode::InvalidTypeCast);

            set.emplace(*item);
            lua_pop(L, 1);
        }

        return set;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index);
    }
};

} 


// End File: Source/LuaBridge/Set.h

// Begin File: Source/LuaBridge/UnorderedMap.h

namespace luabridge {

template <class K, class V>
struct Stack<std::unordered_map<K, V>>
{
    using Type = std::unordered_map<K, V>;

    [[nodiscard]] static Result push(lua_State* L, const Type& map)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        StackRestore stackRestore(L);

        lua_createtable(L, 0, static_cast<int>(map.size()));

        for (auto it = map.begin(); it != map.end(); ++it)
        {
            auto result = Stack<K>::push(L, it->first);
            if (! result)
                return result;

            result = Stack<V>::push(L, it->second);
            if (! result)
                return result;

            lua_settable(L, -3);
        }
        
        stackRestore.reset();
        return {};
    }

    [[nodiscard]] static TypeResult<Type> get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        const StackRestore stackRestore(L);

        Type map;

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        while (lua_next(L, absIndex) != 0)
        {
            auto value = Stack<V>::get(L, -1);
            if (! value)
                return makeErrorCode(ErrorCode::InvalidTypeCast);

            auto key = Stack<K>::get(L, -2);
            if (! key)
                return makeErrorCode(ErrorCode::InvalidTypeCast);

            map.emplace(*key, *value);
            lua_pop(L, 1);
        }

        return map;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index);
    }
};

} 


// End File: Source/LuaBridge/UnorderedMap.h

// Begin File: Source/LuaBridge/Vector.h

namespace luabridge {

template <class T>
struct Stack<std::vector<T>>
{
    using Type = std::vector<T>;

    [[nodiscard]] static Result push(lua_State* L, const Type& vector)
    {
#if LUABRIDGE_SAFE_STACK_CHECKS
        if (! lua_checkstack(L, 3))
            return makeErrorCode(ErrorCode::LuaStackOverflow);
#endif

        StackRestore stackRestore(L);

        lua_createtable(L, static_cast<int>(vector.size()), 0);

        for (std::size_t i = 0; i < vector.size(); ++i)
        {
            lua_pushinteger(L, static_cast<lua_Integer>(i + 1));
            
            auto result = Stack<T>::push(L, vector[i]);
            if (! result)
                return result;

            lua_settable(L, -3);
        }
        
        stackRestore.reset();
        return {};
    }

    [[nodiscard]] static TypeResult<Type> get(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            return makeErrorCode(ErrorCode::InvalidTypeCast);

        const StackRestore stackRestore(L);

        Type vector;
        vector.reserve(static_cast<std::size_t>(get_length(L, index)));

        int absIndex = lua_absindex(L, index);
        lua_pushnil(L);

        while (lua_next(L, absIndex) != 0)
        {
            auto item = Stack<T>::get(L, -1);
            if (! item)
                return makeErrorCode(ErrorCode::InvalidTypeCast);

            vector.emplace_back(*item);
            lua_pop(L, 1);
        }

        return vector;
    }

    [[nodiscard]] static bool isInstance(lua_State* L, int index)
    {
        return lua_istable(L, index);
    }
};

} 


// End File: Source/LuaBridge/Vector.h
// clang-format on

