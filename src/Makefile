EXPORTER	= Exports.def

SOURCE_FILES 	=		\
        Stats.cpp               \
        DefList.cpp             \
        MoveList.cpp            \
        Trick.cpp               \
        Segment.cpp             \
        TrickList.cpp           \
        AltList.cpp             \
	AltMatrix1D.cpp		\
	AltMatrix2D.cpp		\
        Holding.cpp             \
        LoopHold.cpp            \
        SimpleMoves.cpp         \
        ComplexMoves.cpp        \
        Header.cpp              \
        Twiddle.cpp             \
        Dist.cpp

TEST		= single
ITEST		= single

# Compilers

CC_VS     	= cl

# Compiler and linker flags

CC_FLAGS_VS     = /O2 /Oi /Ot /Oy /GL /Zi
CC_LINK_DEBUG   = /INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF

LD_FLAGS_VS	= 
LD_FLAGS_LINUX	=
LD_FLAGS_MACOS	=

ifeq ($(DDS_THREADS),none)
DDS_THR		= -DDDS_THREADS_SINGLE
else
DDS_THR		= 
endif

WINDRES_FLAG	= -F pe-i386

CC		= $(CC_VS)
CC_FLAGS	= $(CC_FLAGS_VS)
LD_FLAGS	= $(LD_FLAGS_VS)

O_FILES 	= $(subst .cpp,.o,$(SOURCE_FILES))

OBJ_FILES 	= $(subst .cpp,.obj,$(SOURCE_FILES))

TEST_SOURCE_FILES = $(TEST).cpp
TEST_O_FILES 	= $(subst .cpp,.o,$(TEST_SOURCE_FILES)) Stats.o
TEST_OBJ_FILES 	= $(subst .cpp,.obj,$(TEST_SOURCE_FILES)) Stats.obj

ITEST_SOURCE_FILES = $(ITEST).cpp
ITEST_O_FILES 	= $(subst .cpp,.o,$(ITEST_SOURCE_FILES))
ITEST_OBJ_FILES = $(subst .cpp,.obj,$(ITEST_SOURCE_FILES))

# Linking directly.

test:	$(OBJ_FILES) $(ITEST_OBJ_FILES)
	link /LTCG $(CC_LINK_DEBUG) $(OBJ_FILES) $(ITEST_OBJ_FILES) /out:$(ITEST).exe


%.o:	%.cpp
	$(CC) $(CC_FLAGS) $(DDS_THR) -c $<

%.obj:	%.cpp
	$(CC_VS) $(CC_FLAGS) $(DDS_THR) /c /EHsc $<


depend:
	makedepend -Y -o.obj -- $(CC_FLAGS) -- $(SOURCE_FILES) $(TEST).cpp

clean:
	rm -f $(OBJ_FILES) $(TEST_O_FILES) $(TEST_OBJ_FILES) $(TEST).o $(ITEST).o $(TEST).obj $(ITEST).obj $(DLL) $(TEST).exe $(ITEST).exe $(DLLBASE).lib $(DLLBASE).exp $(DLLBASE).def $(TEST).exp $(DLLBASE).o $(DLLBASE).obj $(DLLBASE).res


# DO NOT DELETE

Stats.obj: portab.h Stats.h
DefList.obj: cst.h DefList.h AltList.h TrickList.h Segment.h Trick.h
DefList.obj: Holding.h Header.h AltMatrix1D.h AltMatrix2D.h
MoveList.obj: portab.h cst.h MoveList.h Holding.h Trick.h DefList.h AltList.h
MoveList.obj: TrickList.h Segment.h Header.h AltMatrix1D.h AltMatrix2D.h
Trick.obj: cst.h Trick.h Holding.h portab.h
Segment.obj: cst.h Segment.h Trick.h Holding.h Header.h portab.h
TrickList.obj: cst.h TrickList.h Segment.h Trick.h Holding.h Header.h
TrickList.obj: portab.h
AltList.obj: cst.h AltList.h TrickList.h Segment.h Trick.h Holding.h Header.h
AltList.obj: AltMatrix1D.h AltMatrix2D.h portab.h
AltMatrix1D.obj: cst.h AltMatrix1D.h portab.h
AltMatrix2D.obj: cst.h AltMatrix2D.h portab.h
Holding.obj: cst.h Holding.h Trick.h
LoopHold.obj: cst.h LoopHold.h DefList.h AltList.h TrickList.h Segment.h
LoopHold.obj: Trick.h Holding.h Header.h AltMatrix1D.h AltMatrix2D.h
SimpleMoves.obj: cst.h DefList.h AltList.h TrickList.h Segment.h Trick.h
SimpleMoves.obj: Holding.h Header.h AltMatrix1D.h AltMatrix2D.h MoveList.h
SimpleMoves.obj: SimpleMoves.h ComplexMoves.h Twiddle.h Dist.h LoopHold.h
ComplexMoves.obj: portab.h cst.h DefList.h AltList.h TrickList.h Segment.h
ComplexMoves.obj: Trick.h Holding.h Header.h AltMatrix1D.h AltMatrix2D.h
ComplexMoves.obj: MoveList.h SimpleMoves.h ComplexMoves.h Stats.h
Header.obj: cst.h Header.h Trick.h Holding.h
Twiddle.obj: cst.h MoveList.h Holding.h Trick.h DefList.h AltList.h
Twiddle.obj: TrickList.h Segment.h Header.h AltMatrix1D.h AltMatrix2D.h
Twiddle.obj: SimpleMoves.h Twiddle.h
Dist.obj: Holding.h cst.h Trick.h DefList.h AltList.h TrickList.h Segment.h
Dist.obj: Header.h AltMatrix1D.h AltMatrix2D.h SimpleMoves.h
single.obj: portab.h cst.h MoveList.h Holding.h Trick.h DefList.h AltList.h
single.obj: TrickList.h Segment.h Header.h AltMatrix1D.h AltMatrix2D.h
single.obj: SimpleMoves.h ComplexMoves.h Stats.h Twiddle.h Dist.h LoopHold.h
