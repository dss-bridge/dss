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


Header::Header()
{
}


Header::~Header()
{
}


void Header::Set(
  const Trick& trick)
{
  start = trick.GetStart();
  end = trick.GetEnd();
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
  }

  if (start == QT_PARD || start == QT_BOTH)
  {
    cashTricks[QT_PARD] = maxTricks;
    cashRanks[QT_PARD] = maxRanks;
  }
}


void Header::Increase(
  const Trick& tLater)
{
  maxTricks += tLater.GetCashing();
  unsigned mr = tLater.GetRanks();
  if (mr < maxRanks)
    maxRanks = mr;
}


void Header::MergeMax(const Header& newHeader)
{
  if (newHeader.maxTricks > maxTricks)
  {
    maxTricks = newHeader.maxTricks;
    maxRanks = newHeader.maxRanks;

    // Irrelevant, really.
    start = newHeader.start;
    end = newHeader.end;
  }

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
  if (newHeader.maxTricks < maxTricks)
  {
    maxTricks = newHeader.maxTricks;
    // maxRanks = newHeader.maxRanks;

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


void Header::SetKeyNew(
  const unsigned k)
{
  keyNew = k;
}


unsigned Header::GetKeyNew() const
{
  return keyNew;
}


int Header::GetKey() const
{
  return (maxTricks << 7) | (maxRanks << 3) | start;
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

