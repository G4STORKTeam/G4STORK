/*
SCWRConstructor.hh

Created by:		Wesley Ford
Date:			14-05-2014
Modified:       NA

Header for SCWRConstructor class.

This class creates the simulation geometry for a SCWR.  The
material and geometric composition of the lattice cell were taken from the
DRAGON manual[1].

Based on the "C6World" class created by Wesley Ford 10-05-2012.

[1] G. Marleau, A. Hebert, and R. Roy, "A User Guide for DRAGON 3.06".  Ecole
Polytechnique de Montreal, 2012, pp. 148-152. IGE-174 Rev. 10.

*/

#ifndef SCWRCONSTRUCTOR_H
#define SCWRCONSTRUCTOR_H

// Include header files
#include "StorkVWorldConstructor.hh"
#include "StorkNeutronSD.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include <sstream>


class SCWRConstructor: public StorkVWorldConstructor
{
	public:
		// Public member functions

		SCWRConstructor();
		virtual ~SCWRConstructor();


    protected:
        // Protected member functions

		virtual G4VPhysicalVolume* ConstructWorld();
        virtual void ConstructMaterials();

    protected:
        // Protected member variables

        // Logical Volumes
        G4LogicalVolume *cellLogical;
        G4LogicalVolume *pressTubeLogical1;
        G4LogicalVolume *pressTubeLogical2;
        G4LogicalVolume *pressTubeLogical3;
        G4LogicalVolume *pressTubeLogical4;
        G4LogicalVolume *outLinerLogical;
        G4LogicalVolume *insulatorLogical1;
        G4LogicalVolume *insulatorLogical2;
        G4LogicalVolume *insulatorLogical3;
        G4LogicalVolume *insulatorLogical4;
        G4LogicalVolume *linerLogical;
        G4LogicalVolume *coolantLogical;
        G4LogicalVolume *outSheatheLogical;
        G4LogicalVolume *inSheatheLogical;
        G4LogicalVolume *outFuelLogical1;
        G4LogicalVolume *outFuelLogical2;
        G4LogicalVolume *outFuelLogical3;
        G4LogicalVolume *outFuelLogical4;
        G4LogicalVolume *inFuelLogical1;
        G4LogicalVolume *inFuelLogical2;
        G4LogicalVolume *inFuelLogical3;
        G4LogicalVolume *inFuelLogical4;
//        G4LogicalVolume *outFlowTubeLogical;
//        G4LogicalVolume *flowTubeLogical1;
//        G4LogicalVolume *flowTubeLogical2;
//        G4LogicalVolume *inFlowTubeLogical;
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

        G4double moderatorTemp=342.8200*kelvin;
        G4double moderatorDensity=1.0851*g/cm3;

        G4double pressTubeTemp[4]={459.8850*kelvin, 441.8700*kelvin, 424.2300*kelvin, 406.9300*kelvin};
        G4double pressTubeDensity=6.52*g/cm3;

        G4double outLinerTemp=470.5200*kelvin;
        G4double outLinerDensity=6.52*g/cm3;

        G4double insulatorTemp[4]={643.2050*kelvin, 592.4750*kelvin, 543.2950*kelvin, 495.5700*kelvin};
        G4double insulatorDensity=5.83*g/cm3;

        G4double linerTemp=670.2600*kelvin;
        G4double linerDensity=7.9*g/cm3;

        G4double coolantTemp=680.62*kelvin;
        G4double coolantDensity=0.1512*g/cm3;

        G4double inSheatheTemp=768.8200*kelvin;
        G4double inSheatheDensity=7.9*g/cm3;

        G4double outSheatheTemp=780.9300*kelvin;
        G4double outSheatheDensity=7.9*g/cm3;

        G4double innerFuelTemp[4]={1389.8000*kelvin, 1288.6600*kelvin, 1118.6700*kelvin, 899.625*kelvin};
        G4double innerFuelDensity=9.91*g/cm3;

        G4double outerFuelTemp[4]={1625.0500*kelvin, 1480.2500*kelvin, 1241.7850*kelvin, 945.8700*kelvin};
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

        G4double flowTubeTemp=659.1800*kelvin;
        G4double flowTubeDensity=7.9*g/cm3;

        G4double centralCoolantTemp=629.0100*kelvin;
        G4double centralCoolantDensity=0.6059*g/cm3;
};

#endif // SCWRCONSTRUCTOR_H
