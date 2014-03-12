/**
 * @file FPInstabilityShadowObject.h
 * @brief FPInstabilityShadowObject Declarations.
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
 * 3. All advertising materials mentioning features or use of this software must
 * display the following acknowledgement: This product includes software
 * developed by the UC Berkeley.
 *
 * 4. Neither the name of the UC Berkeley nor the names of its contributors may
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

#ifndef FP_INSTABILITY_SHADOW_OBJECT_H
#define FP_INSTABILITY_SHADOW_OBJECT_H

template <class T>
class FPInstabilityShadowObject {
  public:
    typedef enum {
      NONE, CANCELLATION, CONCRETEDIFFERENCE, SYNC
    } ABNORMALTYPE;

  private:
    T val;    // Value in higher precision
    int td;             // Depth of the computation tree
    int pc;             // Address of the instruction that wrote to the variable lastly
    int fid;            // Id of the file containing the pc instruction
    int mcb;            // Maximum cancellation badness
    int mcbSrc;         // Instruction that caused maximum cancellation badness
    long double sre;    // Sum or relative errors
    long double mre;    // Maximum relative error
    int mreSrc[2];      // Pair of instruction addresses that point to the instructions 
                        // that computed the operands that caused the maximum relative
                        // error
    int scb;            // Sum of cancellation badnesses
    ABNORMALTYPE abt;    // Type of abnormal behavior

  public:
    FPInstabilityShadowObject(): val(0), td(1), pc(0), fid(0), mcb(0), mcbSrc(-1), sre(0), mre(0), scb(0), abt(NONE) {};

    FPInstabilityShadowObject(T v, int line): val(v), td(1), pc(line), fid(0), mcb(0), mcbSrc(-1), sre(0), mre(0), scb(0), abt(NONE) {
      mreSrc[0] = -1;
      mreSrc[1] = -1;
    }

    FPInstabilityShadowObject(const FPInstabilityShadowObject& fpISO) {
      create(fpISO);
    };

    ~FPInstabilityShadowObject() {
      uncreate();
    }

    FPInstabilityShadowObject& operator=(const FPInstabilityShadowObject& fpISO) {
      if (&fpISO != this) {

        // free the object in the left-hand side
        uncreate();

        // copy elements from the right-hand side to the left-hand side
        create(fpISO);
      }

      return *this;
    };

    T getValue() const { return val; };

    int getComputationDepth() const { return td; };

    int getPC() const { return pc; };

    int getFileID() const { return fid; }

    int getMaxCBad() const { return mcb; };

    int getMaxCBadSource() const { return mcbSrc; };

    long double getSumRelErr() const { return sre; };

    long double getMaxRelErr() const { return mre; };

    int getMaxRelErrSource(int i) const { return mreSrc[i]; };

    int getSumCBad() const { return scb; };

    ABNORMALTYPE getAbnormalType() const { return abt; };

    void setValue(T val) { this->val = val; };

    void setComputationDepth(int td) { this->td = td; };

    void setPC(int pc) { this->pc = pc; };

    void setFileID(int fid) { this->fid = fid; }

    void setMaxCBad(int mcb) { this->mcb = mcb; };

    void setMaxCBadSource(int mcbSrc) {
      this->mcbSrc = mcbSrc;
    };

    void setSumRelErr(long double sre) {
      this->sre = sre;
    };

    void setMaxRelErr(long double mre) {
      this->mre = mre;
    };

    void setMaxRelErrSource(int i, int pc) {
      this->mreSrc[i] = pc;
    };

    void setSumCBad(int scb) {
      this->scb = scb;
    };

    void setAbnormalType(ABNORMALTYPE abt) {
      this->abt = abt;
    };

    void copyTo(FPInstabilityShadowObject *fpISO) {
      fpISO->setValue(val);
      fpISO->setComputationDepth(td);
      fpISO->setPC(pc);
      fpISO->setFileID(fid);
      fpISO->setMaxCBad(mcb);
      fpISO->setMaxCBadSource(mcbSrc);
      fpISO->setSumRelErr(sre);
      fpISO->setMaxRelErr(mre);
      fpISO->setMaxRelErrSource(0, mreSrc[0]);
      fpISO->setMaxRelErrSource(1, mreSrc[1]);
      fpISO->setSumCBad(scb);
      fpISO->setAbnormalType(abt);
    };

  private:
    void create(const FPInstabilityShadowObject& fpISO) {
      val = fpISO.getValue();
      td = fpISO.getComputationDepth();
      pc = fpISO.getPC();
      fid = fpISO.getFileID();
      mcb = fpISO.getMaxCBad();
      mcbSrc = fpISO.getMaxCBadSource();
      sre = fpISO.getSumRelErr();
      mre = fpISO.getMaxRelErr();
      mreSrc[0] = fpISO.getMaxRelErrSource(0);
      mreSrc[1] = fpISO.getMaxRelErrSource(1);
      scb = fpISO.getSumCBad();
      abt = fpISO.getAbnormalType();
    };

    void uncreate() {};
};

#endif /* FP_INSTABILITY_SHADOW_OBJECT_H_ */
