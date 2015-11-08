/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SDS_HEADER_H
#define SDS_HEADER_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include "cst.h"

class Trick;


class Header
{
  private:

    unsigned char maxTricks;
    unsigned char maxRanks;
    unsigned char minRanks;
    unsigned char cashTricks[DDS_HANDS];
    unsigned char cashRanks[DDS_HANDS];
    posType start;
    reachType reach;

    cmpType CompareSide(
      const Header& newHeader,
      const posType side);

    cmpType CompareReach(const Header& newHeader);

    cmpType ComparePlay(
      const Header& newHeader,
      const posType side);

    cmpType CompareRanks(
      const Header& newHeader,
      const posType side);

  public:

    Header();

    ~Header();

    void Set(
      const int maxTricks,
      const unsigned short maxRanks,
      const int cashTricks[],
      const unsigned short cashRanks[],
      const posType start,
      const reachType reach);

    void SetWithTrick(const Trick& tr);

    void Increase(const Header& hLater);

    cmpType Compare(const Header& newHeader);

    cmpDetailType CompareDetail(const Header& newHeader);

    cmpType CompareFirstPlay(
      const Header& newHeader,
      const posType side);

    cmpType CompareFirstAll(
      const Header& newHeader,
      const posType side);

    void MergeMax(const Header& newHeader);

    void MergeMin(const Header& newHeader);

    bool EqualsExceptPerhapsStart(
      const Header& newHeader,
      const bool skipFlag);

    int GetKey();
    int GetTrickKey();
    int GetRankKey();

    void KeyToText(
      std::ostringstream& out,
      int key);

    unsigned GetMaxRank();

    void Print(
      std::ostream& out = cout) const;

    void ToText(
      std::ostringstream& out) const;
};

#endif

