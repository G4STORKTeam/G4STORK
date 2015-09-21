//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// neutron_hp -- source file
// J.P. Wellisch, Nov-1996
// A prototype of the low energy neutron transport model.
//
// 070523 bug fix for G4FPE_DEBUG on by A. Howard ( and T. Koi)
// 080319 Compilation warnings - gcc-4.3.0 fix by T. Koi
//
#include "StorkNeutronHPElastic.hh"
#include "G4SystemOfUnits.hh"
#include "G4NeutronHPElasticFS.hh"
#include "G4NeutronHPManager.hh"

  StorkNeutronHPElastic::StorkNeutronHPElastic(G4double finalStateTemp)
    :G4HadronicInteraction("NeutronHPElastic")
  {
    overrideSuspension = false;
    G4NeutronHPElasticFS * theFS = new G4NeutronHPElasticFS;
    fsTemp = finalStateTemp;
    if(!getenv("G4NEUTRONHPDATA"))
       throw G4HadronicException(__FILE__, __LINE__, "Please setenv G4NEUTRONHPDATA to point to the neutron cross-section files.");
    dirName = getenv("G4NEUTRONHPDATA");
    G4String tString = "/Elastic";
    dirName = dirName + tString;
//    G4cout <<"StorkNeutronHPElastic::StorkNeutronHPElastic testit "<<dirName<<G4endl;
    numEle = G4Element::GetNumberOfElements();
    //theElastic = new StorkNeutronHPChannel[numEle];
    //for (G4int i=0; i<numEle; i++)
    //{
    //  theElastic[i].Init((*(G4Element::GetElementTable()))[i], dirName);
    //  while(!theElastic[i].Register(theFS)) ;
    //}
    for ( G4int i = 0 ; i < numEle ; i++ )
    {
       theElastic.push_back( new StorkNeutronHPChannel(fsTemp) );
       (*theElastic[i]).Init((*(G4Element::GetElementTable()))[i], dirName);
       while(!(*theElastic[i]).Register(theFS)) ;
    }
    delete theFS;
    SetMinEnergy(0.*eV);
    SetMaxEnergy(20.*MeV);
  }

  StorkNeutronHPElastic::~StorkNeutronHPElastic()
  {
     //delete [] theElastic;
     for ( std::vector<StorkNeutronHPChannel*>::iterator
           it = theElastic.begin() ; it != theElastic.end() ; it++ )
     {
        delete *it;
     }
     theElastic.clear();
  }

  #include "G4NeutronHPThermalBoost.hh"

  G4HadFinalState * StorkNeutronHPElastic::ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& aNucleus )
  {

    if ( numEle < (G4int)G4Element::GetNumberOfElements() ) addChannelForNewElement();

    G4NeutronHPManager::GetInstance()->OpenReactionWhiteBoard();
    const G4Material * theMaterial = aTrack.GetMaterial();
    G4int n = theMaterial->GetNumberOfElements();
    G4int index = theMaterial->GetElement(0)->GetIndex();
    if(n!=1)
    {
      G4int i;
      xSec = new G4double[n];
      G4double sum=0;
      const G4double * NumAtomsPerVolume = theMaterial->GetVecNbOfAtomsPerVolume();
      G4double rWeight;
      G4NeutronHPThermalBoost aThermalE;
      for (i=0; i<n; i++)
      {
        index = theMaterial->GetElement(i)->GetIndex();
        rWeight = NumAtomsPerVolume[i];
        //xSec[i] = theElastic[index].GetXsec(aThermalE.GetThermalEnergy(aTrack,
        xSec[i] = (*theElastic[index]).GetXsec(aThermalE.GetThermalEnergy(aTrack,
  		                                                     theMaterial->GetElement(i),
  								     std::max(0., theMaterial->GetTemperature()-fsTemp)));
        xSec[i] *= rWeight;
        sum+=xSec[i];
      }
      G4double random = G4UniformRand();
      G4double running = 0;
      for (i=0; i<n; i++)
      {
        running += xSec[i];
        index = theMaterial->GetElement(i)->GetIndex();
        //if(random<=running/sum) break;
        if( sum == 0 || random <= running/sum ) break;
      }
      delete [] xSec;
      // it is element-wise initialised.
    }
    //G4HadFinalState* finalState = theElastic[index].ApplyYourself(aTrack);
    G4HadFinalState* finalState = (*theElastic[index]).ApplyYourself(aTrack);
    if (overrideSuspension) finalState->SetStatusChange(isAlive);

    //Overwrite target parameters
    aNucleus.SetParameters(G4NeutronHPManager::GetInstance()->GetReactionWhiteBoard()->GetTargA(),G4NeutronHPManager::GetInstance()->GetReactionWhiteBoard()->GetTargZ());
    const G4Element* target_element = (*G4Element::GetElementTable())[index];
    const G4Isotope* target_isotope=NULL;
    G4int iele = target_element->GetNumberOfIsotopes();
    for ( G4int j = 0 ; j != iele ; j++ ) {
       target_isotope=target_element->GetIsotope( j );
       if ( target_isotope->GetN() == G4NeutronHPManager::GetInstance()->GetReactionWhiteBoard()->GetTargA() ) break;
    }
    //G4cout << "Target Material of this reaction is " << theMaterial->GetName() << G4endl;
    //G4cout << "Target Element of this reaction is " << target_element->GetName() << G4endl;
    //G4cout << "Target Isotope of this reaction is " << target_isotope->GetName() << G4endl;
    aNucleus.SetIsotope( target_isotope );

    G4NeutronHPManager::GetInstance()->CloseReactionWhiteBoard();
    return finalState;
  }

const std::pair<G4double, G4double> StorkNeutronHPElastic::GetFatalEnergyCheckLevels() const
{
   //return std::pair<G4double, G4double>(10*perCent,10*GeV);
   return std::pair<G4double, G4double>(10*perCent,DBL_MAX);
}

void StorkNeutronHPElastic::addChannelForNewElement()
{
   G4NeutronHPElasticFS* theFS = new G4NeutronHPElasticFS;
   for ( G4int i = numEle ; i < (G4int)G4Element::GetNumberOfElements() ; i++ )
   {
      G4cout << "StorkNeutronHPElastic Prepairing Data for the new element of " << (*(G4Element::GetElementTable()))[i]->GetName() << G4endl;
      theElastic.push_back( new StorkNeutronHPChannel(fsTemp) );
      (*theElastic[i]).Init((*(G4Element::GetElementTable()))[i], dirName);
      while(!(*theElastic[i]).Register(theFS)) ;
   }
   delete theFS;
   numEle = (G4int)G4Element::GetNumberOfElements();
}

G4int StorkNeutronHPElastic::GetVerboseLevel() const
{
   return G4NeutronHPManager::GetInstance()->GetVerboseLevel();
}
void StorkNeutronHPElastic::SetVerboseLevel( G4int newValue )
{
   G4NeutronHPManager::GetInstance()->SetVerboseLevel(newValue);
}
