#include "BeliefNode.hpp"

#include <map>                          // for _Rb_tree_iterator, map<>::iterator, map
#include <memory>                       // for unique_ptr
#include <random>                       // for uniform_int_distribution
#include <set>
#include <tuple>                        // for tie, tuple
#include <utility>                      // for pair, make_pair, move

#include "global.hpp"                     // for RandomGenerator, make_unique

#include "abstract-problem/Action.hpp"                   // for Action
#include "abstract-problem/Observation.hpp"              // for Observation
#include "abstract-problem/State.hpp"                    // for State

#include "mappings/ActionMapping.hpp"

#include "ActionNode.hpp"               // for ActionNode
#include "HistoryEntry.hpp"             // for HistoryEntry

#include "RandomAccessSet.hpp"

namespace solver {
BeliefNode::BeliefNode(long id) :
    BeliefNode(nullptr, id) {
}

BeliefNode::BeliefNode(std::unique_ptr<ActionMapping> actionMap) :
        BeliefNode(std::move(actionMap), -1) {
}

BeliefNode::BeliefNode(std::unique_ptr<ActionMapping> actionMap, long id) :
    id_(id),
    numberOfSequenceEdges_(0),
    particles_(),
    tLastChange_(-1),
    actionMap_(std::move(actionMap)) {
}

// Do-nothing destructor
BeliefNode::~BeliefNode() {
}

/* -------------- Particle management / sampling ---------------- */
void BeliefNode::addParticle(HistoryEntry *newHistEntry) {
    tLastChange_ = abt::clock_ms();
    particles_.add(newHistEntry);
}

void BeliefNode::removeParticle(HistoryEntry *histEntry) {
    tLastChange_ = abt::clock_ms();
    particles_.remove(histEntry);
}

HistoryEntry *BeliefNode::sampleAParticle(RandomGenerator *randGen) const {
    long index = std::uniform_int_distribution<long>(
                                 0, getNumberOfParticles() - 1)(*randGen);
    return particles_.get(index);
}

/* ----------------- Q-value update methods. ------------------- */
void BeliefNode::recalculateQValue() {
    actionMap_->update();
}

/* ----------------- Useful calculations ------------------- */
double BeliefNode::distL1Independent(BeliefNode *b) const {
    double dist = 0.0;
    for (HistoryEntry *entry1 : particles_) {
        for (HistoryEntry *entry2 : b->particles_) {
            dist += entry1->getState()->distanceTo(*entry2->getState());
        }
    }
    double averageDist = dist / (getNumberOfParticles() * b->getNumberOfParticles());
    if (averageDist < 0) {
        debug::show_message("ERROR: Distance < 0 between beliefs.");
    } else if (averageDist == 0) {
        debug::show_message("NOTE: Identical belief nodes found!");
    }
    return averageDist;
}

/* -------------------- Simple setters ---------------------- */
void BeliefNode::setId(long id) {
    id_ = id;
}

/* -------------------- Simple getters ---------------------- */
long BeliefNode::getId() const {
    return id_;
}
std::unique_ptr<Action> BeliefNode::getBestAction() const {
    return actionMap_->getBestAction();
}
double BeliefNode::getQValue() const {
    return actionMap_->getBestMeanQValue();
}
long BeliefNode::getNumberOfParticles() const {
    return particles_.size();
}
long BeliefNode::getNumberOfSequenceEdges() const {
    return numberOfSequenceEdges_;
}
std::vector<State const *> BeliefNode::getStates() const {
    std::vector<State const *> states;
    for (HistoryEntry *entry : particles_) {
        states.push_back(entry->getState());
    }
    return states;
}
double BeliefNode::getTimeOfLastChange() const {
    return tLastChange_;
}

/* -------------------- Tree-related methods  ---------------------- */
ActionMapping *BeliefNode::getMapping() {
    return actionMap_.get();
}

BeliefNode *BeliefNode::getChild(Action const &action, Observation const &obs) const {
    ActionNode *node = actionMap_->getActionNode(action);
    if (node == nullptr) {
        return nullptr;
    }
    return node->getChild(obs);
}

std::pair<BeliefNode *, bool> BeliefNode::createOrGetChild(Action const &action,
        Observation const &obs) {
    ActionNode *actionNode = actionMap_->getActionNode(action);
    if (actionNode == nullptr) {
        actionNode = actionMap_->createActionNode(action);
    }
    return actionNode->createOrGetChild(obs);
}
} /* namespace solver */
