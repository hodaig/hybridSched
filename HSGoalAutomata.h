/*
 * xmlUtils.h
 *
 *  Created on: Oct 27, 2016
 *      Author: hodai
 */

#ifndef XMLUTILS_H_
#define XMLUTILS_H_

#include "pugixml-1.7/src/pugixml.hpp"
#include "HSMode.h"
#include <map>

class HSGoalAutomata {
private:
    pugi::xml_document _doc;         // the base xml object
    unsigned int _modeCounter;
    unsigned int _transitionCounter;

    map<HSMode*, char*> _modeTostateIdMap;
    map<char*, HSMode*> _stateIdToModeMap;
public:

    HSGoalAutomata();
    HSGoalAutomata(const char* fileName);
    HSGoalAutomata(HSAutomata* automata);

    ~HSGoalAutomata();

    /*
     * write this automata xml object to file
     */
    void saveXmlToFile(const char* fileName);

    /*
     * add state to this xml representation
     * !! only the given mode is added, follow modes and transitions are ignored
     */
    void addState(HSMode* mode);

    /*
     * add initial state to this xml representation (add also as state if needed
     * !! only the given mode is added, follow modes and transitions are ignored
     */
    void addInitialState(HSMode* mode);

    /*
     * add transition
     * !! only the given transition is added
     */
    void addTransition(HSTransition* transition);

    /*
     * calculate the product of this automata and 'other'.
     * the result is this object
     */
    void product(HSGoalAutomata* other);

    /*
     * simplify this automata.
     * the result is this object
     */
    void simplify();

    /*
     * return new automata object correspond to this xml
     * -- the user should delete this
     */
    HSAutomata* getAsAutomata();



private:
    /*
     * execute goal command
     */
    //void executeGoal(const char* command);

    void clean_modeTostateIdMap();

    void readFromFile(const char* fileName);
};

#endif /* XMLUTILS_H_ */
