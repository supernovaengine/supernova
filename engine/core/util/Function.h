#ifndef FUNCTION_H
#define FUNCTION_H

//
// (c) 2018 Eduardo Doria.
//

#include <functional>
#include <string>
#include "Log.h"
#include "LuaBinding.h"
#include "sol/sol.hpp"

namespace Supernova {

    template<typename T>
    class Function;

    template<typename Ret, typename ...Args>
    class Function<Ret(Args...)> {

    private:

        std::function<Ret(Args...)> cFunction;
        sol::protected_function luaFunction;

    public:

        Function() {
            cFunction = NULL;
        }

        Function(const Function& t){
            this->cFunction = t.cFunction;
            this->luaFunction = t.luaFunction;
        }

        Function(const std::function<Ret(Args...)> function){
            this->set(function);
        }

        Function(sol::protected_function function){
            this->set(function);
        }

        Function& operator = (const Function& t){
            this->cFunction = t.cFunction;
            this->luaFunction = t.luaFunction;

            return *this;
        }

        Function& operator = (sol::protected_function function){
            this->set(function);

            return *this;
        }

        Function& operator = (std::function<Ret(Args...)> function){
            this->set(function);

            return *this;
        }

        void set(sol::protected_function function) {
            luaFunction = function;
        }

        void set(std::function<Ret(Args...)> function) {
            cFunction = function;
        }

        bool remove() {
            cFunction = NULL;
            luaFunction.abandon();
            return true;
        }

        Ret call(Args... args){
            if (cFunction)
                return cFunction(args...);

            auto luafunc = luaFunction(args...);
            if (!luafunc.valid()) {
                sol::error err = luafunc;
                Log::Error("Lua Error: %s", err.what());
            }else{
                return (Ret)luafunc;
            }

            return Ret();
        }

        Ret operator()(Args... args) {
            return call(args...);
        }
    };
}


#endif //FUNCTION_H
