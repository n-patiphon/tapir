#ifndef ROCKSAMPLE_TEXTSERIALIZER_HPP_
#define ROCKSAMPLE_TEXTSERIALIZER_HPP_

#include <iosfwd>                       // for istream, ostream
#include <memory>                       // for unique_ptr

#include "solver/abstract-problem/Action.hpp"
#include "solver/abstract-problem/State.hpp"
#include "solver/abstract-problem/Observation.hpp"

#include "solver/mappings/actions/enumerated_actions.hpp"
#include "solver/mappings/observations/enumerated_observations.hpp"

#include "solver/serialization/TextSerializer.hpp"    // for TextSerializer

#include "position_history.hpp"
#include "smart_history.hpp"
#include "LegalActionsPool.hpp"
#include "global.hpp"

namespace solver {
class Solver;
} /* namespace solver */

namespace rocksample {
class RockSampleTextSerializer: virtual public solver::TextSerializer,
    virtual public solver::EnumeratedObservationTextSerializer {
public:
    RockSampleTextSerializer() = default;
    RockSampleTextSerializer(solver::Solver *solver);
    virtual ~RockSampleTextSerializer() = default;
    _NO_COPY_OR_MOVE(RockSampleTextSerializer);

    /* ------------------ Saving change sequences -------------------- */
    virtual void saveModelChange(solver::ModelChange const &change, std::ostream &os) override;
    virtual std::unique_ptr<solver::ModelChange> loadModelChange(std::istream &is) override;

    void saveState(solver::State const *state, std::ostream &os) override;
    std::unique_ptr<solver::State> loadState(std::istream &is) override;

    void saveObservation(solver::Observation const *obs, std::ostream &os) override;
    std::unique_ptr<solver::Observation> loadObservation(std::istream &is) override;

    void saveAction(solver::Action const *action, std::ostream &os) override;
    std::unique_ptr<solver::Action> loadAction(std::istream &is) override;

    virtual int getActionColumnWidth() override;
    virtual int getTPColumnWidth() override;
    virtual int getObservationColumnWidth() override;
};

class RockSampleBasicTextSerializer: public RockSampleTextSerializer,
    public solver::DiscretizedActionTextSerializer {
public:
    RockSampleBasicTextSerializer(solver::Solver *solver);
    virtual ~RockSampleBasicTextSerializer() = default;
};

class RockSampleLegalActionsTextSerializer: public RockSampleTextSerializer,
        public LegalActionsPoolTextSerializer,
        public PositionDataTextSerializer {
public:
    RockSampleLegalActionsTextSerializer(solver::Solver *solver);
    virtual ~RockSampleLegalActionsTextSerializer() = default;
};

class RockSamplePreferredActionsTextSerializer: public RockSampleTextSerializer,
        public solver::DiscretizedActionTextSerializer,
        public PositionAndRockDataTextSerializer {
public:
    RockSamplePreferredActionsTextSerializer(solver::Solver *solver);
    virtual ~RockSamplePreferredActionsTextSerializer() = default;
};

} /* namespace rocksample */

#endif /* ROCKSAMPLE_TEXTSERIALIZER_HPP_ */
