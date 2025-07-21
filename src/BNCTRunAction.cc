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
// BNCT advanced example for Geant4
// See more at: https://twiki.cern.ch/twiki/bin/view/Geant4/AdvancedExamplesBNCT

#include "BNCTRunAction.hh"
#include "BNCTEventAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "BNCTDetectorConstruction.hh"
#include "G4SDManager.hh"
#include "G4Timer.hh"
#include "BNCTRunAction.hh"
#include "BNCTMatrix.hh"
#include "BNCTLet.hh"
#include "BNCTRBE.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <G4AccumulableManager.hh>

/////////////////////////////////////////////////////////////////////////////
BNCTRunAction::BNCTRunAction()
{
    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->Register(&fRBEAccumulable);
    
}

/////////////////////////////////////////////////////////////////////////////
BNCTRunAction::~BNCTRunAction()
{
}

/////////////////////////////////////////////////////////////////////////////
void BNCTRunAction::BeginOfRunAction(const G4Run* aRun)
{   
    auto analysisManager = G4AnalysisManager::Instance();
    
    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->Reset();

    G4RunManager::GetRunManager()-> SetRandomNumberStore(true);
    G4cout << "Run " << aRun -> GetRunID() << " starts ..." << G4endl;

    BNCTRBE *rbe = BNCTRBE::GetInstance();
    if (rbe->IsCalculationEnabled() && IsMaster() && rbe->GetVerboseLevel() > 0)
    {
        rbe->PrintParameters();
    }
    
    electromagnetic = 0;
    hadronic = 0;

    // per spettro neutroni
    G4String filename = "neutron_spectrum.root";
    analysisManager->OpenFile(filename);

    analysisManager -> CreateNtuple("BNCT", "BNCT");
    analysisManager -> CreateNtupleDColumn("kineticEnergy"); 
    analysisManager -> CreateNtupleIColumn("particleID");

    analysisManager -> FinishNtuple();
    
}

/////////////////////////////////////////////////////////////////////////////
void BNCTRunAction::EndOfRunAction(const G4Run*)
{
    auto analysisManager = G4AnalysisManager::Instance();

    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->Merge();
    if(IsMaster()){
        if ( BNCTMatrix * pMatrix = BNCTMatrix::GetInstance() )
        {
            // pMatrix -> TotalEnergyDeposit();
            pMatrix -> StoreDoseFluenceAscii();
            
        }
        if (BNCTLet *let = BNCTLet::GetInstance())
            if(let -> doCalculation)
            {
                let -> LetOutput();     // Calculate let
                let -> StoreLetAscii(); // Store it
            }
        // Tell the RBE class what we have accumulated...
        BNCTRBE *rbe = BNCTRBE::GetInstance();
        if (rbe->IsCalculationEnabled())
        {
            if (rbe->IsAccumulationEnabled())
            {
                rbe->AddAlphaNumerator(fRBEAccumulable.GetAlphaNumerator());
                rbe->AddBetaNumerator(fRBEAccumulable.GetBetaNumerator());
                rbe->AddDenominator(fRBEAccumulable.GetDenominator());
                rbe->AddEnergyDeposit(fRBEAccumulable.GetEnergyDeposit());
            }
            else
            {
                rbe->SetAlphaNumerator(fRBEAccumulable.GetAlphaNumerator());
                rbe->SetBetaNumerator(fRBEAccumulable.GetBetaNumerator());
                rbe->SetDenominator(fRBEAccumulable.GetDenominator());
                rbe->SetEnergyDeposit(fRBEAccumulable.GetEnergyDeposit());
            }
            
            rbe->StoreAlphaAndBeta();
            rbe->StoreRBE();
        }
    }
    if (analysisManager->IsOpenFile()) {
      analysisManager->Write();
      analysisManager->CloseFile();
    }
}
/////////////////////////////////////////////////////////////////////////////
void BNCTRunAction::AddEMProcess()
{
    electromagnetic += 1;
}

/////////////////////////////////////////////////////////////////////////////
void BNCTRunAction::AddHadronicProcess()
{
    hadronic += 1;
}
