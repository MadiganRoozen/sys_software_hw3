# $Id: Makefile,v 1.55 2024/10/09 18:39:54 leavens Exp $
# Makefile for parser and static analysis in COP 3402

# Add .exe to the end of target to get that suffix in the rules
COMPILER = compiler
# Add .exe to the end of target to get that suffix in the rules
LEXER = lexer

# The name of the programming language
SPL = spl

# Tools used
CC = gcc
LEX = flex
LEXFLAGS =
# on Linux, the following can be used with gcc:
# CFLAGS = -fsanitize=address -static-libasan -g -std=c17 -Wall
CFLAGS = -g -std=c17 -Wall
ZIP = zip -9
YACC = bison -Wcounterexamples
YACCFLAGS = -Wall --locations -d -v
# Other Unix command names
MV = mv
RM = rm -f
CHMOD = chmod
ZIP = zip -9
# the zip file to submit on Webcourses
SUBMISSIONZIPFILE = submission.zip

# Add the names of your own files with a .o suffix to link them in the program
# You may edit the following definition of COMPILER_OBJECTS
# to get it to match the file names you are using.
# Note that the $(SPL).tab.o and $(SPL)_lexer.o files are created by compiling
# the corresponding .c files that were generated by bison and flex
# (respectively).  These are thus part of your solution and not provided.
# The files we provide are listed starting on the third line of this definition.
# You should not need the machine_types.o file
# and there is no parser_types.c file provided,
# but you could add machine_types.o and parser_types.o if need be.
# scope.o scope_check.o symtab.o \ 
COMPILER_OBJECTS =\
		$(SPL).tab.o $(SPL)_lexer.o \
		$(COMPILER)_main.o parser.o unparser.o id_use.o \
		id_attrs.o ast.o file_location.o utilities.o

# If you want to test the lexical analysis part separately,
# then you might want to build the lexer,
# and if so, then add the names of your own .o files for the lexer below
LEXER_OBJECTS = $(LEXER)_main.o $(LEXER).o $(SPL)_lexer.o \
		ast.o $(SPL).tab.o file_location.o utilities.o 

# different kinds of tests
ASTTESTS = hw3-asttest0.spl hw3-asttest1.spl hw3-asttest2.spl \
	hw3-asttest3.spl hw3-asttest4.spl hw3-asttest5.spl \
	hw3-asttest6.spl hw3-asttest7.spl hw3-asttest8.spl \
	hw3-asttest9.spl hw3-asttestA.spl hw3-asttestB.spl \
	hw3-asttestC.spl hw3-asttestD.spl
REGULARTESTS = hw3-test0.spl hw3-test1.spl hw3-test2.spl hw3-test3.spl \
	hw3-test4.spl hw3-test5.spl hw3-test6.spl hw3-test7.spl \
	hw3-test8.spl hw3-test9.spl hw3-testA.spl
ERRTESTS = hw3-errtest0.spl hw3-errtest1.spl hw3-errtest2.spl \
	hw3-errtest3.spl hw3-errtest4.spl hw3-errtest5.spl
PARSEERRTESTS = hw3-parseerrtest0.spl hw3-parseerrtest1.spl hw3-parseerrtest2.spl \
	hw3-parseerrtest3.spl hw3-parseerrtest4.spl hw3-parseerrtest5.spl \
	hw3-parseerrtest6.spl hw3-parseerrtest7.spl hw3-parseerrtest8.spl
NONDECLTESTS = $(ASTTESTS) $(REGULARTESTS) $(ERRTESTS) $(PARSEERRTESTS)
SCOPETESTS = hw3-scope-test0.spl hw3-scope-test1.spl  hw3-scope-test2.spl
DECLERRTESTS = hw3-declerrtest0.spl hw3-declerrtest1.spl hw3-declerrtest2.spl \
	hw3-declerrtest3.spl hw3-declerrtest4.spl hw3-declerrtest5.spl \
	hw3-declerrtest6.spl hw3-declerrtest7.spl hw3-declerrtest8.spl \
	hw3-declerrtest9.spl hw3-declerrtestA.spl hw3-declerrtestB.spl \
	hw3-declerrtestC.spl
DECLTESTS = $(SCOPETESTS) $(DECLERRTESTS)
GOODTESTS = $(ASTTESTS) $(REGULARTESTS) $(SCOPETESTS)
BADTESTS = $(ERRTESTS) $(PARSEERRTESTS) $(DECLERRTESTS)
# ALLTESTS is all of the test files, if you add more tests you can add to this list
ALLTESTS = $(NONDECLTESTS) $(DECLTESTS)
EXPECTEDOUTPUTS = $(ALLTESTS:.spl=.out)
# STUDENTESTOUTPUTS is all of the .myo files corresponding to the tests
# if you add more tests, you can add more to this list
# (or just add more to ALLTESTS, and those will be automatically added)
STUDENTTESTOUTPUTS = $(ALLTESTS:.spl=.myo)

.DEFAULT: $(COMPILER)
$(COMPILER): $(COMPILER_OBJECTS)
	$(CC) $(CFLAGS) -o $(COMPILER) $(COMPILER_OBJECTS)

$(COMPILER)_main.o: $(COMPILER)_main.c
	$(CC) $(CFLAGS) -c $<

$(SPL).tab.o: $(SPL).tab.c $(SPL).tab.h
	$(CC) $(CFLAGS) -c $<

$(SPL).tab.c $(SPL).tab.h: $(SPL).y ast.h parser_types.h machine_types.h 
	$(YACC) $(YACCFLAGS) $(SPL).y

.PHONY: start-bison-file
start-bison-file:
	@if test -f $(SPL).y; \
        then echo "$(SPL).y already exists, not starting it!" >&2; \
              exit 2 ; \
        fi
	cp bison_$(SPL)_y_top.y $(SPL).y
	chmod u+w $(SPL).y

$(SPL)_lexer.l: $(SPL).tab.h

.PRECIOUS: $(SPL)_lexer.c
$(SPL)_lexer.c: $(SPL)_lexer.l $(SPL).tab.h
	$(LEX) $(LEXFLAGS) $<

$(SPL)_lexer.o: $(SPL)_lexer.c ast.h utilities.h file_location.h
	$(CC) $(CFLAGS) -Wno-unused-but-set-variable -c $(SPL)_lexer.c

$(LEXER): $(LEXER_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(LEXER)_main.o: $(LEXER)_main.c
	$(CC) $(CFLAGS) -c $<

# rule for compiling individual .c files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

.PHONY: clean clean-lexer
clean:
	$(RM) *~ *.o '#'*
	$(RM) $(SPL).tab.c $(SPL).tab.h $(SPL).output
	$(RM) $(COMPILER).exe $(COMPILER)
	$(RM) $(LEXER).exe $(LEXER)
	$(RM) *.stackdump core
	$(RM) $(SUBMISSIONZIPFILE)

clean-lexer:
	$(RM) $(SPL)_lexer.c $(SPL)_lexer.h
	$(RM) $(LEXER).exe $(LEXER)

cleanall: clean clean-lexer
	$(RM) *.myo

.PRECIOUS: %.myo
%.myo: %.spl $(COMPILER)
	-./$(COMPILER) $< > $@ 2>&1

.PHONY: check-outputs check-nondecl-outputs check-decl-outputs
check-outputs: check-nondecl-outputs check-decl-outputs
	@echo 'Be sure to look for two test summaries above (nondeclaration and declaration tests)'

check-nondecl-outputs: $(COMPILER) $(NONDECLTESTS)
	@DIFFS=0; \
	for f in `echo $(NONDECLTESTS) | sed -e 's/\\.spl//g'`; \
	do \
		echo running "$$f.spl"; \
		./$(COMPILER) "$$f.spl" >"$$f.myo" 2>&1; \
		diff -w -B "$$f.out" "$$f.myo" && echo 'passed!' || DIFFS=1; \
	done; \
	if test 0 = $$DIFFS; \
	then \
		echo 'All nondeclaration tests passed!'; \
	else \
		echo 'Some nondeclaration test(s) failed!'; \
	fi

check-decl-outputs: $(COMPILER) $(DECLTESTS)
	@DIFFS=0; \
	for f in `echo $(DECLTESTS) | sed -e 's/\\.spl//g'`; \
	do \
		echo running "$$f.spl"; \
		./$(COMPILER) "$$f.spl" >"$$f.myo" 2>&1; \
		diff -w -B "$$f.out" "$$f.myo" && echo 'passed!' || DIFFS=1; \
	done; \
	if test 0 = $$DIFFS; \
	then \
		echo 'All declaration checking tests passed!'; \
	else \
		echo 'Some declaration checking test(s) failed!'; \
	fi

check-good-outputs: $(COMPILER) $(GOODTESTS)
	DIFFS=0; \
	for f in `echo $(GOODTESTS) | sed -e 's/\\.spl//g'`; \
	do \
		$(RM) "$$f.myo" ; \
		./$(COMPILER) $$f.spl >"$$f.myo" 2>&1; \
		diff -w -B "$$f.out" "$$f.myo" && echo 'passed!' || DIFFS=1; \
	done; \
	if test 0 = $$DIFFS; \
	then \
		echo 'All tests passed!'; \
	else \
		echo 'Test(s) failed!'; \
	fi

check-bad-outputs: $(COMPILER) $(BADTESTS)
	DIFFS=0; \
	for f in `echo $(BADTESTS) | sed -e 's/\\.spl//g'`; \
	do \
		$(RM) "$$f.myo" ; \
		./$(COMPILER) $$f.spl >"$$f.myo" 2>&1; \
		diff -w -B "$$f.out" "$$f.myo" && echo 'passed!' || DIFFS=1; \
	done; \
	if test 0 = $$DIFFS; \
	then \
		echo 'All tests passed!'; \
	else \
		echo 'Test(s) failed!'; \
	fi

# Automatically generate the submission zip file
$(SUBMISSIONZIPFILE): *.c *.h $(STUDENTTESTOUTPUTS)
	$(ZIP) $(SUBMISSIONZIPFILE) $(SPL).y $(SPL)_lexer.l *.c *.h Makefile
	$(ZIP) $(SUBMISSIONZIPFILE) $(STUDENTTESTOUTPUTS) $(ALLTESTS) $(EXPECTEDOUTPUTS)

.PHONY: compile-separately check-separately
compile-separately check-separately: spl_lexer.c spl.tab.c
	@for f in *.c ; \
	do echo $(CC) $(CFLAGS) -c $$f ; \
	   if test "$$f" = "$(SPL)_lexer.c" ; \
	   then $(CC) $(CFLAGS) -Wno-unused-but-set-variable -c $(SPL)_lexer.c; \
	   else $(CC) $(CFLAGS) -c $$f ; \
	   fi ; \
	done