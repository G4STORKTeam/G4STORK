/*
StorkNeutronSD.hh

Created by:		Liam Russell
Date:			22-06-2011
Modified:		11-03-2013

Header file for the StorkNeutronSD class.

The StorkNeutronSD class is used to tally all the important hits and to preserve
any surviving or delayed neutrons.  It needs to be used for any region that
may produce or consume neutrons, or any regions where survivors have an
opportunity to enter back into multiplying regions.

Much of the simulation control that allows for reactor simulations occurs in
the ProcessHits() function of the class.

*/

#ifndef STORKNEUTRONSD_H
#define STORKNEUTRONSD_H

// Include G4-STORK headers
#include "StorkTallyHit.hh"
#include "StorkRunManager.hh"
#include "StorkPrimaryNeutronInfo.hh"
#include "StorkContainers.hh"
#include "StorkTrackInfo.hh"
#include "StorkProcessManager.hh"

// Include Geant4 headers
#include "G4Timer.hh"
#include "G4VSensitiveDetector.hh"
#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4VProcess.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4SteppingControl.hh"
#include "G4ThreeVector.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"

// Class forward declarations
class StorkRunManager;


class StorkNeutronSD : public G4VSensitiveDetector
{
    public:
        // Public member functions

        // Constructor and destructor
        StorkNeutronSD(G4String name, G4bool instD = false);
        ~StorkNeutronSD() {;}

        void Initialize(G4HCofThisEvent *HCE);
        G4bool ProcessHits(G4Step *aStep,G4TouchableHistory *ROhist);
        void EndOfEvent(G4HCofThisEvent *HCE);

        // Save the surviors and delayed neutron (for future runs)
        void SaveSurvivors(const G4Track *aTrack);
        void SaveDelayed(const G4Track *sTrack);


    private:
        // Private member functions

        // Print totals for each reaction type (fission, capture, etc)
		void PrintCounterTotals() const;


    private:
        // Private member variables

        // Manager pointers
        StorkRunManager *runMan;
        StorkProcessManager *procMan;


        TallyHC *tallyHitColl;      // Collection of tally hits
        G4int HCID1;                // Index of the hit collection
        G4bool periodicBC;          // Periodic boundary flag
        G4bool instantDelayed;      // Flag to make delayed neutrons prompt
        G4bool logDelayedSpectrum;  // Flag to saved delayed neutron spectrum
        G4double runEnd;            // Time at the end of the run

        G4double totalLifetime;     // Total lifetime of lost neutrons
        G4int nLoss;                // Total number of lost neutrons
        G4int nProd;                // Total number of prompt neutrons produced
        G4int dProd;                // Total number of delayed neutrons produced

		DblVector fnEnergy;         // Energies of incident neutron in fissions
        SiteVector fSites;          // Sites where fission occurs
        NeutronSources survivors;   // Saved survivors
        NeutronSources delayed;     // Saved delayed neutrons


		// DIAGNOSTIC COUNTERS
        G4int nCap;
        G4int nEsc;
        G4int nFiss;
        G4int nInel;

#ifdef G4TIMESD
        // Performance timing variables
        G4Timer sdTimer;
        G4Timer phTimer;
        G4int cycles;
        G4double cycleTime;
        G4double totalSDTime;
#endif
};

#endif // STORKNEUTRONSD_H
