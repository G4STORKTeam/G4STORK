#include "StorkHeatTransfer.hh"
// Constructor
StorkHeatTransfer::StorkHeatTransfer(const StorkParseInput* fIn)
{

    worldname = fIn->GetWorld();
    
    if(worldname == "SLOWPOKE")
        InitializeSlowpokeModel(fIn);

}
// Destructor
StorkHeatTransfer::~StorkHeatTransfer(void)
{
}

void StorkHeatTransfer::InitializeSlowpokeModel(const StorkParseInput* input)
{
    dt = input->GetRunDuration()*ms;
    heatTransferCoeff = input->GetHeatTransferCoefficient();
    temp_infty = input->GetAmbientTemperature();
    saveMaterialData = input->SaveTemperature();
    saveMaterialfilename = input->GetTemperatureDataFile();
    fissionToEnergyCoeff = input->GetFissionToEnergyCoefficient();
    baselineFissionRate = input->GetBaselineFissionRate();
    return;
}

void StorkHeatTransfer::RunThermalCalculation(MSHSiteVector fissionSites)
{
    fSites = &(fissionSites);
    
    if(worldname == "SLOWPOKE"){
        RunSlowpokeModel();
    }
    
    return;
}

void StorkHeatTransfer::RunSlowpokeModel()
{
    
    SetFuelDimensions(theWorld->GetFuelDimensions());
    CalcHeatDistribution();
    
    UpdateFuelProperties(theWorld->GetFuelTemperatures(),theWorld->GetFuelDensities(),theWorld->GetFuelRadii());
    
    if(saveMaterialData)
        SaveSLOWPOKEFuelProperties(saveMaterialfilename);
    
    theWorld->SetPhysChanged(true);
    theWorld->SetMatChanged(true);
    
    return;
}

//
void StorkHeatTransfer::UpdateFuelProperties(G4double FuelTemperatures[],G4double FuelDensities[],G4double FuelRadii[])
{
    
    //Initialize variables.
    StorkMaterial *material;
    G4double newTemperature;
    G4double newDensity;
    G4double newRadius;
    G4double oldDensity;
    G4String fuelNum;
    const char * num;
    G4double heatCapacity;
    G4double mass;
    G4double surfaceArea;
    G4double HeatInMaterial;
    G4int size = fnDistribution.size();
    fuelTempAvg = fuelDensityAvg = fuelRadiusAvg = 0.0;
    
    G4double TotalRunFissions = fSites->size();
    
    G4double heatGenerated = CalcEffectiveHeatGenerated(TotalRunFissions);
    
    //Get material map.
    StorkMaterialMap *matMap = theWorld->GetMaterialMap();
    
    //Iterate through fission material list.
    std::map< G4String, G4double >::iterator it;
    for(it = fnDistribution.begin(); it != fnDistribution.end(); it++){
        
        //Get the fuel name and number ID;
        fuelNum = (it->first);
        
        num = (fuelNum.erase(0,4)).c_str();
        
        G4int i = std::atoi(num);
        
        //Calculate the heat generated in each material.
        HeatInMaterial = heatGenerated*(it->second);
        
        //Get material.
        material = static_cast<StorkMaterial*>((*matMap)[it->first]);
        
        //Get material and geometric properties.
        heatCapacity = material->GetSpecificHeatCapacity();
        oldDensity = material->GetDensity();
        mass = oldDensity*CLHEP::pi*fuelDimensions[2]*FuelRadii[i]*FuelRadii[i];
        surfaceArea = 2*CLHEP::pi*fuelDimensions[2]*FuelRadii[i];
        
        //Calculate new material properties.
        newTemperature = CalcFuelTemperature(HeatInMaterial, mass, surfaceArea, FuelTemperatures[i], heatCapacity);
        //newDensity = CalcFuelDensity(newTemperature);
        //newRadius = CalcFuelRadius(FuelRadii[i], oldDensity, newDensity);
        
        //Set the new fuel properties.
        FuelTemperatures[i] = newTemperature;
        //FuelDensities[i] = newDensity;
        //FuelRadii[i] = newRadius;
        
        //Calculate averages.
        if(saveMaterialData){
            fuelTempAvg += newTemperature/size;
           // fuelDensityAvg += newDensity/size;
           // fuelRadiusAvg += newRadius/size;
        }
        
    }
    

    return;
}

void StorkHeatTransfer::CalcHeatDistribution(){
    

    //Get the total number of fissions.
    G4int numberOfFissions = fSites->size();
    
    //Percentage per fission
    G4double fPercent = (1./numberOfFissions);
    
    //Create material list flag.
    G4bool createList = false;
    
    //Clear the distribution map.
    fnDistribution.clear();

    //Get pointer to the Navigator
    G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    theNavigator->ResetStackAndState();

    if(fnMaterialList.size() == 0)
        createList = true;
    
    std::vector<StorkTripleFloat>::iterator itr = fSites->begin();

    //Iterate through all fission sites of the run.
    for(; itr!=fSites->end(); itr++){
        
        G4ThreeVector site = itr->GetData();

        //Get the material.
        G4String currentMaterialName = theNavigator->LocateGlobalPointAndSetup(site)->GetLogicalVolume()->GetMaterial()->GetName();

        //Check to see if it is already in the material list, else create it.
        if(fnDistribution[currentMaterialName]){
            fnDistribution[currentMaterialName] += fPercent;
        }
        else
            fnDistribution[currentMaterialName] = fPercent;
        
        
    }
    
    //Calculate the effective fission energy.

    return;

}

//Calculates new temperature based on heat generation. Uses FUELPIN solver method.
G4double StorkHeatTransfer::CalcFuelTemperature(G4double heatGeneration, G4double mass, G4double surfaceArea, G4double oldTemp, G4double heatcapacity){
    
    //Initialize with proper units.
    G4double newTemperature;
    G4double htc = heatTransferCoeff*(joule/(s*m2*kelvin));
    G4double t = dt*ns;
    
    //Calculate coefficients for new temperature.
    G4double a = -htc*surfaceArea/(mass*heatcapacity);
    G4double bu = (heatGeneration + htc*surfaceArea*temp_infty)/(mass*heatcapacity);
    
    //Calculate new temperature.
    newTemperature = oldTemp*exp(-a*t) + (1 - exp(-a*t))*(bu/a);

    return newTemperature*kelvin;
}

// Calculates new density (U2O) based on temperature. http://web.ornl.gov/~webworks/cpr/v823/rpt/109264.pdf
G4double StorkHeatTransfer::CalcFuelDensity(G4double temperature){
    
    //Initialize variables.
    G4double density, a, b, c, d ,e, t;
    t = temperature;
   // a = 10970;
    a = 6.6160*pow(10,19);
    b = 9.99672*pow(10,-1);
    c = 1.179*pow(10,-5);
    d = -2.429*pow(10,-9);
    e = 1.219*pow(10,-12);
    
    //Calculate using empirical formula.
    density = a*pow( (b + c*t + d*pow(t,2) + e*pow(t,3)) ,-3);
    
    return density;
}

G4double StorkHeatTransfer::CalcFuelRadius(G4double oldRadius, G4double oldDensity, G4double newDensity)
{
    G4double newRadius = oldRadius*pow( oldDensity/newDensity, 0.5);
    if(newRadius>0.212*cm){
        G4cerr << "WARNING: Fuel is expanding beyond limits!" << G4endl;
        return oldRadius;
    }
    
    return newRadius;
    
}

void StorkHeatTransfer::SetWorld(StorkWorld* world)
{
    theWorld = world;
}

void StorkHeatTransfer::SaveSLOWPOKEFuelProperties(G4String filename)
{
    if(createHeader){
        // Declare and open file stream
        std::ofstream outFile(filename.c_str(),std::ofstream::app);
        
        // Check that stream is ready for use
        if(!outFile.good())
        {
            G4cerr << G4endl << "ERROR:  Could not write material temperatures to file. " << G4endl
            << "Improper file name: " << filename << G4endl
            << "Continuing program without material temperature data output" << G4endl;
            
            return;
        }
        else
        {
        
            outFile.fill(' ');
            outFile << "    *** SLOWPOKE REACTOR - LUMPED PARAMETER THERMAL MODEL ***   " << G4endl;
            outFile << "            VERSION - 1.0 - DATE: 07/14/2015        " << G4endl;

            outFile << " Avg. Fuel Temperature  "
                    << " Avg. Fuel Density      "
                    << " Avg. Fuel Volume       "
                    << G4endl;
        
        
            outFile.width(120);
            outFile.fill('-');
        
            outFile << G4endl;
            outFile.close();
            createHeader = false;
        }
    }
    
    std::ofstream outFile(filename.c_str(),std::ofstream::app);

    if(!outFile.good()){
        G4cerr << G4endl << "ERROR:  Could not write material temperatures to file. " << G4endl
        << "Improper file name: " << filename << G4endl
        << "Continuing program without material temperature data output" << G4endl;
    }
    else{
    
        outFile << std::setw(24) << fuelTempAvg
                << std::setw(24) << fuelDensityAvg
                << std::setw(24) << fuelRadiusAvg
                << G4endl;
        
        outFile.close();
        
    }

    return;
    
}

G4double StorkHeatTransfer::CalcEffectiveHeatGenerated(G4double currentFissions)
{
    //Get the corresponding baseline fissions.
    G4double baselineFissions = baselineFissionRate*dt;
    
    //If the current rate is less than the baseline than return no heat generated.
    if(currentFissions<baselineFissions)
        return 0.;
    
    //Otherwise return the heat; difference in number of fissions times the effective energy/fission
    //and rescale it.
    return fissionToEnergyCoeff*(currentFissions - baselineFissions);
}


