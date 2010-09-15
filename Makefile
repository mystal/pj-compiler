CC := clang

SRCDIR := src
BUILDDIR := build

BIN := lexer
OBJS := main.o buffer.o lexer.o lexfsm.o str.o token.o directive.o

INCS := 
LIBS := 

.PHONY: all clean

all: $(BUILDDIR)/$(BIN)

clean:
	-rm -rf $(BUILDDIR)

$(BUILDDIR)/$(BIN): $(addprefix $(BUILDDIR)/, $(OBJS))
	$(CC) $(LIBS) -static -g -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(INCS) -std=c99 -g -c -o $@ $<

build:
	mkdir $(BUILDDIR)
