/*
 * HSConditionTrue.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#include "HSConditionTrue.h"

/* static member initializer */
HSConditionTrue* HSConditionTrue::_singleton = 0;


HSConditionTrue::HSConditionTrue() {
	// TODO Auto-generated constructor stub

}

HSConditionTrue::~HSConditionTrue() {
	// TODO Auto-generated destructor stub
}

HSConditionTrue* HSConditionTrue::getSingleton(){
	if (!_singleton){
		_singleton = new HSConditionTrue();
	}
	return _singleton;
}
bool HSConditionTrue::check(){
	return true;
}

