CXX = g++
CXXFLAGS = -g -std=c++11 -Wall -Wextra -Wno-sign-compare -O3

#SOURCES = src/backlinks_compression.h src/backlinks_compression.cc

LIB = -lgtest -lgtest_main -lpthread
GTEST_ROOT= lib/gtest-1.6.0

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SRCS := $(wildcard $(SRCDIR)/*.cc)
INCS := $(wildcard $(SRCDIR)/*.h)
OBJS := $(SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

all: bin bin/compress bin/develop

#test: bin/test

bin:
	mkdir -p bin

bin/compress: samples/compress_main.cc $(OBJS)
	$(CXX) $(CXXFLAGS) -Isrc -o $@  $^

bin/develop: samples/develop_main.cc  $(OBJS)
	$(CXX) $(CXXFLAGS) -Isrc -o $@  $^

#bin/test: src/backlinks_compression_test.cc ${SRCS}  $(OBJS)
#	$(CXX) $(CXXFLAGS) -o $@ $^ $(OBJS)-I $(GTEST_ROOT)/include -L $(GTEST_ROOT)/lib $(LIB)
#	./$@

.PHONY:	test clean

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.cc
	    @$(CC) $(CXXFLAGS) -c $< -o $@
			    @echo "Compiled "$<" successfully!"

clean:
	rm -f $(OBJS) $(OBJS:%.o=%.d) $(TARGET)
	rm -rf bin
