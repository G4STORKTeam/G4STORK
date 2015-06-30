/*
SCWRDopplerConstructor.hh

Created by:		Wesley Ford
Date:			14-05-2014
Modified:       NA

Header for SCWRDopplerConstructor class.

This class creates the simulation geometry for a SCWR.  The
material and geometric composition of the lattice cell were taken from the
DRAGON manual[1].

Based on the "C6World" class created by Wesley Ford 10-05-2012.

[1] G. Marleau, A. Hebert, and R. Roy, "A User Guide for DRAGON 3.06".  Ecole
Polytechnique de Montreal, 2012, pp. 148-152. IGE-174 Rev. 10.

*/

#ifndef SCWRDopplerConstructor_H
#define SCWRDopplerConstructor_H

// Include header files
#include "StorkVWorldConstructor.hh"
#include "StorkNeutronSD.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include <sstream>


class SCWRDopplerConstructor: public StorkVWorldConstructor
{
	public:
		// Public member functions

		SCWRDopplerConstructor();
		virtual ~SCWRDopplerConstructor();


    protected:
        // Protected member functions

		virtual G4VPhysicalVolume* ConstructWorld();
        virtual void ConstructMaterials();

    protected:
        // Protected member variables

        // Logical Volumes
        G4LogicalVolume *cellLogical;
        G4LogicalVolume *pressTubeLogical;
        G4LogicalVolume *outLinerLogical;
        G4LogicalVolume *insulatorLogical;
        G4LogicalVolume *linerLogical;
        G4LogicalVolume *coolantLogical;
        G4LogicalVolume *outSheatheLogical;
        G4LogicalVolume *outSheatheLogicalH1;
        G4LogicalVolume *outSheatheLogicalH2;
        G4LogicalVolume *inSheatheLogical;
        G4LogicalVolume *inSheatheLogicalH1;
        G4LogicalVolume *inSheatheLogicalH2;
        G4LogicalVolume *outFuelLogical;
        G4LogicalVolume *outFuelLogicalH1;
        G4LogicalVolume *outFuelLogicalH2;
        G4LogicalVolume *inFuelLogical;
        G4LogicalVolume *inFuelLogicalH1;
        G4LogicalVolume *inFuelLogicalH2;
        G4LogicalVolume *flowTubeLogical;
        G4LogicalVolume *centralCoolantLogical;


        // Visualization attributes
        G4VisAttributes *cellVisAtt;
        G4VisAttributes *pressTubeVisAtt;
        G4VisAttributes *outLinerVisAtt;
        G4VisAttributes *insulatorVisAtt;
        G4VisAttributes *linerVisAtt;
        G4VisAttributes *coolantVisAtt;
        G4VisAttributes *outSheatheVisAtt;
        G4VisAttributes *inSheatheVisAtt;
        G4VisAttributes *outFuelVisAtt;
        G4VisAttributes *inFuelVisAtt;
//        G4VisAttributes *outFlowTubeVisAtt;
        G4VisAttributes *flowTubeVisAtt;
//        G4VisAttributes *inFlowTubeVisAtt;
        G4VisAttributes *centralCoolantVisAtt;

        // Stored variables from infile
        G4double latticePitch= 25.*cm;

        G4double moderatorTemp=342.00*kelvin;
        G4double moderatorDensity=1.0851*g/cm3;

        G4double pressTubeTemp=416.74;
        G4double pressTubeDensity=6.52*g/cm3;

        G4double outLinerTemp=470.5200*kelvin;
        G4double outLinerDensity=6.52*g/cm3;

        G4double insulatorTemp=557.17*kelvin;
        G4double insulatorDensity=5.83*g/cm3;

        G4double linerTemp=671.80*kelvin;
        G4double linerDensity=7.9*g/cm3;

        G4double coolantTemp=681.79*kelvin;
        G4double coolantDensity=0.14933*g/cm3;
        //G4double coolantDensity=0.001*g/cm3;

        G4double inSheatheTemp=756.30*kelvin;
        G4double inSheatheDensity=7.9*g/cm3;

        G4double outSheatheTemp=756.30*kelvin;
        G4double outSheatheDensity=7.9*g/cm3;

        G4double innerFuelTemp=1420.62;
        G4double innerFuelDensity=9.91*g/cm3;

        G4double outerFuelTemp=1420.62;
        G4double outerFuelDensity=9.87*g/cm3;

//        G4double outFlowTubeTemp=420.00+273.15;
//        G4double outFlowTubeDensity=7.9*g/cm3;
//
//        G4double flowTubeTemp[2]={379.00+273.15, 406.42+273.15};
//        G4double flowTubeDensity=5.64*g/cm3;
//
//        G4double inFlowTubeTemp=361.76+273.15;
//        G4double inFlowTubeDensity=7.9*g/cm3;

// AECL Modification, need a new temperature analysis

        G4double flowTubeTemp=657.79*kelvin;
        G4double flowTubeDensity=7.9*g/cm3;

        G4double centralCoolantTemp=633.79*kelvin;
        G4double centralCoolantDensity=0.58756*g/cm3;
};

#endif // SCWRDopplerConstructor_H
