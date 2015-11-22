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
  minRanks = 0;

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


cmpType Header::CompareFirstPlay(
  const Header& newHeader,
  const posType side) const
{
  // side can only be QT_ACE or QT_PARD here.
  assert(side != QT_BOTH);

  reachType oldReach = posToReach[end];
  reachType newReach = posToReach[newHeader.end];

  // Example: BP1K + PA2J, normal reach: A.
  // If played out starting from P, it is possible to make a P move
  // and then to use the B start to get back if needed.  So the
  // reach is actually B.  If played out from A, nothing happens.

  if (start == QT_BOTH)
    oldReach = reachMap[oldReach][side];

  if (newHeader.start == QT_BOTH)
    newReach = reachMap[newReach][side];

  cmpType runningScore = reachMatrix[oldReach][newReach];
  if (runningScore == SDS_DIFFERENT)
    return SDS_DIFFERENT;

  int oldCash = cashTricks[side];
  int newCash = newHeader.cashTricks[side];
  assert(oldCash > 0 && newCash > 0);

  cmpType trickScore;
  if (oldCash > newCash)
    trickScore = SDS_OLD_BETTER;
  else if (oldCash < newCash)
    trickScore = SDS_NEW_BETTER;
  else
    trickScore = SDS_SAME;

  return cmpMergeMatrix[runningScore][trickScore];
}


cmpType Header::ComparePlay(
  const Header& newHeader,
  const posType side) const
{
  cmpType runningScore = Header::CompareFirstPlay(newHeader, side);
  if (runningScore == SDS_DIFFERENT)
    return SDS_DIFFERENT;

  cmpType trickScore;
  if (maxTricks > newHeader.maxTricks)
    trickScore = SDS_OLD_BETTER;
  else if (maxTricks < newHeader.maxTricks)
    trickScore = SDS_NEW_BETTER;
  else
    trickScore = SDS_SAME;

  return cmpMergeMatrix[runningScore][trickScore];
}


cmpType Header::CompareRanks(
  const Header& newHeader,
  const posType side) const
{
  // side can only be QT_ACE or QT_PARD here.
  assert(side != QT_BOTH);

  if (cashRanks[side] > newHeader.cashRanks[side])
    return SDS_OLD_BETTER;
  else if (cashRanks[side] < newHeader.cashRanks[side])
    return SDS_NEW_BETTER;
  else if (maxRanks > newHeader.maxRanks)
    return SDS_OLD_BETTER;
  else if (maxRanks < newHeader.maxRanks)
    return SDS_NEW_BETTER;
  else
    return SDS_SAME;
}


cmpDetailType Header::Compare(
  const Header& newHeader) const
{
  // The highest priority consists of start, cashTricks and
  // range.  If there is equality, the second priority consists
  // of the ranks.

  posType oldStart = start;
  posType newStart = newHeader.start;

  // Use of the same matrix for a slightly different purpose:
  // The starting sides.

  cmpType sideScore = 
    reachMatrix[posToReach[oldStart]][posToReach[newStart]];
  cmpType runningScore = sideScore;
  if (runningScore == SDS_DIFFERENT)
    return SDS_HEADER_PLAY_DIFFERENT;

  cmpType playScore;
  if (sideScore == SDS_OLD_BETTER)
    playScore = Header::ComparePlay(newHeader, newStart);
  else if (sideScore == SDS_NEW_BETTER)
    playScore = Header::ComparePlay(newHeader, oldStart);
  else if (oldStart != QT_BOTH)
    playScore = Header::ComparePlay(newHeader, oldStart);
  else
  {
    cmpType playScore1 = Header::ComparePlay(newHeader, QT_ACE);
    cmpType playScore2 = Header::ComparePlay(newHeader, QT_PARD);
    playScore = cmpMergeMatrix[playScore1][playScore2];
  }

  if (playScore == SDS_DIFFERENT)
    return SDS_HEADER_PLAY_DIFFERENT;

  runningScore = cmpMergeMatrix[runningScore][playScore];
  if (runningScore != SDS_SAME)
    return cmpPlayToDetail[runningScore];

  cmpType rankScore;
  if (sideScore == SDS_OLD_BETTER)
    rankScore = Header::CompareRanks(newHeader, newStart);
  else if (sideScore == SDS_NEW_BETTER)
    rankScore = Header::CompareRanks(newHeader, oldStart);
  else if (oldStart != QT_BOTH)
    rankScore = Header::CompareRanks(newHeader, oldStart);
  else
  {
    cmpType rankScore1 = Header::CompareRanks(newHeader, QT_ACE);
    cmpType rankScore2 = Header::CompareRanks(newHeader, QT_PARD);
    rankScore = cmpMergeMatrix[rankScore1][rankScore2];
  }
    
  return cmpRanksToDetail[rankScore];
}


bool Header::EqualsExceptStart(
  const Header& newHeader) const
{
  // Must be different!
  if (start == newHeader.start ||
      start == QT_BOTH ||
      newHeader.start == QT_BOTH)
    return false;

  posType sideOld = (cashTricks[QT_ACE] ? QT_ACE : QT_PARD);
  posType sideNew = (newHeader.cashTricks[QT_ACE] ? QT_ACE : QT_PARD);
  assert(sideOld != sideNew);

  if (maxTricks != newHeader.maxTricks ||
      maxRanks != newHeader.maxRanks ||
      end != newHeader.end)
    return false;

  if (cashTricks[sideOld] != newHeader.cashTricks[sideNew] ||
      cashRanks[sideOld] != newHeader.cashRanks[sideNew])
    return false;
  
  return true;
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

