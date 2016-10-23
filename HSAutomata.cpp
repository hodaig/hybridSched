/*
 * HSAutomataUtils.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#include "HSAutomata.h"

#include "conditions/HSConditionAND.h"
#include "HSTransition.h"
#include "HSMode.h"

#include <map>
#include <set>
#include <queue>
#include <utility>  //pair
#include <string>

#include "tests/Utils.h"

using namespace std;
#define MAX_MODES 50

HSAutomata::HSAutomata() :
            _allModes(), _initialsModes(), _currentMode(0){
    // do nothing
    _allModes.clear();
    _initialsModes.clear();
}

void HSAutomata::addMode(HSMode* mode){
    std::set<HSMode*>* nexts;

    if (_allModes.count(mode)){
        // no need to add
        return;
    }

    _allModes.insert(mode);

    // add all followers modes
    nexts = mode->getAllNexts();
    if (0 == nexts){
        ASSERT("null set");
    }

    for(std::set<HSMode*>::iterator it = nexts->begin(); it != nexts->end(); it++){
        addMode((*it));
    }

    delete nexts; nexts=0;

}

void HSAutomata::addModes(std::set<HSMode*>* modes){
    for(std::set<HSMode*>::iterator it = modes->begin(); it != modes->end(); it++){
        addMode((*it));
    }
}

HSMode* HSAutomata::removeMode(HSMode* mode){
    if (0 == _allModes.count(mode)){
        if (0 != _initialsModes.count(mode)){
            ASSERT("initial mode not in allModes");
        }
        return 0;
    }

    _allModes.erase(mode);

    if (0 != _initialsModes.count(mode)){
        _initialsModes.erase(mode);
    }

    return mode;
}

void HSAutomata::addInitialMode(HSMode* mode){
    if (0 == _allModes.count(mode)){
        _allModes.insert(mode);
        if (0 != _initialsModes.count(mode)){
            ASSERT("initial mode not in allModes");
        }
    }

    if (0 == _initialsModes.count(mode)){
        _initialsModes.insert(mode);
    }
}

void HSAutomata::addInitialModes(std::set<HSMode*>* modes){
    for(std::set<HSMode*>::iterator it = modes->begin(); it != modes->end(); it++){
        addInitialMode((*it));
    }
}

HSMode* HSAutomata::removeInitialMode(HSMode* mode){
    if (0 != _initialsModes.count(mode)){
        _initialsModes.erase(mode);
        return mode;
    } else {
        return 0;
    }
}

const std::set<HSMode*>* HSAutomata::getInitialsModes(){
    return &_initialsModes;
}

HSAutomata* HSAutomata::product(HSAutomata* other, uint32_t slot_size_micros){
    map<pair<HSMode*,HSMode*>,HSMode*> visited;
    queue<pair<HSMode*,HSMode*> > openList;
    set<pair<HSMode*,HSMode*> > initialPairs;
    const set<HSTransition*>* allTrans1;
    const set<HSTransition*>* allTrans2;
    HSMode* toMode;

    HSAutomata* prodAut = 0;

    DEBUG_TRACE_START();

    prodAut = new HSAutomata();

    // init
    for (set<HSMode*>::iterator it1 = _initialsModes.begin() ; it1 != _initialsModes.end() ; it1++){
        for (set<HSMode*>::iterator it2 = other->_initialsModes.begin() ; it2 != other->_initialsModes.end() ; it2++){
            if ((*it1)->getMaxTimeMicros() + (*it2)->getMaxTimeMicros() <= slot_size_micros){
                pair<HSMode*,HSMode*> initial(*it1, *it2);
                openList.push(initial);
                initialPairs.insert(initial); //unionMode(auto1[i], auto2[j]);
            }
        }
    }


    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, " ");
    while (!openList.empty()){
        pair<HSMode*,HSMode*> cur = openList.front();
        openList.pop();

        if (visited.count(cur)){
            // we already add that
            continue;
        }
        DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, " ");

        visited[cur] = unionMode(cur.first, cur.second);
        DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, " ");

        allTrans1 = cur.first->getTransitions();
        for (set<HSTransition*>::iterator it1 = allTrans1->begin(); it1 != allTrans1->end(); ++it1) {
            allTrans2 = cur.second->getTransitions();
            for (set<HSTransition*>::iterator it2 = allTrans2->begin(); it2 != allTrans2->end(); ++it2) {
                if ((*it1)->getNext()->getMaxTimeMicros() + (*it2)->getNext()->getMaxTimeMicros() <= slot_size_micros){
                    openList.push(pair<HSMode*,HSMode*>((*it1)->getNext(), (*it2)->getNext()));
                }
            }
        }
    }

    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, " ");
    // transitions
    for (map<pair<HSMode*,HSMode*>,HSMode*>::iterator it=visited.begin(); it!=visited.end(); ++it){
        pair<HSMode*,HSMode*> curInter = it->first;
        HSMode* curMode = it->second;
        if (!curMode){
            continue;
        }

        DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, " ");

        allTrans1 = curInter.first->getTransitions();
        for (set<HSTransition*>::iterator it1 = allTrans1->begin(); it1 != allTrans1->end(); ++it1) {
            allTrans2 = curInter.second->getTransitions();
            for (set<HSTransition*>::iterator it2 = allTrans2->begin(); it2 != allTrans2->end(); ++it2) {
                toMode = 0;
                toMode = visited[pair<HSMode*, HSMode*>((*it1)->getNext(), (*it2)->getNext())];
                if (toMode) {
                    curMode->addTransition(toMode,
                            new HSConditionAND((*it1)->getCond(), (*it2)->getCond()),
                            (*it1)->getCost() + (*it2)->getCost());
                }
            }
        }
    }

    // add all states
    for (map<pair<HSMode*,HSMode*>,HSMode*>::iterator it=visited.begin(); it!=visited.end(); ++it){
        if (it->second){
            prodAut->addMode(it->second);
        } else {
            DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_WARNING, "null mode");
        }
    }

    // update the final initial states
    for (set<pair<HSMode*,HSMode*> >::iterator it=initialPairs.begin(); it!=initialPairs.end(); ++it){
        //HSMode* tempMode = cutDeadEnds(visited[*it]);
        //if (tempMode){
            prodAut->addInitialMode(visited[*it]);
        //}
    }
    prodAut->cutDeadEnds();

    DEBUG_TRACE_RETURN(prodAut);
}

int HSAutomata::cutDeadEnds(){
    queue<HSMode*> deadEnds;
    HSMode* tempMode;

    // all the terminate modes
    set<HSMode*>::iterator it=_allModes.begin();
    while (it!=_allModes.end()){
        if ((*it)->getAllNexts()->empty()){
            deadEnds.push(*it);
        }
        it++;
    }

    // Backward search
    while (!deadEnds.empty()) {
        tempMode = deadEnds.front(); deadEnds.pop();

        // remove the mode itself
        removeMode(tempMode);

        //search for predecessors
        set<HSMode*>::iterator it=_allModes.begin();
        while (it!=_allModes.end()){
            (*it)->removeTransitions(tempMode);
            if ((*it)->getAllNexts()->empty()){
                deadEnds.push(*it);
            }
            it++;
        }
    }

    return 0;
}

#if 0 // TODO
HSMode** HSAutomata::product(HSMode** auto1, unsigned int initialCount1,
                                 HSMode** auto2, unsigned int initialCount2, uint32_t slot_size_micros){
	map<pair<HSMode*,HSMode*>,HSMode*> visited;
	queue<pair<HSMode*,HSMode*> > openList;
	HSTransition** allTrans1;
	HSTransition** allTrans2;
	HSMode* toMode;
	HSMode** result;
	pair<HSMode*,HSMode*> initialPairs[initialCount1 * initialCount2];

	unsigned int i,j;
	DEBUG_TRACE_START();
	//DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_TRACE, "start");

	// init
	result = new HSMode*[initialCount1 * initialCount2 + 1];
	result[initialCount1 * initialCount2] = 0; // null terminate this array
	for (i = 0; i < initialCount1; i++) {
	    for (j = 0; j < initialCount2; j++) {
	        if (auto1[i]->getMaxTimeMicros() + auto2[j]->getMaxTimeMicros() <= slot_size_micros){
	            pair<HSMode*,HSMode*> initial(auto1[i], auto2[j]);
	            openList.push(initial);
	            initialPairs[(i*initialCount2) + j] = initial; //unionMode(auto1[i], auto2[j]);
	        }
	    }
	}


	DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, " ");
	while (!openList.empty()){
	    pair<HSMode*,HSMode*> cur = openList.front();
	    openList.pop();

	    if (visited.count(cur)){
	        // we already add that
	        continue;
	    }
	    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, " ");

	    visited[cur] = unionMode(cur.first, cur.second);
	    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, " ");

	    allTrans1 = cur.first->getTransitions();
	    while (allTrans1[0]){
	        //openList.push(pair<HSMode*,HSMode*>(allTrans1[0]->getNext(), cur.second));

	        allTrans2 = cur.second->getTransitions();
	        while (allTrans2[0]){
	            if (allTrans1[0]->getNext()->getMaxTimeMicros() + allTrans2[0]->getNext()->getMaxTimeMicros() <= slot_size_micros){
	                openList.push(pair<HSMode*,HSMode*>(allTrans1[0]->getNext(), allTrans2[0]->getNext()));
	            }
	            allTrans2++;
	        }
	        allTrans1++;
	    }
#if 0 // TODO
	    allTrans2 = cur.second->getTransitions();
	    while (allTrans2[0]){
	        openList.push(pair<HSMode*,HSMode*>(cur.first, allTrans2[0]->getNext()));
	        allTrans2++;
	    }
#endif
	}

	DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, " ");
	// transitions
	for (map<pair<HSMode*,HSMode*>,HSMode*>::iterator it=visited.begin(); it!=visited.end(); ++it){
	    pair<HSMode*,HSMode*> curInter = it->first;
	    HSMode* curMode = it->second;
	    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, " ");

	    allTrans1 = curInter.first->getTransitions();
	    while (allTrans1[0]){
	        //toMode = visited[pair<HSMode*, HSMode*>(allTrans1[0]->getNext(), curInter.second)];
	        //curMode->addTransition(toMode, allTrans1[0]->getCond(), allTrans1[0]->getReset(), allTrans1[0]->getCost());

	        allTrans2 = curInter.second->getTransitions();
	        while (allTrans2[0]){
	            toMode = 0;
	            toMode = visited[pair<HSMode*, HSMode*>(allTrans1[0]->getNext(), allTrans2[0]->getNext())];
	            if (toMode) {
	                curMode->addTransition(toMode, new HSConditionAND(allTrans1[0]->getCond(), allTrans2[0]->getCond()), allTrans1[0]->getCost());
	            }
	            allTrans2++;
	        }
	        allTrans1++;
	    }

#if 0 // TODO
	    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, " ");
	    allTrans2 = curInter.second->getTransitions();
	    while (allTrans2[0]){
	        toMode = visited[pair<HSMode*, HSMode*>(curInter.first, allTrans2[0]->getNext())];
	        curMode->addTransition(toMode, allTrans2[0]->getCond(), allTrans2[0]->getCost());
	        allTrans2++;
	    }
#endif
	}

	// update the final initial states
	j = 0;
	for (i=0 ; i<(initialCount1 * initialCount2) && result[i] ; i++){

	    result[j] = cutDeadEnds(visited[initialPairs[i]]);
	    if (result[j]){
	        j++;
	    }
	}

	//DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_TRACE, "end");
	DEBUG_TRACE_END();
	return result;
}
#endif

HSMode* HSAutomata::unionMode(HSMode* mode1, HSMode* mode2){
    DEBUG_TRACE_START();

    std::string newName;
    newName.append(mode1->getName());
    newName.append("}X{");
    newName.append(mode2->getName());
    HSMode* newMode = new HSMode(newName.c_str());
    const set<HybridSched::Task*>* tasks;

    if (0 == mode1){
        ASSERT("null mode1")
    }
    if (0 == mode2){
	    ASSERT("null mode2")
	}

	tasks = mode1->getTasks();
	DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, "add tasks of %s", mode1->getName());
	for (set<HybridSched::Task*>::iterator it = tasks->begin(); it != tasks->end(); ++it) {
	    if (0 == (*it)){
	        ASSERT("null task in mode");
	    }
	    newMode->addTask(*it);
    }

	tasks = mode2->getTasks();
	DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG, "add tasks of %s", mode2->getName());
	for (set<HybridSched::Task*>::iterator it = tasks->begin(); it != tasks->end(); ++it) {
	    if (0 == (*it)){
	        ASSERT("null task in mode");
	    }
		newMode->addTask(*it);
	}
	newMode->setDomain(new HSConditionAND(mode1->getDomain(), mode2->getDomain()));
	//DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_TRACE, "end");

    DEBUG_TRACE_END();
	return newMode;

}
#if 0
HSMode** HSAutomata::OverflowModeFilter(HSMode** initial, unsigned int initialCount, uint32_t max_slot_time_micros){
    HSMode** newInitial;
    unsigned int newInitialCount = 0;

    HSMode* currentMode;
    set<HSMode*> visited; // TODO - find better data structure that allow the operation "contains()"
    queue<HSMode*> openList;
    HSTransition** transArr;

    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_TRACE, "initialCount=%d, max_slot_time_micros=%d", initialCount, max_slot_time_micros)

    // initial
    //newInitial = (HSMode**) malloc(initialCount * sizeof(HSMode));
    newInitial = new HSMode*[initialCount+1];
    while (initial[0] && initialCount > 0){
        if (initial[0]->getMaxTimeMicros() <= max_slot_time_micros) {
            newInitial[newInitialCount] = initial[0];
            openList.push(initial[0]);
            newInitialCount++;
        }
        initialCount--;
        initial++;
    }
    newInitial[newInitialCount]=0;

    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG,"newInitialCount=%d", newInitialCount);
    if (0 == newInitialCount){
        ASSERT("all initial modes are too long");
        return 0;
    }

    // all the rest
    while (!openList.empty()){
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
        currentMode = openList.front();
        openList.pop();

        visited.insert(currentMode);

    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
        currentMode->removeOverflowTransitions(max_slot_time_micros);
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
        transArr = currentMode->getTransitions();

    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
        while (transArr[0]){
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
            if (0 == visited.count(transArr[0]->getNext())){
                // first time visited
                openList.push(transArr[0]->getNext());
            }
            transArr++;
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
        }
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");
    }
    DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_BEDUG," ");

return newInitial;
}
#endif

#if 0 // TODO
HSMode* HSAutomata::cutDeadEnds(HSMode* initial){
    HSTransition** allTrans;
    HSMode* next;
    unsigned int i;

    if(!initial) {
        DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_WARNING, "null init mode");
        return 0;
    }

    allTrans = initial->getTransitions();
    while (*allTrans) {
        next = cutDeadEnds((*allTrans)->getNext());
        if (next){
            i++;
            allTrans++;
        } else {
            initial->removeTransition(*allTrans);
        }
    }

    if (i > 0) {
        return initial;
    } else {
        return 0;
    }

}
#endif
/*
HSMode** HSAutomata::cutDeadEnds(HSMode** initial){
    HSMode** finalInitis;
    unsigned int i = 0;

    finalInitis = new HSMode*[Utils::getNullTerminatedArraySize((void**)initial)];
    while (initial && *initial){
        finalInitis[i] = cutDeadEnds(*initial);
        if (finalInitis[i]){
            i++;
        }
    }

    if (i > 0){
        return finalInitis;
    } else {
        free(finalInitis);
        return 0;
    }
}
*/

