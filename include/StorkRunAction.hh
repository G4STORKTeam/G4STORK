/*
StorkRunAction.hh

Created by:		Liam Russell
Date:			22-06-2011
Modified:		11-03-2013

Header for StorkRunAction class.

This class is responsible for most of the analysis of the results. At the
beginning of a run, all of the variables and containers are reinitialized.
Additionally, the primary generator's survivors and delayed lists are updated.
During a run, each event is tallied: running totals are kept of important
quantities and the survivors and delayed neutron sources are stored in buffers.
At the end of a run, the buffers are collated into single lists of neutron
sources.  Also, the results for quantities such as k_run and k_eff are
calculated and printed to the output stream (screen or file).

*/


#ifndef STORKRUNACTION_H
#define STORKRUNACTION_H

// Include G4-STORK headers
#include "StorkPrimaryGeneratorAction.hh"
#include "StorkContainers.hh"
#include "StorkEventData.hh"
#include "StorkParseInput.hh"
#include "StorkMatPropManager.hh"
#include "StorkInterpManager.hh"

// Include Geant4 headers
#include "G4Timer.hh"
#include "G4Run.hh"
#include "G4HCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4DataVector.hh"
#include "G4UserRunAction.hh"

// Include other headers
#include <typeinfo>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <dirent.h>

// Class forward declaration
class StorkPrimaryGeneratorAction;


class StorkRunAction : public G4UserRunAction
{
    public:
        // Public member functions

        // Constructor and destructor
        StorkRunAction(StorkPrimaryGeneratorAction *genPtr,
                   const StorkParseInput *fIn);
        ~StorkRunAction();

        // Actions taken at the beginning and end of the current run
        void BeginOfRunAction(const G4Run *aRun);
        void EndOfRunAction(const G4Run *aRun);

        // Unpack and combine tally data from events
        void TallyEvent(const StorkEventData *eventData);
        void CollateNeutronSources();

        // Update the source distributions of StorkPrimaryGeneratorAction
        void UpdateSourceDistributions();
        // Update the value of the variable material-properties
        void UpdateWorldProperties(G4double *values);

        // Write the survivors and delayed neutrons of this run to a file
        void SaveSources(G4String fname, G4int numRuns, G4double runEnd);
        // Save fission data to a file
        G4bool WriteFissionData(G4String fname, G4double avgkrun, G4int start);

        G4bool DirectoryExists( const char* pzPath );

        // Set functions
        void SetWorldDimensions(G4ThreeVector worldDims)
        {
            worldSize = worldDims;
        }
        void SaveFissionData(G4bool yesNo = true) { saveFissionData = yesNo; }

        // Get functions
        G4double* GetRunResults() { return runResults; };
        G4double GetShannonEntropy() { return shannonEntropy[0]; };


    private:
        // Private member functions

        // Calculate the Shannon entropy of the current run
        G4double CalcShannonEntropy(G4int ***sites, G4int total);
        // Calculate the meshing index of a 3D position
        Index3D SpatialIndex(G4ThreeVector position) const;

    private:
        // Private member variables

        G4int kCalcType;

        G4double keff;              // k_eff for the current run
        G4double krun;              // k_run for the current run
        G4double runDuration;       // Duration of the run
        G4int numEvents;            // Number of events per run
        G4int totalFS;              // Total number of fission sites
        G4double avgLifetime;       // Average lifetime of lost neutrons
        G4int numNProduced;         // Number of neutrons produced
        G4int numDelayProd;         // Number of delayed neutrons produced
        G4int numNLost;             // Number of neutrons lost
        G4double runResults[8];     // Run results passed to run manager

        G4double tUnit;             // Time unit for output
        G4String rndmInitializer;   // File containing random seed (optional)
        G4bool saveFissionData;     // Flag for saving fission data
        G4int numFDCollectionRuns;  // Number of runs of collected fission data

        G4double shannonEntropy[2];     // Shannon entropy (fission sites,
                                        //                    survivors)
        G4double maxShannonEntropy;     // Maximum Shannon entropy
        G4int numSpatialSteps[3];       // Number of RS spatial steps in each
                                        // dimension (3D)
        G4ThreeVector worldSize;        // Size of world

        // Output stream (file or stdout)
        std::ostream *output;

        // Storage containers
        NeutronSources survivors;
        NeutronSources delayed;
        NeutronSources *survivorsBuffer;
        NeutronSources *delayedBuffer;
        G4int ***fSites;
        G4int ***sSites;
        DblVector fnEnergies;
        MSHSiteVector fnSites;

        // Pointers to other classes
        StorkPrimaryGeneratorAction *genAction;
        const StorkParseInput *infile;

        // Timer for the run
        G4Timer runTimer;

        // Variable material-property variables
        WorldPropertyMap worldPropMap;
        G4int* nameLen;
        const StorkInterpManager *theMPInterpMan;
        G4double* variableProps;

#ifdef G4TIMERA
        // Performance timer
        G4Timer runCalcTimer;
        G4double totalRunCalcTime;
#endif
};

#endif // STORKRUNACTION_H
