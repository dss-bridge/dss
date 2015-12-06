/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include "misc.h"


bool SortIsGreater(
  const SortEntry& a,
  const SortEntry& b)
{
  return (b.count < a.count || (b.count == a.count && b.no > a.no));
}
