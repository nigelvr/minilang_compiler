CC=clang++
CFLAGS=-g -std=c++11 -Wno-deprecated-register -Wno-deprecated
LLVM_FLAGS=$(shell llvm-config --cxxflags --libs engine)
MAINSRC=ast.cpp main.cpp
BISONGENC=ex.tab.c
BISONGEN=ex.tab.c ex.tab.h
BISONSRC=ex.y
FLEXGEN=lex.yy.c
FLEXSRC=ex.l
TARGET=lang
 
$(TARGET): $(MAINSRC) $(BISONGEN) $(FLEXGEN)
	$(CC) $(MAINSRC) $(BISONGENC) $(FLEXGEN) $(CFLAGS) $(LLVM_FLAGS) -o $(TARGET)

$(BISONGEN): $(BISONSRC)
	bison -d $(BISONSRC) -Wcounterexamples

$(FLEXGEN): $(FLEXSRC)
	flex $(FLEXSRC)

.PHONY: clean
clean:
	rm -f $(BISONGEN) $(FLEXGEN) $(TARGET)