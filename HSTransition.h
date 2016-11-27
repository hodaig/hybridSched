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
#include <string>

/* forward deceleration */
class HSMode;

class HSTransition {
private:
    HSCondition* _cond;
    std::set<HybridSched::Task*> _yesTasks;
    std::set<HybridSched::Task*> _noTasks;
    HSMode* _fromMode;
    HSMode* _toMode;
    std::set<hsVariable_t*> _reset;
    uint32_t _cost;

    uint16_t _modeMaxTimeMicros;
    bool     _maxTimeValid;                 // true if the value of '_modeMaxTimeMicros' is valid


public:
    HSTransition(HSMode* fromMode, HSCondition* cond, HSMode* toMode, const std::set<hsVariable_t*>* reset, uint32_t cost);
    HSTransition(HSMode* fromMode, HSCondition* cond, HSMode* toMode, uint32_t cost);
    HSTransition(HSTransition first, HSTransition second, HSMode* toMode);
    HSTransition(HSTransition* other);
    virtual ~HSTransition();

    HSTransition* addTask(HybridSched::Task* task);
    HSTransition* addTasks(const char* lable);
    HSTransition* addNoTask(HybridSched::Task* task);
    HSTransition* addNoTasks(const char* lable);
    const std::set<HybridSched::Task*>* getTasks();

    /*
     * search in '_yesTasks' and 'noTasks
     */
    bool containsTask(HybridSched::Task* task);

    HSCondition* getCond();

    /*
     * return copy, deleted by the user
     */
    std::string* getLabel();
    /*
     * return copy, deleted by the user
     * must be deep free
     */
    std::set<std::string*>* getPropositions();

    const std::set<hsVariable_t*>* getReset();
    uint32_t getCost();
    uint16_t getMaxTimeMicros();

    // TODO refactor this function to the constructor
    void setFromMode(HSMode* fromMode);
    HSMode* getFrom();
    HSMode* getTo();

    bool check();
    HSMode* takeTransition();

private:

    /*
     * return new string, must be deleted by user
     */
    std::string* taskToGoalProposition(HybridSched::Task* task);

    /*
     * return pointer to the task, not constructing the task, assume the task exist.
     */
    HybridSched::Task* taskToGoalProposition(const char* task);


};

#endif /* HSTRANSITION_H_ */
