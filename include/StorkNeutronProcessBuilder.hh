/*
StorkNeutronProcessBuilder.hh

Created by:		Liam Russell
Date:			23-02-2012
Modified:

Header file for StorkNeutronProcessBuilder class.

Based on G4NeutronBuilder, this builder class sets up all neutron physics
processes and models (fission, capture, elastic, inelastic, and step limiters).

*/

#ifndef STORKNEUTRONPROCESSBUILDER_H
#define STORKNEUTRONPROCESSBUILDER_H

// Include G4-STORK headers
#include "StorkHadronFissionProcess.hh"
#include "StorkHadronCaptureProcess.hh"
#include "StorkNeutronInelasticProcess.hh"
#include "StorkHadronElasticProcess.hh"
#include "StorkHPNeutronBuilder.hh"
#include "StorkTimeStepLimiter.hh"
#include "StorkPeriodicBCStepLimiter.hh"

// Include Geant4 headers
#include "G4VNeutronBuilder.hh"
//#include "G4LElastic.hh"
#include "G4DiffuseElastic.hh"
//#include "G4ChipsElasticModel.hh"
#include "G4LFission.hh"
//#include "QGSP_BERT_HP.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "globals.hh"

// Include other headers
#include <vector>
#include <typeinfo>


class StorkNeutronProcessBuilder
{
    public:
        // Public member functions

        StorkNeutronProcessBuilder(G4bool pBC = false);
        ~StorkNeutronProcessBuilder();

        // Build and register the models
        void Build();
        void RegisterMe(G4VNeutronBuilder * aB)
        {
            theModelCollections.push_back(aB);
        }


    private:
        // Private member variables

        StorkNeutronInelasticProcess * theNeutronInelastic;
        StorkHadronFissionProcess * theNeutronFission;
        StorkHadronCaptureProcess  * theNeutronCapture;
        StorkHadronElasticProcess * theNeutronElastic;
        StorkTimeStepLimiter * theStepLimiter;
        StorkPeriodicBCStepLimiter * thePeriodicBoundary;

        G4DiffuseElastic *theHighElasticModel;
        //G4ChipsElasticModel *theHighElasticModel;
        G4LFission *theHighFissionModel;

        std::vector<G4VNeutronBuilder *> theModelCollections;

        G4bool wasActivated;
        G4bool periodicBC;
};

#endif // STORKNEUTRONPROCESSBUILDER_H
