/*
StorkVWorldConstructor.hh

Created by:		Liam Russell
Date:			23-05-2012
Modified:       11-03-2013

Header for StorkVWorldConstructor class.

This class is the base class used by all of the world constructor classes.
It contains some of the major functions common to all the world constructors.
The main purpose of this class is to simplify coding else where in the code
(using polymorphism) and to create a better defined hierarchy.
*/

#ifndef STORKWORLDCONSTRUCTOR_H
#define STORKWORLDCONSTRUCTOR_H

// Include G4-STORK headers
#include "StorkMatPropManager.hh"
#include "StorkContainers.hh"
#include "StorkParseInput.hh"
#include "StorkNeutronSD.hh"
#include "StorkMatPropManager.hh"

// Include Geant4 headers
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4Isotope.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4SolidStore.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VSensitiveDetector.hh"
#include "G4SDManager.hh"
#include "G4SDParticleFilter.hh"
#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"

// Include other headers
#include <map>


class StorkNeutronSD;

class StorkVWorldConstructor
{
    public:
        // Public member functions

		// Constructor and destructor
        StorkVWorldConstructor();
        virtual ~StorkVWorldConstructor();

		// Create and update simulation world
		virtual G4VPhysicalVolume* ConstructNewWorld(const StorkParseInput* infile);
        virtual G4VPhysicalVolume* UpdateWorld(StorkMatPropChangeVector changes);

		// Check whether material-property is variable for this world
        G4bool IsApplicable(MatPropPair matProp);

		// Get the current value of a material-property
        G4double GetWorldProperty(MatPropPair matProp);

        // Get dimensions of smallest box enclosing world
        virtual G4ThreeVector GetEncWorldDim() { return encWorldDim; }
        G4ThreeVector GetReactorDim() { return reactorDim; }

		// Get the reactor logical volume
        virtual G4LogicalVolume* GetWorldLogical() { return worldLogical; }


    protected:
        // Protected member functions

		// Construct and destroy simulation materials
		virtual G4VPhysicalVolume* ConstructWorld()=0;
		virtual void ConstructMaterials() = 0;
		void DestroyMaterials();
		// Update the variable material-properties
		G4bool UpdateWorldProperties(StorkMatPropChangeVector changes);

    protected:
        // Protected member variables

		// Maps materials and properties
		WorldPropertyMap variablePropMap;
        StorkMaterialMap matMap;
        StorkMatPropChangeVector initialChanges;
        StorkMatPropManager *theMPMan;

        // Sensitive Detector
        StorkNeutronSD *sDReactor;

        // Flags used in world update
        G4bool geomChanged;
        G4bool matChanged;

        // World variables
        G4ThreeVector encWorldDim;
        G4ThreeVector reactorDim;
        G4LogicalVolume *worldLogical;
        G4VPhysicalVolume *worldPhysical;
        G4VisAttributes *worldVisAtt;
};

#endif // STORKWORLDCONSTRUCTOR_H
