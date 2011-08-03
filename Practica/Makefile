all: erlanger

erlanger: erlanger.c scan.c err.c graphgen.o graphwriter.o 
	g++ -o $@ $^

erlanger.c err.c parser.dlg tokens.h: erlanger.g
	antlr -gt $<

mode.h scan.c: parser.dlg
	dlg $< scan.c

graphgen.o graphwriter.o: %.o: %.cc %.hh
	g++ -c $<

clean:
	rm erlanger erlanger.c graphwriter.o graphgen.o mode.h parser.dlg scan.c tokens.h err.c