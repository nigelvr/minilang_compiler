CXX   = clang++
LLVM_FLAGS=$(shell llvm-config --cxxflags --libs engine) 
CXXFLAGS = -g -Wall -std=c++11 $(LLVM_FLAGS) -Wno-unused-but-set-variable -Wno-unused-command-line-argument -Wno-deprecated-register -Wno-register 
CPPOBJ = ast main minidriver emitter
SOBJ =  parser lexer
FILES = $(addsuffix .cpp, $(CPPOBJ))
OBJS  = $(addsuffix .o, $(CPPOBJ))
TARGET=minilang
CLEANLIST =  $(addsuffix .o, $(OBJ)) $(OBJS) miniparser.tab.cc \
	     miniparser.tab.hh location.hh position.hh \
	     stack.hh miniparser.output parser.o lexer.o lex.yy.cc ast.o emitter.o $(TARGET)\

.PHONY: all
all: $(TARGET)

$(TARGET): $(FILES)
	$(MAKE) $(SOBJ)
	$(MAKE) $(OBJS)
	$(CXX) $(CXXFLAGS) -v -o $(TARGET) $(OBJS) parser.o lexer.o $(LIBS)


parser: miniparser.yy
	bison -d -v miniparser.yy
	$(CXX) $(CXXFLAGS) -c -o parser.o miniparser.tab.cc

lexer: minilexer.l
	flex $<
	$(CXX)  $(CXXFLAGS) -c lex.yy.cc -o lexer.o

.PHONY: test
test:
	test/test0.pl

.PHONY: clean
clean:
	rm -rf $(CLEANLIST) *.tab.* *.output *~

