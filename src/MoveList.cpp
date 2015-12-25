/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

#include <assert.h>

#include "cst.h"
#include "MoveList.h"
#include "misc.h"



MoveList::MoveList()
{
  for (int key = 0; key < ML_MAXKEY; key++)
  {
    indexSide[0][key] = nullptr;
    indexSide[1][key] = nullptr;

    indexCountSide[0][key] = 0;
    indexCountSide[1][key] = 0;
  }

  for (int i = 0; i < POOLSIZE; i++)
  {
    moveCountSide[0][i] = 0;
    moveCountSide[1][i] = 0;
    noToSideNumbers[i][0] = 0;
    noToSideNumbers[i][1] = 0;
  }

  numEntries = 1;
  numEntriesNew = 1;
  numEntriesSide[0] = 1;
  numEntriesSide[0] = 1;
}


MoveList::~MoveList()
{
}


unsigned MoveList::AddMoves(
  DefList& def, 
  const Holding& holding, 
  bool& newFlag)
{
  unsigned no = sideMerged.AddMoves(def, holding, newFlag);
  return no;
}


unsigned MoveList::AddMoves(
  WholeMove& whole,
  const Holding& holding, 
  bool& newFlag)
{
  DefList def = whole.GetCombinedMove();
  unsigned no = sideMerged.AddMoves(def, holding, newFlag);
  return no;
}



unsigned MoveList::AddSideMove(
  DefList& defSide,
  const Holding& holding, 
  const unsigned side,
  bool& newFlag)
{
assert(false);
  if (defSide.IsEmpty())
    return 0;

  Header& hp = defSide.GetHeader();
  unsigned key = hash.GetKey(hp);

  ListEntrySide * lp = indexSide[side][key];
  newFlag = false;

  if (lp == nullptr)
  {
    indexSide[side][key] = new ListEntrySide;
    lp = indexSide[side][key];
    newFlag = true;
  }
  else
  {
    while (lp)
    {
      if (defSide == *(lp->defp))
      {
        moveCountSide[side][lp->no]++;
        return lp->no;
      }

      if (! lp->next)
      {
        lp->next = new ListEntrySide;
        newFlag = true;
        lp = lp->next;
        break;
      }
      lp = lp->next;
    }
  }

  if (newFlag)
    indexCountSide[side][key]++;

  moveCountSide[side][numEntriesSide[side]]++;

  lp->no = numEntriesSide[side];
  listSide[side][numEntriesSide[side]].suitLengthExample = holding.GetSuitLength();
  listSide[side][numEntriesSide[side]].counterExample = holding.GetCounter();
  lp->defp = &listSide[side][numEntriesSide[side]++].def;
  *(lp->defp) = defSide;
  lp->next = nullptr;

  return lp->no;
}


unsigned MoveList::PairToNo(
  const unsigned noAB,
  const unsigned noP)
{
assert(false);
  string strAB = static_cast<ostringstream *>
    (&(ostringstream() << noAB))->str();
  string strP = static_cast<ostringstream *>
    (&(ostringstream() << noP))->str();
  string str = strAB + '.' + strP;

  map<string, unsigned>::iterator it = sideMap.find(str);
  if (it == sideMap.end())
    return 0;
  else
    return sideMap[str];
}


unsigned MoveList::SetPairNo(
  const unsigned noAB,
  const unsigned noP)
{
assert(false);
  // Concatenate to string, put in map
  // Also put in noToSideNumbers
  //

  string strAB = static_cast<ostringstream *>
    (&(ostringstream() << noAB))->str();
  string strP = static_cast<ostringstream *>
    (&(ostringstream() << noP))->str();
  string str = strAB + '.' + strP;

  unsigned no = numEntriesNew++;
  sideMap[str] = no;
  return no;
}


unsigned MoveList::AddMoves(
  DefList& defAB,
  DefList& defP,
  const Holding& holding, 
  bool& newFlag)
{
assert(false);
  bool newFlagAB = false;
  bool newFlagP = false;

  unsigned noAB = MoveList::AddSideMove(defAB, holding, 0, newFlagAB);
  unsigned noP = MoveList::AddSideMove(defAB, holding, 1, newFlagP);

  if (newFlagAB || newFlagP)
  {
    newFlag = true;
    return MoveList::SetPairNo(noAB, noP);
  }

  unsigned no = MoveList::PairToNo(noAB, noP);
  assert(no > 0);
  return no;
}


unsigned MoveList::GetMaxRank(
  const unsigned no)
{
  return sideMerged.GetMaxRank(no);
}


DefList& MoveList::GetCombinedMove(
  const unsigned no)
{
  return sideMerged.GetMove(no);
}


void MoveList::Print(
  const unsigned no) const
{
  sideMerged.Print(no);
}


void MoveList::PrintCaseCombos()
{
  sideMerged.PrintCaseCombos();
}


void MoveList::PrintCount() const
{
  sideMerged.PrintCount();
}


void MoveList::PrintMoveList(
  ostream& out)
{
  sideMerged.PrintMoveList(out);
}


void MoveList::PrintMoveListByKeys(
  ostream& fout)
{
  sideMerged.PrintMoveListByKeys(fout);
}


void MoveList::PrintMove(
  ostream& out,
  const int n)
{
  sideMerged.PrintMove(out, n);
}


void MoveList::PrintMoveStats(
  ostream& out) const
{
  sideMerged.PrintMoveStats(out);
}


void MoveList::PrintListStats(
  ostream& out) const
{
  sideMerged.PrintListStats(out);
}


void MoveList::PrintHashCounts() const
{
  sideMerged.PrintHashCounts();
}
