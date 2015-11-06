/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_COMPLEXMOVES_H
#define SDS_COMPLEXMOVES_H

#include "DefList.h"
#include "Holding.h"

void MakeComplexMoves();

void MakeComplexSingleMove(
  Holding& holding,
  DefList& sp);

void MakeComplexMovesParallel();

#endif
