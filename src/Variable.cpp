#include "Variable.h"


void Variable::setType(KIND t) {
  type = t;
}

void Variable::setValue(VALUE v) {
  value = v;
}

void Variable::setLocal(bool l) {
  local = l;
}

void Variable::setSize(unsigned int s) {
  size = s;
}

void Variable::setOffset(int o) {
  offset = o;
}

void Variable::setIndex(unsigned i) {
  index = i;
}

void Variable::setFirstByte(unsigned f) {
  firstByte = f;
}

void Variable::setLength(unsigned l) {
  length = l;
}

KIND Variable::getType() {
  return type;
}

VALUE Variable::getValue() {
  return value;
}

bool Variable::getLocal() {
  return local;
}

unsigned int Variable::getSize() {
  return size;
}

int Variable::getOffset() {
  return offset;
}

unsigned Variable::getIndex() {
  return index;
}

unsigned Variable::getFirstByte() {
  return firstByte;
}

unsigned Variable::getLength() {
  return length;
}

bool Variable::isInitialized() {
  return init;
}

string Variable::toString() {
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


void Variable::copy(Variable *dest) {
  dest->setType(type);
  dest->setValue(value);
  dest->setSize(size);

  // we never want to rewrite these fields in heap objects
  if (!dest->isInHeap()) {
    dest->setOffset(offset);
    dest->setIndex(index);
    dest->setFirstByte(firstByte);
  }

  dest->setLength(length);
  dest->setLocal(local);
}

bool Variable::isInHeap() {
  return heap;
}
