      SUBROUTINE ana_tobc (ng, tile, model)
!
!! svn $Id: ana_tobc.h 429 2009-12-20 17:30:26Z arango $
!!======================================================================
!! Copyright (c) 2002-2010 The ROMS/TOMS Group                         !
!!   Licensed under a MIT/X style license                              !
!!   See License_ROMS.txt                                              !
!=======================================================================
!                                                                      !
!  This routine sets tracer-type variables open boundary conditions    !
!  using analytical expressions.                                       !
!                                                                      !
!=======================================================================
!
      USE mod_param
      USE mod_boundary
      USE mod_grid
      USE mod_ncparam
      USE mod_ocean
      USE mod_stepping
!
! Imported variable declarations.
!
      integer, intent(in) :: ng, tile, model

#include "tile.h"
!
      CALL ana_tobc_tile (ng, tile, model,                              &
     &                    LBi, UBi, LBj, UBj,                           &
     &                    IminS, ImaxS, JminS, JmaxS,                   &
     &                    nstp(ng),                                     &
     &                    GRID(ng) % z_r,                               &
     &                    OCEAN(ng) % t)
!
! Set analytical header file name used.
!
#ifdef DISTRIBUTE
      IF (Lanafile) THEN
#else
      IF (Lanafile.and.(tile.eq.0)) THEN
#endif
        ANANAME(34)=__FILE__
      END IF

      RETURN
      END SUBROUTINE ana_tobc
!
!***********************************************************************
      SUBROUTINE ana_tobc_tile (ng, tile, model,                        &
     &                          LBi, UBi, LBj, UBj,                     &
     &                          IminS, ImaxS, JminS, JmaxS,             &
     &                          nstp,                                   &
     &                          z_r, t)
!***********************************************************************
!
      USE mod_param
      USE mod_scalars
      USE mod_boundary
      USE mod_ocean
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, tile, model
      integer, intent(in) :: LBi, UBi, LBj, UBj, nstp
      integer, intent(in) :: IminS, ImaxS, JminS, JmaxS

#ifdef ASSUMED_SHAPE
      real(r8), intent(in) :: z_r(LBi:,LBj:,:)
      real(r8), intent(in) :: t(LBi:,LBj:,:,:,:)
#else
      real(r8), intent(in) :: z_r(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(in) :: t(LBi:UBi,LBj:UBj,N(ng),3,NT(ng))
#endif
!
!  Local variable declarations.
!
      integer :: i, ised, itrc, j, k
      real(r8) :: cff

#include "set_bounds.h"
!
!-----------------------------------------------------------------------
!  Tracers open boundary conditions.
!-----------------------------------------------------------------------
!
#ifdef MY_APPLICATION
# ifdef EAST_TOBC
      IF (EASTERN_EDGE) THEN
        DO itrc=1,NT(ng)
          DO k=1,N(ng)
            DO j=JstrR,JendR
              BOUNDARY(ng)%t_east(j,k,itrc)=???
            END DO
          END DO
        END DO
      END IF
# endif
# ifdef WEST_TOBC
      IF (WESTERN_EDGE) THEN
        DO itrc=1,NT(ng)
          DO k=1,N(ng)
            DO j=JstrR,JendR
              BOUNDARY(ng)%t_west(j,k,itrc)=???
            END DO
          END DO
        END DO
      END IF
# endif
# ifdef SOUTH_TOBC
      IF (SOUTHERN_EDGE) THEN
        DO itrc=1,NT(ng)
          DO k=1,N(ng)
            DO i=IstrR,IendR
              BOUNDARY(ng)%t_south(i,k,itrc)=???
            END DO
          END DO
        END DO
      END IF
# endif
# ifdef NORTH_TOBC
      IF (NORTHERN_EDGE) THEN
        DO itrc=1,NT(ng)
          DO k=1,N(ng)
            DO i=IstrR,IendR
              BOUNDARY(ng)%t_north(i,k,itrc)=???
            END DO
          END DO
        END DO
      END IF
# endif
#else
      ana_tobc.h: No values provided for BOUNDARY(ng)%t_xxxx.
#endif

      RETURN
      END SUBROUTINE ana_tobc_tile
