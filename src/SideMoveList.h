/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_SIDEMOVELIST_H
#define SDS_SIDEMOVELIST_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "cst.h"
#include "Holding.h"
#include "DefList.h"
#include "Hash.h"

#define SIDEMOVE_CHUNK_SIZE 1000


class SideMoveList
{
  private:

    struct ListEntry
    {
      int no;
      ListEntry * next;
    };

    struct SuitListEntrySide
    {
      DefList def;
      int suitLengthExample;
      int counterExample;
    };

    std::vector<SuitListEntrySide> list;
    std::vector<unsigned> moveCount;
    unsigned listLength;

    ListEntry * index[ML_MAXKEY];

    int indexCount[ML_MAXKEY];

    Hash hash;

    int numEntries;

    unsigned histD[SDS_MAX_DEF];
    unsigned histAsum[SDS_MAX_DEF * SDS_MAX_ALT];

    void PrintMoveStats(
      std::ostream& out = std::cout) const;

    void PrintHashStats(
      std::ostream& out = std::cout) const;
    
    void PrintList(
      const unsigned hist[],
      const unsigned l,
      const char text[]) const;

  public:

    SideMoveList();

    ~SideMoveList();

    unsigned AddMoves(
      DefList& def, 
      const Holding& holding,
      bool& newFlag);

    unsigned GetMaxRank(
      const unsigned no);

    DefList& GetMove(
      const unsigned no);

    void Print(
      const unsigned no) const;

    void PrintMove(
      std::ostream& out,
      const int no);

    void PrintMoveList(
      std::ostream& out = std::cout);

    void PrintMoveListByKeys(
      std::ostream& out = std::cout);

    void PrintLists(
      std::ostream& out,
      const std::string text) const;

    void PrintStats(
      const std::string text) const;
};

#endif
