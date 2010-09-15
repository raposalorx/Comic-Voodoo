######################################################
##                                                  ##
## Makefile ComicVoodoo								##
##                                                  ##
######################################################

##==========##
## Settings ##
##==========##

CXX = g++
SED = sed
RM = rm -f
MV = mv -f
CP = cp -f

.SUFFIXES: .o .cpp

main = comics
cxxflags = -W -Wall -Wextra -ansi
ldflags = -L/opt/local/lib
libs = -lcurl -lpcrecpp -lyaml-cpp
includes = -I/opt/local/include
source := ${wildcard src/*.cpp}
objects := ${addprefix bin/,${notdir ${source:.cpp=.o}}}
dependencies := ${source:.cpp=.d}

##===============##
## Build Targets ##
##===============##

.PHONY: all
all: ${main}
	@echo =======================================
	@echo ComicVoodoo has been built successfully
	@echo =======================================

${main}: ${objects}
	${CXX} ${ldflags} ${includes} ${objects} -o ${main} ${libs}

bin/%.o: src/%.cpp
	${CXX} -c ${cxxflags} ${includes} -MMD -o $@ $<

.PHONY: clean
clean:
	-@${RM} ${main} *~ ${addprefix bin/,${notdir ${dependencies}}} ${objects}

ifneq ($(MAKECMDGOALS),clean)
-include ${dependencies}
endif
