/*
StorkVWorldConstructor.cc

Created by:		Liam Russell
Date:			23-05-2012
Modified:       11-03-2013

Source file for StorkVWorldConstructor class.

*/

// Include header file
#include "StorkVWorldConstructor.hh"


// Constructor
StorkVWorldConstructor::StorkVWorldConstructor()
: worldLogical(0), worldPhysical(0), worldVisAtt(0)
{
	// Initialize member variables
	geomChanged = true;
	matChanged = true;
	encWorldDim = G4ThreeVector(0.,0.,0.);
	reactorDim = encWorldDim;
}


// Destructor
StorkVWorldConstructor::~StorkVWorldConstructor()
{
	// Destroy all materials, elements and isotopes
    DestroyMaterials();

    delete worldVisAtt;
}


// DestroyMaterials()
// Delete all materials, elements, and isotopes
void StorkVWorldConstructor::DestroyMaterials()
{
    // Destroy all allocated materials, elements and isotopes
    size_t i;

    G4MaterialTable *matTable = (G4MaterialTable*)G4Material::GetMaterialTable();
    for(i=0; i<matTable->size(); i++)
    { delete (*(matTable))[i]; }
    matTable->clear();

    G4ElementTable *elemTable = (G4ElementTable*)G4Element::GetElementTable();
    for(i=0; i<elemTable->size(); i++)
    { delete (*(elemTable))[i]; }
    elemTable->clear();

    G4IsotopeTable *isoTable = (G4IsotopeTable*)G4Isotope::GetIsotopeTable();
    for(i=0; i<isoTable->size(); i++)
    { delete (*(isoTable))[i]; }
    isoTable->clear();

    return;
}


// GetWorldProperty()
// Returns the current world property associated with the given MatPropPair
G4double StorkVWorldConstructor::GetWorldProperty(MatPropPair matProp)
{
    if (IsApplicable(matProp))
    {
		return *(variablePropMap[matProp]) / theMPMan->GetUnits(matProp.second);
    }
    else
    {
        return -1.0;
    }
}


// IsApplicable()
// Determines whether the given material and property pair is valid in the
// current world
G4bool StorkVWorldConstructor::IsApplicable(MatPropPair matProp)
{
    if(variablePropMap.find(matProp)==variablePropMap.end())
        return false;
	else
	    return true;
}


// ConstructNewWorld()
// Set the initial properties (material-properties), sensitive detector,
// and neutron filter for the sensitive detector.  Then construct the world
// using a derived class implementation of ConstructWorld().
G4VPhysicalVolume*
StorkVWorldConstructor::ConstructNewWorld(const StorkParseInput* infile)
{
    // Get any initial changes (vector may be empty)
	initialChanges = infile->GetIntialWorldProperties();

	// Build sensitive detector
    G4SDManager *sDMan = G4SDManager::GetSDMpointer();
    sDReactor = new StorkNeutronSD("Reactor",infile->GetInstantDelayed(),
							  infile->GetPeriodicBC());
    sDMan->AddNewDetector(sDReactor);

    // Add filters to the sensitive detectors so that they only track neutrons
    G4SDParticleFilter *nFilter = new G4SDParticleFilter("neutronFilter",
														 "neutron");
    sDReactor->SetFilter(nFilter);


    // Add any initial changes to the world properties
	if(G4int(initialChanges.size()) > 0)
		UpdateWorldProperties(initialChanges);

	return ConstructWorld();
}


// UpdateWorld()
// Update the variable material-properties.
G4VPhysicalVolume*
StorkVWorldConstructor::UpdateWorld(StorkMatPropChangeVector changes)
{
	if(UpdateWorldProperties(changes))
	{
		return ConstructWorld();
	}
	else
	{
		return worldPhysical;
	}
}


// UpdateWorldProperties()
// Check whether ALL of the proposed changes are valid.  If so, apply the
// changes.
G4bool
StorkVWorldConstructor::UpdateWorldProperties(StorkMatPropChangeVector changes)
{
		// Check whether proposed changes are applicable to the world
	for(G4int i=0; i<G4int(changes.size()); i++)
	{
		if(!IsApplicable(changes[i].GetMatPropPair()))
		{
			G4cerr << "*** ERROR: Inapplicable world changes."
				   << " Returning original world." << G4endl;

			return false;
		}
	}

	// Apply changes
	for(G4int i=0; i<G4int(changes.size()); i++)
	{
		// Change the variable properties
		*(variablePropMap[changes[i].GetMatPropPair()]) = changes[i].change;

        if(changes[i].GetMatPropPair().second!=theMPMan->ParsePropEnum("position"))
            matChanged = true;
	}

	return true;
}



