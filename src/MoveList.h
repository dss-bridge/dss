/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_MOVELIST_H
#define SDS_MOVELIST_H

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include "cst.h"
#include "Holding.h"
#include "DefList.h"
#include "Hash.h"
#include "SideMoveList.h"

#define POOLSIZE_AB 3000


class MoveList
{
  private:

    struct MoveNumberStruct
    {
      unsigned noAB;
      unsigned noP;
    };

    struct ListEntrySide
    {
      DefList * defp;
      int no;
      ListEntrySide * next;
    };

    struct SuitListEntrySide
    {
      DefList def;
      int suitLengthExample;
      int counterExample;
    };

    SideMoveList sideMerged;

    SuitListEntrySide listSide[2][POOLSIZE_AB]; // _AB

    ListEntrySide * indexSide[2][ML_MAXKEY];

    int indexCountSide[2][ML_MAXKEY];

    unsigned moveCountSide[2][POOLSIZE_AB]; // _AB

    Hash hash;

    int numEntries;
    int numEntriesSide[2];
    int numEntriesNew;

    unsigned noToSideNumbers[POOLSIZE_AB][2];

    std::map<std::string, unsigned> sideMap;

    unsigned PairToNo(
      const unsigned noAB,
      const unsigned noP);

    unsigned SetPairNo(
      const unsigned noAB,
      const unsigned noP);

    unsigned AddSideMove(
      DefList& defSide,
      const Holding& holding,
      const unsigned side,
      bool& newFlag);
      
    void PrintMove(
      std::ostream& out,
      const int n);

  public:

    MoveList();

    ~MoveList();

    unsigned AddMoves(
      DefList& sm, 
      const Holding& holding,
      bool& newFlag);

    unsigned AddMoves(
      DefList& defAB, 
      DefList& defP, 
      const Holding& holding,
      bool& newFlag);

    unsigned GetMaxRank(
      const unsigned no);

    DefList& GetCombinedMove(
      const unsigned no);

    void Print(
      const unsigned no) const;

    void PrintCaseCombos();

    void PrintCount() const;

    void PrintMoveList(
      std::ostream& out = std::cout);

    void PrintMoveListByKeys(
      std::ostream& out = std::cout);

    void PrintMoveStats(
      std::ostream& out = std::cout) const;

    void PrintListStats(
      std::ostream& out = std::cout) const;
    
    void PrintHashCounts() const;
};

#endif
