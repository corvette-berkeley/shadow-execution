#ifndef VARIABLE_H_
#define VARIABLE_H_

#include "Common.h"

using namespace std;

class Variable {

 private:
  KIND type; // see Common.h for definitions
  VALUE value; // union type (use extension later on)
  bool local;
  MACHINEFLAG flag;
  void* metadata;

 public:
 Variable(KIND t, VALUE v, bool l): type(t), value(v), local(l) {}

 Variable(KIND t, bool l): type(t), local(l) {}
  
  void setType(KIND t);

  void setValue(VALUE v);

  void setLocal(bool l);

  KIND getType();

  VALUE getValue();

  bool getLocal();

  string toString();

};

#endif /* LOCATION_H_ */
