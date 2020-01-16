#include "scrpt.hh"

/*******************************************************/
void
Scrpt::Append (const void *bytes, int size)
{
    if (offset + size >= 32)
        return;
    memcpy (this->data + offset, bytes, size);
    this->offset += size;
}

/*******************************************************/
Scrpt::~Scrpt () { delete[] data; }

/*******************************************************/
Scrpt::Scrpt (short opcodeId)
{
    this->data   = new unsigned char[32];
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
#define operator(ID, TYPE)                                                     \
    void Scrpt::operator<< (TYPE n)                                            \
    {                                                                          \
        unsigned char typeId = ID;                                             \
        Append (&typeId, 1);                                                   \
        Append (&n, sizeof (TYPE));                                            \
    }

operator(4, char) operator(1, int) operator(2, short) operator(6, float)
                                                      operator(2, GlobalVar)
                                                      operator(3, LocalVar)
