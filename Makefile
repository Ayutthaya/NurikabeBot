TST=test
SRC_TST=test.cpp

all: clean ${TST}

${TST}: clean_test
	${CXX} -O2 ${SRC_TST} -o ${TST}

check: ${TST}
	./${TST}

clean: clean_test

clean_test:
	rm -f ${TST}
