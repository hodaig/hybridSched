/*
 * HSConditionAND.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#include "HSConditionAND.h"

HSConditionAND::HSConditionAND(HSCondition* first, HSCondition* second)
			: _first(first), _second(second){
	// TODO Auto-generated constructor stub
}

HSConditionAND::~HSConditionAND() {
	// TODO Auto-generated destructor stub
}

bool HSConditionAND::check(){
	return _first->check() and _second->check();
}
