/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SDS_HEADER_H
#define SDS_HEADER_H

#include <iostream>

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

    cmpType ComparePlay(
      const Header& newHeader,
      const posType side) const;

    cmpType CompareRanks(
      const Header& newHeader,
      const posType side) const;

    cmpType CompareFirstPlay(
      const Header& newHeader,
      const posType side) const;

  public:

    Header();

    ~Header();

    void Set(
      const Trick& trick);

    void Increase(
      const Trick& tLater);

    cmpDetailType Compare(
      const Header& newHeader) const;

    void MergeMax(
      const Header& newHeader);

    void MergeMin(
      const Header& newHeader);

    bool EqualsExceptStart(
      const Header& newHeader) const;

    int GetKey();
    int GetTrickKey();
    int GetRankKey();

    void KeyToText(
      std::ostringstream& out,
      int key);

    unsigned GetMaxRank() const;

    void Print(
      std::ostream& out = std::cout,
      const bool skipMax = false) const;
};

#endif

