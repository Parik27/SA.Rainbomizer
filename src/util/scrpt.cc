#include "util/scrpt.hh"

/*******************************************************/
void
Scrpt::Append (const void *bytes, int size)
{
    if (offset + size >= 128)
        return;
    memcpy (this->data + offset, bytes, size);

    this->offset += size;
}

/*******************************************************/
unsigned char *
Scrpt::CreateNop (unsigned char *dst, int size)
{
    if (size > 4)
        {
            Scrpt         opcode = Scrpt (0x661);
            unsigned char typeId = 0xE;
            unsigned char length = size - 4;

            opcode.Append (&typeId, 1);
            opcode.Append (&length, 1);

            memcpy (dst, opcode.GetData (), opcode.offset);
        }
    else
        {
            Scrpt opcode = Scrpt (0x0);
            memcpy (dst, opcode.GetData (), opcode.offset);
        }

    return dst + size;
}

/*******************************************************/
unsigned char *
Scrpt::CreateNop (unsigned char *dst, int start, int end)
{
    return CreateNop (dst + start, end - start);
}

/*******************************************************/
Scrpt::~Scrpt () { delete[] data; }

/*******************************************************/
Scrpt::Scrpt (short opcodeId)
{
    this->data   = new unsigned char[128];
    this->offset = 0;

    Append (&opcodeId, sizeof (short));
}

/*******************************************************/
void
Scrpt::operator<< (const char *str)
{
    unsigned char typeId = 0xE;
    unsigned char len    = strlen (str);

    Append (&typeId, 1);
    Append (&len, 1);

    Append (str, len);
}

/*******************************************************/
void
Scrpt::operator<< (int *ptr)
{
    *this << LocalVar (this->savedParams.size ());
    this->savedParams.push_back (ptr);
}

/*******************************************************/
void
Scrpt::StoreParameters (CRunningScript *scr)
{
    int j = 0;
    for (auto i : savedParams)
        *i = scr->m_aLocalVars[j++];
}

/*******************************************************/
#define operator(ID, TYPE)                                                     \
    void Scrpt::operator<< (TYPE n)                                            \
    {                                                                          \
        unsigned char typeId = ID;                                             \
        Append (&typeId, 1);                                                   \
        Append (&n, sizeof (TYPE));                                            \
    }

operator(4, char);
operator(1, int);
operator(2, short);
operator(6, float);
operator(2, GlobalVar);
operator(3, LocalVar);
