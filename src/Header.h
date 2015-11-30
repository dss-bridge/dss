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

    posType start;
    posType end;
    unsigned char maxTricks;
    unsigned char maxRanks;
    unsigned char minRanks;
    unsigned char cashTricks[DDS_HANDS];
    unsigned char cashRanks[DDS_HANDS];

  public:

    Header();

    ~Header();

    void Set(
      const Trick& trick);

    void Increase(
      const Trick& tLater);

    void MergeMax(
      const Header& newHeader);

    void MergeMin(
      const Header& newHeader);

    int GetKey() const;
    int GetTrickKey() const;
    int GetRankKey() const;
    unsigned GetMaxRank() const;

    void PrintKey(
      std::ostream& out,
      const int key) const;

    void Print(
      std::ostream& out = std::cout,
      const bool skipMax = false) const;
};

#endif

