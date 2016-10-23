/*
 * HSConditionFalse.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#include "HSConditionFalse.h"

/* static member initializer */
HSConditionFalse* HSConditionFalse::_singleton = 0;


HSConditionFalse::HSConditionFalse() {
	// TODO Auto-generated constructor stub

}

HSConditionFalse::~HSConditionFalse() {
	// TODO Auto-generated destructor stub
}

HSConditionFalse* HSConditionFalse::getSingleton(){
	if (!_singleton){
		_singleton = new HSConditionFalse();
	}
	return _singleton;
}

bool HSConditionFalse::check(){
	return false;
}


