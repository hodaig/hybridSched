/*
 * HSConditionFalse.h
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#ifndef HSCONDITIONFALSE_H_
#define HSCONDITIONFALSE_H_

#include "HSCondition.h"

class HSConditionFalse: public HSCondition {
	static HSConditionFalse* _singleton;

private:
	HSConditionFalse();
	virtual ~HSConditionFalse();

public:
	static HSConditionFalse* getSingleton();
	bool check();

};

#endif /* HSCONDITIONFALSE_H_ */
