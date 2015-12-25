/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_MOVELIST_H
#define SDS_MOVELIST_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Holding.h"
#include "DefList.h"
#include "SideMoveList.h"


class MoveList
{
  private:

    struct MoveNumberStruct
    {
      unsigned noComb;
      unsigned no1;
      unsigned no2;
    };

    SideMoveList sideComb;
    SideMoveList sideList1;
    SideMoveList sideList2;

    std::vector<MoveNumberStruct> noToComponents;
    unsigned noLen;
    unsigned noCount;

    std::map<std::string, unsigned> compMap;

    unsigned Update(
      const MoveNumberStruct& mnos,
      const bool newFlag,
      const unsigned ret = 0);

    unsigned PairToNo(
      const MoveNumberStruct& mnos);

    void SetPairNo(
      const MoveNumberStruct& mnos);

    void Extend();

  public:

    MoveList();

    ~MoveList();

    unsigned AddMoves(
      DefList& def, 
      const Holding& holding,
      bool& newFlag);

    unsigned AddMoves(
      DefList& def1, 
      DefList& def2, 
      const Holding& holding,
      bool& newFlag);

    unsigned GetMaxRank(
      const unsigned no);

    DefList GetCombinedMove(
      const unsigned no);

    void Print(
      const unsigned no) const;

    void PrintMoveList(
      std::ostream& out = std::cout);

    void PrintMoveListByKeys(
      std::ostream& out = std::cout);

    void PrintStats() const;
    
    void PrintLists(
      std::ostream& out = std::cout) const;
};

#endif
