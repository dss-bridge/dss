/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_MISC_H
#define SDS_MISC_H

struct SortEntry
{
  unsigned no;
  unsigned count;
};

bool SortIsGreater(
  const SortEntry& a,
  const SortEntry& b);

#endif

