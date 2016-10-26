#pragma once

#include <lumpy/core.h>

namespace lumpy
{

namespace unittest
{

typedef   void(*func_t)();

class lumpy_api _IUnitTest
{
public:
    static int invoke (const char* type, const char* name=nullptr);

protected:
    static int install(const type_info&, void* pnew, void* pdel);
    static int install(const type_info&, void* func, const char* file, int line);

};

template<class T>
class __declspec(dllexport) IUnitTest
    : public _IUnitTest
{
public:
    using self = T;

    IUnitTest()
    {
        // force init.
        (void)_install_class;
    }

    static auto _install()
    {
        return install(typeid(T), &_new, &_del);
    }

    static auto _install(const char*(*func)(void*), const char* file, int line)
    {
        return install(typeid(T), func, file, line);
    }

    static void _invoke(void* obj, void(T::*func)())
    {
        (static_cast<T*>(obj)->*func)();
    }


private:
    static int _install_class;

    static T*   _new()          { return new T();       }
    static void _del(T* ptr)    { return delete ptr;    }
};

template<class T>
int IUnitTest<T>::_install_class = _install();

}

}

lumpy_abi int lumpy_unittest_invoke(const char* type = nullptr, const char* name = nullptr);

#define unittest(name)                                                      \
struct __declspec(dllexport) name : lumpy::unittest::IUnitTest<name>

#define testcase(name)                                                                  \
static const char* name##_test(void* obj) { _invoke(obj, &name); return __FUNCTION__;}  \
int  _install_##name = _install(&name##_test, __FILE__, __LINE__);                      \
void name()