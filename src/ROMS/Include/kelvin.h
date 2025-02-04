/*
** svn $Id: kelvin.h 429 2009-12-20 17:30:26Z arango $
*******************************************************************************
** Copyright (c) 2002-2010 The ROMS/TOMS Group                               **
**   Licensed under a MIT/X style license                                    **
**   See License_ROMS.txt                                                    **
*******************************************************************************
**
** Options for Kelvin Wave Test.
**
** Application flag:   KELVIN
** Input script:       ocean_kelvin.in
*/

#define ROMS_MODEL
#define UV_ADV
#define UV_COR
#define UV_QDRAG
#define UV_VIS2
#define MIX_S_UV
#define DJ_GRADPS
#define TS_DIF2
#define MIX_S_TS
#define SOLVE3D
#define NORTHERN_WALL
#define SOUTHERN_WALL
#define RADIATION_2D
#define EAST_FSRADIATION
#define EAST_M2RADIATION
#define EAST_M3RADIATION
#define EAST_TRADIATION
#define WEST_FSCHAPMAN
#define WEST_M2FLATHER
#undef  WEST_FSCLAMPED
#undef  WEST_M2CLAMPED
#define WEST_M3RADIATION
#define WEST_TRADIATION
#define ANA_GRID
#define ANA_INITIAL
#define ANA_FSOBC
#define ANA_M2OBC
#define ANA_SMFLUX
#define ANA_STFLUX
#define ANA_SRFLUX
#define ANA_BTFLUX
