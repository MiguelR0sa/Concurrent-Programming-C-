# to generate core file execute in bash:
#    ulimit -c unlimited

.PHONY: clean cleanall

OBJS=test.o

CPPFLAGS=-static -Wall -ggdb -pthread -I../../include -L../../lib    # if necessary add/remove options
SYMBOLS=-DEXIT_POLICY            # -DEXCEPTION_POLICY or -DEXIT_POLICY; for ascii output: -DACSII

all: test

test: $(OBJS) ../../lib/libsoconcur.a
	g++ $(SYMBOLS) $(CPPFLAGS) $(OBJS) $(OBJS_THREAD) -lsoconcur -o test

%.o: %.cpp
	g++ $(SYMBOLS) $(CPPFLAGS) -c $<

clean:
	rm -fv $(OBJS) core

cleanall: clean
	rm -fv test

