/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

#include <assert.h>

#include "cst.h"
#include "Header.h"
#include "Trick.h"


Header::Header()
{
}


Header::~Header()
{
}


void Header::Set(
  const int maxTricksArg,
  const unsigned short maxRanksArg,
  const int cashTricksArg[],
  const unsigned short cashRanksArg[],
  const posType startArg,
  const reachType reachArg)
{
  this->maxTricks = maxTricksArg;
  this->maxRanks = maxRanksArg;
  this->minRanks = maxRanksArg;

  for (int h = 0; h < DDS_HANDS; h++)
  {
    this->cashTricks[h] = cashTricksArg[h];
    this->cashRanks[h] = cashRanksArg[h];
  }

  this->start = startArg;
  this->reach = reachArg;
}


void Header::SetWithTrick(
  const Trick& tr)
{
  maxTricks = tr.GetCashing();
  maxRanks =  tr.GetRanks();

  start = tr.GetStart();
  posType end = tr.GetEnd();

  if (end == QT_BOTH)
    reach = SDS_SIDE_BOTH;
  else if (end == QT_ACE)
    reach = SDS_SIDE_ACE;
  else if (end == QT_PARD)
    reach = SDS_SIDE_PARD;
  else
    assert(false);

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


void Header::Increase(const Header& hLater)
{
  this->maxTricks += hLater.maxTricks;
  this->maxRanks = Min(this->maxRanks, hLater.maxRanks);
}


cmpType Header::CompareSide(
  const Header& newHeader,
  const posType side)
{
  // This is a partial comparison function.  It only considers
  // cashTricks and cashRanks for a specific side.
  // Actually by now it's more complex.

  int oldCash = cashTricks[side];
  int newCash = newHeader.cashTricks[side];

  if ((oldCash == 0 && newCash > 0) ||
      (oldCash > 0 && newCash == 0))
  {
    cout << "Header::CompareSide:  Only one list cashes (" <<
      static_cast<int>(side) << ").\n";
    cout << "Old header\n";
    Header::Print();
    cout << "New header\n";
    newHeader.Print();
    cout.flush();
    assert(false);
  }

  int pointsOld = 0, pointsNew = 0;
  if (oldCash > newCash)
    pointsOld++;
  else if (oldCash < newCash)
    pointsNew++;
  else if (cashRanks[side] > newHeader.cashRanks[side])
    pointsOld++;
  else if (cashRanks[side] < newHeader.cashRanks[side])
    pointsNew++;

  if (pointsNew)
  {
    return (maxTricks > newHeader.maxTricks ? SDS_DIFFERENT : SDS_NEW_BETTER);
  }
  else if (pointsOld)
  {
    return (maxTricks < newHeader.maxTricks ? SDS_DIFFERENT : SDS_OLD_BETTER);
  }
  else if (maxTricks > newHeader.maxTricks)
    return SDS_OLD_BETTER;
  else if (maxTricks < newHeader.maxTricks)
    return SDS_NEW_BETTER;
  else if (maxRanks > newHeader.maxRanks)
    return SDS_OLD_BETTER;
  else if (maxRanks < newHeader.maxRanks)
    return SDS_NEW_BETTER;
  else
    return SDS_SAME;
}


cmpType Header::CompareReach(const Header& newHeader)
{
  // This is a partial comparison function.  It only considers
  // the sides reached by the trick list.

  reachType oldReach = reach;
  reachType newReach = newHeader.reach;

  // Special case, e.g.
  // 1. BP1K + PA2J vs.
  // 2. PA1J + BP1- + AA1-.
  // #1 is played out starting at P and is considered to have
  // a range of A.  But because it starts with B, it is possible
  // to start at P with another move, and then to use the B.
  // So #1 in this context also gets a range of P, for a total
  // of B.

  posType newStart = newHeader.start;
  if (start == QT_BOTH && newStart != QT_BOTH)
    oldReach = reachMap[oldReach][newStart];
  else if (start != QT_BOTH && newStart == QT_BOTH)
    newReach = reachMap[newReach][start];

  return reachMatrix[oldReach][newReach];
}


cmpType Header::Compare(const Header& newHeader)
{
  /*
     There is no natural order of comparison for two trick lists
     (same, old better, new better, different).  Here we use the
     following rules.

     * If the lists don't start the same place, they're different.
     * If the lists can start from QT_ACE, there's a point for
       being better in that regard.
     * Similarly for QT_PARD.
     * Similarly for the reach (hands reached during play).

     If there are no points given out, then the lists are the same.
     If only one list gets points, it wins.
     If both lists get points, they are different.
  */

  int pointsOld = 0, pointsNew = 0;

  posType oldStart = start;
  posType newStart = newHeader.start;

  if (oldStart != newStart)
  {
    if (oldStart == QT_BOTH)
      pointsOld++;
    else if (newStart == QT_BOTH)
      pointsNew++;
    else
      return SDS_DIFFERENT;
  }
  
  if (oldStart == QT_ACE || (oldStart == QT_BOTH && newStart != QT_PARD))
  {
    int p = Header::CompareSide(newHeader, QT_ACE);
    if (p == SDS_OLD_BETTER)
      pointsOld++;
    else if (p == SDS_NEW_BETTER)
      pointsNew++;
    else if (p == SDS_DIFFERENT)
      return SDS_DIFFERENT;
  }

  if (oldStart == QT_PARD || (oldStart == QT_BOTH && newStart != QT_ACE))
  {
    int p = Header::CompareSide(newHeader, QT_PARD);
    if (p == SDS_OLD_BETTER)
      pointsOld++;
    else if (p == SDS_NEW_BETTER)
      pointsNew++;
    else if (p == SDS_DIFFERENT)
      return SDS_DIFFERENT;
  }

  if (pointsOld > 0 && pointsNew > 0)
    return SDS_DIFFERENT;

  int p = Header::CompareReach(newHeader);
  if (p == SDS_OLD_BETTER)
    pointsOld++;
  else if (p == SDS_NEW_BETTER)
      pointsNew++;
  else if (p == SDS_DIFFERENT)
    return SDS_DIFFERENT;

  if (pointsOld > 0 && pointsNew > 0)
    return SDS_DIFFERENT;
  else if (pointsOld == 0 && pointsNew == 0)
  {
    if (maxTricks == newHeader.maxTricks)
    {
      if (maxRanks > newHeader.maxRanks)
        return SDS_OLD_BETTER;
      else if (maxRanks < newHeader.maxRanks)
        return SDS_NEW_BETTER;
      else
        return SDS_SAME;
    }
    else if (maxTricks > newHeader.maxTricks)
      return SDS_OLD_BETTER;
    else
      return SDS_NEW_BETTER;
  }
  else if (pointsOld > 0)
  {
    if (maxTricks >= newHeader.maxTricks)
      return SDS_OLD_BETTER;
    else
      return SDS_DIFFERENT;
  }
  else if (maxTricks <= newHeader.maxTricks)
    return SDS_NEW_BETTER;
  else
    return SDS_DIFFERENT;
}


cmpType Header::CompareFirstPlay(
  const Header& newHeader,
  const posType side)
{
  // side can only be QT_ACE or QT_PARD here.
if (side != QT_ACE && side != QT_PARD)
{
  Header::Print();
  newHeader.Print();
  cout.flush();
}
  assert(side != QT_BOTH);

  reachType oldReach = reach;
  reachType newReach = newHeader.reach;

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

  if ((oldCash == 0 && newCash > 0) ||
      (oldCash > 0 && newCash == 0))
  {
    cout << "Header::ComparePlay:  Only one list cashes (" <<
      static_cast<int>(side) << ").\n";
    cout << "Old header\n";
    Header::Print();
    cout << "New header\n";
    newHeader.Print();
    cout.flush();
    assert(false);
  }

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
  const posType side)
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


cmpType Header::CompareFirstAll(
  const Header& newHeader,
  const posType side)
{
assert(false);
  cmpType runningScore = Header::CompareFirstPlay(newHeader, side);
  if (runningScore != SDS_SAME)
    return runningScore;

  if (cashRanks[side] > newHeader.cashRanks[side])
    return SDS_OLD_BETTER;
  else if (cashRanks[side] < newHeader.cashRanks[side])
    return SDS_NEW_BETTER;
  else
    return SDS_SAME;
}


cmpType Header::CompareRanks(
  const Header& newHeader,
  const posType side)
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


cmpDetailType Header::CompareDetail(const Header& newHeader)
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
  // else if (sideScore != QT_BOTH)
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


void Header::MergeMax(const Header& newHeader)
{
  if (newHeader.maxTricks > maxTricks)
  {
    maxTricks = newHeader.maxTricks;
    maxRanks = newHeader.maxRanks;

    // Irrelevant, really.
    start = newHeader.start;
    reach = newHeader.reach;
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
    reach = newHeader.reach;
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


bool Header::EqualsExceptPerhapsStart(
  const Header& newHeader,
  bool skipFlag)
{
  bool bothOld = false, bothNew = false;
  bool emptyOld = true, emptyNew = true;
  posType sideOld = QT_ACE, sideNew = QT_ACE; // Keeps g++ happy

  // Must be different!
  if (skipFlag && 
      (start == newHeader.start ||
      start == QT_BOTH ||
      newHeader.start == QT_BOTH))
    return false;

  if (cashTricks[QT_ACE])
  {
    emptyOld = false;
    if (cashTricks[QT_PARD])
      bothOld = true;
    else
      sideOld = QT_ACE;
  }
  else if (cashTricks[QT_PARD])
  {
    emptyOld = false;
    sideOld = QT_PARD;
  }

  if (newHeader.cashTricks[QT_ACE])
  {
    emptyNew = false;
    if (newHeader.cashTricks[QT_PARD])
      bothNew = true;
    else
      sideNew = QT_ACE;
  }
  else if (newHeader.cashTricks[QT_PARD])
  {
    emptyNew = false;
    sideNew = QT_PARD;
  }

  if (emptyOld && emptyNew)
    return true;

  if (emptyOld || emptyNew)
    return false;

  if (bothOld && bothNew)
    return (Header::Compare(newHeader) == SDS_SAME ? true : false);
  
  if (bothOld || bothNew)
    return false;

  if (sideOld == sideNew)
    return (Header::Compare(newHeader) == SDS_SAME ? true : false);
  
  // Is this the right place for it?
  if (! skipFlag)
    return false;
  
  if (maxTricks != newHeader.maxTricks)
    return false;

  if (maxRanks != newHeader.maxRanks)
    return false;
  
  if (cashTricks[sideOld] != newHeader.cashTricks[sideNew])
    return false;
  
  if (cashRanks[sideOld] != newHeader.cashRanks[sideNew])
    return false;
  
  if (reach != newHeader.reach)
    return false;

  return true;
}


int Header::GetKey()
{
  return (maxTricks << 7) | (maxRanks << 3) | start;
}


int Header::GetTrickKey()
{
  return (maxTricks << 8) | (cashTricks[QT_ACE] << 4) | cashTricks[QT_PARD];
}


int Header::GetRankKey()
{
  return (minRanks << 12) | (maxRanks << 8) | 
         (cashRanks[QT_ACE] << 4) | cashRanks[QT_PARD];
}


void Header::KeyToText(
  std::ostringstream& out,
  int key)
{
  int s = key & 0x7;
  int mr = (key >> 3) & 0xf;
  int mt = (key >> 7) & 0xf;

  out << "maxTricks " << mt << ", maxRank '" << 
    SDS_RANK_NAMES[mr] << "'";
}


unsigned Header::GetMaxRank()
{
  return static_cast<unsigned>(maxRanks);
}


void Header::ToText(std::ostringstream& out) const
{
  out <<
    setw(10) << "cashTricks" <<
    setw(3) << static_cast<unsigned>(cashTricks[0]) <<
    setw(3) << static_cast<unsigned>(cashTricks[1]) <<
    setw(3) << static_cast<unsigned>(cashTricks[2]) <<
    setw(3) << static_cast<unsigned>(cashTricks[3]) << "\n" <<
    setw(10) << "cashRanks " <<
    setw(3) << SDS_RANK_NAMES[cashRanks[0]] <<
    setw(3) << SDS_RANK_NAMES[cashRanks[1]] <<
    setw(3) << SDS_RANK_NAMES[cashRanks[2]] <<
    setw(3) << SDS_RANK_NAMES[cashRanks[3]] <<"\n";
}


void Header::Print(
  std::ostream& out) const
{
  out << "maxTricks " << static_cast<unsigned>(maxTricks) <<
    ", maxRanks '" << SDS_RANK_NAMES[maxRanks] <<
    "' start '" << POS_NAMES[start] <<
    "' reach '" << REACH_NAMES[reach] << "'\n";

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

