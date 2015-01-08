/*
StorkPrimaryGeneratorAction.cc

Created by:		Liam Russell
Date:			22-06-2011
Modified:		11-03-2013

Source code for StorkPrimaryGeneratorAction class.

*/


// Include header file

#include "StorkPrimaryGeneratorAction.hh"



// Constructor
StorkPrimaryGeneratorAction::StorkPrimaryGeneratorAction(
                                        const StorkParseInput* infile,
                                        G4bool master)
{
    // Set default and user input values
    runEnd = 0.;
    numPrimaries = infile->GetNumberOfPrimariesPerEvent();
    realNumPrimaries = numPrimaries;
    initEnergy = infile->GetInitialEnergy();
    uniformDis = infile->GetUniformDistribution();
    shape = infile->GetUniformDistributionShape();
    origin = infile->GetInitialSourcePos();
    initialSource = false;
    primaryData = NULL;
    normalize = infile->GetRenormalizeAfterRun();
    instantDelayed = infile->GetInstantDelayed();
    initialDelayed = false;
    theNav = G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();

    // Set initial source files (on master only)
    if(master)
    {
    	if(infile->LoadInitialSources())
			sourceFile = infile->GetInitialSourceFile();
		if(infile->LoadInitialDelayed())
			delayedSourceFile = infile->GetInitialDelayedFile();
    }

    // Find neutron definition
    G4ParticleTable* pTable = G4ParticleTable::GetParticleTable();
    neutron = pTable->FindParticle("neutron");

    // Set the particle type for the generator
    nGen.SetParticleType(neutron);

    // Set the pointer to the run manager
    runMan = dynamic_cast<StorkRunManager*>(G4RunManager::GetRunManager());


#ifdef G4TIMEPG
    totalGenTime = 0.0;
#endif
}


// Destructor
StorkPrimaryGeneratorAction::~StorkPrimaryGeneratorAction()
{
	if(primaryData) delete primaryData;

    if (fSites) delete [] fSites;

    if (fnEnergy) delete [] fnEnergy;
}


// GeneratePrimaries()
// The survivors for the current run need to be set and the number of survivors
// is assumed to be renormalized before this function is called.
void StorkPrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
#ifdef G4TIMEPG
    genTimer.Start();
#endif

    nGen.GeneratePrimaryVertices(anEvent,&currPrimaries);

#ifdef G4TIMEPG
    genTimer.Stop();
    totalGenTime += genTimer.GetRealElapsed();

    G4cout << "Primary Generator Timing: " << G4endl
           << "Total primary generator time = " << totalGenTime << "s"
           << G4endl;

    totalGenTime = 0.0;
#endif
}


// SetPrimaries()
// Set the primaries for the current event (i.e. set on slaves by master)
void StorkPrimaryGeneratorAction::SetPrimaries(StorkPrimaryData *pData)
{
    // Assign the primaries to the current primaries vector
    currPrimaries = *(pData->primaries);
}


// GetPrimaryData()
// Get primary data for the next event
StorkPrimaryData* StorkPrimaryGeneratorAction::GetPrimaryData(G4int /*eventNum*/)
{
    if(primaryIt == survivors.end()) return NULL;

    // Create a new primary data container and delete the old one if it exists
    if(primaryData) delete primaryData;
    primaryData = new StorkPrimaryData();

    // Create a new primaries vector
    primaryData->primaries->reserve(realNumPrimaries);

    // Insert the appropriate survivors into the primary data container
	for(G4int i=0; i<realNumPrimaries && primaryIt != survivors.end(); i++)
	{
		primaryData->primaries->push_back(*primaryIt);
		primaryIt++;
	}

    return primaryData;
}



// InitializeRun()
// Add delayed neutrons born in this run to survivors and renormalize survivors
// if necessary.
void StorkPrimaryGeneratorAction::InitializeRun()
{
#ifdef G4TIMEPG
    genTimer.Start();
#endif

    // Set up initial sources if needed
    if(!initialSource)
    {
        if(sourceFile.empty())
            InitialSource();
        else
            LoadSource(sourceFile);
    }

    // Get the end time for the run
    runEnd = runMan->GetRunEnd();

    // Check that some survivors exist
    if(survivors.size() == 0)
    {
        G4cerr << "***ERROR: No survivors (neutrons) for this run." << G4endl;
        return;
    }


    // Add delayed neutrons born in the current run to survivors
    AddCurrentDelayed();

    // Check to see whether the size of the survivor distribution is correct
    G4int missing = numPrimaries*numEvents - G4int(survivors.size());

    if(normalize && missing != 0)
    {
        // Renormalize survivors
        RenormalizeSurvivors(missing);

        // Check that renormalized survivors have correct number of elements
        if(G4int(survivors.size()) !=  numPrimaries*numEvents)
        {
            G4cerr << "***Error: Suvivors not properly normalized." << G4endl;
        }

    }

    // Calculate the actual number of primaries per event
    realNumPrimaries = G4int(std::ceil(G4double(survivors.size())
                                           /G4double(numEvents)));

#ifdef G4TIMEPG
    genTimer.Stop();
    totalGenTime += genTimer.GetRealElapsed();
#endif

	primaryIt = survivors.begin();

    return;
}


// RenormalizeSurvivors()
// Delete/duplicate survivors until the total number of survivors equals
// the number of primaries for each run.  The process is randomized using the
// modified Fisher-Yates shuffle algorithm
void StorkPrimaryGeneratorAction::RenormalizeSurvivors(G4int numMissing)
{
     // Do nothing if no renormalization is needed.
    if(numMissing == 0) return;


    // Local variables
    G4int numSurvivors = G4int(survivors.size());

    // Copy the unrenormalized survivors to a new vector
    NeutronSources originalSurvivors = survivors;

    // Clear original vector and set to proper size
    survivors.clear();
    survivors.reserve(numPrimaries*numEvents);


    // If numMissing is positive duplicate survivors
    if(numMissing > 0)
    {
        // Add original survivors to number of missing
        numMissing += numSurvivors;

        // Duplicate all survivors until the number of missing is less than the
        // ORIGINAL number of survivors
        while(numMissing > numSurvivors)
        {
            survivors.insert(survivors.end(),originalSurvivors.begin(),
                             originalSurvivors.end());
            numMissing -= numSurvivors;
        }

        // Check that there are still missing survivors
        if(numMissing == 0) return;
    }
    // Otherwise set the number of survivors to add as the total number of
    // survivors per run
    else
    {
        numMissing = numPrimaries*numEvents;
    }


    // Extract numMissing survivors from the originals either from the
    // shuffled or unshuffled depending on which involves the least shuffling

    // If number of missing is less than half of survivors, add shuffled
    if(G4double(numMissing) < 0.5*G4double(numSurvivors))
    {
        ShuffleSurvivors(numMissing,&originalSurvivors);

        // Add shuffled to survivors
        survivors.insert(survivors.end(),originalSurvivors.begin(),
                         originalSurvivors.begin()+numMissing);
    }
    // Otherwise add unshuffled
    else
    {
        ShuffleSurvivors(numSurvivors-numMissing,&originalSurvivors);

        // Add unshuffled to survivors
        survivors.insert(survivors.end(),originalSurvivors.end()-numMissing,
                         originalSurvivors.end());
    }


    return;

}


// ShuffleSurvivors()
// Shuffle the first 'numShuffle' elements of a neutron source vector.
void StorkPrimaryGeneratorAction::ShuffleSurvivors(G4int numShuffle,
                                                NeutronSources *origSurvivors)
{
    // Local variables
    StorkNeutronData tempData;
    G4int totalSurvivors = G4int(origSurvivors->size());
    G4int randIndex = totalSurvivors;

    // Make sure numMissing is positive
    numShuffle = std::abs(numShuffle);

    // Shuffle numMissing random survivors to the start of the vector
    for(G4int i = 0; i < numShuffle ; i++)
    {
        randIndex = G4int(G4UniformRand()*(totalSurvivors - i)) + i;
        tempData = survivors[randIndex];
        survivors[randIndex] = survivors[i];
        survivors[i] = tempData;
    }

    return;
}


// AddCurrentDelayed()
// Add delayed neutrons in the current run to the survivors.  Renormalize the
// delayed neutrons relative to the k_run values of intervening runs.
void StorkPrimaryGeneratorAction::AddCurrentDelayed()
{
    //If a fission file was provided, generate delayed neutrons.	
    if(initialDelayed) GenerateDelayedNeutrons();
    // Reset delayed neutron primary counter to zero
    numDNPrimaries = 0;
    NeutronList::iterator it = dNeutrons.begin();
    NeutronList::iterator target;
    NeutronSources delayToBeAdded;
    G4double totalWeight = 0.;


    // Find the delayed neutrons that will be added in the current run
    while(!instantDelayed && it != dNeutrons.end())
    {

        target = it;
        it++;
        delayToBeAdded.push_back(*target);
        dNeutrons.erase(target);
        numDNPrimaries++;

    }

    // Add delayed to survivors
    survivors.insert(survivors.end(),delayToBeAdded.begin(),
                     delayToBeAdded.end());

    return;
}


// UpdateSourceDistributions()
// Rewrite the survivors distribution with the survivors from the current run
// and add the delayed neutrons to the current list
void StorkPrimaryGeneratorAction::UpdateSourceDistributions(
                                                const NeutronSources *nSource,
                                                const NeutronSources *dnSource)
{
#ifdef G4TIMEPG
    genTimer.Start();
#endif

    // Assign the new distribution to survivors
    survivors.assign(nSource->begin(),nSource->end());

	// Only add the delayed neutrons after the source has converged
	if(runMan->GetSourceConvergence())
	{
		// Add the delayed neutrons to the end of the delayed neutron list
		//dNeutrons.insert(dNeutrons.end(),dnSource->begin(),dnSource->end());
	}

#ifdef G4TIMEPG
    genTimer.Stop();
    totalGenTime += genTimer.GetRealElapsed();
#endif
}


// GetNumPrimaries()
// Returns current size of survivors. If run has started and the survivors
// vector is empty, use the expected number of survivors
G4int StorkPrimaryGeneratorAction::GetNumPrimaries()
{
    if(survivors.empty() && !runMan->GetCurrentRun()->GetRunID())
        return numEvents * numPrimaries;
    else
        return G4int(survivors.size());
}


// GetNumDNPrimaries()
// This is called AFTER the run occurs
G4int StorkPrimaryGeneratorAction::GetNumDNPrimaries()
{
	G4int count = 0;
	G4double runStart = runMan->GetRunStart();
	NeutronSources::iterator it = survivors.begin();

	if(it->first > runStart)
		count++;

	return count;
}


// LoadSource()
// Loads an initial source distribution from file (csv text file)
// Assume that the input file is properly formatted and the values are correct
// FILE FORMAT: each on a new line(s)
// a) header lines (start with #)
// b) current time of records
// c) number of survivors
// d) survivors ...
// e) number of delayed
// f) delayed ...
void StorkPrimaryGeneratorAction::LoadSource(G4String fname)
{
    if(initialSource){
        return;
    }
    // Variables
    std::ifstream infile;
    char buffer[256];
    StorkNeutronData input;
    G4double recordTime;
    G4int numRecords;
    G4String line;
    std::stringstream conv;
    G4int lineNum = 0;

    infile.open(fname.c_str());

    // Ignore header lines
    while(infile.peek() == '#' || lineNum < 5)
    {
    	infile.getline(buffer,256);
    	lineNum++;
    }


    infile >> recordTime >> numRecords;


    // Read in each line of file
    // Format is: global time, local time, x, y, z, px, py, pz

    // Read in the survivors
    for(G4int i=0; i<numRecords; i++)
    {
        // Extract the data from the file and store it in input
        infile >> input.first >> input.second
               >> input.third[0] >> input.third[1] >> input.third[2]
               >> input.fourth[0] >> input.fourth[1] >> input.fourth[2]
               >> input.fifth >> input.sixth >> input.seventh >> input.eigth
               >> input.ninth;

        // Correct for the current time of the records
        input.first -= recordTime;

        // Write survivor
        survivors.push_back(input);
    }


    // Reserve memory for the delayed vector
    infile >> numRecords;

    if(!initialDelayed){
        // Read in delayed
        for(G4int i=0; i<numRecords; i++)
        {
            // Extract the data from the file and store it in input
            infile >> input.first >> input.second
                   >> input.third[0] >> input.third[1] >> input.third[2]
                   >> input.fourth[0] >> input.fourth[1] >> input.fourth[2]
                   >> input.fifth >> input.sixth >> input.seventh >> input.eigth
                   >> input.ninth;
            // Correct for the current time of the records
            input.first -= recordTime;

            dNeutrons.push_back(input);
        }
    }
    infile.close();

    initialSource = true;
}


// InitialSource()
// Default initial source if no source file is provided.
// Initial neutron energy is 14 MeV
void StorkPrimaryGeneratorAction::InitialSource()
{
    if(initialSource) return;


    G4double mass = neutron->GetPDGMass();
    G4double meanEng = initEnergy;
	G4double stdEng = 0.25*meanEng;
    G4double phi;
    G4double theta;
    G4double rEng;
    G4double rMom;

    // Build neutron data common values (position,time,lifetime,eta(-1.0))
    StorkNeutronData input(0.,0.,origin,G4ThreeVector(0.,0.,0.));

    for(G4int i=0; i<numPrimaries*numEvents; i++)
    {

        if(uniformDis)
        {
            UniformPosition(&input);
        }

        // Select a random diretion
        phi = G4UniformRand()*2.0*CLHEP::pi;
        theta = G4UniformRand()*CLHEP::pi;

        // Select a random energy (Gaussian distribution)
        rEng = G4RandGauss::shoot(meanEng,stdEng) + mass;
		rMom = std::sqrt(rEng*rEng - mass*mass);

        // Set momentum
        input.fourth.setRThetaPhi(rMom,theta,phi);

        survivors.push_back(input);
    }

    // Create the delayed neutrons
    initialDelayed = CreateInitialPrecursors();

    // Make sure the default source can't be called again
    initialSource = true;
}

// CreateInitialPrecursors()
// Create an initial precursor population using fission data from previous simulation.
G4bool StorkPrimaryGeneratorAction::CreateInitialPrecursors()
{
	// Local variables
	char line[256];
	G4double avgkrun;
	G4int totalPrimaries;
    G4double FissionPerPrimary;
    std::vector<G4double> Ci(6);
    G4int index;



	// The fission process
	theFissionProcess = dynamic_cast<StorkHadronFissionProcess*>(StorkProcessManager::GetStorkProcessManagerPtr()->
                                                                 GetProcess("StorkHadronFission"));


	// Load data from file
	std::ifstream dnfile(delayedSourceFile);

	// Check if file opened properly
	if(!dnfile.good())
	{
		G4cerr << "*** WARNING:  Unable to open initial delayed neutron file:"
        << delayedSourceFile << G4endl;
		return false;
	}

	// Skip header lines
	while(dnfile.peek() == '#')
		dnfile.getline(line,256);

	// Read in delayed neutron distribution parameters
	dnfile >> numEntries >> runDuration >> avgkrun >> totalPrimaries;

	// Assign memory for delayed neutron data
	fSites = new G4ThreeVector[numEntries];
	fnEnergy = new G4double[numEntries];

	// Read in delayed neutron data
	for(G4int i=0; i<numEntries && dnfile.good(); i++)
	{
		dnfile >> fSites[i][0] >> fSites[i][1] >> fSites[i][2] >> fnEnergy[i];

	}

    FissionPerPrimary = G4double(numEntries) / G4double(totalPrimaries);

    for(G4int i=0; i<numEntries; i++){

    /* // For now assume that U235 is the only fuel, uncomment or edit if you want to factor in more types of fuel.
       //Can also edit the StorkDelayedData.hh to change delay constants and fission yields.
        G4String iso = theFissionProcess->GetFissionIso(fnEnergy[i],fSites[i]).GetIsotope()->GetName();

        if(iso == "U235"){
            if(fnEnergy[i]<0.0000005) index = 0;
            else index = 3;

        }
        else if(iso == "U238"){
            index = 6;

        }
        else if(iso == "U233"){
            if(fnEnergy[i]<0.0000005) index = 2;
            else index = 5;

        }
        else if(iso == "Pu239"){
            if(fnEnergy[i]<0.0000005) index = 1;
            else index = 4;
        }
        else if(iso == "Pu240"){
            index = 7;
        }
        else if(iso == "Th232"){
            index = 8;
        }
        else {
            index = 0;
        }
        */

        index = 0;

        for(G4int i_=0;i_<6;i_++){

            Ci[i_] += ( 0.5 + FissionYields[index][i_] / ( pow(10,-9)*runDuration*DecayConstants[index][i_]+ log(avgkrun) )
                                                     );
         }


    }

    for (G4int i__ = 0; i__<6; i__++)
        Precursors[i__] = (G4int) std::floor(FissionPerPrimary*Ci[i__] +0.5);


    return true;
}


//Generate number of Delayed Neutrons based on the Precursor groups and decay constants.
void StorkPrimaryGeneratorAction::GenerateDelayedNeutrons(){
    G4int nuD = 0;
    G4int dnInd;
    G4ThreeVector theMomDir;
	StorkNeutronData theDelayed;
	G4DynamicParticle *theNeutron = NULL;
	G4Material *theMat = NULL;

    theNav->ResetStackAndState();


    for(G4int i_=0; i_<6; i_++){

        for(G4int i__ = 0; i__< Precursors[i_] ; i__++){
            G4double r = G4UniformRand();
            G4double probability = exp(-runDuration*pow(10,-9)*DecayConstants[0][i_]);
            if (r>probability){
                nuD++;
                //Precursors[i_]--;
            }

        }
    }



    for(G4int i=0; i < nuD; i++)
    {
        // Randomly sample a position and incoming neutron energy
        dnInd = G4int(std::floor(G4UniformRand() *
                                 numEntries + 0.5));

        // Create the incident neutron
        theMomDir.setRThetaPhi(1.0, G4UniformRand()*CLHEP::pi,
                               G4UniformRand()*2.0*CLHEP::pi);
        theNeutron = new G4DynamicParticle(neutron, theMomDir,
                                           fnEnergy[dnInd]);
        theNeutron->SetProperTime(0.0);

        // Find the current material
        theMat = theNav->LocateGlobalPointAndSetup(fSites[dnInd])->
        GetLogicalVolume()->GetMaterial();



        // Use nuclear data to create a delayed neutron
        theDelayed = theFissionProcess->GetADelayedNeutron(theNeutron,
                                                               theMat);

        // Set the position of the delayed neutron
        theDelayed.third = fSites[dnInd];

        // Set global time
        theDelayed.first = runEnd+runDuration*G4UniformRand();


        // Add to delayed neutron list
        dNeutrons.push_back(theDelayed);


        delete theNeutron;
        theNeutron = NULL;

    }
}



// UniformPosition()
// Generate uniformly distributed positions in the simulation geometry
void StorkPrimaryGeneratorAction::UniformPosition(StorkNeutronData* input)
{
    StorkWorld *worldPointerCD = runMan->GetWorld();

    // Determining the shape of the world volume
    // By default we want to generate random
    // points everywhere inside the world voulme
    if(shape == "Cell")
        shape = worldPointerCD->GetWorldLogicalVolume()
                                    ->GetDaughter(0)->GetLogicalVolume()
                                    ->GetSolid()->GetEntityType();

    G4ThreeVector extent = worldPointerCD->GetWorldDimensions();

    G4double limit[3];

    if(shape=="G4Orb")
    {
        // Select a random position
        limit[0] = G4UniformRand()*extent[0];
        limit[1] = G4UniformRand()*2.0*CLHEP::pi;
        limit[2] = G4UniformRand()*CLHEP::pi;

        // Set position
        input->third.setRThetaPhi(limit[0],limit[2],limit[1]);
    }

    else if(shape=="G4Tubs")
    {
        // Select a random position
        limit[0] = G4UniformRand()*extent[1];
        limit[1] = G4UniformRand()*2.0*CLHEP::pi;
        limit[2] = G4UniformRand()*extent[2]*2-extent[2];

        // Set position
        input->third.setRhoPhiZ(limit[0],limit[1],limit[2]);
    }

    else
    {
        limit[0] = G4UniformRand()*extent[0]*2-extent[0];
        limit[1] = G4UniformRand()*extent[1]*2-extent[1];
        limit[2] = G4UniformRand()*extent[2]*2-extent[2];

        // Set position
        input->third.set(limit[0],limit[2],limit[1]);
    }
    input->third = input->third + origin;
}


