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
    unsigned keyNew;

    unsigned dCum;
    unsigned aCum;
    unsigned sCum;
    unsigned tCum;

    unsigned startNew;
    unsigned endNew;

    unsigned cashAceMin;
    unsigned cashAceMax;
    unsigned cashPardMin;
    unsigned cashPardMax;
    unsigned rankAceMin;
    unsigned rankAceMax;
    unsigned rankPardMin;
    unsigned rankPardMax;

  public:

    Header();

    ~Header();

    void Set(
      const Trick& trick,
      unsigned tno);

    void Increase(
      const Trick& tLater,
      unsigned tno);

    void MergeMax(
      const Header& newHeader);

    void MergeMin(
      const Header& newHeader);

    void GetAD(
      unsigned& d,
      unsigned& asum) const;
    int GetTrickKey() const;
    int GetRankKey() const;
    unsigned GetMaxRank() const;
    unsigned GetKeyNew() const;
    unsigned CheckManual() const;

    void PrintKey(
      std::ostream& out,
      const int key) const;

    void Print(
      std::ostream& out = std::cout,
      const bool skipMax = false) const;
};

#endif

