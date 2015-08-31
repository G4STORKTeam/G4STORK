/*
StorkHPNeutronBuilder.hh

Created by:		Liam Russell
Date:			23-02-2012
Modified:       11-03-2013

Header file for StorkHPNeutronBuilderclass.

This class creates all of the physics processes, models, particles and data.
It takes a temperature and a bool flag. The temperature denotes the temperature
the cross sections were evaluated at, which determines whether doppler
broadening is used.

*/

#ifndef STORKHPNEUTRONBUILDER_H
#define STORKHPNEUTRONBUILDER_H

// Include header files
#include "G4VNeutronBuilder.hh"
#include "globals.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"

// Processes
#include "G4HadronElasticProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4HadronCaptureProcess.hh"
#include "G4HadronFissionProcess.hh"

// Models
#include "G4NeutronHPElastic.hh"
#include "G4NeutronHPInelastic.hh"
#include "G4NeutronHPCapture.hh"
#include "G4NeutronHPFission.hh"

// Data Sets
#include "StorkNeutronHPCSData.hh"

//
#include "G4NeutronHPCaptureData.hh"
#include "G4NeutronHPFissionData.hh"
#include "G4NeutronHPElasticData.hh"
#include "G4NeutronHPInelasticData.hh"
//

#ifdef G4USE_TOPC
#include "topc.h"
#include "G4HadronicProcessStore.hh"
#endif


class StorkHPNeutronBuilder : public G4VNeutronBuilder
{
    public:
        StorkHPNeutronBuilder(G4double aT);
        ~StorkHPNeutronBuilder();

        void Build(G4HadronElasticProcess *aP);
        void Build(G4HadronFissionProcess *aP);
        void Build(G4HadronCaptureProcess *aP);
        void Build(G4NeutronInelasticProcess *aP);

        void SetMinEnergy(G4double aM) { theIMin = theMin = aM; }
        void SetMaxEnergy(G4double aM) { theIMax = theMax = aM; }
        void SetMinInelasticEnergy(G4double aM) { theIMin = aM; }
        void SetMaxInelasticEnergy(G4double aM) { theIMax = aM; }

    private:

        // Applicability limits
        G4double theMin;
        G4double theMax;
        G4double theIMin;
        G4double theIMax;

        // User input and limits
        G4double temperature;

        // Models
        G4NeutronHPElastic *nElasticModel;
        G4NeutronHPInelastic *nInelasticModel;
        G4NeutronHPFission *nFissionModel;
        G4NeutronHPCapture *nCaptureModel;

        // Data
//        StorkNeutronHPCSData *theHPElasticData;
        G4NeutronHPElasticData *theHPElasticData;
 //       StorkNeutronHPCSData *theHPInelasticData;
        G4NeutronHPInelasticData *theHPInelasticData;
 //       StorkNeutronHPCSData *theHPFissionData;
        G4NeutronHPFissionData *theHPFissionData;
 //       StorkNeutronHPCSData *theHPCaptureData;
        G4NeutronHPCaptureData *theHPCaptureData;
};

#endif // STORKHPNEUTRONBUILDER_H
