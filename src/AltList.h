/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_ALTLIST_H
#define SDS_ALTLIST_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

#include "TrickList.h"
#include "AltMatrix1D.h"
#include "AltMatrix2D.h"


class AltList
{
  private:

    TrickList list[SDS_MAX_ALT];
    unsigned len;
    Header header;
    bool headerDirty;

    void Backtrack1D(
      AltMatrix1D& comp,
      const unsigned dimFixed,
      const unsigned dimVar,
      vector<fixType>& fixVector);

    void FillMatrix1D(
      AltMatrix1D& comp);

    void FillMatrix2D(
      AltList& aNew,
      AltMatrix1D& compX,
      AltMatrix1D& compY,
      vector<bool>& softX,
      vector<bool>& softY);

    bool AllFalse(
      const vector<bool>& vec,
      const unsigned vlen) const;

    void PurgeList(
      AltMatrix1D& comp);

    void PurgeList(
      const vector<bool>& purgeList);

    void PunchOut(
      const AltList * alist,
      const unsigned int purgeNo,
      const vector<bool>& purgeList,
      const int pstart);

    void PunchOut(
      const AltList * alist,
      const int pstart);

    void Reduce();

    void PurgeMulti();

    bool CompareMulti(
      const TrickList& tref);

    bool CompareMultiSide(
      const posType sideToLose,
      const AltMatrix2D& comp,
      const AltList& altToLose);

    bool operator >= (
      TrickList& tlist);

    bool FrontIsGE(
      TrickList& tlist);

    bool FrontIsGE(
      const Trick& trick);

    void ConnectFirst(
      const posType pend);

    void RegisterSize(
      const string& text);


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

    Header& GetHeader();

    unsigned GetLen() const;

    cmpDetailType Compare(AltList& aNew);

    bool operator == (
      const AltList& alt2) const;

    bool operator != (
      const AltList& alt2) const;

    void operator += (
      const Holding& holding);

    AltList operator + (const AltList& aRight);

    bool MergeSides(AltList& aNew);

    void Print(
      std::ostream& pout,
      const string text,
      const unsigned d = 0) const;
};

#endif

