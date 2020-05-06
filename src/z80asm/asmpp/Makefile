all: asmpp.exe

asmpp.exe: asmpp.c common.c common.h errors.c errors.h scan.c scan.h tokens.c tokens.h utils.c utils.h
	cl /nologo asmpp.c common.c errors.c scan.c tokens.c utils.c

clean:
	del /q *.obj *.exe

test: asmpp.exe
	perl test.pl
