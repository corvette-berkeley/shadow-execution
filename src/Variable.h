#ifndef VARIABLE_H_
#define VARIABLE_H_

#include "Common.h"

using namespace std;

class Variable {

 private:
  KIND type; // see Common.h for definitions
  VALUE value; // union type (use extension later on)
  unsigned int size;
  int offset;
  bool local;
  MACHINEFLAG flag;
  void* metadata;

 public:
 Variable(KIND t, VALUE v, bool l): type(t), value(v), size(0), offset(0), local(l) {}

 Variable(KIND t, VALUE v, unsigned int s, int o, bool l): type(t), value(v), size(s), offset(o), local(l) {}

 Variable(KIND t, bool l): type(t), size(0), offset(0), local(l) {}

 Variable(): type(INV_KIND), size(0), offset(0), local(false) {}
  
  void setType(KIND t);

  void setValue(VALUE v);

  void setLocal(bool l);

  KIND getType();

  VALUE getValue();

  bool getLocal();

  string toString();

};

#endif /* LOCATION_H_ */
