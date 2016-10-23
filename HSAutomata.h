/*
 * HSAutomataUtils.h
 *
 *  Created on: Sep 22, 2016
 *      Author: hodai
 */

#ifndef HSAUTOMATA_H_
#define HSAUTOMATA_H_

//#include "HSMode.h"

#include <set>

#include "tests/Utils.h"

class HSMode;

class HSAutomata {
private:
    std::set<HSMode*> _allModes;
    std::set<HSMode*> _initialsModes;

public:
    HSMode*              _currentMode;

public:
    HSAutomata();

    void addMode(HSMode* mode);
    void addModes(std::set<HSMode*>* modes);
    /*
     * remove mode from allModes and from initialModes
     * return - the mode itself if exist, 0 if not
     */
    HSMode* removeMode(HSMode* mode);

    void addInitialMode(HSMode* mode);
    void addInitialModes(std::set<HSMode*>* modes);
    /*
     * remove mode from initialModes
     * return - the mode itself if exist, 0 if not
     */
    HSMode* removeInitialMode(HSMode* mode);
    /*
     * return pointer to the initial modes set
     * -- do not edit
     */
    const std::set<HSMode*>* getInitialsModes();

    /*
     * product this automata with other automata.
     * return - a new HSAutomata for the product
     */
    HSAutomata* product(HSAutomata* other, uint32_t slot_size_micros);
    /*
     * cut all terminated modes
     * return - number of initial modes left after cutting
     */
    int cutDeadEnds();


private:

    //static HSMode** product(HSMode** auto1, unsigned int initialCount1, HSMode** auto2, unsigned int initialCount2, uint32_t slot_size_micros);
	static HSMode* unionMode(HSMode* mode1, HSMode* mode2);
	static HSMode** OverflowModeFilter(HSMode** initial, unsigned int initialCount, uint32_t max_slot_time_micros);

	//static HSMode* cutDeadEnds(HSMode* initial);
	//static HSMode** cutDeadEnds(HSMode** initial);

};

#endif /* HSAUTOMATA_H_ */
