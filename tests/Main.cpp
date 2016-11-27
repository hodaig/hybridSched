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
#include "../pugixml-1.7/src/pugixml.hpp"
#include <iostream>  // std::cout
#include <string.h>  // strtok()

#include "../HSMode.h"
#include "../HybridSched.h"
#include "../conditions/HSCondition.h"
#include "../conditions/HSConditionTrue.h"
#include "../conditions/HSConditionFunc.h"
#include "../HSGoalAutomata.h"
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

    HSTransition* trans1 = new HSTransition(&mode1, HSConditionTrue::getSingleton(), &mode2, 0);
    trans1->addTask(&print1);
    trans1->addTask(&print2);
	mode1.addTransition(trans1);

    HSTransition* trans2 = new HSTransition(&mode2, HSConditionTrue::getSingleton(), &mode1, 0);
    trans2->addTask(&print1);
    trans2->addTask(&incXTask);
	mode2.addTransition(trans2);

    HSTransition* trans3 = new HSTransition(&mode2, new HSConditionFunc(&testX), &mode3, 0);
    trans3->addTask(&terminationTask);
	mode3.addTransition(trans3);

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

    HSGoalAutomata xml1;
    xml1.addInitialState(new HSMode("boo"));
    xml1.addInitialState(new HSMode("boo2"));

    HSGoalAutomata xml2;
    xml2.addInitialState(new HSMode("goo"));
    xml2.addInitialState(new HSMode("goo2"));

    xml1.product(&xml2);
    //HSxmlAutomata xml3("./out.gff");
    xml1.saveXmlToFile("test.gff");

	TestAutoUtil::test();
	printf("done./n");

	return 0;
#if 0
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
#endif
	return 0;
}


