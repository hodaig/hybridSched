/*
 * HSConditionSmallEq.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#include "HSConditionSmallEq.h"

HSConditionSmallEq::HSConditionSmallEq(int* left, int* right)
			: _left(left), _right(right), _constant(0){
}
HSConditionSmallEq::HSConditionSmallEq(int left, int* right)
			: _left(&_constant), _right(right), _constant(left) {
}
HSConditionSmallEq::HSConditionSmallEq(int* left, int right)
			:_left(left), _right(&_constant), _constant(right){
}

bool HSConditionSmallEq::check(){
	return ((*_left) <= (*_right));
}

