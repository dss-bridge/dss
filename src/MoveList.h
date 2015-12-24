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
#include "WholeMove.h"

#define POOLSIZE 15000
#define POOLSIZE_AB 3000

#define LENTRICK 4096
#define LENRANK 65536
#define LENCASE 4096
#define LENRANKCASE 131072


class MoveList
{
  private:

    struct MoveNumberStruct
    {
      unsigned noAB;
      unsigned noP;
    };

    struct ListEntry
    {
      WholeMove * wholep;
      int no;
      ListEntry * next;
    };

    struct ListEntrySide
    {
      DefList * defp;
      int no;
      ListEntrySide * next;
    };

    struct SuitListEntry
    {
      WholeMove whole;
      int suitLengthExample;
      int counterExample;
    };

    struct SuitListEntrySide
    {
      DefList def;
      int suitLengthExample;
      int counterExample;
    };

    SuitListEntry list[POOLSIZE];
    SuitListEntrySide listSide[2][POOLSIZE]; // _AB

    ListEntry * index[ML_MAXKEY];
    ListEntrySide * indexSide[2][ML_MAXKEY];

    int indexCount[ML_MAXKEY];
    int indexCountSide[2][ML_MAXKEY];

    int moveCount[POOLSIZE];
    unsigned moveCountSide[2][POOLSIZE]; // _AB

    Hash hash;

    int numEntries;
    int numEntriesSide[2];
    int numEntriesNew;

    unsigned noToSideNumbers[POOLSIZE][2];

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
      WholeMove &whole,
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
      const unsigned no);

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
