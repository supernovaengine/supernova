lua-intf
========

`lua-intf` is a binding between C++11 and Lua language, it provides three different set of API in one package:

+ `LuaBinding`, Export C++ class or function to Lua script
+ `LuaRef`, High level API to access Lua object
+ `LuaState`, Low level API as simple wrapper for Lua C API

`lua-intf` has no dependencies other than Lua and C++11. And by default it is headers-only library, so there is no makefile or other install instruction, just copy the source, include `LuaIntf.h`, and you are ready to go.

`lua-intf` is inspired by [vinniefalco's LuaBridge](https://github.com/vinniefalco/LuaBridge) work, but has been rewritten to take advantage of C++11 features.

Lua and C++ error handling
--------------------------

By default LuaIntf expect the Lua library to build under C++, this will allow Lua library to throw exception upon error, and make sure C++ objects on stack to be destructed correctly. For more info about error handling issues, please see:

http://lua-users.org/wiki/ErrorHandlingBetweenLuaAndCplusplus

If you really want to use Lua library compiled under C and want to live with `longjmp` issues, you can define `LUAINTF_LINK_LUA_COMPILED_IN_CXX` to 0 before including `lua-intf` headers:
````c++
#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 0
#include "LuaIntf/LuaIntf.h"
````

Compile Lua library in C++
--------------------------

Most distributions of precompiled Lua library are compiled under C, if you need Lua library compiled under C++, you probably need to compile it by yourself. It is actually very easy to build Lua library under C++, first get a copy of source code of the Lua library:
````
curl http://www.lua.org/ftp/lua-5.3.0.tar.gz -o lua-5.3.0.tar.gz
tar xf lua-5.3.0.tar.gz
cd lua-5.3.0
````

To compile on Linux:
````
make linux MYCFLAGS="-x c++" CC="g++"
````

To compile on Mac OSX:
````
make macosx MYCFLAGS="-x c++" MYLDFLAGS="-lc++"
````

To compile on Windows with MINGW and MSYS:
````
make mingw MYCFLAGS="-x c++" CC="g++"
````

And then install to your chosen directory in `<path>`:
````
make install INSTALL_TOP=<path>
````

Export C++ class or function to Lua script
------------------------------------------

You can easily export C++ class or function for Lua script, consider the following C++ class:
````c++
    class Web
    {
    public:
        // base_url is optional
        Web(const std::string& base_url);
        ~Web();

        static void go_home();

        static std::string home_url();
        static void set_home_url(const std::string& url);

        std::string url() const;
        void set_url(const std::string& url);
        std::string resolve_url(const std::string& uri);

        // doing reload if uri is empty
        std::string load(const std::string& uri);
    };
````
You can export the `Web` class by the following code:
````c++
    LuaBinding(L).beginClass<Web>("Web")
        .addConstructor(LUA_ARGS(_opt<std::string>))
        .addStaticProperty("home_url", &Web::home_url, &Web::set_home_url)
        .addStaticFunction("go_home", &Web::go_home)
        .addProperty("url", &Web::url, &Web::set_url)
        .addFunction("resolve_url", &Web::resolve_url)
        .addFunction("load", &Web::load, LUA_ARGS(_opt<std::string>))
        .addStaticFunction("lambda", [] {
            // you can use C++11 lambda expression here too
            return "yes";
        })
    .endClass();
````
To access the exported `Web` class in Lua:
````lua
    local w = Web()								-- auto w = Web("");
    w.url = "http://www.yahoo.com"				-- w.set_url("http://www.yahoo.com");
    local page = w:load()						-- auto page = w.load("");
    page = w:load("http://www.google.com")		-- page = w.load("http://www.google.com");
    local url = w.url							-- auto url = w.url();
````

Module and class
----------------

C++ class or functions exported are organized by module and class. The general layout of binding looks like:
````c++
    LuaBinding(L)
        .beginModule(string module_name)
            .addFactory(function* func)

            .addConstant(string constant_name, VALUE_TYPE value)

            .addVariable(string property_name, VARIABLE_TYPE* var, bool writable = true)
            .addVariableRef(string property_name, VARIABLE_TYPE* var, bool writable = true)

            .addProperty(string property_name, FUNCTION_TYPE getter, FUNCTION_TYPE setter)
            .addProperty(string property_name, FUNCTION_TYPE getter)

            .addFunction(string function_name, FUNCTION_TYPE func)

            .beginModule(string sub_module_name)
                ...
            .endModule()

            .beginClass<CXX_TYPE>(string class_name)
                ...
            .endClass()

            .beginExtendClass<CXX_TYPE, SUPER_CXX_TYPE>(string sub_class_name)
                ...
            .endClass()
        .endModule()

        .beginClass<CXX_TYPE>(string class_name)
            .addFactory(FUNCTION_TYPE func)		// you can only have one addFactory or addConstructor
            .addConstructor(LUA_ARGS(...))

            .addConstant(string constant_name, VALUE_TYPE value)

            .addStaticVariable(string property_name, VARIABLE_TYPE* var, bool writable = true)
            .addStaticVariableRef(string property_name, VARIABLE_TYPE* var, bool writable = true)

            .addStaticProperty(string property_name, FUNCTION_TYPE getter, FUNCTION_TYPE setter)
            .addStaticProperty(string property_name, FUNCTION_TYPE getter)

            .addStaticFunction(string function_name, FUNCTION_TYPE func)

            .addVariable(string property_name, CXX_TYPE::FIELD_TYPE* var, bool writable = true)
            .addVariableRef(string property_name, CXX_TYPE::FIELD_TYPE* var, bool writable = true)

            .addProperty(string property_name, CXX_TYPE::FUNCTION_TYPE getter, CXX_TYPE::FUNCTION_TYPE setter)
            .addProperty(string property_name, CXX_TYPE::FUNCTION_TYPE getter, CXX_TYPE::FUNCTION_TYPE getter_const, CXX_TYPE::FUNCTION_TYPE setter)
            .addProperty(string property_name, CXX_TYPE::FUNCTION_TYPE getter)

            .addPropertyReadOnly(string property_name, CXX_TYPE::FUNCTION_TYPE getter, CXX_TYPE::FUNCTION_TYPE getter_const)
            .addPropertyReadOnly(string property_name, CXX_TYPE::FUNCTION_TYPE getter)

            .addFunction(string function_name, CXX_TYPE::FUNCTION_TYPE func)
        .endClass()

        .beginExtendClass<CXX_TYPE, SUPER_CXX_TYPE>(string sub_class_name)
            ...
        .endClass()
````
A module binding is like a package or namespace, it can also contain other sub-module or class. A module can have the following bindings:

+ factory function - bind to global or static C++ functions, or forward to sub-class constructor or sub-module factory
+ constant - bind to constant value
+ variable - bind to global or static variable, the valued pushed to lua is by-value, can be read-only
+ variable reference - bind to global or static variable, the valued pushed to lua is by-reference, can be read-only
+ property - bind to getter and setter functions, can be read-only
+ function - bind to global or static function

A class binding is modeled after C++ class, it models the const-ness correctly, so const object can not access non-const functions. It can have the following bindings:

+ constructor - bind to class constructor
+ factory function - bind to global or static function that return the newly created object
+ constant - bind to constant value, constant is static
+ static variable - bind to global or static variable, the valued pushed to lua is by-value, can be read-only
+ static variable reference - bind to global or static variable, the valued pushed to lua is by-reference, can be read-only
+ static property - bind to global or static getter and setter functions, can be read-only
+ static function - bind to global or static function
+ member variable - bind to member fields, the valued pushed to lua is by-value, can be read-only
+ member variable reference - bind to member fields, the valued pushed to lua is by-reference, can be read-only
+ member property - bind to member getter and setter functions, you can bind const and non-const version of getters, can be read-only
+ member function - bind to member functions

For module and class, you can have only one constructor or factory function. To access the factory or constructor in Lua script, you call the module or class name like a function, for example the above `Web` class:
````lua
    local w = Web("http://www.google.com")
````
The static or module variable, property and constant is accessible by module/class name, it is just like table field:
````lua
    local url = Web.home_url
    Web.home_url = "http://www.google.com"
````
The static function can be called by the following, note the '.' syntax and class name:
````lua
    Web.go_home()
````
The member variable and property is associated with object, so it is accessible by variable:
````lua
    local session = Web("http://www.google.com")
    local url = session.url
    session.url = "http://www.google.com"
````
The member function can be called by the following, note the ':' syntax and object name:
````lua
    session:load("http://www.yahoo.com")
````

Integrate with Lua module system
--------------------------------

The lua module system don't register modules in global variables. So you'll need to pass a local reference to `LuaBinding`. For example:
````c++
    extern "C" int luaopen_modname(lua_State* L)
    {
        LuaRef mod = LuaRef::createTable(L);
        LuaBinding(mod)
            ...;
        mod.pushToStack();
        return 1;
    }
````

C++ object life-cycle
---------------------

`lua-intf` store C++ object via Lua `userdata`, and it stores the object in the following ways:

+ By value, the C++ object is stored inside `userdata`. So when Lua need to gc the `userdata`, the memory is automatically released. `lua-intf` will make sure the C++ destructor is called when that happened. C++ constructor or function returns object struct will create this kind of Lua object.

+ By pointer, only the pointer is stored inside `userdata`. So when Lua need to gc the `userdata`, the object is still alive. The object is owned by C++ code, it is important the registered function does not return pointer to newly allocated object that need to be deleted explicitly, otherwise there will be memory leak. C++ function returns pointer or reference to object will create this kind of Lua object.

+ By shared pointer, the shared pointer is stored inside `userdata`. So when Lua need to gc the `userdata`, the shared pointer is destructed, that usually means Lua is done with the object. If the object is still referenced by other shared pointer, it will keep alive, otherwise it will be deleted as expected. C++ function returns shared pointer will create this kind of Lua object. A special version of `addConstructor` will also create shared pointer automatically.

Using shared pointer
--------------------

If both C++ and Lua code need to access the same object, it is usually better to use shared pointer in both side, thus avoiding memory leak. You can use any kind of shared pointer class, as long as it provides:

+ `operator ->`
+ `operator *`
+ `operator bool`
+ default constructor
+ copy constructor

Before you can use it, you need to register it with `lua-intf`, take `std::shared_ptr` for example:
````c++
    namespace LuaIntf
    {
        LUA_USING_SHARED_PTR_TYPE(std::shared_ptr)
    }
````
For constructing shared pointer inside Lua `userdata`, you can register the constructor by adding LUA_SP macro and the actual shared pointer type, for example:
````c++
    LuaBinding(L).beginClass<Web>("web")
        .addConstructor(LUA_SP(std::shared_ptr<Web>), LUA_ARGS(_opt<std::string>))
        ...
    .endClass();
````

Using custom deleter
--------------------

If custom deleter is needed instead of the destructor, you can register the constructor with deleter by adding LUA_DEL macro, for example:
````c++
     class MyClass
     {
     public:
          MyClass(int, int);
          void release();
     };

     struct MyClassDeleter
     {
         void operator () (MyClass* p)
         {
             p->release();
         }
     };

    LuaBinding(L).beginClass<MyClass>("MyClass")
        .addConstructor(LUA_DEL(MyClassDeleter), LUA_ARGS(int, int))
        ...
    .endClass();
````

Using STL-style container
-------------------------

By default `lua-intf` does not add conversion for container types, however, you can enable support for container type with the following:

````c++
    namespace LuaIntf
    {
        LUA_USING_LIST_TYPE(std::vector)
        LUA_USING_MAP_TYPE(std::map)
    }
````

For non-template or non-default template container type, you can use:

````c++
    class non_template_int_list
    { ... };

    template <typename T, typename A, typename S>
    class custom_template_list<T, A, S>
    { ... };

    namespace LuaIntf
    {
        LUA_USING_LIST_TYPE_X(non_template_int_list)

        // you need to use LUA_COMMA for , to workaround macro limitation
        LUA_USING_LIST_TYPE_X(custom_template_list<T LUA_COMMA A LUA_COMMA S>,
            typename T, typename A, typename S)
    }
````

Function calling convention
---------------------------

C++ function exported to Lua can follow one of the two calling conventions:

+ Normal function, the return value will be the value seen on the Lua side.

+ Lua `lua_CFunction` convention, that the first argument is `lua_State*` and the return type is `int`. And just like `lua_CFunction` you need to manually push the result onto Lua stack, and return the number of results pushed.

`lua-intf` extends `lua_CFunction` convention by allowing more arguments besides `lua_State*`, the following functions will all follow the `lua_CFunction` convention:
````c++
    // regular lua_CFunction convention
    int func_1(lua_state* L);

    // lua_CFunction convention, but allow arg1, arg2 to map to arguments
    int func_2(lua_state* L, const std::string& arg1, int arg2);

    // this is *NOT* lua_CFunction
    // the L can be placed anywhere, and it is stub to capture lua_State*,
    // and do not contribute to actual Lua arguments
    int func_3(const std::string& arg1, lua_state* L);

    class Object
    {
    public:
        // class method can follow lua_CFunction convention too
        int func_1(lua_state* L);

        // class lua_CFunction convention, but allow arg1, arg2 to map to arguments
        int func_2(lua_state* L, const std::string& arg1, int arg2);
    };

    // the following can also be lua_CFunction convention if it is added as class functions
    // note the first argument must be the pointer type of the registered class
    int obj_func_1(Object* obj, lua_state* L);
    int obj_func_2(Object* obj, lua_state* L, const std::string& arg1, int arg2);
````
For every function registration, `lua-intf` also support C++11 `std::function` type, so you can use `std::bind` or lambda expression if needed. You can use lambda expression freely without giving full `std::function` declaration; `std::bind` on the other hand, must be declared:
````c++
    LuaBinding(L).beginClass<Web>("Web")
        .addStaticFunction("lambda", [] {
            // you can use C++11 lambda expression here too
            return "yes";
        })
        .addStaticFunction<std::function<std::string()>>("bind",
            std::bind(&Web::url, other_web_object))
    .endClass();
````

If your C++ function is overloaded, pass `&function` is not enough, you have to explicitly cast it to proper type:
````c++
    static int test(string, int);
    static string test(string);

    LuaBinding(L).beginModule("utils")

        // this will bind int test(string, int)
        .addFunction("test_1", static_cast<int(*)(string, int)>(&test))

        // this will bind string test(string), by using our LUA_FN macro
        // LUA_FN(RETURN_TYPE, FUNC_NAME, ARG_TYPES...)
        .addFunction("test_2", LUA_FN(string, test, string))

    .endModule();
````

Function argument modifiers
---------------------------

By default the exported functions expect every argument to be mandatory, if the argument is missing or not compatible with the expected type, the Lua error will be raised. You can change the function passing requirement by adding argument passing modifiers in `LUA_ARGS`, `lua-intf` supports the following modifiers:

+ `_opt<TYPE>`, specify the argument is optional; if the argument is missing, the value is created with default constructor

+ `_def<TYPE, DEF_NUM, DEF_DEN = 1>`, specify the argument is optional; if the argument is missing, the default value is used as `DEF_NUM / DEF_DEN`

+ `_out<TYPE&>`, specify the argument is for output only; the output value will be pushed after the normal function return value, and in argument order if there is multiple output

+ `_ref<TYPE&>`, specify the argument is mandatory and for input/output; the output value will be pushed after the normal function return value, and in argument order if there is multiple output

+ `_ref_opt<TYPE&>`, combine `_ref<TYPE&>` and `_opt<TYPE>`

+ `_ref_def<TYPE&, DEF_NUM, DEF_DEN = 1>`, combine `_ref<TYPE&>` and `_def<TYPE, DEF_NUM, DEF_DEN = 1>`

+ If none of the above modifiers are used, the argument is mandatory and for input only

All output modifiers require the argument to be reference type, using pointer type for output is not supported. The reason `_def<TYPE, DEF_NUM, DEF_DEN = 1>` requires DEF_NUM and DEF_DEN is to workaround C++ limitation. The C++ template does not allow floating point number as non-type argument, in order specify default value for float, you have to specify numerator and denominator pair (the denominator is 1 by default). For example:

````c++
    struct MyString
    {
        std::string indexOf(const std::string& str, int pos);
        std::string desc(float number);
        ...
    };

    #define _def_float(f) _def<float, long((f) * 1000000), 1000000>

    LuaBinding(L).beginClass<MyString>("mystring")

        // this will make pos = 1 if it is not specified in Lua side
        .addFunction("indexOf", &MyString::indexOf, LUA_ARGS(std::string, _def<int, 1>))

        // this will make number = 1.333 = (4 / 3) if it is not specified in Lua side
        // because C++ does not allow float as non-type template parameter
        // you have to use ratio to specify floating numbers 1.333 = (4 / 3)
        // LUA_ARGS(_def<float, 1.33333f>) will result in error
        .addFunction("indexOf", &MyString::desc, LUA_ARGS(_def<float, 4, 3>))

        // you can define your own macro to make it easier to specify float
        // please see _def_float for example
        .addFunction("indexOf2", &MyString::desc, LUA_ARGS(_def_float(1.3333f)))
    .endClass();
````

Return multiple results for Lua
-------------------------------

It is possible to return multiple results by telling which argument is for output, for example:
````c++
    static std::string match(const std::string& src, const std::string& pat, int pos, int& found_pos);

    LuaBinding(L).beginModule("utils")

        // this will return (string) (found_pos)
        .addFunction("match", &match, LUA_ARGS(std::string, std::string, _def<int, 1>, _out<int&>))

    .endModule();
````

Yet another way to return multiple results is to use `std::tuple`:
````c++
    static std::tuple<std::string, int> match(const std::string& src, const std::string& pat, int pos);

    LuaBinding(L).beginModule("utils")

        // this will return (string) (found_pos)
        .addFunction("match", &match)

    .endModule();
````

And you can always use `lua_CFunction` to manually push multiple results by yourself.

Lua for-loop iteration function
-------------------------------

`lua-intf` provides a helper class `CppFunctor` to make it easier to implement for-loop iteration function for Lua.  To use it, user need to inherit CppFunctor, and override run method and optional destructor. Then call pushToStack to create the functor object on Lua stack.
````c++
    class MyIterator : public CppFunctor
    {
        MyIterator(...)
        {
            ...
        }

        virtual ~MyIterator()
        {
            ...
        }

        // the for-loop will call this function for each step until it return 0
        virtual int run(lua_State* L) override
        {
            ...
            // return the number of variables for each step of for-loop
            // or return 0 to end the for-loop
            return 2;
        }
    }

    int xpairs(lua_State* L)
    {
        return CppFunctor::make<MyIterator>(L, ...); // ... is constructor auguments
    }
````
To register the for-loop iteration function:
````c++
    LuaBinding(L).beginModule("utils")

        .addFunction("xpairs", &xpairs)

    .endModule();
````
To use the iteration function in Lua code:
````lua
    for x, y in utils.xpairs(...) do
        ...
    end
````

Custom type mapping
-------------------

It is possible to add primitive type mapping to the `lua-intf`, all you need to do is to add template specialization to `LuaTypeMapping<Type>`. You need to:

+ provide `void push(lua_State* L, const Type& v)`
+ provide `Type get(lua_State* L, int index)`
+ provide `Type opt(lua_State* L, int index, const Type& def)`

For example, to add `std::wstring` mapping to Lua string:
````c++
    namespace LuaIntf
    {

    template <>
    struct LuaTypeMapping <std::wstring>
    {
        static void push(lua_State* L, const std::wstring& str)
        {
            if (str.empty()) {
                lua_pushliteral(L, "");
            } else {
                std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
                std::string buf = conv.to_bytes(str);
                lua_pushlstring(L, buf.data(), buf.length());
            }
        }

        static std::wstring get(lua_State* L, int index)
        {
            size_t len;
            const char* p = luaL_checklstring(L, index, &len);
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            return conv.from_bytes(p, p + len);
        }

        static std::wstring opt(lua_State* L, int index, const std::wstring& def)
        {
            return lua_isnoneornil(L, index) ? def : get(L, index);
        }
    };

    } // namespace LuaIntf
````
After that, you are able to push `std::wstring` onto or get `std::wstring` from Lua stack directly:
````c++
    std::wstring s = ...;
    Lua::push(L, s);

    ...

    s = Lua::pop<std::wstring>(L);
````

High level API to access Lua object
-----------------------------------

`LuaRef` is designed to provide easy access to Lua object, and in most case you don't have to deal with Lua stack like the low level API. For example:
````c++
    lua_State* L = ...;
    lua_getglobal(L, "module");
    lua_getfield(L, -1, "method");
    lua_pushintteger(L, 1);
    lua_pushstring(L, "yes");
    lua_pushboolean(L, true);
    lua_call(L, 3, 0);
````
The above code can be rewritten as:
````c++
    LuaRef func(L, "module.method");
    func(1, "yes", true);
````
Table access is as simple:
````c++
    LuaRef table(L, "my_table");
    table["value"] = 15;
    int value = table.get<int>("value");

    for (auto& e : table) {
        std::string key = e.key<std::string>();
        LuaRef value = e.value<LuaRef>();
        ...
    }
````
And you can mix it with the low level API:
````c++
    lua_State* L = ...;
    LuaRef v = ...;
    lua_getglobal(L, "my_method");
    Lua::push(L, 1);					// the same as lua_pushinteger
    Lua::push(L, v);					// push v to lua stack
    Lua::push(L, true);					// the same as lua_pushboolean
    lua_call(L, 3, 2);
    LuaRef r(L, -2); 					// map r to lua stack index -2
````
You can use the `std::tuple` for multiple return values:
````c++
    LuaRef func(L, "utils.match");
    std::string found;
    int found_pos;
    std::tie(found, found_pos) = func.call<std::tuple<std::string, int>>("this is test", "test");
````

Low level API as simple wrapper for Lua C API
---------------------------------------------

`LuaState` is a simple wrapper of one-to-one mapping for Lua C API, consider the following code:
````c++
    lua_State* L = ...;
    lua_getglobal(L, "module");
    lua_getfield(L, -1, "method");
    lua_pushintteger(L, 1);
    lua_pushstring(L, "yes");
    lua_pushboolean(L, true);
    lua_call(L, 3, 0);
````
It can be effectively rewritten as:
````c++
    LuaState lua = L;
    lua.getGlobal("module");
    lua.getField(-1, "method");
    lua.push(1);
    lua.push("yes");
    lua.push(true);
    lua.call(3, 0);
````
This low level API is completely optional, and you can still use the C API, or mix the usage. `LuaState` is designed to be a lightweight wrapper, and has very little overhead (if not as fast as the C API), and mostly can be auto-casting to or from `lua_State*`. In the `lua-intf`, `LuaState` and `lua_State*` are inter-changeable, you can pick the coding style you like most.

`LuaState` does not manage `lua_State*` life-cycle, you may take a look at `LuaContext` class for that purpose.
