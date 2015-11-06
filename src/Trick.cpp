/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <assert.h>

#include "cst.h"
#include "Trick.h"


Trick::Trick()
{
  Trick::Reset();
}


Trick::~Trick()
{
}


void Trick::Reset()
{
}


void Trick::Set(
  const posType start,
  const posType end,
  const unsigned ranks,
  const unsigned cashing)
{
  trick.start = start;
  trick.end = end;
  trick.ranks = ranks;
  trick.cashing = cashing;
}


void Trick::SetStart(
  const posType start)
{
  trick.start = start;
}


void Trick::SetEnd(
  const posType end)
{
  trick.end = end;
}


void Trick::SetRanks(
  const unsigned ranks)
{
  trick.ranks = ranks;
}


posType Trick::GetStart() const
{
  return trick.start;
}


posType Trick::GetEnd() const
{
  return trick.end;
}


unsigned Trick::GetCashing() const
{
  return trick.cashing;
}


unsigned Trick::GetRanks() const
{
  return trick.ranks;
}


bool Trick::operator == (
  const Trick& t2) const
{
  return (t2.trick.start == trick.start &&
          t2.trick.end == trick.end &&
          t2.trick.cashing == trick.cashing &&
          t2.trick.ranks == trick.ranks);
}


bool Trick::operator != (
  const Trick& t2) const
{
  return ! (* this == t2);
}


bool Trick::Extends(
  const Trick& tEarlier) const
{
  if (tEarlier.trick.end != QT_BOTH &&
      trick.start != QT_BOTH &&
      trick.start != tEarlier.trick.end)
    return false;
  else
    return true;
}


void Trick::Localize(
  const Holding& holding)
{
  // When a trick list is looked up, its parameters are relative
  // to its circumstances.  When a trick is appended to that list,
  // the trick has to be updated.

  if (holding.GetAceFlip())
  {
    if (trick.start != QT_BOTH)
      trick.start = SDS_PARTNER[trick.start];

    if (trick.end != QT_BOTH)
      trick.end = SDS_PARTNER[trick.end];
  }

  trick.ranks = holding.GetNewRank(trick.ranks);
}


appendType Trick::Prepend(
  const Trick& tPrep,
  const bool mergeSpecialFlag,
  const bool lastFlag)
{
  /*      AA AP AB   PA PP PB   BA BP BB (this)
        +-------------------------------
     AA |  m  m  B    -  -  -    m sb  B
     AP |  -  -  -    s  m  B   sb  m  B
     AB |  m  m  B    s  m  B   sb sb  B
        |
     PA |  m  s  B    -  -  -    m sb  B
     PP |  -  -  -    m  m  B   sb  m  B
     PB |  m  s  B    m  m  B   sb sb  B
        |
     BA |  m  s  B    -  -  -    m sb  B
     BP |  -  -  -    s  m  B   sb  m  B
     BB |  m  s  B    s  m  B   sb sb  B
     (tPrep)

     B = Merges to end on B in any case.
     b = Only merges to end on B if lastFlag is set.
     s = Same segment but does not merge.
     m = Merges normally (first start to last end).
     - = Does not merge (with possible exception).
  */

  // Single case of combining tricks that don't match up.
  if (mergeSpecialFlag &&
      lastFlag &&
      tPrep.trick.start != QT_BOTH &&
      tPrep.trick.end == QT_ACE &&
      trick.start == QT_PARD &&
      trick.end == QT_ACE)
  {
    // (A|P)A + PA = (A|P)A with maximum rank.
    trick.start = tPrep.trick.start;
    trick.ranks = Max(tPrep.trick.ranks, trick.ranks);
    trick.cashing += tPrep.trick.cashing;
    return SDS_APPEND_MERGED;
  }

  if (! Trick::Extends(tPrep))
    return SDS_APPEND_NEW_SEG;

  // The ending side is generally trick.end.
  // This gets overridden to QT_BOTH in some lastFlag cases.
  // This in turn gets overridden in some special void cases below.
  posType newEnd;

  if (lastFlag &&
      trick.start == QT_BOTH &&
      tPrep.trick.end != trick.end)
  {
    // Special cases where we deviate from trick.end:
    // AA+BP, AP+BA, AB+BA, AB+BP, PA+BP, PP+BA, PB+BA, PB+BP,
    // BA+BP, BP+BA, BB+BA, BB+BP.
    newEnd = QT_BOTH;
  }
  else
    newEnd = trick.end;

  if (mergeSpecialFlag && lastFlag)
  {
    if (trick.end == QT_BOTH ||
       (tPrep.trick.start == QT_PARD && newEnd == QT_BOTH))
    {
      // Anything ending on B; PA+BP, PP+BA, PB+BA, PB+BP.
      // Rather special case.
      trick.start = QT_BOTH;
      trick.end = QT_ACE;
      trick.ranks = Max(tPrep.trick.ranks, trick.ranks);
      trick.cashing += tPrep.trick.cashing;
      return SDS_APPEND_MERGED;
    }
    else if (tPrep.trick.end == trick.end)
    {
      // AA+AA, AA+BA, AP+PP, AP+BP, 
      // PA+AA, PA+BA, PP+PP, PP+BP,
      // BA+AA, BA+BA, BP+PP, BP+BP: Use maximum rank.
      trick.start = tPrep.trick.start;
      trick.ranks = Max(tPrep.trick.ranks, trick.ranks);
      trick.cashing += tPrep.trick.cashing;
      return SDS_APPEND_MERGED;
    }
  }

  // Now we are down to regular prepends.
  if (trick.start != QT_BOTH &&
      trick.end == SDS_PARTNER[trick.start] &&
      tPrep.trick.start != trick.start)
  {
    // PA+AP, PB+AP, BA+AP, BB+AP; AP+PA, AB+PA, BP+PA, BB+PA.
    return SDS_APPEND_SAME_SEG;
  }
  else if (newEnd == QT_BOTH ||
      trick.start != QT_BOTH ||
      tPrep.trick.end == trick.end ||
      (tPrep.trick.end == QT_BOTH && tPrep.trick.start != trick.end))

      // tPrep.trick.start == tPrep.trick.end ||
      // tPrep.trick.start == SDS_PARTNER[trick.end] ||
      // tPrep.trick.end == trick.end)
  {
    trick.start = tPrep.trick.start;
    trick.end = newEnd;
    trick.ranks = Min(tPrep.trick.ranks, trick.ranks);
    trick.cashing += tPrep.trick.cashing;
    return SDS_APPEND_MERGED;
  }
  else
  {
    // AP+BA, AB+BA, PA+BP, PB+BP, BA+BP, BP+BA without lastFlag.
    return SDS_APPEND_SAME_SEG;
  }
}


bool Trick::ReduceBoth(
  Trick& t1,
  const bool lastSegFlag)
{
  if (t1.trick.start == QT_BOTH &&
    t1.trick.end != QT_BOTH &&
    t1.trick.end == trick.start &&
    trick.end == SDS_PARTNER[t1.trick.end])
  {
    // BA+AP = BP, BP+PA = BA.
    trick.start = QT_BOTH;
    trick.cashing += t1.trick.cashing;
    trick.ranks = Min(trick.ranks, t1.trick.ranks);
    return true;
  }
  else
    return false;
}


bool Trick::operator >= (
  const Trick& t1)
{
  assert(trick.start != SDS_PARTNER[t1.trick.start]);

  if (t1.trick.end == QT_BOTH && trick.end != QT_BOTH)
    return false;
  else if (t1.trick.end != QT_BOTH && trick.end == SDS_PARTNER[t1.trick.end])
    return false;
  else if (t1.trick.cashing > trick.cashing)
    return false;
  else if (t1.trick.cashing < trick.cashing)
    return true;
  else if (t1.trick.ranks <= trick.ranks)
    return true;
  else
    return false;
}



cmpType Trick::CashRankOrder(
  const unsigned c,
  const unsigned r)
{
  if (trick.cashing > c)
    return SDS_OLD_BETTER;
  else if (trick.cashing < c)
    return SDS_NEW_BETTER;
  else if (trick.ranks > r)
    return SDS_OLD_BETTER;
  else if (trick.ranks < r)
    return SDS_NEW_BETTER;
  else
    return SDS_SAME;
}


void Trick::Print(
  std::ostream& out) const
{
  out <<
    setw(6) << SDS_POS_NAMES[trick.start] <<
    setw(6) << SDS_POS_NAMES[trick.end] <<
    setw(6) << static_cast<unsigned>(trick.cashing) <<
    setw(8) << SDS_RANK_NAMES[trick.ranks] << "\n";
}


void Trick::Print(
  std::ostream& out,
  const unsigned d,
  const unsigned a,
  const unsigned p) const
{
  out <<
    setw(4) << d <<
    setw(6) << a <<
    setw(6) << p;

  Trick::Print(out);
}

