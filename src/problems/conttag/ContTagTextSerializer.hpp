#pragma once


#include <iosfwd>                       // for istream, ostream
#include <memory>                       // for unique_ptr

#include "global.hpp"

#include "solver/abstract-problem/Action.hpp"
#include "solver/abstract-problem/ModelChange.hpp"
#include "solver/abstract-problem/Observation.hpp"
#include "solver/abstract-problem/State.hpp"

#include "solver/mappings/actions/continuous_actions.hpp"
#include "solver/mappings/observations/discrete_observations.hpp"

#include "solver/serialization/TextSerializer.hpp"    // for TextSerializer


namespace solver {
class Solver;
}  /* namespace solver */

namespace conttag {

/** A simple method to serialize a vector of longs to an output stream. */
void saveVector(std::vector<long> values, std::ostream &os);
/** A simple method to de-serialize a vector of longs from an input stream. */
std::vector<long> loadVector(std::istream &is);

/** A serialization class for the Tag problem.
 *
 * This contains serialization methods for TagChange, TagState, TagAction, and TagObservation;
 * this class also inherits from solver::EnumeratedActionTextSerializer in order to serialize
 * the action mappings, and from solver::DiscreteObservationTextSerializer in order to serialize
 * the observation mappings.
 */
class ContTagTextSerializer: virtual public solver::TextSerializer, virtual solver::ContinuousActionTextSerializer, virtual solver::DiscreteObservationTextSerializer {
  public:
    /** Creates a new TagTextSerializer instance, associated with the given solver. */
	ContTagTextSerializer(solver::Solver *solver);

    virtual ~ContTagTextSerializer() = default;
    _NO_COPY_OR_MOVE(ContTagTextSerializer);


    virtual void save(std::ostream &os) override;

    virtual void load(std::istream &is) override;


    void saveState(solver::State const *state, std::ostream &os) override;
    std::unique_ptr<solver::State> loadState(std::istream &is) override;

    void saveObservation(solver::Observation const *obs,
            std::ostream &os) override;
    std::unique_ptr<solver::Observation> loadObservation(
            std::istream &is) override;

    void saveAction(solver::Action const *action, std::ostream &os) override;
    std::unique_ptr<solver::Action> loadAction(std::istream &is) override;

	/** Saves the construction data to the output stream */
	virtual void saveConstructionData(const ThisActionConstructionDataBase*, std::ostream& os) override;
	/** Loads the construction data from the input stream */
	virtual std::unique_ptr<ThisActionConstructionDataBase> loadConstructionData(std::istream& is) override;


    virtual int getActionColumnWidth() override;
    virtual int getTPColumnWidth() override;
    virtual int getObservationColumnWidth() override;
};
} /* namespace conttag */

