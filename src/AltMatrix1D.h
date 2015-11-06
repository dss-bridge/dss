/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_ALTMATRIX1D_H
#define SDS_ALTMATRIX1D_H


class AltMatrix1D
{
  private:

    cmpDetailType matrix[SDS_MAX_ALT][SDS_MAX_ALT];

    unsigned num;

    bool active[SDS_MAX_ALT];

    bool verifiedFlag;

    bool has[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE];

    void Print();

    void PrintVector(
      const bool cvec[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
      const unsigned len);

  public:

    AltMatrix1D();

    ~AltMatrix1D();

    void Reset();

    void SetDimension(
      const unsigned x);

    void SetDimension(
      const unsigned x,
      const int actX[]);

    bool IsPurged(
      const unsigned x);
    
    void SetValue(
      const unsigned x,
      const unsigned y,
      const cmpDetailType c);

    void Purge(
      const unsigned x);

    void Verify();

    void Print(
      const char text[]);

    void PrintSymm(
      const char text[]);

    void PrintVector(
      const char text[],
      const bool cvec[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
      const unsigned len);
};

#endif

