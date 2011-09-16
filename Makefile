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
libs ?= -lcurl -lpcrecpp -lyaml-cpp -largtable2 -lsqlite3
includes ?=
source := ${shell find ${srcdir} -name *.cpp}
objects := ${source:.cpp=.o}
dependencies := ${source:.cpp=.d}

##===============##
## Build Targets ##
##===============##

.PHONY: all
all: ${main}
	@echo ===================================
	@echo Project has been built successfully
	@echo ===================================

#.PHONY: install
#install: ${main}

.PHONY: clean
clean:
	-@${RM} ${main} ${test} ${objects} ${test_objects} ${dependencies} ${test_dependencies}

.PHONY: linecount
linecount:
	-@wc -l ${shell find ${srcdir} ${test_srcdir} -regex ".*\.\(h\|cpp\)$$"}

${main}: ${objects}
	${CXX} ${ldflags} ${includes} ${objects} -o ${main} ${libs}

${srcdir}/%.o: ${srcdir}/%.cpp
	${CXX} -c ${cxxflags} ${includes} -MMD -o $@ $<

ifneq ($(MAKECMDGOALS),clean)
-include ${dependencies}
-include ${test_dependencies}
endif
