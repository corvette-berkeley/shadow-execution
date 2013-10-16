#ifndef VARIABLE_H_
#define VARIABLE_H_

#include "Common.h"

using namespace std;

class Variable {

 private:
  KIND type; // see Common.h for definitions
  VALUE value; // union type (use extension later on)
  unsigned size; // dataSize
  int offset;
  unsigned index; // index of this object
  unsigned firstByte;
  unsigned length; // number of elements (only for pointers)
  bool local;
  bool init;
  MACHINEFLAG flag;
  void* metadata;

 public:
 Variable(KIND t, VALUE v, bool l): type(t), value(v), size(0), offset(0), local(l), init(true) {}

 Variable(KIND t, VALUE v, unsigned int s, int o, bool l): type(t), value(v), size(s), offset(o), local(l), init(true) {}

 Variable(KIND t, bool l): type(t), size(0), offset(0), local(l), init(false) {}

 Variable(): type(INV_KIND), size(0), offset(0), local(false), init(false) {}
  
  void setType(KIND t);

  void setValue(VALUE v);

  void setLocal(bool l);

  void setSize(unsigned int s);

  void setOffset(int o);

  KIND getType();

  VALUE getValue();

  bool getLocal();

  unsigned getIndex();

  unsigned getFirstByte();

  unsigned getLength();

  unsigned int getSize();

  int getOffset();

  bool isInitialized();

  string toString();

  void copy(Variable* dest);

};

#endif /* LOCATION_H_ */
