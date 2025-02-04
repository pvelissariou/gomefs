# svn $Id: Darwin-pgi.mk 655 2008-07-25 18:57:05Z arango $
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
# Copyright (c) 2002-2010 The ROMS/TOMS Group                           :::
#   Licensed under a MIT/X style license                                :::
#   See License_ROMS.txt                                                :::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#
# Include file for PGI (version 8.x) compiler on Darwin
# -------------------------------------------------------------------------
#
# ARPACK_LIBDIR  ARPACK libary directory
# FC             Name of the fortran compiler to use
# FFLAGS         Flags to the fortran compiler
# CPP            Name of the C-preprocessor
# CPPFLAGS       Flags to the C-preprocessor
# CLEAN          Name of cleaning executable after C-preprocessing
# NETCDF_INCDIR  NetCDF include directory
# NETCDF_LIBDIR  NetCDF libary directory
# LD             Program to load the objects into an executable
# LDFLAGS        Flags to the loader
# RANLIB         Name of ranlib command
# MDEPFLAGS      Flags for sfmakedepend  (-s if you keep .f files)
#
# First the defaults
#
               FC := pgf90
           FFLAGS :=
              CPP := /usr/bin/cpp-4.0
         CPPFLAGS := -P -traditional-cpp
          LDFLAGS :=
         XTRALIBS :=
               AR := ar
          ARFLAGS := r
            MKDIR := mkdir -p
               RM := rm -f
           RANLIB := ranlib
             PERL := perl
             TEST := test

        MDEPFLAGS := --cpp --fext=f90 --file=- --objdir=$(SCRATCH_DIR)

#
# Library locations, can be overridden by environment variables.
#

ifdef USE_NETCDF4
        NC_CONFIG ?= nc-config
    NETCDF_INCDIR ?= $(shell $(NC_CONFIG) --prefix)/include
             LIBS := $(shell $(NC_CONFIG) --flibs)
else
    NETCDF_INCDIR ?= /usr/local/include
    NETCDF_LIBDIR ?= /usr/local/lib
             LIBS := -L$(NETCDF_LIBDIR) -lnetcdff -lnetcdf
endif

ifdef USE_ARPACK
 ifdef USE_MPI
   PARPACK_LIBDIR ?= /usr/local/lib
             LIBS += -L$(PARPACK_LIBDIR) -lparpack
 endif
    ARPACK_LIBDIR ?= /usr/local/lib
             LIBS += -L$(ARPACK_LIBDIR) -larpack
endif

ifdef USE_MPI
         CPPFLAGS += -DMPI
 ifdef USE_MPIF90
               FC := mpif90
 else
             LIBS += -Bdynamic -lfmpi-pgi -lmpi-pgi -Bstatic
 endif
endif

ifdef USE_OpenMP
         CPPFLAGS += -D_OPENMP
           FFLAGS += -mp
endif

# According to the PGI manual, the -u -Bstatic flags initializes
# the symbol table with -Bstatic, which is undefined for the linker.
# An undefined symbol triggers loading of the first member of an
# archive library. The -u flag fails with version 7.x of the compiler
# because it expects an argument.

ifdef USE_DEBUG
#          FFLAGS += -g -C -Mchkstk -Mchkfpstk
           FFLAGS += -g -C
#          FFLAGS += -g
else
           FFLAGS += -fastsse -Mipa=fast
endif

ifdef USE_MCT
       MCT_INCDIR ?= /usr/local/mct/include
       MCT_LIBDIR ?= /usr/local/mct/lib
           FFLAGS += -I$(MCT_INCDIR)
             LIBS += -L$(MCT_LIBDIR) -lmct -lmpeu
endif

ifdef USE_ESMF
      ESMF_SUBDIR := $(ESMF_OS).$(ESMF_COMPILER).$(ESMF_ABI).$(ESMF_COMM).$(ESMF_SITE)
      ESMF_MK_DIR ?= $(ESMF_DIR)/lib/lib$(ESMF_BOPT)/$(ESMF_SUBDIR)
                     include $(ESMF_MK_DIR)/esmf.mk
           FFLAGS += $(ESMF_F90COMPILEPATHS)
             LIBS += $(ESMF_F90LINKPATHS) -lesmf -lC
endif

ifdef USE_JASPER
       JASPER_INCDIR ?= /usr/include/jasper
       JASPER_LIBDIR ?= /usr/lib
endif

ifdef USE_WRF
           FFLAGS += -I$(MCT_INCDIR)
             LIBS += -L$(MCT_LIBDIR) -lmct -lmpeu
             LIBS += WRF/main/module_wrf_top.o
             LIBS += WRF/main/libwrflib.a
             LIBS += WRF/external/fftpack/fftpack5/libfftpack.a
             LIBS += WRF/external/io_grib1/libio_grib1.a
ifdef USE_JASPER
ifneq ($(wildcard WRF/external/io_grib2/libio_grib2.a),)
             LIBS += WRF/external/io_grib2/libio_grib2.a
             LIBS += -L$(JASPER_LIBDIR) -ljasper
endif
endif
             LIBS += WRF/external/io_grib_share/libio_grib_share.a
             LIBS += WRF/external/io_int/libwrfio_int.a
             LIBS += WRF/external/esmf_time_f90/libesmf_time.a
             LIBS += WRF/external/RSL_LITE/librsl_lite.a
             LIBS += WRF/frame/module_internal_header_util.o
             LIBS += WRF/frame/pack_utils.o
             LIBS += WRF/external/io_netcdf/libwrfio_nf.a
#            LIBS += WRF/external/io_netcdf/wrf_io.o
endif

       clean_list += ifc* work.pc*

#
# Use full path of compiler.
#
               FC := $(shell which ${FC})
               LD := $(FC)
             LIBS += $(XTRALIBS)

#
# Perform floating-point operations in strict conformance with the
# IEEE standard. This may slow down computations because some
# optimizations are disabled.  However, we noticed a speed-up.
# The user may want to uncomment this option to allow similar,
# if not identical solutions between different of the PGI compiler.

#          FFLAGS += -Kieee

#
# Set free form format in source files to allow long string for
# local directory and compilation flags inside the code.
#

$(SCRATCH_DIR)/mod_ncparam.o: FFLAGS += -Mfree
$(SCRATCH_DIR)/mod_strings.o: FFLAGS += -Mfree
$(SCRATCH_DIR)/analytical.o: FFLAGS += -Mfree
$(SCRATCH_DIR)/biology.o: FFLAGS += -Mfree
ifdef USE_ADJOINT
$(SCRATCH_DIR)/ad_biology.o: FFLAGS += -Mfree
endif
ifdef USE_REPRESENTER
$(SCRATCH_DIR)/rp_biology.o: FFLAGS += -Mfree
endif
ifdef USE_TANGENT
$(SCRATCH_DIR)/tl_biology.o: FFLAGS += -Mfree
endif

#
# Supress free format in SWAN source files since there are comments
# beyond column 72.
#

ifdef USE_SWAN

$(SCRATCH_DIR)/ocpcre.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/ocpids.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/ocpmix.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swancom1.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swancom2.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swancom3.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swancom4.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swancom5.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swanmain.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swanout1.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swanout2.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swanparll.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swanpre1.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swanpre2.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swanser.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swmod1.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/swmod2.o: FFLAGS += -Mnofree
$(SCRATCH_DIR)/SwanCompdata.o: FFLAGS += -Mfree
$(SCRATCH_DIR)/SwanGriddata.o: FFLAGS += -Mfree
$(SCRATCH_DIR)/m_constants.o:  FFLAGS += -Mfree
$(SCRATCH_DIR)/m_fileio.o:     FFLAGS += -Mfree
$(SCRATCH_DIR)/mod_xnl4v5.o:   FFLAGS += -Mfree
$(SCRATCH_DIR)/serv_xnl4v5.o:  FFLAGS += -Mfree
$(SCRATCH_DIR)/nctablemd.o:    FFLAGS += -Mfree
$(SCRATCH_DIR)/agioncmd.o:     FFLAGS += -Mfree
$(SCRATCH_DIR)/swn_outnc.o:    FFLAGS += -Mfree

endif
