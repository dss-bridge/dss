/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <assert.h>

#include "cst.h"
#include "TrickList.h"

using namespace std;



TrickList::TrickList()
{
  TrickList::Reset();
}


TrickList::~TrickList()
{
}


void TrickList::Reset()
{
  len = 0;
}


bool TrickList::Set1(
  const Trick& trick)
{
  len = 1;
  list[0].Set1(trick);
  return true;
}


bool TrickList::Set2(
  const Trick& trick1,
  const Trick& trick2)
{
  if (trick2.Extends(trick1))
  {
    list[0].Set2(trick1, trick2);
    len = 1;
  }
  else
  {
    list[1].Set1(trick1);
    list[0].Set1(trick2);
    len = 2;
  }
  return true;
}


void TrickList::SetStart(
  const posType start)
{
  assert(len > 0);
  list[len-1].SetStart(start);
}


posType TrickList::GetFirstStart() const
{
  assert(len > 0);
  return list[len-1].GetStart();
}


posType TrickList::GetFirstEnd() const
{
  assert(len > 0);
  return list[len-1].GetEnd();
}


unsigned int TrickList::GetLength() const
{
  return len;
}


const void TrickList::GetHeader(
  Header& header,
  const unsigned startNo) const
{
  assert(len > startNo);

  Trick t;
  list[len-1-startNo].GetSummaryTrick(t);
  header.Set(t, list[len-1-startNo].GetLength());

  for (unsigned l = startNo+1; l < len; l++)
  {
    list[len-1-l].GetSummaryTrick(t);
    header.Increase(t, list[len-1-l].GetLength());
  }
}


void TrickList::GetFirstSummaryTrick(
  Trick& t) const
{
  assert(len > 0);
  list[len-1].GetSummaryTrick(t);
}


cmpDetailType TrickList::CompareInit(
  const TrickList& lNew,
  CompareStruct cdata) const
{
  cmpDetailType cc;
  do
  {
    if (cdata.lenOld == 0 && cdata.lenNew == 0)
      return SDS_HEADER_SAME;
    else if (cdata.lenOld == 0)
      return SDS_HEADER_PLAY_NEW_BETTER;
    else if (cdata.lenNew == 0)
      return SDS_HEADER_PLAY_OLD_BETTER;

    cdata.lenOld--;
    cdata.lenNew--;

    cc = list[cdata.lenOld].Compare(lNew.list[cdata.lenNew]);
    if (cc == SDS_HEADER_PLAY_DIFFERENT)
      return SDS_HEADER_PLAY_DIFFERENT;

    cdata.winnerFirst = cmpDetailMatrix[cdata.winnerFirst][cc];
    assert(cdata.winnerFirst != SDS_HEADER_PLAY_DIFFERENT);

    Trick t1;
    list[cdata.lenOld].GetSummaryTrick(t1);
    cdata.tricksOld += t1.GetCashing();
    unsigned r1 = t1.GetRanks();
    cdata.ranksOld = Min(cdata.ranksOld, r1);

    Trick t2;
    lNew.list[cdata.lenNew].GetSummaryTrick(t2);
    cdata.tricksNew += t2.GetCashing();
    unsigned r2 = t2.GetRanks();
    cdata.ranksNew = Min(cdata.ranksNew, r2);
  
    cdata.winnerRunning = TrickList::CompareRunning(cdata);
  }
  while (cc != SDS_HEADER_PLAY_NEW_BETTER &&
      cc != SDS_HEADER_PLAY_OLD_BETTER);

  return cc;
}


cmpDetailType TrickList::Compare(
  const TrickList& lNew) const
{
  assert(len > 0);
  assert(lNew.len > 0);

  CompareStruct cdata;
  cdata.lenOld = len;
  cdata.lenNew = lNew.len;
  cdata.tricksOld = 0;
  cdata.tricksNew = 0;
  cdata.ranksOld = SDS_VOID;
  cdata.ranksNew = SDS_VOID;
  cdata.winnerFirst = SDS_HEADER_SAME;
  cdata.winnerRunning = SDS_HEADER_SAME;

  cmpDetailType cc = TrickList::CompareInit(lNew, cdata);
  cdata.winnerFirst = cc;

  if (cc == SDS_HEADER_PLAY_DIFFERENT)
    return SDS_HEADER_PLAY_DIFFERENT;
  else
  {
    cdata.winnerRunning = cc;
    return TrickList::CompareTail(lNew, cdata);
  }
}


cmpDetailType TrickList::CompareRunning(
  const CompareStruct cdata) const
{
  if (cdata.tricksOld > cdata.tricksNew)
    return SDS_HEADER_PLAY_OLD_BETTER;
  else if (cdata.tricksOld < cdata.tricksNew)
    return SDS_HEADER_PLAY_NEW_BETTER;
  else if (cdata.ranksOld > cdata.ranksNew)
    return SDS_HEADER_RANK_OLD_BETTER;
  else if (cdata.ranksOld < cdata.ranksNew)
    return SDS_HEADER_RANK_NEW_BETTER;
  else
    return SDS_HEADER_SAME;
}


cmpDetailType TrickList::CompareTail(
  const TrickList& lNew,
  CompareStruct cdata) const
{
  if (cdata.lenOld == 0 && cdata.lenNew == 0)
  {
    return cmpDetailMatrix[cdata.winnerFirst][cdata.winnerRunning];
  }
  else if (cdata.lenOld == 0)
  {
    cmpDetailType c = cdata.winnerRunning;
    if (c == SDS_HEADER_SAME ||
        c == SDS_HEADER_PLAY_NEW_BETTER ||
        c == SDS_HEADER_RANK_NEW_BETTER)
    {
      return cmpDetailMatrix[cdata.winnerFirst][SDS_HEADER_PLAY_NEW_BETTER];
    }
  }
  else if (cdata.lenNew == 0)
  {
    cmpDetailType c = cdata.winnerRunning;
    if (c == SDS_HEADER_SAME ||
        c == SDS_HEADER_PLAY_OLD_BETTER ||
        c == SDS_HEADER_RANK_OLD_BETTER)
    {
      return cmpDetailMatrix[cdata.winnerFirst][SDS_HEADER_PLAY_OLD_BETTER];
    }
  }

  if (cdata.lenOld > 0)
  {
    Trick t;
    list[cdata.lenOld-1].GetSummaryTrick(t);
    cdata.tricksOld += t.GetCashing();
    cdata.ranksOld = Min(cdata.ranksOld, t.GetRanks());
    cdata.lenOld--;
  }

  if (cdata.lenNew > 0)
  {
    Trick t;
    lNew.list[cdata.lenNew-1].GetSummaryTrick(t);
    cdata.tricksNew += t.GetCashing();
    cdata.ranksNew = Min(cdata.ranksNew, t.GetRanks());
    cdata.lenNew--;
  }

  cmpDetailType d = TrickList::CompareRunning(cdata);
  cdata.winnerRunning = cmpDetailMatrix[cdata.winnerRunning][d];

  cmpDetailType c = TrickList::CompareTail(lNew, cdata);
  return cmpDetailMatrix[cdata.winnerFirst][c];
}


bool TrickList::EqualsExceptStart(
  const TrickList& lNew) const
{
  assert(len > 0 && lNew.len > 0);

  if (len != lNew.len)
    return false;
  else if (! list[len-1].EqualsExceptStart(lNew.list[lNew.len-1]))
    return false;
  else
  {
    int p = 1;
    while (p <= len-1)
    {
      if (list[len-1-p] != lNew.list[lNew.len-1-p])
        return false;
      p++;
    }
  }

  return true;
}


bool TrickList::operator == (
  const TrickList& t2) const
{
  if (t2.len != len)
    return false;

  for (unsigned p = 0; p < len; p++)
    if (list[p] != t2.list[p])
      return false;

  return true;
}


bool TrickList::operator != (
  const TrickList& t2) const
{
  return ! (* this == t2);
}


void TrickList::operator += (
  const Holding& holding)
{
  // Add a move in front of the first segment.

  assert(len > 0);
  for (unsigned p = 0; p < len; p++)
    list[p].Localize(holding);

  if (list[len-1].Prepend(holding, len == 1))
  {
    if (list[len-1].GetLength() == 2 &&
        holding.GetLength(QT_PARD) == 2 &&
        holding.GetLength(QT_RHO) >= 2 &&
        holding.LHOIsVoid() && 
        holding.IsAATrick())
    {
      // Essentially AKx / - / Qx / xx.  Conservative.
      // Never start with AA.
      TrickList::Set1(holding.GetTrick());
    }
    else
    {
      // Normal case.  It can happen that the prepending trick
      // has the same rank as a later segment, as we sometimes
      // take the rank below the real one.
      for (unsigned p = 1; p < len; p++)
      {
        if (list[len-1-p].FixRanks(holding.GetPrependRank()))
          break;
      }
    }
  }
  else if (holding.IsAATrick() &&
      list[len-1].GetRanks() > holding.GetLHOMaxRank() &&
      holding.GetMinDeclLength() >= 2)
  {
    // Rather special case: AA1A + Pxyz, where LHO is either void
    // or is below the first rank to win the next trick.
    // There is no reason ever to start from A if there is a PA move
    // available.  If there isn't, we'll just have to cash from A.
    // So we can always consider this to be AA1A.
    TrickList::Set1(holding.GetTrick());
  }
  else
  {
    // Didn't fit, so make a new segment.
    assert(len < TRICKLIST_MAXSEGS);
    len++;
    list[len-1].Set1(holding.GetTrick());
  }
}


cmpDetailType TrickList::CompareToTrick(
  const Trick& trick) const
{
  assert(len > 0);
  Trick ltrick;
  TrickList::GetFirstSummaryTrick(ltrick);
  return ltrick.Compare(trick);
}


cmpDetailType TrickList::FixOrCompare(
  TrickList& lOther,
  fixType& fix1,
  fixType& fix2)
{
  if (TrickList::Fix(lOther, fix1, fix2))
    return SDS_HEADER_PLAY_DIFFERENT;

  cmpDetailType c;
  switch (c = TrickList::Compare(lOther))
  {
    case SDS_HEADER_PLAY_OLD_BETTER:
    case SDS_HEADER_RANK_OLD_BETTER:
    case SDS_HEADER_SAME:
      fix1 = SDS_FIX_UNCHANGED;
      fix2 = SDS_FIX_PURGED;
      break;

    case SDS_HEADER_PLAY_NEW_BETTER:
    case SDS_HEADER_RANK_NEW_BETTER:
      fix1 = SDS_FIX_PURGED;
      fix2 = SDS_FIX_UNCHANGED;
      break;

    default:
      break;
  }

  return c;
}


bool TrickList::Fix(
  TrickList& lOther,
  fixType& fix1,
  fixType& fix2)
{
  fix1 = SDS_FIX_UNCHANGED;
  fix2 = SDS_FIX_UNCHANGED;

  if (TrickList::EqualsExceptStart(lOther))
  {
    list[len-1].SetStart(QT_BOTH);
    fix1 = SDS_FIX_STRONGER;
    fix2 = SDS_FIX_PURGED;
  }
  else if (len == 1 && lOther.len == 1)
  {
    if (list[0].Fix11(lOther.list[0], fix1, fix2))
    {
      if (fix1 == SDS_FIX_SPLIT)
      {
        TrickList::Split();
        fix1 = SDS_FIX_WEAKER;
      }

      if (fix2 == SDS_FIX_SPLIT)
      {
        lOther.Split();
        fix2 = SDS_FIX_WEAKER;
      }
    }
  }
  else if (len == 1 && lOther.len == 2)
  {
    if (list[0].Fix12(lOther.list[1], lOther.list[0], fix1, fix2))
    {
      if (fix2 == SDS_FIX_COLLAPSE)
      {
        lOther.list[0] = lOther.list[1];
        lOther.len = 1;
        fix2 = SDS_FIX_WEAKER;
      }
    }
  }
  else if (len == 2 && lOther.len == 1)
  {
    if (lOther.list[0].Fix12(list[1], list[0], fix2, fix1))
    {
      if (fix1 == SDS_FIX_COLLAPSE)
      {
        list[0] = list[1];
        len = 1;
        fix1 = SDS_FIX_WEAKER;
      }
    }
  }
  else if (len == 1 && lOther.len >= 3)
    list[0].Fix1n(lOther.list[lOther.len-1], fix1, fix2);
  else if (len >= 3 && lOther.len == 1)
    lOther.list[0].Fix1n(list[len-1], fix2, fix1);

  if (fix1 == SDS_FIX_UNCHANGED && fix2 == SDS_FIX_UNCHANGED)
    return false;

  return true;
}


void TrickList::Split()
{
  len = 2;
  list[1] = list[0];
  list[1].PunchOut(1);
  list[0].PunchOut(0);
}


posType TrickList::ConnectFirst(
  const posType pend)
{
  if (TrickList::GetFirstEnd() != pend)
    return QT_LHO; // Anything
  
  return TrickList::ConnectFirst();
}


posType TrickList::ConnectFirst()
{
  assert(len > 0);
  if (len == 1)
  {
    // Must be a connector to the other side.
    list[0].SetEnd(QT_BOTH);
    return QT_BOTH;
  }
  
  len--;
  return list[len-1].Connect(list[len]);
}


void TrickList::Print(
  ostream& out,
  const unsigned d,
  const unsigned a) const
{
  unsigned offset = 0;
  for (unsigned p = 0; p < len; p++)
    list[len-1-p].Print(out, d, a, offset);
}

