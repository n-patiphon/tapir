#include "BeliefNode.hpp"

#include <cmath>                        // for log, sqrt
#include <ctime>                        // for clock, CLOCKS_PER_SEC, clock_t

#include <iostream>                     // for cerr, endl, operator<<, basic_ostream, ostream
#include <map>                          // for map, _Rb_tree_iterator, map<>::iterator
#include <memory>                       // for unique_ptr
#include <queue>                        // for queue
#include <utility>                      // for pair
#include <tuple>                        // for ignore
#include <vector>                       // for vector, vector<>::iterator

#include "defs.hpp"                     // for make_unique, RandomGenerator
#include "Action.hpp"                   // for Action
#include "ActionNode.hpp"               // for ActionNode
#include "HistoryEntry.hpp"             // for HistoryEntry
#include "Observation.hpp"              // for Observation
#include "StateInfo.hpp"             // for StateInfo

using std::cerr;
using std::endl;

long BeliefNode::currId = 0;
double BeliefNode::exploreParam = 1.0;
std::clock_t BeliefNode::startTime = std::clock();
long BeliefNode::maxParticles = 0;
long BeliefNode::nStVars = 0;

BeliefNode::BeliefNode() :
    BeliefNode(currId) {
}

BeliefNode::BeliefNode(long id) :
    distChecked(false),
    id(id),
    nParticles(0),
    nextActionToTry(0),
    bestMeanQValue(0),
    bestAction(-1),
    tLastAddedParticle(0),
    tNNComp(-1.0),
    tEmdSig(-1.0),
    nnBel(nullptr),
    particles(),
    actChildren() {
    if (currId <= id) {
        currId = id + 1;
    }
}

Action BeliefNode::getUCBAction() {
    double tmpVal;
    ActionMap::iterator actionIter = actChildren.begin();
    double maxVal = (actionIter->second->meanQValue + exploreParam *
            std::sqrt(std::log(nParticles) / actionIter->second->nParticles));
    Action bestActId = actionIter->first;
    actionIter++;
    for (; actionIter != actChildren.end(); actionIter++) {
        tmpVal = (actionIter->second->meanQValue + exploreParam * std::sqrt(
                     std::log(nParticles) / actionIter->second->nParticles));
        if (maxVal < tmpVal) {
            maxVal = tmpVal;
            bestActId = actionIter->first;
        }
    }
    return bestActId;
}

Action BeliefNode::getBestAction() {
    if (getNActChildren() == 0) {
        cerr << "No children - could not retrieve best action!!" << endl;
        return -1;
    }
    updateBestValue();
    return bestAction;
}

double BeliefNode::getBestMeanQValue() {
    updateBestValue();
    return bestMeanQValue;
}

void BeliefNode::updateBestValue() {
    if (getNActChildren() == 0) {
        return;
    }
    ActionMap::iterator actionIter = actChildren.begin();
    double bestQVal = actionIter->second->meanQValue;
    Action bestActId = actionIter->first;
    actionIter++;
    for (; actionIter != actChildren.end(); actionIter++) {
        if (bestQVal < actionIter->second->meanQValue) {
            bestQVal = actionIter->second->meanQValue;
            bestActId = actionIter->first;
        }
    }
}

void BeliefNode::add(HistoryEntry *newHistEntry) {
    tLastAddedParticle = (double) (std::clock() - startTime)
                         * 10000/ CLOCKS_PER_SEC;
    particles.push_back(newHistEntry);
    nParticles++;
}

BeliefNode *BeliefNode::addChild(Action const &action, Observation const &obs) {
    std::unique_ptr<ActionNode> newActionNode = std::make_unique<ActionNode>(
            action);
    std::map<Action, std::unique_ptr<ActionNode> >::iterator actionIter;
    std::tie(actionIter, std::ignore) = actChildren.insert(std::make_pair(
            action, std::move(newActionNode)));
    ActionNode *actChild = actionIter->second.get();
    return actChild->addChild(obs);
}

HistoryEntry *BeliefNode::sampleAParticle(RandomGenerator *randGen) {
    return particles[std::uniform_int_distribution<long>(0, nParticles - 1)(*randGen)];
}

void BeliefNode::updateQValue(Action &action, double increase) {
    ActionMap::iterator iter = actChildren.find(action);
    if (iter == actChildren.end()) {
        return;
    }
    iter->second->updateQValue(increase);
    updateBestValue();
}

void BeliefNode::updateQValue(Action &action, double oldValue, double newValue,
        bool reduceParticles) {
    ActionMap::iterator iter = actChildren.find(action);
    if (iter == actChildren.end()) {
        return;
    }
    iter->second->updateQValue(oldValue, newValue, reduceParticles);
    updateBestValue();
}

double BeliefNode::distL1Independent(BeliefNode *b) {
    double dist = 0.0;
    long nComparisons = nParticles * b->nParticles;
    if (nComparisons > 1000000) {
        cerr << "Comparing belief nodes; " << nParticles << " * " << b->nParticles << endl;
    }
    for (HistoryEntry *entry1 : particles) {
        for (HistoryEntry *entry2 : b->particles) {
            dist += entry1->getState()->distanceTo(*entry2->getState());
        }
    }
    return dist / (nParticles * b->nParticles);
}


BeliefNode *BeliefNode::getChild(Action const &action, Observation const &obs) {
    ActionMap::iterator iter = actChildren.find(action);
    if (iter == actChildren.end()) {
        return nullptr;
    }
    return iter->second->getBeliefChild(obs);
}

void BeliefNode::enqueueChildren(std::queue<BeliefNode *> &queue) {
    ActionMap::iterator actionIter;
    for (actionIter = actChildren.begin(); actionIter != actChildren.end();
            actionIter++) {
        actionIter->second->enqueueChildren(queue);
    }
}

Action BeliefNode::getNextActionToTry() {
    return nextActionToTry++;
}
