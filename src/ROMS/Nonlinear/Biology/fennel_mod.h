!
!svn $Id: fennel_mod.h 429 2009-12-20 17:30:26Z arango $
!================================================== Hernan G. Arango ===
!  Copyright (c) 2002-2010 The ROMS/TOMS Group                         !
!    Licensed under a MIT/X style license                              !
!    See License_ROMS.txt                                              !
!=======================================================================
!                                                                      !
!  Parameters for Fennel et al. (2006) model:                          !
!                                                                      !
!   AttSW    Light attenuation due to sea water [1/m].                 !
!   AttChl   Light attenuation by Chlorophyll [1/(mg_Chl m2)].         !
!   BioIter  Maximum number of iterations to achieve convergence       !
!              of the nonlinear solution.                              !
!   Chl2C_m  Maximum chlorophyll to carbon ratio [mg_Chl/mg_C].        !
!   ChlMin   Chlorophill minimum threshold value [mg_Chl/m3].          !
!   CoagR    Coagulation rate: agregation rate of SDeN + Phyt ==> LDeN !
!              [1/day].                                                !
!   D_p5NH4  Half-saturation radiation for nitrification inhibition    !
!              [Watts/m2].                                             !
!   I_thNH4  Radiation threshold for nitrification inhibition          !
!              [Watts/m2].                                             !
!   K_NH4    Inverse half-saturation for Phytoplankton NH4 uptake      !
!              [m3/(mmol_N)].                                          !
!   K_NO3    Inverse half-saturation for Phytoplankton NO3 uptake      !
!              [m3/(mmol_N)].                                          !
!   K_Phy    Zooplankton half-saturation, squared constant for         !
!              ingestion [mmol_N/m3]^2.                                !
!   LDeRR    Large Detrital re-mineralization rate [1/day].            !
!   NitriR   Nitrification rate: oxidation of NH4 to NO3 [1/day].      !
!   PARfrac  Fraction of shortwave radiation that is available for     !
!              photosyntesis [nondimensional].                         !
!   PhyCN    Phytoplankton Carbon:Nitrogen ratio [mol_C/mol_N].        !
!   PhyIP    Phytoplankton NH4 inhibition parameter [1/(mmol_N)].      !
!   PhyIS    Phytoplankton, initial slope of the P-I curve             !
!              [mg_C/(mg_Chl W m-2 day)].                              !
!   ZooMin   Phytoplankton minimum threshold value [mmol_N/m3].        !
!   PhyMR    Phytoplankton mortality rate [1/day] to small detritus.   !
!   SDeAR    Small detritus aggregation rate into Large detritus       !
!              [1/day].                                                !
!   SDeBR    Small Detrital breakdown to NH4 rate [1/day].             !
!   SDeRR    Large Detrital re-mineralization rate [1/day].            !
!   Vp0      Eppley temperature-limited and light-limited growth       !
!              tuning parameter [nondimensional].                      !
!   wLDet    Vertical sinking velocities for Large Detritus            !
!              fraction [m/day].                                       !
!   wPhy     Vertical sinking velocity for Phytoplankton               !
!              fraction [m/day].                                       !
!   wSDet    Vertical sinking velocities for Small Detritus            !
!              fraction [m/day].                                       !
!   ZooAE_N  Zooplankton nitrogen assimilation efficiency fraction     !
!              [nondimensional].                                       !
!   ZooBM    Zooplankton basal metabolism [1/day].                     !
!   ZooCN    Zooplankton Carbon:Nitrogen ratio [mol_C/mol_N].          !
!   ZooER    Zooplankton specific excretion rate [1/day].              !
!   ZooGR    Zooplankton maximum growth rate [1/day].                  !
!   ZooMin   Zooplankton minimum threshold value [mmol_N/m3].          !
!   ZooMR    Zooplankton mortality to Detritus [1/day].                !
!   pCO2air  CO2 partial pressure in the air [ppmv].                   !
!                                                                      !
!=======================================================================
!
      USE mod_param
!
      implicit none
!
!  Set biological tracer identification indices.
!
      integer, allocatable :: idbio(:)  ! Biological tracers
      integer :: iNO3_                  ! Nitrate concentration
      integer :: iNH4_                  ! Ammonium concentration
      integer :: iChlo                  ! Chlorophyll concentration
      integer :: iPhyt                  ! Phytoplankton concentration
      integer :: iZoop                  ! Zooplankton concentration
      integer :: iLDeN                  ! Large detritus N-concentration
      integer :: iSDeN                  ! Small detritus N-concentration
#ifdef CARBON
      integer :: iLDeC                  ! Large detritus C-concentration
      integer :: iSDeC                  ! Small detritus C-concentration
      integer :: iTIC_                  ! Total inorganic carbon
      integer :: iTAlk                  ! Total alkalinity
#endif
#ifdef OXYGEN
      integer :: iOxyg                  ! Dissolved oxygen concentration
#endif

#if defined DIAGNOSTICS && defined DIAGNOSTICS_BIO
!
!  Biological 2D diagnostic variable IDs.
!
      integer, allocatable :: iDbio2(:)       ! 2D biological terms

      integer  :: iCOfx                       ! air-sea CO2 flux
      integer  :: iDNIT                       ! denitrification flux
      integer  :: ipCO2                       ! partial pressure of CO2
      integer  :: iO2fx                       ! air-sea O2 flux
!
!  Biological 3D diagnostic variable IDs.
!
      integer, allocatable :: iDbio3(:)       ! 3D biological terms

      integer  :: iPPro = 1                   ! primary productivity
      integer  :: iNO3u = 2                   ! NO3 uptake
#endif
!
!  Biological parameters.
!
      integer, dimension(Ngrids) :: BioIter

      real(r8), dimension(Ngrids) :: AttSW           ! 1/m
      real(r8), dimension(Ngrids) :: AttChl          ! 1/(mg_Chl m2)
      real(r8), dimension(Ngrids) :: Chl2C_m         ! mg_Chl/mg_C
      real(r8), dimension(Ngrids) :: ChlMin          ! mg_Chl/m3
      real(r8), dimension(Ngrids) :: CoagR           ! 1/day
      real(r8), dimension(Ngrids) :: D_p5NH4         ! Watts/m2
      real(r8), dimension(Ngrids) :: I_thNH4         ! Watts/m2
      real(r8), dimension(Ngrids) :: K_NH4           ! m3/mmol_N
      real(r8), dimension(Ngrids) :: K_NO3           ! m3/mmol_N
      real(r8), dimension(Ngrids) :: K_Phy           ! (mmol_N/m3)^2
      real(r8), dimension(Ngrids) :: LDeRRN          ! 1/day
      real(r8), dimension(Ngrids) :: LDeRRC          ! 1/day
      real(r8), dimension(Ngrids) :: NitriR          ! 1/day
      real(r8), dimension(Ngrids) :: PARfrac         ! nondimensional
      real(r8), dimension(Ngrids) :: PhyCN           ! mol_C/mol_N
      real(r8), dimension(Ngrids) :: PhyIP           ! 1/mmol_N
      real(r8), dimension(Ngrids) :: PhyIS           ! 1/(Watts m-2 day)
      real(r8), dimension(Ngrids) :: PhyMin          ! mmol_N/m3
      real(r8), dimension(Ngrids) :: PhyMR           ! 1/day
      real(r8), dimension(Ngrids) :: SDeAR           ! 1/day
      real(r8), dimension(Ngrids) :: SDeBR           ! 1/day
      real(r8), dimension(Ngrids) :: SDeRRN          ! 1/day
      real(r8), dimension(Ngrids) :: SDeRRC          ! 1/day
      real(r8), dimension(Ngrids) :: Vp0             ! nondimensional
      real(r8), dimension(Ngrids) :: wLDet           ! m/day
      real(r8), dimension(Ngrids) :: wPhy            ! m/day
      real(r8), dimension(Ngrids) :: wSDet           ! m/day
      real(r8), dimension(Ngrids) :: ZooAE_N         ! nondimensional
      real(r8), dimension(Ngrids) :: ZooBM           ! 1/day
      real(r8), dimension(Ngrids) :: ZooCN           ! mol_C/mol_N
      real(r8), dimension(Ngrids) :: ZooER           ! 1/day
      real(r8), dimension(Ngrids) :: ZooGR           ! 1/day
      real(r8), dimension(Ngrids) :: ZooMin          ! mmol_N/m3
      real(r8), dimension(Ngrids) :: ZooMR           ! 1/day
      real(r8), dimension(Ngrids) :: pCO2air         ! ppmv

      CONTAINS

      SUBROUTINE initialize_biology
!
!=======================================================================
!                                                                      !
!  This routine sets several variables needed by the biology model.    !
!  It allocates and assigns biological tracers indices.                !
!                                                                      !
!=======================================================================
!
!  Local variable declarations
!
      integer :: i, ic
!
!-----------------------------------------------------------------------
!  Determine number of biological tracers.
!-----------------------------------------------------------------------
!
#ifdef CARBON
# ifdef OXYGEN
      NBT=12
# else
      NBT=11
# endif
#else
# ifdef OXYGEN
      NBT=8
# else
      NBT=7
# endif
#endif

#if defined DIAGNOSTICS && defined DIAGNOSTICS_BIO
!
!-----------------------------------------------------------------------
!  Set sources and sinks biology diagnostic parameters.
!-----------------------------------------------------------------------
!
!  Set number of diagnostics terms.
!
      NDbio3d=2
      NDbio2d=0
# ifdef DENITRIFICATION
      NDbio2d=NDbio2d+1
# endif
# ifdef CARBON
      NDbio2d=NDbio2d+2
# endif
# ifdef OXYGEN
      NDbio2d=NDbio2d+1
# endif
!
!  Allocate biological diagnostics vectors
!
      IF (.not.allocated(iDbio2)) THEN
        allocate ( iDbio2(NDbio2d) )
      END IF
      IF (.not.allocated(iDbio3)) THEN
        allocate ( iDbio3(NDbio3d) )
      END IF
!
!  Initialize biology diagnostic indices.
!
      ic=0
# ifdef DENITRIFICATION
      iDNIT=ic+1
      ic=ic+1
# endif
# ifdef CARBON
      iCOfx=ic+1
      ipCO2=ic+2
      ic=ic+2
# endif
# ifdef OXYGEN
      iO2fx=ic+1
# endif
#endif
!
!-----------------------------------------------------------------------
!  Initialize tracer identification indices.
!-----------------------------------------------------------------------
!
!  Allocate biological tracer vector.
!
      IF (.not.allocated(idbio)) THEN
        allocate ( idbio(NBT) )
      END IF
!
!  Set identification indices.
!
      ic=NAT+NPT+NCS+NNS
      DO i=1,NBT
        idbio(i)=ic+i
      END DO
      iNO3_=ic+1
      iNH4_=ic+2
      iChlo=ic+3
      iPhyt=ic+4
      iZoop=ic+5
      iLDeN=ic+6
      iSDeN=ic+7
      ic=ic+7
# ifdef CARBON
      iLDeC=ic+1
      iSDeC=ic+2
      iTIC_=ic+3
      iTAlk=ic+4
      ic=ic+4
# endif
# ifdef OXYGEN
      iOxyg=ic+1
      ic=ic+1
# endif

      RETURN
      END SUBROUTINE initialize_biology
