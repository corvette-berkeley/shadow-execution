#ifndef VARIABLE_H_
#define VARIABLE_H_

#include "Common.h"

using namespace std;

class Variable {

 private:
  KIND type; // see Common.h for definitions
  VALUE value; // union type (use extension later on)
  unsigned int origSize;
  unsigned int currSize; // dataSize
  int offset;
  unsigned int offsetSize;
  bool local;
  bool init;
  MACHINEFLAG flag;
  void* metadata;

 public:
 Variable(KIND t, VALUE v, bool l): type(t), value(v), origSize(64), currSize(0), offset(0), local(l), init(true) {}

 Variable(KIND t, VALUE v, unsigned int s, unsigned int c, int o, bool l): type(t), value(v), origSize(s), currSize(c), offset(o), local(l), init(true) {}

 Variable(KIND t, VALUE v, unsigned int s, unsigned int c, int o, unsigned int os, bool l): 
  type(t), value(v), origSize(s), currSize(c), offset(o), offsetSize(os), local(l), init(true) {}

 Variable(KIND t, bool l): type(t), origSize(64), currSize(0), offset(0), local(l), init(false) {}

 Variable(): type(INV_KIND), origSize(64), currSize(0), offset(0), local(false), init(false) {}
  
  void setType(KIND t);

  void setValue(VALUE v);

  void setLocal(bool l);

  void setOrigSize(unsigned int s);

  void setCurrSize(unsigned int c);

  void setOffset(int o);

  void setOffsetSize(unsigned int os);

  KIND getType();

  VALUE getValue();

  bool getLocal();

  bool isInitialized();

  unsigned int getOrigSize();

  unsigned int getCurrSize();

  int getOffset();

  unsigned int getOffsetSize();

  string toString();

  void copy(Variable* dest);

  bool isEqualPtrSize();

  bool isSmallerPtrSize();

};

#endif /* LOCATION_H_ */
