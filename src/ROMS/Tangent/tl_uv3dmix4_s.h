# ifdef EW_PERIODIC
#  define IV_RANGE Istr-1,Iend+1
#  define IU_RANGE Istr-1,Iend+1
# else
#  define IV_RANGE MAX(1,Istr-1),MIN(Iend+1,Lm(ng))
#  define IU_RANGE MAX(2,IstrU-1),MIN(Iend+1,Lm(ng))
# endif
# ifdef NS_PERIODIC
#  define JU_RANGE Jstr-1,Jend+1
#  define JV_RANGE Jstr-1,Jend+1
# else
#  define JU_RANGE MAX(1,Jstr-1),MIN(Jend+1,Mm(ng))
#  define JV_RANGE MAX(2,JstrV-1),MIN(Jend+1,Mm(ng))
# endif

      SUBROUTINE tl_uv3dmix4 (ng, tile)
!
!svn $Id: tl_uv3dmix4_s.h 523 2011-01-05 03:21:38Z arango $
!************************************************** Hernan G. Arango ***
!  Copyright (c) 2002-2011 The ROMS/TOMS Group       Andrew M. Moore   !
!    Licensed under a MIT/X style license                              !
!    See License_ROMS.txt                                              !
!***********************************************************************
!                                                                      !
!  This subroutine computes tangent linear biharmonic mixing of        !
!  momentum,  along  constant  S-surfaces,  from the horizontal        !
!  divergence  of the  stress tensor.  A transverse isotropy is        !
!  assumed  so the  stress  tensor is split  into  vertical and        !
!  horizontal subtensors.                                              !
!                                                                      !
!  Reference:                                                          !
!                                                                      !
!      Wajsowicz, R.C, 1993: A consistent formulation of the           !
!         anisotropic stress tensor for use in models of the           !
!         large-scale ocean circulation, JCP, 105, 333-338.            !
!                                                                      !
!      Sadourny, R. and K. Maynard, 1997: Formulations of              !
!         lateral diffusion in geophysical fluid dynamics              !
!         models, In Numerical Methods of Atmospheric and              !
!         Oceanic Modelling. Lin, Laprise, and Ritchie,                !
!         Eds., NRC Research Press, 547-556.                           !
!                                                                      !
!      Griffies, S.M. and R.W. Hallberg, 2000: Biharmonic              !
!         friction with a Smagorinsky-like viscosity for               !
!         use in large-scale eddy-permitting ocean models,             !
!         Monthly Weather Rev., 128, 8, 2935-2946.                     !
!                                                                      !
!  Basic state variables required:  visc4, u, v, Hz.                   !
!                                                                      !
!***********************************************************************
!
      USE mod_param
      USE mod_coupling
!!#ifdef DIAGNOSTICS_UV
!!    USE mod_diags
!!#endif
      USE mod_grid
      USE mod_mixing
      USE mod_ocean
      USE mod_stepping
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, tile
!
!  Local variable declarations.
!
#include "tile.h"
!
#ifdef PROFILE
      CALL wclock_on (ng, iTLM, 32)
#endif
      CALL tl_uv3dmix4_tile (ng, tile,                                  &
     &                       LBi, UBi, LBj, UBj,                        &
     &                       IminS, ImaxS, JminS, JmaxS,                &
     &                       nrhs(ng), nnew(ng),                        &
#ifdef MASKING
     &                       GRID(ng) % pmask,                          &
#endif
     &                       GRID(ng) % Hz,                             &
     &                       GRID(ng) % tl_Hz,                          &
     &                       GRID(ng) % om_p,                           &
     &                       GRID(ng) % om_r,                           &
     &                       GRID(ng) % on_p,                           &
     &                       GRID(ng) % on_r,                           &
     &                       GRID(ng) % pm,                             &
     &                       GRID(ng) % pmon_p,                         &
     &                       GRID(ng) % pmon_r,                         &
     &                       GRID(ng) % pn,                             &
     &                       GRID(ng) % pnom_p,                         &
     &                       GRID(ng) % pnom_r,                         &
     &                       MIXING(ng) % visc4_p,                      &
     &                       MIXING(ng) % visc4_r,                      &
!!#ifdef DIAGNOSTICS_UV
!!   &                       DIAGS(ng) % DiaRUfrc,                      &
!!   &                       DIAGS(ng) % DiaRVfrc,                      &
!!   &                       DIAGS(ng) % DiaU3wrk,                      &
!!   &                       DIAGS(ng) % DiaV3wrk,                      &
!!#endif
     &                       OCEAN(ng) % u,                             &
     &                       OCEAN(ng) % v,                             &
     &                       COUPLING(ng) % tl_rufrc,                   &
     &                       COUPLING(ng) % tl_rvfrc,                   &
     &                       OCEAN(ng) % tl_u,                          &
     &                       OCEAN(ng) % tl_v)
#ifdef PROFILE
      CALL wclock_off (ng, iTLM, 32)
#endif
      RETURN
      END SUBROUTINE tl_uv3dmix4

!
!***********************************************************************
      SUBROUTINE tl_uv3dmix4_tile (ng, tile,                            &
     &                             LBi, UBi, LBj, UBj,                  &
     &                             IminS, ImaxS, JminS, JmaxS,          &
     &                             nrhs, nnew,                          &
#ifdef MASKING
     &                             pmask,                               &
#endif
     &                             Hz, tl_Hz,                           &
     &                             om_p, om_r, on_p, on_r,              &
     &                             pm, pmon_p, pmon_r,                  &
     &                             pn, pnom_p, pnom_r,                  &
     &                             visc4_p, visc4_r,                    &
!!#ifdef DIAGNOSTICS_UV
!!   &                             DiaRUfrc, DiaRVfrc,                  &
!!   &                             DiaU3wrk, DiaV3wrk,                  &
!!#endif
     &                             u, v,                                &
     &                             tl_rufrc, tl_rvfrc, tl_u, tl_v)
!***********************************************************************
!
      USE mod_param
      USE mod_scalars
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, tile
      integer, intent(in) :: LBi, UBi, LBj, UBj
      integer, intent(in) :: IminS, ImaxS, JminS, JmaxS
      integer, intent(in) :: nrhs, nnew

#ifdef ASSUMED_SHAPE
# ifdef MASKING
      real(r8), intent(in) :: pmask(LBi:,LBj:)
# endif
      real(r8), intent(in) :: Hz(LBi:,LBj:,:)
      real(r8), intent(in) :: tl_Hz(LBi:,LBj:,:)
      real(r8), intent(in) :: om_p(LBi:,LBj:)
      real(r8), intent(in) :: om_r(LBi:,LBj:)
      real(r8), intent(in) :: on_p(LBi:,LBj:)
      real(r8), intent(in) :: on_r(LBi:,LBj:)
      real(r8), intent(in) :: pm(LBi:,LBj:)
      real(r8), intent(in) :: pmon_p(LBi:,LBj:)
      real(r8), intent(in) :: pmon_r(LBi:,LBj:)
      real(r8), intent(in) :: pn(LBi:,LBj:)
      real(r8), intent(in) :: pnom_p(LBi:,LBj:)
      real(r8), intent(in) :: pnom_r(LBi:,LBj:)
      real(r8), intent(in) :: visc4_p(LBi:,LBj:)
      real(r8), intent(in) :: visc4_r(LBi:,LBj:)

      real(r8), intent(in) :: u(LBi:,LBj:,:,:)
      real(r8), intent(in) :: v(LBi:,LBj:,:,:)

!!# ifdef DIAGNOSTICS_UV
!!    real(r8), intent(inout) :: DiaRUfrc(LBi:,LBj:,:,:)
!!    real(r8), intent(inout) :: DiaRVfrc(LBi:,LBj:,:,:)
!!    real(r8), intent(inout) :: DiaU3wrk(LBi:,LBj:,:,:)
!!    real(r8), intent(inout) :: DiaV3wrk(LBi:,LBj:,:,:)
!!# endif

      real(r8), intent(inout) :: tl_rufrc(LBi:,LBj:)
      real(r8), intent(inout) :: tl_rvfrc(LBi:,LBj:)
      real(r8), intent(inout) :: tl_u(LBi:,LBj:,:,:)
      real(r8), intent(inout) :: tl_v(LBi:,LBj:,:,:)

#else

# ifdef MASKING
      real(r8), intent(in) :: pmask(LBi:UBi,LBj:UBj)
# endif
      real(r8), intent(in) :: Hz(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(in) :: tl_Hz(LBi:UBi,LBj:UBj,N(ng))
      real(r8), intent(in) :: om_p(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: om_r(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: on_p(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: on_r(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: pm(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: pmon_p(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: pmon_r(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: pn(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: pnom_p(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: pnom_r(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: visc4_p(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: visc4_r(LBi:UBi,LBj:UBj)

      real(r8), intent(inout) :: u(LBi:UBi,LBj:UBj,N(ng),2)
      real(r8), intent(inout) :: v(LBi:UBi,LBj:UBj,N(ng),2)

!!# ifdef DIAGNOSTICS_UV
!!    real(r8), intent(inout) :: DiaRUfrc(LBi:UBi,LBj:UBj,3,NDM2d-1)
!!    real(r8), intent(inout) :: DiaRVfrc(LBi:UBi,LBj:UBj,3,NDM2d-1)
!!    real(r8), intent(inout) :: DiaU3wrk(LBi:UBi,LBj:UBj,N(ng),NDM3d)
!!    real(r8), intent(inout) :: DiaV3wrk(LBi:UBi,LBj:UBj,N(ng),NDM3d)
!!# endif

      real(r8), intent(inout) :: tl_rufrc(LBi:UBi,LBj:UBj)
      real(r8), intent(inout) :: tl_rvfrc(LBi:UBi,LBj:UBj)
      real(r8), intent(inout) :: tl_u(LBi:UBi,LBj:UBj,N(ng),2)
      real(r8), intent(inout) :: tl_v(LBi:UBi,LBj:UBj,N(ng),2)
#endif
!
!  Local variable declarations.
!
      integer :: i, j, k

      real(r8) :: cff, cff1, cff2
      real(r8) :: tl_cff, tl_cff1, tl_cff2

      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: LapU
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: LapV
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: UFe
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: VFe
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: UFx
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: VFx

      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: tl_LapU
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: tl_LapV
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: tl_UFe
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: tl_VFe
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: tl_UFx
      real(r8), dimension(IminS:ImaxS,JminS:JmaxS) :: tl_VFx

#include "set_bounds.h"
!
!-----------------------------------------------------------------------
!  Compute horizontal biharmonic viscosity along constant S-surfaces.
!  The biharmonic operator is computed by applying the harmonic
!  operator twice.
!-----------------------------------------------------------------------
!
      K_LOOP : DO k=1,N(ng)
!
!  Compute flux-components of the horizontal divergence of the stress
!  tensor (m4 s^-3/2) in XI- and ETA-directions.  It is assumed here
!  that "visc4_r" and "visc4_p" are the squared root of the biharmonic
!  viscosity coefficient.  For momentum balance purposes, the
!  thickness "Hz" appears only when computing the second harmonic
!  operator.
!
        DO j=-1+JV_RANGE
          DO i=-1+IU_RANGE
            cff=visc4_r(i,j)*0.5_r8*                                    &
     &          (pmon_r(i,j)*                                           &
     &           ((pn(i  ,j)+pn(i+1,j))*u(i+1,j,k,nrhs)-                &
     &            (pn(i-1,j)+pn(i  ,j))*u(i  ,j,k,nrhs))-               &
     &           pnom_r(i,j)*                                           &
     &           ((pm(i,j  )+pm(i,j+1))*v(i,j+1,k,nrhs)-                &
     &            (pm(i,j-1)+pm(i,j  ))*v(i,j  ,k,nrhs)))
            tl_cff=visc4_r(i,j)*0.5_r8*                                 &
     &             (pmon_r(i,j)*                                        &
     &              ((pn(i  ,j)+pn(i+1,j))*tl_u(i+1,j,k,nrhs)-          &
     &               (pn(i-1,j)+pn(i  ,j))*tl_u(i  ,j,k,nrhs))-         &
     &              pnom_r(i,j)*                                        &
     &              ((pm(i,j  )+pm(i,j+1))*tl_v(i,j+1,k,nrhs)-          &
     &               (pm(i,j-1)+pm(i,j  ))*tl_v(i,j  ,k,nrhs)))
            UFx(i,j)=on_r(i,j)*on_r(i,j)*cff
            tl_UFx(i,j)=on_r(i,j)*on_r(i,j)*tl_cff
            VFe(i,j)=om_r(i,j)*om_r(i,j)*cff
            tl_VFe(i,j)=om_r(i,j)*om_r(i,j)*tl_cff
          END DO
        END DO
        DO j=JU_RANGE+1
          DO i=IV_RANGE+1
            cff=visc4_p(i,j)*0.5_r8*                                    &
     &          (pmon_p(i,j)*                                           &
     &           ((pn(i  ,j-1)+pn(i  ,j))*v(i  ,j,k,nrhs)-              &
     &            (pn(i-1,j-1)+pn(i-1,j))*v(i-1,j,k,nrhs))+             &
     &           pnom_p(i,j)*                                           &
     &           ((pm(i-1,j  )+pm(i,j  ))*u(i,j  ,k,nrhs)-              &
     &            (pm(i-1,j-1)+pm(i,j-1))*u(i,j-1,k,nrhs)))
            tl_cff=visc4_p(i,j)*0.5_r8*                                 &
     &             (pmon_p(i,j)*                                        &
     &              ((pn(i  ,j-1)+pn(i  ,j))*tl_v(i  ,j,k,nrhs)-        &
     &               (pn(i-1,j-1)+pn(i-1,j))*tl_v(i-1,j,k,nrhs))+       &
     &              pnom_p(i,j)*                                        &
     &              ((pm(i-1,j  )+pm(i,j  ))*tl_u(i,j  ,k,nrhs)-        &
     &               (pm(i-1,j-1)+pm(i,j-1))*tl_u(i,j-1,k,nrhs)))
#ifdef MASKING
            cff=cff*pmask(i,j)
            tl_cff=tl_cff*pmask(i,j)
#endif
            UFe(i,j)=om_p(i,j)*om_p(i,j)*cff
            tl_UFe(i,j)=om_p(i,j)*om_p(i,j)*tl_cff
            VFx(i,j)=on_p(i,j)*on_p(i,j)*cff
            tl_VFx(i,j)=on_p(i,j)*on_p(i,j)*tl_cff
          END DO
        END DO
!
!  Compute first harmonic operator (m s^-3/2).
!
        DO j=JU_RANGE
          DO i=IU_RANGE
            LapU(i,j)=0.125_r8*                                         &
     &                (pm(i-1,j)+pm(i,j))*(pn(i-1,j)+pn(i,j))*          &
     &                ((pn(i-1,j)+pn(i,j))*                             &
     &                 (UFx(i,j  )-UFx(i-1,j))+                         &
     &                 (pm(i-1,j)+pm(i,j))*                             &
     &                 (UFe(i,j+1)-UFe(i  ,j)))
            tl_LapU(i,j)=0.125_r8*                                      &
     &                   (pm(i-1,j)+pm(i,j))*(pn(i-1,j)+pn(i,j))*       &
     &                   ((pn(i-1,j)+pn(i,j))*                          &
     &                    (tl_UFx(i,j  )-tl_UFx(i-1,j))+                &
     &                    (pm(i-1,j)+pm(i,j))*                          &
     &                    (tl_UFe(i,j+1)-tl_UFe(i  ,j)))
          END DO
        END DO
        DO j=JV_RANGE
          DO i=IV_RANGE
            LapV(i,j)=0.125_r8*                                         &
     &                (pm(i,j)+pm(i,j-1))*(pn(i,j)+pn(i,j-1))*          &
     &                ((pn(i,j-1)+pn(i,j))*                             &
     &                 (VFx(i+1,j)-VFx(i,j  ))-                         &
     &                 (pm(i,j-1)+pm(i,j))*                             &
     &                 (VFe(i  ,j)-VFe(i,j-1)))
            tl_LapV(i,j)=0.125_r8*                                      &
     &                   (pm(i,j)+pm(i,j-1))*(pn(i,j)+pn(i,j-1))*       &
     &                   ((pn(i,j-1)+pn(i,j))*                          &
     &                    (tl_VFx(i+1,j)-tl_VFx(i,j  ))-                &
     &                    (pm(i,j-1)+pm(i,j))*                          &
     &                    (tl_VFe(i  ,j)-tl_VFe(i,j-1)))
          END DO
        END DO
!
!  Apply boundary conditions (other than periodic) to the first
!  harmonic operator. These are gradient or closed (free slip or
!  no slip) boundary conditions.
!
#ifndef EW_PERIODIC
        IF (WESTERN_EDGE) THEN
          DO j=JU_RANGE
# ifdef WESTERN_WALL
            LapU(Istr,j)=0.0_r8
            tl_LapU(Istr,j)=0.0_r8
# else
            LapU(Istr,j)=LapU(Istr+1,j)
            tl_LapU(Istr,j)=tl_LapU(Istr+1,j)
# endif
          END DO
          DO j=JV_RANGE
# ifdef WESTERN_WALL
            LapV(Istr-1,j)=gamma2(ng)*LapV(Istr,j)
            tl_LapV(Istr-1,j)=gamma2(ng)*tl_LapV(Istr,j)
# else
            LapV(Istr-1,j)=0.0_r8
            tl_LapV(Istr-1,j)=0.0_r8
# endif
          END DO
        END IF
        IF (EASTERN_EDGE) THEN
          DO j=JU_RANGE
# ifdef EASTERN_WALL
            LapU(Iend+1,j)=0.0_r8
            tl_LapU(Iend+1,j)=0.0_r8
# else
            LapU(Iend+1,j)=LapU(Iend,j)
            tl_LapU(Iend+1,j)=tl_LapU(Iend,j)
# endif
          END DO
          DO j=JV_RANGE
# ifdef EASTERN_WALL
            LapV(Iend+1,j)=gamma2(ng)*LapV(Iend,j)
            tl_LapV(Iend+1,j)=gamma2(ng)*tl_LapV(Iend,j)
# else
            LapV(Iend+1,j)=0.0_r8
            tl_LapV(Iend+1,j)=0.0_r8
# endif
          END DO
        END IF
#endif
#ifndef NS_PERIODIC
        IF (SOUTHERN_EDGE) THEN
          DO i=IU_RANGE
# ifdef SOUTHERN_WALL
            LapU(i,Jstr-1)=gamma2(ng)*LapU(i,Jstr)
            tl_LapU(i,Jstr-1)=gamma2(ng)*tl_LapU(i,Jstr)
# else
            LapU(i,Jstr-1)=0.0_r8
            tl_LapU(i,Jstr-1)=0.0_r8
# endif
          END DO
          DO i=IV_RANGE
# ifdef SOUTHERN_WALL
            LapV(i,Jstr)=0.0_r8
            tl_LapV(i,Jstr)=0.0_r8
# else
            LapV(i,Jstr)=LapV(i,Jstr+1)
            tl_LapV(i,Jstr)=tl_LapV(i,Jstr+1)
# endif
          END DO
        END IF
        IF (NORTHERN_EDGE) THEN
          DO i=IU_RANGE
# ifdef NORTHERN_WALL
            LapU(i,Jend+1)=gamma2(ng)*LapU(i,Jend)
            tl_LapU(i,Jend+1)=gamma2(ng)*tl_LapU(i,Jend)
# else
            LapU(i,Jend+1)=0.0_r8
            tl_LapU(i,Jend+1)=0.0_r8
# endif
          END DO
          DO i=IV_RANGE
# ifdef NORTHERN_WALL
            LapV(i,Jend+1)=0.0_r8
            tl_LapV(i,Jend+1)=0.0_r8
# else
            LapV(i,Jend+1)=LapV(i,Jend)
            tl_LapV(i,Jend+1)=tl_LapV(i,Jend)
# endif
          END DO
        END IF
#endif
#if !defined EW_PERIODIC && !defined NS_PERIODIC
        IF ((SOUTHERN_EDGE).and.(WESTERN_EDGE)) THEN
          LapU(Istr  ,Jstr-1)=0.5_r8*(LapU(Istr+1,Jstr-1)+              &
     &                                LapU(Istr  ,Jstr  ))
          tl_LapU(Istr  ,Jstr-1)=0.5_r8*(tl_LapU(Istr+1,Jstr-1)+        &
     &                                   tl_LapU(Istr  ,Jstr  ))
          LapV(Istr-1,Jstr  )=0.5_r8*(LapV(Istr-1,Jstr+1)+              &
     &                                LapV(Istr  ,Jstr  ))
          tl_LapV(Istr-1,Jstr  )=0.5_r8*(tl_LapV(Istr-1,Jstr+1)+        &
     &                                   tl_LapV(Istr  ,Jstr  ))
        END IF
        IF ((SOUTHERN_EDGE).and.(EASTERN_EDGE)) THEN
          LapU(Iend+1,Jstr-1)=0.5_r8*(LapU(Iend  ,Jstr-1)+              &
     &                                LapU(Iend+1,Jstr  ))
          tl_LapU(Iend+1,Jstr-1)=0.5_r8*(tl_LapU(Iend  ,Jstr-1)+        &
     &                                   tl_LapU(Iend+1,Jstr  ))
          LapV(Iend+1,Jstr  )=0.5_r8*(LapV(Iend  ,Jstr  )+              &
     &                                LapV(Iend+1,Jstr+1))
          tl_LapV(Iend+1,Jstr  )=0.5_r8*(tl_LapV(Iend  ,Jstr  )+        &
     &                                   tl_LapV(Iend+1,Jstr+1))
        END IF
        IF ((NORTHERN_EDGE).and.(WESTERN_EDGE)) THEN
          LapU(Istr  ,Jend+1)=0.5_r8*(LapU(Istr+1,Jend+1)+              &
     &                                LapU(Istr  ,Jend  ))
          tl_LapU(Istr  ,Jend+1)=0.5_r8*(tl_LapU(Istr+1,Jend+1)+        &
     &                                   tl_LapU(Istr  ,Jend  ))
          LapV(Istr-1,Jend+1)=0.5_r8*(LapV(Istr  ,Jend+1)+              &
     &                                LapV(Istr-1,Jend  ))
          tl_LapV(Istr-1,Jend+1)=0.5_r8*(tl_LapV(Istr  ,Jend+1)+        &
     &                                   tl_LapV(Istr-1,Jend  ))
        END IF
        IF ((NORTHERN_EDGE).and.(EASTERN_EDGE)) THEN
          LapU(Iend+1,Jend+1)=0.5_r8*(LapU(Iend  ,Jend+1)+              &
     &                                LapU(Iend+1,Jend  ))
          tl_LapU(Iend+1,Jend+1)=0.5_r8*(tl_LapU(Iend  ,Jend+1)+        &
     &                                   tl_LapU(Iend+1,Jend  ))
          LapV(Iend+1,Jend+1)=0.5_r8*(LapV(Iend  ,Jend+1)+              &
     &                                LapV(Iend+1,Jend  ))
          tl_LapV(Iend+1,Jend+1)=0.5_r8*(tl_LapV(Iend  ,Jend+1)+        &
     &                                   tl_LapV(Iend+1,Jend  ))
        END IF
#endif
!
!  Compute flux-components of the horizontal divergence of the
!  harmonic stress tensor (m4/s2) in XI- and ETA-directions.
!
        DO j=JstrV-1,Jend
          DO i=IstrU-1,Iend
!>          cff=visc4_r(i,j)*Hz(i,j,k)*0.5_r8*                          &
!>   &          (pmon_r(i,j)*                                           &
!>   &           ((pn(i  ,j)+pn(i+1,j))*LapU(i+1,j)-                    &
!>   &            (pn(i-1,j)+pn(i  ,j))*LapU(i  ,j))-                   &
!>   &           pnom_r(i,j)*                                           &
!>   &           ((pm(i,j  )+pm(i,j+1))*LapV(i,j+1)-                    &
!>   &            (pm(i,j-1)+pm(i,j  ))*LapV(i,j  )))
!>
            tl_cff=visc4_r(i,j)*0.5_r8*                                 &
     &             (tl_Hz(i,j,k)*                                       &
     &              (pmon_r(i,j)*                                       &
     &               ((pn(i  ,j)+pn(i+1,j))*LapU(i+1,j)-                &
     &                (pn(i-1,j)+pn(i  ,j))*LapU(i  ,j))-               &
     &               pnom_r(i,j)*                                       &
     &               ((pm(i,j  )+pm(i,j+1))*LapV(i,j+1)-                &
     &                (pm(i,j-1)+pm(i,j  ))*LapV(i,j  )))+              &
     &              Hz(i,j,k)*                                          &
     &              (pmon_r(i,j)*                                       &
     &               ((pn(i  ,j)+pn(i+1,j))*tl_LapU(i+1,j)-             &
     &                (pn(i-1,j)+pn(i  ,j))*tl_LapU(i  ,j))-            &
     &               pnom_r(i,j)*                                       &
     &               ((pm(i,j  )+pm(i,j+1))*tl_LapV(i,j+1)-             &
     &                (pm(i,j-1)+pm(i,j  ))*tl_LapV(i,j  ))))
!>          UFx(i,j)=on_r(i,j)*on_r(i,j)*cff
!>
            tl_UFx(i,j)=on_r(i,j)*on_r(i,j)*tl_cff
!>          VFe(i,j)=om_r(i,j)*om_r(i,j)*cff
!>
            tl_VFe(i,j)=om_r(i,j)*om_r(i,j)*tl_cff
          END DO
        END DO
        DO j=Jstr,Jend+1
          DO i=Istr,Iend+1
!>          cff=visc4_p(i,j)*0.125_r8*(Hz(i-1,j  ,k)+Hz(i,j  ,k)+       &
!>   &                                 Hz(i-1,j-1,k)+Hz(i,j-1,k))*      &
!>   &          (pmon_p(i,j)*                                           &
!>   &           ((pn(i  ,j-1)+pn(i  ,j))*LapV(i  ,j)-                  &
!>   &            (pn(i-1,j-1)+pn(i-1,j))*LapV(i-1,j))+                 &
!>   &           pnom_p(i,j)*                                           &
!>   &           ((pm(i-1,j  )+pm(i,j  ))*LapU(i,j  )-                  &
!>   &            (pm(i-1,j-1)+pm(i,j-1))*LapU(i,j-1)))
!>
            tl_cff=visc4_p(i,j)*0.125_r8*                               &
     &             ((tl_Hz(i-1,j  ,k)+tl_Hz(i,j  ,k)+                   &
     &               tl_Hz(i-1,j-1,k)+tl_Hz(i,j-1,k))*                  &
     &              (pmon_p(i,j)*                                       &
     &               ((pn(i  ,j-1)+pn(i  ,j))*LapV(i  ,j)-              &
     &                (pn(i-1,j-1)+pn(i-1,j))*LapV(i-1,j))+             &
     &               pnom_p(i,j)*                                       &
     &               ((pm(i-1,j  )+pm(i,j  ))*LapU(i,j  )-              &
     &                (pm(i-1,j-1)+pm(i,j-1))*LapU(i,j-1)))+            &
     &              (Hz(i-1,j  ,k)+Hz(i,j  ,k)+                         &
     &               Hz(i-1,j-1,k)+Hz(i,j-1,k))*                        &
     &               (pmon_p(i,j)*                                      &
     &                ((pn(i  ,j-1)+pn(i  ,j))*tl_LapV(i  ,j)-          &
     &                 (pn(i-1,j-1)+pn(i-1,j))*tl_LapV(i-1,j))+         &
     &                pnom_p(i,j)*                                      &
     &                ((pm(i-1,j  )+pm(i,j  ))*tl_LapU(i,j  )-          &
     &                 (pm(i-1,j-1)+pm(i,j-1))*tl_LapU(i,j-1))))
#ifdef MASKING
!>          cff=cff*pmask(i,j)
!>
            tl_cff=tl_cff*pmask(i,j)
#endif
!>          UFe(i,j)=om_p(i,j)*om_p(i,j)*cff
!>
            tl_UFe(i,j)=om_p(i,j)*om_p(i,j)*tl_cff
!>          VFx(i,j)=on_p(i,j)*on_p(i,j)*cff
!>
            tl_VFx(i,j)=on_p(i,j)*on_p(i,j)*tl_cff
          END DO
        END DO
!
! Time-step biharmonic, S-surfaces viscosity term.  Notice that
! momentum at this stage is HzU and HzV and has units m2/s.  Add
! contribution for barotropic forcing terms.
!
        DO j=Jstr,Jend
          DO i=IstrU,Iend
            cff=0.25_r8*(pm(i-1,j)+pm(i,j))*(pn(i-1,j)+pn(i,j))
!>          cff1=0.5_r8*((pn(i-1,j)+pn(i,j))*                           &
!>   &                   (UFx(i,j  )-UFx(i-1,j))+                       &
!>   &                   (pm(i-1,j)+pm(i,j))*                           &
!>   &                   (UFe(i,j+1)-UFe(i  ,j)))
!>
            tl_cff1=0.5_r8*((pn(i-1,j)+pn(i,j))*                        &
     &                      (tl_UFx(i,j  )-tl_UFx(i-1,j))+              &
     &                      (pm(i-1,j)+pm(i,j))*                        &
     &                      (tl_UFe(i,j+1)-tl_UFe(i  ,j)))
!>          cff2=dt(ng)*cff*cff1
!>
            tl_cff2=dt(ng)*cff*tl_cff1
!>          rufrc(i,j)=rufrc(i,j)-cff1
!>
            tl_rufrc(i,j)=tl_rufrc(i,j)-tl_cff1
!>          u(i,j,k,nnew)=u(i,j,k,nnew)-cff2
!>
            tl_u(i,j,k,nnew)=tl_u(i,j,k,nnew)-tl_cff2
!!#ifdef DIAGNOSTICS_UV
!!          DiaRUfrc(i,j,3,M2hvis)=DiaRUfrc(i,j,3,M2hvis)-cff1
!!          DiaU3wrk(i,j,k,M3hvis)=-cff2
!!#endif
          END DO
        END DO
        DO j=JstrV,Jend
          DO i=Istr,Iend
            cff=0.25_r8*(pm(i,j)+pm(i,j-1))*(pn(i,j)+pn(i,j-1))
!>          cff1=0.5_r8*((pn(i,j-1)+pn(i,j))*                           &
!>   &                   (VFx(i+1,j)-VFx(i,j  ))-                       &
!>   &                   (pm(i,j-1)+pm(i,j))*                           &
!>   &                   (VFe(i  ,j)-VFe(i,j-1)))
!>
            tl_cff1=0.5_r8*((pn(i,j-1)+pn(i,j))*                        &
     &                      (tl_VFx(i+1,j)-tl_VFx(i,j  ))-              &
     &                      (pm(i,j-1)+pm(i,j))*                        &
     &                      (tl_VFe(i  ,j)-tl_VFe(i,j-1)))
!>          cff2=dt(ng)*cff*cff1
!>
            tl_cff2=dt(ng)*cff*tl_cff1
!>          rvfrc(i,j)=rvfrc(i,j)-cff1
!>
            tl_rvfrc(i,j)=tl_rvfrc(i,j)-tl_cff1
!>          v(i,j,k,nnew)=v(i,j,k,nnew)-cff2
!>
            tl_v(i,j,k,nnew)=tl_v(i,j,k,nnew)-tl_cff2
!!#ifdef DIAGNOSTICS_UV
!!          DiaRVfrc(i,j,3,M2hvis)=DiaRVfrc(i,j,3,M2hvis)-cff1
!!          DiaV3wrk(i,j,k,M3hvis)=-cff2
!!#endif
          END DO
        END DO
      END DO K_LOOP
#undef IU_RANGE
#undef IV_RANGE
#undef JU_RANGE
#undef JV_RANGE
      RETURN
      END SUBROUTINE tl_uv3dmix4_tile
