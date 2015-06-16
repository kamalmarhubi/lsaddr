# Public domain Makefile template derived from
#   http://jingram.sdf.org/2014/11/09/a-makefile-template-for-simple-c-projects.html

.DEFAULT_GOAL = all

PROG := lsaddr
SRCS := $(addsuffix .c, $(PROG))
DOCS := $(addsuffix .1, $(PROG))

A2X := a2x
CC := cc
CFLAGS := -std=gnu11 -Wall -Wextra -Werror -MMD
LDFLAGS :=

OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

%.1: %.adoc
	$(A2X) -f manpage $<

.PHONY: all
all: $(PROG) $(DOCS)

-include $(DEPS)

.PHONY: clean cleaner
clean:
	rm -f $(OBJS) $(DEPS)

cleaner: clean
	rm -rf $(PROG) $(DOCS)
