/*
 * HSTransition.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: hodai
 */

#include "HSTransition.h"
#include "conditions/HSConditionAND.h"
#include "HSMode.h"

HSTransition::HSTransition(HSCondition* cond, HSMode* toMode, const set<hsVariable_t*> *reset, uint32_t cost) :
            _cond(cond), _tasks(), _toMode(toMode), _reset(*reset), _cost(cost){
}

HSTransition::HSTransition(HSCondition* cond, HSMode* toMode, uint32_t cost) :
            _cond(cond), _tasks(), _toMode(toMode), _reset(), _cost(cost){
    _reset.clear();
}

HSTransition::HSTransition(HSTransition first, HSTransition second, HSMode* toMode):
            _cond(new HSConditionAND(first._cond, second._cond)),
            _toMode(toMode),
            _reset(first._reset),
            _cost(first._cost + second._cost){
    _reset.insert(second._reset.begin(), second._reset.end());
}

HSTransition::~HSTransition() {
    // TODO Auto-generated destructor stub
}

void HSTransition::addTask(HybridSched::Task* task){
    if (!task) {
        ASSERT("null task");
    }

    _tasks.insert(task);
}

const std::set<HybridSched::Task*> HSTransition::getTasks(){
    return _tasks;
}


HSCondition* HSTransition::getCond(){
    return _cond;
}

const std::set<hsVariable_t*>* HSTransition::getReset(){
    return &_reset;
}

uint32_t HSTransition::getCost(){
    return _cost;
}

HSMode* HSTransition::getNext(){
    return _toMode;
}

bool HSTransition::check(){
    return _cond->check();
}

HSMode* HSTransition::takeTransition(){
    std::set<hsVariable_t*>::iterator it;

    for (it=_reset.begin() ; it!=_reset.end() ; it++) {
        (**it) = 0;
    }

    return _toMode;
}
