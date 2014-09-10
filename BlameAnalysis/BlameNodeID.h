// Author: Cuong Nguyen

#ifndef BLAME_NODE_ID_H
#define BLAME_NODE_ID_H

#include "BlameTreeUtilities.h"

class BlameNodeID {
  private:
    int dpc;
    PRECISION precision;

  public:
    BlameNodeID(int dp, PRECISION prec): dpc(dp), precision(prec) {};

    BlameNodeID(const BlameNodeID& bnID) { create(bnID); };

    ~BlameNodeID() {
      uncreate();
    };

    BlameNodeID& operator=(const BlameNodeID& bnID) {
      if (&bnID != this) {
        // free the object in the left-hand side
        uncreate();

        // copy elements from the right-hand side to the left-hand side
        create(bnID);
      }

      return *this;
    }

    bool operator<(const BlameNodeID& bnID) const {
      if (dpc == bnID.dpc)
      {
        return precision < bnID.getPrecision(); 
      }

      return dpc < bnID.getDPC();
    };

    int getDPC() const {return dpc;};

    void setDPC(int dpc) { this->dpc = dpc; };

    PRECISION getPrecision() const { return precision; };

    void setPrecision(PRECISION precision) { this->precision = precision; };

  private:
    void create(const BlameNodeID& bnID) {
      dpc = bnID.getDPC();
      precision = bnID.getPrecision();
    };

    void uncreate() {};
};

#endif
