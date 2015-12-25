/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_COMPLEXMOVES_H
#define SDS_COMPLEXMOVES_H

#include "Holding.h"
#include "DefList.h"

void MakeComplexMoves();

void MakeComplexSingleMove(
  Holding& holding,
  DefList& def1,
  DefList& def2);

void MakeComplexMovesParallel();

#endif
