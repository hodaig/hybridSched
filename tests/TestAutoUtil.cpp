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
#include <string.h>             // strcmp()
#include <unistd.h>             // usleep()


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
	//auto1->setDomain(HSConditionTrue::getSingleton());
	HSTransition* trans = new HSTransition(auto1, HSConditionTrue::getSingleton(), auto1, 0);
	auto1->addTransition(trans->addTask(&t1));
	DEBBUG_PRINTF_info("");
	// auto2 :
	// ({B} #T)
	auto2 = new HSMode("mode2");
	//auto2->setDomain(HSConditionTrue::getSingleton());
	trans = new HSTransition(auto2, HSConditionTrue::getSingleton(), auto2, 0);
	auto2->addTransition(trans->addTask(&t2));
	DEBBUG_PRINTF_info("");

	// auto1x2 :
	// ({A,B} #T)
	auto1x2 = new HSMode("mode1+2");
	//auto1x2->setDomain(HSConditionTrue::getSingleton());
	trans = new HSTransition(auto1x2, HSConditionTrue::getSingleton(), auto1x2, 0);
	auto1x2->addTransition(trans->addTask(&t1));
	auto1x2->addTransition(trans->addTask(&t2));

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


struct periodicTestSpecs_t{
    uint16_t min;
    uint16_t max;
    uint16_t time;
    const char*    name;
    uint16_t timeStamp;     // for the after-run check
};

#if 0
struct periodicTest_t{
    uint16_t taskCount;
    periodicTestSpecs_t tasks[20];
};
#endif
struct periodicTestSpecs_t periodTests[][20] = {
             // {min, max, time, name}
        {
                {1,   1,   50,   "t1"}
        },
        {
                {2,   2,   50,   "t2"}
        },
        {
                {2,   2,   50,   "t3"},
                {2,   2,   50,   "t4"}
        },
        {
                {1,   2,   60,   "t5"},
                {4,   6,   60,   "t6"}
        },
        {
                {1,   2,   60,   "t7"},
                {2,   2,   60,   "t8"}
        },
        {
                {2,   3,   60,   "t9"},
                {3,   3,   60,   "t10"}
        },
        {
                {2,   2,   60,   "t11"},
                {3,   4,   60,   "t12"}
        }
};

bool testOverflowPeriodic(){
    unsigned int i,j;
    HybridSched* hs;
    HybridSched::Task* task;

    DEBUG_TRACE_START();

    for (i=2; i<ARRAY_SIZE(periodTests); i++){
        // prepare the scheduler
        hs = new HybridSched();
        outQ_before.clear();
        hs->setTestQ(&outQ_before);

        for (j=0; (j<20 && periodTests[i][j].min > 0); j++){
            task = new HybridSched::Task();
            task->type = HS_TASK_TYPE_TEST;
            task->name = periodTests[i][j].name;
            task->max_time_micros = periodTests[i][j].time;

            hs->addPeriodTask(task,periodTests[i][j].min,periodTests[i][j].max);
        }

        // run the scheduler
        if (false == runSched(hs, 50)){
            DEBBUG_PRINTF_info("periodic test: %d, fail to prepare the scheduler", i);
            DEBUG_TRACE_RETURN(false);
        }

        DEBBUG_PRINTF("result Q: ");
        for (std::deque<const char*>::iterator it = outQ_before.begin(); it!=outQ_before.end(); ++it){
            DEBBUG_PRINTF("%s ", *it);
        }
        DEBBUG_PRINTF("\n");

        // some sanity
        if (outQ_before.size() < 30){
            DEBBUG_PRINTF_info("fail: test %d (outQ_before.size() < 30)", i);
            DEBUG_TRACE_RETURN(false);
        }
#if 0
        tickString = outQ_before.front();
        if (0 != strcmp(tickString, HS_TICS_SEPARATOR)){
            DEBBUG_PRINTF_info("fail: test %d not starter with tick mark", i);
            DEBUG_TRACE_RETURN(false);
        }
#endif

        // inspect results for individual task
        for (j=0; (j<20 && periodTests[i][j].min > 0); j++){
            unsigned int tics = 0;
            periodTests[i][j].timeStamp = 0;

            DEBBUG_PRINTF("test %s:", periodTests[i][j].name);
            for (std::deque<const char*>::iterator it = outQ_before.begin(); it!=outQ_before.end(); ++it){
                DEBBUG_PRINTF("%s ", *it);
                if (0 == strcmp((*it), HS_TICS_SEPARATOR)){
                    tics++;
                } else if (0 == strcmp((*it), periodTests[i][j].name)){
                    // min & max
                    if (tics - periodTests[i][j].timeStamp < periodTests[i][j].min){
                        if (0 == periodTests[i][j].timeStamp) {
                            // the first time do not count;
                        } else {
                            DEBBUG_PRINTF_info("periodic test task: %d:%d, fail", i, j);
                            DEBUG_TRACE_RETURN(false);
                        }
                    }
                    if (tics - periodTests[i][j].timeStamp > periodTests[i][j].max){
                        DEBBUG_PRINTF_info("periodic test task: %d:%d, fail", i, j);
                        DEBUG_TRACE_RETURN(false);
                    }
                    periodTests[i][j].timeStamp = tics;
                }
            }

            DEBBUG_PRINTF("\n");
            if (tics - periodTests[i][j].timeStamp > periodTests[i][j].max){
                DEBBUG_PRINTF_info("periodic test task: %d:%d, fail", i, j);
                DEBUG_TRACE_RETURN(false);
            }

            if (tics < 30) {
                DEBBUG_PRINTF_info("fail: test %d, not enough ticks", i);
                DEBUG_TRACE_RETURN(false);
            }
        }

        // inspect results for time overflow
        DEBBUG_PRINTF("test time overflow:");
        uint32_t time_milis = 0;
        for (std::deque<const char*>::iterator it = outQ_before.begin(); it!=outQ_before.end(); ++it){
            if (0 == strcmp((*it), HS_TICS_SEPARATOR)){
                time_milis = 0;
            } else {
                for (j=0; (j<20 && periodTests[i][j].min > 0); j++){
                    if (periodTests[i][j].name == (*it)) {
                        time_milis += periodTests[i][j].time;
                    }
                }
            }
            if (time_milis > HS_SLOT_SIZE_MICROS) {
                DEBBUG_PRINTF_info("fail: test %d, overflow", i);
                DEBUG_TRACE_RETURN(false);
            }
        }

        // free the scheduler
        delete hs; hs=0;
    }

    DEBUG_TRACE_RETURN(true);

}


bool TestAutoUtil::test(){
	bool ans;

	//ans = testBasic();
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
