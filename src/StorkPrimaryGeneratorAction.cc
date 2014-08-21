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
    uniformDisWithDim = infile->GetUniformDistWithDim();
    shape = infile->GetUniformDistributionShape();
    uniDisDim = infile->GetUniformDistDim();
    origin = infile->GetInitialSourcePos();
    initialSource = false;
    primaryData = NULL;
    theFissionFS = NULL;
    normalize = infile->GetRenormalizeAfterRun();
    instantDelayed = infile->GetInstantDelayed();

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
void StorkPrimaryGeneratorAction::SetPrimaries(StorkPrimaryData *primaryData)
{
    // Assign the primaries to the current primaries vector
    currPrimaries = *(primaryData->primaries);
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
    // Reset delayed neutron primary counter to zero
    numDNPrimaries = 0;
    NeutronList::iterator it = dNeutrons.begin();
    NeutronList::iterator target;
    NeutronSources delayToBeAdded;
    G4double totalWeight = 0.;

    // Find the delayed neutrons that will be added in the current run
    while(!instantDelayed && it != dNeutrons.end())
    {
        if((*it).first < runEnd)
        {
            target = it;
            it++;
            delayToBeAdded.push_back(*target);
            dNeutrons.erase(target);
            numDNPrimaries++;
        }
        else
        {
            it++;
        }
    }

    // Comb delayed based on weights

    // Check if weight needs to be calculated and sum all weights
    for(G4int i=0; i < G4int(delayToBeAdded.size()); i++)
    {
        if(delayToBeAdded[i].ninth < 0)
        {
            // Get k_run product from run manager

        }

        // Sum weights
        totalWeight += 1.0 / delayToBeAdded[i].ninth;
    }

    // Comb



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
		dNeutrons.insert(dNeutrons.end(),dnSource->begin(),dnSource->end());
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
    if(initialSource) return;

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
    //CreateInitialDelayed();

    // Make sure the default source can't be called again
    initialSource = true;
}


// CreateInitialDelayed()
// Create an initial delayed neutron distribution using fission data from a
// previous simulation and the low energy neutron fission process,
// StorkHadronFissionProcess.
G4bool StorkPrimaryGeneratorAction::CreateInitialDelayed()
{
	// Local variables
	char line[256];
	G4int numEntries;
	G4double avgkrun;
	G4double avgNuD;
	G4double nuVariance = 0.05;
	G4int nuD;
	G4ThreeVector *fSites;
	G4double *fnEnergy;
	G4double currTime = 0.0;
	G4double runDuration;
	G4int dnProdFail = 0;
	G4int dnFailLimit = 25;
	G4int dnInd;
	G4bool prodDN = false;
	G4ThreeVector theMomDir;
	StorkNeutronData theDelayed;
	G4DynamicParticle *theNeutron = NULL;
	G4Material *theMat = NULL;
	G4Navigator *theNav = G4TransportationManager::GetTransportationManager()->
                                                    GetNavigatorForTracking();


	// The fission process
	StorkHadronFissionProcess *theFissionProcess =  NULL;
	theFissionProcess = dynamic_cast<StorkHadronFissionProcess*>(
                            StorkProcessManager::GetStorkProcessManagerPtr()->
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
	dnfile >> numEntries >> runDuration >> avgkrun >> avgNuD;

	// Assign memory for delayed neutron data
	fSites = new G4ThreeVector[numEntries];
	fnEnergy = new G4double[numEntries];

	// Read in delayed neutron data
	for(G4int i=0; i<numEntries && dnfile.good(); i++)
	{
		dnfile >> fSites[i][0] >> fSites[i][1] >> fSites[i][2] >> fnEnergy[i];
	}


	// Produce delayed neutrons until none are produced in positive time
	while(dnProdFail < dnFailLimit)
	{
		// Reset flags
		prodDN = false;

		// Determine the requisite number of delayed neutrons
		nuD = G4int(std::floor(avgNuD *
								(1.0 + nuVariance*G4UniformRand()) + 0.5));

		// Produce the requisite number of neutrons
		for(G4int i=0; i < nuD; i++)
		{
			// Randomly sample a position and incoming neutron energy
			dnInd = G4int(std::floor(G4UniformRand() *
										G4double(numEntries) + 0.5));

			// Create the incident neutron
			theMomDir.setRThetaPhi(1.0, G4UniformRand()*CLHEP::pi,
									G4UniformRand()*2.0*CLHEP::pi);
			theNeutron = new G4DynamicParticle(neutron, theMomDir,
												fnEnergy[dnInd]);
			theNeutron->SetProperTime(currTime);

			// Find the current material
			theMat = theNav->LocateGlobalPointAndSetup(fSites[dnInd])->
											GetLogicalVolume()->GetMaterial();

			// Use nuclear data to create a delayed neutron
			theDelayed = theFissionProcess->GetADelayedNeutron(theNeutron,
																theMat);

			// Set the position of the delayed neutron
			theDelayed.third = fSites[dnInd];

			// Correct the time of birth of the delayed neutron
//			theDelayed.first -= currTime;

			// Check if delayed neutron is viable
			if(theDelayed.first >= 0.0)
			{
				// Add to delayed neutron list
				dNeutrons.push_back(theDelayed);

				// Set flag
				prodDN = true;
			}

			// Delete dynamic variables
			delete theNeutron;

		}

		// Decrement the time
		currTime -= runDuration;

		// Increment the fail counter if algorithm failed to create any viable
		// delayed neutrons
		if(!prodDN)
			dnProdFail++;

	}



	// Delete dynamic variables
	delete [] fSites;
	delete [] fnEnergy;

    return true;
}


// UniformPosition()
// Generate uniformly distributed positions in the simulation geometry
void StorkPrimaryGeneratorAction::UniformPosition(StorkNeutronData* input)
{
    StorkWorld *worldPointerCD = runMan->GetWorld();

    if(shape == "Cell")
        shape = worldPointerCD->GetWorldLogicalVolume()
                                    ->GetDaughter(0)->GetLogicalVolume()
                                    ->GetSolid()->GetEntityType();

    G4double limit[3];
    G4ThreeVector extent = worldPointerCD->GetWorldDimensions();

    if(shape=="G4Orb")
    {
        if(uniformDisWithDim)
        {
                // Select a random position
            limit[0] = (G4UniformRand()*(uniDisDim[1]-uniDisDim[0])+uniDisDim[0])*CLHEP::cm;
            limit[1] = (G4UniformRand()*(uniDisDim[3]-uniDisDim[2])+uniDisDim[2])*CLHEP::pi;
            limit[2] = (G4UniformRand()*(uniDisDim[5]-uniDisDim[4])+uniDisDim[4])*CLHEP::pi;
        }
        else
        {
            // Select a random position
            limit[0] = G4UniformRand()*extent[0];
            limit[1] = G4UniformRand()*2.0*CLHEP::pi;
            limit[2] = G4UniformRand()*CLHEP::pi;
        }

        // Set position
        input->third.setRThetaPhi(limit[0],limit[2],limit[1]);
    }

    else if(shape=="G4Tubs")
    {
        if(uniformDisWithDim)
        {
            limit[0] = (G4UniformRand()*(uniDisDim[1]-uniDisDim[0])+uniDisDim[0])*CLHEP::cm;
            limit[1] = (G4UniformRand()*uniDisDim[2])*CLHEP::pi;
            limit[2] = (G4UniformRand()*uniDisDim[3]-0.5*uniDisDim[3])*CLHEP::cm;
            input->third.setRhoPhiZ(limit[0],limit[1],limit[2]);
            input->third.rotate((uniDisDim[4]*CLHEP::pi),(uniDisDim[5]*CLHEP::pi),0.);

        }
        else
        {
            // Select a random position
            limit[0] = G4UniformRand()*extent[1];
            limit[1] = G4UniformRand()*2.0*CLHEP::pi;
            limit[2] = G4UniformRand()*2*extent[2]-extent[2];
            input->third.setRhoPhiZ(limit[0],limit[1],limit[2]);
        }
    }

    else
    {
        if(uniformDisWithDim)
        {
            limit[0] = G4UniformRand()*uniDisDim[0]-0.5*uniDisDim[0];
            limit[1] = G4UniformRand()*uniDisDim[1]-0.5*uniDisDim[1];
            limit[2] = G4UniformRand()*uniDisDim[2]-0.5*uniDisDim[2];
            input->third.set(limit[0],limit[2],limit[1]);
            input->third.rotate((uniDisDim[3]*CLHEP::pi),(uniDisDim[4]*CLHEP::pi),0.);
        }
        else
        {
            limit[0] = G4UniformRand()*extent[0]*2-extent[0];
            limit[1] = G4UniformRand()*extent[1]*2-extent[1];
            limit[2] = G4UniformRand()*extent[2]*2-extent[2];
            input->third.set(limit[0],limit[2],limit[1]);
        }
    }
    input->third = input->third + origin;
}


