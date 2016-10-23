/*
 * Main.cpp
 *
 *  Created on: Sep 14, 2016
 *      Author: hodai
 */

#include <unistd.h>
#include "Utils.h"

#include <utility>  //pair
#include <map>

#include "../HSMode.h"
#include "../HybridSched.h"
#include "../conditions/HSCondition.h"
#include "../conditions/HSConditionTrue.h"
#include "../conditions/HSConditionFunc.h"
#include "TestAutoUtil.h"

#define HS_MAKE_TASK(name, type, fun, time) \
	HybridSched::Task name = {type, fun, #name, time};

#define HS_MAKE_TASK_DUMMY(name, deadline) \
	HybridSched::Task name = {HS_TASK_TYPE_PRINT, 0, #name, deadline};
#define HS_MAKE_TASK_REGULAR(name, fun, deadline) \
	HybridSched::Task name = {HS_TASK_TYPE_REGULAR, fun, #name, deadline};
#define HS_MAKE_TASK_INC_var(name, var) \
	HybridSched::Task name = {HS_TASK_TYPE_INC, 0, "increment " #var, 0, var};


/* globals */
int x = 0;

/* the tasks */
HS_MAKE_TASK_DUMMY(print1, 100);
HS_MAKE_TASK_DUMMY(print2, 100);

int incX(){x++; return 0;};
bool testX(){ return (x>3);};

int returnFalse(){return -1;}
HS_MAKE_TASK_REGULAR(terminationTask, returnFalse, 0);
HS_MAKE_TASK_INC_var(incXTask, &x);

/* the automata modes */
HSMode mode1("mode1");
HSMode mode2("mode2");
HSMode mode3("terminationMode");

/* conditions */

int cond_true() {return 0;};

//HSCondition_true cond_true;

HybridSched* createSched1(){
	mode1.addTask(&print1);
	mode1.addTask(&incXTask);

	mode2.addTask(&print1);
	mode2.addTask(&print2);

	mode3.addTask(&terminationTask);

	mode1.addTransition(&mode2, HSConditionTrue::getSingleton(), 0);
	mode2.addTransition(&mode1, HSConditionTrue::getSingleton(), 0);
	mode2.addTransition(&mode3, new HSConditionFunc(&testX), 0);

	HybridSched*  sched = new HybridSched();
	sched->addTaskSpec(&mode1);

	return sched;
}

int startSched(HybridSched* sched){
	int res = 0;

	sched->reset(100);

	while (0 == res){
		sched->tics();
		res = sched->run(100);
		usleep(300*1000L);
	}
	DEBBUG_PRINTF_info("fail with: %d", res);
	return res;
}

int test1(){
	DEBBUG_PRINTF_info("start");

	HybridSched* sched = createSched1();
	startSched(sched);

	DEBBUG_PRINTF_info("end");
	return 1;
}

int main(int argc, char **argv) {


	TestAutoUtil::test();
	printf("done./n");
	return 0;

	HSMode* mode1 = new HSMode();
	HSMode* mode2 = new HSMode();

	std::map<std::pair<HSMode*,HSMode*>, HSMode*> m;
	std::pair<HSMode*,HSMode*>* initial = new std::pair<HSMode*,HSMode*>(mode1, mode2);
	m[*initial] = mode2;

	std::pair<HSMode*,HSMode*>* pair2 = new std::pair<HSMode*,HSMode*>(mode1, mode2);
	std::pair<HSMode*,HSMode*>* pair3 = new std::pair<HSMode*,HSMode*>(mode1, mode1);
	HSMode* theMode = m[*pair2];

	if (mode1 == theMode){
		printf("1\n");
	} else if (mode2 == theMode){
		printf("2\n");
	} else {
		printf("none\n");
	}

	printf("m.count=%d\n", m.count(*pair2));
	printf("m.count=%d\n", m.count(*pair3));


	test1();

	return 0;
}


