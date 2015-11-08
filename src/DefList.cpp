/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <assert.h>

#include "cst.h"
#include "DefList.h"

extern bool debugDefList;
extern unsigned highestDefNo;
extern vector<unsigned> holdCtr;


DefList::DefList()
{
  DefList::Reset();
}


DefList::~DefList()
{
}


void DefList::Reset()
{
  headerDirty = true;
  len = 0;
}


void DefList::SetMaxOppRank(
  const int r)
{
  /*
     This is necessary in order to know how to fix the ranks later 
     in certain cases.  It happens for instance when declarer leads
     the J from JT from a finesse, and the defender does not cover,
     even though he could.  The trick is won with the J, but the
     defender would cover if declarer didn't have the T.  This comes
     out of an optimization, probably evil, where declarer only leads
     one of the cards in this program, and then we figure out 
     after the trick which card it was...
  */

  // If FixRanks goes, then this might go, too.

  maxOppRank = r;
}


bool DefList::Set1(
  const Trick& trick)
{
  headerDirty = true;
  len = 1;
  return list[0].Set1(trick);
}


bool DefList::Set2(
  const Trick& trick10,
  const Trick& trick11)
{
  headerDirty = true;
  len = 1;
  return list[0].Set2(trick10, trick11);
}


bool DefList::Set11(
  const Trick& trick1,
  const Trick& trick2)
{
  headerDirty = true;
  len = 1;
  return list[0].Set11(trick1, trick2);
}


bool DefList::Set12(
  const Trick& trick1,
  const Trick& trick20,
  const Trick& trick21)
{
  headerDirty = true;
  len = 1;
  return list[0].Set12(trick1, trick20, trick21);
}


Header& DefList::GetHeader()
{
  if (! headerDirty)
    return header;
  headerDirty = false;

  assert(len > 0);

  header = list[0].GetHeader();
  for (unsigned d = 1; d < len; d++)
    header.MergeMin(list[d].GetHeader());

  return header;
}


unsigned DefList::GetKey() 
{
  unsigned sum = 0;
  for (unsigned d = 0; d < len; d++)
    sum += list[d].GetLen();
  
  return (len-1) + (sum << 4);
}


bool DefList::operator == (
  const DefList& def2)
{
  if (len != def2.len)
    return false;

  vector<bool> same1(len, false);
  vector<bool> same2(len, false);
  unsigned num = 0;

  for (unsigned d2 = 0; d2 < len; d2++)
  {
    if (same2[d2])
      continue;

    for (unsigned d1 = 0; d1 < len; d1++)
    {
      if (same1[d1])
        continue;

      if (list[d1] == def2.list[d2])
      {
        num++;
        same1[d1] = true;
        same2[d2] = true;
      }
    }
  }

  return (num == len);
}


bool DefList::operator != (
  const DefList& def2)
{
  return ! (* this == def2);
}


void DefList::operator += (
  const Holding& holding)
{
  // Prepends the current move in holding to each defense.

  headerDirty = true;
  for (unsigned d = 0; d < len; d++)
    list[d] += holding;

  DefList::RegisterSize("NEWDEF1");

  DefList::Reduce();
}


void DefList::operator += (
  AltList& alt)
{
  // Adds an alternative to the current set of alternatives,
  // eliminating inferior ones (as seen from the defensive side).

  vector<int> seen(SDS_CMP_SIZE, 0);
  vector<bool> skip(len, false);
  bool purge = false;

  for (unsigned d = 0; d < len; d++)
  {
    cmpDetailType cd = list[d].Compare(alt);
    cmpType c = cmpDetailToShort[cd];
    seen[c] = 1;

    if (c == SDS_OLD_BETTER)
    {
      // Defense will prefer the new one.
      purge = true;
      skip[d] = true;
    }
  }

  int cc = seen[SDS_SAME] + seen[SDS_NEW_BETTER] + seen[SDS_OLD_BETTER];
  assert(cc <= 1);

  if (cc == 1 && ! purge)
    return;

  if (purge)
  {
    assert(seen[SDS_OLD_BETTER]);
    DefList::Purge(skip);
  }

  assert(len < SDS_MAX_DEF);
  list[len++] = alt;
  headerDirty = true;

  DefList::RegisterSize("NEWDEF2");
}


void DefList::operator += (
  DefList& def2)
{
  // For the defense, we just concatenate the defenses.

  if (len == 0)
  {
    * this = def2;
    return;
  }

  assert(def2.len > 0);
  
  headerDirty = true;

  if (debugDefList)
  {
    DefList::Print(cout, "DefList::MergeDefender: old");
    def2.Print(cout, "DefList::MergeDefender: new");
  }

  for (unsigned d2 = 0; d2 < def2.len; d2++)
    * this += def2.list[d2];

  if (debugDefList)
    DefList::Print(cout, "DefList::MergeDefender after merge");

  DefList::RegisterSize("NEWDEF3");
}


void DefList::operator *= (
  DefList& def2)
{
  /*
     Need to make the "cross product" for declarer.

     General principle:
    
     List 1: min( max(a1, a2), max(a3, a4)
     List 2: min( max(a5, a6), max(a7, a8)
    
     max(List 1, List2) = 
       min( max(a1256), max(a1278), max(3456), max(3478))

     For each defense, there is a worst rank needed in any of the
     alternatives.  If we eliminate that alternative, we lose the
     reference to the rank, making the result too optimistic.  So
     we first figure out the worst rank for each defense.  Then for
     that defense, we go through its alternatives and lower the
     ranks in each alternative.  This is a bit tricky.  We need
     to know the highest rank held by the opponents.
  */

  assert(len > 0);
  assert(def2.len > 0);

  headerDirty = true;

  if (debugDefList)
  {
    DefList::Print(cout, "DefList::MergeDeclarer old");
    def2.Print(cout, "DefList::MergeDeclarer: new");
  }

  // Make one copy.
  unsigned oldLen = len;
  vector<AltList> oldList(oldLen);
  for (unsigned d = 0; d < oldLen; d++)
    oldList[d] = list[d];

  len = 0;

  for (unsigned dnew = 0; dnew < def2.len; dnew++)
    for (unsigned dold = 0; dold < oldLen; dold++)
    {
      AltList alt = oldList[dold] + def2.list[dnew];
      * this += alt;
    }

  if (debugDefList)
    DefList::Print(cout, "DefList::MergeDeclarer after Merge");

  DefList::RegisterSize("NEWDEF4");
}


void DefList::MergeSides(
  DefList& def1,
  DefList& def2)
{
  // The two defenses have different starting sides.
  // It is not clear that these should be merged.  Here we take
  // the view that it is nice to merge those entries that are the
  // same except for the starting point.  The rest are concatenated.

  assert(def1.len > 0);
  assert(def2.len > 0);

  headerDirty = true;

  if (debugDefList)
  {
    def1.Print(cout, "DefList::MergeSideMoves def1");
    def2.Print(cout, "DefList::MergeSideMoves def2");
  }

  len = 0;

  vector<bool> purge1(def1.len, false);
  vector<bool> purge2(def2.len, false);

  for (unsigned d2 = 0; d2 < def2.len; d2++)
  {
    if (purge2[d2])
      continue;

    for (unsigned d1 = 0; d1 < def1.len; d1++)
    {
      if (purge1[d1])
        continue;

      if (def1.list[d1].MergeSides(def2.list[d2]))
      {
        purge1[d1] = true;
        purge2[d2] = true;

        * this += def1.list[d1];

        break;
      }
    }
  }

  // Merge defenses as a cross product.

  for (unsigned d2 = 0; d2 < def2.len; d2++)
  {
    if (purge2[d2])
      continue;
    for (unsigned d1 = 0; d1 < def1.len; d1++)
    {
      if (purge1[d1])
        continue;
      AltList alt = def1.list[d1] + def2.list[d2];
      * this += alt;
    }
  }

  if (debugDefList)
    DefList::Print(cout, "DefList::MergeSideMoves result");

  DefList::RegisterSize("NEWDEF5");
}


void DefList::Purge(
  const vector<bool> skip)
{
  unsigned p = 0;
  for (unsigned d = 0; d < len; d++)
  {
    if (! skip[d])
    {
      if (d > p)
        list[p] = list[d];
      p++;
    }
  }
  len = p;
}


void DefList::FixRanks(
  const unsigned m)
{
  if (len <= 1)
    return;

  DefList::GetHeader();
  unsigned hrank = header.GetMaxRank();
  bool fixed = false;

  for (unsigned d = 0; d < len; d++)
  {
    if (list[d].FixRanks(m, hrank))
      fixed = true;
  }

  if (! fixed)
    return;

  headerDirty = true;

  if (debugDefList)
    DefList::Print(cout, "After FixRanks");

  if (! DefList::Reduce())
    return;

  if (debugDefList)
    DefList::Print(cout, "After Reduce");
}


bool DefList::Reduce()
{
  // Eliminate superfluous combinations, as seen by defenders.

  if (len <= 1)
    return false;

  vector<bool> skip(len, false);
  bool purge = false;

  for (unsigned d1 = 0; d1 < len; d1++)
  {
    if (skip[d1])
      continue;

    for (unsigned d2 = d1+1; d2 < len; d2++)
    {
      if (skip[d2])
        continue;

      cmpDetailType cd = list[d2].Compare(list[d1]);
      cmpType c = cmpDetailToShort[cd];

      // If new (d1) is better, the defense will not choose it.
      if (c == SDS_SAME || c == SDS_NEW_BETTER)
      {
        skip[d1] = true;
        purge = true;
      }
      else if (c == SDS_OLD_BETTER)
      {
        skip[d2] = true;
        purge = true;
      }
    }
  }

  if (purge)
    DefList::Purge(skip);

  return purge;
}


void DefList::RegisterSize(
  const string& text)
{
  if (len > highestDefNo) 
  {
    highestDefNo = len;
    cout << text << ": " << len << "\n";

    if (len > SDS_MAX_DEF)
    {
      cerr << "Too many alternatives" << endl;
      assert(false);
    }
  }
}


void DefList::Print(
  ostream& out,
  const string& text)
{
  if (! text.empty())
    out << setw(0) << text << "\n";

  out << 
      setw(4) << "Def" << 
      setw(6) << "Alt" << 
      setw(6) << "No" << 
      setw(6) << "Start" << 
      setw(6) << "End" <<
      setw(6) << "Cash" <<
      setw(9) << "Ranks\n";
  
  for (unsigned d = 0; d < len; d++)
    list[d].Print(out, "", d);

  out << endl;
}

