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

#include "cst.h"
#include "Holding.h"
#include "DefList.h"
#include "Hash.h"
#include "WholeMove.h"

#define POOLSIZE 16000

#define LENTRICK 4096
#define LENRANK 65536
#define LENCASE 4096
#define LENRANKCASE 131072


class MoveList
{
  private:

    struct ListEntry
    {
      WholeMove * wholep;
      int no;
      ListEntry * next;
    };

    struct SuitListEntry
    {
      WholeMove whole;
      int suitLengthExample;
      int counterExample;
    };

    SuitListEntry list[POOLSIZE];

    ListEntry * index[ML_MAXKEY];

    int indexCount[ML_MAXKEY];

    int moveCount[POOLSIZE];

    Hash hash;

    int numEntries;

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
      WholeMove &whole,
      const Holding& holding,
      bool& newFlag);

    unsigned GetMaxRank(
      const unsigned no);

    DefList& GetCombinedMove(
      const unsigned no);

    void Print(
      const unsigned no);

    void CountTrickCombos();
    void CountRankCombos();
    void CountCaseCombos();

    void PrintCount();

    void PrintMoveList(
      std::ostream& out = std::cout);

    void PrintMoveListByKeys(
      std::ostream& out = std::cout);

    void PrintMoveStats(
      std::ostream& out = std::cout);

    void PrintListStats(
      std::ostream& out = std::cout);
    
    void PrintHashCounts() const;
};

#endif
