#include <map>

#include <lumpy/unittest.h>

namespace lumpy
{
namespace unittest
{

struct ClassInfo
{
    size_t hash;
    void* (*pnew)();
    void  (*pdel)(void*);
};

struct MethodInfo
{
    void*       func;
    const char* name;
    const char* file;
    int         line;

    bool invoke(void* obj) const
    {
        static const auto run_str = "[ RUN      ]";
        static const auto ok_str  = "[     OK   ]";
        static const auto fail_str= "[     FAIL ]";
        static const auto fmt_str = "%32s \n";

        printf(run_str);
        printf(fmt_str,name);

        auto method = static_cast<void(*)(void*)>(func);
        auto status = true;
        try {
            method(obj);

            printf(ok_str);
            printf(fmt_str, name);

        } catch(...) {
            printf(fail_str);
            printf(fmt_str, name);
            return false;
        }

        return true;
    }
};

static std::map<string, ClassInfo>                      gClasses;
static std::map<size_t, std::map<string, MethodInfo>>   gMethods;

int _IUnitTest::install(const type_info& type, void* pnew, void* pdel)
{
    auto class_info = gClasses.find(type.name());
    if (class_info != gClasses.end()) return -1;
    gClasses[type.name()] = { type.hash_code(), static_cast<void*(*)()>(pnew), static_cast<void(*)(void*)>(pdel) };
    return 0;
}

int _IUnitTest::install(const type_info& type, void* func, const char* file, int line)
{
    auto name        = static_cast<const char*(*)(void*)>(func)(nullptr);
    auto& class_info = gMethods[type.hash_code()];

    auto  method_itr = class_info.find(name);
    if (method_itr != class_info.end()) return -1;

    class_info[name] = { func, name, file, line };
    return 0;
}

int _IUnitTest::invoke(const char* type, const char* name)
{
    for (auto class_itr  = type==nullptr ? gClasses.begin() : gClasses.find(type);
         class_itr      != gClasses.end();
         ++class_itr)
    {
        auto object = class_itr->second.pnew();
        {
            auto& methods = gMethods[class_itr->second.hash];

            if (name == nullptr || name[0] == '\0') {
                for (auto& pair : methods) {
                    auto& method = pair.second;
                    method.invoke(object);
                }
            }
            else {
                auto method_itr = methods.find(name);
                if (method_itr == methods.end()) return -1;
                method_itr->second.invoke(object);
            }

        }
        class_itr->second.pdel(object);

        if (type != nullptr) break;
    }
    return 0;
}

}

}

lumpy_abi int lumpy_unittest_invoke(const char* type, const char* name)
{
    using namespace lumpy::unittest;
    return _IUnitTest::invoke(type, name);
}



