#ifndef LOCATION_H_
#define LOCATION_H_

#include "Common.h"

using namespace std;

class Location {

 private:
  KIND type; // see Common.h for definitions
  VALUE value; // union type (use extension later on)
  bool local;

 public:
 Location(KIND t, VALUE v, bool l): type(t), value(v), local(l) {}

 Location(KIND t, bool l): type(t), local(l) {}
  
  void setType(KIND t);

  void setValue(VALUE v);

  void setLocal(bool l);

  KIND getType();

  VALUE getValue();

  bool getLocal();

  void print();

};

#endif /* LOCATION_H_ */
