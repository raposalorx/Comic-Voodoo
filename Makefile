######################################################
##                                                  ##
## Makefile ComicVoodoo								##
##                                                  ##
######################################################

##==========##
## Settings ##
##==========##

SHELL = /bin/sh
CXX = g++
RM = rm -f

.SUFFIXES: .o .cpp

main = comicvoodoo
srcdir = ./src
cxxflags ?= -W -Wall -Wextra -ansi -g
ldflags ?=
libs ?= -lcurl -lpcrecpp -lyaml-cpp -largtable2
includes ?=
source := ${shell find ${srcdir} -name *.cpp}
objects := ${source:.cpp=.o}
dependencies := ${source:.cpp=.d}

test = unittest
test_srcdir = ./test
test_cxxflags ?= ${cxxflags}
test_ldflags ?= ${ldflags}
test_libs ?= ${libs} -lUnitTest++
test_includes ?= ${includes} -I/usr/include/unittest++
test_source := ${shell find ${test_srcdir} -name *.cpp}
test_objects := ${test_source:.cpp=.o}
test_dependencies := ${test_source:.cpp=.d}

##===============##
## Build Targets ##
##===============##

.PHONY: all
all: ${test}
	@echo ===================================
	@echo Project has been built successfully
	@echo ===================================
	@./${test}

#.PHONY: install
#install: ${main}

.PHONY: clean
clean:
	-@${RM} ${main} ${test} ${objects} ${test_objects} ${dependencies} ${test_dependencies}

.PHONY: linecount
linecount:
	-@wc -l ${shell find ${srcdir} ${test_srcdir} -regex ".*\.\(h\|cc\)$$"}

${main}: ${objects}
	${CXX} ${ldflags} ${includes} ${objects} -o ${main} ${libs}

${test}: ${test_objects} ${objects}
	${CXX} ${test_ldflags} ${test_includes} ${test_objects} ${filter-out ${srcdir}/main.o,${objects}} -o ${test} ${test_libs}

${srcdir}/%.o: ${srcdir}/%.cpp
	${CXX} -c ${cxxflags} ${includes} -MMD -o $@ $<

${test_srcdir}/%.o: ${test_srcdir}/%.cpp
	${CXX} -c ${test_cxxflags} ${test_includes} -MMD -o $@ $<

ifneq ($(MAKECMDGOALS),clean)
-include ${dependencies}
-include ${test_dependencies}
endif
