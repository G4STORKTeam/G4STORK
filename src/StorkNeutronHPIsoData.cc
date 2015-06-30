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
//080901 Avoiding troubles which caused by G4PhysicsVecotor of length 0 by T. Koi
//
#include "StorkNeutronHPIsoData.hh"
#include "G4NeutronHPManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4NeutronHPDataUsed.hh"

  //G4bool StorkNeutronHPIsoData::Init(G4int A, G4int Z, G4double abun, G4String dirName, G4String aFSType)
  G4bool StorkNeutronHPIsoData::Init(G4int A, G4int Z, G4int M, G4double abun, G4String dirName, G4String aFSType)
  {
    theChannelData = 0;

    G4double abundance = abun/100.;
    G4String filename;
    G4bool result = true;
    //G4NeutronHPDataUsed aFile = theNames.GetName(A, Z, dirName, aFSType, result);
    G4NeutronHPDataUsed aFile = theNames.GetName(A, Z, M, dirName, aFSType, result);
    filename = aFile.GetName();
    //G4cout << "File name: " << filename << " for Z: " << Z << " and A: " << A << endl;
//    if(filename=="") return false;
   //std::ifstream theChannel(filename);
   std::istringstream theChannel(filename,std::ios::in);
   G4NeutronHPManager::GetInstance()->GetDataStream(filename,theChannel);

    if(Z==1 && (aFile.GetZ()!=Z || std::abs(aFile.GetA()-A)>0.0001) )
    {
      if(getenv("NeutronHPNamesLogging")) G4cout << "Skipped = "<< filename <<" "<<A<<" "<<Z<<G4endl;
      //080901 TKDB No more necessary below protection, cross sections set to 0 in StorkNeutronHPNames
      //And below two lines causes trouble with G4PhysicsVector
      //theChannel.close();
      //return false;
    }
    if(!theChannel) {/*theChannel.close()*/; return false;}
    // accommodating deficiencie of some compilers
    if(theChannel.eof()) {/*theChannel.close()*/; return false;}
    if(!theChannel) {/*theChannel.close()*/; return false;}
    G4int dummy;
    theChannel >> dummy >> dummy;
    theChannelData = new G4NeutronHPVector;
    G4int nData;
    theChannel >> nData;
    theChannelData->Init(theChannel, nData, eV, abundance*barn);
//    G4cout << "Channel Data Statistics: "<<theChannelData->GetVectorLength()<<G4endl;
//    G4cout << "Channel data"<<G4endl;
//     G4int hpw;
//     G4cin >> hpw;
//    theChannelData->Dump();
    //theChannel.close();
    return result;
  }

  //void StorkNeutronHPIsoData::Init(G4int A, G4int Z, G4double abun) //fill PhysicsVector for this Isotope
  void StorkNeutronHPIsoData::Init(G4int A, G4int Z, G4int M, G4double abun, G4String dirName) //fill PhysicsVector for this Isotope
  {
    G4String baseName;
    if (dirName=="DEFAULT")
    {
        if(!getenv("G4NEUTRONHPDATA"))
           throw G4HadronicException(__FILE__, __LINE__, "Please setenv G4NEUTRONHPDATA to point to the neutron cross-section files.");
        baseName = getenv("G4NEUTRONHPDATA");
        dirName = baseName+"/Fission";
    }
    else
    {
        baseName = dirName;
        dirName = baseName+"/Fission";
    }
    //if(Z>89)
    if(Z>87) //TK Modifed for ENDF VII.0
    {
      //Init(A, Z, abun, dirName, "/CrossSection/");
      Init(A, Z, M, abun, dirName, "/CrossSection");
    }
    else
    {
       theChannelData = new G4NeutronHPVector;
    }
    theFissionData = theChannelData;
    theChannelData = 0; // fast fix for double delete; revisit later. @@@@@@@
    dirName = baseName+"/Capture";
    //Init(A, Z, abun, dirName, "/CrossSection/");
    Init(A, Z, M, abun, dirName, "/CrossSection");
    theCaptureData = theChannelData;
    theChannelData = 0;
    dirName = baseName+"/Elastic";
    //Init(A, Z, abun, dirName, "/CrossSection/");
    Init(A, Z, M, abun, dirName, "/CrossSection");
    theElasticData = theChannelData;
    theChannelData = 0;
    dirName = baseName+"/Inelastic";
    //Init(A, Z, abun, dirName, "/CrossSection/");
    Init(A, Z, M, abun, dirName, "/CrossSection");
    theInelasticData = theChannelData;
    theChannelData = 0;

//    if(theInelasticData!=0) G4cout << "Inelastic Data Statistics: "<<theInelasticData->GetVectorLength()<<G4endl;
//    if(theElasticData!=0) G4cout << "Elastic Data Statistics: "<<theElasticData->GetVectorLength()<<G4endl;
//    if(theCaptureData!=0) G4cout << "Capture Data Statistics: "<<theCaptureData->GetVectorLength()<<G4endl;
//    if(theFissionData!=0) G4cout << "Fission Data Statistics: "<<theFissionData->GetVectorLength()<<G4endl;
//  G4cout << "Inelastic data"<<G4endl;
//  if(theInelasticData!=0) theInelasticData->Dump();
//  G4cout << "Elastic data"<<G4endl;
//  if(theElasticData!=0) theElasticData->Dump();
//  G4cout << "Capture data"<<G4endl;
//  if(theCaptureData!=0) theCaptureData->Dump();
//  G4cout << "Fission data"<<G4endl;
//  if(theFissionData!=0) theFissionData->Dump();

  }

  G4bool StorkNeutronHPIsoData::FileExists(G4int A, G4int Z, G4String DirName )
  {
    G4String processes[4] = {"/Inelastic", "/Elastic", "/Capture", "/Fission"};
    G4String filename;
    ElementNames elemNames;
    std::stringstream stream;
    std::ifstream *in;

    for(G4int i =0; i<4; i++)
    {
        if(!((Z<88)&&(i==3)))
        {
            stream.str("");
            stream.clear();
            stream << DirName << processes[i] << "/CrossSection/" << Z << "_" << A << "_" << elemNames.GetName(Z);
            filename = stream.str();

            in = new std::ifstream ( filename.c_str() );
            if(!(in->good()))
            {
                in->close();
                delete in;
                filename+=".z";
                in = new std::ifstream ( filename.c_str() , std::ios::binary );
                if(!(in->good()))
                {
                    in->close();
                    delete in;
                    stream.str("");
                    stream.clear();
                    stream << DirName << processes[i] << "/CrossSection/" << Z << "_" << "nat" << "_" << elemNames.GetName(Z);
                    filename = stream.str();

                    in = new std::ifstream ( filename.c_str() );
                    if(!(in->good()))
                    {
                        in->close();
                        delete in;
                        filename+=".z";
                        in = new std::ifstream ( filename.c_str() , std::ios::binary );
                        if(!(in->good()))
                        {
                            in->close();
                            delete in;
                            return false;
                        }
                        else
                        {
                            in->close();
                            delete in;
                        }
                    }
                    else
                    {
                        in->close();
                        delete in;
                    }

                }
                else
                {
                    in->close();
                    delete in;
                }
            }
            else
            {
                in->close();
                delete in;

            }

        }
    }

    return true;
  }

