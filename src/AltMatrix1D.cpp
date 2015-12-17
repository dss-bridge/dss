/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iomanip>
#include <assert.h>

#include "AltMatrix1D.h"

using namespace std;


// matrix is not needed for actual functionality -- could be deleted
// for (a bit of) performance.


AltMatrix1D::AltMatrix1D()
{
  AltMatrix1D::Reset();
}


AltMatrix1D::~AltMatrix1D()
{
}


void AltMatrix1D::Reset()
{
  num = 0;
}


void AltMatrix1D::SetDimension(
  const unsigned x)
{
  assert(x <= SDS_MAX_ALT);

  num = x;

  for (unsigned i = 0; i < num; i++)
    active[i] = true;

  for (unsigned i = 0; i < num; i++)
    for (unsigned j = 0; j < num; j++)
      matrix[i][j] = SDS_HEADER_SAME;
}


bool AltMatrix1D::IsPurged(
  const unsigned x) const
{
  return ! active[x];
}


void AltMatrix1D::SetValue(
  const unsigned x,
  const unsigned y,
  const cmpDetailType c)
{
  assert(x < num);
  assert(y < num && x != y);

  if (x < y)
    matrix[x][y] = c;
  else
    matrix[y][x] = c;
}


void AltMatrix1D::Purge(
  const unsigned x)
{
  assert(x < num);
  assert(active[x]);
  active[x] = false;
}


void AltMatrix1D::Verify()
{
  for (unsigned i = 0; i < num; i++)
  {
    if (! active[i])
      continue;

    for (unsigned c = 0; c < SDS_HEADER_CMP_SIZE; c++)
      has[i][c] = false;

    for (unsigned j = 0; j < num; j++)
    {
      if (i == j || ! active[j])
        continue;

      has[i][matrix[i][j]] = true;
    }

    if (has[i][SDS_HEADER_PLAY_OLD_BETTER] ||
        has[i][SDS_HEADER_RANK_OLD_BETTER])
    {
      if (has[i][SDS_HEADER_PLAY_NEW_BETTER] ||
          has[i][SDS_HEADER_RANK_NEW_BETTER] ||
          has[i][SDS_HEADER_SAME])
      {
        AltMatrix1D::Print(cout, "Verify error I");
        cout.flush();
	assert(false);
      }
    }
    else if (has[i][SDS_HEADER_PLAY_NEW_BETTER] ||
        has[i][SDS_HEADER_RANK_NEW_BETTER])
    {
      if (has[i][SDS_HEADER_SAME])
      {
        AltMatrix1D::Print(cout, "Verify error II");
        cout.flush();
	assert(false);
      }
    }
  }
}


void AltMatrix1D::PrintVector(
  ostream& out) const
{
  out << setw(10) << "";
  for (unsigned i = 0; i < SDS_HEADER_CMP_SIZE; i++)
    out << setw(10) << CMP_DETAIL_NAMES[i];
  out << "\n";

  for (unsigned i = 0; i < num; i++)
  {
    out << setw(2) << i << setw(8) << "";
    for (unsigned j = 0; j < SDS_HEADER_CMP_SIZE; j++)
      out << setw(10) << (has[i][j] ? "yes" : "");
    out << "\n";
  }
  out << "\n";
}


void AltMatrix1D::Print(
  ostream& out,
  const string text) const
{
  if (text != "")
    out << "AltMatrix1D " << text << "\n";

  out << setw(4) << left << "D1" << setw(4) << "D2";
  for (unsigned j = 0; j < num; j++)
    out << setw(3) << j;
  out << "\n";

  for (unsigned i = 0; i < num; i++)
  {
    out << setw(2) << i <<
      setw(6) << (active[i] ? "yes" : "-");

    for (unsigned j = 0; j <= i; j++)
      out << setw(10) << "-";

    for (unsigned j = i+1; j < num; j++)
      out << setw(10) << CMP_DETAIL_NAMES[matrix[i][j]];
    out << "\n";
  }
  out << "\n";

  out << "has\n";
  AltMatrix1D::PrintVector(out);
}

