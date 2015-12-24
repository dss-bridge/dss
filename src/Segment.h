/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_SEGMENT_H
#define SDS_SEGMENT_H

#include <iostream>

#include "Trick.h"
#include "Holding.h"

#define SEGMENT_MAXMOVES 2


class Segment
{
  private:

    Trick list[SEGMENT_MAXMOVES];
    unsigned len;

    void Reset();

    void PrependSimple(
      const Trick& tPrep);

    bool PrependSpecial(
      const Trick& mergingMove,
      const Holding& holding);

    void PrependDeep(
      const Trick& mergingMove);

    void PrependFix(
      const bool lastFlag);

    bool Fix11_12(
      const Segment& seg2,
      fixType& fix1,
      fixType& fix2);

    bool Fix11_OneB(
      Segment& seg2,
      fixType& fix1,
      fixType& fix2) const;

    bool Fix12Special(
      Segment& seg20,
      Segment& seg21,
      fixType& fix1,
      fixType& fix2) const;

    bool Fix1nSpecial(
      Segment& seg20,
      fixType& fix1,
      fixType& fix2) const;

  public:

    Segment();

    ~Segment();

    bool Set1(const Trick& trick);

    bool Set2(
      const Trick& trick1,
      const Trick& trick2);

    void PunchOut(
      const unsigned no);

    void SetStart(
      const posType start);

    posType GetStart() const;

    void SetEnd(
      const posType end);

    posType GetEnd() const;

    unsigned int GetRanks() const;

    unsigned int GetLength() const;

    void GetSummaryTrick(
      Trick& summaryTrick) const;

    cmpDetailType Compare(
      const Segment& seg2) const;

    bool EqualsExceptStart(
      const Segment& seg2) const;

    bool operator == (
      const Segment& seg2) const;

    bool operator != (
      const Segment& seg2) const;

    void Localize(
      const Holding& holding);

    bool Prepend(
      const Holding& holding,
      const bool lastSegFlag);

    bool FixRanks(
      const unsigned rPrep);

    posType Connect(
      const Segment& sPrepend);

    bool Fix11(
      Segment& seg2,
      fixType& fix1,
      fixType& fix2);
    
    bool Fix12(
      Segment& seg20,
      Segment& seg21,
      fixType& fix1,
      fixType& fix2);
    
    bool Fix1n(
      Segment& seg20,
      fixType& fix1,
      fixType& fix2) const;
    
    void Print(
      std::ostream& out = std::cout) const;

    void Print(
      std::ostream& out,
      const unsigned d,
      const unsigned a,
      unsigned& offset) const;
};

#endif

