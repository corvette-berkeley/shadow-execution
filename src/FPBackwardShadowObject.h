/**
 * @file FPBackwardShadowObject.h
 * @brief FPBackwardShadowObject Declarations.
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

// Author: Cindy Rubio-Gonzalez

#ifndef FP_BACKWARD_SHADOW_OBJECT_H
#define FP_BACKWARD_SHADOW_OBJECT_H

#include <iostream>

struct FPBackwardShadowObject {

private:
	double val;  // Value in higher precision
	long double abserr;  // Maximum absolute error
	int line;  // source line information

public:
	FPBackwardShadowObject(double v = 0, int l = -1) : val(v), abserr(0), line(l) {}

	void print() {
		std::cout << "line: " << line << ", val: " << val << ", abserr: " << abserr << endl;
		return;
	}
};

#endif /* FP_BACKWARD_SHADOW_OBJECT_H_ */
