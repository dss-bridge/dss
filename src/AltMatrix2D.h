/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_ALTMATRIX2D_H
#define SDS_ALTMATRIX2D_H

#include <iostream>
#include <string>

#include "cst.h"


class AltMatrix2D
{
  private:

    cmpDetailType matrix[SDS_MAX_ALT][SDS_MAX_ALT];

    unsigned numX;
    unsigned numY;

    bool activeX[SDS_MAX_ALT];
    bool activeY[SDS_MAX_ALT];

    // Actually does not have to be stored.
    bool hasX[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE];
    bool hasY[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE];
    bool hasXsum[SDS_HEADER_CMP_SIZE];
    bool hasYsum[SDS_HEADER_CMP_SIZE];

    cmpDetailType cval;

    void ResetVectors(
      bool has[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
      const bool active[],
      const unsigned len,
      bool hasSum[]);

    void VerifyVector(
      const bool has[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
      const bool active[],
      const unsigned len,
      bool hasSum[],
      const bool verifyFlag);

    cmpDetailType ComparePartial(
      const cmpDetailType diff,
      const cmpDetailType winX,
      const cmpDetailType winY);

    cmpDetailType ComparePartialDeclarer(
      const cmpDetailType diff,
      const cmpDetailType winX,
      const cmpDetailType winY);

    void PrintVector(
      std::ostream& out,
      const std::string text,
      const bool cvec[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
      const unsigned len) const;

  public:

    AltMatrix2D();

    ~AltMatrix2D();

    void Reset();

    void SetDimensions(
      const unsigned x,
      const unsigned y);
    
    bool IsPurgedX(
      const unsigned x) const;
    
    bool IsPurgedY(
      const unsigned y) const;

    void SetValue(
      const unsigned x,
      const unsigned y,
      const cmpDetailType c);

    void PurgeX(
      const unsigned x);

    void PurgeY(
      const unsigned y);

    void Verify(
      const bool verifyFlag = true);

    cmpDetailType Compare();

    cmpDetailType CompareDeclarer();

    cmpDetailType CompareHard();

    bool CandList(
      const posType side,
      bool use[],
      cmpDetailType& c) const;

    void Print(
      std::ostream& out,
      const std::string text) const;
};

#endif

