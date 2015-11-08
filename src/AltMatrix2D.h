/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_ALTMATRIX2D_H
#define SDS_ALTMATRIX2D_H


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
      bool hasSum[]);

    cmpDetailType ComparePartial(
      const cmpDetailType diff,
      const cmpDetailType winX,
      const cmpDetailType winY);

    void Print() const;

    void PrintVector(
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
      const unsigned x);
    
    bool IsPurgedY(
      const unsigned y);

    void SetValue(
      const unsigned x,
      const unsigned y,
      const cmpDetailType c);

    void PurgeX(
      const unsigned x);

    void PurgeY(
      const unsigned y);

    void Verify();

    cmpDetailType Compare();

    bool CandList(
      const posType side,
      bool use[]) const;

    void Print(
      const char text[]) const;

    void PrintVector(
      const char text[],
      const bool cvec[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
      const unsigned len) const;
};

#endif

