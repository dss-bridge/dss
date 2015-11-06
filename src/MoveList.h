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

#define POOLSIZE 19200
#define ML_TRICKS 14
#define ML_MAXKEY (1 << 20)

#define LENTRICK 4096
#define LENRANK 65536
#define LENCASE 4096
#define LENRANKCASE 131072


class MoveList
{
  private:

    struct ListEntry
    {
      DefList * defp;
      int no;
      ListEntry * next;
    };

    struct SuitListEntry
    {
      DefList def;
      int suitLengthExample;
      int counterExample;
    };

    SuitListEntry list[POOLSIZE];

    ListEntry * index[ML_MAXKEY];

    int indexCount[ML_MAXKEY];

    int moveCount[POOLSIZE];

    int trickKeyMap[LENTRICK];
    int rankKeyMap[LENRANK];
    int caseKeyMap[LENCASE];
    int rankCaseKeyMap[LENRANKCASE];
    int keyMapNo;
    int rankMapNo;
    int caseMapNo;
    int rankCaseMapNo;


    int numEntries;

    void PrintMove(
      std::ostream& fout,
      const int n);

  public:

    MoveList();

    ~MoveList();

    DefList * AddMoves(
      DefList& sm, 
      const Holding& holding,
      bool& newFlag);

    void CountTrickCombos();
    void CountRankCombos();
    void CountCaseCombos();

    void PrintCount();

    void PrintMoveList();
    void PrintMoveList(
      std::ostream& fout);

    void PrintMoveListByKeys();
    void PrintMoveListByKeys(
      std::ostream& fout);

    void PrintMoveStats();
    void PrintMoveStats(
      std::ostream& fout);

    void PrintListStats();
    void PrintListStats(
      std::ostream& fout);
};

#endif
