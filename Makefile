CC := clang

SRCDIR := src
BUILDDIR := build

BINS := parser expr lexer
SBINS := $(addsuffix -static, $(BINS))
BIN_FILES := $(addprefix $(BUILDDIR)/, $(BINS))
SBIN_FILES := $(addprefix $(BUILDDIR)/, $(SBINS))
OBJS := $(addprefix $(BUILDDIR)/, \
	bst.o buffer.o directive.o error.o exprparser.o exprprods.o exprsymbol.o \
	lexer.o lexfsm.o list.o rdparser.o stack.o str.o token.o)

INCS := 
LIBS := 

.PHONY: all all-static $(BINS) $(SBINS)

$(BINS) $(SBINS): %: $(BUILDDIR)/%

all: $(BINS)

all-static: $(SBINS)

clean:
	-rm -rf $(BUILDDIR)

$(BIN_FILES): %: $(OBJS) %driver.o
	$(CC) $(LIBS) -g -o $@ $^

$(SBIN_FILES): %: $(OBJS) %driver.o
	$(CC) $(LIBS) -std=c99 -static -g -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(INCS) -std=c99 -g -c -o $@ $<

$(BUILDDIR):
	mkdir $(BUILDDIR)
