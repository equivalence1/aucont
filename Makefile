srcdir = src
bindir = bin
objdir = obj
incdir = src/include

CXX = gcc
CXXFLAGS = -Wall -pedantic

start_sources = aucont_start.c start_options.c
start_objects = $(start_sources:.c=.o)

all: $(bindir) $(objdir) aucont_start

$(bindir):
	mkdir -p $(bindir)

$(objdir):
	mkdir -p $(objdir)

aucont_start: $(start_objects)
	$(CXX) $(CXXFLAGS) $(addprefix $(objdir)/, $(start_objects)) -o $(bindir)/aucont_start

%.o: $(srcdir)/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $(objdir)/$@ -I./src/include


.PHONY: clean
clean:
	rm -rf $(bindir)
	rm -rf $(objdir)
