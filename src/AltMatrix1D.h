/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_ALTMATRIX1D_H
#define SDS_ALTMATRIX1D_H

#include <iostream>
#include <string>

#include "cst.h"


class AltMatrix1D
{
  private:

    cmpDetailType matrix[SDS_MAX_ALT][SDS_MAX_ALT];

    unsigned num;

    bool active[SDS_MAX_ALT];

    bool has[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE];

    void PrintVector(
      std::ostream& out) const;

  public:

    AltMatrix1D();

    ~AltMatrix1D();

    void Reset();

    void SetDimension(
      const unsigned x);

    bool IsPurged(
      const unsigned x) const;
    
    void SetValue(
      const unsigned x,
      const unsigned y,
      const cmpDetailType c);

    void Purge(
      const unsigned x);

    void Verify();

    void Print(
      std::ostream& out = std::cout,
      const std::string text = "") const;
};

#endif

