/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <assert.h>

#include "cst.h"
#include "AltList.h"
// #include "portab.h"

extern bool debugAltList;
extern unsigned highestAltNo;


bool AltListMulti = false;


AltList::AltList()
{
  AltList::Reset();
}


AltList::~AltList()
{
}


void AltList::Reset()
{
  headerDirty = true;
  len = 0;
}


bool AltList::Set1(
  const Trick& trick)
{
  headerDirty = true;
  len = 1;
  return list[0].Set1(trick);
}


bool AltList::Set2(
  const Trick& trick10,
  const Trick& trick11)
{
  headerDirty = true;
  len = 1;
  return list[0].Set2(trick10, trick11);
}


bool AltList::Set11(
  const Trick& trick1,
  const Trick& trick2)
{
  headerDirty = true;
  len = 2;
  (void) list[0].Set1(trick1);
  return list[1].Set1(trick2);
}


bool AltList::Set12(
  const Trick& trick1,
  const Trick& trick20,
  const Trick& trick21)
{
  headerDirty = true;
  len = 2;
  (void) list[0].Set1(trick1);
  return list[1].Set2(trick20, trick21);
}


Header& AltList::GetHeader()
{
  if (! headerDirty)
    return header;
  headerDirty = false;

  assert(len > 0);

  header = list[0].GetHeader();
  for (unsigned a = 1; a < len; a++)
    header.MergeMax(list[a].GetHeader());

  return header;
}


unsigned AltList::GetLen() const
{
  return len;
}


cmpDetailType AltList::Compare(
  AltList& aNew)
{
  unsigned numOld = len;
  unsigned numNew = aNew.len;

  AltMatrix2D comp;
  comp.SetDimensions(numOld, numNew);

  for (unsigned lOld = 0; lOld < numOld; lOld++)
    for (unsigned lNew = 0; lNew < numNew; lNew++)
      comp.SetValue(lOld, lNew, list[lOld].Compare(aNew.list[lNew]));

  cmpDetailType c = comp.Compare();
  return c;
  // if (c != SDS_HEADER_PLAY_DIFFERENT && c != SDS_HEADER_RANK_DIFFERENT)
    // return c;

  /*
  if (AltList::CompareMultiSide(QT_PARD, comp, aNew))
    return SDS_HEADER_PLAY_OLD_BETTER;
  else if (aNew.CompareMultiSide(QT_ACE, comp, * this))
    return SDS_HEADER_PLAY_NEW_BETTER;
  else
    return c;
  */
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
  headerDirty = true;

  for (unsigned a = 0; a < len; a++)
    list[a] += holding;
  
  AltList::Reduce();
}


AltList AltList::operator + (const AltList& aRight)
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
    if (! compY.IsPurged(a2))
    {
      assert(aOld.len < SDS_MAX_ALT);
      aOld.list[aOld.len++] = aNew.list[a2];
    }
  }

  aOld.RegisterSize("NEWALT");

  aOld.PurgeMulti();

  headerDirty = true;

  if (debugAltList)
    aOld.Print(cout, "AltList::++ result");

  return aOld;
}


bool AltList::MergeSides(AltList& aNew)
{
  if (len != aNew.len)
    return false;

  vector<bool> oldUsed(len, false);
  vector<bool> newUsed(len, false);
  vector<bool> oldFix(len, false);
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
        oldFix[a1] = true;

        count++;
      }
      else if (list[a1] == aNew.list[a2])
      {
        oldUsed[a1] = true;
        newUsed[a2] = true;

        count++;
      }
    }
  }

  if (count != len)
    return false;

  headerDirty = true;
  for (unsigned a = 0; a < len; a++)
  {
    if (oldFix[a])
      list[a].SetStart(QT_BOTH);
  }

  AltList::Reduce();

  return true;
}


void AltList::Backtrack1D(
  AltMatrix1D& comp, 
  const unsigned dimFixed, 
  const unsigned dimVar, 
  vector<fixType> fixVector)
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

      assert(fix1 != SDS_FIX_STRONGER && fix1 != SDS_FIX_WEAKER);
      assert(fix2 != SDS_FIX_PURGED && fix2 != SDS_FIX_WEAKER);

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
  vector<bool> softX,
  vector<bool> softY)
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
  vector<bool> vec, 
  const unsigned vlen) const
{
  for (unsigned i = 0; i < vlen; i++)
    if (vec[i])
      return false;

  return true;
}


void AltList::PurgeList(
  AltMatrix1D& comp)
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
  vector<bool>& purgeList)
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
  headerDirty = true;
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
  headerDirty = true;
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

  headerDirty = true;

  if (debugAltList)
    AltList::Print(cout, "AltList::Reduce after");
}


void AltList::PurgeMulti()
{
  if (len < 3)
    return;

if (AltListMulti)
  AltList::Print(cout, "PrePurgeMulti");

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

    if (aRed >= tlist)
      purgeList[a] = true;
  }

  AltList::PurgeList(purgeList);
}


bool AltList::CompareMulti(const TrickList& tref)
{
  if (len < 2)
    return false;

if (AltListMulti)
  AltList::Print(cout, "PreCompMulti");

  AltList aRed;
  TrickList tlist;
  for (unsigned a = 0; a < len; a++)
  {
    tlist = tref;
    posType pstart = tlist.GetFirstStart();
    if (tlist.GetLength() == 1 || pstart == QT_BOTH)
      //
      // Can probably just return false here?
      //
      continue;

    aRed.PunchOut(this, pstart);
    if (aRed.len < 2)
      continue;

    if (aRed >= tlist)
      return true;
  }

  return false;
}


bool AltList::CompareMultiSide(
  const posType sideToLose,
  const AltMatrix2D& comp,
  const AltList& altToLose)
{
  bool use[SDS_MAX_ALT];
  if (! comp.CandList(sideToLose, use))
{
cout << "CMS no candidates\n";
    return false;
}

  for (unsigned a = 0; a < altToLose.len; a++)
  {
    if (! use[a])
      continue;
cout << "CMS try a " << a << "\n";

    if (! AltList::CompareMulti(altToLose.list[a]))
{
cout << "CMS failed\n";
      return false;
}
  }
cout << "CMS succeeded\n";
  return true;
}


bool AltList::operator >= (TrickList& tlist)
{
  // Both AltList and tlist are modified in this call!
  // They should be temporary to the caller.
  // Returns true if AltList in totality is >= tlist,
  // possibly using more than one trick list to do so.
  // The comparison is done one segment at a time, then
  // connecting up the segments.

  unsigned int tlen = tlist.GetLength();

if (AltListMulti)
{
  cout << "AltGE tlen " << len << "\n";
  AltList::Print(cout, "aRed");
  cout << "\n";
  tlist.Print(cout);
  cout << "\n";
}

  for (unsigned s = 0; s < tlen; s++)
  {
if (AltListMulti)
  cout << "s " << s << endl;
    if (! AltList::FrontIsGE(tlist))
      return false;
if (AltListMulti)
  cout << "ISGE so far" << endl;
    
    posType pend = tlist.ConnectFirst();
if (AltListMulti)
{
  cout << "pend " << SDS_POS_NAMES[pend] << "\n";
  tlist.Print(cout);
  cout << endl;
}
    AltList::ConnectFirst(pend);
if (AltListMulti)
{
  AltList::Print(cout, "aRed");
  cout << endl;
  cout << "After connects\n";
}
  }

if (AltListMulti)
{
  cout << "AltR HIT\n";
  AltList::Print(cout, "aOrig");
  cout << "\n";
  tlist.Print(cout);
  cout << endl;
  // assert(false);
}

  return true;
}


bool AltList::FrontIsGE(
  TrickList& tlist)
{
  Trick htrick = tlist.GetFirstHeaderTrick();
if (AltListMulti)
{
  cout << "AL FIGE\n";
  htrick.Print(cout);
}
  if (htrick.GetEnd() == QT_BOTH)
  {
    htrick.SetEnd(QT_ACE);
    if (! AltList::FrontIsGE(htrick))
      return false;

    htrick.SetEnd(QT_PARD);
  }

  return AltList::FrontIsGE(htrick);
}


bool AltList::FrontIsGE(
  const Trick& trick)
{
  for (unsigned a = 0; a < len; a++)
  {
    if (list[a] >= trick)
      return true;
  }
  return false;
}


void AltList::ConnectFirst(
  const posType pend)
{
  for (unsigned a = 0; a < len; a++)
    list[a].ConnectFirst(pend);
}


bool AltList::FixRanks(
  const unsigned maxOppRank,
  const unsigned hrank)
{
  bool fixed = false;

  for (unsigned a = 0; a < len; a++)
  {
    if (list[a].FixRanks(maxOppRank, hrank))
      fixed = true;
  }

  if (! fixed)
    return false;

  headerDirty = true;

  AltList::Reduce();

  return fixed;
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
