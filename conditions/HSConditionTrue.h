/*
 * HSConditionTrue.h
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#ifndef HSCONDITIONTRUE_H_
#define HSCONDITIONTRUE_H_

#include "HSCondition.h"

class HSConditionTrue: public HSCondition {
private:
	static HSConditionTrue* _singleton;

private:
	HSConditionTrue();
	virtual ~HSConditionTrue();

public:
	static HSConditionTrue* getSingleton();
	bool check();
};

#endif /* HSCONDITIONTRUE_H_ */
