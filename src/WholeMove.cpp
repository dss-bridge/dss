/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iostream>
#include <iomanip>

using namespace std;

#include <assert.h>

#include "WholeMove.h"
#include "Header.h"



WholeMove::WholeMove()
{
  WholeMove::Reset();
}


WholeMove::~WholeMove()
{
}


void WholeMove::Reset()
{
  def1.Reset();
  def2.Reset();
  defMerged.Reset();
}


void WholeMove::Add(
  const DefList& res1)
{
  def1 = res1;
  def2.Reset();
  defMerged = res1;
}


void WholeMove::Add(
  DefList& res1,
  DefList& res2)
{
  def1 = res1;
  def2 = res2;

  if (defMerged.MergeSidesSoft(res1, res2))
  {
    def1 = defMerged;
    def2.Reset();
  }
  else
  {
    // unsigned d1, d2, dm, a;
    // def1.GetHeader().GetAD(d1, a);
    // def2.GetHeader().GetAD(d2, a);
    defMerged.MergeSidesHard(res1, res2);
    // defMerged.GetHeader().GetAD(dm, a);

    // if (d1*d2 != dm)
      // cout << "defMerged " << dm << " != " << d1 << " * " << d2 << "\n";
  }
}


bool WholeMove::operator == (
  const WholeMove& whole2)
{
  return ((def1 == whole2.def1 && def2 == whole2.def2));
}


bool WholeMove::operator != (
  const WholeMove& whole2)
{
  return ! (* this == whole2);
}


Header& WholeMove::GetHeader()
{
  return defMerged.GetHeader();
}


DefList& WholeMove::GetCombinedMove()
{
  return defMerged;
}


unsigned WholeMove::GetKeyNew() 
{
  Header& header = defMerged.GetHeader();
  return header.GetKeyNew();
}


unsigned WholeMove::GetTrickKey() 
{
  Header& header = defMerged.GetHeader();
  return header.GetTrickKey();
}


unsigned WholeMove::GetRankKey()
{
  Header& header = defMerged.GetHeader();
  return header.GetRankKey();
}


manualMoveType WholeMove::GetManualType(
  const unsigned maxDeclLen) 
{
  Header& header = defMerged.GetHeader();
  unsigned t = header.CheckManual();

  if (t == 0)
    return MANUAL_NONE;
  else if (t < maxDeclLen)
    return MANUAL_SHORT;
  else
    return MANUAL_ALL;
}


unsigned WholeMove::GetMaxRank()
{
  Header& header1 = def1.GetHeader();
  unsigned r1 = header1.GetMaxRank();
  if (def2.IsEmpty())
    return r1;
  else
  {
    Header& header2 = def2.GetHeader();
    unsigned r2 = header2.GetMaxRank();

    // In very rare cases this differs from the maximum rank of
    // defMerged:  AA1A + PA36 can lose to PA4T even though they
    // are from different sides.  And there can be two AltLists
    // that differ only in ranks; one later loses on ranks after
    // a merge.
    return Min(r1, r2);
  }
}


void WholeMove::Print(
  ostream& out) const
{
  defMerged.Print(out);
}
