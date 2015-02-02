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
#ifndef StorkHadProjectile_hh
#define StorkHadProjectile_hh

#include "globals.hh"
#include "G4Material.hh"
#include "G4ParticleDefinition.hh"
#include "G4LorentzVector.hh"
#include "G4LorentzVector.hh"
#include "G4LorentzRotation.hh"
#include "G4HadProjectile.hh"

class G4Track;
class G4DynamicParticle;

class StorkHadProjectile:G4HadProjectile
{
public:
  StorkHadProjectile();
  StorkHadProjectile(const G4Track &aT);
  StorkHadProjectile(const G4DynamicParticle &aT, const G4Material *mat);
  ~StorkHadProjectile();

  void Initialise(const G4Track &aT);

  inline const G4Material * GetMaterial() const;
  inline const G4ParticleDefinition * GetDefinition() const;
  inline const G4LorentzVector & Get4Momentum() const;
  inline G4LorentzRotation & GetTrafoToLab();
  inline G4double GetKineticEnergy() const;
  inline G4double GetTotalEnergy() const;
  inline G4double GetTotalMomentum() const;
  inline G4double GetGlobalTime() const;
  inline G4double GetBoundEnergy() const;
  inline void SetGlobalTime(G4double t);
  inline void SetBoundEnergy(G4double e);

private:

  // hide assignment operator as private
  StorkHadProjectile& operator=(const StorkHadProjectile &right);
  StorkHadProjectile(const StorkHadProjectile& );

  const G4Material * theMat;
  G4LorentzVector theOrgMom;
  G4LorentzVector theMom;
  const G4ParticleDefinition * theDef;
  G4LorentzRotation toLabFrame;
  G4double theTime;
  G4double theBoundEnergy;
};

const G4Material * StorkHadProjectile::GetMaterial() const
{
  return theMat;
}

const G4ParticleDefinition * StorkHadProjectile::GetDefinition() const
{
  return theDef;
}

inline const G4LorentzVector& StorkHadProjectile::Get4Momentum() const
{
  return theMom;
}

inline G4LorentzRotation& StorkHadProjectile::GetTrafoToLab()
{
  return toLabFrame;
}

G4double StorkHadProjectile::GetTotalEnergy() const
{
  return Get4Momentum().e();
}

G4double StorkHadProjectile::GetTotalMomentum() const
{
  return Get4Momentum().vect().mag();
}

G4double StorkHadProjectile::GetKineticEnergy() const
{
  G4double ekin = GetTotalEnergy() - GetDefinition()->GetPDGMass();
  if(ekin < 0.0) { ekin = 0.0; }
  return ekin;
}

inline G4double StorkHadProjectile::GetGlobalTime() const
{
  return theTime;
}

inline G4double StorkHadProjectile::GetBoundEnergy() const
{
  return theBoundEnergy;
}

inline void StorkHadProjectile::SetGlobalTime(G4double t)
{
  theTime = t;
}

inline void StorkHadProjectile::SetBoundEnergy(G4double e)
{
  theBoundEnergy = e;
}

#endif
