/**
 * @file Instrumentation.cpp
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
 */

#include "Common.h"
#include "Instrumentation.h"

/**
 * Create singleton instrumentation object.
 */
Instrumentation* Instrumentation::instance_ = NULL;

Instrumentation::~Instrumentation() {
  for(InstrumenterPtrList::iterator itr = instrumenters_.begin();
      itr < instrumenters_.end();
      itr = instrumenters_.erase(itr)) {
    InstrumenterBase* instrumenter = *itr;
    safe_assert(instrumenter != NULL);
    delete instrumenter;
  }
}

void Instrumentation::RegisterInstrumenter(InstrumenterBase* instrumenter) {
  instrumenters_.push_back(instrumenter);
}

bool Instrumentation::Initialize(Module &M __attribute__((__unused__))) {
  return true;
}

bool Instrumentation::Finalize(Module &M __attribute__((__unused__))) {
  for(InstrumenterPtrList::iterator itr = instrumenters_.begin();
      itr < instrumenters_.end();
      itr = instrumenters_.erase(itr)) {
    InstrumenterBase* instrumenter = *itr;
    safe_assert(instrumenter != NULL);
    outs() << instrumenter->ReportToString();
  }
  return true;
}

void Instrumentation::BeginBasicBlock(BasicBlock* BB, Function* F, Module* M) {
  safe_assert(BB != NULL);	BB_ = BB;
  safe_assert(F != NULL);		F_ = F;
  safe_assert(M != NULL);		M_ = M;
  AS_ = 0U;
}

void Instrumentation::BeginFunction() {
  varCount = 0;
  indices.clear();
}

void Instrumentation::createIndex(uint64_t iid)
{
  indices[iid] = varCount;
  varCount++;
}

int Instrumentation::getIndex(Instruction* inst)
{
  IID iid = static_cast<IID>(reinterpret_cast<ADDRINT>(inst));
  safe_assert(indices.find(iid) != indices.end());
  return indices[iid];
}

int Instrumentation::getFrameSize()
{
  return varCount;
}

bool Instrumentation::CheckAndInstrument(Instruction* I) {
  // find an instrumenter to handle the given instruction
  for(InstrumenterPtrList::iterator itr = instrumenters_.begin(); itr < instrumenters_.end(); ++itr) {
    InstrumenterBase* instrumenter = *itr;
    safe_assert(instrumenter != NULL);
    safe_assert(instrumenter->parent_ == this);

    if(instrumenter->CheckAndInstrument(I)) {
      return true;
    }
  }
  return false;
}
