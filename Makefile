LIBNAME := cifog
EXEC := $(LIBNAME)
 
LIBSRC:=./src/init.c \
		./src/confObj.c \
		./src/parse_ini.c \
		./src/xmem.c \
		./src/xstring.c \
		./src/grid.c \
		./src/sources.c \
		./src/sources_to_grid.c \
		./src/fraction_q.c \
		./src/filtering.c \
		./src/phys_const.c \
		./src/self_shielding.c \
		./src/density_distribution.c \
		./src/recombination.c \
		./src/mean_free_path.c \
		./src/convolution_fftw.c \
		./src/utils.c\
		./src/input_redshifts.c\
		./src/input_grid.c \
		./src/photion_background.c \
		./src/redshift_tools.c \
		./src/restart.c \
		./src/cifog.c

SRC:= ./src/main.c $(LIBSRC)
OBJS := $(SRC:.c=.o)

LIBOBJS := $(LIBSRC:.c=.o)
CIFOGLIB := lib$(LIBNAME).a

USE-MPI ?= true

ON_CI ?= false
ifeq ($(CI), true)
    ON_CI ?= true
endif

include common.mk

.PHONY: all clean clena celan celna

all: $(EXEC) 

$(EXEC): $(SOURCES) $(EXECUTABLE)

$(CIFOGLIB): $(LIBOBJS)
	ar rcs $@ $(LIBOBJS)

$(EXEC): $(OBJS) $(CIFOGLIB)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.phony: clean celan celna clena tests
celan celna clena: clean
clean:
	rm -rf $(OBJS) $(EXEC) $(CIFOGLIB)
