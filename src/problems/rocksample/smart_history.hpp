#ifndef ROCKSAMPLE_SMART_HISTORY_HPP_
#define ROCKSAMPLE_SMART_HISTORY_HPP_

#include "solver/abstract-problem/HistoricalData.hpp"

#include "solver/serialization/TextSerializer.hpp"

#include "problems/shared/GridPosition.hpp"

namespace rocksample {
class RockSampleAction;
class RockSampleModel;

struct RockData {
    long checkCount = 0;
    long goodnessCount = 0;

    double chanceGood = 0.5;
};

class PositionAndRockData : public solver::HistoricalData {
    friend class PreferredActionsMap;
    friend class PositionAndRockDataTextSerializer;
public:
    PositionAndRockData(RockSampleModel *model, GridPosition position);
    virtual ~PositionAndRockData() = default;

    // We allow copying via copy constructor only.
    PositionAndRockData(PositionAndRockData const &other);
    PositionAndRockData(PositionAndRockData &&other) = delete;
    PositionAndRockData &operator=(PositionAndRockData const &other) = delete;
    PositionAndRockData &operator=(PositionAndRockData &&other) = delete;

    std::unique_ptr<solver::HistoricalData> createChild(
            solver::Action const &action,
            solver::Observation const &observation) const override;

    std::vector<long> generateLegalActions() const;
    std::vector<long> generatePreferredActions() const;

    void print(std::ostream &os) const override;

private:
    RockSampleModel *model_;
    GridPosition position_;
    std::vector<RockData> allRockData_;
};

class PositionAndRockDataTextSerializer : virtual public solver::TextSerializer {
public:
    void saveHistoricalData(solver::HistoricalData const *data, std::ostream &os) override;
    std::unique_ptr<solver::HistoricalData> loadHistoricalData(std::istream &is) override;
};
} /* namespace rocksample */

#endif /* ROCKSAMPLE_SMART_HISTORY_HPP_ */
