/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_CST_H
#define SDS_CST_H

#define MAXNOOFCORES 12

#define DDS_HANDS 4

#define SDS_VOID 13
#define SDS_ACE 12

#define SDS_MAX_DEF 4
#define SDS_MAX_ALT 6


#define Max(x, y) (((x) >= (y)) ? (x) : (y))
#define Min(x, y) (((x) <= (y)) ? (x) : (y))


const int SDS_NUMSINGLES[14] = { 
  0x0000001, // length 0
  0x0000001, // length 1,
  0x0000004, // length 2
  0x0000010, // length 3
  0x0000040, // length 4
  0x0000100, // length 5
  0x0000400, // length 6
  0x0001000, // length 7
  0x0004000, // length 8
  0x0010000, // length 9
  0x0040000, // length 10
  0x0100000, // length 11
  0x0400000, // length 12, 4.2 million
  0x1000000, // length 13, 16.8 million
};

const int SDS_NUMBOTTOMS[8] = { 1, 4, 10, 20, 35, 56, 84, 120 };

const char SDS_RANK_NAMES[14] =
{
  '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A', '-'
};


enum posType
{
  QT_ACE = 0, 
  QT_LHO = 1, 
  QT_PARD = 2, 
  QT_RHO = 3, 
  QT_BOTH = 4,
  QT_SIZE = 5
};

const char POS_NAMES[QT_SIZE][10] =
{ "Ace", "LHO", "Pard", "RHO", "Both" };


const char POS_SHORT_NAMES[QT_SIZE][10] =
{ "A", "L", "P", "R", "B" };


const posType SDS_PARTNER[QT_BOTH] =
{
  QT_PARD,
  QT_RHO,
  QT_ACE,
  QT_LHO
};

enum reachType
{
  SDS_SIDE_NONE = 0, 
  SDS_SIDE_ACE = 1, 
  SDS_SIDE_PARD = 2, 
  SDS_SIDE_BOTH = 3, 
  SDS_SIDE_SIZE = 4
};

const reachType posToReach[QT_SIZE] =
{ 
  SDS_SIDE_ACE, 
  SDS_SIDE_NONE, 
  SDS_SIDE_PARD, 
  SDS_SIDE_NONE, 
  SDS_SIDE_BOTH 
};

const char REACH_NAMES[SDS_SIDE_SIZE][10] =
{ "None", "Ace", "Pard", "Both" };


enum appendType
{
  SDS_APPEND_MERGED = 0,
  SDS_APPEND_SAME_SEG = 1,
  SDS_APPEND_NEW_SEG = 2
};

enum cmpType
{
  SDS_SAME = 0, 
  SDS_NEW_BETTER = 1, 
  SDS_OLD_BETTER = 2, 
  SDS_DIFFERENT = 3,
  SDS_SAME_EXCEPT_START = 4, // Not clean
  SDS_CMP_SIZE = 4
};

enum fixType
{
  SDS_FIX_UNCHANGED = 0,
  SDS_FIX_PURGED = 1,
  SDS_FIX_STRONGER = 2,
  SDS_FIX_WEAKER = 3,
  SDS_FIX_SPLIT = 4,
  SDS_FIX_COLLAPSE = 5,
  SDS_FIX_SIZE = 6

};

const char CMP_NAMES[SDS_CMP_SIZE][10] =
{ "Same", "New", "Old", "Diff" };


enum cmpDetailType
{
  SDS_HEADER_SAME = 0,
  SDS_HEADER_PLAY_NEW_BETTER = 1,
  SDS_HEADER_PLAY_OLD_BETTER = 2,
  SDS_HEADER_PLAY_DIFFERENT = 3,
  SDS_HEADER_RANK_NEW_BETTER = 4,
  SDS_HEADER_RANK_OLD_BETTER = 5,
  SDS_HEADER_RANK_DIFFERENT = 6,
  SDS_HEADER_CMP_SIZE = 7
};

const char CMP_DETAIL_NAMES[SDS_HEADER_CMP_SIZE][10] =
{ "Same", "Play new", "Play old", "Playdiff",
  "Rank new", "Rank old", "Rankdiff" };


const cmpDetailType cmpPlayToDetail[SDS_CMP_SIZE] =
{
  SDS_HEADER_SAME,
  SDS_HEADER_PLAY_NEW_BETTER,
  SDS_HEADER_PLAY_OLD_BETTER,
  SDS_HEADER_PLAY_DIFFERENT
};

const cmpDetailType cmpRanksToDetail[SDS_CMP_SIZE] =
{
  SDS_HEADER_SAME,
  SDS_HEADER_RANK_NEW_BETTER,
  SDS_HEADER_RANK_OLD_BETTER,
  SDS_HEADER_RANK_DIFFERENT
};

const cmpType cmpDetailToShort[SDS_HEADER_CMP_SIZE] =
{
  SDS_SAME,
  SDS_NEW_BETTER,
  SDS_OLD_BETTER,
  SDS_DIFFERENT,
  SDS_NEW_BETTER,
  SDS_OLD_BETTER,
  SDS_DIFFERENT
};

const bool cmpDetailToGE[SDS_HEADER_CMP_SIZE] =
{
  true,
  false,
  true,
  false,
  false,
  true,
  false
};

const cmpType reachMatrix[SDS_SIDE_SIZE][SDS_SIDE_SIZE] =
{
  // NONE            ACE             PARD            BOTH
  {  SDS_SAME,       SDS_NEW_BETTER, SDS_NEW_BETTER, SDS_NEW_BETTER },
  {  SDS_OLD_BETTER, SDS_SAME,       SDS_DIFFERENT,  SDS_NEW_BETTER },
  {  SDS_OLD_BETTER, SDS_DIFFERENT,  SDS_SAME,       SDS_NEW_BETTER },
  {  SDS_OLD_BETTER, SDS_OLD_BETTER, SDS_OLD_BETTER, SDS_SAME }
};

const cmpType cmpMergeMatrix[SDS_CMP_SIZE][SDS_CMP_SIZE] =
{
  // Same            New             Old             Diff
  {  SDS_SAME,       SDS_NEW_BETTER, SDS_OLD_BETTER, SDS_DIFFERENT },
  {  SDS_NEW_BETTER, SDS_NEW_BETTER, SDS_DIFFERENT,  SDS_DIFFERENT },
  {  SDS_OLD_BETTER, SDS_DIFFERENT,  SDS_OLD_BETTER, SDS_DIFFERENT },
  {  SDS_DIFFERENT,  SDS_DIFFERENT,  SDS_DIFFERENT,  SDS_DIFFERENT }
};

const reachType reachMap[SDS_SIDE_SIZE][QT_SIZE] =
{
  // LHO, RHO and BOTH as 2nd index can't really happen.
  // QT_ACE         QT_LHO         QT_PARD        QT_RHO         QT_BOTH 
  {  SDS_SIDE_ACE,  SDS_SIDE_NONE, SDS_SIDE_PARD, SDS_SIDE_NONE, SDS_SIDE_NONE },
  {  SDS_SIDE_ACE,  SDS_SIDE_NONE, SDS_SIDE_BOTH, SDS_SIDE_NONE, SDS_SIDE_NONE },
  {  SDS_SIDE_BOTH, SDS_SIDE_NONE, SDS_SIDE_PARD, SDS_SIDE_NONE, SDS_SIDE_NONE },
  {  SDS_SIDE_BOTH, SDS_SIDE_NONE, SDS_SIDE_BOTH, SDS_SIDE_NONE, SDS_SIDE_NONE }
};

// A lot of the combinations can't happen in practice
const cmpDetailType 
  cmpDetailMatrix[SDS_HEADER_CMP_SIZE][SDS_HEADER_CMP_SIZE] =
{
  // SDS_HEADER_SAME
  { 
    SDS_HEADER_SAME,            // SDS_HEADER_SAME
    SDS_HEADER_PLAY_NEW_BETTER, // SDS_HEADER_PLAY_NEW_BETTER
    SDS_HEADER_PLAY_OLD_BETTER, // SDS_HEADER_PLAY_OLD_BETTER
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_PLAY_DIFFERENT
    SDS_HEADER_RANK_NEW_BETTER, // SDS_HEADER_RANK_NEW_BETTER
    SDS_HEADER_RANK_OLD_BETTER, // SDS_HEADER_RANK_OLD_BETTER
    SDS_HEADER_RANK_DIFFERENT   // SDS_HEADER_RANK_DIFFERENT
  },

  // SDS_HEADER_PLAY_NEW_BETTER
  {
    SDS_HEADER_PLAY_NEW_BETTER, // SDS_HEADER_SAME
    SDS_HEADER_PLAY_NEW_BETTER, // SDS_HEADER_PLAY_NEW_BETTER
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_PLAY_OLD_BETTER
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_PLAY_DIFFERENT
    SDS_HEADER_PLAY_NEW_BETTER, // SDS_HEADER_RANK_NEW_BETTER
    SDS_HEADER_PLAY_NEW_BETTER, // SDS_HEADER_RANK_OLD_BETTER
    SDS_HEADER_PLAY_NEW_BETTER  // SDS_HEADER_RANK_DIFFERENT
  },

  // SDS_HEADER_PLAY_OLD_BETTER
  {
    SDS_HEADER_PLAY_OLD_BETTER, // SDS_HEADER_SAME
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_PLAY_NEW_BETTER
    SDS_HEADER_PLAY_OLD_BETTER, // SDS_HEADER_PLAY_OLD_BETTER
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_PLAY_DIFFERENT
    SDS_HEADER_PLAY_OLD_BETTER, // SDS_HEADER_RANK_NEW_BETTER
    SDS_HEADER_PLAY_OLD_BETTER, // SDS_HEADER_RANK_OLD_BETTER
    SDS_HEADER_PLAY_OLD_BETTER  // SDS_HEADER_RANK_DIFFERENT
  },

  // SDS_HEADER_PLAY_DIFFERENT
  {
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_SAME
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_PLAY_NEW_BETTER
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_PLAY_OLD_BETTER
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_PLAY_DIFFERENT
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_RANK_NEW_BETTER
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_RANK_OLD_BETTER
    SDS_HEADER_PLAY_DIFFERENT   // SDS_HEADER_RANK_DIFFERENT
  },

  // SDS_HEADER_RANK_NEW_BETTER
  {
    SDS_HEADER_RANK_NEW_BETTER, // SDS_HEADER_SAME
    SDS_HEADER_PLAY_NEW_BETTER, // SDS_HEADER_PLAY_NEW_BETTER
    SDS_HEADER_PLAY_OLD_BETTER, // SDS_HEADER_PLAY_OLD_BETTER
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_PLAY_DIFFERENT
    SDS_HEADER_RANK_NEW_BETTER, // SDS_HEADER_RANK_NEW_BETTER
    SDS_HEADER_RANK_DIFFERENT,  // SDS_HEADER_RANK_OLD_BETTER
    SDS_HEADER_RANK_DIFFERENT   // SDS_HEADER_RANK_DIFFERENT
  },

  // SDS_HEADER_RANK_OLD_BETTER
  {
    SDS_HEADER_RANK_OLD_BETTER, // SDS_HEADER_SAME
    SDS_HEADER_PLAY_NEW_BETTER, // SDS_HEADER_PLAY_NEW_BETTER
    SDS_HEADER_PLAY_OLD_BETTER, // SDS_HEADER_PLAY_OLD_BETTER
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_PLAY_DIFFERENT
    SDS_HEADER_RANK_DIFFERENT,  // SDS_HEADER_RANK_NEW_BETTER
    SDS_HEADER_RANK_OLD_BETTER, // SDS_HEADER_RANK_OLD_BETTER
    SDS_HEADER_RANK_DIFFERENT   // SDS_HEADER_RANK_DIFFERENT
  },

  // SDS_HEADER_RANK_DIFFERENT
  {
    SDS_HEADER_RANK_DIFFERENT,  // SDS_HEADER_SAME
    SDS_HEADER_PLAY_NEW_BETTER, // SDS_HEADER_PLAY_NEW_BETTER
    SDS_HEADER_PLAY_OLD_BETTER, // SDS_HEADER_PLAY_OLD_BETTER
    SDS_HEADER_PLAY_DIFFERENT,  // SDS_HEADER_PLAY_DIFFERENT
    SDS_HEADER_RANK_DIFFERENT,  // SDS_HEADER_RANK_NEW_BETTER
    SDS_HEADER_RANK_DIFFERENT,  // SDS_HEADER_RANK_OLD_BETTER
    SDS_HEADER_RANK_DIFFERENT   // SDS_HEADER_RANK_DIFFERENT
  }
};


const char SDS_POS_NAMES[7][6] = 
{ 
  "Ace", "LHO", "Pard", "RHO", "Both", "Open", "Defs" 
};

enum histIndexType
{
  HIST_ORIG_COUNT = 0,
  HIST_ACE_SHORT = 1,
  HIST_BLOCKED = 2,
  HIST_PARD_VOID = 3,
  HIST_CRASH = 4,
  HIST_SINGLE = 5,
  HIST_CASHES = 6,
  HIST_COMPLEX = 7,
  HIST_SIZE = 8
};

const char HIST_NAMES[HIST_SIZE][10] =
{
  "Lengths",
  "Ace tops",
  "Blocked",
  "Pd void",
  "Crash",
  "One",
  "Cashes",
  "General"
};

enum manualMoveType
{
  MANUAL_NONE = 0,
  MANUAL_SHORT = 1,
  MANUAL_ALL = 2
};

struct singleType
{
  unsigned moveNo;
  unsigned char declLen;
  unsigned char minLen;
  unsigned char oppLen;
};

#endif

