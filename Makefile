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

.SUFFIXES: .o .cc

main = comicvoodoo
srcdir = ./src
cxxflags ?= -W -Wall -Wextra -ansi -g
ldflags ?= -L/opt/local/lib
libs ?= -lcurl -lpcrecpp -lyaml-cpp -largtable2
includes ?= -I/opt/local/include
source := ${shell find ${srcdir} -name *.cc}
objects := ${source:.cc=.o}
dependencies := ${source:.cc=.d}

test = unittest
test_srcdir = ./test
test_cxxflags ?= ${cxxflags}
test_ldflags ?= ${ldflags}
test_libs ?= ${libs} -lUnitTest++
test_includes ?= ${includes}
test_source := ${shell find ${test_srcdir} -name *.cc}
test_objects := ${test_source:.cc=.o}
test_dependencies := ${test_source:.cc=.d}

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
	-@wc -l ${shell find ${srcdir} ${test_srcdir} -regex "\.(h|cc)$$"}

${main}: ${objects}
	${CXX} ${ldflags} ${includes} ${libs} ${objects} -o ${main}

${test}: ${test_objects} ${objects}
	${CXX} ${test_ldflags} ${test_includes} ${test_libs} ${test_objects} ${filter-out ${srcdir}/main.o,${objects}} -o ${test}

${srcdir}/%.o: ${srcdir}/%.cc
	${CXX} -c ${cxxflags} ${includes} -MMD -o $@ $<

${test_srcdir}/%.o: ${test_srcdir}/%.cc
	${CXX} -c ${test_cxxflags} ${test_includes} -MMD -o $@ $<

ifneq ($(MAKECMDGOALS),clean)
-include ${dependencies}
-include ${test_dependencies}
endif
