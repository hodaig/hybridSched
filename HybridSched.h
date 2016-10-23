/*
 * HybridSched.h
 *
 *  Created on: Sep 14, 2016
 *      Author: hodai
 */

#ifndef HYBRIDSCHED_H_
#define HYBRIDSCHED_H_

#include "HSAutomata.h"

#include <stdint.h> 		// types
#include <queue>

#define HS_RETVAL_OK 1
#define HS_RETVAL_ERR -1

#define HS_DOM_ALWAYS_TRUE  // meaning we not using the hybrid systems methodology (the domain of mode must always be true)
// #define HS_CONF_AUTO_ADVANCE_VARIABLES

#define HS_MAX_GENERAL_VARS  40
#define HS_MAX_INITIAL_MODES 40

//TODO #include "HSMode.h"
class HSMode;

typedef int (*HSTask_fn_t)(void);
enum HSTask_type_t {
	HS_TASK_TYPE_NON = 0,				// undefined
	HS_TASK_TYPE_REGULAR = 1,           // normal function
	HS_TASK_TYPE_PRINT = 2,				// print the task name as task operation
	HS_TASK_TYPE_INC = 3,				// increment some counter
	HS_TASK_TYPE_NULLIFY = 4,           // nullify (back to zero) some counter
	HS_TASK_TYPE_TEST = 5,              // push value to queue for testing
};

typedef int hsVariable_t;

class HybridSched {
private:
    hsVariable_t _generalVars[HS_MAX_GENERAL_VARS];
	int _generalVarsCount;

	HSAutomata* _automata;
	HSMode* _currentMode;
	std::deque<const char*>* _testQ;

public:

    struct Task {
    	HSTask_type_t type;
    	HSTask_fn_t function;
        const char *name;
        //float rate_hz;
        uint16_t max_time_micros;
        int* value;
    };

	HybridSched();
	virtual ~HybridSched();

	void tics();
	int run(uint32_t time_available);
	int reset(uint32_t slot_time_micros);

	int addTaskSpec(HSMode* initialMode);
	int addTaskSpec(HSMode** initialMode);
	int addPeriodTask(Task* task, uint16_t period_slots_min, uint16_t period_slots_max);

	// for testing
	void setTestQ(std::deque<const char*>* testQ);

private:
	int exacuteModeTasks(HSMode* mode);
};

#endif /* HYBRIDSCHED_H_ */
