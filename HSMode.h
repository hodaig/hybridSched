/*
 * HSMode.h
 *
 *  Created on: Sep 14, 2016
 *      Author: hodai
 */

#ifndef HSMODE_H_
#define HSMODE_H_

#include "HybridSched.h"
#include "conditions/HSCondition.h"
#include "HSTransition.h"

#include <set>

//TODO typedef int (*HSCondition)(HybridSched*);
//typedef int (*HSCondition)(void);

#define HS_MAX_TASKS_IN_MODE 20
#define HS_MAX_TRANSITIONS_FROM_MODE 20

using namespace std;

/* Forward deceleration */
class HSMode;
//class HSTransition;

#if 0 //TODO
typedef struct {
	HSCondition* cond;
	HSMode* toMode;
	set<hsVariable_t*> reset;
} HSMode_transition;
#endif

class HSMode {
private:
	//const char* _name;												// mode name (for debugging)
	const char* _name;												// mode name (for debugging)
	HSCondition* _dom;												// mode domain
	//TODO HSTask* _tasks[HS_MAX_TASKS_IN_MODE];							// null terminated tasks array
	HybridSched::Task* _tasks[HS_MAX_TASKS_IN_MODE];							// null terminated tasks array
	//HSMode_transition _transitions[HS_MAX_TRANSITIONS_FROM_MODE];	// null terminated transitions array
	HSTransition* _transitions[HS_MAX_TRANSITIONS_FROM_MODE];	// null terminated transitions array

	int _tasksCount;
	unsigned int _transCount;

public:
	HSMode();
	HSMode(const char* name);
	virtual ~HSMode();

	void addTask(HybridSched::Task* task);
	HybridSched::Task** getTasks();

	void addTransition(HSMode* toMode, HSCondition* cond, uint32_t cost);
	void addTransition(HSMode* toMode, HSCondition* cond, const set<hsVariable_t*>* reset, uint32_t cost);
	HSTransition** getTransitions();
	unsigned int getAvailableTransitions(set<HSTransition*>* retTransitionsSet);
	HSTransition* getAvailableNext();
	void removeTransition(HSTransition* transition);
	/*
	 * remove all the transitions to the mode 'toMode'
	 */
	void removeTransitions(HSMode* toMode);


	/*
	 * return new set with all the followers modes
	 * -- the user should delete this set
	 */
	set<HSMode*>* getAllNexts();

	void setDomain(HSCondition* dom);
	HSCondition* getDomain();

	uint16_t getMaxTimeMicros();

	void removeOverflowTransitions(uint32_t max_slot_time_micros);

	const char* getName();
private:
	void removeTransition(unsigned int transitionIndex);
};

#endif /* HSMODE_H_ */
