appname = alpha

CXX = g++
CXXFLAGS = -Wall -g -std=c++20

SRCS = main.cpp src/tokenizer.cpp src/readwrite.cpp


.PHONY: test
test: $(appname)
	./$(appname) test

.PHONY: build
build: $(appname)

$(appname): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(appname)

.PHONY: help
help:
	@echo "Commands for compiler alpha:"
	@echo "Run tests: make test"
	@echo "Build alpha: make build"
	@echo "Show this help: make help"
	@echo "Clean binaries: make clean"

.PHONY: clean
clean:
	$(RM) $(appname)
