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
//
#include "StorkNeutronHPCapture.hh"
#include "G4NeutronHPManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4NeutronHPCaptureFS.hh"
#include "G4NeutronHPDeExGammas.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"

  StorkNeutronHPCapture::StorkNeutronHPCapture(G4double finalStateTemp)
   :G4HadronicInteraction("NeutronHPCapture")
  {
    SetMinEnergy( 0.0 );
    SetMaxEnergy( 20.*MeV );
    fsTemp = finalStateTemp;
//    G4cout << "Capture : start of construction!!!!!!!!"<<G4endl;
    if(!getenv("G4NEUTRONHPDATA"))
       throw G4HadronicException(__FILE__, __LINE__, "Please setenv G4NEUTRONHPDATA to point to the neutron cross-section files.");
    dirName = getenv("G4NEUTRONHPDATA");
    G4String tString = "/Capture";
    dirName = dirName + tString;
    numEle = G4Element::GetNumberOfElements();
//    G4cout << "+++++++++++++++++++++++++++++++++++++++++++++++++"<<G4endl;
//    G4cout <<"Disname="<<dirName<<" numEle="<<numEle<<G4endl;
    //theCapture = new StorkNeutronHPChannel[numEle];
//    G4cout <<"StorkNeutronHPChannel constructed"<<G4endl;
    G4NeutronHPCaptureFS * theFS = new G4NeutronHPCaptureFS;
    //for (G4int i=0; i<numEle; i++)
    //{
//  //    G4cout << "initializing theCapture "<<i<<" "<< numEle<<G4endl;
    //  theCapture[i].Init((*(G4Element::GetElementTable()))[i], dirName);
    //  theCapture[i].Register(theFS);
    //}
    for ( G4int i = 0 ; i < numEle ; i++ )
    {
       theCapture.push_back( new StorkNeutronHPChannel(fsTemp) );
       (*theCapture[i]).Init((*(G4Element::GetElementTable()))[i], dirName);
       (*theCapture[i]).Register(theFS);
    }
    delete theFS;
//    G4cout << "-------------------------------------------------"<<G4endl;
//    G4cout << "Leaving StorkNeutronHPCapture::StorkNeutronHPCapture"<<G4endl;
  }

  StorkNeutronHPCapture::~StorkNeutronHPCapture()
  {
    //delete [] theCapture;
//    G4cout << "Leaving StorkNeutronHPCapture::~StorkNeutronHPCapture"<<G4endl;
     for ( std::vector<StorkNeutronHPChannel*>::iterator
           ite = theCapture.begin() ; ite != theCapture.end() ; ite++ )
     {
        delete *ite;
     }
     theCapture.clear();
  }

  #include "G4NeutronHPThermalBoost.hh"
  G4HadFinalState * StorkNeutronHPCapture::ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& aNucleus )
  {

    if ( numEle < (G4int)G4Element::GetNumberOfElements() ) addChannelForNewElement();

    G4NeutronHPManager::GetInstance()->OpenReactionWhiteBoard();
    if(getenv("NeutronHPCapture")) G4cout <<" ####### StorkNeutronHPCapture called"<<G4endl;
    const G4Material * theMaterial = aTrack.GetMaterial();
    G4int n = theMaterial->GetNumberOfElements();
    G4int index = theMaterial->GetElement(0)->GetIndex();
    if(n!=1)
    {
      xSec = new G4double[n];
      G4double sum=0;
      G4int i;
      const G4double * NumAtomsPerVolume = theMaterial->GetVecNbOfAtomsPerVolume();
      G4double rWeight;
      G4NeutronHPThermalBoost aThermalE;
      for (i=0; i<n; i++)
      {
        index = theMaterial->GetElement(i)->GetIndex();
        rWeight = NumAtomsPerVolume[i];
        //xSec[i] = theCapture[index].GetXsec(aThermalE.GetThermalEnergy(aTrack,
        xSec[i] = (*theCapture[index]).GetXsec(aThermalE.GetThermalEnergy(aTrack,
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
      if(i==n) i=std::max(0, n-1);
      delete [] xSec;
    }

    //return theCapture[index].ApplyYourself(aTrack);
    //G4HadFinalState* result = theCapture[index].ApplyYourself(aTrack);
    G4HadFinalState* result = (*theCapture[index]).ApplyYourself(aTrack);

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
    return result;
  }

const std::pair<G4double, G4double> StorkNeutronHPCapture::GetFatalEnergyCheckLevels() const
{
   //return std::pair<G4double, G4double>(10*perCent,10*GeV);
   return std::pair<G4double, G4double>(10*perCent,DBL_MAX);
}

void StorkNeutronHPCapture::addChannelForNewElement()
{
   G4NeutronHPCaptureFS* theFS = new G4NeutronHPCaptureFS;
   for ( G4int i = numEle ; i < (G4int)G4Element::GetNumberOfElements() ; i++ )
   {
      G4cout << "StorkNeutronHPCapture Prepairing Data for the new element of " << (*(G4Element::GetElementTable()))[i]->GetName() << G4endl;
      theCapture.push_back( new StorkNeutronHPChannel(fsTemp) );
      (*theCapture[i]).Init((*(G4Element::GetElementTable()))[i], dirName);
      (*theCapture[i]).Register(theFS);
   }
   delete theFS;
   numEle = (G4int)G4Element::GetNumberOfElements();
}

G4int StorkNeutronHPCapture::GetVerboseLevel() const
{
   return G4NeutronHPManager::GetInstance()->GetVerboseLevel();
}
void StorkNeutronHPCapture::SetVerboseLevel( G4int newValue )
{
   G4NeutronHPManager::GetInstance()->SetVerboseLevel(newValue);
}
