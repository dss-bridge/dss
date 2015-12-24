/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iomanip>
#include <assert.h>

#include "DefList.h"

using namespace std;


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


bool DefList::IsEmpty() const
{
  return (len == 0);
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

  list[0].GetHeader(header);
  Header headerNew;
  for (unsigned d = 1; d < len; d++)
  {
    list[d].GetHeader(headerNew);
    header.MergeMin(headerNew);
  }

  return header;
}


bool DefList::operator == (
  const DefList& def2) const
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
  const DefList& def2) const
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
  const AltList& alt)
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
  if (cc > 1)
  {
    cout << seen[SDS_SAME] << " " << 
      seen[SDS_NEW_BETTER] << " " <<
      seen[SDS_OLD_BETTER] << "\n";
    DefList::Print();
    cout << "\n";
    alt.Print(cout, "Offending alt");
    cout << endl;
    assert(cc <= 1);
  }

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
  const DefList& def2)
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
  const DefList& def2)
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

  /*
     If one side has > (at least once), = and no !=, AND
     if the other side has <, = and perhaps !=, THEN
     the first side is better.
     Declarer will not choose the second side, as the defense
     can then stick to the inferior ones and always do at least
     as well.
  */

  AltMatrix2D comp;
  comp.SetDimensions(len, def2.len);

  for (unsigned dOld = 0; dOld < len; dOld++)
    for (unsigned dNew = 0; dNew < def2.len; dNew++)
      comp.SetValue(dOld, dNew, list[dOld].CompareHard(def2.list[dNew]));

  cmpDetailType c = comp.CompareDeclarer();
  if (c == SDS_HEADER_PLAY_OLD_BETTER ||
      c == SDS_HEADER_RANK_OLD_BETTER ||
      c == SDS_HEADER_SAME)
  {
    // Nothing more to do.
  }
  else if (c == SDS_HEADER_PLAY_NEW_BETTER ||
      c == SDS_HEADER_RANK_NEW_BETTER)
  {
    * this = def2;
  }
  else
  {
    // Make one copy.
    unsigned oldLen = len;
    vector<AltList> oldList(oldLen);
    for (unsigned d = 0; d < oldLen; d++)
      oldList[d] = list[d];

    len = 0;

    for (unsigned dnew = 0; dnew < def2.len; dnew++)
    {
      for (unsigned dold = 0; dold < oldLen; dold++)
      {
        AltList alt = oldList[dold] + def2.list[dnew];
        * this += alt;
      }
    }
  }

  if (debugDefList)
    DefList::Print(cout, "DefList::MergeDeclarer after Merge");

  DefList::RegisterSize("NEWDEF4");
}


bool DefList::MergeSidesSoft(
  const DefList& def1,
  const DefList& def2)
{
  if (def2.len == 0)
  {
    * this = def1;
    return true;
  }

  if (def1.len != def2.len)
    return false;

  if (debugDefList)
  {
    def1.Print(cout, "DefList::MergeSidesSoft def1");
    def2.Print(cout, "DefList::MergeSidesSoft def2");
  }

  DefList::Reset();

  if (def1.len == 1 && def2.len == 1)
  {
    if (def1.list[0].CanMergeSides(def2.list[0]))
    {
      * this = def1;
      list[0].SetStart();
    }
    else
    {
      AltList alt = def1.list[0] + def2.list[0];
      * this += alt;
    }
    return true;
  }

  vector<bool> purge1(def1.len, false);
  vector<bool> purge2(def2.len, false);
  unsigned count = 0;

  for (unsigned d2 = 0; d2 < def2.len; d2++)
  {
    if (purge2[d2])
      continue;

    for (unsigned d1 = 0; d1 < def1.len; d1++)
    {
      if (purge1[d1])
        continue;

      if (def1.list[d1].CanMergeSides(def2.list[d2]))
      {
        purge1[d1] = true;
        purge2[d2] = true;
        count++;

        * this += def1.list[d1];

        break;
      }
    }
  }

  if (count != def1.len)
    return false;

  * this = def1;
  for (unsigned d = 0; d < len; d++)
    list[d].SetStart();

  if (debugDefList)
    DefList::Print(cout, "DefList::MergeSidesSoft result");

  return true;
}


void DefList::MergeSidesHard(
  const DefList& def1,
  const DefList& def2)
{
  // The two defenses have different starting sides.
  // It is not clear that these should be merged.  Here we take
  // the view that it is nice to merge those entries that are the
  // same except for the starting point.  The rest are concatenated.

  assert(def1.len > 0);
  assert(def2.len > 0);

  DefList::Reset();

  if (debugDefList)
  {
    def1.Print(cout, "DefList::MergeSidesHard def1");
    def2.Print(cout, "DefList::MergeSidesHard def2");
  }

  // Merge defenses as a cross product.

  for (unsigned d2 = 0; d2 < def2.len; d2++)
  {
    for (unsigned d1 = 0; d1 < def1.len; d1++)
    {
      AltList alt = def1.list[d1] + def2.list[d2];
      * this += alt;
    }
  }

  if (debugDefList)
    DefList::Print(cout, "DefList::MergeSidesHard result");

  DefList::RegisterSize("NEWDEF5");
}


void DefList::Purge(
  const vector<bool>& skip)
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
  const string& text) const
{
  if (! text.empty())
    out << setw(0) << text << "\n";

  out << 
      right <<
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

