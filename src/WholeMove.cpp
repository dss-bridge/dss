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
  if (def1.MergeSidesSoft(res1, res2))
  {
    def2.Reset();
    defMerged = def1;
  }
  else
  {
    def2 = res2;
    defMerged.MergeSidesHard(res1, res2);
  }
}


bool WholeMove::operator == (
  const DefList& def)
{
assert(false);
  if (! def2.IsEmpty())
    return false;
  else
    return (def1 == def);
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


unsigned WholeMove::GetMaxRank()
{
  Header& header = defMerged.GetHeader();
  return header.GetMaxRank();
}


void WholeMove::Print(
  ostream& out) const
{
  defMerged.Print(out);
}
