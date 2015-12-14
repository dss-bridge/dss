/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SDS_DIST_H
#define SDS_DIST_H

void InitDist();

void SetChain();

void SetAllLowCards(
  const int& suitLength,
  const int& counter,
  unsigned moveNo,
  Holding& holding,
  const int rank,
  const int histNo,
  bool& newFlag);

void PrintDist();

void PrintHistLowCards();

#endif
