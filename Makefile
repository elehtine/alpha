appname = alpha

CXX = g++
CXXFLAGS = -Wall -g -std=c++20

SRCS = main.cpp tokenizer.cpp

.PHONY: all
all: $(appname)
	./$(appname)

$(appname): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(appname)

.PHONY: clean
clean:
	$(RM) $(appname)
