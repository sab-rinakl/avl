CXX = g++
CPPFLAGS = -Wall -g

all: avl

avl: bst.h avlbst.h 
	$(CXX) $(CPPFLAGS) avlbst.h bst.h