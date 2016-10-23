/*
 * HSMode.cpp
 *
 *  Created on: Sep 14, 2016
 *      Author: hodai
 */

#include "HSMode.h"
#include "tests/Utils.h"
#include "conditions/HSConditionTrue.h"

HSMode::HSMode() :
		_name("noName"),
		_dom(HSConditionTrue::getSingleton()),
		_tasksCount(0),
		_transCount(0) {
    _transitions[_transCount] = 0;
    _tasks[_tasksCount] = 0; // null terminate this array
}

HSMode::HSMode(const char* name) :
		_name(name),
		_dom(HSConditionTrue::getSingleton()),
		_tasksCount(0),
		_transCount(0){
    _transitions[_transCount] = 0;
    _tasks[_tasksCount] = 0; // null terminate this array
}

HSMode::~HSMode() {}


void HSMode::addTask(HybridSched::Task* task){
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_LOG, "add task %s to mode %s", task->name, getName());

	if (_tasksCount >= HS_MAX_TASKS_IN_MODE){
		// TODO - return error
	    ASSERT("task overflow in mode");
		return;
	}
	if (!task){
	    ASSERT("null task added");
		return;
	}

	_tasks[_tasksCount++] = task;
	_tasks[_tasksCount] = 0; // null terminate this array
}

void HSMode::addTransition(HSMode* toMode, HSCondition* cond, uint32_t cost){
    addTransition(toMode, cond, 0, cost);
}

void HSMode::addTransition(HSMode* toMode, HSCondition* cond, const set<hsVariable_t*>* reset, uint32_t cost){
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_LOG, "_tasksCount=%d", _transCount);
	if (_transCount >= HS_MAX_TRANSITIONS_FROM_MODE){
		ASSERT("_transCount >= HS_MAX_TRANSITIONS_FROM_MODE");
		return;
	}
	if (!toMode){
		ASSERT("!toMode");
		return;
	}

	if (0 == reset){
	_transitions[_transCount] = new HSTransition(cond, toMode, cost);
	} else {
	_transitions[_transCount] = new HSTransition(cond, toMode, reset, cost);
	}
	_transCount++;
	_transitions[_transCount] = 0;
}

void HSMode::setDomain(HSCondition* dom){
	_dom = dom;
}

HSCondition* HSMode::getDomain(){
	return _dom;
}

HSTransition** HSMode::getTransitions(){
	// TODO - debug
	if (0 == _transitions[0]){
	    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_LOG, "0 == _transitions[0]");
	}

	// TODO - it's open privacy issue
	return _transitions;
}

unsigned int HSMode::getAvailableTransitions(set<HSTransition*>* retTransitionsSet){
    unsigned int i;

    if (!retTransitionsSet->empty()) {
        DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_WARNING, "filling a non empty set of transitions");
    }

    for (i = 0; i < _transCount; ++i) {
        if (0 == _transitions[i]) {
            ASSERT("unexpected null transition");
        }

        if (_transitions[i]->check()) {
            retTransitionsSet->insert(_transitions[i]);
        }
    }

    return retTransitionsSet->size();
}

HSTransition* HSMode::getAvailableNext(){
    unsigned int i;
    uint32_t mostHeavy = 0;
    //HSTransition* best = 0;

    if (_dom->check()) {
        // only for hybrid automata
        // mostHeavy = getMaxTimeMicros();
    }
    for (i = 0; i < _transCount; ++i) {
        if (0 == _transitions[i]) {
            ASSERT("unexpected null transition");
        }

        if (_transitions[i]->check()) {
            if (mostHeavy > _transitions[i]->getNext()->getMaxTimeMicros()){
                continue;
            }
            return _transitions[i];
        }
    }

    return 0;
}

void HSMode::removeTransition(HSTransition* transition){
    unsigned int transIndex;
    for (transIndex = 0; transIndex < _transCount; ++transIndex) {
        if (_transitions[transIndex] == transition) {
            removeTransition(transIndex);
            return;
        }
    }
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_WARNING, "transition not found");
}

void HSMode::removeTransitions(HSMode* toMode){
    unsigned int transIndex;
    for (transIndex = 0; transIndex < _transCount; ++transIndex) {
        if (_transitions[transIndex]->getNext() == toMode) {
            removeTransition(transIndex);
            transIndex--;
            return;
        }
    }
}

set<HSMode*>* HSMode::getAllNexts(){
    set<HSMode*>* theSet = new set<HSMode*>();
    unsigned int i;

    for (i=0 ; i < _transCount ; i++){
        if (0 == _transitions[i]){
            ASSERT("unexpected empty mode transition");
        } else if (_transitions[i]->getNext() &&
                0 == theSet->count(_transitions[i]->getNext())){
            theSet->insert(_transitions[i]->getNext());
        }
    }

    return theSet;
}

HybridSched::Task** HSMode::getTasks(){
    // TODO - it's open privacy issue
    return _tasks;
}

uint16_t HSMode::getMaxTimeMicros(){
	HybridSched::Task** t = _tasks;
	uint16_t sum = 0;

	while (*t){
		sum += (*t)->max_time_micros;
		t++;
	}

	return sum;
}

void HSMode::removeOverflowTransitions(uint32_t max_slot_time_micros){
    unsigned int i;
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
    for (i=0 ; i < _transCount ; i++){
        DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
        if (0 == _transitions[i]){
            ASSERT("unexpected empty mode transition");
        } else {
            DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
            if(_transitions[i]->getNext()->getMaxTimeMicros() >= max_slot_time_micros){
                // this transition must be removed
                // TODO - memory leak potential here
                DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
                removeTransition(i);
                DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
                // this transition could be replaced successor transition that should also be checked
                i--;

            }
        }
    }
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," \n");
}

const char* HSMode::getName(){
    return _name;
}

void HSMode::removeTransition(unsigned int transitionIndex){
    unsigned int i;
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_TRACE, "start");
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_LOG, "remove transition from %s to %s", getName(), _transitions[transitionIndex]->getNext()->getName());

    if (transitionIndex >= _transCount || 0 > transitionIndex){
        ASSERT("Illegal transition index");
    }

    // iterate over all the transition in order to keep their order
    for (i = transitionIndex ; i < _transCount ; i++){
        if (i + 1 == _transCount){
            _transitions[i] = 0;
        } else {
            _transitions[i] = _transitions[i+1];
        }

    }

    _transCount--;
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_TRACE, "end");
}

