CC := clang

SRCDIR := src
BUILDDIR := build

BINS := $(addprefix $(BUILDDIR)/, lexer expr)
SBINS := $(addsuffix -static, $(BINS))
OBJS := $(addprefix $(BUILDDIR)/, buffer.o lexer.o lexfsm.o str.o token.o directive.o stack.o)

INCS := 
LIBS := 

.PHONY: all all-static clean

expr: $(BUILDDIR)/expr

expr-static: $(BUILDDIR)/expr-static

lexer: $(BUILDDIR)/lexer

lexer-static: $(BUILDDIR)/lexer-static

all: $(BINS)

all-static: $(SBINS)

clean:
	-rm -rf $(BUILDDIR)

$(BINS): %: $(OBJS) %driver.o
	$(CC) $(LIBS) -g -o $@ $^

$(SBINS): %: $(OBJS) %driver.o
	$(CC) $(LIBS) -static -g -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(INCS) -std=c99 -g -c -o $@ $<

$(BUILDDIR):
	mkdir $(BUILDDIR)
