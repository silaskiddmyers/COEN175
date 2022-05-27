CXX		= g++ 
CXXFLAGS	= -g -Wall -std=c++11
OBJS		= lexer.o parser.o string.o type.o checker.o symbol.o scope.o
PROG		= scc

all:		$(PROG)

$(PROG):	$(OBJS)
			$(CXX) -o $(PROG) $(OBJS)

clean:;		$(RM) $(PROG) core *.o
