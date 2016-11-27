/*
 * HSTransition.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: hodai
 */

#include "HSTransition.h"
#include "conditions/HSConditionAND.h"
#include "HSMode.h"
#include "tests/Utils.h"

#include <string.h>
#include <inttypes.h>  // PRIuPTR (printf of pointer)


HSTransition::HSTransition(HSMode* fromMode, HSCondition* cond, HSMode* toMode, const set<hsVariable_t*> *reset, uint32_t cost) :
            _cond(cond), _yesTasks(), _fromMode(fromMode),_toMode(toMode), _reset(*reset), _cost(cost),
            _modeMaxTimeMicros(0),
            _maxTimeValid(false){
    _yesTasks.clear();
}

HSTransition::HSTransition(HSMode* fromMode, HSCondition* cond, HSMode* toMode, uint32_t cost) :
            _cond(cond), _yesTasks(), _fromMode(fromMode), _toMode(toMode), _reset(), _cost(cost),
            _modeMaxTimeMicros(0),
            _maxTimeValid(false){
    _reset.clear();
    _yesTasks.clear();
}
#if 0
HSTransition::HSTransition(HSTransition first, HSTransition second, HSMode* toMode):
            _cond(new HSConditionAND(first._cond, second._cond)),
            _yesTasks(),
            _fromMode(0),
            _toMode(toMode),
            _reset(first._reset),
            _cost(first._cost + second._cost),
            _modeMaxTimeMicros(0),
            _maxTimeValid(false){
    _reset.insert(second._reset.begin(), second._reset.end());
}
#endif

HSTransition::HSTransition(HSTransition* other) :
        HSTransition(other->_fromMode, other->_cond, other->_toMode, &(other->_reset), other->_cost){

    _yesTasks.insert(other->_yesTasks.begin(), other->_yesTasks.end());
    _noTasks.insert(other->_noTasks.begin(), other->_noTasks.end());
    _modeMaxTimeMicros = other->_modeMaxTimeMicros;
    _maxTimeValid = other->_maxTimeValid;

}

HSTransition::~HSTransition() {
    // TODO Auto-generated destructor stub
}

HSTransition* HSTransition::addTask(HybridSched::Task* task){
    if (!task){
        ASSERT("null task added");
    }

    if (0 != _yesTasks.count(task)){
        ASSERT("adding twice the same task to a mode");
    }

    _yesTasks.insert(task);
    _maxTimeValid = false;

    return this;
}

HSTransition* HSTransition::addTasks(const char* labelStr){
    HybridSched::Task* tmpTask;
    int lableSize = strlen(labelStr) + 1;
    char* lableToc = (char*) malloc(lableSize);
    lableToc = strcpy(lableToc, labelStr);
    char *p = strtok(lableToc, " ");
    while (p) {
        tmpTask = taskToGoalProposition(p);
        if (0 != tmpTask){
            addTask(tmpTask);
        }
        p = strtok(NULL, " ");
    }

    return this;
}

HSTransition* HSTransition::addNoTask(HybridSched::Task* task){
    if (!task){
        ASSERT("null task added");
    }

    if (0 != _noTasks.count(task)){
        ASSERT("adding twice the same task to a mode");
    }

    _noTasks.insert(task);

    return this;
}

HSTransition* HSTransition::addNoTasks(const char* labelStr){
    HybridSched::Task* tmpTask;
    int lableSize = strlen(labelStr) + 1;
    char* lableToc = (char*) malloc(lableSize);
    lableToc = strcpy(lableToc, labelStr);
    char *p = strtok(lableToc, " ");
    while (p) {
        if (p[0] == '~'){
            tmpTask = taskToGoalProposition(p + 1);
            if (0 != tmpTask){
                addNoTask(tmpTask);
            }
        }
        p = strtok(NULL, " ");
    }

    return this;
}

const std::set<HybridSched::Task*>* HSTransition::getTasks(){
    // TODO - it's makes privacy issue
    return &_yesTasks;
}

bool HSTransition::containsTask(HybridSched::Task* task){
    for (set<HybridSched::Task*>::iterator it = _yesTasks.begin(); it != _yesTasks.end(); it++){
        if ((*it) == task){
            return true;
        }
    }
    for (set<HybridSched::Task*>::iterator it = _noTasks.begin(); it != _noTasks.end(); it++){
        if ((*it) == task){
            return true;
        }
    }

    return false;
}

HSCondition* HSTransition::getCond(){
    return _cond;
}

std::string* HSTransition::getLabel(){
    std::string* str = new std::string();
    std::string* tmpStr;

    // TODO add also the conditions

    // the Yes tasks
    for (set<HybridSched::Task*>::iterator it = _yesTasks.begin(); it != _yesTasks.end(); it++){
        if (!(*it)){
            ASSERT("null task");
        }

        tmpStr = taskToGoalProposition(*it);
        str->append(*tmpStr);
        str->append(" ");
        delete tmpStr; tmpStr = 0;
    }

    // the No tasks
    for (set<HybridSched::Task*>::iterator it = _noTasks.begin(); it != _noTasks.end(); it++){
        if (!(*it)){
            ASSERT("null task");
        }

        tmpStr = taskToGoalProposition(*it);
        str->append("~");
        str->append(*tmpStr);
        str->append(" ");
        delete tmpStr; tmpStr = 0;
    }
    return str;
}

std::set<std::string*>* HSTransition::getPropositions(){
    std::string* str;
    std::set<std::string*>* propo = new std::set<std::string*>();

    // TODO add also the conditions

    // the Yes tasks
    for (set<HybridSched::Task*>::iterator it = _yesTasks.begin(); it != _yesTasks.end(); it++){
        if (!(*it)){
            ASSERT("null task");
        }

        str = taskToGoalProposition(*it);
        propo->insert(str);
    }

    // the No tasks
    for (set<HybridSched::Task*>::iterator it = _noTasks.begin(); it != _noTasks.end(); it++){
        if (!(*it)){
            ASSERT("null task");
        }

        str = taskToGoalProposition(*it);
        propo->insert(str);
    }
    return propo;
}

const std::set<hsVariable_t*>* HSTransition::getReset(){
    return &_reset;
}

uint32_t HSTransition::getCost(){
    return _cost;
}

uint16_t HSTransition::getMaxTimeMicros(){
    if (!_maxTimeValid){
        _modeMaxTimeMicros = 0;
        for (set<HybridSched::Task*>::iterator it = _yesTasks.begin(); it != _yesTasks.end(); ++it) {
            _modeMaxTimeMicros += (*it)->max_time_micros;
        }
        _maxTimeValid = true;
    }

    return _modeMaxTimeMicros;
}

void HSTransition::setFromMode(HSMode* fromMode) {
    _fromMode = fromMode;
}

HSMode* HSTransition::getFrom(){
    return _fromMode;
}

HSMode* HSTransition::getTo(){
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

std::string* HSTransition::taskToGoalProposition(HybridSched::Task* task){
    string* str = new std::string();
    char* taskstr = (char*) malloc(20);

    //sprintf(taskstr,"%d", (task));
    sprintf(taskstr,"%" PRIuPTR , (uintptr_t)task);

    str->append("task");
    str->append(taskstr);

    free(taskstr); taskstr = 0;

    return str;
}

HybridSched::Task* HSTransition::taskToGoalProposition(const char* task){
    if (0 == strncmp("task", task, 4)){
        unsigned long taskAddr = atol(task + 4);
        return (HybridSched::Task*)taskAddr;
    }
    return 0;
}

