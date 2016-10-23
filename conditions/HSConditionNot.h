/*
 * HSConditionNot.h
 *
 *  Created on: Sep 19, 2016
 *      Author: hodai
 */

#ifndef HSCONDITIONNOT_H_
#define HSCONDITIONNOT_H_

#include "HSCondition.h"

class HSCondition_Not: public HSCondition {
private:
	HSCondition* _inner;

public:
	HSCondition_Not(HSCondition* inner);
	virtual ~HSCondition_Not();

	bool check();
};

#endif /* HSCONDITIONNOT_H_ */
