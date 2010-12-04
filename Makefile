CC := clang

SRCDIR := src
BUILDDIR := build
DEPSDIR := .deps

BIN := pjc
SBIN := pjc-static
OLD_BINS := expr lexer
OLD_SBINS := $(addsuffix -static, $(OLD_BINS))
BIN_FILES := $(addprefix $(BUILDDIR)/, $(OLD_BINS))
SBIN_FILES := $(addprefix $(BUILDDIR)/, $(OLD_SBINS))
OBJS := bst.o buffer.o codegen.o directive.o error.o exprparser.o exprprods.o \
	exprsymbol.o hypomac.o lexer.o lexfsm.o list.o pjlang.o rdparser.o stack.o \
	str.o symbol.o symtable.o token.o tokenbst.o typecheck.o

INCS := 
LIBS := -lm

.PHONY: all all-static pjc pjc-static $(OLD_BINS) $(OLD_SBINS)

$(BIN) $(SBIN): %: $(BUILDDIR)/%

$(OLD_BINS) $(OLD_SBINS): %: $(BUILDDIR)/%

all: $(BIN) $(OLD_BINS)

all-static: $(SBIN) $(OLD_SBINS)

clean:
	-rm -rf $(BUILDDIR)

$(BUILDDIR)/pjc: $(addprefix $(BUILDDIR)/, $(OBJS)) $(BUILDDIR)/driver.o
	$(CC) $(LIBS) -g -o $@ $^

$(BUILDDIR)/pjc-static: $(addprefix $(BUILDDIR)/, $(OBJS)) $(BUILDDIR)/driver.o
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
