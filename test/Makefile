SH_DIR = ..
SUBDIRS = 
include $(SH_DIR)/default.mk

BACKENDS = $(SH_DIR)/backends/sm/Backend.o
LIBS += $(BACKENDS) $(SH_LIB) 
TESTS = $(OBJECTS:.o=)

all: $(TESTS)

check: $(TESTS)
	for i in $(TESTS) ; do ./$$i ; done

clean: clean-tests

clean-tests:
	rm -f $(TESTS)

$(TESTS): %: %.o $(SH_LIB) $(BACKENDS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

%.ps: %.dot
	dot -o $@ -Tps $<

