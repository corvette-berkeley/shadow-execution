/**
 * @file Constants.h
 * @brief Constants Declaration.
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

#ifndef CONSTANTS_DEFS_H_
#define CONSTANTS_DEFS_H_

typedef enum {
  BITWISE_INVALID,
  SHL, LSHR, ASHR, 
  AND, OR, XOR
} BITWISE;

typedef enum {
  BINOP_INVALID,
  ADD, SUB, MUL, UDIV, SDIV, UREM, SREM,
  FADD, FSUB, FMUL, FDIV, FREM
} BINOP;

typedef enum {
  CASTOP_INVALID,
  BITCAST, TRUNC, ZEXT,
  FPEXT, FPTOSI, FPTOUI, FPTRUNC,
  INTTOPTR, PTRTOINT,
  SEXT, SITOFP, UITOFP
} CASTOP;

typedef enum {
  UNINTERPRETEDINST_INVALID,
  EXTRACTELEMENT, INSERTELEMENT, SHUFFLEVECTOR,
  INSERTVALUE,
  UNREACHABLE, RESUME, INVOKE,
  SELECT, LANDINGPAD,
  FENCE,
  VAARG,
  ATOMICRMW
} UNINTERPRETEDINST;

typedef enum {
  // not implemented yet
} MACHINEFLAG;

typedef enum {
  SCOPE_INVALID,
  REGISTER,
  CONSTANT,
  LOCAL,
  GLOBAL
} SCOPE;

#endif /* CONSTANTS_DEFS_H_ */
