#ifndef IVALUE_H_
#define IVALUE_H_

#include "Common.h"

using namespace std;

class IValue {

  private:
    KIND type; // see Common.h for definitions
    VALUE value; // union type (use extension later on)
    unsigned size; // dataSize
    int offset;
    unsigned index; // index of this object
    unsigned firstByte;
    unsigned length; // number of elements (only for pointers)
    bool local;
    MACHINEFLAG flag;
    void* metadata;

  public:
    IValue(KIND t, VALUE v, bool l): type(t), value(v), size(0), offset(0), index(0), firstByte(0), length(0), local(l) {}

    IValue(KIND t, VALUE v, unsigned f, bool l): type(t), value(v), size(0), offset(0), index(0), firstByte(f), length(0), local(l) {}

    IValue(KIND t, VALUE v, unsigned s, int o, int i, unsigned e, bool l): type(t), value(v), size(s), offset(o), index(i), firstByte(0), length(e), local(l) {}

    IValue(KIND t, bool l): type(t), size(0), offset(0), index(0), firstByte(0), length(0), local(l) {}

    IValue(): type(INV_KIND), size(0), offset(0), index(0), firstByte(0), length(0), local(false) {}

    void setType(KIND t);

    void setValue(VALUE v);

    void setLocal(bool l);

    void setSize(unsigned int s);

    void setOffset(int o);

    void setIndex(unsigned i);

    void setFirstByte(unsigned f);

    void setLength(unsigned l);

    KIND getType();

    VALUE getValue();

    bool getLocal();

    unsigned getIndex();

    unsigned getFirstByte();

    unsigned getLength();

    unsigned int getSize();

    int getOffset();

    string toString();

    void copy(IValue* dest);

    /**
     * Read a chunk of byte from a pointer value, given the offset to read from
     * and the number of bytes to read.
     *
     * @note This function is specialized for pointer value. 
     *
     * @param offset the offset to start reading from.
     * @param byte the number of bytes to read.
     *
     * @return a VALUE object that wrap all the bytes readed.
     */
    VALUE readValue(int offset, int byte);

    /**
     * Write a chunk of byte to a pointer value, given the offset to start
     * writing to, the number of bytes to write, and the content to write.
     *
     * This function updates the current pointer value.
     *
     * @note This function is specialized for pointer value.
     *
     * @param offset the offset to start writing to.
     * @param byte the number of bytes to write.
     * @param value the value to write to.
     */
    void writeValue(int offset, int byte, uint8_t value);
};

#endif /* IVALUE_H_ */
