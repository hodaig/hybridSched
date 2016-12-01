/*
 * HybridSched.h
 *
 *  Created on: Sep 14, 2016
 *      Author: hodai
 */

#ifndef HYBRIDSCHED_H_
#define HYBRIDSCHED_H_

#define HS_TICS_SEPARATOR "<tick>"
#define HS_SLOT_SIZE_MICROS 100
#define HS_SLOT_SIZE_RATIO  0.8

#include <stdint.h> 		// types
#include <queue>

#include <AP_Scheduler/AP_Scheduler.h>

#define HS_RETVAL_OK 1
#define HS_RETVAL_ERR -1

// #define HS_CONF_AUTO_ADVANCE_VARIABLES

#define HS_MAX_GENERAL_VARS  40
#define HS_MAX_INITIAL_MODES 40


class HSMode;
class HSAutomata;
class HSTransition;

#ifdef HS_ALONE
typedef int (*HSTask_fn_t)(void);
#else
typedef AP_Scheduler::task_fn_t HSTask_fn_t;
#endif

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

	HSAutomata* _timeGuardedAutomata;

	std::deque<const char*>* _testQ;

	uint32_t _slot_size_micros;

	AP_Scheduler* _base_ap_sched;

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

	/*
	 * init the scheduler based on the ArduPilot default scheduler
	 */
	HybridSched(AP_Scheduler* ap_sched);
	virtual ~HybridSched();

	void tics();
	int run(uint32_t time_available);
#if 0
	int reset(uint32_t slot_time_micros);
#else
	int reset();
	int reset(AP_Scheduler* ap_sched);
#endif
	int addTaskSpec(HSMode* initialMode);
	int addTaskSpec(HSAutomata* specAuto);
	int addPeriodTask(Task* task, uint16_t period_slots_min, uint16_t period_slots_max);
	//void addPeriodTasks(const Task *tasks, uint8_t num_tasks);

	// for testing
	void setTestQ(std::deque<const char*>* testQ);

private:
	int exacuteModeTasks(HSMode* mode);
	int exacuteTransitionTasks(HSTransition* trans);

	/*
	 * add task of the automata to the existing guarded automata.
	 * the guarded automata prevent from time overflow
	 */
	void addTasksToGuardedAutomata(HSAutomata* automata);

	/*
	 * use the guarded automata to remove the illegal transitions
	 */
	void removeIlegalTramsitions();
};

#endif /* HYBRIDSCHED_H_ */
