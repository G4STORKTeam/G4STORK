/*
StorkNeutronProcessBuilder.cc

Created by:		Liam Russell
Date:			23-02-2012
Modified:       11-03-2013

Source code for StorkNeutronProcessBuilder class.

*/


// Include header file
#include "StorkNeutronProcessBuilder.hh"


// Constructor
StorkNeutronProcessBuilder::StorkNeutronProcessBuilder(std::vector<G4int>* pBCVec, std::vector<G4int>* rBCVec)
:wasActivated(false)
{
    // Create the physics processes
    theNeutronElastic = new StorkHadronElasticProcess;
    theNeutronInelastic = new StorkNeutronInelasticProcess;
    theNeutronCapture = new StorkHadronCaptureProcess;
    theNeutronFission = new StorkHadronFissionProcess;
    theStepLimiter = new StorkTimeStepLimiter;
    TheUserBoundaryCond = new StorkUserBCStepLimiter(pBCVec,rBCVec);
    TheZeroBoundaryCond = new StorkZeroBCStepLimiter(pBCVec,rBCVec);
}


// Destructor
StorkNeutronProcessBuilder::~StorkNeutronProcessBuilder()
{
    // Delete the processes
    if(theNeutronElastic)
        delete theNeutronElastic;
    if(theNeutronInelastic)
        delete theNeutronInelastic;
    if(theNeutronCapture)
        delete theNeutronCapture;
    if(theNeutronFission)
        delete theNeutronFission;
    if(theStepLimiter)
        delete theStepLimiter;
    if(TheUserBoundaryCond)
        delete TheUserBoundaryCond;
    if(TheZeroBoundaryCond)
        delete TheZeroBoundaryCond;
    /*
    if(theHighElasticModel)
        delete theHighElasticModel;
    if(theHighFissionModel)
        delete theHighFissionModel;
    if(theHighCaptureModel)
        delete theHighCaptureModel;
        */

}


// Build()
// Build the data and models for the neutron processes
void StorkNeutronProcessBuilder::Build()
{
    wasActivated = true;
    StorkHPNeutronBuilder *aHPBuilder;

    // Build the models and data for the neutron processes (all energies)
    std::vector<G4VNeutronBuilder *>::iterator i;
    for(i=theModelCollections.begin(); i!=theModelCollections.end(); i++)
    {
        aHPBuilder = dynamic_cast<StorkHPNeutronBuilder*>(*i);

        // Only build low energy models for the elastic processes
        if(aHPBuilder)
            (*i)->Build(theNeutronElastic);

        (*i)->Build(theNeutronInelastic);
        (*i)->Build(theNeutronCapture);
        (*i)->Build(theNeutronFission);
    }

    // Create the high energy elastic model

    theHighElasticModel = new G4DiffuseElastic();
    //theHighElasticModel = new G4ChipsElasticModel();
    //theHighElasticModel = new G4HadronElastic();


    // Set the minimum energy limit
    theHighElasticModel->SetMinEnergy(20.0*MeV);
    theHighElasticModel->SetMaxEnergy(20000.*GeV);

    // Register model with elastic process
    theNeutronElastic->RegisterMe(theHighElasticModel);

    // Create the high energy fission model
    theHighFissionModel = new G4LFission();

    // Set the minimum energy limit
    theHighFissionModel->SetMinEnergy(20.0*MeV);
    theHighFissionModel->SetMaxEnergy(20000.*GeV);

    // Register model with fission process
    theNeutronFission->RegisterMe(theHighFissionModel);

    theHighCaptureModel = new G4NeutronRadCapture();

    // Set the minimum energy limit
    theHighCaptureModel->SetMinEnergy(20.0*MeV);
    theHighCaptureModel->SetMaxEnergy(20000.*GeV);


    // Register model with fission process
    theNeutronCapture->RegisterMe(theHighCaptureModel);


    // Add processes to the process manager
    G4ProcessManager * theProcMan = G4Neutron::Neutron()->GetProcessManager();

    theProcMan->AddDiscreteProcess(theNeutronElastic);
    theProcMan->AddDiscreteProcess(theNeutronInelastic);
    theProcMan->AddDiscreteProcess(theNeutronCapture);
    theProcMan->AddDiscreteProcess(theNeutronFission);
    theProcMan->AddDiscreteProcess(theStepLimiter);
    theProcMan->AddDiscreteProcess(TheUserBoundaryCond);
    theProcMan->AddDiscreteProcess(TheZeroBoundaryCond);
}



