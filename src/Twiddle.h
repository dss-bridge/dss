/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_TWIDDLE_H
#define SDS_TWIDDLE_H

void SetAllPermutations(
  const int& suitLength,
  const int& counter,
  unsigned moveNo,
  Holding& holding,
  const int rank,
  const int histNo,
  bool& newFlag);

void PrintGroupHist();

#endif
