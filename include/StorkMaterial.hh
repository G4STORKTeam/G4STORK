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
//
// $Id: StorkMaterial.hh 68070 2013-03-13 15:03:06Z gcosmo $
//

//---------------------------------------------------------------------------
//
// ClassName:   StorkMaterial
//
// Description: Contains material properties
//
// Class description:
//
// Is used to define the material composition of Geant4 volumes.
// A StorkMaterial is always made of StorkElements. It should has the name,
// the list of StorkElements, material density, material state, temperature,
// pressure. Other parameters are optional and may be set by the user code
// or computed at initialisation.
//
// There is several ways to construct StorkMaterial:
//   - from single element;
//   - from a list of components (elements or other materials);
//   - from internal Geant4 database of materials
//
// A collection of constituent Elements/Materials should be defined
// with specified weights by fractional mass or atom counts (only for Elements).
//
// Quantities, with physical meaning or not, which are constant in a given
// material are computed and stored here as Derived data members.
//
// The class contains as a private static member the Table of defined
// materials (an ordered vector of materials).
//
// It is strongly not recommended to delete materials in user code.
// All materials will be deleted automatically at the end of Geant4 session.
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// 10-07-96, new data members added by L.Urban
// 12-12-96, new data members added by L.Urban
// 20-01-97, aesthetic rearrangement. RadLength calculation modified
//           Data members Zeff and Aeff REMOVED (i.e. passed to the Elements).
//           (local definition of Zeff in DensityEffect and FluctModel...)
//           Vacuum defined as a G4State. Mixture flag removed, M.Maire
// 29-01-97, State=Vacuum automatically set density=0 in the contructors.
//           Subsequent protections have been put in the calculation of
//           MeanExcEnergy, ShellCorrectionVector, DensityEffect, M.Maire
// 20-03-97, corrected initialization of pointers, M.Maire
// 10-06-97, new data member added by V.Grichine (fSandiaPhotoAbsCof)
// 27-06-97, new function GetElement(int), M.Maire
// 24-02-98, fFractionVector become fMassFractionVector
// 28-05-98, kState=kVacuum removed:
//           The vacuum is an ordinary gas vith very low density, M.Maire
// 12-06-98, new method AddMaterial() allowing mixture of materials, M.Maire
// 09-07-98, Ionisation parameters removed from the class, M.Maire
// 04-08-98, new method GetMaterial(materialName), M.Maire
// 05-10-98, change name: NumDensity -> NbOfAtomsPerVolume
// 18-11-98, SandiaTable interface modified.
// 19-07-99, new data member (chemicalFormula) added by V.Ivanchenko
// 12-03-01, G4bool fImplicitElement (mma)
// 30-03-01, suppression of the warning message in GetMaterial
// 17-07-01, migration to STL. M. Verderi.
// 14-09-01, Suppression of the data member fIndexInTable
// 31-10-01, new function SetChemicalFormula() (mma)
// 26-02-02, fIndexInTable renewed
// 06-08-02, remove constructors with ChemicalFormula (mma)
// 15-11-05, GetMaterial(materialName, G4bool warning=true)
// 13-04-12, std::map<StorkMaterial*,G4double> fMatComponents (mma)
// 21-04-12, fMassOfMolecule (mma)

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef StorkMaterial_HH
#define StorkMaterial_HH 1

#include <sstream>

#include "StorkElement.hh"
#include "G4Material.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class StorkMaterial : public G4Material
{
public:  // with description
  //
  // Constructor to create a material from single element
  //
  StorkMaterial(const G4String& name,				//its name
                   G4double  z, 				//atomic number
                   G4double  a,					//mass of mole
                   G4double  density, 				//density
                   G4State   state    = kStateUndefined,	//solid,gas
                   G4double  temp     = CLHEP::STP_Temperature,	//temperature
                   G4double  pressure = CLHEP::STP_Pressure);	//pressure

  //
  // Constructor to create a material from a combination of elements
  // and/or materials subsequently added via AddElement and/or AddMaterial
  //
  StorkMaterial(const G4String& name,				//its name
                   G4double  density, 				//density
                   G4int     nComponents,			//nbOfComponents
                   G4State   state    = kStateUndefined,	//solid,gas
                   G4double  temp     = CLHEP::STP_Temperature,	//temperature
                   G4double  pressure = CLHEP::STP_Pressure);	//pressure

  //
  // Constructor to create a material from the base material
  //
  StorkMaterial(const G4String& name,				//its name
                   G4double  density, 				//density
                    StorkMaterial* baseMaterial,			//base material
                   G4State   state    = kStateUndefined,	//solid,gas
                   G4double  temp     = CLHEP::STP_Temperature,	//temperature
                   G4double  pressure = CLHEP::STP_Pressure);	//pressure

  //
  // Add an element, giving number of atoms
  //
  void AddElement(StorkElement* element,				//the element
                  G4int      nAtoms);				//nb of atoms in
		    						// a molecule
  //
  // Add an element or material, giving fraction of mass
  //
  void AddElement (StorkElement* element ,				//the element
                   G4double   fraction);			//fractionOfMass

  void AddMaterial(StorkMaterial* material,			//the material
                   G4double   fraction);			//fractionOfMass

  ~StorkMaterial();
  //
  //printing methods
  //
  /*
  friend std::ostream& operator<<(std::ostream&, StorkMaterial*);
  friend std::ostream& operator<<(std::ostream&, StorkMaterial&);
  friend std::ostream& operator<<(std::ostream&, G4MaterialTable);
  */

public:  // without description

  G4int operator==(const StorkMaterial&) const;
  G4int operator!=(const StorkMaterial&) const;
  StorkMaterial(__void__&);
  // Fake default constructor for usage restricted to direct object
    // persistency for clients requiring preallocation of memory for
    // persistifiable objects.


  void SetTemperature(G4double matTemp, G4bool UpdateElemTemp);
  StorkMaterial(StorkMaterial&);
  const StorkMaterial& operator=(StorkMaterial&);

private:

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
