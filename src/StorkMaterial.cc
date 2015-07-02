
#include <iomanip>

#include "StorkMaterial.hh"
#include "G4NistManager.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Constructor to create a material from scratch


StorkMaterial::StorkMaterial(const G4String& name, G4double z,
                       G4double a, G4double density,
                       G4State state, G4double temp, G4double pressure)
  : G4Material(name, z, a, density, state, temp, pressure)
{
    G4int index;
    G4Material* mat = dynamic_cast<G4Material*>(this);
    G4ElementVector* elemVec = const_cast<G4ElementVector*>(mat->GetElementVector());

    (*elemVec)[0]->GetElementTable()->pop_back();

    StorkElement* elem = new StorkElement(*((*elemVec)[0]));
    delete (*elemVec)[0];
    //When a G4Element object is deleted it sets the pointer in the G4ElementTable in the index that the element was added to when it was created to zero, so we must reAdd the new StorkElement here
    elem->GetElementTable()->back()=elem;
    elem->SetTemperature(-1);

    if(elem->Exists(temp,index))
    {
        (*elemVec)[0]=(*((*elemVec)[0]->GetElementTable()))[index];
        delete elem;
        (*elemVec)[0]->GetElementTable()->pop_back();
    }
    else
    {
        elem->SetTemperature(temp);
        std::stringstream ss;
        ss.str("");
        ss<<'T'<<elem->GetTemperature()<<'k';
        G4String elemName = name+ss.str();
        elem->SetName(elemName);
        (*elemVec)[0]=elem;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Constructor to create a material from a List of constituents
// (elements and/or materials)  added with AddElement or AddMaterial

StorkMaterial::StorkMaterial(const G4String& name, G4double density,
                       G4int nComponents,
                       G4State state, G4double temp, G4double pressure)
  : G4Material(name, density, nComponents, state, temp, pressure)
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Constructor to create a material from base material

StorkMaterial::StorkMaterial(const G4String& name, G4double density,
                       StorkMaterial* bmat,
                       G4State state, G4double temp, G4double pressure)
  : G4Material(name, density, bmat->GetMaxNbComponents(), state, temp, pressure)
{
    AddMaterial(bmat, 1.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Fake default constructor - sets only member data and allocates memory
//                            for usage restricted to object persistency

StorkMaterial::StorkMaterial(__void__& fake)
  : G4Material(fake)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StorkMaterial::~StorkMaterial()
{
  //  G4cout << "### Destruction of material " << fName << " started" <<G4endl;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// AddElement -- composition by atom count

void StorkMaterial::AddElement(StorkElement* element, G4int nAtoms)
{
    G4int index;
    G4Material* mat = dynamic_cast<G4Material*>(this);

    if(element->GetTemperature()<0.)
    {
        std::stringstream ss;
        ss.str("");
        ss<<'T'<<GetTemperature()<<'k';
        element->SetTemperature(GetTemperature());
        element->SetName(element->GetName()+ss.str());
        mat->AddElement(dynamic_cast<G4Element*>(element), nAtoms);
    }
    else if(element->Exists(GetTemperature(),index))
    {
        mat->AddElement(((*(element->GetElementTable()))[index]), nAtoms);
    }
    else
    {
        std::stringstream ss;
        ss.str("");
        ss<<'T'<<element->GetTemperature()<<'k';
        G4String elemName = element->GetName(), check;
        int pos=elemName.find_last_of('T'), pos2=elemName.find_last_of('k');

        if((pos>0)&&(pos2>0)&&(pos2>pos))
            check= elemName.substr(pos, pos2-pos+1);

        StorkElement *newElem = new StorkElement(*element);

        if(check==G4String(ss.str()))
        {
            ss.str("");
            ss.clear();
            ss<<'T'<<GetTemperature()<<'k';
            newElem->SetName(elemName.substr(0, elemName.find_last_of('T'))+ss.str());
        }
        else
        {
            ss.str("");
            ss.clear();
            ss<<'T'<<GetTemperature()<<'k';
            newElem->SetName(element->GetName()+ss.str());
        }
        newElem->SetTemperature(GetTemperature());
        mat->AddElement(dynamic_cast<G4Element*>(newElem), nAtoms);
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// AddElement -- composition by fraction of mass

void StorkMaterial::AddElement(StorkElement* element, G4double fraction)
{
    G4Material* mat = dynamic_cast<G4Material*>(this);
  // filling ...
    G4int index;
    if(element->GetTemperature()<0.)
    {
        element->SetTemperature(GetTemperature());
        std::stringstream elemName;
        elemName << element->GetName() << 'T' << element->GetTemperature() << 'k';
        element->SetName(elemName.str());
        mat->AddElement(dynamic_cast<G4Element*>(element), fraction);
    }
    else if(element->Exists(GetTemperature(),index))
    {
        mat->AddElement(((*(element->GetElementTable()))[index]), fraction);
    }
    else
    {
        std::stringstream ss;
        ss.str("");
        ss<<'T'<<element->GetTemperature()<<'k';
        G4String elemName = element->GetName(), check;
        int pos=elemName.find_last_of('T'), pos2=elemName.find_last_of('k');

        if((pos>0)&&(pos2>0)&&(pos2>pos))
            check= elemName.substr(pos, pos2-pos+1);

        StorkElement *newElem = new StorkElement(*element);

        if(check==G4String(ss.str()))
        {
            ss.str("");
            ss.clear();
            ss<<'T'<<GetTemperature()<<'k';
            newElem->SetName(elemName.substr(0, elemName.find_last_of('T'))+ss.str());
        }
        else
        {
            ss.str("");
            ss.clear();
            ss<<'T'<<GetTemperature()<<'k';
            newElem->SetName(element->GetName()+ss.str());
        }
        newElem->SetTemperature(GetTemperature());
        mat->AddElement(dynamic_cast<G4Element*>(newElem), fraction);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// AddMaterial -- composition by fraction of mass

void StorkMaterial::AddMaterial(StorkMaterial* material, G4double fraction)
{

G4int index;

G4Material* mat = dynamic_cast<G4Material*>(this);
G4Material* mat2 = dynamic_cast<G4Material*>(material);
G4String name = mat->GetName()+"-"+mat2->GetName();
G4Material* temp = new G4Material(name, mat2->GetDensity(), mat2->GetNumberOfElements(), mat2->GetState(), mat2->GetTemperature(), mat2->GetPressure());
const G4double* fracVec = mat2->GetFractionVector();

for (size_t elm=0; elm<mat2->GetNumberOfElements(); ++elm)
{
    StorkElement* element = static_cast<StorkElement*>((*(mat2->GetElementVector()))[elm]);
    if(element->Exists(GetTemperature(), index))
    {
        temp->AddElement(((*(element->GetElementTable()))[index]), fracVec[elm]);
    }
    else
    {
        std::stringstream ss;
        ss.str("");
        ss<<'T'<<element->GetTemperature()<<'k';
        G4String name = element->GetName(), check;
        int pos=name.find_last_of('T'), pos2=name.find_last_of('k');

        if((pos>0)&&(pos2>0)&&(pos2>pos))
            check= name.substr(pos, pos2-pos+1);

        StorkElement *newElem = new StorkElement(*element);

        if(check==G4String(ss.str()))
        {
            ss.str("");
            ss.clear();
            ss<<'T'<<GetTemperature()<<'k';
            newElem->SetName(name.substr(0, name.find_last_of('T'))+ss.str());
        }
        else
        {
            ss.str("");
            ss.clear();
            ss<<'T'<<GetTemperature()<<'k';
            newElem->SetName(element->GetName()+ss.str());
        }
        newElem->SetTemperature(GetTemperature());
        temp->AddElement(dynamic_cast<G4Element*>(newElem), fracVec[elm]);
    }
}

mat->AddMaterial(temp, fraction);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StorkMaterial::StorkMaterial(StorkMaterial& right): G4Material(right.GetName(), right.GetDensity(),
                       right.GetNumberOfElements(), right.GetState(), right.GetTemperature(), right.GetPressure())
{
    G4ElementVector* elemVec = right.GetElementVector();
    G4double* fracVec = right.GetFractionVector();
    for(G4int i=0; i<right.GetNumberOfElements(); i++)
    {
        AddElement(dynamic_cast<StorkElement*>((*elemVec)[i]), fracVec[i]);
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const StorkMaterial& StorkMaterial::operator=(StorkMaterial& right)
{
  if (this != &right)
    {
        //this may leak memory
        this->~StorkMaterial();
        G4cout << "### The StorkMaterial this pointer points to " << this << " ###" << G4endl;
        StorkMaterial(right.GetName(), right.GetDensity(), right.GetNumberOfElements(), right.GetState(), right.GetTemperature(), right.GetPressure());
        G4cout << "### The StorkMaterial this pointer points to " << this << " ###" << G4endl;
        G4cout << "### The new material name is " << this->GetName() << " ###" << G4endl;
        G4ElementVector* elemVec = right.GetElementVector();
        G4double* fracVec = right.GetFractionVector();
        for(G4int i=0; i<right.GetNumberOfElements(); i++)
        {
            AddElement(dynamic_cast<StorkElement*>((*elemVec)[i]), fracVec[i]);
        }
    }
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int StorkMaterial::operator==(const StorkMaterial& right) const
{
  return (this == (StorkMaterial *) &right);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int StorkMaterial::operator!=(const StorkMaterial& right) const
{
  return (this != (StorkMaterial *) &right);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*

std::ostream& operator<<(std::ostream& flux, StorkMaterial* material)
{
  std::ios::fmtflags mode = flux.flags();
  flux.setf(std::ios::fixed,std::ios::floatfield);
  G4long prec = flux.precision(3);

  flux
    << " Material: "         << std::setw(8) <<  material->fName
    << " " << material->fChemicalFormula << " "
    << "  density: "         << std::setw(6) << std::setprecision(3)
    << G4BestUnit(material->fDensity,"Volumic Mass")
    << "  RadL: "            << std::setw(7)  << std::setprecision(3)
    << G4BestUnit(material->fRadlen,"Length")
    << "  Nucl.Int.Length: " << std::setw(7)  << std::setprecision(3)
    << G4BestUnit(material->fNuclInterLen,"Length") <<"\n" << std::setw(30)
    << "  Imean: "           << std::setw(7)  << std::setprecision(3)
    << G4BestUnit(material->GetIonisation()->GetMeanExcitationEnergy(),"Energy");

  if(material->fState == kStateGas) {
    flux
      << "  temperature: " << std::setw(6) << std::setprecision(2)
      << (material->GetTemperature())/kelvin << " K"
      << "  pressure: "    << std::setw(6) << std::setprecision(2)
      << (material->fPressure)/atmosphere << " atm";
  }
  flux << "\n";

  for (size_t i=0; i<material->fNumberOfElements; i++) {
    flux
      << "\n   ---> " << (*(material->theElementVector))[i]
      << "\n          ElmMassFraction: "
      << std::setw(6)<< std::setprecision(2)
      << (material->fMassFractionVector[i])/perCent << " %"
      << "  ElmAbundance "     << std::setw(6)<< std::setprecision(2)
      << 100*(material->VecNbOfAtomsPerVolume[i])
      /(material->TotNbOfAtomsPerVolume)
      << " % \n";
  }
  flux.precision(prec);
  flux.setf(mode,std::ios::floatfield);

  return flux;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::ostream& operator<<(std::ostream& flux, StorkMaterial& material)
{
  flux << &material;
  return flux;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::ostream& operator<<(std::ostream& flux, G4MaterialTable MaterialTable)
{
  //Dump info for all known materials
  flux << "\n***** Table : Nb of materials = " << MaterialTable.size()
       << " *****\n" << G4endl;

  for (size_t i=0; i<MaterialTable.size(); ++i) {
    flux << MaterialTable[i] << G4endl << G4endl;
  }

  return flux;
}
*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
