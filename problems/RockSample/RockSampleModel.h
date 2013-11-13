#ifndef RockSampleModel_H
#define RockSampleModel_H

#include <ostream>
#include <vector>
#include <map>
#include <string>

#include <cstdlib>

#include "Model.h"
#include "GlobalResources.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

struct Coords {
    long i;
    long j;
    Coords() : i(0), j(0) {}
    Coords(long i, long j) : i(i), j(j) {}

    double distance(Coords other) {
        return std::abs(i - other.i) + std::abs(j - other.j);
    }
};

class RockSampleModel : public Model {
	public:
		RockSampleModel(po::variables_map vm);
		~RockSampleModel();

		/***** Start implementation of Model's virtual functions *****/

		void sampleAnInitState(StateVals& tmpStVals);
		bool isTerm(StateVals &sVals);
		void solveHeuristic(StateVals &s, double *qVal);
		double getDefaultVal();

		bool getNextState(StateVals &sVals, long actIdx, StateVals &nxtSVals,
		        ObsVals &obs);
		bool getNextState(StateVals &sVals, long actIdx,
		        double *immediateRew, StateVals &nxtSVals, ObsVals &obs);
		double getNextStateNRew(StateVals &sVals, long actId, ObsVals &obs,
		        bool &isTerm);
		double getReward(StateVals &sVals);
		double getReward(StateVals &sVals, long actId);

		void getStatesSeeObs(long actId, ObsVals &obs,
		        std::vector<StateVals> &partSt,
		        std::map<int, StateVals> &partNxtSt);
		void getStatesSeeObs(ObsVals &obs, std::vector<StateVals> &posNxtSt);

		void setChanges(const char* chName, std::vector<long> &chTime);
		void update(long tCh, std::vector<StateVals> &affectedRange,
		        std::vector<ChType> &typeOfChanges);
		bool modifStSeq(std::vector<StateVals> &seqStVals,
		        long startAffectedIdx, long endAffectedIdx,
				std::vector<StateVals> &modifStSeq,
				std::vector<long> &modifActSeq,
				std::vector<ObsVals> &modifObsSeq,
				std::vector<double> &modifRewSeq);

		void drawEnv(std::ostream &os);

    private:
        /** The number of state particles in the initial belief. */
        long nInitBel;
        /** A vector of all the states in the initial belief. */
        std::vector<StateVals> initBel;

        /**
        * Finds and counts the rocks on the map, and initialisese the required
        * data structures and variables.
        */
	    void initialise();

        /**
         * Generates a next state for the given state and action;
         * returns true if the action was legal, and false if it was illegal.
         */
        bool makeNextState(StateVals &sVals, long actId, StateVals &nxtSVals);
        /** Generates an observation given a current state and action. */
        RockSampleObservation makeObs(StateVals &sVals, long actId);

	    /**
	     * Enumerates the possible actions. Note that there are actually
	     * multiple check actions; Check-i is represented by CHECK+i,
	     * where i is the rock number from 0..k-1 and k is the number
	     * of rocks.
	     */
	    enum RockSampleAction {
	        NORTH=0,
	        EAST=1,
	        SOUTH=2,
	        WEST=3,
	        SAMPLE=4,
	        CHECK=5
        };

        /**
         * There are only two possible observations - the rock
         * is either good or bad. Note that observations are
         * only meaningful when the action taken was CHECK;
         * they are meaningless otherwise.
         */
        enum RockSampleObservation {
            NONE = 0,
            BAD = 1,
            GOOD = 2
        };

        /**
         * Rocks are enumerated 0, 1, 2, ... ;
         * other cell types should be negative.
         */
        enum CellType {
            ROCK = 0,
            EMPTY = -1,
            GOAL = -2,
        };

        /** The number of rows in the map. */
        long nRows;
        /** The number of columns in the map. */
        long nCols;
        /** The number of rocks on the map. */
        long nRocks;
        /** The starting position. */
        Coords startPos;
        /** The coordinates of the rocks. */
        std::vector<Coords> rockCoords;

        /** The reward for sampling a good rock. */
        double goodRockReward;
        /** The penalty for sampling a bad rock. */
        double badRockPenalty;
        /** The reward for exiting the map. */
        double exitReward;
        /** The penalty for an illegal move. */
        double illegalMovePenalty;

        /** The half efficiency distance d0 */
        double halfEfficiencyDistance;

        /** The environment map in text form. */
        std::vector<std::string> mapText;

        /** The environment map in vector form. */
        std::vector<std::vector<int>> envMap;
};

#endif
