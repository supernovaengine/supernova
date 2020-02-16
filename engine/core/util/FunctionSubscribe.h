/*
 * Inspired by the work of https://nikitablack.github.io/2016/04/26/stdfunction-as-delegate.html
 */

//
// (c) 2020 Eduardo Doria.
//

#ifndef FUNCTIONSUBSCRIBE_H
#define FUNCTIONSUBSCRIBE_H

#include <functional>
#include <string>
#include <vector>
#include "Function.h"
#include "IntegerSequence.h"

template<size_t>
struct MyPlaceholder {};

template<size_t N>
struct std::is_placeholder<MyPlaceholder<N>> : public std::integral_constant<size_t, N> {};

namespace Supernova {

    template<typename T>
    class FunctionSubscribe;

    template<typename Ret, typename ...Args>
    class FunctionSubscribe<Ret(Args...)> {

    private:

        bool addImpl(const std::string& tag, Function<Ret(Args...)> function) {
            if (find(tags.begin(), tags.end(), tag) != tags.end())
            {
                return false;
            }

            functions.push_back(function);
            tags.push_back(tag);

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

        std::vector<Function<Ret(Args...)>> functions;
        std::vector<std::string> tags;

    public:

        FunctionSubscribe() {
        }

        FunctionSubscribe(const FunctionSubscribe& t){
            this->functions = t.functions;
            this->tags = t.tags;
        }

        FunctionSubscribe& operator = (const FunctionSubscribe& t){
            this->functions = t.functions;
            this->tags = t.tags;

            return *this;
        }

        FunctionSubscribe& operator = (lua_State *L){
            add("luaFunction", L);

            return *this;
        }

        FunctionSubscribe& operator = (std::function<Ret(Args...)> function){
            add("userFunction", function);

            return *this;
        }

        bool add(const std::string& tag, lua_State *L) {
            addImpl(tag, L);
            return true;
        }

        bool add(const std::string& tag, std::function<Ret(Args...)> function) {
            addImpl(tag, function);
            return true;
        }

        template<Ret(*funcPtr)(Args...)>
        bool add(const std::string& tag)
        {
            addImpl(tag, std::function<Ret(Args...)>(funcPtr));
            return true;
        }

        template<typename T, Ret(T::*funcPtr)(Args...)>
        bool add(const std::string& tag, std::shared_ptr<T> obj)
        {
            addImpl(tag, bindImpl(obj.get(), funcPtr, index_sequence_for<Args...>{}));
            return true;
        }

        template<typename T>
        bool add(const std::string& tag, std::shared_ptr<T> t)
        {
            addImpl(tag, *t.get());
            return true;
        }

        bool remove(const std::string& tag)
        {
            auto it = find(tags.begin(), tags.end(), tag);
            if (it == tags.end())
            {
                return false;
            }

            auto index{ distance(tags.begin(), it) };
            tags.erase(it);

            functions.erase(functions.begin() + index);

            return true;
        }

        void call(Args... args){
            for (auto& function : functions)
            {
                function(args...);
            };
        }

        void operator()(Args... args)
        {
            call(args...);
        }
    };
}

#endif //FUNCTIONSUBSCRIBE_H
