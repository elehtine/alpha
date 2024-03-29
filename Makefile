appname = alpha

CXX = g++
BOOST = -lboost_system -lboost_filesystem
CXXFLAGS = -Wall -g -std=c++17

SRCS = main.cpp \
	   src/tokeniser.cpp \
	   src/parser.cpp src/ir_generator.cpp src/asm_generator.cpp \
	   src/interpreter.cpp src/checker.cpp \
	   src/compiler.cpp src/command.cpp src/intrinsics.cpp \
	   src/types/source.cpp src/types/token.cpp src/types/ast.cpp \
	   src/types/value.cpp src/types/type.cpp src/types/ir.cpp \
	   src/types/location.cpp src/types/symtab.cpp \
	   src/tools/files.cpp src/tools/printer.cpp src/tools/exceptions.cpp
OBJS = $(SRCS:.cpp=.o)


.PHONY: test
test: $(appname)
	./$(appname) test

$(appname): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(BOOST) -o $(appname)

.PHONY: clean
clean:
	$(RM) $(appname) $(OBJS)

.PHONY: help
help:
	@echo "Commands for compiler alpha:"
	@echo "  make help  - show this help"
	@echo "  make       - run tests"
	@echo "  make clean - remove binaries"
