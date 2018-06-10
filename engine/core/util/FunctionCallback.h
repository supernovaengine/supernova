#ifndef FUNCTIONCALLBACK_H
#define FUNCTIONCALLBACK_H

/*
 * Inspired by the work of https://nikitablack.github.io/2016/04/26/stdfunction-as-delegate.html
 */

//
// (c) 2018 Eduardo Doria.
//

#include <functional>
#include <string>
#include <vector>
#include "LuaFunction.h"

/*
template<int ...>
struct ints
{
};

template<int N, int... Is>
struct int_seq : int_seq<N - 1, N, Is...>
{
};

template<int... Is>
struct int_seq<0, Is...>
{
    using type = ints<0, Is...>;
};

template<size_t>
struct MyPlaceholder {};

template<size_t N>
struct std::is_placeholder<MyPlaceholder<N>> : public std::integral_constant<size_t, N> {};
*/
namespace Supernova {

    template<typename T>
    class FunctionCallback;

    template<typename Ret, typename ...Args>
    class FunctionCallback<Ret(Args...)> {

    private:

        bool addImpl(std::function<Ret(Args...)> function) {
            this->cFunction = function;
            return true;
        }
/*
 * Removed to compile project without -std=c++11
 
        template<typename T, size_t... Idx>
        std::function<Ret(Args...)> bindImpl(T *obj, Ret(T::*funcPtr)(Args...), ints<Idx...>) {
            return std::bind(funcPtr, obj, getPlaceholder<Idx>()...);
        }

        template<size_t N>
        MyPlaceholder<N + 1> getPlaceholder() {
            return {};
        }
*/

        std::function<Ret(Args...)> cFunction;
        LuaFunction luaFunction;

    public:

        FunctionCallback() {
            cFunction = NULL;
        }

        FunctionCallback(const FunctionCallback& t){
            this->cFunction = t.cFunction;
            this->luaFunction = t.luaFunction;
        }

        FunctionCallback& operator = (const FunctionCallback& t){
            this->cFunction = t.cFunction;
            this->luaFunction = t.luaFunction;

            return *this;
        }

        int set(lua_State *L) {
            return luaFunction.set(L);
        }

        bool set(std::function<Ret(Args...)> function) {
            addImpl(function);
            return true;
        }

        template<Ret(*funcPtr)(Args...)>
        bool set() {
            addImpl(funcPtr);
            return true;
        }
/*
        template<typename T, Ret(T::*funcPtr)(Args...)>
        bool set(std::shared_ptr<T> obj) {
            addImpl(bindImpl(obj.get(), funcPtr, int_seq<sizeof...(Args)>{}));
            return true;
        }
*/
        template<typename T>
        bool set(std::shared_ptr<T> t) {
            addImpl(*t.get());
            return true;
        }

        bool remove() {
            cFunction = NULL;
            luaFunction.reset();
            return true;
        }

        void call(Args... args){
            if (cFunction)
                cFunction(args...);

            luaFunction.call(args...);
        }

        int operator()(lua_State *L) {
            return set(L);
        }

        void operator()(std::function<Ret(Args...)> function) {
            set(function);
        }
    };
}


#endif //FUNCTIONCALLBACK_H
