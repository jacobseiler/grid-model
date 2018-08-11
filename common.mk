OPTIMIZE = -O3 -ftree-vectorize
WARNING = -Wall -Wextra -Wshadow -g

FFTW3DIR :=/opt/local/include
FFTW_CFLAGS := -I$(FFTW3DIR)
FFTW3LIBDIR :=/opt/local/lib
FFTW3_LINK := -L$(FFTW3LIBDIR) -lfftw3 -Xlinker -rpath -Xlinker $(FFTW3_LIBDIR) 

GSL_FOUND := $(shell gsl-config --version)
ifndef GSL_FOUND
  $(error $(ccred)Error:$(ccreset) GSL not found in path - please install GSL before installing $(DISTNAME).$(VERSION) $(ccreset))
endif
GSL_CFLAGS := $(shell gsl-config --cflags)
GSL_LIBDIR := $(shell gsl-config --prefix)/lib
GSL_LINK := $(shell gsl-config --libs) -Xlinker -rpath -Xlinker $(GSL_LIBDIR)

LDFLAGS := $(GSL_LINK) $(FFTW3_LINK) -lm
CFLAGS := -c -std=c99 -march=native $(WARNING) $(OPTIMIZE) $(GSL_CFLAGS) $(FFTW_CFLAGS)

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    COMPILER := clang
else
    COMPILER := gcc
endif

ifdef USE-MPI
ifeq ($(ON_CI), false) #  Don't build with MPI if we're on a continuous integration service. 
    CC := mpicc
    CFLAGS += -D __MPI
    LDFLAGS += -lfftw3_mpi
    OPT += -DMPI  #  This creates an MPI version that can be used to process files in parallel
    CC = mpicc  # sets the C-compiler
endif
else
     CC := $(COMPILER)
endif
