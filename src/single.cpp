/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <stdio.h>
#include <assert.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

#include "portab.h"

#include "cst.h"
#include "MoveList.h"
#include "SimpleMoves.h"
#include "ComplexMoves.h"
#include "Stats.h"
#include "Twiddle.h"
#include "Dist.h"
#include "LoopHold.h"

#define MOVEFILE "moves.txt"

unsigned headerStats[4] = {0};


// Globals

singleType * singles[14];
MoveList moveList;
int histCount[HIST_SIZE][14], histRank[HIST_SIZE][14];
int histMoveCount[HIST_SIZE][14], histMoveRank[HIST_SIZE][14];

vector<unsigned> holdCtr(1000, 0);

unsigned highestDefNo = 0;
unsigned highestAltNo = 0;

// Global arguments, essentially

int suitLengthArg = 0;
int counterArg = 0;
int noOfCores = 1;
bool debugComplex = false;
bool debugDefList = false;
bool debugAltList = false;

// Function prototypes

bool GetArgs(int argc, char * argv[]);
int GetNextToken(int argc, char * argv[]);
void MainIdentify();
void MakeLists();
void FreeLists();
void SetLengths();
void PrintHists();
void PrintHists(std::ostream& fout);
void PrintHist(
  std::ostream& fout,
  const int hist[][14],
  const int type);

void setTTlist();
void addTTlist(int index);
void printTTlist();


int main(int argc, char * argv[])
{
  if (! GetArgs(argc, argv))
    return 1;

  MainIdentify();
  InitTimer();
  InitTimerList();


  MakeLists();
  SetLengths();

  InitDist();
  SetChain();
  // PrintDist();
  // exit(1);

setTTlist();

#if 0

cout << "sizeof(def) " << sizeof(DefList) << "\n";
cout << "sizeof(Header) " << sizeof(Header) << "\n";
cout << "sizeof(AltList) " << sizeof(AltList) << "\n";
cout << "sizeof(TrickList) " << sizeof(TrickList) << "\n";
cout << "sizeof(Segment) " << sizeof(Segment) << "\n";
cout << "sizeof(Trick) " << sizeof(Trick) << "\n";
cout << "sizeof(bool) " << sizeof(bool) << "\n";

  LoopHold holding;
  holding.Set(9, 0xe4a5);
  // holding.Set(9, 0x92e5);
  // holding.Set(13, 0x986c97);
  // holding.Set(12, 0xb907f);

  // holding.Set(13, 0xc2cbcb);
  // holding.Set(12, 0x3a4245);
  // holding.Set(13, 0x90e17f);
  // holding.Set(13, 0xa65642);
  // holding.Set(13, 0xa65642);
  // holding.Set(13, 0x222e1);

  holding.Print(cout);
  cout.flush();

  DefList def1, def2;

  debugComplex = true;
  debugDefList = true;
  // debugAltList = false;
  debugAltList = true;
  MakeComplexSingleMove(holding, def1, def2);
  cout << "Solving directly with no tables:\n";
  def1.Print();
  def2.Print();

  cout << "Looking up:\n";

  HoldingSimpleMove move;
  /// if (holding.SolveStopped(move))
  int tricks, rank, rankB;
  DefList def2;

  if (holding.SolveStopped(move))
  {
    cout << "Stopped\n";
    DefList def;

    Trick trick;
    trick.Set(move.start, move.end, move.rank, move.tricks);
    def.Set1(trick);
    def.Print(cout);
  }
  else if (holding.CashoutBoth(def2, rank))
  {
    cout << "Cashout both (complex)\n";
    def2.Print(cout);
  }
  else
    cout << "No hit\n";

  cout << "\nHolding hits\n";
  for (int i = 0; i < 1000; i++)
  {
    if (holdCtr[i] == 0) continue;
    cout << setw(4) << i << setw(10) << holdCtr[i] << "\n";
  }
  cout.flush();
  exit(1);
#endif

  ofstream fout(MOVEFILE);
  if (! fout)
  {
    cout << "Can't open " << MOVEFILE << " for writing\n";
    cout.flush();
    exit(1);
  }

  if (debugComplex)
  {
debugComplex = false;
debugDefList = false;
debugAltList = false;
MakeMovesSimple();
singles[suitLengthArg][counterArg].moveNo = 0;
debugComplex = true;
debugDefList = true;
debugAltList = true;

    Holding holding2;
    holding2.Set(suitLengthArg, counterArg);
    holding2.Print(cout);
    
    if (singles[suitLengthArg][counterArg].moveNo)
      moveList.Print(singles[suitLengthArg][counterArg].moveNo);
    else
    {
      DefList def1, def2;

      MakeComplexSingleMove(holding2, def1, def2);
    }
  }
  else if (noOfCores > 1)
  {
    MakeMovesSimple();

    cout << "noOfCores " << noOfCores << "\n";
    MakeComplexMovesParallel();
    moveList.PrintMoveList(fout);
    moveList.PrintLists(fout);
  }
  else
  {
// debugComplex = true;
    MakeMovesSimple();

    MakeComplexMoves();
    moveList.PrintMoveList(fout);
    moveList.PrintLists(fout);
  }

  moveList.PrintStats();
  cout.flush();
  fout.flush();

  FreeLists();

  PrintHists(fout);
  fout.close();

  PrintGroupHist();
  PrintTimer();
  PrintTimerList();

  cout << "\nHolding hits\n";
  for (unsigned i = 0; i < 1000; i++)
  {
    if (holdCtr[i] == 0) continue;
    cout << 
      setw(4) << i << setw(10) << holdCtr[i] << "\n";
  }

  cout << "Maxdef/alt " << highestDefNo << " " << highestAltNo << "\n";
  printTTlist();

  cout << "max header " << headerStats[0] << " " << headerStats[1] << " " <<
    headerStats[2] << " " << headerStats[3] << endl;
}


int nextToken = 1;
char * optarg;

bool GetArgs(int argc, char * argv[])
{
  int c;
  bool errFlag = false;
  char * temp;

  while ((c = GetNextToken(argc, argv)) != -1)
  {
    switch(c)
    {
      case 'n':
        noOfCores = static_cast<int>(strtol(optarg, &temp, 0));
        if (* temp != '\0')
        {
          cerr << "Cores -n must be numerical (" << optarg << ")\n";
          errFlag = true;
        }
        else if (noOfCores < 1 || noOfCores > 12)
        {
          cerr << "Cores -n (" << noOfCores << ") must be 1..12\n";
          errFlag = true;
        }
        break;

      case 's':
        suitLengthArg = static_cast<int>(strtol(optarg, &temp, 0));
        if (* temp != '\0')
        {
          cerr << "Suit length -s must be numerical (" << optarg << ")\n";
          errFlag = true;
        }
        else if (suitLengthArg < 2 || suitLengthArg > 13)
        {
          cerr << "Suit length -s (" << suitLengthArg << 
            ") must be 2..13\n";
          errFlag = true;
        }
        break;

      case 'c':
        counterArg = static_cast<int>(strtol(optarg, &temp, 0));
        if (* temp != '\0')
        {
          cerr << "Counter -c must be numerical (" << optarg << ")\n";
          errFlag = true;
        }
        else if (counterArg < 0 || counterArg > SDS_NUMSINGLES[13])
        {
          cerr << "Counter -c (" << counterArg << ") must be 0..2^13\n";
          errFlag = true;
        }
        break;

      case ':':
        cerr << "Option -" << c << " requires an operand\n";
        errFlag = true;
        break;

      case '?':
        cerr << "Unrecognized option: '-" << c << "'\n";
        errFlag = true;
        break;

      default:
        cerr << "Unrecognized option: '-" << c << "'\n";
        errFlag = true;
        break;
    }
  }

  if (counterArg >= SDS_NUMSINGLES[suitLengthArg])
  {
    cerr << "-c (" << counterArg << ") must be < " <<
      hex << SDS_NUMSINGLES[suitLengthArg] << dec <<
      " (suit length " << suitLengthArg << "\n";
    errFlag = 1;
  }

  if (errFlag)
  {
    cerr << "Usage: [-option value]\n\n";
    cerr << "  -n    No. of cores (1..12, default 1)\n";
    cerr << "  -s    Suit length (2..13, default range)\n";
    cerr << "  -c    Counter (consistent with suit length)\n";

    return false;
  }

  if (suitLengthArg > 0 && counterArg > 0)
  {
    debugComplex = true;
    debugDefList = true;
    debugAltList = true;
  }

  return true;
}


int GetNextToken(int argc, char * argv[])
{
  if (nextToken >= argc)
  {
    return -1;
  }

  char opt[8];
  strcpy(opt, argv[nextToken]);
  if (opt[0] != '-')
    return '?';

  optarg = argv[nextToken+1];

  nextToken += 2;
  return opt[1];
}


void MainIdentify()
{
  cout << "single\n----------\n";

#if defined(_WIN32) || defined(__CYGWIN__)
  cout << setw(12) << left << "System" << setw(23) << 
    right << "Windows\n";
#if defined(_MSC_VER)
  cout << setw(12) << left << "Compiler" << setw(23) << 
      right << "Microsoft Visual C++\n";
#elif defined(__MINGW32__)
  cout << setw(12) << left << "Compiler" << setw(23) << 
    right << "MinGW\n";
#else
  cout << setw(12) << left << "Compiler" << setw(23) << 
    right << "GNU g++\n";
#endif

#elif defined(__linux)
  cout << setw(12) << left << "System" << 
    right << setw(23) << "Linux\n";
  cout << setw(12) << left << "Compiler" << 
    right << setw(23) << "GNU g++\n";

#elif defined(__APPLE__)
  cout << setw(12) << left << "System" << 
    right << setw(23) << "Apple\n";
#if defined(__clang__)
  cout << setw(12) << left << "Compiler" << 
    right << setw(23) << "clang\n";
#else
  cout << setw(12) << left << "Compiler" << 
    right << setw(23) << "GNU g++\n";
#endif
#endif

#if defined(__cplusplus)
  cout << setw(12) << left << "Dialect" << 
    right << setw(22) << __cplusplus << "\n";
#endif

  cout << "\n";
}

void MakeLists()
{
  singles[0] = nullptr;

  for (int l = 1; l <= 13; l++)
  {
    size_t n = static_cast<size_t>(SDS_NUMSINGLES[l]);
    singles[l] = static_cast<singleType *>(malloc(n * sizeof(singleType)));
  }
}


void FreeLists()
{
  for (int l = 1; l <= 13; l++)
    free(singles[l]);
}


void SetLengths()
{
  for (int hno = 0; hno < HIST_SIZE; hno++)
    for (int sl = 0; sl < 14; sl++)
    {
      histCount[hno][sl] = 0;
      histRank[hno][sl] = 0;

      histMoveCount[hno][sl] = 0;
      histMoveRank[hno][sl] = 0;
    }

  for (int sl = 1; sl <= 13; sl++)
  {
    int n = SDS_NUMSINGLES[sl];
    for (int c = 0; c < n; c++)
    {
      int tmp = c;
      int count[DDS_HANDS] = {1, 0, 0, 0}; // The ace
      for (int m = 0; m < sl-1; m++)
      {
        count[tmp & 3]++;
        tmp >>= 2;
      }
      singles[sl][c].moveNo = 0;
      singles[sl][c].declLen = (count[0] >= count[2] ? count[0] : count[2]);
      singles[sl][c].minLen = (count[0] <= count[2] ? count[0] : count[2]);
      singles[sl][c].oppLen = (count[1] >= count[3] ? count[1] : count[3]);

      histCount[HIST_ORIG_COUNT][singles[sl][c].declLen]++;
    }
  }
}


void PrintHists()
{
  PrintHists(cout);
}


void PrintHists(std::ostream& fout)
{
  PrintHist(fout, histCount, 0);
  PrintHist(fout, histRank, 1);

  PrintHist(fout, histMoveCount, 0);
  PrintHist(fout, histMoveRank, 1);
}


void PrintHist(
  std::ostream& fout,
  const int hist[][14],
  const int type)
{
  if (type == 0)
    fout << "Suit length count\n\n";
  else
    fout << "Max ranks\n\n";

  fout << setw(2) << "No";
  for (int hno = 0; hno < HIST_SIZE; hno++)
    fout << setw(12) << HIST_NAMES[hno];
  fout << "\n";

  int sum[HIST_SIZE] = {0};

  for (int sl = 0; sl < 14; sl++)
  {
    if (type == 0)
      fout << setw(2) << sl;
    else
      fout << setw(2) << SDS_RANK_NAMES[sl];

    for (int hno = 0; hno < HIST_SIZE; hno++)
    {
      fout << setw(12) << hist[hno][sl];
      sum[hno] += hist[hno][sl];
    }
    fout << "\n";
  }

  fout << "\n  ";
  for (int hno = 0; hno < HIST_SIZE; hno++)
    fout << setw(12) << sum[hno];
  fout << "\n\n";
}




int TTlist[729];

void setTTlist()
{
  for (int i = 0; i < 729; i++)
    TTlist[i] = 0;
}


void addTTlist(int index)
{
  TTlist[index]++;
}


void printTTlist()
{
  int j;
  for (int i = 0; i < 729; i++)
  {
    if (TTlist[i] == 0)
      continue;

    int ra = i % 3;
    j = i / 3;

    int ca = j % 3;
    j /= 3;

    int e2 = j % 3;
    j /= 3;

    int s2 = j % 3;
    j /= 3;

    int e1 = j % 3;

    int s1 = j / 3;

    cout <<
      setw(4) << i <<
      setw(6) << POS_NAMES[2*s1] <<
      setw(6) << POS_NAMES[2*e1] <<
      setw(6) << POS_NAMES[2*s2] <<
      setw(6) << POS_NAMES[2*e2] <<
      setw(6) << ca <<
      setw(6) << ra <<
      setw(10) << TTlist[i] << "\n";
  }
}
