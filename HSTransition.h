/*
 * HSTransition.h
 *
 *  Created on: Oct 6, 2016
 *      Author: hodai
 */

#ifndef HSTRANSITION_H_
#define HSTRANSITION_H_

//#include "HSMode.h"
#include "HybridSched.h"
#include "conditions/HSCondition.h"

#include <set>

/* forward deceleration */
class HSMode;

class HSTransition {
private:
    HSCondition* _cond;
    std::set<HybridSched::Task*> _tasks;
    HSMode* _toMode;
    std::set<hsVariable_t*> _reset;
    uint32_t _cost;

public:
    HSTransition(HSCondition* cond, HSMode* toMode, const std::set<hsVariable_t*>* reset, uint32_t cost);
    HSTransition(HSCondition* cond, HSMode* toMode, uint32_t cost);
    HSTransition(HSTransition first, HSTransition second, HSMode* toMode);
    virtual ~HSTransition();

    void addTask(HybridSched::Task* task);
    const std::set<HybridSched::Task*> getTasks();

    HSCondition* getCond();
    const std::set<hsVariable_t*>* getReset();
    uint32_t getCost();

    HSMode* getNext();

    bool check();
    HSMode* takeTransition();


};

#endif /* HSTRANSITION_H_ */
