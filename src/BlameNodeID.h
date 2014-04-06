/**
 * @file BlameNodeID.h
 * @brief BlameNodeID Declarations.
 */

/*
 * Copyright (c) 2013, UC Berkeley All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1.  Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the UC Berkeley nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY UC BERKELEY ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL UC BERKELEY BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

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
