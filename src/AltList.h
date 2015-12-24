/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_ALTLIST_H
#define SDS_ALTLIST_H

#include <iostream>
#include <vector>

#include "TrickList.h"
#include "AltMatrix1D.h"
#include "AltMatrix2D.h"


class AltList
{
  private:

    TrickList list[SDS_MAX_ALT];
    unsigned len;

    void Backtrack1D(
      AltMatrix1D& comp,
      const unsigned dimFixed,
      const unsigned dimVar,
      std::vector<fixType>& fixVector);

    void FillMatrix1D(
      AltMatrix1D& comp);

    void FillMatrix2D(
      AltList& aNew,
      AltMatrix1D& compX,
      AltMatrix1D& compY,
      const std::vector<bool>& softX,
      const std::vector<bool>& softY);

    bool AllFalse(
      const std::vector<bool>& vec,
      const unsigned vlen) const;

    void PurgeList(
      const AltMatrix1D& comp);

    void PurgeList(
      const std::vector<bool>& purgeList);

    void PunchOut(
      const AltList * alist,
      const unsigned int purgeNo,
      const std::vector<bool>& purgeList,
      const int pstart);

    void PunchOut(
      const AltList * alist,
      const int pstart);

    void Reduce();

    void PurgeMulti();

    cmpDetailType CompareMulti(
      const TrickList& tref) const;

    bool CompareMultiSide(
      const posType sideToLose,
      const AltMatrix2D& comp,
      const AltList& altToLose) const;

    cmpDetailType CompareMultiTrickList(
      TrickList& tlist);

    cmpDetailType FrontIsGE(
      const TrickList& tlist) const;

    cmpDetailType FrontIsGE(
      const Trick& trick) const;

    void ConnectFirst(
      const posType pend);

    void RegisterSize(
      const std::string& text);


  public:

    AltList();

    ~AltList();

    void Reset();

    bool Set1(
      const Trick& trick);

    bool Set2(
      const Trick& trick10,
      const Trick& trick11);

    bool Set11(
      const Trick& trick1,
      const Trick& trick2);

    bool Set12(
      const Trick& trick1,
      const Trick& trick20,
      const Trick& trick21);

    void GetHeader(
      Header& header) const;

    unsigned GetLength() const;

    cmpDetailType Compare(
      const AltList& aNew) const;

    cmpDetailType CompareHard(
      const AltList& aNew) const;

    bool operator == (
      const AltList& alt2) const;

    bool operator != (
      const AltList& alt2) const;

    void operator += (
      const Holding& holding);

    AltList operator + (
      const AltList& aRight) const;

    bool CanMergeSides(
      const AltList& aNew) const;

    void SetStart();

    bool MergeSides(
      const AltList& aNew);

    void Print(
      std::ostream& pout,
      const std::string text,
      const unsigned d = 0) const;
};

#endif

