/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iostream>
#include <assert.h>

#include "Header.h"
#include "Trick.h"

using namespace std;


extern unsigned headerStats[4];

Header::Header()
{
  dCum = 0;
  aCum = 0;
  sCum = 0;
  tCum = 0;
}


Header::~Header()
{
}


void Header::Set(
  const Trick& trick,
  unsigned tno)
{
  dCum = 1;
  aCum = 1;
  sCum = 1;
  tCum = tno;

  start = trick.GetStart();
  if (start == QT_ACE)
    startNew = 1;
  else if (start == QT_PARD)
    startNew = 2;
  else if (start == QT_BOTH)
    startNew = 4;
  else
    assert(false);

  end = trick.GetEnd();
  if (end == QT_ACE)
    endNew = 1;
  else if (end == QT_PARD)
    endNew = 2;
  else if (end == QT_BOTH)
    endNew = 4;
  else
    assert(false);

  maxTricks = trick.GetCashing();

  maxRanks = trick.GetRanks();
  minRanks = maxRanks;

  for (int h = 0; h < DDS_HANDS; h++)
  {
    cashTricks[h] = 0;
    cashRanks[h] = 0;
  }

  if (start == QT_ACE || start == QT_BOTH)
  {
    cashTricks[QT_ACE] = maxTricks;
    cashRanks[QT_ACE] = maxRanks;

    cashAceMin = maxTricks;
    cashAceMax = maxTricks;
    rankAceMin = maxRanks;
    rankAceMax = maxRanks;
  }
  else
  {
    cashAceMin = 0;
    cashAceMax = 0;
    rankAceMin = SDS_VOID;
    rankAceMax = 0;
  }

  if (start == QT_PARD || start == QT_BOTH)
  {
    cashTricks[QT_PARD] = maxTricks;
    cashRanks[QT_PARD] = maxRanks;

    cashPardMin = maxTricks;
    cashPardMax = maxTricks;
    rankPardMin = maxRanks;
    rankPardMax = maxRanks;
  }
  else
  {
    cashPardMin = 0;
    cashPardMax = 0;
    rankPardMin = SDS_VOID;
    rankPardMax = 0;
  }
}


void Header::Increase(
  const Trick& tLater,
  const unsigned tno)
{
  sCum++;
  tCum += tno;

  maxTricks += tLater.GetCashing();
  unsigned mr = tLater.GetRanks();
  if (mr < maxRanks)
    maxRanks = mr;
}


void Header::MergeMax(const Header& newHeader)
{
  aCum++;
  sCum += newHeader.sCum;
  tCum += newHeader.tCum;

  if (newHeader.start == QT_ACE)
    startNew |= 1;
  else if (newHeader.start == QT_PARD)
    startNew |= 2;
  else if (newHeader.start == QT_BOTH)
    startNew |= 4;
  else
    assert(false);

  if (newHeader.end == QT_ACE)
    endNew |= 1;
  else if (newHeader.end == QT_PARD)
    endNew |= 2;
  else if (newHeader.end == QT_BOTH)
    endNew |= 4;
  else
    assert(false);

  unsigned t = newHeader.maxTricks;
  cashAceMin = Min(cashAceMin, t);
  cashAceMax = Max(cashAceMax, t);
  cashPardMin = Min(cashPardMin, t);
  cashPardMax = Max(cashPardMax, t);

  unsigned r = newHeader.maxRanks;
  rankAceMin = Min(rankAceMin, r);
  rankAceMax = Max(rankAceMax, r);
  rankPardMin = Min(rankPardMin, r);
  rankPardMax = Max(rankPardMax, r);

  if (newHeader.maxTricks > maxTricks)
  {
    maxTricks = newHeader.maxTricks;

    // Irrelevant, really.
    start = newHeader.start;
    end = newHeader.end;
  }

  if (newHeader.maxRanks < maxRanks)
    maxRanks = newHeader.maxRanks;

  if (newHeader.minRanks < minRanks && newHeader.minRanks > 0)
    minRanks = newHeader.minRanks;
    
  for (int h = 0; h < DDS_HANDS; h++)
  {
    if (newHeader.cashTricks[h] > cashTricks[h])
    {
      cashTricks[h] = newHeader.cashTricks[h];
      cashRanks[h] = newHeader.cashRanks[h];
    }
    else if (newHeader.cashTricks[h] == cashTricks[h] &&
      newHeader.cashRanks[h] > cashRanks[h])
    {
      cashRanks[h] = newHeader.cashRanks[h];
    }
  }
}


void Header::MergeMin(const Header& newHeader)
{
  dCum++;
  aCum += newHeader.aCum;
  sCum += newHeader.sCum;
  tCum += newHeader.tCum;

  if (newHeader.start == QT_ACE)
    startNew |= 1;
  else if (newHeader.start == QT_PARD)
    startNew |= 2;
  else if (newHeader.start == QT_BOTH)
    startNew |= 4;
  else
    assert(false);

  if (newHeader.end == QT_ACE)
    endNew |= 1;
  else if (newHeader.end == QT_PARD)
    endNew |= 2;
  else if (newHeader.end == QT_BOTH)
    endNew |= 4;
  else
    assert(false);

  unsigned t = newHeader.maxTricks;
  cashAceMin = Min(cashAceMin, t);
  cashAceMax = Max(cashAceMax, t);
  cashPardMin = Min(cashPardMin, t);
  cashPardMax = Max(cashPardMax, t);

  unsigned r = newHeader.maxRanks;
  rankAceMin = Min(rankAceMin, r);
  rankAceMax = Max(rankAceMax, r);
  rankPardMin = Min(rankPardMin, r);
  rankPardMax = Max(rankPardMax, r);

  assert(aCum >= dCum);
  assert(sCum >= aCum);
  assert(tCum >= sCum);

  if (dCum > headerStats[0]) 
    headerStats[0] = dCum;
  if (aCum-dCum > headerStats[1]) 
    headerStats[1] = aCum-dCum;
  if (sCum-aCum > headerStats[2]) 
    headerStats[2] = sCum-aCum;
  if (tCum-sCum > headerStats[3]) 
    headerStats[3] = tCum-sCum;

  if (newHeader.maxTricks < maxTricks)
  {
    maxTricks = newHeader.maxTricks;

    // Irrelevant, really.
    start = newHeader.start;
    end = newHeader.end;
  }

  if (newHeader.maxRanks < maxRanks)
    maxRanks = newHeader.maxRanks;

  if (newHeader.minRanks < minRanks && newHeader.minRanks > 0)
    minRanks = newHeader.minRanks;
    
  for (int h = 0; h < DDS_HANDS; h++)
  {
    if (newHeader.cashTricks[h] < cashTricks[h])
    {
      cashTricks[h] = newHeader.cashTricks[h];
      cashRanks[h] = newHeader.cashRanks[h];
    }
    else if (newHeader.cashTricks[h] == cashTricks[h] &&
      newHeader.cashRanks[h] < cashRanks[h])
    {
      cashRanks[h] = newHeader.cashRanks[h];
    }
  }
}


void Header::GetAD(
  unsigned& d,
  unsigned& asum) const
{
  d = dCum;
  asum = aCum;
}


unsigned Header::CheckManual() const
{
  if (cashTricks[QT_ACE] == cashTricks[QT_PARD] &&
      maxTricks == cashTricks[QT_ACE] &&
      cashRanks[QT_ACE] == cashRanks[QT_PARD] &&
      maxRanks == cashRanks[QT_ACE])
    return maxTricks;
  else
    return 0;
}


unsigned Header::GetKeyNew() const
{
  return (dCum-1) + (aCum << 4);
}


int Header::GetTrickKey() const
{
  return (maxTricks << 8) | (cashTricks[QT_ACE] << 4) | cashTricks[QT_PARD];
}


int Header::GetRankKey() const
{
  return (minRanks << 12) | (maxRanks << 8) | 
         (cashRanks[QT_ACE] << 4) | cashRanks[QT_PARD];
}


unsigned Header::GetMaxRank() const
{
  return static_cast<unsigned>(maxRanks);
}


void Header::PrintKey(
  ostream& out,
  const int key) const
{
  out << "\n\nKey " <<
    setw(6) << key <<
    " (" <<
    "maxTricks " << static_cast<unsigned>(maxTricks) <<
    ", maxRank '" <<
    SDS_RANK_NAMES[maxRanks] <<
    "')\n";
}


void Header::Print(
  ostream& out,
  const bool skipMax) const
{
  if (! skipMax)
    out << "maxTricks " << static_cast<unsigned>(maxTricks) <<
      ", maxRanks '" << SDS_RANK_NAMES[maxRanks] <<
      "' start '" << POS_NAMES[start] <<
      "' reach '" << REACH_NAMES[posToReach[end]] << "'\n";

  out <<
    setw(10) << "cashTricks" <<
    setw(3) << static_cast<unsigned>(cashTricks[0]) <<
    setw(3) << static_cast<unsigned>(cashTricks[1]) <<
    setw(3) << static_cast<unsigned>(cashTricks[2]) <<
    setw(3) << static_cast<unsigned>(cashTricks[3]) << "\n";
  
  out <<
    setw(10) << "cashRanks " <<
    setw(3) << SDS_RANK_NAMES[cashRanks[0]] <<
    setw(3) << SDS_RANK_NAMES[cashRanks[1]] <<
    setw(3) << SDS_RANK_NAMES[cashRanks[2]] <<
    setw(3) << SDS_RANK_NAMES[cashRanks[3]] << "\n";
}

