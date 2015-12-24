/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_TRICK_H
#define SDS_TRICK_H

#include <iostream>

#include "cst.h"
#include "Holding.h"

class Holding;
class Segment;


class Trick
{
  private:

    struct trickType
    {
      posType start;
      posType end;
      unsigned char cashing;
      unsigned char ranks;
    };

    trickType trick;

    void Reset();

    cmpType ComparePlay(
      const Trick& t1,
      const posType side) const;

  public:

    friend class Segment;

    Trick();

    ~Trick();

    void Set(
      const posType start,
      const posType end,
      const unsigned ranks,
      const unsigned cashing);

    void SetStart(
      const posType start);

    void SetEnd(
      const posType end);

    posType GetStart() const;

    posType GetEnd() const;

    unsigned GetCashing() const;

    unsigned GetRanks() const;

    bool operator == (
      const Trick& t2) const;

    bool operator != (
      const Trick& t2) const;

    bool EqualsExceptStart(
      const Trick& t2) const;

    bool Extends(
      const Trick& tEarlier) const;

    void Localize(
      const Holding& holding);

    appendType Prepend(
      const Trick& tPrep,
      const bool mergeSpecialFlag,
      const bool lastFlag);

    bool ReduceBoth(
      const Trick& t1);

    cmpDetailType Compare(
      const Trick& t1) const;

    cmpType CashRankOrder(
      const unsigned char c,
      const unsigned char r) const;

    void Print(
      std::ostream& out = std::cout) const;

    void Print(
      std::ostream& out,
      const unsigned d,
      const unsigned a,
      const unsigned p) const;
};

#endif

