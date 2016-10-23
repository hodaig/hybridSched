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
		_tasks(),
		_transitions(),
		_modeMaxTimeMicros(0),
		_maxTimeValid(false){
}

HSMode::HSMode(const char* name) :
		_name(name),
		_dom(HSConditionTrue::getSingleton()),
		_tasks(),
		_transitions(),
		_modeMaxTimeMicros(0),
		_maxTimeValid(false) {
}

HSMode::~HSMode() {}


void HSMode::addTask(HybridSched::Task* task){
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_LOG, "add task %s to mode %s", task->name, getName());

	if (!task){
	    ASSERT("null task added");
		return;
	}

	if (0 != _tasks.count(task)){
	    ASSERT("adding twice the same task to a mode");
	}

	_tasks.insert(task);
	_maxTimeValid = false;
}

void HSMode::addTransition(HSMode* toMode, HSCondition* cond, uint32_t cost){
    addTransition(toMode, cond, 0, cost);
}

void HSMode::addTransition(HSMode* toMode, HSCondition* cond, const set<hsVariable_t*>* reset, uint32_t cost){

    if (!toMode){
		ASSERT("!toMode");
		return;
	}

	if (0 == reset){
	_transitions.insert(new HSTransition(cond, toMode, cost));
	} else {
	_transitions.insert(new HSTransition(cond, toMode, reset, cost));
	}
}

void HSMode::setDomain(HSCondition* dom){
	_dom = dom;
}

HSCondition* HSMode::getDomain(){
	return _dom;
}

const set<HSTransition*>* HSMode::getTransitions(){
	// TODO - debug
	if (_transitions.empty()){
	    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_LOG, "_transitions.empty()");
	}

	// TODO - it's makes privacy issue
	return &_transitions;
}

unsigned int HSMode::getAvailableTransitions(set<HSTransition*>* retTransitionsSet){
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_WARNING, "using problematic function");

    if (!retTransitionsSet->empty()) {
        DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_WARNING, "filling a non empty set of transitions");
    }

    for (set<HSTransition*>::iterator it = _transitions.begin(); it != _transitions.end(); ++it) {
        if (0 == *it) {
            ASSERT("unexpected null transition");
        }

        if ((*it)->check()) {
            retTransitionsSet->insert(*it);
        }
    }

    return retTransitionsSet->size();
}

HSTransition* HSMode::getBestTransition(){
    uint32_t mostHeavy = 0;
    //HSTransition* best = 0;

    if (_dom->check()) {
        // only for hybrid automata
        // mostHeavy = getMaxTimeMicros();
    }

    for (set<HSTransition*>::iterator it = _transitions.begin(); it != _transitions.end(); ++it) {
        if (0 == *it) {
            ASSERT("unexpected null transition");
        }

        if ((*it)->check()) {
            if (mostHeavy > (*it)->getNext()->getMaxTimeMicros()){
                continue;
            }
            return (*it);
        }
    }

    return 0;
}

void HSMode::removeTransition(HSTransition* transition){
    if (0 == _transitions.count(transition)){
        DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_WARNING, "transition not found");
    } else {
        _transitions.erase(transition);
    }
}

void HSMode::removeTransitions(HSMode* toMode){
    set<HSTransition*> tempSet;

    for (set<HSTransition*>::iterator it = _transitions.begin(); it != _transitions.end(); ++it) {
        if ((*it)->getNext() == toMode) {
            tempSet.insert(*it);
        }
    }

    for (set<HSTransition*>::iterator it = tempSet.begin(); it != tempSet.end(); ++it) {
        removeTransition(*it);
    }
}

set<HSMode*>* HSMode::getAllNexts(){
    set<HSMode*>* theSet = new set<HSMode*>();

    for (set<HSTransition*>::iterator it = _transitions.begin(); it != _transitions.end(); ++it) {
        if (0 == *it){
            ASSERT("unexpected empty mode transition");
        } else if ((*it)->getNext() && 0 == theSet->count((*it)->getNext())){
            theSet->insert((*it)->getNext());
        }
    }

    return theSet;
}

const set<HybridSched::Task*>* HSMode::getTasks(){
    // TODO - it's makes privacy issue
    return &_tasks;
}

uint16_t HSMode::getMaxTimeMicros(){
    if (!_maxTimeValid){
        _modeMaxTimeMicros = 0;
        for (set<HybridSched::Task*>::iterator it = _tasks.begin(); it != _tasks.end(); ++it) {
            _modeMaxTimeMicros += (*it)->max_time_micros;
        }
        _maxTimeValid = true;
    }

    return _modeMaxTimeMicros;
}
#if o
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
#endif
const char* HSMode::getName(){
    return _name;
}
#if 0
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
#endif
