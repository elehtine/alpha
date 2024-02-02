appname = alpha

CXX = g++
BOOST = -lboost_system -lboost_filesystem
CXXFLAGS = -Wall -g -std=c++17

SRCS = main.cpp \
	   src/compiler.cpp src/interpreter.cpp src/parser.cpp src/tokeniser.cpp \
	   src/command.cpp src/test.cpp \
	   src/tools/readwrite.cpp src/tools/exceptions.cpp


.PHONY: test
test: $(appname)
	./$(appname) test

$(appname): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(BOOST) -o $(appname)

.PHONY: clean
clean:
	$(RM) $(appname)

.PHONY: help
help:
	@echo "Commands for compiler alpha:"
	@echo "  make test  - Run tests"
	@echo "  make alpha - Build alpha"
	@echo "  make clean - Clean binaries:"
	@echo "  make help  - Show this help:"
