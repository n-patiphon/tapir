#ifndef UnderwaterNavModifModel_H
#define UnderwaterNavModifModel_H

#include <ostream>
#include <vector>
#include <map>
#include <string>

#include "Model.h"
#include "GlobalResources.h"
#include "StRoadmap.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

class UnderwaterNavModifModel : public Model {
	public:
		UnderwaterNavModifModel(po::variables_map vm);
		~UnderwaterNavModifModel();

		/***** Start implementation of Model's virtual functions *****/
		void sampleAnInitState(StateVals& tmpStVals);
		bool isTerm(StateVals &sVals);
		void solveHeuristic(StateVals &s, double *qVal);
		double getDefaultVal();

		bool getNextState(StateVals &currStVals, long actIdx,
		        double *immediateRew, StateVals &nxtSVals, ObsVals &obs);
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

		// Additional initialisation.
		void setInitObsGoal();

	private:
        /** The number of state particles in the initial belief. */
        long nInitBel;
        /** A vector of all the states in the initial belief. */
        std::vector<StateVals> initBel;

		enum { EAST=0, NORTH=1, SOUTH=2, NORTHEAST=3, SOUTHEAST=4 };

		long nX, nY, nGoals, nRocks;
		double goalReward, crashPenalty, moveCost, moveDiagCost;
		double ctrlCorrectProb, ctrlErrProb1;
		double rolloutExploreTh;
		std::vector<std::string> envMap;
		std::vector<StateVals> goals;
		std::vector<StateVals> rocks;
		std::vector<StateVals> allObservations;
		std::map< long, std::map<long, short> > cellType;
		// 0: usual
		// 1: goals
		// 2: rocks
		// 3: observation
		// 4: spc. reward
		// 5: obstacle.
		short nSpcRew;
		std::vector<double> spcRew;
		std::map< long, std::vector<std::string> > changes;
		std::vector<StateVals> obstacleRegion;

		StRoadmap *roadmap;
		long nTryCon, maxDistCon, nVerts;

		//double getExpDist(StateVals &s, long firstAct);
		double getDist(StateVals &s1, StateVals &s2);
		void getNextState(StateVals &s, long actId, StateVals &sp);
		void inObsRegion(StateVals &st, ObsVals &obs);
		double getDistToNearestGoal(StateVals &st);
		double getDistToNearestObs(StateVals &st, StateVals &nxtSt);
		bool inGoal(StateVals &st);
		bool inRock(StateVals &st);
		void getReachableSt(StateVals &s, long actId,
		        std::vector<StateVals> &nxtS);
		std::vector<StateVals>::iterator getIterator(
		        std::vector<StateVals> &vecStVals, long x, long y);

		int findCollision(StateVals &s);

};

#endif
