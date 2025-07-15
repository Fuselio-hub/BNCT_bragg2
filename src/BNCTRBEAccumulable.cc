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
#include "BNCTRBEAccumulable.hh"
#include "BNCTRBE.hh"

#include <tuple>
#include <G4SystemOfUnits.hh>

using namespace std;

BNCTRBEAccumulable::BNCTRBEAccumulable()
    : G4VAccumulable("RBE")
{

}

void BNCTRBEAccumulable::Merge(const G4VAccumulable& rhs)
{
    if (GetVerboseLevel() > 1)
    {
        G4cout << "BNCTRBEAccumulable::Merge()" << G4endl;
    }
    const BNCTRBEAccumulable& other = dynamic_cast<const BNCTRBEAccumulable&>(rhs);
    fAlphaNumerator += other.fAlphaNumerator;
    fDenominator += other.fDenominator;
    fBetaNumerator += other.fBetaNumerator;
    fEnergyDeposit += other.fEnergyDeposit;
}

void BNCTRBEAccumulable::Reset()
{
    if (GetVerboseLevel() > 1)
    {
        G4cout << "BNCTRBEAccumulable::Reset()" << G4endl;
    }
    if (fInitialized)
    {
        fAlphaNumerator = 0.0;
        fBetaNumerator = 0.0;
        fDenominator = 0.0;
        fEnergyDeposit = 0.0;
    }
    else
    {
        Initialize();
    }
}

void BNCTRBEAccumulable::Accumulate(G4double E, G4double energyDeposit, G4double dX, G4int Z, G4int i, G4int j, G4int k)
{
    if (!fInitialized)
    {
        G4Exception("BNCTRBEAccumulable::Accumulate", "NotInitialized", FatalException, "Accumulable not initialized. Must be a programming error.");
    }
    if (GetVerboseLevel() > 2)
    {
        G4cout << "BNCTRBEAccumulable::Accumulate() in " << i << ", " << j << ", " << k << G4endl;
    }
    if (energyDeposit <= 0)
    {
        return;
    }
    size_t n = GetIndex(i, j, k);
    fEnergyDeposit[n] += energyDeposit;

    if ((Z >= 1) && (dX > 0) && (E > 0)) // TODO: Verify this condition
    {
        tuple<G4double, G4double> alpha_beta = BNCTRBE::GetInstance()->GetHitAlphaAndBeta(E, Z);
        fDenominator[n] += energyDeposit;
        fAlphaNumerator[n] += get<0>(alpha_beta) * energyDeposit;
        fBetaNumerator[n] += sqrt(get<1>(alpha_beta)) * energyDeposit;
    }
}

const BNCTRBEAccumulable::array_type BNCTRBEAccumulable::GetEnergyDeposit() const
{
    return fEnergyDeposit;
}

G4int BNCTRBEAccumulable::GetVerboseLevel() const
{
    return BNCTRBE::GetInstance()->GetVerboseLevel();
}

void BNCTRBEAccumulable::Initialize()
{
    if (GetVerboseLevel() > 1)
    {
        G4cout << "BNCTRBEAccumulable::Initialize(): ";
    }
    auto rbe = BNCTRBE::GetInstance();

    fVoxelsAlongX = rbe->GetNumberOfVoxelsAlongX();
    fVoxelsAlongY = rbe->GetNumberOfVoxelsAlongY();
    fVoxelsAlongZ = rbe->GetNumberOfVoxelsAlongZ();
    fVoxels = (size_t) (fVoxelsAlongX * fVoxelsAlongY * fVoxelsAlongZ);

    if (GetVerboseLevel() > 1)
    {
        G4cout << fVoxels << " voxels." << G4endl;
    }

    fAlphaNumerator = array_type(0.0, fVoxels);
    fBetaNumerator = array_type(0.0, fVoxels);
    fDenominator = array_type(0.0, fVoxels);
    fEnergyDeposit = array_type(0.0, fVoxels);
    fInitialized = true;
}
