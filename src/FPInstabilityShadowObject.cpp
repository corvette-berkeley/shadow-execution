/**
 * @file FPInstabilityShadowObject.cpp
 * @brief 
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

#include "FPInstabilityShadowObject.h"

long double FPInstabilityShadowObject::getValue() {
  return val;
}

int FPInstabilityShadowObject::getComputationDepth() {
  return td;
}

int FPInstabilityShadowObject::getPC() {
  return pc;
}

int FPInstabilityShadowObject::getMaxCBad() {
  return mcb;
}

int FPInstabilityShadowObject::getMaxCBadSource() {
  return mcbSrc;
}

long double FPInstabilityShadowObject::getSumRelErr() {
  return sre;
}

long double FPInstabilityShadowObject::getMaxRelErr() {
  return mre;
}

int FPInstabilityShadowObject::getMaxRelErrSource(int i) {
  return mreSrc[i];
}

int FPInstabilityShadowObject::getSumCBad() {
  return scb;
}

void FPInstabilityShadowObject::setValue(long double val) {
  this->val = val;
}

void FPInstabilityShadowObject::setComputationDepth(int td) {
  this->td = td;
}

void FPInstabilityShadowObject::setPC(int pc) {
  this->pc = pc;
}

void FPInstabilityShadowObject::setMaxCBad(int mcb) {
  this->mcb = mcb;
}

void FPInstabilityShadowObject::setMaxCBadSource(int mcbSrc) {
  this->mcbSrc = mcbSrc;
}

void FPInstabilityShadowObject::setSumRelErr(long double sre) {
  this->sre = sre;
}

void FPInstabilityShadowObject::setMaxRelErr(long double mre) {
  this->mre = mre;
}

void FPInstabilityShadowObject::setMaxRelErrSource(int i, int pc) {
  this->mreSrc[i] = pc;
}

void FPInstabilityShadowObject::setSumCBad(int scb) {
  this->scb = scb;
}

void FPInstabilityShadowObject::copyTo(FPInstabilityShadowObject *fpISO) {
  fpISO->setValue(val);
  fpISO->setComputationDepth(td);
  fpISO->setPC(pc);
  fpISO->setMaxCBad(mcb);
  fpISO->setMaxCBadSource(mcbSrc);
  fpISO->setSumRelErr(sre);
  fpISO->setMaxRelErr(mre);
  fpISO->setMaxRelErrSource(0, mreSrc[0]);
  fpISO->setMaxRelErrSource(1, mreSrc[1]);
  fpISO->setSumCBad(scb);
}
