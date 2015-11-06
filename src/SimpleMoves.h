/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_SIMPLEMOVES_H
#define SDS_SIMPLEMOVES_H

void MakeMovesSimple();

void UpdateHist(
  const int histNo,
  const int suitLength,
  const int counter,
  const int rank,
  bool newFlag);

void CompareRecurse(
  DefList& def,
  Holding& Holding,
  const int histNo);

#endif
