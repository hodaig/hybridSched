/*
 * HSConditionSmallEq.h
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#ifndef HSCONDITIONSMALLEQ_H_
#define HSCONDITIONSMALLEQ_H_

#include "HSCondition.h"

class HSConditionSmallEq: public HSCondition {
private:
	int* _left;
	int* _right;
	int _constant;

public:
	HSConditionSmallEq(int* left, int* right);
	HSConditionSmallEq(int left, int* right);
	HSConditionSmallEq(int* left, int right);

	bool check();

};

#endif /* HSCONDITIONSMALLEQ_H_ */
