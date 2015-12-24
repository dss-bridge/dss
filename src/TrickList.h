/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_TRICKLIST_H
#define SDS_TRICKLIST_H

#include <iostream>

#include "Segment.h"
#include "Trick.h"
#include "Header.h"
#include "Holding.h"

#define TRICKLIST_MAXSEGS 4


class TrickList
{
  private:

    Segment list[TRICKLIST_MAXSEGS];
    unsigned len;

    struct CompareStruct
    {
      unsigned lenOld;
      unsigned lenNew;
      cmpDetailType winnerFirst;
      cmpDetailType winnerRunning;
      unsigned tricksOld;
      unsigned tricksNew;
      unsigned ranksOld;
      unsigned ranksNew;
    };

    cmpDetailType CompareInit(
      const TrickList& lNew,
      CompareStruct cdata) const;

    cmpDetailType CompareRunning(
      const CompareStruct cdata) const;

    cmpDetailType CompareTail(
      const TrickList& lNew,
      CompareStruct cdata) const;

    void Split();

  public:

    TrickList();

    ~TrickList();

    void Reset();

    bool Set1(
      const Trick& trick);

    bool Set2(
      const Trick& trick1,
      const Trick& trick2);

    void SetStart(
      const posType start);

    posType GetFirstStart() const;

    posType GetFirstEnd() const;

    unsigned int GetLength() const;

    const void GetHeader(
      Header& header,
      const unsigned startNo = 0) const;

    void GetFirstSummaryTrick(
      Trick& t) const;

    cmpDetailType Compare(
      const TrickList& lNew) const;

    bool EqualsExceptStart(
      const TrickList& lNew) const;

    bool operator == (
      const TrickList& t2) const;

    bool operator != (
      const TrickList& t2) const;

    void operator += (
      const Holding& holding);

    cmpDetailType CompareToTrick (
      const Trick& trick) const;

    cmpDetailType FixOrCompare(
      TrickList& lOther,
      fixType& fix1,
      fixType& fix2);
    
    bool Fix(
      TrickList& lOther,
      fixType& fix1,
      fixType& fix2);
    
    posType ConnectFirst(
      const posType pend);

    posType ConnectFirst();

    void Print(
      std::ostream& out = std::cout,
      const unsigned d = 0,
      const unsigned a = 0) const;
};

#endif

