/*
StorkHPNeutronBuilder.cc

Created by:		Liam Russell
Date:			23-02-2012
Modified:       11-03-2013

Source code for StorkHPNeutronBuilder class.

*/


// Include header file
#include "StorkHPNeutronBuilder.hh"

// Constructor
StorkHPNeutronBuilder::StorkHPNeutronBuilder(G4double aT)
{
    // Set temperature for cross sections
	temperature = aT;

	// Initialize the applicability limits
	theMin = theIMin = 0.*MeV;
	theMax = theIMax = 20.*MeV;

    // Initialize data pointers
	theHPCaptureData = 0;
	theHPInelasticData = 0;
	theHPElasticData = 0;
	theHPFissionData = 0;
	nElasticModel = 0;
	nInelasticModel = 0;
	nFissionModel = 0;
	nCaptureModel = 0;

	// Insure that the slave processes do not output the physics process table
#ifdef G4USE_TOPC
	if(!TOPC_is_master())
	{
		G4HadronicProcessStore *theStore = G4HadronicProcessStore::Instance();
		theStore->SetVerbose(0);
	}
#endif
}

// Destructor
StorkHPNeutronBuilder::~StorkHPNeutronBuilder()
{
    // Delete the cross section data
    delete theHPCaptureData;
	delete theHPInelasticData;
	delete theHPFissionData;
	delete theHPElasticData;
}


// Build( Elastic Process )
// Build the elastic model and data.
// Set the limits of applicability for the model
void StorkHPNeutronBuilder::Build(G4HadronElasticProcess *aP)
{
    // Create the model and data
    if(nElasticModel==0) nElasticModel = new G4NeutronHPElastic();
    if(theHPElasticData==0)
        theHPElasticData = new G4NeutronHPElasticData();
//        theHPElasticData = new StorkNeutronHPCSData(maxOL,numIL,temperature,'E');

    // Set the limits of the model
    nElasticModel->SetMinEnergy(theMin);
    nElasticModel->SetMaxEnergy(theMax);

    // Register both
    aP->AddDataSet(theHPElasticData);
    aP->RegisterMe(nElasticModel);
}


// Build( Inelastic Process )
// Build the elastic model and data.
// Set the limits of applicability for the model
void StorkHPNeutronBuilder::Build(G4NeutronInelasticProcess *aP)
{
    // Create the model and data
    if(nInelasticModel==0) nInelasticModel = new G4NeutronHPInelastic();
    if(theHPInelasticData==0)
        theHPInelasticData = new G4NeutronHPInelasticData();
//        theHPInelasticData = new StorkNeutronHPCSData(maxOL,numIL,temperature,'I');

    // Set the limits of the model
    nInelasticModel->SetMinEnergy(theIMin);
    nInelasticModel->SetMaxEnergy(theIMax);

    // Register both
    aP->AddDataSet(theHPInelasticData);
    aP->RegisterMe(nInelasticModel);
}


// Build( Fission Process )
// Build the elastic model and data.
// Set the limits of applicability for the model
void StorkHPNeutronBuilder::Build(G4HadronFissionProcess *aP)
{
    // Create the model and data
    if(nFissionModel==0) nFissionModel = new G4NeutronHPFission();
    if(theHPFissionData==0)
        theHPFissionData = new G4NeutronHPFissionData();
//        theHPFissionData = new StorkNeutronHPCSData(maxOL,numIL,temperature,'F');

    // Set the limits of the model
    nFissionModel->SetMinEnergy(theMin);
    nFissionModel->SetMaxEnergy(theMax);

    // Register both
    aP->AddDataSet(theHPFissionData);
    aP->RegisterMe(nFissionModel);
}


// Build( Capture Process )
// Build the elastic model and data.
// Set the limits of applicability for the model
void StorkHPNeutronBuilder::Build(G4HadronCaptureProcess *aP)
{
    // Create the model and data
    if(nCaptureModel==0) nCaptureModel = new G4NeutronHPCapture();
    if(theHPCaptureData==0)
        theHPCaptureData = new G4NeutronHPCaptureData();
//        theHPCaptureData = new StorkNeutronHPCSData(maxOL,numIL,temperature,'C');

    // Set the limits of the model
    nCaptureModel->SetMinEnergy(theMin);
    nCaptureModel->SetMaxEnergy(theMax);

    // Register both
    aP->AddDataSet(theHPCaptureData);
    aP->RegisterMe(nCaptureModel);
}


