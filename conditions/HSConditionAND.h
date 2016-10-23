/*
 * HSConditionAND.h
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#ifndef HSCONDITIONAND_H_
#define HSCONDITIONAND_H_

#include "HSCondition.h"

class HSConditionAND: public HSCondition {
private:
	HSCondition* _first;
	HSCondition* _second;

public:
	HSConditionAND(HSCondition* first, HSCondition* second);
	virtual ~HSConditionAND();

	bool check();
};

#endif /* HSCONDITIONAND_H_ */
