#ifndef IVALUE_H_
#define IVALUE_H_

#include "Common.h"

using namespace std;

class IValue {

  private:
    KIND type; // see Common.h for definitions
    VALUE value; // union type (use extension later on)
    unsigned size; // size of data the pointer points to
    int offset;
    int bitOffset; // to represent data smaller than a byte; value ranges from 0 to 7
    unsigned index; // index of this object
    unsigned firstByte; // the length in byte to the base pointer
    unsigned length; // number of elements (only for pointers)
    SCOPE scope; // defined in Constants.h
    int lineNumber; // source line number
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
    IValue(KIND t, VALUE v, SCOPE s): type(t), value(v), size(0), offset(0), bitOffset(0), index(0), firstByte(0), length(1), scope(s), lineNumber(0) {}

    IValue(KIND t, VALUE v): type(t), value(v), size(0), offset(0), bitOffset(0), index(0), firstByte(0), length(1), scope(REGISTER), lineNumber(0) {}

    IValue(KIND t, VALUE v, unsigned f): type(t), value(v), size(0), offset(0), bitOffset(0), index(0), firstByte(f), length(1), scope(REGISTER), lineNumber(0) {}

    IValue(KIND t, VALUE v, unsigned s, int o, int i, unsigned e): type(t), value(v), size(s), offset(o), bitOffset(0), index(i), firstByte(0), length(e), scope(REGISTER), lineNumber(0) {}

    IValue(KIND t): type(t), size(0), offset(0), bitOffset(0), index(0), firstByte(0), length(1), scope(REGISTER), lineNumber(0) {}

    IValue(): type(INV_KIND), size(0), offset(0), bitOffset(0), index(0), firstByte(0), length(1), scope(REGISTER), lineNumber(0) {}

    void setType(KIND t);

    void setValue(VALUE v);

    void setScope(SCOPE s);

    void setSize(unsigned int s);

    void setOffset(int o);

    void setBitOffset(int bo);

    void setIndex(unsigned i);

    void setFirstByte(unsigned f);

    void setLength(unsigned l);

    void setInitialized();

    void setShadow(void* addr);

    void setLineNumber(int l);

    KIND getType();

    VALUE getValue();

    int64_t getIntValue();

    void* getPtrValue();

    double getFlpValue();

    SCOPE getScope();

    bool isInitialized();

    unsigned getIndex();

    unsigned getFirstByte();

    unsigned getLength();

    unsigned int getSize();

    int getOffset();

    int getBitOffset();

    void* getShadow();

    int getLineNumber();

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
