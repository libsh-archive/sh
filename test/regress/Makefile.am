INCLUDES = -I../../src
AM_CXXFLAGS = -Wall -g -O0
AM_LDFLAGS = -g -L../../src
LDADD = -lsh -lGL
TESTS = ADD MUL DIV SLT SLE SGT SGE SEQ SNE ABS ACOS ASIN ATAN ATAN2 CEIL COS DOT \
	EXP EXP2 EXP10 FLR FRAC LRP MAD MAX MIN MOD LOG LOG2 LOG10 POW RCP RSQ SIN SQRT TAN NORM XPD COND
check_PROGRAMS = $(TESTS)
BACKENDS = arb gcc
EXTRA_DIST = regress.py

clean:
	rm -f *.log
	rm -f testlist
	rm -f $(TESTS)
	rm -f *.o
	rm -f $(TESTS:=.cpp)
	rm -rf .libs

%.check: %
	@for j in $(BACKENDS) ; do ./$< $$j 2> $*.$$j.log ; done

check: $(TESTS:=.check)

testlist: ./regress.py
	./regress.py > testlist

ADD_SOURCES = ADD.cpp test.cpp
ADD.cpp: testlist
MUL_SOURCES = MUL.cpp test.cpp
MUL.cpp: testlist
DIV_SOURCES = DIV.cpp test.cpp
DIV.cpp: testlist
SLT_SOURCES = SLT.cpp test.cpp
SLT.cpp: testlist
SLE_SOURCES = SLE.cpp test.cpp
SLE.cpp: testlist
SGT_SOURCES = SGT.cpp test.cpp
SGT.cpp: testlist
SGE_SOURCES = SGE.cpp test.cpp
SGE.cpp: testlist
SEQ_SOURCES = SEQ.cpp test.cpp
SEQ.cpp: testlist
SNE_SOURCES = SNE.cpp test.cpp
SNE.cpp: testlist
ABS_SOURCES = ABS.cpp test.cpp
ABS.cpp: testlist
ACOS_SOURCES = ACOS.cpp test.cpp
ACOS.cpp: testlist
ASIN_SOURCES = ASIN.cpp test.cpp
ASIN.cpp: testlist
ATAN_SOURCES = ATAN.cpp test.cpp
ATAN.cpp: testlist
ATAN2_SOURCES = ATAN2.cpp test.cpp
ATAN2.cpp: testlist
CEIL_SOURCES = CEIL.cpp test.cpp
CEIL.cpp: testlist
COS_SOURCES = COS.cpp test.cpp
COS.cpp: testlist
DOT_SOURCES = DOT.cpp test.cpp
DOT.cpp: testlist
EXP_SOURCES = EXP.cpp test.cpp
EXP.cpp: testlist
EXP2_SOURCES = EXP2.cpp test.cpp
EXP2.cpp: testlist
EXP10_SOURCES = EXP10.cpp test.cpp
EXP10.cpp: testlist
FLR_SOURCES = FLR.cpp test.cpp
FLR.cpp: testlist
FRAC_SOURCES = FRAC.cpp test.cpp
FRAC.cpp: testlist
LRP_SOURCES = LRP.cpp test.cpp
LRP.cpp: testlist
MAD_SOURCES = MAD.cpp test.cpp
MAD.cpp: testlist
MAX_SOURCES = MAX.cpp test.cpp
MAX.cpp: testlist
MIN_SOURCES = MIN.cpp test.cpp
MIN.cpp: testlist
MOD_SOURCES = MOD.cpp test.cpp
MOD.cpp: testlist
LOG_SOURCES = LOG.cpp test.cpp
LOG.cpp: testlist
LOG2_SOURCES = LOG2.cpp test.cpp
LOG2.cpp: testlist
LOG10_SOURCES = LOG10.cpp test.cpp
LOG10.cpp: testlist
POW_SOURCES = POW.cpp test.cpp
POW.cpp: testlist
RCP_SOURCES = RCP.cpp test.cpp
RCP.cpp: testlist
RSQ_SOURCES = RSQ.cpp test.cpp
RSQ.cpp: testlist
SIN_SOURCES = SIN.cpp test.cpp
SIN.cpp: testlist
SQRT_SOURCES = SQRT.cpp test.cpp
SQRT.cpp: testlist
TAN_SOURCES = TAN.cpp test.cpp
TAN.cpp: testlist
NORM_SOURCES = NORM.cpp test.cpp
NORM.cpp: testlist
XPD_SOURCES = XPD.cpp test.cpp
XPD.cpp: testlist
COND_SOURCES = COND.cpp test.cpp
COND.cpp: testlist
