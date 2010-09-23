CC := clang

SRCDIR := src
BUILDDIR := build

BIN := lexer
SBIN := lexer-static
OBJS := main.o buffer.o lexer.o lexfsm.o str.o token.o directive.o

INCS := 
LIBS := 

.PHONY: all clean

all: $(BUILDDIR)/$(BIN)

static: $(BUILDDIR)/$(SBIN)

clean:
	-rm -rf $(BUILDDIR)

$(BUILDDIR)/$(BIN): $(addprefix $(BUILDDIR)/, $(OBJS))
	$(CC) $(LIBS) -g -o $@ $^

$(BUILDDIR)/$(SBIN): $(addprefix $(BUILDDIR)/, $(OBJS))
	$(CC) $(LIBS) -static -g -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(INCS) -std=c99 -g -c -o $@ $<

build:
	mkdir $(BUILDDIR)
