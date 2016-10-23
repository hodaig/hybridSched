/*
 * HSConditionFunc.h
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#ifndef HSCONDITIONFUNC_H_
#define HSCONDITIONFUNC_H_

#include "HSCondition.h"

class HSConditionFunc: public HSCondition {
private:
	bool (*_function)(void);

public:
	HSConditionFunc(bool (*function)(void));
	virtual ~HSConditionFunc();

	bool check();
};

#endif /* HSCONDITIONFUNC_H_ */
