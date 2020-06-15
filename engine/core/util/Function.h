#ifndef FUNCTION_H
#define FUNCTION_H

//
// (c) 2018 Eduardo Doria.
//

#include <functional>
#include <string>
#include "script/LuaFunction.h"

namespace Supernova {

    template<typename T>
    class Function;

    template<typename Ret, typename ...Args>
    class Function<Ret(Args...)> {

    private:

        std::function<Ret(Args...)> cFunction;
        LuaFunction luaFunction;

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

        Function(lua_State *L){
            this->set(L);
        }

        Function& operator = (const Function& t){
            this->cFunction = t.cFunction;
            this->luaFunction = t.luaFunction;

            return *this;
        }

        Function& operator = (lua_State *L){
            this->set(L);

            return *this;
        }

        Function& operator = (std::function<Ret(Args...)> function){
            this->set(function);

            return *this;
        }

        int set(lua_State *L) {
            return luaFunction.set(L);
        }

        bool set(std::function<Ret(Args...)> function) {
            cFunction = function;
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


#endif //FUNCTION_H
