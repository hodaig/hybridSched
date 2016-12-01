/*
 * xmlUtils.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: hodai
 */

#include "HSGoalAutomata.h"

#include "tests/Utils.h"
#include "conditions/HSConditionTrue.h"
#include "HSAutomata.h"

#include <iostream>
#include <string.h>

#include <stdlib.h>     // atoi()

#define HS_XML_BASE "Structure"

#define HS_XML_states "StateSet"
#define HS_XML_state "State"

#define HS_XML_initial_states "InitialStateSet"
#define HS_XML_initial_state_id "StateID"

#define HS_XML_Acc "Acc"
#define HS_XML_Acc_id HS_XML_initial_state_id

#define HS_XML_transitions "TransitionSet"
#define HS_XML_transition "Transition"
#define HS_XML_trans_From "From"
#define HS_XML_trans_To "To"
#define HS_XML_trans_Label "Label"

#define HS_XML_PROPS_TYPE "type"
#define HS_XML_PROPS_TYPE_value "Propositional"
#define HS_XML_PROPS_BASE "Alphabet"
#define HS_XML_PROPS_INDIVIDUAL "Proposition"


#define HS_XML_state_id "sid"
#define HS_XML_transition_id "tid"

#define HS_GOAL_WORKING_DIRECTORY "." // where goal files will be saved
//#define HS_GOAL_EXEC              "/home/hodai/bgu/teza/teza/goal/GOAL-20151018/gc"
#define HS_GOAL_EXEC              "./goal-cl"

using namespace pugi;
HSGoalAutomata::HSGoalAutomata():
                    _modeCounter(0), _transitionCounter(0){
    _doc.load_string("<Structure label-on=\"Transition\" type=\"FiniteStateAutomaton\">\
    <Alphabet type=\"Propositional\">\
    </Alphabet>\
    <StateSet>\
    </StateSet>\
    <InitialStateSet>\
    </InitialStateSet>\
    <TransitionSet>\
    </TransitionSet>\
    <Acc type=\"Buchi\"/>\
    </Structure>");

    pugi::xml_node decl = _doc.prepend_child(pugi::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";
    decl.append_attribute("standalone") = "no";

}

HSGoalAutomata::HSGoalAutomata(const char* fileName):
                            _modeCounter(0), _transitionCounter(0){
    readFromFile(fileName);
}


HSGoalAutomata::HSGoalAutomata(HSAutomata* automata): HSGoalAutomata(){
    if (!automata){
        ASSERT("null automata");
    }

    for (set<HSMode*>::iterator it = automata->getAllModes()->begin(); it != automata->getAllModes()->end(); it++){
        addState(*it);
    }

    for (set<HSMode*>::iterator it = automata->getInitialsModes()->begin(); it != automata->getInitialsModes()->end(); it++){
        addInitialState(*it);
    }

    for (set<HSTransition*>::iterator it = automata->getTransitions()->begin(); it != automata->getTransitions()->end(); it++){
        addTransition(*it);
    }
}

HSGoalAutomata::~HSGoalAutomata(){
    clean_modeTostateIdMap();
}

void HSGoalAutomata::readFromFile(const char* fileName){
    xml_parse_result res= _doc.load_file(fileName);
    if (!res.operator bool()){
        DEBBUG_PRINTF_INFO_LEVEL(DEBUG_VERB_FATAL, "%s", res.description());
        ASSERT("fail to read file")
    }

    clean_modeTostateIdMap();

    // update mode counter
    xml_node states = _doc.child(HS_XML_BASE).child(HS_XML_states);
    if (states.empty()){
        ASSERT("no base states root in xml");
    }

    for (xml_node state = states.first_child(); state; state=state.next_sibling()){
        if (_modeCounter < state.attribute(HS_XML_state_id).as_uint()){
            _modeCounter = state.attribute(HS_XML_state_id).as_uint();
        }
    }

    // update mode transition
    xml_node trans = _doc.child(HS_XML_BASE).child(HS_XML_transitions);
    if (trans.empty()){
        ASSERT("no base trans root in xml");
    }

    for (xml_node tran = trans.first_child(); tran; tran=tran.next_sibling()){
        if (_transitionCounter < tran.attribute(HS_XML_transition_id).as_uint()){
            _transitionCounter = tran.attribute(HS_XML_transition_id).as_uint();
        }
    }

}

void HSGoalAutomata::clean_modeTostateIdMap(){
    for (std::map<HSMode*, char*>::iterator it = _modeTostateIdMap.begin(); it !=_modeTostateIdMap.end(); ++it){
        free(it->second);
    }
    _modeTostateIdMap.clear();
}

void HSGoalAutomata::saveXmlToFile(const char* fileName){
    //std::cout << "Saving result: " << doc.save_file("save_file_output.xml") << std::endl;
    if (!_doc.save_file(fileName)){
        ASSERT("fail to save xml file");
    }
}

void HSGoalAutomata::addState(HSMode* mode){
    if (!mode){
        ASSERT("null moed");
    }

    if (0 < _modeTostateIdMap.count(mode)){
        // the mode is already there
        return;
    }

    // add the state
    xml_node states = _doc.child(HS_XML_BASE).child(HS_XML_states);
    if (states.empty()){
        ASSERT("no base states root in xml");
    }

    xml_node newState = states.append_child(HS_XML_state);
    if (newState.empty()){
        ASSERT("fail to create state");
    }

    char* modeId = (char*) malloc(10);
    sprintf(modeId,"%d", _modeCounter++);
    _modeTostateIdMap[mode] = modeId;
    _stateIdToModeMap[modeId] = mode;
    newState.append_attribute(HS_XML_state_id).set_value(modeId);

    // make it acceptance
    // TODO - maybe is beter to let the user decide which states are acceptance
    xml_node accStates = _doc.child(HS_XML_BASE).child(HS_XML_Acc);
    if (accStates.empty()){
        ASSERT("no base Acc root in xml");
    }

    xml_node newAcc = accStates.append_child(HS_XML_Acc_id);
    if (newAcc.empty()){
        ASSERT("fail to create state");
    }

    newAcc.append_child(pugi::node_pcdata).set_value(_modeTostateIdMap[mode]);

}

void HSGoalAutomata::addInitialState(HSMode* mode){
    if (!mode){
        ASSERT("null moed");
    }

    xml_node initStates = _doc.child(HS_XML_BASE).child(HS_XML_initial_states);

    if (initStates.empty()){
        ASSERT("no base states root in xml");
    }

#if 0 // TODO - need to check for duplications
    if (0 < _modeTostateIdMap.count(mode) && initStates.children()){
        // the mode is already there
        return;
    }
#endif

    xml_node newState = initStates.append_child(HS_XML_initial_state_id);
    if (newState.empty()){
        ASSERT("fail to create state");
    }

    if (0 == _modeTostateIdMap.count(mode)){
        addState(mode);
    }

    newState.append_child(pugi::node_pcdata).set_value(_modeTostateIdMap[mode]);
}

void HSGoalAutomata::addTransition(HSTransition* transition){
    if (!transition){
        ASSERT("null transition");
    }

    if (0 == _modeTostateIdMap.count(transition->getFrom())){
        ASSERT("null fromMode");
    }

    if (0 == _modeTostateIdMap.count(transition->getTo())){
        ASSERT("null toMode");
    }

    xml_node trans = _doc.child(HS_XML_BASE).child(HS_XML_transitions);
    if (trans.empty()){
        ASSERT("no base trans root in xml");
    }

    xml_node newTran = trans.append_child(HS_XML_transition);
    if (newTran.empty()){
        ASSERT("fail to create transition");
    }

    newTran.append_attribute(HS_XML_transition_id).set_value(_transitionCounter++);

    xml_node from = newTran.append_child(HS_XML_trans_From);
    if (newTran.empty()){
        ASSERT("fail to create from");
    }
    from.append_child(pugi::node_pcdata).set_value(_modeTostateIdMap[transition->getFrom()]);

    xml_node to = newTran.append_child(HS_XML_trans_To);
    if (newTran.empty()){
        ASSERT("fail to create to");
    }
    to.append_child(pugi::node_pcdata).set_value(_modeTostateIdMap[transition->getTo()]);

    xml_node label = newTran.append_child(HS_XML_trans_Label);
    if (newTran.empty()){
        ASSERT("fail to create lable");
    }
    std::string* strLabel = transition->getLabel();
    label.append_child(pugi::node_pcdata).set_value(strLabel->c_str());
    free(strLabel); strLabel = 0;

    // add propositions
    std::set<std::string*>* propo = transition->getPropositions();
    xml_node propNodes = _doc.child(HS_XML_BASE).child(HS_XML_PROPS_BASE);
    if (propNodes.empty()){
        ASSERT("no base propNodes root in xml");
    }
    //propNodes.append_attribute(HS_XML_PROPS_TYPE).set_value(HS_XML_PROPS_TYPE_value);
    for (std::set<std::string*>::iterator it = propo->begin(); it != propo->end(); it++){
        xml_node newProp = propNodes.append_child(HS_XML_PROPS_INDIVIDUAL);
        if (newProp.empty()){
            ASSERT("fail to create prop");
        }
        newProp.append_child(pugi::node_pcdata).set_value((*it)->c_str());
        free(*it);
    }
    free(propo);


}

void HSGoalAutomata::product(HSGoalAutomata* other){
    saveXmlToFile(HS_GOAL_WORKING_DIRECTORY "/auto1.gff");
    other->saveXmlToFile(HS_GOAL_WORKING_DIRECTORY "/auto2.gff");

    system(HS_GOAL_EXEC " product "\
            "-o " HS_GOAL_WORKING_DIRECTORY "/out.gff "\
            HS_GOAL_WORKING_DIRECTORY "/auto1.gff "\
            HS_GOAL_WORKING_DIRECTORY "/auto2.gff ");

    readFromFile(HS_GOAL_WORKING_DIRECTORY "/out.gff");
/*
    system("rm " HS_GOAL_WORKING_DIRECTORY "/out.gff "\
            HS_GOAL_WORKING_DIRECTORY "/auto1.gff "\
            HS_GOAL_WORKING_DIRECTORY "/auto2.gff ");
*/
}

void HSGoalAutomata::simplify(){
    saveXmlToFile(HS_GOAL_WORKING_DIRECTORY "/beforSimplify.gff");

    system(HS_GOAL_EXEC " reduce "\
            "-o " HS_GOAL_WORKING_DIRECTORY "/afterReduce.gff "\
            HS_GOAL_WORKING_DIRECTORY "/beforSimplify.gff ");

    system(HS_GOAL_EXEC " simplify "\
            "-o " HS_GOAL_WORKING_DIRECTORY "/afterSimplify.gff "\
            HS_GOAL_WORKING_DIRECTORY "/afterReduce.gff ");

    readFromFile(HS_GOAL_WORKING_DIRECTORY "/afterSimplify.gff");
/*
    system("rm " HS_GOAL_WORKING_DIRECTORY "/afterSimplify.gff "\
            HS_GOAL_WORKING_DIRECTORY "/afterReduce.gff "\
            HS_GOAL_WORKING_DIRECTORY "/beforSimplify.gff ");
*/
}

HSAutomata* HSGoalAutomata::getAsAutomata(){
    HSAutomata* ans = new HSAutomata();
    map<int, HSMode*> modesIdMap;

    // add all modes
    xml_node states = _doc.child(HS_XML_BASE).child(HS_XML_states);
    if (states.empty()){
        ASSERT("no base states root in xml");
    }
    for (xml_node state = states.first_child(); state; state=state.next_sibling()){
        xml_attribute temAtribute = state.attribute(HS_XML_state_id);
        if (temAtribute.empty()){
            ASSERT("bad automata");
        }
        int tempId = temAtribute.as_int();
        if (0 < modesIdMap.count(tempId)) {
            ASSERT("duplicated mode");
        }
        modesIdMap[tempId]= new HSMode();
        ans->addMode(modesIdMap[tempId]);
    }

    // add all initial modes
    states = _doc.child(HS_XML_BASE).child(HS_XML_initial_states);
    if (states.empty()){
        ASSERT("no base initial states root in xml");
    }
    for (xml_node state = states.first_child(); state; state=state.next_sibling()){

        int tempId = state.text().as_int();
        if (0 == modesIdMap.count(tempId)) {
            ASSERT("mode not found");
        }
        ans->addInitialMode(modesIdMap[tempId]);
    }

    // add all transitions
    xml_node trans = _doc.child(HS_XML_BASE).child(HS_XML_transitions);
    if (trans.empty()){
        ASSERT("no base trans root in xml");
    }
    for (xml_node tran = trans.first_child(); tran; tran=tran.next_sibling()){
        int from = tran.child(HS_XML_trans_From).text().as_int();
        if (0 == modesIdMap.count(from)){
            ASSERT("no mode");
        }
        HSMode* fromMode = modesIdMap[from];

        int to = tran.child(HS_XML_trans_To).text().as_int();
        if (0 == modesIdMap.count(to)){
            ASSERT("no mode");
        }
        HSMode* toMode = modesIdMap[to];

        HSTransition* newTrans = new HSTransition(fromMode, HSConditionTrue::getSingleton(), toMode, 0);

        const char* labelStr = tran.child(HS_XML_trans_Label).text().as_string();
        newTrans->addTasks(labelStr);
        newTrans->addNoTasks(labelStr);

        ans->addTransition(newTrans);

    }


    return ans;
}


