#include <cstring>
#include <cstdio>
#include "base.hh"
#include "functions.hh"
#include <vector>
#include <memory>

struct GlobalVar
{
    short val;
    GlobalVar (short val) { this->val = val * 4; }
};
struct LocalVar
{
    short val;
    LocalVar (short val) { this->val = val; }
};

/*******************************************************/
class Scrpt
{
    unsigned char *    data;
    int                offset;
    std::vector<int *> savedParams;

    void Append (const void *bytes, int size);

public:
    Scrpt (short opcodeId);
    ~Scrpt ();

    unsigned char *
    GetData ()
    {
        return data;
    };

    void operator<< (char n);
    void operator<< (int n);
    void operator<< (short n);
    void operator<< (float n);
    void operator<< (GlobalVar n);
    void operator<< (LocalVar n);
    void operator<< (int *n);
    void operator<< (const char *str);

    void StoreParameters (CRunningScript *scr);

    /*******************************************************/
    template <typename T>
    void
    Pack (T value)
    {
        operator<< (value);
    }

    /*******************************************************/
    template <typename First, typename... Rest>
    void
    Pack (First first, Rest... rest)
    {
        Pack (first);
        Pack (rest...);
    }

    void
    Pack ()
    {
    }

    /*******************************************************/
    template <typename... Args>
    static unsigned char *
    CreateOpcode (short opcodeId, const char *name, unsigned char *dst,
                  Args... args)
    {
        Scrpt opcode = Scrpt (opcodeId);
        opcode.Pack (args...);

        memcpy (dst, opcode.GetData (), opcode.offset);
        return dst + opcode.offset;
    }

    static unsigned char *CreateNop (unsigned char *dst, int size);
    static unsigned char *CreateNop (unsigned char *dst, int start, int end);

    /*******************************************************/
    template <typename... Args>
    static void
    CallOpcode (short opcodeId, const char *name, Args... args)
    {
        CRunningScript scr;
        scr.Init ();

        Scrpt opcode = Scrpt (opcodeId);
        opcode.Pack (args...);

        scr.m_pBaseIP    = opcode.GetData ();
        scr.m_pCurrentIP = scr.m_pBaseIP;

        static std::unique_ptr<int> originalScriptParams (new int[10]);
        memcpy (originalScriptParams.get (), ScriptParams, 10 * sizeof (int));

        scr.ProcessOneCommand ();
        opcode.StoreParameters (&scr);

        memcpy (ScriptParams, originalScriptParams.get (), 10 * sizeof (int));
    }
};
