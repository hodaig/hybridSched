/*
 * HSCondition1.h
 *
 *  Created on: Sep 19, 2016
 *      Author: hodai
 */

#ifndef HSCONDITION_H_
#define HSCONDITION_H_

class HSCondition {
public:
	virtual ~HSCondition();

	virtual bool check() = 0;
};

#endif /* HSCONDITION_H_ */
