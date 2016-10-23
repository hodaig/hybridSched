/*
 * TestAutoUtil.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#include "TestAutoUtil.h"
#include "../HSMode.h"
#include "../conditions/HSConditionTrue.h"
#include "Utils.h"

#include <queue>
#include <stdio.h>				// for printf();

#define HS_SLOT_SIZE 100

static HSMode* auto1 = 0;
static HSMode* auto2 = 0;
static HSMode* auto1x2 = 0;

static HybridSched::Task t1;
static HybridSched::Task t2;
static HybridSched::Task t3;

static std::deque<const char*> outQ_before;
static std::deque<const char*> outQ_after;

void makeTasks(){
	t1.type = HS_TASK_TYPE_TEST;
	t1.name = "t1";
	t1.max_time_micros = 10;

	t2.type = HS_TASK_TYPE_TEST;
	t2.name = "t2";
	t2.max_time_micros = 30;

	t3.type = HS_TASK_TYPE_TEST;
	t3.name = "t3";
	t3.max_time_micros = 80;
}

void makeAutos1(){
	DEBBUG_PRINTF_info("");
	static bool firstTime = true;
	if(!firstTime){
		//return;
	}

	makeTasks();

	// auto1 :
	// ({A} #t)
	auto1 = new HSMode("mode1");
	auto1->addTask(&t1);
	auto1->setDomain(HSConditionTrue::getSingleton());
	auto1->addTransition(auto1, HSConditionTrue::getSingleton(), 0);
	DEBBUG_PRINTF_info("");
	// auto2 :
	// ({B} #T)
	auto2 = new HSMode("mode2");
	auto2->addTask(&t2);
	auto2->setDomain(HSConditionTrue::getSingleton());
	auto2->addTransition(auto2, HSConditionTrue::getSingleton(), 0);
	DEBBUG_PRINTF_info("");

	// auto1x2 :
	// ({A,B} #T)
	auto1x2 = new HSMode("mode1+2");
	auto1x2->addTask(&t1);
	auto1x2->addTask(&t2);
	auto1x2->setDomain(HSConditionTrue::getSingleton());
	auto1x2->addTransition(auto1x2, HSConditionTrue::getSingleton(), 0);

	DEBBUG_PRINTF_info("");


}

bool runSched(HybridSched* hs, int numIter){
    int i;
    DEBUG_TRACE_START();

    if(HS_RETVAL_OK != hs->reset(HS_SLOT_SIZE)){
        printf("hs1->reset fail\n");
        DEBUG_TRACE_RETURN( false);
    }
    for (i=0; i < numIter; i++){
        hs->tics();
        if(HS_RETVAL_OK != hs->run(HS_SLOT_SIZE)){
            printf("hs1->run fail\n");
            DEBUG_TRACE_RETURN( false);
        }
    }
    DEBUG_TRACE_RETURN( true);
}

bool testSched(HybridSched* hs1, HybridSched* hs2, int numIter){

    if (runSched(hs1, numIter) == false){
        return false;
    }
	DEBBUG_PRINTF_info("");
    if (runSched(hs2, numIter) == false){
        return false;
    }
	DEBBUG_PRINTF_info("");
	while (!outQ_before.empty() && !outQ_after.empty()){
		if (outQ_before.front() != outQ_after.front()){
			printf("test queues: %s != %s\n",outQ_before.front(), outQ_after.front());
			return false;
		}
		outQ_before.pop_front();
		outQ_after.pop_front();
	}
	DEBBUG_PRINTF_info("");
	if (!outQ_before.empty()){
		printf("!outQ_before.empty()\n");
		return false;
	}
	DEBBUG_PRINTF_info("");
	if (!outQ_after.empty()){
		printf("!outQ_after.empty()\n");
		return false;
	}
	DEBBUG_PRINTF_info("");

	return true;
}


bool testBasic(){
	DEBBUG_PRINTF_info("");
	makeAutos1();
	DEBBUG_PRINTF_info("");

	HybridSched hs1;
	DEBBUG_PRINTF_info("");
	hs1.setTestQ(&outQ_before);
	DEBBUG_PRINTF_info("");
	hs1.addTaskSpec(auto1);
	DEBBUG_PRINTF_info("");
	hs1.addTaskSpec(auto2);

	DEBBUG_PRINTF_info("");
	HybridSched hs2;
	hs2.setTestQ(&outQ_after);
	hs2.addTaskSpec(auto1x2);

	runSched(&hs1, 5);
	DEBBUG_PRINTF("result Q: ");
	for (std::deque<const char*>::iterator it = outQ_before.begin(); it!=outQ_before.end(); ++it){
	    DEBBUG_PRINTF("%s || ", *it);
	}
	    DEBBUG_PRINTF("\n");
	outQ_before.clear();

	DEBBUG_PRINTF_info("");

	return testSched(&hs1, &hs2, 5);

}

bool testOverflowPeriodic(){
    DEBUG_TRACE_START();
    makeTasks(); // for preparing the tasks

#if 0
    // result in blocking automata (result of slot overflow)
    // TODO - find a way to test that

    HybridSched hs1;
    hs1.setTestQ(&outQ_before);
    hs1.addPeriodTask(&t2, 2, 2);
    hs1.addPeriodTask(&t3, 3, 3);
#endif

    HybridSched hs1;
    outQ_before.clear();
    hs1.setTestQ(&outQ_before);
    hs1.addPeriodTask(&t3, 2, 3);
    hs1.addPeriodTask(&t2, 3, 3);

    if (false == runSched(&hs1, 15)){
        DEBUG_TRACE_RETURN(false);
    }

    DEBBUG_PRINTF("result Q: ");
    for (std::deque<const char*>::iterator it = outQ_before.begin(); it!=outQ_before.end(); ++it){
        DEBBUG_PRINTF("%s || ", *it);
    }
    // no overflow
    //for (std::deque<const char*>::iterator it = outQ_before.begin(); it!=outQ_before.end(); ++it){
        //if (strcmp(*it,)){

        //}
    //}

    // t2 occur at least every 3 intervals

    // t3 occur every 3 intervals
    if(Utils::_identLevel > 100){
        ASSERT("boo")
    }

    Utils::printTimes(">> ", Utils::_identLevel);\
    DEBBUG_PRINTF_info("end");\
    Utils::_identLevel--;
    DEBUG_TRACE_RETURN(true);

}


bool TestAutoUtil::test(){
	bool ans;

	ans = testBasic();
	if (ans){
		printf("testBasic pass\n");
	} else {
		printf("testBasic fail\n");
	}

	ans = testOverflowPeriodic();
	if (ans){
	    printf("testOverflowPeriodic pass\n");
	} else {
	    printf("testOverflowPeriodic fail\n");
	}

	return ans;
}
