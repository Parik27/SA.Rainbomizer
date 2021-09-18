#include <cstdint>
#include <injector/injector.hpp>
#include <injector/calling.hpp>

namespace Rainbomizer {

template <typename Ret = void, bool Method = false> class FunctionCb
{
public:
    uintptr_t address = 0;

    template <typename... Args>
    Ret
    Call (Args... args)
    {
        if constexpr (Method)
            return reinterpret_cast<Ret (__thiscall *) (Args...)> (address) (
                args...);
        else
            return reinterpret_cast<Ret (__cdecl *) (Args...)> (address) (
                args...);
    }

    template <typename... Args>
    Ret
    operator() (Args... args)
    {
        if (address != 0)
            return Call (args...);
    }

    explicit operator bool () const { return address != 0; }

    void
    SetAddress (uint32_t addr)
    {
        address = addr;
    }
};

class HookType
{
public:
    struct CALL
    {
        static uintptr_t
        Hook (uintptr_t src, uintptr_t dest)
        {
            uintptr_t orig = injector::GetBranchDestination (src).as_int ();
            injector::MakeCALL (src, dest);

            return orig;
        }
    };

    struct JMP
    {
        static uintptr_t
        Hook (uintptr_t src, uintptr_t dest)
        {
            std::uint32_t orig = injector::GetBranchDestination (src).as_int ();
            injector::MakeJMP (src, dest);

            return orig;
        }
    };
};

class HooksManager
{
private:
    template <auto Cb, typename Ret, auto &Orig, typename... Args>
    static Ret
    Trampoline (Args... args)
    {
        return Cb (Orig, args...);
    }

    template <auto Cb, typename HookType, typename Signature>
    struct RegisterHookClass;

    template <auto Cb, typename HookType, typename Ret, typename OrigType,
              typename... Args>
    struct RegisterHookClass<Cb, HookType, Ret (*) (OrigType, Args...)>
    {
        template <std::uintptr_t addr>
        static void
        Register ()
        {
            static OrigType origType;
            origType.SetAddress (HookType::Hook (
                addr, std::uintptr_t (Trampoline<Cb, Ret, origType, Args...>)));
        }
    };

public:
    template <auto Cb, typename HookType, std::uintptr_t... addrs>
    static void
    Add ()
    {
        (..., RegisterHookClass<Cb, HookType,
                                decltype (Cb)>::template Register<addrs> ());
    }

    template <auto Cb, std::uintptr_t... addrs>
    static void
    Add ()
    {
        (..., RegisterHookClass<Cb, HookType::CALL,
                                decltype (Cb)>::template Register<addrs> ());
    }
};

} // namespace Rainbomizer
