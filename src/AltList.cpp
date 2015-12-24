/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <assert.h>

#include "cst.h"
#include "AltList.h"

using namespace std;


extern bool debugAltList;
extern unsigned highestAltNo;


AltList::AltList()
{
  AltList::Reset();
}


AltList::~AltList()
{
}


void AltList::Reset()
{
  len = 0;
}


bool AltList::Set1(
  const Trick& trick)
{
  len = 1;
  return list[0].Set1(trick);
}


bool AltList::Set2(
  const Trick& trick10,
  const Trick& trick11)
{
  len = 1;
  return list[0].Set2(trick10, trick11);
}


bool AltList::Set11(
  const Trick& trick1,
  const Trick& trick2)
{
  len = 2;
  (void) list[0].Set1(trick1);
  return list[1].Set1(trick2);
}


bool AltList::Set12(
  const Trick& trick1,
  const Trick& trick20,
  const Trick& trick21)
{
  len = 2;
  (void) list[0].Set1(trick1);
  return list[1].Set2(trick20, trick21);
}


void AltList::GetHeader(
  Header& header) const
{
  assert(len > 0);

  list[0].GetHeader(header);
  Header hNew;
  for (unsigned a = 1; a < len; a++)
  {
    list[a].GetHeader(hNew);
    header.MergeMax(hNew);
  }
}


unsigned AltList::GetLength() const
{
  return len;
}


cmpDetailType AltList::Compare(
  const AltList& aNew) const
{
  unsigned numOld = len;
  unsigned numNew = aNew.len;

  AltMatrix2D comp;
  comp.SetDimensions(numOld, numNew);

  for (unsigned lOld = 0; lOld < numOld; lOld++)
    for (unsigned lNew = 0; lNew < numNew; lNew++)
      comp.SetValue(lOld, lNew, list[lOld].Compare(aNew.list[lNew]));

  cmpDetailType c = comp.Compare();
  if (c != SDS_HEADER_PLAY_DIFFERENT && c != SDS_HEADER_RANK_DIFFERENT)
    return c;

  if (AltList::CompareMultiSide(QT_PARD, comp, aNew))
    return SDS_HEADER_PLAY_OLD_BETTER;
  else if (aNew.CompareMultiSide(QT_ACE, comp, * this))
    return SDS_HEADER_PLAY_NEW_BETTER;
  else
    return c;
}


cmpDetailType AltList::CompareHard(
  const AltList& aNew) const
{
  unsigned numOld = len;
  unsigned numNew = aNew.len;

  AltMatrix2D comp;
  comp.SetDimensions(numOld, numNew);

  for (unsigned lOld = 0; lOld < numOld; lOld++)
    for (unsigned lNew = 0; lNew < numNew; lNew++)
      comp.SetValue(lOld, lNew, list[lOld].Compare(aNew.list[lNew]));

  return comp.CompareHard();
}


bool AltList::operator == (
  const AltList& alt2) const
{
  if (len != alt2.len)
    return false;

  vector<bool> same1(len, false);
  vector<bool> same2(len, false);
  unsigned num = 0;

  for (unsigned a2 = 0; a2 < len; a2++)
  {
    if (same2[a2])
      continue;

    for (unsigned a1 = 0; a1 < len; a1++)
    {
      if (same1[a1])
        continue;

      if (list[a1] == alt2.list[a2])
      {
        num++;
        same1[a1] = true;
        same2[a2] = true;
      }
    }
  }

  return (num == len);
}


bool AltList::operator != (
  const AltList& alt2) const
{
  return ! (* this == alt2);
}


void AltList::operator += (
  const Holding& holding)
{
  for (unsigned a = 0; a < len; a++)
    list[a] += holding;
  
  AltList::Reduce();
}


AltList AltList::operator + (
  const AltList& aRight) const
{
  // We might change the alts, so we have to copy them (ugh).
  AltList aOld = * this;
  AltList aNew = aRight;

  if (debugAltList)
  {
    aOld.Print(cout, "AltList::++ aOld");
    aNew.Print(cout, "AltList::++ aNew");
  }

  vector<bool> softX(aOld.len, true);
  vector<bool> softY(aNew.len, true);

  AltMatrix1D compX, compY;
  compX.SetDimension(aOld.len);
  compY.SetDimension(aNew.len);

  aOld.FillMatrix2D(aNew, compX, compY, softX, softY);

  aOld.PurgeList(compX);

  for (unsigned a2 = 0; a2 < aNew.len; a2++)
  {
    if (compY.IsPurged(a2))
     continue;

    assert(aOld.len < SDS_MAX_ALT);
    aOld.list[aOld.len++] = aNew.list[a2];
  }

  aOld.RegisterSize("NEWALT");

  aOld.Reduce();

  if (debugAltList)
    aOld.Print(cout, "AltList::++ result");

  return aOld;
}


bool AltList::CanMergeSides(
  const AltList& aNew) const
{
  if (len != aNew.len)
    return false;

  vector<bool> oldUsed(len, false);
  vector<bool> newUsed(len, false);
  unsigned count = 0;

  for (unsigned a1 = 0; a1 < len; a1++)
  {
    if (oldUsed[a1])
      continue;

    for (unsigned a2 = 0; a2 < len; a2++)
    {
      if (newUsed[a2])
        continue;

      if (list[a1].EqualsExceptStart(aNew.list[a2]))
      {
        oldUsed[a1] = true;
        newUsed[a2] = true;
        count++;
      }
    }
  }

  return (count == len);
}


void AltList::SetStart()
{
  for (unsigned a = 0; a < len; a++)
    list[a].SetStart(QT_BOTH);

  AltList::Reduce();
}


bool AltList::MergeSides(
  const AltList& aNew) 
{
  if (! AltList::CanMergeSides(aNew))
    return false;

  AltList::SetStart();
  return true;
}


void AltList::Backtrack1D(
  AltMatrix1D& comp, 
  const unsigned dimFixed, 
  const unsigned dimVar, 
  vector<fixType>& fixVector)
{
  fixType fix1, fix2;
  cmpDetailType c;
 
  if (fixVector[dimFixed] == SDS_FIX_STRONGER)
  {
    for (unsigned b = 0; b < dimVar; b++)
    {
      unsigned a = dimVar-1 - b;
      if (a == dimFixed || comp.IsPurged(a))
        continue;

      c = list[dimFixed].FixOrCompare(list[a], fix1, fix2);
      comp.SetValue(dimFixed, a, c);

      assert(fix1 != SDS_FIX_STRONGER && fix1 != SDS_FIX_PURGED);
      assert(fix2 != SDS_FIX_STRONGER);

      if (fix1 == SDS_FIX_WEAKER)
      {
        // Unusual, but can happen.
        fixVector[dimFixed] = SDS_FIX_WEAKER;
	AltList::Backtrack1D(comp, dimFixed, dimVar, fixVector);
      }

      if (fix2 == SDS_FIX_PURGED)
        comp.Purge(a);
      else if (fix2 == SDS_FIX_WEAKER)
      {
        assert(fixVector[a] != SDS_FIX_STRONGER);

        fixVector[a] = SDS_FIX_WEAKER;
	AltList::Backtrack1D(comp, a, dimFixed, fixVector);
      }
    }
  }
  else if (fixVector[dimFixed] == SDS_FIX_WEAKER)
  {
    for (unsigned b = 0; b < dimVar; b++)
    {
      unsigned a = dimVar-1-b;
      if (a == dimFixed || comp.IsPurged(a))
        continue;

      c = list[dimFixed].FixOrCompare(list[a], fix1, fix2);
      comp.SetValue(dimFixed, a, c);

      assert(fix1 != SDS_FIX_WEAKER);
      assert(fix2 != SDS_FIX_WEAKER);

      if (fix1 == SDS_FIX_PURGED)
      {
        comp.Purge(dimFixed);
        break;
      }

      if (fix2 == SDS_FIX_STRONGER)
      {
        assert(fixVector[a] == SDS_FIX_UNCHANGED);

        fixVector[a] = SDS_FIX_STRONGER;
	AltList::Backtrack1D(comp, a, dimFixed, fixVector);
      }
    }
  }

  if (debugAltList)
    cout << "Backtrack1D done\n";
}


void AltList::FillMatrix1D(
  AltMatrix1D& comp)
{
  fixType fix1, fix2;

  for (unsigned a1 = 0; a1 < len-1; a1++)
  {
    if (comp.IsPurged(a1))
      continue;

    for (unsigned a2 = a1+1; a2 < len; a2++)
    {
      if (comp.IsPurged(a2))
        continue;

      comp.SetValue(a1, a2, list[a1].FixOrCompare(list[a2], fix1, fix2));

      if (fix2 == SDS_FIX_PURGED)
        comp.Purge(a2);
      else if (fix2 == SDS_FIX_STRONGER || fix2 == SDS_FIX_WEAKER)
      {
        vector<fixType> fixVector(len, SDS_FIX_UNCHANGED);
	fixVector[a2] = fix2;

	AltList::Backtrack1D(comp, a2, a1, fixVector);
      }

      if (fix1 == SDS_FIX_PURGED && ! comp.IsPurged(a1))
        comp.Purge(a1);
      else if (fix1 == SDS_FIX_STRONGER || fix1 == SDS_FIX_WEAKER)
      {
        vector<fixType> fixVector(len, SDS_FIX_UNCHANGED);
	fixVector[a1] = fix1;

	AltList::Backtrack1D(comp, a1, a2, fixVector);
      }

      if (comp.IsPurged(a1))
        break;
    }
  }
}


void AltList::FillMatrix2D(
  AltList& aNew,
  AltMatrix1D& compX,
  AltMatrix1D& compY,
  const vector<bool>& softX,
  const vector<bool>& softY)
{
  // The soft lists are true for those entries that have just
  // been weakened or strengthened.

  fixType fix1, fix2;

  vector<bool> nextSoftX(len, false);
  vector<bool> nextSoftY(aNew.len, false);

  for (unsigned a1 = 0; a1 < len; a1++)
  {
    if (compX.IsPurged(a1))
      continue;

    for (unsigned a2 = 0; a2 < aNew.len; a2++)
    {
      if ((! softX[a1] && ! softY[a2]) || compY.IsPurged(a2))
        continue;

      (void) list[a1].FixOrCompare(aNew.list[a2], fix1, fix2);
      if (fix2 == SDS_FIX_PURGED)
      {
        compY.Purge(a2);
        nextSoftY[a2] = false;
      }
      else if (fix2 == SDS_FIX_STRONGER || fix2 == SDS_FIX_WEAKER)
      {
        nextSoftY[a2] = true;
        vector<fixType> fixVector(aNew.len, SDS_FIX_UNCHANGED);
        fixVector[a2] = fix2;

        aNew.Backtrack1D(compY, a2, aNew.len, fixVector);
      }

      if (fix1 == SDS_FIX_PURGED && ! compX.IsPurged(a1))
      {
        compX.Purge(a1);
        nextSoftX[a1] = false;
      }
      else if (fix1 == SDS_FIX_STRONGER || fix1 == SDS_FIX_WEAKER)
      {
        nextSoftX[a1] = true;
        vector<fixType> fixVector(len, SDS_FIX_UNCHANGED);
        fixVector[a1] = fix1;

        AltList::Backtrack1D(compX, a1, len, fixVector);
      }

      if (compX.IsPurged(a1))
        break;
    }
  }

  if (AltList::AllFalse(nextSoftX, len) && 
      AltList::AllFalse(nextSoftY, len))
    return;

  AltList::FillMatrix2D(aNew, compX, compY, nextSoftX, nextSoftY);
}


bool AltList::AllFalse(
  const vector<bool>& vec, 
  const unsigned vlen) const
{
  for (unsigned i = 0; i < vlen; i++)
    if (vec[i])
      return false;

  return true;
}


void AltList::PurgeList(
  const AltMatrix1D& comp)
{
  unsigned p = 0;
  for (unsigned a1 = 0; a1 < len; a1++)
  {
    if (comp.IsPurged(a1))
      continue;

    if (a1 > p)
      list[p] = list[a1];
    p++;
  }

  len = p; 

  assert(p <= SDS_MAX_ALT);
}


void AltList::PurgeList(
  const vector<bool>& purgeList)
{
  // Later think of a way to have only one PurgeList function.

  unsigned p = 0;
  for (unsigned a1 = 0; a1 < len; a1++)
  {
    if (purgeList[a1])
      continue;

    if (a1 > p)
      list[p] = list[a1];
    p++;
  }

  len = p; 

  assert(p <= SDS_MAX_ALT);
}


void AltList::PunchOut(
  const AltList * alist,
  const unsigned int purgeNo,
  const vector<bool>& purgeList,
  const int pstart)
{
  len = 0;
  for (unsigned int a = 0; a < alist->len; a++)
  {
    if (a == purgeNo || 
        purgeList[a] || 
        alist->list[a].GetFirstStart() == SDS_PARTNER[pstart])
      continue;
    
    list[len++] = alist->list[a];
  }
}


void AltList::PunchOut(
  const AltList * alist,
  const int pstart)
{
  len = 0;
  for (unsigned int a = 0; a < alist->len; a++)
  {
    if (alist->list[a].GetFirstStart() == SDS_PARTNER[pstart])
      continue;
    
    list[len++] = alist->list[a];
  }
}


void AltList::Reduce()
{
  if (len <= 1)
    return;

  if (debugAltList)
    AltList::Print(cout, "AltList::Reduce before");

  AltMatrix1D comp;
  comp.SetDimension(len);

  AltList::FillMatrix1D(comp);

  AltList::PurgeList(comp);

  AltList::PurgeMulti();

  if (debugAltList)
    AltList::Print(cout, "AltList::Reduce after");
}


void AltList::PurgeMulti()
{
  if (len < 3)
    return;

  AltList aRed;
  TrickList tlist;
  vector<bool> purgeList(len, false);
  for (unsigned a = 0; a < len; a++)
  {
    tlist = list[a];
    posType pstart = tlist.GetFirstStart();
    if (tlist.GetLength() == 1 || pstart == QT_BOTH)
      continue;

    aRed.PunchOut(this, a, purgeList, pstart);
    if (aRed.len < 2)
      continue;

    if (cmpDetailToGE[aRed.CompareMultiTrickList(tlist)])
      purgeList[a] = true;
  }

  AltList::PurgeList(purgeList);
}


cmpDetailType AltList::CompareMulti(
  const TrickList& tref) const
{
  if (len < 2)
    return SDS_HEADER_PLAY_DIFFERENT;

  AltList aRed;
  TrickList tlist = tref;
  posType pstart = tlist.GetFirstStart();
  if (pstart == QT_BOTH)
    return SDS_HEADER_PLAY_DIFFERENT;

  aRed.PunchOut(this, pstart);
  if (aRed.len < 2)
    return SDS_HEADER_PLAY_DIFFERENT;

  return aRed.CompareMultiTrickList(tlist);
}


bool AltList::CompareMultiSide(
  const posType sideToLose,
  const AltMatrix2D& comp,
  const AltList& altToLose) const
{
  bool use[SDS_MAX_ALT];
  cmpDetailType cRunning;
  if (! comp.CandList(sideToLose, use, cRunning))
    return false;

  for (unsigned a = 0; a < altToLose.len; a++)
  {
    if (! use[a])
      continue;

    cmpDetailType c = AltList::CompareMulti(altToLose.list[a]);
    if (c == SDS_HEADER_PLAY_DIFFERENT)
      return false;
    else
      cRunning = cmpDetailMatrix[cRunning][c];
  }
  return cmpDetailToGE[cRunning];
}


cmpDetailType AltList::CompareMultiTrickList(
  TrickList& tlist)
{
  // Both AltList and tlist are modified in this call!
  // They should be temporary to the caller.
  // Returns true if AltList in totality is >= tlist,
  // possibly using more than one trick list to do so.
  // The comparison is done one segment at a time, then
  // connecting up the segments.

  unsigned int tlen = tlist.GetLength();

  cmpDetailType cRunning = SDS_HEADER_SAME;
  for (unsigned s = 0; s < tlen; s++)
  {
    cmpDetailType c = AltList::FrontIsGE(tlist);
    if (c == SDS_HEADER_PLAY_DIFFERENT)
      return SDS_HEADER_PLAY_DIFFERENT;
    cRunning = cmpDetailMatrix[cRunning][c];
    
    posType pend = tlist.ConnectFirst();
    AltList::ConnectFirst(pend);
  }
  return cRunning;
}


cmpDetailType AltList::FrontIsGE(
  const TrickList& tlist) const
{
  // Uses same return values as below.

  Trick htrick;
  tlist.GetFirstSummaryTrick(htrick);
  if (htrick.GetEnd() == QT_BOTH)
  {
    htrick.SetEnd(QT_ACE);
    cmpDetailType ca = AltList::FrontIsGE(htrick);

    if (ca == SDS_HEADER_PLAY_DIFFERENT)
      return SDS_HEADER_PLAY_DIFFERENT;

    htrick.SetEnd(QT_PARD);
    cmpDetailType cp = FrontIsGE(htrick);
    if (cp == SDS_HEADER_PLAY_DIFFERENT)
      return SDS_HEADER_PLAY_DIFFERENT;

    return cmpDetailMatrix[ca][cp];
  }
  else
    return AltList::FrontIsGE(htrick);
}


cmpDetailType AltList::FrontIsGE(
  const Trick& trick) const
{
  // Only uses SDS_HEADER_SAME, SDS_HEADER_PLAY_OLD_BETTER,
  // SDS_HEADER_RANK_NEW_BETTER, SDS_PLAY_DIFFERENT.

  bool rankNewFlag = false;
  for (unsigned a = 0; a < len; a++)
  {
    switch(list[a].CompareToTrick(trick))
    {
      case SDS_HEADER_PLAY_OLD_BETTER:
        return SDS_HEADER_PLAY_OLD_BETTER;
      case SDS_HEADER_SAME:
      case SDS_HEADER_RANK_OLD_BETTER:
        return SDS_HEADER_SAME;
      case SDS_HEADER_RANK_NEW_BETTER:
        rankNewFlag = true;
        break;
      case SDS_HEADER_PLAY_NEW_BETTER:
      case SDS_HEADER_PLAY_DIFFERENT:
      case SDS_HEADER_RANK_DIFFERENT:
        break;
    }
  }
  return (rankNewFlag ? SDS_HEADER_RANK_NEW_BETTER : 
    SDS_HEADER_PLAY_DIFFERENT);
}


void AltList::ConnectFirst(
  const posType pend)
{
  for (unsigned a = 0; a < len; a++)
    list[a].ConnectFirst(pend);
}


void AltList::RegisterSize(
  const string& text)
{
  if (len <= highestAltNo)
    return;

  highestAltNo = len;
  cout << text << ": " << len << "\n";

  if (len > SDS_MAX_ALT)
  {
    cout << "Too many alternatives" << endl;
    assert(false);
  }
}


void AltList::Print(
  std::ostream& out,
  const string text,
  const unsigned d) const
{
  if (! text.empty())
    out << text << "\n";

  for (unsigned a = 0; a < len; a++)
    list[a].Print(out, d, a);
}

