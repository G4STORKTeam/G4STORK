/*
StorkParseInput.cc

Created by:		Liam Russell
Date:			July 13, 2011
Modified:		July 09, 2012

Source code file for StorkParseInput class.

*/


// Include header file

#include "StorkParseInput.hh"

StorkParseInput::StorkParseInput(G4bool master)
: isMaster(master)
{
	// Default values
	worldName = "Unknown";
	reactorMat = 0;
	csDirName = "DEFAULT";

	numSteps[0]=20;
	numSteps[1]=20;
    numSteps[2]=20;

	initEnergy = 1.*MeV;
	initialSourcePos.set(0.,0.,0.);
	randomSeed = 0;
	saveSources = 0;
	numberOfEvents =0;
	numberOfPrimariesPerEvent=0;
	numberOfRuns=0;
	limitSE = 2.0;
	convRuns = 25;

	// Set default flags
	loadSources=false;
	logData=false;
	instantDelayed=false;
	normalize = true;
	uniformDis = false;
	uniformDisWithDim = false;
	interpStart = false;
	saveFissionData = false;
	overrideInitRandomSeed = false;

	// Set null file names
	logFile = "";
	sourceFile = "";
	initialSourceFile = "";

	// Set default output stream (G4cout)
	logStream = (std::ofstream*)&G4cout;

	// Get a pointer to the material/property manager
	theMPMan = StorkMatPropManager::GetStorkMatPropManager();
}


StorkParseInput::~StorkParseInput()
{
	if(logData)
	{
		logStream->close();
		delete logStream;
		logStream = NULL;
	}
	if(theMPMan)
        delete theMPMan;
}

G4bool StorkParseInput::FinalizeInputs()
{
    //Check to make sure necessary input data has been given

    //initialize local variables
    G4int pos;

	// Check whether world was set properly
    if(worldName == "Unknown")
    {
        G4cerr << "Missing world type from input file \n";
        return false;
    }
	// Check whether other mandatory data was set
    else if(numberOfEvents==0 || numberOfPrimariesPerEvent==0 ||
            numberOfRuns==0 || runDuration==0)
    {
        G4cerr << "Missing simulation data from input file \n";
        return false;
    }

    // Set the saveSources interval to total number of runs if not set by user
    if(saveSources < 0)
    {
        saveSources=numberOfRuns;
    }
    // Reset interpStart flag if no interpolations set
    if(interpStart && theMPInterpMan.GetNumberOfInterpVectors() == 0)
    {
    	interpStart = false;
    }
    // Set the random seed if not set by the input file
    if(isMaster && randomSeed <= 0)
    {
    	srand(time(NULL));
		randomSeed = rand();
    }

    // Open the logging file (if necessary)
    if(isMaster && logData)
    {
    	logStream = new std::ofstream(logFile, std::ios_base::app);
    }

    // Set other file names if not given using logging file name
    if(logData)
    {
    	if(saveSources && sourceFile == "")
    	{
    		pos = logFile.find_last_of("/");

    		sourceFile = logFile.substr(0,pos) + "/Src-" +
							logFile.substr(pos+1,logFile.length()-1);
    	}


		if(saveFissionData && fissionDataFile == "")
		{
    		pos = logFile.find_last_of("/");

    		fissionDataFile = logFile.substr(0,pos) + "/Fission-" +
								logFile.substr(pos+1,logFile.length()-1);
    	}
    }


    // Set the temperature of the neutron cross section data
	if(!getenv("G4NEUTRONHPDATA"))
	{
	    G4cerr << "Environment variable for Neutron HP data not set." << G4endl;
	    return false;
	}

	datapath = getenv("G4NEUTRONHPDATA");

	// Find the folder name for the cross sections from the data path
    pos = datapath.find_last_of("/");

	// Check to make sure "/" is not the last character. If it is, erase it and
    // find the second to last slash
    while(pos == (G4int)(datapath.length()) - 1)
    {
        datapath.erase(pos,1);
        pos = datapath.find_last_of("/");
    }

	// Check where the dataPath points to a G4NDL library (0 K by default)
	if(datapath.find("G4NDL") != G4String::npos)
	{
	    xsTemp = 0.0*kelvin;
	}
	else
	{
	    // Find the start of the temperature
	    pos = datapath.find("_T");

	    // Determine xsTemp from datapath
	    xsTemp = atof((datapath.substr(pos+2)).c_str());

	    // Correct for the decimal in room temperature
	    if(xsTemp == 293.*kelvin)
            xsTemp = 293.6*kelvin;
	}

    // Set the units of input data
	runDuration *= ns;
	initEnergy *= MeV;
    // Create the inital properties vector from the world property map
	SetInitialPropertiesVector();

	return ErrorChecking();
}

void StorkParseInput::SetRequiredInputs(G4String worldNam, G4int numRuns, G4int numEvents, G4int numPrimariesPerEvent, G4double runDur)
{
    SetWorld(worldNam);
    numberOfRuns=numRuns;
    numberOfEvents=numEvents;
    numberOfPrimariesPerEvent=numPrimariesPerEvent;
    runDuration=runDur;
}

void StorkParseInput::SetWorld(G4String worldNam)
{
    if(worldNam=="Sphere")
        {
            reactorMat=92235;
            theWorldProps[MatPropPair(all,dimension)] = 8.7*cm;
            theWorldProps[MatPropPair(all,temperature)] = 293.6*kelvin;
        }
        else if(worldNam=="C6Lattice")
        {
            reactorMat=6;
            for(int i=0; i<6; i++)
            {
                periodicBC.push_back(i);
            }
            theWorldProps[MatPropPair(all,dimension)] = 28.575*cm;
        }
        else if(worldNam=="Cube")
        {
            reactorMat=8;
            for(int i=0; i<6; i++)
            {
                periodicBC.push_back(i);
            }
            uniformDis = true;

            theWorldProps[MatPropPair(all,dimension)] = 50.*cm;
            theWorldProps[MatPropPair(fuel,concentration)] = 0.7204*perCent;
            theWorldProps[MatPropPair(moderator,concentration)] = 90.0*perCent;
        }
        else if(userWorlds.find(worldNam) != userWorlds.end())
        {
            StorkMatPropChangeVector defaults = userWorlds[worldNam];

            for(G4int i=0; i < G4int(defaults.size()); i++)
            {
                theWorldProps[defaults[i].GetMatPropPair()] = defaults[i].change;
            }
        }
        else
        {
            worldName = "Unknown";
        }
}

void StorkParseInput::SetReactorMaterial(G4int reactorMaterial)
{
    if ((worldName=="Sphere")||(worldName=="Cube"))
    reactorMat=reactorMaterial;
    else
    G4cout<<"Cannot select material for this world";
 }

//Set source distribution and save options
void StorkParseInput::SetRandomSeed(G4long randSeed)
{
    randomSeed=randSeed;
    overrideInitRandomSeed=true;
}

void StorkParseInput::SetResultsFile(G4String logFil)
{
    logFile=logFil;
    logData=true;
}

void StorkParseInput::SetInitialSourceFile(G4String SourceFile)
{
    initialSourceFile=SourceFile;
    loadSources=true;
}

void StorkParseInput::SetInitialDelayedFile(G4String DelayedFile)
{
    initialDelayedFile=DelayedFile;
    loadDelayed=true;
}

void StorkParseInput::SetFissionDataFile(G4String fissionFile)
{
    fissionDataFile=fissionFile;
    saveFissionData=true;
}

void StorkParseInput::SetNSInterpolationManager(G4String mat, G4String prop, G4String interpDataFile)
{
    // Parse the strings
    MatPropPair mpPair (theMPMan->ParseMatEnum(mat), theMPMan->ParsePropEnum(prop));
    G4String mpString = PrintMatPropPair(mpPair);

    // Store the data file name
    std::pair<G4String,G4String> dataFile ("# " + mpString, interpDataFile);
    theDataProfiles.push_back(dataFile);

    // Create a new interpolation vector from the data file
    theMPInterpMan.CreateInterpVector(mpString.strip(':'),
                                dataFile.second, mpPair,
                                theMPMan->GetUnits(mpPair.second));
}

void StorkParseInput::SetIntialWorldProperties(G4String mat, G4String prop, G4double newValue)
{
    // Parse the strings
    MatPropPair mpPair (theMPMan->ParseMatEnum(mat), theMPMan->ParsePropEnum(prop));

    // Add the property to the world property map
    theWorldProps[mpPair] = newValue*theMPMan->GetUnits(mpPair.second);
}

// ReadInputFile()
// Reads the input file and sets the material temperature based on the
// loaction of the input data.
G4bool StorkParseInput::ReadInputFile(G4String filename)
{
	std::ifstream infile(filename.c_str(),std::ifstream::in);

    // Data file identifier and name
	std::pair<G4String,G4String> dataFile;
	MatPropPair mpPair;
	G4String mat, prop, mpString;
	G4double newValue;
	G4int pos;

	if(!infile.good())
		return false;

	char line[256];
	G4String keyWord;

    //Goes through input file and extracts parameters need to initialize the simualtion
    while(infile.good())
    {
    	// Skip empty or comment lines
        while(infile.peek()=='#' || infile.peek()=='\n')
        {
            infile.getline(line,256);
        }

		// Get keyword
		infile >> std::ws >> keyWord;

		// Check if keyword is empty (if so skip line)
		if(keyWord=="")
		{
			infile.getline(line,256);
		}

		// World selection
		else if(keyWord=="WORLD")
		{
			infile >> std::ws >> keyWord;
			worldName = keyWord;

			if(keyWord=="Sphere")
			{
				reactorMat=92235;
				theWorldProps[MatPropPair(all,dimension)] = 8.7*cm;
				theWorldProps[MatPropPair(all,temperature)] = 293.6*kelvin;
			}
			else if(keyWord=="C6Lattice")
			{
				reactorMat=6;

				for(int i=0; i<6; i++)
                {
                    periodicBC.push_back(i);
                }

				theWorldProps[MatPropPair(all,dimension)] = 28.575*cm;
			}
			else if(keyWord=="Cube")
			{
				reactorMat=8;
				for(int i=0; i<6; i++)
                {
                    periodicBC.push_back(i);
                }
				uniformDis = true;

				theWorldProps[MatPropPair(all,dimension)] = 50.*cm;
				theWorldProps[MatPropPair(fuel,concentration)] = 0.7204*perCent;
				theWorldProps[MatPropPair(moderator,concentration)] = 90.0*perCent;
			}
			else if(keyWord=="ZED2")
			{

			}
			else if(keyWord=="SLOWPOKE")
			{

			}
			else if(keyWord=="SCWR")
			{

			}
			else if(keyWord=="SCWRDoppler")
			{

			}
			else if(keyWord=="SCWRJason")
			{

			}
			 else if(keyWord=="Test")
			{

			}
			else if(keyWord=="Q_ZED2")
			{

			}
            else if(userWorlds.find(keyWord) != userWorlds.end())
            {
                StorkMatPropChangeVector defaults = userWorlds[keyWord];

                for(G4int i=0; i < G4int(defaults.size()); i++)
                {
                    theWorldProps[defaults[i].GetMatPropPair()] = defaults[i].change;
                }
            }
			else
			{
				worldName = "Unknown";
			}
		}
		else if(keyWord=="MAT"&&(worldName == "Sphere" || worldName == "Cube"))
		{
			infile >> reactorMat;
		}

		//CSDataFileLocation
		else if(keyWord=="CS_DATA_DIR")
		{
			infile >> csDirName;
		}

		// Shannon Entropy
		else if(keyWord=="SE_MESH")
		{
			infile >> numSteps[0] >> numSteps[1] >> numSteps[2];
		}
		else if(keyWord == "SE_NUM_CONV_RUNS")
		{
			infile >> convRuns;
		}
		else if(keyWord == "SE_CONV_LIMIT")
		{
			infile >> limitSE;
		}


		//Initial Particles
		else if(keyWord=="NEUTRON_ENERGY")
		{
			infile >> initEnergy;
		}
		else if(keyWord=="INITIAL_SOURCE_FILE")
		{
			infile >> initialSourceFile;
			loadSources=true;
		}
		else if(keyWord=="INITIAL_DELAYED_FILE")
		{
			infile >> initialDelayedFile;
			loadDelayed=true;
		}


		//Runs
		else if(keyWord=="NUM_RUNS")
		{
			infile >> numberOfRuns;
		}
		else if(keyWord=="NUM_EVENTS")
		{
			infile >> numberOfEvents;
		}
		else if(keyWord=="NUM_PRIMARY_PER_EVENT")
		{
			infile >> numberOfPrimariesPerEvent;
		}
		else if(keyWord=="RUN_DURATION")
		{
			infile >> runDuration;
		}


		//Options
		else if(keyWord=="SEED")
		{
			infile >> randomSeed;
			overrideInitRandomSeed = true;
		}
		else if(keyWord=="PERIODIC_BC")
		{
            G4int nPBCSides;
			infile >> nPBCSides;
			G4String side;
			for(int i=0; i<nPBCSides; i++)
			{
                infile >> side;
                periodicBC.push_back(ConvertSide(side));
			}
		}
		else if(keyWord=="REFLECT_BC")
		{
			G4int nRBCSides;
			infile >> nRBCSides;
			G4String side;
			for(int i=0; i<nRBCSides; i++)
			{
                infile >> side;
                reflectBC.push_back(ConvertSide(side));
			}
		}
		else if(keyWord=="RENORMALIZE")
		{
			infile >> normalize;
		}
		else if(keyWord=="INSTANT_DELAYED")
		{
			infile >> instantDelayed;
		}
		else if(keyWord=="UNIFORM_DISTRIBUTION")
		{
			infile >> uniformDis >> uniDisShape;
        }
        else if(keyWord=="UNIFORM_DIST_WITH_DIM")
		{
            G4double tempDim[6];
			infile >> uniformDis >> uniDisShape >> uniformDisWithDim >> tempDim[0] >> tempDim[1] >> tempDim[2] >> tempDim[3] >> tempDim[4] >> tempDim[5];
			uniDisDim = tempDim;
        }
		else if(keyWord=="INITIAL_SOURCE_POS")
		{
			infile >> initialSourcePos[0] >> initialSourcePos[1] >> initialSourcePos[2];
		}
		else if(keyWord=="INTERP_START_COND")
		{
			infile >> interpStart;
		}


		// Logging
		else if(keyWord=="OUTPUT_LOG")
		{
			infile >> logFile;
			logData=true;
		}
		else if(keyWord=="OUTPUT_SOURCE")
		{
			saveSources=-1;

			while(infile.peek() == ' ' || infile.peek() == '\t')
				infile.get();

			if(infile.peek() == '\n')
				infile.getline(line,256);
			else
				infile >> sourceFile;
		}
		else if(keyWord=="OUTPUT_SRC_FREQ")
		{
			infile >> saveSources;
		}
		else if(keyWord == "OUTPUT_FISSION")
		{
			infile >> fissionDataFile;
			saveFissionData = true;
		}

		// World data (initial and interpolation)
		else if(keyWord == "INTERPOLATION_DATA")
		{
			// Read the material and property
			infile >> mat >> prop;

			// Parse the strings
			mpPair.first = theMPMan->ParseMatEnum(mat);
			mpPair.second = theMPMan->ParsePropEnum(prop);
			mpString = PrintMatPropPair(mpPair);

			// Store the data file name
			dataFile.first = "# " + mpString;
			infile >> dataFile.second;
			theDataProfiles.push_back(dataFile);

			// Create a new interpolation vector from the data file
			theMPInterpMan.CreateInterpVector(mpString.strip(':'),
										dataFile.second, mpPair,
										theMPMan->GetUnits(mpPair.second));
		}
		else if(keyWord=="SET_WORLD_PROP")
		{
			// Read the material and property
			infile >> mat >> prop >> newValue;

			// Parse the strings
			mpPair.first = theMPMan->ParseMatEnum(mat);
			mpPair.second = theMPMan->ParsePropEnum(prop);

			// Add the property to the world property map
			theWorldProps[mpPair] = newValue*theMPMan->GetUnits(mpPair.second);
		}

        // Error if unknown keyword
		else
		{
			G4cerr << "*** WARNING: Unrecognized or inapplicable keyword: "
				   << keyWord << G4endl;
		}

		// Reset keyword
		keyWord = "";
    }

	// Close the input file
	infile.close();


    //Check to make sure necessary input data has been given

	// Check whether world was set properly
    if(worldName == "Unknown")
    {
        G4cerr << "Missing world type from input file \n";
        return false;
    }
	// Check whether other mandatory data was set
    else if(numberOfEvents==0 || numberOfPrimariesPerEvent==0 ||
            numberOfRuns==0 || runDuration==0)
    {
        G4cerr << "Missing simulation data from input file \n";
        return false;
    }

    // Set the saveSources interval to total number of runs if not set by user
    if(saveSources < 0)
    {
        saveSources=numberOfRuns;
    }
    // Reset interpStart flag if no interpolations set
    if(interpStart && theMPInterpMan.GetNumberOfInterpVectors() == 0)
    {
    	interpStart = false;
    }
    // Set the random seed if not set by the input file
    if(isMaster && randomSeed <= 0)
    {
    	srand(time(NULL));
		randomSeed = rand();
    }

    // Open the logging file (if necessary)
    if(isMaster && logData)
    {
    	logStream = new std::ofstream(logFile, std::ios_base::app);
    }

    // Set other file names if not given using logging file name
    if(logData)
    {
    	if(saveSources && sourceFile == "")
    	{
    		pos = logFile.find_last_of("/");

    		sourceFile = logFile.substr(0,pos) + "/Src-" +
							logFile.substr(pos+1,logFile.length()-1);
    	}


		if(saveFissionData && fissionDataFile == "")
		{
    		pos = logFile.find_last_of("/");

    		fissionDataFile = logFile.substr(0,pos) + "/Fission-" +
								logFile.substr(pos+1,logFile.length()-1);
    	}
    }


    // Set the temperature of the neutron cross section data
	if(!getenv("G4NEUTRONHPDATA"))
	{
	    G4cerr << "Environment variable for Neutron HP data not set." << G4endl;
	    return false;
	}

	datapath = getenv("G4NEUTRONHPDATA");

	// Find the folder name for the cross sections from the data path
    pos = datapath.find_last_of("/");

	// Check to make sure "/" is not the last character. If it is, erase it and
    // find the second to last slash
    while(pos == (G4int)(datapath.length()) - 1)
    {
        datapath.erase(pos,1);
        pos = datapath.find_last_of("/");
    }

	// Check where the dataPath points to a G4NDL library (0 K by default)
	if(datapath.find("G4NDL") != G4String::npos)
	{
	    xsTemp = 0.0*kelvin;
	}
	else
	{
	    // Find the start of the temperature
	    pos = datapath.find("_T");

	    // Determine xsTemp from datapath
	    xsTemp = atof((datapath.substr(pos+2)).c_str());

	    // Correct for the decimal in room temperature
	    if(xsTemp == 293.*kelvin)
            xsTemp = 293.6*kelvin;
	}

    // Set the units of input data
	runDuration *= ns;
	initEnergy *= MeV;

	// Create the inital properties vector from the world property map
	SetInitialPropertiesVector();

	return ErrorChecking();
}

G4int StorkParseInput::ConvertSide(G4String side)
{
    G4int sideNum=0;
    G4bool check1=false, check2=false;
    for(G4int i=0; i<G4int(side.size()); i++)
    {
        if((side[i]=='-')&&(!check1))
        {
            sideNum++;
            check1=true;
        }
        else if((side[i]=='y')&&(!check2))
        {
            sideNum+=2;
            check2=true;
        }
        else if((side[i]=='z')&&(!check2))
        {
            sideNum+=4;
            check2=true;
        }
    }
    return sideNum;
}

// AddWorldType()
// Add user defined world type to ParseInput
void StorkParseInput::AddWorldType(G4String name,
                                   StorkMatPropChangeVector defaults)
{
    userWorlds[name] = defaults;
}


// PrintInput()
// Prints all of the input parameters in a formatted table
void StorkParseInput::PrintInput(std::ostream *output) const
{
	// Find current time
	time_t rawtime = time(NULL);
	G4String currentTime = asctime(localtime(&rawtime));

    // outputs the given data
    output->fill(' ');
    output->setf(std::ios_base::left);

	// Outputs Geant4 and NStable version info and current time
	*output << "# ---------------------------------------------------------"
            << G4endl << "#" << G4endl
            << "# Geant 4 Simulation of Neutron Stability" << G4endl
			<< "#" << G4endl
			<< "#" << g4Version << G4endl
			<< "# Neutron Stability rev." << BZR_REVNO
			<< "  (Bazaar build date " << BZR_BUILD_DATE << ")" << G4endl
			<< "#" << G4endl
			<< "# Current time: \t" << currentTime
			<< "# " << G4endl;

	// Outputs the initial parameters needed to create the world and start the
	// run manager
	*output << "## World Choice: " << G4endl
			<< std::setw(37) << "# World: " << worldName << G4endl
			<< std::setw(37) << "# Reactor Material: " << reactorMat << G4endl
			<< std::setw(37) << "# Shannon entropy mesh: " << "(" << numSteps[0] << ", "
			<< numSteps[1] << ", " << numSteps[2] << ")" << G4endl
			<< "#" << G4endl;

	// Ouput the run options
	*output << "## Run Options:" << G4endl
			<< std::setw(37) << "# Number of runs: " << numberOfRuns << G4endl
			<< std::setw(37) << "# Number of primaries per Event: "
			<< numberOfPrimariesPerEvent << G4endl
			<< std::setw(37) << "# Number of events: " << numberOfEvents << G4endl
			<< std::setw(37) << "# Run duration (ns): " << runDuration << G4endl
			<< std::setw(37) << "# Initial Neutron Energy (MeV): ";
    if(loadSources)
        *output << "N/A";
    else
        *output << initEnergy;

	if(overrideInitRandomSeed)
		*output << G4endl << std::setw(37) << "# Random Seed: " << randomSeed;

	*output << G4endl << "#" << G4endl;

	// Outputs world properties
	*output << "## World Properties:";
	for(G4int i=0; i < G4int(initialWorldProps.size()); i++)
	{
		*output << G4endl << "# "<< std::setw(35)
		        << PrintMatPropPair(initialWorldProps[i].GetMatPropPair())
		        << initialWorldProps[i].change /
					theMPMan->GetUnits((PropEnum)initialWorldProps[i].propN);
	}

	// Outputs the nuclear data and simulation options
    *output << G4endl << "#" << G4endl
            << "## Nuclear Data Options:" << G4endl
            << std::setw(37) << "# Data Library: " << datapath << G4endl
			<< std::setw(37) << "# Cross section temperature (K): " << xsTemp << G4endl
			<< "#" << G4endl;

	// Outputs warning if renormalization is turned off
	if(!normalize)
		*output << std::setw(37) << "# *** Renormalize after run:" << "NO ***"
		        << G4endl << "#" << G4endl;

	// Outputs the input and output files to be used
	if(logData || saveSources || loadSources || saveFissionData)
	{
		*output << "## Log Files:" << G4endl;

		if(logData)
		{
			*output	<< std::setw(37) << "# Logging File: "
				    << logFile << G4endl;
		}


		if(saveSources!=0)
		{
			*output	<< std::setw(37) << "# Output source file: "
			        << sourceFile << G4endl
					<< std::setw(37) << "# Save source distribution interval: "
					<< saveSources << G4endl;
		}

		if(saveFissionData)
		{
			*output << std::setw(37) << "# Output fission data file: "
			        << fissionDataFile << G4endl;
		}

		if(loadSources)
		{
			*output	<< std::setw(37) << "# Initial source file: "
			        << initialSourceFile << G4endl;
		}

		*output	<< "#" << G4endl;
	}


	// Outputs the file names of the interpolation files to be used
	if(theDataProfiles.size() > 0)
	{
		*output << "## Interpolation Data Files:" << G4endl;

		for(G4int i=0; i < G4int(theDataProfiles.size()); i++)
		{
			*output << std::setw(37) << theDataProfiles[i].first
					<< theDataProfiles[i].second << G4endl;
		}

		*output	<< "#" << G4endl;
	}
}


// GetWorldDimensions()
// Gets the size of the smallest box surrounding the world
G4ThreeVector StorkParseInput::GetWorldDimensions() const
{
    G4double maxDim = theWorldProps.find(MatPropPair(all,dimension))->second;

    if(reactorMat == 6)
    {
		maxDim += 2.0;
    }
    else
    {
		maxDim = 2.0*(maxDim + 1.0);
    }

    return G4ThreeVector(maxDim,maxDim,maxDim);
}


// SetInitialProperty()
// Set up the initial properties for the given world
void StorkParseInput::SetInitialPropertiesVector()
{
	// Build property change vector based on world properties map
	InitialPropertyMap::iterator itr = theWorldProps.begin();

	for(; itr != theWorldProps.end(); itr++)
	{
		initialWorldProps.push_back(StorkMatPropChange(itr->first,itr->second));
	}
}


// PrintMatPropPair()
// Get a string for printing a material property pair with the appropriate units
G4String StorkParseInput::PrintMatPropPair(MatPropPair mpPair) const
{
	std::stringstream buff;

	// Print material type
	buff << theMPMan->GetEnumKeyword(mpPair.first) << "/";

	// Print property (if dimension and all, print "Radius" or "Pitch")
	if(mpPair.second == dimension && mpPair.first == all)
	{
		if(worldName == "Sphere")
			buff << "Radius";
		else
			buff << "Pitch";
	}
	else
	{
		buff << theMPMan->GetEnumKeyword(mpPair.second);
	}

	// Print units
	buff << " (" << theMPMan->GetUnitsName(mpPair.second) << "):";


	return buff.str();
}


// ErrorChecking()
// Check to see if there are errors in the data passed from the input file
G4bool StorkParseInput::ErrorChecking()
{
    G4bool test = true;

    if(randomSeed<0)
    {
        test=false;
        G4cerr << "*** ERROR:  Invalid random seed: " << randomSeed << G4endl;
    }

    if(reactorMat<0 || reactorMat>104000)
    {
        test=false;
        G4cerr << "*** ERROR:  Invalid reactor material: " << reactorMat
               << G4endl;
    }

    if(numberOfEvents<=0||numberOfPrimariesPerEvent<=0||numberOfRuns<=0)
    {
        test=false;
        G4cerr << "*** ERROR:  Invalid number of events/primaries/runs: "
               << numberOfEvents << "/"
               << numberOfPrimariesPerEvent << "/"
               << numberOfRuns << G4endl;
    }

    if(numSteps[0]<=0 || numSteps[1]<=0 || numSteps[2]<=0)
    {
        test=false;
        G4cerr << "*** ERROR:  Invalid Shannon entropy mesh: "
			   << numSteps[0] << "/" << numSteps[1] << "/" << numSteps[2]
			   << G4endl;
    }

    if(convRuns < 5 || limitSE <= 0)
    {
    	test = false;
    	G4cerr << "*** ERROR:  Invalid Shannon entropy convergence limit/runs: "
               << limitSE << "/" << convRuns << G4endl;
    }

    if(initEnergy <= 0)
    {
        test=false;
        G4cerr << "*** ERROR:  Invalid initial neutron energy: " << initEnergy
               << G4endl;
    }

    if(saveSources < 0)
    {
        test=false;
        G4cerr << "*** ERROR:  Invalid source distribution save interval"
               << G4endl;
    }

    // Check world properties
    for(G4int i=0; i < G4int(initialWorldProps.size()); i++)
	{
		if(initialWorldProps[i].change <= 0)
		{
			test = false;
			G4cerr << "*** ERROR:  Invalid "
			       << PrintMatPropPair(initialWorldProps[i].GetMatPropPair())
			       << " " << initialWorldProps[i].change << G4endl;
		}
	}

	// Check file names
	if(logData && logFile == "")
	{
		test=false;
        G4cerr << "*** ERROR:  Empty log file name." << G4endl;
	}

	if(saveSources && sourceFile == "")
	{
		test=false;
        G4cerr << "*** ERROR:  Empty output source file name." << G4endl;
	}

	if(saveFissionData && fissionDataFile == "")
	{
		test=false;
        G4cerr << "*** ERROR:  Empty fission data file name." << G4endl;
	}

	if(loadSources && initialSourceFile == "")
	{
		test=false;
        G4cerr << "*** ERROR:  Empty initial source file name." << G4endl;
	}

	if(loadSources)
	{
		std::ifstream initFile(initialSourceFile);

		if(!initFile.is_open())
		{
			test=false;
			G4cerr << "*** ERROR: Unable to open initial source file."
				   << G4endl;
		}
		else
		{
			initFile.close();
		}
	}

    return test;
}
