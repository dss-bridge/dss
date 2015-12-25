CC       	= cl
CC_FLAGS        = /O2 /Oi /Ot /Oy /GL /Zi /EHsc
CC_LINK_DEBUG   = /INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF
LD_FLAGS	= 


SOURCE_FILES 	=		\
        Stats.cpp               \
        DefList.cpp             \
        MoveList.cpp            \
        SideMoveList.cpp        \
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
        Hash.cpp                \
        Twiddle.cpp             \
        Dist.cpp                \
        misc.cpp                \
        single.cpp

# OBJ_FILES 	= $(subst .cpp,.obj,$(SOURCE_FILES))
OBJ_FILES 	= $(SOURCE_FILES:.cpp=.obj)


test:	$(OBJ_FILES) 
	link /LTCG $(CC_LINK_DEBUG) $(OBJ_FILES) /out:single.exe


.cpp.obj:
	$(CC) $(CC_FLAGS) /c /EHsc $<

%.obj:	%.cpp
	$(CC) $(CC_FLAGS) /c /EHsc $<

depend:
	makedepend -Y -o.obj -- $(CC_FLAGS) -- $(SOURCE_FILES)

clean:
	rm -f $(OBJ_FILES) single.exe single.exp 

# DO NOT DELETE

Stats.obj: portab.h Stats.h
DefList.obj: DefList.h AltList.h TrickList.h Segment.h Trick.h cst.h
DefList.obj: Holding.h Header.h AltMatrix1D.h AltMatrix2D.h
MoveList.obj: cst.h MoveList.h Holding.h Trick.h DefList.h AltList.h
MoveList.obj: TrickList.h Segment.h Header.h AltMatrix1D.h AltMatrix2D.h
MoveList.obj: Hash.h SideMoveList.h misc.h
SideMoveList.obj: cst.h SideMoveList.h Holding.h Trick.h DefList.h AltList.h
SideMoveList.obj: TrickList.h Segment.h Header.h AltMatrix1D.h AltMatrix2D.h
SideMoveList.obj: Hash.h misc.h
Trick.obj: cst.h Trick.h Holding.h
Segment.obj: cst.h Segment.h Trick.h Holding.h
TrickList.obj: cst.h TrickList.h Segment.h Trick.h Holding.h Header.h
AltList.obj: cst.h AltList.h TrickList.h Segment.h Trick.h Holding.h Header.h
AltList.obj: AltMatrix1D.h AltMatrix2D.h
AltMatrix1D.obj: AltMatrix1D.h cst.h
AltMatrix2D.obj: AltMatrix2D.h cst.h
Holding.obj: cst.h Holding.h Trick.h
LoopHold.obj: cst.h LoopHold.h DefList.h AltList.h TrickList.h Segment.h
LoopHold.obj: Trick.h Holding.h Header.h AltMatrix1D.h AltMatrix2D.h
SimpleMoves.obj: cst.h DefList.h AltList.h TrickList.h Segment.h Trick.h
SimpleMoves.obj: Holding.h Header.h AltMatrix1D.h AltMatrix2D.h MoveList.h
SimpleMoves.obj: Hash.h SideMoveList.h SimpleMoves.h ComplexMoves.h Twiddle.h
SimpleMoves.obj: Dist.h LoopHold.h
ComplexMoves.obj: portab.h cst.h DefList.h AltList.h TrickList.h Segment.h
ComplexMoves.obj: Trick.h Holding.h Header.h AltMatrix1D.h AltMatrix2D.h
ComplexMoves.obj: MoveList.h Hash.h SideMoveList.h SimpleMoves.h
ComplexMoves.obj: ComplexMoves.h Stats.h
Header.obj: Header.h cst.h Trick.h Holding.h
Hash.obj: Hash.h Header.h cst.h
Twiddle.obj: cst.h MoveList.h Holding.h Trick.h DefList.h AltList.h
Twiddle.obj: TrickList.h Segment.h Header.h AltMatrix1D.h AltMatrix2D.h
Twiddle.obj: Hash.h SideMoveList.h SimpleMoves.h Twiddle.h
Dist.obj: Holding.h cst.h Trick.h DefList.h AltList.h TrickList.h Segment.h
Dist.obj: Header.h AltMatrix1D.h AltMatrix2D.h SimpleMoves.h
misc.obj: misc.h
single.obj: portab.h cst.h MoveList.h Holding.h Trick.h DefList.h AltList.h
single.obj: TrickList.h Segment.h Header.h AltMatrix1D.h AltMatrix2D.h Hash.h
single.obj: SideMoveList.h SimpleMoves.h ComplexMoves.h Stats.h Twiddle.h
single.obj: Dist.h LoopHold.h
