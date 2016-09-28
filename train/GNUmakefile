PROGNAME      = tmvaTrain
SOURCEFILES   = tmvaTrain.cc
INCLFILES     = Event.h
INCLDIR       = ../inc
INCLUDES      = $(INCLDIR)/$(INCLFILES)
CFLAGS       += -I$(INCLDIR)
OBJS          = $(patsubst %.cc, %.o, $(SOURCEFILES))
ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
LDFLAGS       = -O
LIBS         += $(ROOTLIBS)
CFLAGS       += $(ROOTCFLAGS)

#
LIBS         += -lMinuit
LIBS         += -lTMVA

%.o: %.cc 
	g++ ${CFLAGS} -c  -g -o $@ $<

$(PROGNAME):    $(OBJS) $(INCLUDES)
	g++ -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

test:
	@echo $(CFLAGS)

clean:	
	-rm -f ${PROGNAME} ${OBJS}
