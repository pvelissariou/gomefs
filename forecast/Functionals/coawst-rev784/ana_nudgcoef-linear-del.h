      SUBROUTINE ana_nudgcoef (ng, tile, model)
!
!! svn $Id: ana_nudgcoef.h 429 2009-12-20 17:30:26Z arango $
!!================================================= Hernan G. Arango ===
!! Copyright (c) 2002-2010 The ROMS/TOMS Group                         !
!!   Licensed under a MIT/X style license                              !
!!   See License_ROMS.txt                                              !
!=======================================================================
!                                                                      !
!  This routine set nudging coefficients time-scales (1/s).            !
!                                                                      !
!=======================================================================
!
      USE mod_param
      USE mod_ncparam
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, tile, model
!
!  Local variable declarations.
!
#include "tile.h"
!
      CALL ana_nudgcoef_tile (ng, tile, model,                          &
     &                        LBi, UBi, LBj, UBj,                       &
     &                        IminS, ImaxS, JminS, JmaxS)
!
! Set analytical header file name used.
!
#ifdef DISTRIBUTE
      IF (Lanafile) THEN
#else
      IF (Lanafile.and.(tile.eq.0)) THEN
#endif
        ANANAME(16)=__FILE__
      END IF

      RETURN
      END SUBROUTINE ana_nudgcoef
!
!***********************************************************************
      SUBROUTINE ana_nudgcoef_tile (ng, tile, model,                    &
     &                              LBi, UBi, LBj, UBj,                 &
     &                              IminS, ImaxS, JminS, JmaxS)
!***********************************************************************
!
      USE mod_param
      USE mod_parallel
      USE mod_boundary
#ifdef CLIMATOLOGY
      USE mod_clima
#endif
      USE mod_grid
      USE mod_ncparam
      USE mod_scalars
#ifdef DISTRIBUTE
!
      USE distribute_mod, ONLY : mp_collect
#endif
!
      implicit none
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, tile, model
      integer, intent(in) :: LBi, UBi, LBj, UBj
      integer, intent(in) :: IminS, ImaxS, JminS, JmaxS
!
!  Local variable declarations.
!
      integer :: Iwrk, i, itrc, j
      integer :: II1, II2, JJ1, JJ2
      integer :: nCELLS, IS_BEG, IN_BEG

      real(r8) :: cff1, cff2
      real(r8) :: rtim1, rtim2, rratio

      real(r8), parameter :: IniVal = 0.0_r8

      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: wrk

#include "set_bounds.h"
!
!-----------------------------------------------------------------------
!  Set up nudging towards data time-scale coefficients (1/s).
!-----------------------------------------------------------------------
!
!  Initialize.
!
      DO j=JstrR,JendR
        DO i=IstrR,IendR
          wrk(i,j)=0.0_r8
        END DO
      END DO

#if   defined GOM05K_COAPS || \
      defined GOM1K_COAPS || defined GOM2K_COAPS || \
      defined GOM3K_COAPS || defined GOM4K_COAPS || \
      defined GOM5K_COAPS || defined GOM6K_COAPS || \
      defined GOM7K_COAPS || defined GOM8K_COAPS

      ! nCELLS  values are based on a guessed maximum internal Rossby
      !         radius of deformation for GoM equal to: ~ 62 km
      !         calculated as: ceiling(62000.0 / resolution(m)).
      !         Min Rossby radius : 27 km
      !         Mean Rossby radius: 42 km
      !         Max Rossby radius : 62 km
      !         It defines a buffer zone around the four boundaries
      !         (west, east, south, north)

#if   defined GOM05K_COAPS
      nCELLS =  124
      IS_BEG = 1808
      IN_BEG = 1808
#endif

#if   defined GOM1K_COAPS
      nCELLS =  62
      IS_BEG = 904
      IN_BEG = 904
#endif

#if   defined GOM2K_COAPS
      nCELLS =  31
      IS_BEG = 452
      IN_BEG = 452
#endif

#if   defined GOM3K_COAPS
      nCELLS =  21
      IS_BEG = 301
      IN_BEG = 301
#endif

#if   defined GOM4K_COAPS
      nCELLS =  16
      IS_BEG = 226
      IN_BEG = 226
#endif

#if   defined GOM5K_COAPS
      nCELLS =  13
      IS_BEG = 181
      IN_BEG = 181
#endif

#if   defined GOM6K_COAPS
      nCELLS =  11
      IS_BEG = 151
      IN_BEG = 151
#endif

#if   defined GOM7K_COAPS
      nCELLS =   9
      IS_BEG = 129
      IN_BEG = 129
#endif

#if   defined GOM8K_COAPS
      nCELLS =   8
      IS_BEG = 114
      IN_BEG = 114
#endif

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!  Set nudging coefficients in the southern, northern and the eastern
!  edges. See:
!  1) Patrick Marchesiello, et al., 2001. "Open boundary conditions for
!       long-term integration of regional oceanic models". Ocean Modelling,
!       2, 2001, 1-20.
!  2) A. Barth, et al., 2008. "Benefit of nesting a regional model instead
!       of climatology. Application to the West Florida Shelf". Continental
!       Shelf Research, 28, 2008, 561-573.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

      ! We use 1s to produce a value cff1 = 1.0 at the first boundary
      ! grid point (subject to change).
      ! (minus the GHOST points).
      rtim1 =  0.1_r8
      rtim2 =  1.0_r8

      cff1 = 1.0_r8 / (rtim1 * 86400.0_r8)
      cff2 = 1.0_r8 / (rtim2 * 86400.0_r8)
!  Southern Boundary
      II1 = IstrR
      II2 = IendR
      JJ1 = JstrR
      JJ2 = MIN(nCELLS + 1, JendR)
      DO j = JJ1, JJ2
        DO i = II1, II2
          IF (II1.GE.IS_BEG) THEN
!            rratio = REAL(j - JJ1, r8) / REAL(JJ2 - JJ1, r8)
            rratio = REAL(nCELLS + 1 - j, r8) / REAL(nCELLS, r8)
            wrk(i,j) = cff2 + rratio * (cff1 - cff2)
          END IF
        END DO
      END DO

!  Northern Boundary
      II1 = IstrR
      II2 = IendR
      JJ1 = MAX(JstrR, Mm(ng) - nCELLS)
      JJ2 = JendR
      DO j = JJ1, JJ2
        DO i = II1, II2
          IF (II1.GE.IN_BEG) THEN
!            rratio = REAL(JJ2 - j, r8) / REAL(JJ2 - JJ1, r8)
            rratio = REAL(Mm(ng) - j, r8) / REAL(nCELLS, r8)
            wrk(i,j) = cff1 + rratio * (cff2 - cff1)
          END IF
        END DO
      END DO

!  Eastern Boundary
      II1 = MAX(IstrR, Lm(ng) - nCELLS)
      II2 = IendR
      JJ1 = JstrR
      JJ2 = JendR
      DO j = JJ1, JJ2
        DO i = II1, II2
!          rratio = REAL(II2 - i, r8) / REAL(II2 - II1, r8)
          rratio = REAL(Lm(ng) - i, r8) / REAL(nCELLS, r8)
          wrk(i,j) = cff1 + rratio * (cff2 - cff1)
        END DO
      END DO

      DO j=JstrR,JendR
        DO i=IstrR,IendR
# ifdef TCLM_NUDGING
          CLIMA(ng)%Tnudgcof(i,j,itemp)=wrk(i,j)
          CLIMA(ng)%Tnudgcof(i,j,isalt)=wrk(i,j)
# endif
# ifdef M3CLM_NUDGING
          CLIMA(ng)%M3nudgcof(i,j)=wrk(i,j)
# endif
# ifdef ZCLM_NUDGING
	  CLIMA(ng)%Znudgcof(i,j)=wrk(i,j)
# endif
# ifdef M2CLM_NUDGING
          CLIMA(ng)%M2nudgcof(i,j)=wrk(i,j)
# endif
        END DO
      END DO
#else
!
!  Default nudging coefficients.  Set nudging coefficients uniformly to
!  the values specified in the standard input file.
!
# ifdef ZCLM_NUDGING
      DO j=JstrR,JendR
        DO i=IstrR,IendR
          CLIMA(ng)%Znudgcof(i,j)=Znudg(ng)
        END DO
      END DO
# endif
# ifdef M2CLM_NUDGING
      DO j=JstrR,JendR
        DO i=IstrR,IendR
          CLIMA(ng)%M2nudgcof(i,j)=M2nudg(ng)
        END DO
      END DO
# endif
# ifdef SOLVE3D
#  ifdef TCLM_NUDGING
      DO itrc=1,NT(ng)
        DO j=JstrR,JendR
          DO i=IstrR,IendR
            CLIMA(ng)%Tnudgcof(i,j,itrc)=Tnudg(itrc,ng)
          END DO
        END DO
      END DO
#  endif
#  ifdef M3CLM_NUDGING
      DO j=JstrR,JendR
        DO i=IstrR,IendR
          CLIMA(ng)%M3nudgcof(i,j)=M3nudg(ng)
        END DO
      END DO
#  endif
# endif
#endif
#ifdef NUDGING_COFF
!
!-----------------------------------------------------------------------
!  Set nudging coefficients (1/s) for passive/active (outflow/inflow)
!  open boundary conditions.  Weak nudging is expected in passive
!  outflow conditions and strong nudging is expected in active inflow
!  conditions.  Notice that interior nudging coefficient defined
!  above are zero out when boundary condition nudging.  The USER needs
!  to adapt this to his/her application!
!-----------------------------------------------------------------------
!
!! WARNING:  This section is generic for all applications. Please do not
!!           change the code below.
!!
!  Free-surface nudging coefficients.
!
# ifdef WEST_FSNUDGING
#  ifdef ZCLM_NUDGING
      IF (SOUTH_WEST_CORNER) THEN
        FSobc_out(ng,iwest)=CLIMA(ng)%Znudgcof(0,1)
        FSobc_in (ng,iwest)=obcfac(ng)*FSobc_out(ng,iwest)
      END IF
      IF (WESTERN_EDGE) THEN
        DO j=JstrR,JendR
          CLIMA(ng)%Znudgcof(0,j)=0.0_r8
        END DO
      END IF
#   ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, FSobc_out(:,iwest))
        CALL mp_collect (ng, model, Ngrids, IniVal, FSobc_in (:,iwest))
      END IF
#   endif
#  else
      IF (SOUTH_WEST_TEST) THEN
        FSobc_out(ng,iwest)=Znudg(ng)
        FSobc_in (ng,iwest)=obcfac(ng)*Znudg(ng)
      END IF
#  endif
# endif
# ifdef EAST_FSNUDGING
#  ifdef ZCLM_NUDGING
      IF (NORTH_EAST_CORNER) THEN
        FSobc_out(ng,ieast)=CLIMA(ng)%Znudgcof(Lm(ng)+1,Mm(ng))
        FSobc_in (ng,ieast)=obcfac(ng)*FSobc_out(ng,ieast)
      END IF
      IF (EASTERN_EDGE) THEN
        DO j=JstrR,JendR
          CLIMA(ng)%Znudgcof(Lm(ng)+1,j)=0.0_r8
        END DO
      END IF
#   ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, FSobc_out(:,ieast))
        CALL mp_collect (ng, model, Ngrids, IniVal, FSobc_in (:,ieast))
      END IF
#   endif
#  else
      IF (NORTH_EAST_TEST) THEN
        FSobc_out(ng,ieast)=Znudg(ng)
        FSobc_in (ng,ieast)=obcfac(ng)*Znudg(ng)
      END IF
#  endif
# endif
# ifdef SOUTH_FSNUDGING
#  ifdef ZCLM_NUDGING
      IF (SOUTH_WEST_CORNER) THEN
        FSobc_out(ng,isouth)=CLIMA(ng)%Znudgcof(1,0)
        FSobc_in (ng,isouth)=obcfac(ng)*FSobc_out(ng,isouth)
      END IF
      IF (SOUTHERN_EDGE) THEN
        DO i=IstrR,IendR
          CLIMA(ng)%Znudgcof(i,0)=0.0_r8
        END DO
      END IF
#   ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, FSobc_out(:,isouth))
        CALL mp_collect (ng, model, Ngrids, IniVal, FSobc_in (:,isouth))
      END IF
#   endif
#  else
      IF (SOUTH_WEST_TEST) THEN
        FSobc_out(ng,isouth)=Znudg(ng)
        FSobc_in (ng,isouth)=obcfac(ng)*Znudg(ng)
      END IF
#  endif
# endif
# ifdef NORTH_FSNUDGING
#  ifdef ZCLM_NUDGING
      IF (NORTH_EAST_CORNER) THEN
        FSobc_out(ng,inorth)=CLIMA(ng)%Znudgcof(Lm(ng),Mm(ng)+1)
        FSobc_in (ng,inorth)=obcfac(ng)*FSobc_out(ng,inorth)
      END IF
      IF (NORTHERN_EDGE) THEN
        DO i=IstrR,IendR
          CLIMA(ng)%Znudgcof(i,Mm(ng)+1)=0.0_r8
        END DO
      END IF
#   ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, FSobc_out(:,inorth))
        CALL mp_collect (ng, model, Ngrids, IniVal, FSobc_in (:,inorth))
      END IF
#   endif
#  else
      IF (NORTH_EAST_TEST) THEN
        FSobc_out(ng,inorth)=Znudg(ng)
        FSobc_in (ng,inorth)=obcfac(ng)*Znudg(ng)
      END IF
#  endif
# endif
!
!  2D momentum nudging coefficients.
!
# ifdef WEST_M2NUDGING
#  ifdef M2CLM_NUDGING
      IF (SOUTH_WEST_CORNER) THEN
        M2obc_out(ng,iwest)=0.5_r8*(CLIMA(ng)%M2nudgcof(0,1)+           &
     &                              CLIMA(ng)%M2nudgcof(1,1))
        M2obc_in (ng,iwest)=obcfac(ng)*M2obc_out(ng,iwest)
      END IF
      IF (WESTERN_EDGE) THEN
        DO j=JstrR,JendR
          CLIMA(ng)%M2nudgcof(0,j)=-CLIMA(ng)%M2nudgcof(1,j)
        END DO
      END IF
#   ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, M2obc_out(:,iwest))
        CALL mp_collect (ng, model, Ngrids, IniVal, M2obc_in (:,iwest))
      END IF
#   endif
#  else
      IF (SOUTH_WEST_TEST) THEN
        M2obc_out(ng,iwest)=M2nudg(ng)
        M2obc_in (ng,iwest)=obcfac(ng)*M2nudg(ng)
      END IF
#  endif
# endif
# ifdef EAST_M2NUDGING
#  ifdef M2CLM_NUDGING
      IF (NORTH_EAST_CORNER) THEN
        M2obc_out(ng,ieast)=0.5_r8*                                     &
     &                      (CLIMA(ng)%M2nudgcof(Lm(ng)  ,Mm(ng))+      &
     &                       CLIMA(ng)%M2nudgcof(Lm(ng)+1,Mm(ng)))
        M2obc_in (ng,ieast)=obcfac(ng)*M2obc_out(ng,ieast)
      END IF
      IF (EASTERN_EDGE) THEN
        DO j=JstrR,JendR
          CLIMA(ng)%M2nudgcof(Lm(ng)+1,j)=-CLIMA(ng)%M2nudgcof(Lm(ng),j)
        END DO
      END IF
#   ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, M2obc_out(:,ieast))
        CALL mp_collect (ng, model, Ngrids, IniVal, M2obc_in (:,ieast))
      END IF
#   endif
#  else
      IF (NORTH_EAST_TEST) THEN
        M2obc_out(ng,ieast)=M2nudg(ng)
        M2obc_in (ng,ieast)=obcfac(ng)*M2nudg(ng)
      END IF
#  endif
# endif
# ifdef SOUTH_M2NUDGING
#  ifdef M2CLM_NUDGING
      IF (SOUTH_WEST_CORNER) THEN
        M2obc_out(ng,isouth)=0.5_r8*(CLIMA(ng)%M2nudgcof(1,0)+          &
     &                               CLIMA(ng)%M2nudgcof(1,1))
        M2obc_in (ng,isouth)=obcfac(ng)*M2obc_out(ng,isouth)
      END IF
      IF (SOUTHERN_EDGE) THEN
        DO i=IstrR,IendR
          CLIMA(ng)%M2nudgcof(i,0)=-CLIMA(ng)%M2nudgcof(i,1)
        END DO
      END IF
#   ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, M2obc_out(:,isouth))
        CALL mp_collect (ng, model, Ngrids, IniVal, M2obc_in (:,isouth))
      END IF
#   endif
#  else
      IF (SOUTH_WEST_TEST) THEN
        M2obc_out(ng,isouth)=M2nudg(ng)
        M2obc_in (ng,isouth)=obcfac(ng)*M2nudg(ng)
      END IF
#  endif
# endif
# ifdef NORTH_M2NUDGING
#  ifdef M2CLM_NUDGING
      IF (NORTH_EAST_CORNER) THEN
        M2obc_out(ng,inorth)=0.5_r8*                                    &
     &                       (CLIMA(ng)%M2nudgcof(Lm(ng),Mm(ng)  )+     &
     &                        CLIMA(ng)%M2nudgcof(Lm(ng),Mm(ng)+1))
        M2obc_in (ng,inorth)=obcfac(ng)*M2obc_out(ng,inorth)
      END IF
      IF (NORTHERN_EDGE) THEN
        DO i=IstrR,IendR
          CLIMA(ng)%M2nudgcof(i,Mm(ng)+1)=-CLIMA(ng)%M2nudgcof(i,Mm(ng))
        END DO
      END IF
#   ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, M2obc_out(:,inorth))
        CALL mp_collect (ng, model, Ngrids, IniVal, M2obc_in (:,inorth))
      END IF
#   endif
#  else
      IF (NORTH_EAST_TEST) THEN
        M2obc_out(ng,inorth)=M2nudg(ng)
        M2obc_in (ng,inorth)=obcfac(ng)*M2nudg(ng)
      END IF
#  endif
# endif
# ifdef SOLVE3D
!
!  Tracers nudging coefficients.
!
#  ifdef WEST_TNUDGING
#   ifdef TCLM_NUDGING
      DO itrc=1,NT(ng)
        IF (SOUTH_WEST_CORNER) THEN
          Tobc_out(itrc,ng,iwest)=CLIMA(ng)%Tnudgcof(0,1,itrc)
          Tobc_in (itrc,ng,iwest)=obcfac(ng)*Tobc_out(itrc,ng,iwest)
        END IF
        IF (WESTERN_EDGE) THEN
          DO j=JstrR,JendR
            CLIMA(ng)%Tnudgcof(0,j,itrc)=0.0_r8
          END DO
        END IF
      END DO
#    ifdef DISTRIBUTE
      CALL mp_collect (ng, model, MT, IniVal, Tobc_out(:,ng,iwest))
      CALL mp_collect (ng, model, MT, IniVal, Tobc_in (:,ng,iwest))
#    endif
#   else
      DO itrc=1,NT(ng)
        IF (SOUTH_WEST_TEST) THEN
          Tobc_out(itrc,ng,iwest)=Tnudg(itrc,ng)
          Tobc_in (itrc,ng,iwest)=obcfac(ng)*Tnudg(itrc,ng)
        END IF
      END DO
#   endif
#  endif
#  ifdef EAST_TNUDGING
#   ifdef TCLM_NUDGING
      DO itrc=1,NT(ng)
        IF (NORTH_EAST_CORNER) THEN
          Tobc_out(itrc,ng,ieast)=                                      &
     &             CLIMA(ng)%Tnudgcof(Lm(ng)+1,Mm(ng),itrc)
          Tobc_in (itrc,ng,ieast)=obcfac(ng)*Tobc_out(itrc,ng,ieast)
        END IF
        IF (EASTERN_EDGE) THEN
          DO j=JstrR,JendR
            CLIMA(ng)%Tnudgcof(Lm(ng)+1,j,itrc)=0.0_r8
          END DO
        END IF
      END DO
#    ifdef DISTRIBUTE
      CALL mp_collect (ng, model, MT, IniVal, Tobc_out(:,ng,ieast))
      CALL mp_collect (ng, model, MT, IniVal, Tobc_in (:,ng,ieast))
#    endif
#   else
      DO itrc=1,NT(ng)
        IF (NORTH_EAST_TEST) THEN
          Tobc_out(itrc,ng,ieast)=Tnudg(itrc,ng)
          Tobc_in (itrc,ng,ieast)=obcfac(ng)*Tnudg(itrc,ng)
        END IF
      END DO
#   endif
#  endif
#  ifdef SOUTH_TNUDGING
#   ifdef TCLM_NUDGING
      DO itrc=1,NT(ng)
        IF (SOUTH_WEST_CORNER) THEN
          Tobc_out(itrc,ng,isouth)=CLIMA(ng)%Tnudgcof(1,0,itrc)
          Tobc_in (itrc,ng,isouth)=obcfac(ng)*Tobc_out(itrc,ng,isouth)
        END IF
        IF (SOUTHERN_EDGE) THEN
          DO i=IstrR,IendR
            CLIMA(ng)%Tnudgcof(i,0,itrc)=0.0_r8
          END DO
        END IF
      END DO
#    ifdef DISTRIBUTE
      CALL mp_collect (ng, model, MT, IniVal, Tobc_out(:,ng,isouth))
      CALL mp_collect (ng, model, MT, IniVal, Tobc_in (:,ng,isouth))
#    endif
#   else
      DO itrc=1,NT(ng)
        IF (SOUTH_WEST_TEST) THEN
          Tobc_out(itrc,ng,isouth)=Tnudg(itrc,ng)
          Tobc_in (itrc,ng,isouth)=obcfac(ng)*Tnudg(itrc,ng)
        END IF
      END DO
#   endif
#  endif
#  ifdef NORTH_TNUDGING
#   ifdef TCLM_NUDGING
      DO itrc=1,NT(ng)
        IF (NORTH_EAST_CORNER) THEN
          Tobc_out(itrc,ng,inorth)=                                     &
     &             CLIMA(ng)%Tnudgcof(Lm(ng),Mm(ng)+1,itrc)
          Tobc_in (itrc,ng,inorth)=obcfac(ng)*Tobc_out(itrc,ng,inorth)
        END IF
        IF (NORTHERN_EDGE) THEN
          DO i=IstrR,IendR
            CLIMA(ng)%Tnudgcof(i,Mm(ng)+1,itrc)=0.0_r8
          END DO
        END IF
      END DO
#    ifdef DISTRIBUTE
      CALL mp_collect (ng, model, MT, IniVal, Tobc_out(:,ng,inorth))
      CALL mp_collect (ng, model, MT, IniVal, Tobc_in (:,ng,inorth))
#    endif
#   else
      DO itrc=1,NT(ng)
        IF (NORTH_EAST_TEST) THEN
          Tobc_out(itrc,ng,inorth)=Tnudg(itrc,ng)
          Tobc_in (itrc,ng,inorth)=obcfac(ng)*Tnudg(itrc,ng)
        END IF
      END DO
#   endif
#  endif
!
!  3D momentum nudging coefficients.
!
#  ifdef WEST_M3NUDGING
#   ifdef M3CLM_NUDGING
      IF (SOUTH_WEST_CORNER) THEN
        M3obc_out(ng,iwest)=0.5_r8*                                     &
     &                      (CLIMA(ng)%M3nudgcof(0,1)+                  &
     &                       CLIMA(ng)%M3nudgcof(1,1))
        M3obc_in (ng,iwest)=obcfac(ng)*M3obc_out(ng,iwest)
      END IF
      IF (WESTERN_EDGE) THEN
        DO j=JstrR,JendR
          CLIMA(ng)%M3nudgcof(0,j)=-CLIMA(ng)%M3nudgcof(1,j)
        END DO
      END IF
#    ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, M3obc_out(:,iwest))
        CALL mp_collect (ng, model, Ngrids, IniVal, M3obc_in (:,iwest))
      END IF
#    endif
#   else
      IF (SOUTH_WEST_TEST) THEN
        M3obc_out(ng,iwest)=M3nudg(ng)
        M3obc_in (ng,iwest)=obcfac(ng)*M3nudg(ng)
      END IF
#   endif
#  endif
#  ifdef EAST_M3NUDGING
#   ifdef M3CLM_NUDGING
      IF (NORTH_EAST_CORNER) THEN
        M3obc_out(ng,ieast)=0.5_r8*                                     &
     &                      (CLIMA(ng)%M3nudgcof(Lm(ng)  ,Mm(ng))+      &
     &                       CLIMA(ng)%M3nudgcof(Lm(ng)+1,Mm(ng)))
        M3obc_in (ng,ieast)=obcfac(ng)*M3obc_out(ng,ieast)
      END IF
      IF (EASTERN_EDGE) THEN
        DO j=JstrR,JendR
          CLIMA(ng)%M3nudgcof(Lm(ng)+1,j)=-CLIMA(ng)%M3nudgcof(Lm(ng),j)
        END DO
      END IF
#    ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, M3obc_out(:,ieast))
        CALL mp_collect (ng, model, Ngrids, IniVal, M3obc_in (:,ieast))
      END IF
#    endif
#   else
      IF (NORTH_EAST_TEST) THEN
        M3obc_out(ng,ieast)=M3nudg(ng)
        M3obc_in (ng,ieast)=obcfac(ng)*M3nudg(ng)
      END IF
#   endif
#  endif
#  ifdef SOUTH_M3NUDGING
#   ifdef M3CLM_NUDGING
      IF (SOUTH_WEST_CORNER) THEN
        M3obc_out(ng,isouth)=0.5_r8*                                    &
     &                       (CLIMA(ng)%M3nudgcof(1,0)+                 &
     &                        CLIMA(ng)%M3nudgcof(1,1))
        M3obc_in (ng,isouth)=obcfac(ng)*M3obc_out(ng,isouth)
      END IF
      IF (SOUTHERN_EDGE) THEN
        DO i=IstrR,IendR
          CLIMA(ng)%M3nudgcof(i,0)=-CLIMA(ng)%M3nudgcof(i,1)
        END DO
      END IF
#    ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, M3obc_out(:,isouth))
        CALL mp_collect (ng, model, Ngrids, IniVal, M3obc_in (:,isouth))
      END IF
#    endif
#   else
      IF (SOUTH_WEST_TEST) THEN
        M3obc_out(ng,isouth)=M3nudg(ng)
        M3obc_in (ng,isouth)=obcfac(ng)*M3nudg(ng)
      END IF
#   endif
#  endif
#  ifdef NORTH_M3NUDGING
#   ifdef M3CLM_NUDGING
      IF (NORTH_EAST_CORNER) THEN
        M3obc_out(ng,inorth)=0.5_r8*                                    &
     &                       (CLIMA(ng)%M3nudgcof(Lm(ng),Mm(ng)  )+     &
     &                        CLIMA(ng)%M3nudgcof(Lm(ng),Mm(ng)+1))
        M3obc_in (ng,inorth)=obcfac(ng)*M3obc_out(ng,inorth)
      END IF
      IF (NORTHERN_EDGE) THEN
        DO i=IstrR,IendR
          CLIMA(ng)%M3nudgcof(i,Mm(ng)+1)=-CLIMA(ng)%M3nudgcof(i,Mm(ng))
        END DO
      END IF
#    ifdef DISTRIBUTE
      IF (ng.eq.Ngrids) THEN
        CALL mp_collect (ng, model, Ngrids, IniVal, M3obc_out(:,inorth))
        CALL mp_collect (ng, model, Ngrids, IniVal, M3obc_in (:,inorth))
      END IF
#    endif
#   else
      IF (NORTH_EAST_TEST) THEN
        M3obc_out(ng,inorth)=M3nudg(ng)
        M3obc_in (ng,inorth)=obcfac(ng)*M3nudg(ng)
      END IF
#   endif
#  endif
# endif
#endif
      RETURN
      END SUBROUTINE ana_nudgcoef_tile

