/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_SIMPLEMOVES_H
#define SDS_SIMPLEMOVES_H

#include "Holding.h"


void MakeMovesSimple();

void UpdateHist(
  const int histNo,
  const int suitLength,
  const int counter,
  const int rank,
  bool newFlag);

void CompareRecurse(
  unsigned moveNo,
  Holding& Holding,
  const int histNo);

#endif
