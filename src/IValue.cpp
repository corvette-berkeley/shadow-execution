#include "IValue.h"


void IValue::setType(KIND t) {
  type = t;
}

void IValue::setValue(VALUE v) {
  value = v;
}

void IValue::setLocal(bool l) {
  local = l;
}

void IValue::setSize(unsigned int s) {
  size = s;
}

void IValue::setOffset(int o) {
  offset = o;
}

void IValue::setIndex(unsigned i) {
  index = i;
}

void IValue::setFirstByte(unsigned f) {
  firstByte = f;
}

void IValue::setLength(unsigned l) {
  length = l;
}

KIND IValue::getType() {
  return type;
}

VALUE IValue::getValue() {
  return value;
}

bool IValue::getLocal() {
  return local;
}

unsigned int IValue::getSize() {
  return size;
}

int IValue::getOffset() {
  return offset;
}

unsigned IValue::getIndex() {
  return index;
}

unsigned IValue::getFirstByte() {
  return firstByte;
}

unsigned IValue::getLength() {
  return length;
}

string IValue::toString() {
  std::stringstream s;

  if (local) {
    s << "LOCAL    => ";
  }
  else {
    s << "REGISTER => ";
  }

  switch(type) {
  case PTR_KIND:
    s << "[PTR: " << value.as_ptr << "]";
    break;
  case ARRAY_KIND:
    s << "[ARRAY: " << value.as_ptr << "]";
    break;
  case STRUCT_KIND:
    s << "[STRUCT: " << value.as_ptr << "]";
    break;
  case INT1_KIND:
    s << "[INT1: " << value.as_int << "]";
    break;
  case INT8_KIND:
    s << "[INT8: " << value.as_int << "]";
    break;
  case INT16_KIND:
    s << "[INT16: " << value.as_int << "]";
    break;
  case INT32_KIND:
    s << "[INT32: " << value.as_int << "]";
    break;
  case INT64_KIND:
    s << "[INT64: " << value.as_int << "]";
    break;
  case FLP32_KIND:
    // TODO: this assumes value.as_flp returns a long double value
    s << "[FLP32: " << (float)value.as_flp << "]";
    break;
  case FLP64_KIND:
    // TODO: this assumes value.as_flp returns a long double value
    s << "[FLP64: " << (double) value.as_flp << "]";
    break;
  case FLP80X86_KIND:
    // TODO: this is incorrect because value.as_flp returns a double
    s << "[FLP80X86: " << (long double) value.as_flp << "]";
    break;
  default: 
    cout << "Here?\n";
    safe_assert(false);
    break;
  }

  s << ", Size: " << size;
  s << ", Offset: " << offset; 
  s << ", Index: " << index;
  s << ", FirstByte: " << firstByte;
  s << ", Length: " << length;

  return s.str();
}

void IValue::copy(IValue *dest) {
  dest->setType(type);
  dest->setValue(value);
  dest->setSize(size);
  dest->setOffset(offset);
  dest->setIndex(index);
  dest->setLength(length);
  dest->setLocal(local);
  // note: we do never overwrite the field firstByte
}

VALUE IValue::readValue(int offset, int byte) {
  IValue* valueArray = static_cast<IValue*>(value.as_ptr);
  VALUE value;

  if (offset == 0 && KIND_GetSize(valueArray[index].getType()) == byte) {
    // efficient code for common cases
    value = valueArray[index].getValue();
  } else {
    // uncommon cases
    unsigned nextIndex = index;
    int totalByte = 0;

    while (totalByte < offset + byte) {
      IValue value = valueArray[nextIndex];
      totalByte += KIND_GetSize(value.getType());
      nextIndex++;
    }

    // totalContent stores the accumulative content from IValue at index 
    // to IValue at nextIndex-1
    uint8_t* totalContent = (uint8_t*) malloc(totalByte*sizeof(uint8_t));
    int tocInx = 0;

    for (unsigned i = index; i < nextIndex; i++) {
      IValue value = valueArray[i];
      KIND type = value.getType();
      int size = KIND_GetSize(type);
      VALUE valValue = value.getValue();
      uint8_t* valueContent = (uint8_t*) &valValue;
      for (int j = 0; j < size; j++) {
        totalContent[tocInx] = valueContent[j];
        tocInx++;
      }
    }

    // truncate content from total content
    uint8_t* truncContent = (uint8_t*) calloc(byte, sizeof(uint8_t));
    int trcInx = 0;

    for (int i = 0; i < totalByte; i++) {
      if (i >= offset && i < offset + byte) {
        truncContent[trcInx] = totalContent[i];
        trcInx++;
      }
    }

    // cast truncate content array to an actual value 
    int64_t* truncValue = (int64_t*) truncContent;

    value.as_int = truncValue[0];
  }

  return value;
}

int IValue::setValue(int offset, int byte, uint8_t* content) {
  uint8_t* valueBytes = (uint8_t*)&value;
  int maxOffset = KIND_GetSize(type)-1;
  if (byte > maxOffset - offset +1) {
    byte = maxOffset - offset + 1;
  }

  for (int i = 0; i < byte; i++) {
    valueBytes[i+offset] = content[i];
  }

  return byte;
} 

void IValue::writeValue(int offset, int byte, IValue* src) {
  IValue* valueArray = static_cast<IValue*>(value.as_ptr);

  if (offset == 0 && KIND_GetSize(valueArray[index].getType()) == byte) {
    cout << "here" << endl;
    // efficient code for common case
    //valueArray[index].copy(src);
    src->copy(&valueArray[index]);
  } else {
    VALUE srcValue = src->getValue();

    // get content from source value
    uint8_t* srcContent = (uint8_t*)(&srcValue);
    uint8_t* content = (uint8_t*) malloc(byte*sizeof(uint8_t*));
    for (int i = 0; i < byte; i++) {
      content[i] = srcContent[i];
    }

    // write the content to this value array
    int currentIndex = index;
    int byteWrittens = 0;

    while (byteWrittens < byte) {
      IValue *currentValue = &valueArray[currentIndex];
      cout << "=== Ivalue: " << currentValue->toString() << endl;
      cout << "=== current value: " << currentValue->getValue().as_int << endl;
      byteWrittens += currentValue->setValue(offset, byte - byteWrittens, content);
      cout << "=== current value after: " << currentValue->getValue().as_int << endl;
      cout << "=== Ivalue after: " << currentValue->toString() << endl;
      content += byteWrittens;
      currentIndex++;
      offset = 0;
    }
  }
}
