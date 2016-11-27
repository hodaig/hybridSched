/*
 * HybridSched.cpp0
 *
 *  Created on: Sep 14, 2016
 *      Author: hodai
 */

#include "HybridSched.h"
#include "HSMode.h"
#include "tests/Utils.h"
#include "conditions/HSConditionFalse.h"
#include "conditions/HSConditionTrue.h"
#include "conditions/HSConditionSmallEq.h"
#include "HSAutomata.h"


HybridSched::HybridSched()
			: _generalVarsCount(0),
			  _automata(0), _currentMode(0),
			  _timeGuardedAutomata(new HSAutomata()),
			  _testQ(0){// : _generalVarsCount(0), _initialMode(0), _currentMode(0){

    HSMode* mode = new HSMode();
    _timeGuardedAutomata->addInitialMode(mode);
    HSTransition* trans = new HSTransition(mode, HSConditionTrue::getSingleton(), mode, 0);
    _timeGuardedAutomata->addTransition(trans);
}

HybridSched::~HybridSched() {
	// TODO Auto-generated destructor stub
}

void HybridSched::tics() {
    if (_testQ){
       _testQ->push_back(HS_TICS_SEPARATOR);
    }

#ifdef HS_CONF_AUTO_ADVANCE_VARIABLES
    // advance all clocks
    unsigned int i;
    for (i=0 ; i<_generalVarsCount ; i++) {
        _generalVars[i]++;
    }
#endif

	// TODO - complete
}

int HybridSched::run(uint32_t time_available) {
	int res;
	HSTransition* bestTrans = 0;
	uint32_t run_started_usec;
	DEBUG_TRACE_START();

	run_started_usec = MICROS(); //micros64() & 0xFFFFFFFF

	if(!_currentMode){
	    ASSERT("scheduler not initialized");
	}

	bestTrans = _currentMode->getBestTransition();

	if (bestTrans){
	    if (0 == bestTrans->getTo()){
	        ASSERT("empty transition");
	    }

	//} else if (!_currentMode->getDomain()->check()) {
	} else {
	    ASSERT("can't continue")
	}

	// run the actual tasks
	res = exacuteTransitionTasks(bestTrans);
	_currentMode = bestTrans->takeTransition();

	//TODO - check time slot
	if (MICROS() - run_started_usec < 0){
		DEBBUG_PRINTF_info("time slot overflow");
	}

	DEBUG_TRACE_END();
	return res;
}

int HybridSched::addTaskSpec(HSMode* initialMode){
    //DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_TRACE, "start");
    int ans;
    HSAutomata* newAuto;

    DEBUG_TRACE_START();

    newAuto = new HSAutomata();
    newAuto->addInitialMode(initialMode);

    newAuto->cutDeadEnds();
    if (newAuto->getInitialsModes()->empty()){
        DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_WARNING, "new spec with no initial mode");
    }
    ans = addTaskSpec(newAuto);

    DEBUG_TRACE_RETURN(ans);
}
int HybridSched::addTaskSpec(HSAutomata* specAuto){
    HSAutomata* oldAuto = 0;

    DEBUG_TRACE_START();

    if (specAuto->getInitialsModes()->empty()){
		ASSERT("empty spec auto mode");
	}

    addTasksToGuardedAutomata(specAuto);

    if (0 == _automata){
	    _automata = specAuto;
	} else {
	    oldAuto = _automata;
	    _automata = _automata->product(specAuto, HS_SLOT_SIZE_MICROS);

	    delete specAuto; specAuto=0;
	    delete oldAuto; oldAuto=0;
	}

    removeIlegalTramsitions();

    DEBUG_TRACE_RETURN(HS_RETVAL_OK);
}

int HybridSched::addPeriodTask(Task* task, uint16_t period_slots_min, uint16_t period_slots_max){
	// TODO - potential memory leak here
	//hsVariable_t* onVar;  // zero in "on" mode

    //HSMode** initModesArr;
    HSAutomata* newAuto;
    HSMode** allModesArr;
    //char* taskNameOn, taskNameOff;

    unsigned int i;
    int ans;

	DEBUG_TRACE_START();

    if(!task){
        ASSERT("(!task");
    }
    if(_generalVarsCount >= HS_MAX_GENERAL_VARS){
        ASSERT("(_generalVarsCount >= HS_MAX_GENERAL_VARS)");
    }

    allModesArr = new HSMode*[period_slots_max + 1];         // local (free here) (malloc)
    //initModesArr = new HSMode*[period_slots_min + 2];
    newAuto = new HSAutomata();
    //initModesArr[period_slots_max] = 0;

    allModesArr[0] = new HSMode(task->name);
    //allModesArr[0]->addTask(task);
    newAuto->addInitialMode(allModesArr[0]);

    for (i = 1; i < period_slots_min; i++) {
        allModesArr[i] = new HSMode("idle");
        newAuto->addInitialMode(allModesArr[i]);
        HSTransition* t = new HSTransition(allModesArr[i-1], HSConditionTrue::getSingleton(), allModesArr[i], 0);
        t->addNoTask(task);
        newAuto->addTransition(t);
    }
    HSTransition* t = new HSTransition(allModesArr[i-1], HSConditionTrue::getSingleton(), allModesArr[0], 0);
    t->addTask(task);
    newAuto->addTransition(t);

    for (i = period_slots_min; i < period_slots_max; i++) {
        allModesArr[i] = new HSMode("idle");
        newAuto->addMode(allModesArr[i]);

        //allModesArr[i-1]->addTransition(allModesArr[i], HSConditionTrue::getSingleton(),(i-period_slots_min));
        HSTransition* tNo = new HSTransition(allModesArr[i-1], HSConditionTrue::getSingleton(), allModesArr[i], (i-period_slots_min));
        tNo->addNoTask(task);
        newAuto->addTransition(tNo);

        HSTransition* tYes = new HSTransition(allModesArr[i], HSConditionTrue::getSingleton(), allModesArr[0], 0);
        tYes->addTask(task);
        newAuto->addTransition(tYes);
        //allModesArr[i]->addTransition(t);
    }

    if (period_slots_max > period_slots_min){
        newAuto->addInitialMode(allModesArr[period_slots_min]);
    }


    free(allModesArr); allModesArr=0;

    ans = addTaskSpec(newAuto);
	DEBUG_TRACE_RETURN(ans);

#if 0 // TODO
    HSMode* taskOnMode;
    HSMode* taskOffMode;
	hsVariable_t* counterVar;
	Task* advanceByOne = new Task();//(Task*)malloc(sizeof (Task));
	Task* backToZero   = new Task();

	HSMode* taskOnMode;
	HSMode* taskOffMode;

	HSMode** modesArr;


	if(!task){
		ASSERT("(!task");
	}
	if(_generalVarsCount >= HS_MAX_GENERAL_VARS){
		ASSERT("(_generalVarsCount >= HS_MAX_GENERAL_VARS)");
	}

	counterVar = &(_generalVars[_generalVarsCount++]);
	//onVar = &(_generalVars[_generalVarsCount]);
	*counterVar = 0;
	//*onVar = 0;

	advanceByOne->type = HS_TASK_TYPE_INC;
	advanceByOne->name = "auto advance";
	advanceByOne->max_time_micros = 0;
	advanceByOne->value = counterVar;

	backToZero->type = HS_TASK_TYPE_NULLIFY;
	backToZero->name = "auto nullify";
	backToZero->max_time_micros = 0;
	backToZero->value = counterVar;

	taskOnMode = new HSMode(task->name);
	taskOffMode = new HSMode(task->name);

	taskOnMode->setDomain(new HSConditionSmallEq(1, counterVar));
	taskOffMode->setDomain(new HSConditionSmallEq(counterVar, period_slots_max-1));

	taskOnMode->addTask(task);
	taskOnMode->addTask(backToZero);
	taskOffMode->addTask(advanceByOne);

	// TODO taskOnMode->setDomain()
	taskOnMode->addTransition(taskOffMode, HSConditionTrue::getSingleton(), 0);

	set<hsVariable_t*> tempReset;
	tempReset.clear();
	tempReset.insert(counterVar);
	taskOffMode->addTransition(taskOnMode, new HSConditionSmallEq(period_slots_min-1, counterVar), &tempReset, 0);

	modesArr = new HSMode*[3];
	modesArr[0] = taskOnMode;
	modesArr[1] = taskOffMode;
	modesArr[2] = 0;

	int ans = addTaskSpec(modesArr);
	DEBUG_TRACE_RETURN(ans);
#endif
}

int HybridSched::reset(uint32_t slot_time_micros){
    DEBUG_TRACE_START();

    if(0 == _automata || _automata->getInitialsModes()->empty()){
        ASSERT("no initial mode");
    }

#if 0 // TODO
    // remove overflow modes
    _initialMode = HSAutomata::OverflowModeFilter(_initialMode,
                Utils::getNullTerminatedArraySize((void **)_initialMode), slot_time_micros);
    if(0 == _initialMode || 0 == _initialMode[0]){
        ASSERT("all initial modes are overflow modes");
        return HS_RETVAL_ERR;
    }
#endif

    // Everything OK, set the initial state
    _currentMode = (*_automata->getInitialsModes()->begin());

    DEBUG_TRACE_END();
	return HS_RETVAL_OK;
}
#if 0
int HybridSched::exacuteModeTasks(HSMode* mode){
	const set<Task*>* tasks;
	int res = HS_RETVAL_OK;

	DEBUG_TRACE_START();

	if (!mode){
		ASSERT("null mode");
		return -1;
	}

	// execute all tasks
	// TODO - use also HS_MAX_TASKS_IN_MODE
	tasks = mode->getTasks();
	for (set<Task*>::iterator it = tasks->begin(); it != tasks->end(); ++it) {

		switch ((*it)->type) {
			case HS_TASK_TYPE_PRINT:
				printf("running dummy task: %s\n", (*it)->name);
				break;
			case HS_TASK_TYPE_REGULAR:
				(*it)->function();

				break;
			case HS_TASK_TYPE_INC:
				if (!(*it)->value){
					ASSERT("null value");
				}

				(*(*it)->value)++;

				break;
			case HS_TASK_TYPE_TEST:
				if (_testQ){
					_testQ->push_back((*it)->name);
				}
				break;
			default:
				break;
		}
	}

	DEBUG_TRACE_RETURN(res);
}
#endif
int HybridSched::exacuteTransitionTasks(HSTransition* trans){
    const set<Task*>* tasks;
    int res = HS_RETVAL_OK;

    DEBUG_TRACE_START();

    if (!trans){
        ASSERT("null trans");
        return -1;
    }

    // execute all tasks
    // TODO - use also HS_MAX_TASKS_IN_MODE
    tasks = trans->getTasks();
    for (set<Task*>::iterator it = tasks->begin(); it != tasks->end(); ++it) {

        switch ((*it)->type) {
        case HS_TASK_TYPE_PRINT:
            printf("running dummy task: %s\n", (*it)->name);
            break;
        case HS_TASK_TYPE_REGULAR:
            (*it)->function();

            break;
        case HS_TASK_TYPE_INC:
            if (!(*it)->value){
                ASSERT("null value");
            }

            (*(*it)->value)++;

            break;
        case HS_TASK_TYPE_TEST:
            if (_testQ){
                _testQ->push_back((*it)->name);
            }
            break;
        default:
            break;
        }
    }

    DEBUG_TRACE_RETURN(res);
}

void HybridSched::setTestQ(std::deque<const char*>* testQ){
	_testQ = testQ;
}

void HybridSched::addTasksToGuardedAutomata(HSAutomata* automata){
    const set<Task*>* tasks = automata->getAllTasks();
    HSMode* theMode = *(_timeGuardedAutomata->getInitialsModes()->begin());

    if(!theMode){
        ASSERT("null mode");
    }

    const set<HSTransition*>* trans = theMode->getTransitions();

    for (set<HSTransition*>::iterator transIt = trans->begin(); transIt != trans->end(); ++transIt) {
        for (set<Task*>::iterator taskIt = tasks->begin(); taskIt != tasks->end(); ++taskIt) {
            if ((*transIt)->containsTask(*taskIt)){
                continue;
            }

            if ( ((*transIt)->getMaxTimeMicros() + (*taskIt)->max_time_micros) < HS_SLOT_SIZE_MICROS){
                HSTransition* newTrans = new HSTransition(*transIt);
                newTrans->addTask(*taskIt);
                _timeGuardedAutomata->addTransition(newTrans);
            }

            (*transIt)->addNoTask(*taskIt);
        }
    }

}

void HybridSched::removeIlegalTramsitions(){
    HSAutomata* oldAuto = 0;

    // remove illegal transitions
    oldAuto = _automata;
    _automata = _automata->product(_timeGuardedAutomata, HS_SLOT_SIZE_MICROS);
    delete oldAuto; oldAuto=0;

    // remove unreachable/blocking states
    oldAuto = _automata;
    _automata = _automata->simplify();
    delete oldAuto; oldAuto=0;

}

