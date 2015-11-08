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
    Header header;
    bool headerDirty;

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

    const Header& GetHeader(
      const unsigned startNo = 0);

    const Trick& GetFirstHeaderTrick();

    cmpDetailType Compare(
      TrickList& lNew);

    bool EqualsExceptStart(
      TrickList& lNew);

    bool operator == (
      const TrickList& t2) const;

    bool operator != (
      const TrickList& t2) const;

    void operator += (
      const Holding& holding);

    cmpDetailType FixOrCompare(
      TrickList& lOther,
      fixType& fix1,
      fixType& fix2);
    
    bool Fix(
      TrickList& lOther,
      fixType& fix1,
      fixType& fix2);
    
    bool operator >= (
      const Trick& trick);

    posType ConnectFirst(
      const posType pend);

    posType ConnectFirst();

    void Print(
      std::ostream& pout,
      const unsigned d = 0,
      const unsigned a = 0) const;
};

#endif

