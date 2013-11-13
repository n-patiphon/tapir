#ifndef MODEL_H
#define MODEL_H

#include <ostream>
#include <vector>
#include <map>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

typedef std::vector<double> StateVals;
typedef std::vector<double> ObsVals;
enum ChType {
    UNDEFINED=0,
    ADDSTATE=1,
    REWARD=2,
    TRANSITION=3,
    DELSTATE=4,
    ADDOBSERVATION=5,
    ADDOBSTACLE=6
};

class Model {
	public:
		Model(po::variables_map vm) {
		    nParticles = vm["SBT.nParticles"].as<long>();
		    maxTrials = vm["SBT.maxTrials"].as<long>();
		    maxDistTry = vm["SBT.maxDistTry"].as<long>();

		    exploreCoef = vm["SBT.exploreCoef"].as<double>();
		    depthTh = vm["SBT.depthTh"].as<double>();
		    distTh = vm["SBT.distTh"].as<double>();

		    discount = vm["problem.discount"].as<double>();
        }

        // Note: Subclasses must initially calculate the following parameters:
        // nActions, nObservations, nStVars,
        // minVal, maxVal

        // Problem parameters
        /** Returns the POMDP discount factor. */
		inline double getDiscount() { return discount; }
		/** Returns the # of actions for this POMDP. */
		inline long getNActions() { return nActions; }
		/** Returns the # of observations for this POMDP. */
		inline long getNObservations() { return nObservations; }
		/** Returns the number of state variables for this PODMP. */
		inline long getNStVars() { return nStVars; }

        // SBT parameters
        /** Returns the maximum number of particles */
		inline long getNParticles() { return nParticles; }
        /** Returns the maximum number of trials to run. */
		inline long getMaxTrials() { return maxTrials; }
		/** Returns the maximum depth. */
		inline double getDepthTh() { return depthTh; }
		/** Returns the exploration coefficient used for rollouts.
		 * ??
		 */
		inline double getExploreCoef() { return exploreCoef; }

		/** Returns a lower bound on the q-value. */
		inline double getMinVal() { return minVal; }
		/** Returns an upper bound on the q-value. */
		inline double getMaxVal() { return maxVal; }

        /** Returns the maximum number of nodes to check when searching
         * for a nearest-neighbour belief node.
         */
		inline long getMaxDistTry() { return maxDistTry; }
		/** Returns the smallest allowable distance when searching for
		 * a nearest-neighbour belief node.
		 */
		inline double getDistTh() { return distTh; }


		/* --------------- Start virtual functions ----------------- */

		/** Samples an initial state from the belief vector. */
		virtual void sampleAnInitState(StateVals& tmpStVals)=0;
		/** Returns true iff the given state is terminal. */
		virtual bool isTerm(StateVals &sVals)=0;
		/** Approximates the q-value of a state */
		virtual void solveHeuristic(StateVals &sVals, double *qVal)=0;
		/** Returns the default reward */
		virtual double getDefaultVal()=0;

		/** Generates the next state, an observation, and the reward. */
		virtual bool getNextState(StateVals &sVals, long actId,
		        double *immediateRew, StateVals &nxtSVals, ObsVals &obs)=0;
		/** Returns the reward for the given state. */
		virtual double getReward(StateVals &sVals)=0;
		/** Returns the reward for the given state and action. */
		virtual double getReward(StateVals &sVals, long actId)=0;

        /** Creates a new belief node based on the state particles of the
         * previous node, and 
		virtual void getStatesSeeObs(long actId, ObsVals &obs,
		        std::vector<StateVals> &partSt,
		        std::map<int, StateVals> &partNxtSt)=0;
		virtual void getStatesSeeObs(ObsVals &obs,
		        std::vector<StateVals> &posNxtSt)=0;

		virtual void setChanges(const char* chName,
		        std::vector<long> &chTime)=0;
		virtual void update(long tCh, std::vector<StateVals> &affectedRange,
		        std::vector<ChType> &typeOfChanges)=0;
		virtual bool modifStSeq(std::vector<StateVals> &seqStVals,
		        long startAffectedIdx, long endAffectedIdx,
				std::vector<StateVals> &modifStSeq,
				std::vector<long> &modifActSeq,
				std::vector<ObsVals> &modifObsSeq,
				std::vector<double> &modifRewSeq)=0;

		virtual void drawEnv(std::ostream &os)=0;

    protected:
        // Problem parameters.
        double discount;
        long nActions;
        long nObservations;
        long nStVars;

        // SBT parameters
        long nParticles;
        long maxTrials;
        double depthTh;
        double exploreCoef;
        double minVal;
        double maxVal;

        long maxDistTry;
        double distTh;
};

#endif
