/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include <assert.h>

#include "cst.h"
#include "AltMatrix1D.h"
#include "portab.h"

extern bool debugComplex;


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
  verifiedFlag = false;
}


void AltMatrix1D::SetDimension(
  const unsigned x)
{
  assert(x < SDS_MAX_ALT);

  num = x;

  for (unsigned i = 0; i < num; i++)
    active[i] = true;

  for (unsigned i = 0; i < num; i++)
    for (unsigned j = 0; j < num; j++)
      matrix[i][j] = SDS_HEADER_SAME;
}


void AltMatrix1D::SetDimension(
  const unsigned x,
  const int actX[])
{
  assert(x < SDS_MAX_ALT);

  num = x;

  for (unsigned i = 0; i < num; i++)
    active[i] = actX[i];

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
  verifiedFlag = true;

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
        AltMatrix1D::Print("Verify error I");
        cout.flush();
	assert(false);
      }
    }
    else if (has[i][SDS_HEADER_PLAY_NEW_BETTER] ||
        has[i][SDS_HEADER_RANK_NEW_BETTER])
    {
      if (has[i][SDS_HEADER_SAME])
      {
        AltMatrix1D::Print("Verify error II");
        cout.flush();
	assert(false);
      }
    }
  }
}


void AltMatrix1D::Print(
  const char text[])
{
  cout << "AltMatrix1D " << text << "\n";
  AltMatrix1D::Print();
}


void AltMatrix1D::Print()
{
  cout << setw(4) << left << "D1" << setw(4) << "D2";
  for (unsigned j = 0; j < num; j++)
    cout << setw(3) << j;
  cout << "\n";

  for (unsigned i = 0; i < num; i++)
  {
    cout << setw(2) << i <<
      setw(6) << (active[i] ? "yes" : "-");

    for (unsigned j = 0; j <= i; j++)
      cout << setw(10) << "-";

    for (unsigned j = i+1; j < num; j++)
      cout << setw(10) << CMP_DETAIL_NAMES[matrix[i][j]];
    cout << "\n";
  }
  cout << "\n";

  if (! verifiedFlag)
    AltMatrix1D::Verify();

  AltMatrix1D::PrintVector("has", has, num);
}


void AltMatrix1D::PrintVector(
  const char text[],
  const bool cvec[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
  const unsigned len)
{
  cout << text << "\n";
  AltMatrix1D::PrintVector(cvec, len);
}


void AltMatrix1D::PrintVector(
  const bool cvec[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
  const unsigned len)
{
  cout << setw(10) << "";
  for (unsigned i = 0; i < SDS_HEADER_CMP_SIZE; i++)
    cout << setw(10) << CMP_DETAIL_NAMES[i];
  cout << "\n";

  for (unsigned i = 0; i < len; i++)
  {
    cout << setw(2) << i << setw(8) << "";
    for (unsigned j = 0; j < SDS_HEADER_CMP_SIZE; j++)
      cout << setw(10) << (cvec[j] ? "yes" : "");
    cout << "\n";
  }
  cout << "\n";
}

