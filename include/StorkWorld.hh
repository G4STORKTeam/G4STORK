/*
StorkWorld.hh

Created by:		Liam Russell
Date:			17-02-2011
Modified:		11-03-2013

Header for StorkWorld class.

This class creates the simulation geometry and materials based on the input
file. The geometry is a sphere of various diameters and the materials are
either solid U235 or a homogeneous mixture of natural uranium and heavy water.
Additionally, it sets the sensisitive detector.

*/

#ifndef STORKWORLD_H
#define STORKWORLD_H

// Include G4-STORK headers
#include "StorkParseInput.hh"
#include "StorkMatPropManager.hh"

// Include Geant4 headers
#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"


// Type definition for world constructor map
class StorkVWorldConstructor;
typedef std::map<G4String,StorkVWorldConstructor*> StorkWorldMap;


class StorkWorld : public G4VUserDetectorConstruction
{
    public:
		// Public member functions

        // Constructor and destructor
        StorkWorld();
        StorkWorld(const StorkParseInput* infile);
        ~StorkWorld();

        void InitializeWorldData(G4String worlnam);
        void InitializeWorldData(const StorkParseInput* infile);

        // Construct the world (called when simulation begins)
        G4VPhysicalVolume* Construct();

        // Dump the geometrical tree to G4cout
        void DumpGeometricalTree();

        // Update the world based on a vector of changes
        G4VPhysicalVolume* UpdateWorld(StorkMatPropChangeVector theChanges);

		// Get the dimensions of the smallest box enclosing the world
        G4ThreeVector GetWorldBoxDimensions();

		// Get the current value of a world property
        G4double GetWorldProperty(MatPropPair matProp);

		// Get the logical volume of the world
        G4LogicalVolume* GetWorldLogicalVolume();

        // Get major world dimensions
        G4ThreeVector GetWorldDimensions();

        // Add world type
        void AddWorld(G4String name, StorkVWorldConstructor *aNewWorld);


    private:
        // Private member functions

        void DumpGeometricalTree(G4VPhysicalVolume *vol, G4int depth=0);

    private:
        // Private member variables

		// Pointer to input file
        const StorkParseInput* inFile;

        // Physical Volume
        G4VPhysicalVolume *worldPhysical;

        // World constructor pointer
        StorkVWorldConstructor *theWorld;

        // Map of available worlds
        StorkWorldMap availableWorlds;

        // Dimensions of smallest box enclosing world
        G4String worldName;
};

#endif // STORKWORLD_H
