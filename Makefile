CC := clang

SRCDIR := src
BUILDDIR := build
DEPSDIR := .deps

BIN := pj-compiler
SBIN := pj-compiler-static
OLD_BINS := expr lexer
OLD_SBINS := $(addsuffix -static, $(OLD_BINS))
BIN_FILES := $(addprefix $(BUILDDIR)/, $(OLD_BINS))
SBIN_FILES := $(addprefix $(BUILDDIR)/, $(OLD_SBINS))
OBJS := bst.o buffer.o directive.o error.o exprparser.o exprprods.o \
	exprsymbol.o lexer.o lexfsm.o list.o rdparser.o stack.o str.o token.o

INCS := 
LIBS := 

.PHONY: all all-static pj-compiler pj-compiler-static $(OLD_BINS) $(OLD_SBINS)

$(BIN) $(SBIN): %: $(BUILDDIR)/%

$(OLD_BINS) $(OLD_SBINS): %: $(BUILDDIR)/%

all: $(BIN) $(OLD_BINS)

all-static: $(SBIN) $(OLD_SBINS)

clean:
	-rm -rf $(BUILDDIR)

$(BUILDDIR)/pj-compiler: $(addprefix $(BUILDDIR)/, $(OBJS)) $(BUILDDIR)/driver.o
	$(CC) $(LIBS) -g -o $@ $^

$(BUILDDIR)/pj-compiler-static: $(addprefix $(BUILDDIR)/, $(OBJS)) $(BUILDDIR)/driver.o
	$(CC) $(LIBS) -static -g -o $@ $^

$(BIN_FILES): %: $(addprefix $(BUILDDIR)/, $(OBJS)) %driver.o
	$(CC) $(LIBS) -g -o $@ $^

$(SBIN_FILES): %: $(OBJS) %driver.o
	$(CC) $(LIBS) -static -g -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(INCS) -std=c99 -g -c -o $@ $<

$(BUILDDIR):
	mkdir $(BUILDDIR)

$(DEPSDIR)/%.d: $(SRCDIR)/%.c | $(DEPSDIR)
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(BUILDDIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(DEPSDIR):
	mkdir $(DEPSDIR)

include $(addprefix $(DEPSDIR)/, $(OBJS:.o=.d))
