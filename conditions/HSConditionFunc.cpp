/*
 * HSConditionFunc.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#include "HSConditionFunc.h"

HSConditionFunc::HSConditionFunc(bool (*function)(void))
			:_function(function){
}

HSConditionFunc::~HSConditionFunc() {
	// TODO Auto-generated destructor stub
}

bool HSConditionFunc::check() {
	return (*_function)();
}
