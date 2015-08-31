/*
StorkNeutronHPCSData.cc

Created by:		Liam Russell
Date:			22-06-2011
Modified:		17-02-2012

Source code for StorkNeutronHPCSData class.

*/


// Include header file

#include "StorkNeutronHPCSData.hh"


G4bool StorkNeutronHPCSData::IsApplicable(const G4DynamicParticle*aP,
									 const G4Element*)
{
    G4bool result = true;
    G4double eKin = aP->GetKineticEnergy();
    if(eKin>20*MeV||aP->GetDefinition()!=G4Neutron::Neutron()) result = false;
    return result;
}


StorkNeutronHPCSData::StorkNeutronHPCSData(G4int /*OL*/, G4int IL, G4double aT, char aType)
{
    // TKDB
    theCrossSections = 0;
//    numMaxOL = OL;
	numMaxOL = INT_MAX;
    numIL = IL;
    temperature = aT;
    reactionType = aType;

    BuildPhysicsTable(*G4Neutron::Neutron());
}


StorkNeutronHPCSData::~StorkNeutronHPCSData()
{
    // TKDB
    if ( theCrossSections != 0 )
        theCrossSections->clearAndDestroy();
    delete theCrossSections;
}


void StorkNeutronHPCSData::BuildPhysicsTable(const G4ParticleDefinition& aP)
{

  if(&aP!=G4Neutron::Neutron())
     throw G4HadronicException(__FILE__, __LINE__,
		"Attempt to use NeutronHP data for particles other than neutrons!!!");


  size_t numberOfElements = G4Element::GetNumberOfElements();

  if ( theCrossSections == 0 )
	theCrossSections = new G4PhysicsTable( numberOfElements );
  else
	return;  // Skip this if table is already built ***

	/* *** I have NO idea why this is not standard behaviour!!!! If this return
		is not included, every time the physics is rebuilt, another new data set
		is added.  This results in all physical memory being consumed.
			-Liam 20/08/2012
	*/

  // Null pointers to tell G4NeutronHPElementData what type of data to return
  G4NeutronHPCaptureData *Cptr = NULL;
  G4NeutronHPFissionData *Fptr = NULL;
  G4NeutronHPElasticData *Eptr = NULL;
  G4NeutronHPInelasticData *Iptr = NULL;

  // make a PhysicsVector for each element
  static const G4ElementTable *theElementTable = G4Element::GetElementTable();
  for( size_t i=0; i<numberOfElements; ++i )
  {
  	G4PhysicsVector *physVec = NULL;

  	switch(reactionType)
  	{
  		case 'C':
			physVec = G4NeutronHPData::
				Instance()->MakePhysicsVector((*theElementTable)[i], Cptr);
			break;
		case 'E':
			physVec = G4NeutronHPData::
				Instance()->MakePhysicsVector((*theElementTable)[i], Eptr);
			break;
		case 'I':
			physVec = G4NeutronHPData::
				Instance()->MakePhysicsVector((*theElementTable)[i], Iptr);
			break;
		case 'F':
			physVec = G4NeutronHPData::
				Instance()->MakePhysicsVector((*theElementTable)[i], Fptr);
			break;
  	}

    theCrossSections->push_back(physVec);
  }
}


void StorkNeutronHPCSData::DumpPhysicsTable(const G4ParticleDefinition& aP)
{
  if(&aP!=G4Neutron::Neutron())
     throw G4HadronicException(__FILE__, __LINE__,
		"Attempt to use NeutronHP data for particles other than neutrons!!!");


   // Dump element based cross section range 10e-5 eV to 20 MeV 10 point per
   // decade in barn
   G4String type;

   // Set the reaction type
   switch(reactionType)
   {
  		case 'C':
			type = "Capture ";
			break;
		case 'E':
			type = "Elastic ";
			break;
		case 'I':
			type = "Inelastic ";
			break;
		case 'F':
			type = "Fission ";
			break;
   }

   G4cout << G4endl << G4endl << type << "Cross Section of Neutron HP"<< G4endl
          << "(Pointwise cross-section at 0 Kelvin.)" << G4endl << G4endl
          << "Name of Element" << G4endl << "Energy[eV]  XS[barn]" << G4endl
          << G4endl;

   size_t numberOfElements = G4Element::GetNumberOfElements();
   static const G4ElementTable *theElementTable = G4Element::GetElementTable();

   for ( size_t i = 0 ; i < numberOfElements ; ++i )
   {

      G4cout << (*theElementTable)[i]->GetName() << G4endl;

      for (G4int ie = 0 ; ie < 130 ; ie++ )
      {
         G4double eKinetic = 1.0e-5 * std::pow ( 10.0 , ie/10.0 ) *eV;
         G4bool outOfRange = false;

         if ( eKinetic < 20*MeV )
         {
            G4cout << eKinetic/eV << " "
                   << (*((*theCrossSections)(i))).GetValue(eKinetic,outOfRange)/barn
                   << G4endl;
         }
      }

      G4cout << G4endl;
   }
}


G4double StorkNeutronHPCSData::
GetCrossSection(const G4DynamicParticle* aP, const G4Element*anE, G4double aT)
{
    if(reactionType == 'F' && anE->GetZ() < 90) return 0;

    G4bool outOfRange;
    G4int index = anE->GetIndex();

    // prepare neutron
    G4double eKinetic = aP->GetKineticEnergy();
    G4ReactionProduct theNeutron( aP->GetDefinition() );
    theNeutron.SetMomentum( aP->GetMomentum() );
    theNeutron.SetKineticEnergy( eKinetic );
    G4ThreeVector neutronVelocity =
		1./G4Neutron::Neutron()->GetPDGMass()*theNeutron.GetMomentum();

    // prepare thermal nucleus
    G4Nucleus aNuc;
    G4double eps = 0.0001;
    G4double theA = anE->GetN();
    G4double theZ = anE->GetZ();
    G4double eleMass;

    eleMass = (G4NucleiProperties::GetNuclearMass(static_cast<G4int>(theA+eps),
			static_cast<G4int>(theZ+eps))) / G4Neutron::Neutron()->GetPDGMass();

	// Find the temperature difference between the temperature the cross
	// section was evaluated at versus the temperature of the material
	G4double tempDiff = aT - temperature;

	// If there is no temperature difference, return the cross section directly
	if(std::abs(tempDiff) <= 0.1)
	{
		return (*((*theCrossSections)(index))).GetValue(eKinetic, outOfRange);
	}

    // Declarations for averaging loop
    G4ReactionProduct boosted;      // neutron in rest frame of nucleus
    G4double aXsection;             // random xsec value
    G4double result = 0;            // final averaged cross section
    G4double buffer = 1;            // temporary value for previous average
    G4int counter = 0;              // counts the number averaged xsec sets

    // Reset the loop counters if standard Doppler broadening is being used
//    if(std::abs(numMaxOL - INT_MAX) < 1)
//    {
//        numIL = G4int(std::max(10.,tempDiff/60.*kelvin));
//    }

	numIL = G4int(std::max(10.,tempDiff/60.*kelvin));

    // Find the Doppler broadened cross section
    while(counter < numMaxOL &&
		std::abs((buffer-result/(std::max(1,counter)*numIL))/buffer) > 0.03)
    {
        if(counter)
            buffer = result/(numIL*counter);
        else
            buffer = 0;

        for(G4int i=0; i < numIL; i++)
        {
            // Create a random nucleus of the target element
            G4ReactionProduct aThermalNuc = aNuc.GetThermalNucleus(eleMass,
																   tempDiff);

            // Find the neutron in the rest frame of the nucleus
            boosted.Lorentz(theNeutron, aThermalNuc);
            G4double theEkin = boosted.GetKineticEnergy();

            // Find the cross-section of the element at the energy of the
            // neutron IN THE REST FRAME OF THE NUCLEUS
            aXsection =
				(*((*theCrossSections)(index))).GetValue(theEkin, outOfRange);

            // Velocity correction
            G4ThreeVector targetVelocity =
				1./aThermalNuc.GetMass()*aThermalNuc.GetMomentum();
            aXsection *=
				(targetVelocity-neutronVelocity).mag()/neutronVelocity.mag();

            // Add cross section to results
            result += aXsection;
        }

        counter++;
    }

    return result/(numIL*counter);
}

