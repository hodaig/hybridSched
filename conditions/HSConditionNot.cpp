/*
 * HSConditionNot.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: hodai
 */

#include "HSConditionNot.h"

HSCondition_Not::HSCondition_Not(HSCondition* inner): _inner(inner){
}

HSCondition_Not::~HSCondition_Not() {
	// TODO Auto-generated destructor stub
}

bool HSCondition_Not::check(){
	return !_inner->check();
}

