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

#define MOVE_CHUNK_SIZE 1000


MoveList::MoveList()
{
  noToComponents.clear();

  noLen = MOVE_CHUNK_SIZE;
  noToComponents.resize(MOVE_CHUNK_SIZE);
  noCount = 1;
}


MoveList::~MoveList()
{
}


void MoveList::Extend()
{
  noLen += MOVE_CHUNK_SIZE;
  noToComponents.resize(noLen);
}


unsigned MoveList::Update(
  const MoveNumberStruct& mnos,
  const bool newFlag,
  const unsigned ret)
{
  unsigned r;
  if (newFlag)
  {
    if (noCount == noLen)
      MoveList::Extend();

    noToComponents[noCount] = mnos;
    MoveList::SetPairNo(mnos);
    r = noCount++;
  }
  else if (ret)
  {
    // Small optimization in order not to search twice sometimes.
    r = ret;
  }
  else
  {
    r = MoveList::PairToNo(mnos);
    assert(r > 0);
  }
  return r;
}


unsigned MoveList::AddMoves(
  DefList& def, 
  const Holding& holding, 
  bool& newFlag)
{
  unsigned no = sideComb.AddMoves(def, holding, newFlag);
  unsigned ret;

  MoveNumberStruct mnos;
  mnos.noComb = no;
  mnos.no1 = 0;
  mnos.no2 = 0;

  return MoveList::Update(mnos, newFlag);
}


unsigned MoveList::AddMoves(
  DefList& defAB,
  DefList& defP,
  const Holding& holding, 
  bool& newFlag)
{
  MoveNumberStruct mnos;
  DefList def;
  unsigned ret;

  if (def.MergeSidesSoft(defAB, defP))
    return MoveList::AddMoves(def, holding, newFlag);
  else
  {
    def.MergeSidesHard(defAB, defP);
    return MoveList::AddMoves(def, holding, newFlag);

    bool newFlag1 = false;
    bool newFlag2 = false;

    mnos.noComb = 0;
    mnos.no1 = sideList1.AddMoves(defAB, holding, newFlag1);
    mnos.no2 = sideList2.AddMoves(defP, holding, newFlag2);
   
    if (newFlag1 || newFlag2 || ! (ret = MoveList::PairToNo(mnos)))
      newFlag = true;
    else
      newFlag = false;

    return MoveList::Update(mnos, newFlag, ret);
  }

  /*
  unsigned d1, d2, dm, a;
  defAB.GetHeader().GetAD(d1, a);
  defP.GetHeader().GetAD(d2, a);
  def.GetHeader().GetAD(dm, a);
  if (d1*d2 != dm)
    cout << "defMerged " << dm << " != " << d1 << " * " << d2 << "\n";
  */
}


#define u2s(x) static_cast<ostringstream *>(&(ostringstream() << (x)))->str()

unsigned MoveList::PairToNo(
  const MoveNumberStruct& mnos)
{
  string str = u2s(mnos.noComb) + '.' + u2s(mnos.no1) + '.' + u2s(mnos.no2);
  map<string, unsigned>::iterator it = compMap.find(str);
  if (it == compMap.end())
    return 0;
  else
    return compMap[str];
}


void MoveList::SetPairNo(
  const MoveNumberStruct& mnos)
{
  string str = u2s(mnos.noComb) + '.' + u2s(mnos.no1) + '.' + u2s(mnos.no2);
  compMap[str] = noCount;
}


unsigned MoveList::GetMaxRank(
  const unsigned no)
{
  if (noToComponents[no].noComb)
    return sideComb.GetMaxRank(noToComponents[no].noComb);
  else
  {
    // Very rare differences from merged move rank.
    unsigned n1 = sideList1.GetMaxRank(noToComponents[no].no1);
    unsigned n2 = sideList2.GetMaxRank(noToComponents[no].no2);
    return Min(n1, n2);
  }
}


DefList MoveList::GetCombinedMove(
  const unsigned no)
{
  if (noToComponents[no].noComb)
    return sideComb.GetMove(noToComponents[no].noComb);
  else
  {
    DefList def1 = sideList1.GetMove(noToComponents[no].no1);
    DefList def2 = sideList2.GetMove(noToComponents[no].no2);
    DefList defMerged;
    defMerged.MergeSidesHard(def1, def2);
    return defMerged;
  }
}


void MoveList::Print(
  const unsigned no) const
{
  if (noToComponents[no].noComb)
    sideComb.Print(noToComponents[no].noComb);
  else
  {
    sideList1.Print(noToComponents[no].no1);
    sideList2.Print(noToComponents[no].no2);
  }
}


void MoveList::PrintMoveList(
  ostream& out)
{
  for (unsigned i = 1; i < noCount; i++)
  {
    if (noToComponents[i].noComb)
      sideComb.PrintMove(out, noToComponents[i].noComb);
    else
    {
      sideList1.PrintMove(out, noToComponents[i].no1);
      sideList2.PrintMove(out, noToComponents[i].no2);
    }
  }
}


void MoveList::PrintMoveListByKeys(
  ostream& fout)
{
  sideComb.PrintMoveListByKeys(fout);
}


void MoveList::PrintLists(
  ostream& out) const
{
  sideComb.PrintLists(out, "Combined");
  sideList1.PrintLists(out, "Side 1");
  sideList2.PrintLists(out, "Side 2");
}


void MoveList::PrintStats() const
{
  sideComb.PrintStats("Combined");
  sideList1.PrintStats("Side 1");
  sideList2.PrintStats("Side 2");
}
