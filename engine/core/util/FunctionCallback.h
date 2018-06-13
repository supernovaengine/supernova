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
#include "IntegerSequence.h"

template<size_t>
struct MyPlaceholder {};

template<size_t N>
struct std::is_placeholder<MyPlaceholder<N>> : public std::integral_constant<size_t, N> {};

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
 
        template<typename T, size_t... Idx>
        std::function<Ret(Args...)> bindImpl(T *obj, Ret(T::*funcPtr)(Args...), index_sequence<Idx...>) {
            return std::bind(funcPtr, obj, getPlaceholder<Idx>()...);
        }

        template<size_t N>
        MyPlaceholder<N + 1> getPlaceholder() {
            return {};
        }

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

        FunctionCallback& operator = (lua_State *L){
            this->set(L);

            return *this;
        }

        FunctionCallback& operator = (std::function<Ret(Args...)> function){
            this->set(function);

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

        template<typename T, Ret(T::*funcPtr)(Args...)>
        bool set(std::shared_ptr<T> obj) {
            addImpl(bindImpl(obj.get(), funcPtr, index_sequence_for<Args...>{}));
            return true;
        }

        template<typename T, Ret(T::*funcPtr)(Args...)>
        bool set(T *obj) {
            addImpl(bindImpl(obj, funcPtr, index_sequence_for<Args...>{}));
            return true;
        }

        template<typename T>
        bool set(std::shared_ptr<T> t) {
            addImpl(*t.get());
            return true;
        }

        template<typename T>
        bool set(T* t) {
            addImpl(*t);
            return true;
        }

        bool remove() {
            cFunction = NULL;
            luaFunction.remove();
            return true;
        }

        Ret call(Args... args){
            if (cFunction)
                return cFunction(args...);

            return luaFunction.call<Ret,Args...>(args...);
        }

        Ret operator()(Args... args) {
            return call(args...);
        }
    };
}


#endif //FUNCTIONCALLBACK_H
