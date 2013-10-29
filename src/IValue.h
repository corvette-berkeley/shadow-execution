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
    SCOPE scope; // defined in Constants.h
    MACHINEFLAG flag;
    void* shadow;

    /**
     * Write a chunk of byte to value. This functions returns the actual number
     * of byte written.
     *
     * @note the actual number of byte written is min(maxOffset-offset+1, byte)
     * where maxOffset is size of this value in byte minus one.
     *
     * @param offset the offset to start writing to.
     * @param byte the expected number of byte to write to.
     * @param content the content to read from.
     */
    int setValue(int offset, int byte, uint8_t* content);

  public:
    IValue(KIND t, VALUE v, SCOPE s): type(t), value(v), size(0), offset(0), index(0), firstByte(0), length(1), scope(s) {}

    IValue(KIND t, VALUE v): type(t), value(v), size(0), offset(0), index(0), firstByte(0), length(1), scope(REGISTER) {}

    IValue(KIND t, VALUE v, unsigned f): type(t), value(v), size(0), offset(0), index(0), firstByte(f), length(1), scope(REGISTER) {}

    IValue(KIND t, VALUE v, unsigned s, int o, int i, unsigned e): type(t), value(v), size(s), offset(o), index(i), firstByte(0), length(e), scope(REGISTER) {}

    IValue(KIND t): type(t), size(0), offset(0), index(0), firstByte(0), length(1), scope(REGISTER) {}

    IValue(): type(INV_KIND), size(0), offset(0), index(0), firstByte(0), length(1), scope(REGISTER) {}

    void setType(KIND t);

    void setValue(VALUE v);

    void setScope(SCOPE s);

    void setSize(unsigned int s);

    void setOffset(int o);

    void setIndex(unsigned i);

    void setFirstByte(unsigned f);

    void setLength(unsigned l);

    void setInitialized();

    void setShadow(void* addr);

    KIND getType();

    VALUE getValue();

    SCOPE getScope();

    bool isInitialized();

    unsigned getIndex();

    unsigned getFirstByte();

    unsigned getLength();

    unsigned int getSize();

    int getOffset();

    void* getShadow();

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
    VALUE readValue(int offset, KIND type);

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
     * @param src the source to get value form.
     */
    void writeValue(int offset, int byte, IValue* src);

    /**
     * Check whether this is an IValue by comparing its type with its expected
     * type.
     *
     * @param t the expected type of this iValue.
     *
     * @return true if this is an IValue; false otherwise.
     */
    bool isIValue(KIND t);
};

#endif /* IVALUE_H_ */
