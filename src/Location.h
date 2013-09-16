#ifndef LOCATION_H_
#define LOCATION_H_

using namespace std;

class Location {

 private:
  int type;
  long double value;
  bool local;

 public:
 Location(int t, long double v, bool l): type(t), value(v), local(l) {}

 Location(int t, bool l): type(t), value(0), local(l) {}
  
  void setType(int t);

  void setValue(long double v);

  void setLocal(bool l);

  int getType();

  long double getValue();

  bool getLocal();

};

#endif /* LOCATION_H_ */
