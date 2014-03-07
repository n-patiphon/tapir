#include "RockSampleAction.hpp"

#include <cstddef>                      // for size_t

#include <algorithm>                    // for copy
#include <iterator>                     // for ostream_iterator
#include <ostream>                      // for operator<<, ostream
#include <vector>                       // for vector, operator==, _Bit_const_iterator, _Bit_iterator_base, hash, vector<>::const_iterator

#include "global.hpp"
#include "problems/shared/GridPosition.hpp"  // for GridPosition, operator==, operator<<
#include "solver/abstract-problem/State.hpp"             // for State

namespace rocksample {
RockSampleAction::RockSampleAction(ActionType actionType, long rockNo) :
        actionType_(actionType),
        rockNo_(rockNo) {
}

RockSampleAction::RockSampleAction(long code) :
        actionType_(code <= 5 ? static_cast<ActionType>(code) : ActionType::CHECK),
        rockNo_(actionType_ == ActionType::CHECK ? code-5 : 0) {
}

std::unique_ptr<solver::Action> RockSampleAction::copy() const {
    return std::make_unique<RockSampleAction>(actionType_,rockNo_);
}

double RockSampleAction::distanceTo(solver::Action const &/*otherAction*/) const {
    return 0;
}

void RockSampleAction::print(std::ostream &os) const {
    if (actionType_ == ActionType::CHECK) {
        os << "CHECK-" << rockNo_;
        return;
    }
    switch (actionType_) {
    case ActionType::NORTH:
        os << "NORTH";
        break;
    case ActionType::EAST:
        os << "EAST";
        break;
    case ActionType::SOUTH:
        os << "SOUTH";
        break;
    case ActionType::WEST:
        os << "WEST";
        break;
    case ActionType::SAMPLE:
        os << "SAMPLE";
        break;
    default:
        os << "ERROR-" << static_cast<long>(actionType_);
        break;
    }
}

long RockSampleAction::getCode() const {
    long code = static_cast<long>(actionType_);
    if (actionType_ == ActionType::CHECK) {
        code += rockNo_;
    }
    return code;
}

ActionType RockSampleAction::getActionType() const {
    return actionType_;
}

long RockSampleAction::getRockNo() const {
    return rockNo_;
}
} /* namespace rocksample */
