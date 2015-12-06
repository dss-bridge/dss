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
}


void WholeMove::Add(
  const DefList& res1)
{
  def1 = res1;
  def2.Reset();
}


void WholeMove::Add(
  DefList& res1,
  DefList& res2)
{
  def1.MergeSides(res1, res2);
  def2.Reset();
}


bool WholeMove::operator == (
  const DefList& def)
{
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
  return def1.GetHeader();
}


DefList& WholeMove::GetCombinedMove()
{
  // Later on this will have to call MergeSides, forcing a merge.
  return def1;
}


unsigned WholeMove::GetKeyNew() 
{
  Header& header = def1.GetHeader();
  return header.GetKeyNew();
}


unsigned WholeMove::GetTrickKey() 
{
  Header& header = def1.GetHeader();
  return header.GetTrickKey();
}


unsigned WholeMove::GetRankKey()
{
  Header& header = def1.GetHeader();
  return header.GetRankKey();
}


unsigned WholeMove::GetMaxRank()
{
  Header& header = def1.GetHeader();
  return header.GetMaxRank();
}


void WholeMove::Print(
  ostream& out) const
{
  def1.Print(out);
  // def2.Print(out);
}
