srcdir = src
bindir = bin
objdir = obj
incdir = src/include

CXX = gcc
CXXFLAGS = -Wall -Wpedantic -std=gnu11

start_sources = aucont_start.c start_options.c init.c user_ns.c common.c uts.c pid_ns.c mount_ns.c cgroups.c netns.c
start_objects = $(start_sources:.c=.o)

stop_sources = aucont_stop.c common.c
stop_objects = $(stop_sources:.c=.o)

exec_sources = aucont_exec.c common.c
exec_objects = $(exec_sources:.c=.o)

all: $(bindir) $(objdir) aucont_start aucont_stop aucont_exec

$(bindir):
	mkdir -p $(bindir)

$(objdir):
	mkdir -p $(objdir)

aucont_start: $(start_objects)
	$(CXX) $(CXXFLAGS) $(addprefix $(objdir)/, $(start_objects)) -o $(bindir)/aucont_start

aucont_stop: $(stop_objects)
	$(CXX) $(CXXFLAGS) $(addprefix $(objdir)/, $(stop_objects)) -o $(bindir)/aucont_stop

aucont_exec: $(exec_objects)
	$(CXX) $(CXXFLAGS) $(addprefix $(objdir)/, $(exec_objects)) -o $(bindir)/aucont_exec

%.o: $(srcdir)/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $(objdir)/$@ -I./src/include


.PHONY: clean
clean:
	rm -f $(bindir)/aucont_start
	rm -rf $(objdir)
