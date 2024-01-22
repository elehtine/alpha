appname = alpha

CXX = g++
CXXFLAGS = -Wall -g -std=c++17

SRCS = main.cpp src/tokeniser.cpp src/readwrite.cpp src/command.cpp


.PHONY: test
test: $(appname)
	./$(appname) test

$(appname): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(appname)

.PHONY: clean
clean:
	$(RM) $(appname) tests/*.output

.PHONY: help
help:
	@echo "Commands for compiler alpha:"
	@echo "  make test  - Run tests"
	@echo "  make alpha - Build alpha"
	@echo "  make clean - Clean binaries:"
	@echo "  make help  - Show this help:"
