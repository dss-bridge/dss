/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_SEGMENT_H
#define SDS_SEGMENT_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

#include "Trick.h"
#include "Header.h"
#include "Holding.h"

#define SEGMENT_MAXMOVES 2


class Segment
{
  private:

    Trick list[SEGMENT_MAXMOVES];
    unsigned len;
    Trick headerTrick;
    bool headerDirty;

    void Reset();

    reachType GetReach() const;

    bool PrependSpecial(
      const Trick& mergingMove,
      const Holding& holding);

    void PrependDeep(const Trick& mergingMove);

    void PrependFix(const bool lastFlag);

    bool Fix11_12(
      Segment& seg2,
      fixType& fix1,
      fixType& fix2);

    bool Fix11_OneB(
      Segment& seg2,
      fixType& fix1,
      fixType& fix2);

  public:

    Segment();

    ~Segment();

    bool Set1(const Trick& trick);

    bool Set2(
      const Trick& trick1,
      const Trick& trick2);

    void PunchOut(const unsigned no);

    void SetStart(
      const posType start,
      const bool lastSegFlag);

    posType GetStart() const;

    posType GetEnd() const;

    unsigned int GetLength() const;

    bool FirstRankExceeds(const unsigned lhoRank) const;

    const Trick& GetHeaderTrick();

    bool FixRanks(
      const unsigned maxOppRank,
      const unsigned hrank);

    cmpDetailType CompareHeader(Segment& seg2);

    bool EqualsExceptStart(Segment& seg2);

    void Localize(const Holding& holding);

    bool operator == (const Segment& seg2) const;

    bool operator != (const Segment& seg2) const;

    bool Prepend(
      const Holding& holding,
      const bool lastSegFlag);

    void PrependSimple(
      const Trick& tPrep);

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
    
    void Print(
      std::ostream& pout) const;

    void Print(
      std::ostream& out,
      const unsigned d,
      const unsigned a,
      unsigned& offset) const;
};

#endif

