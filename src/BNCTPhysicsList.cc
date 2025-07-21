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
//
//
//    ******      SUGGESTED PHYSICS FOR ACCURATE SIMULATIONS    *********
//    ******            IN MEDICAL PHYSICS APPLICATIONS         *********
//
// 'HADRONTHERAPY_1' and 'HADRONTHERAPY_2' are both suggested;
// It can be activated inside any macro file using the command:
// /Physics/addPhysics HADRONTHERAPY_1 (HADRONTHERAPY_2)
#include "BNCTHadronElasticPhysicsHP.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "BNCTPhysicsList.hh"
#include "BNCTPhysicsListMessenger.hh"
#include "BNCTStepMax.hh"
#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4LossTableManager.hh"
#include "G4UnitsTable.hh"
#include "G4ProcessManager.hh"
#include "G4IonFluctuations.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4AutoDelete.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_AllHP.hh"

/////////////////////////////////////////////////////////////////////////////
BNCTPhysicsList::BNCTPhysicsList() : G4VModularPhysicsList()
{
    G4LossTableManager::Instance();
    defaultCutValue = 1.*mm;
    cutForGamma     = defaultCutValue;
    cutForElectron  = defaultCutValue;
    cutForPositron  = defaultCutValue;
    
    pMessenger = new BNCTPhysicsListMessenger(this);  // prende il comando della macro
    SetVerboseLevel(1);
    decay_List = new G4DecayPhysics();
    // Elecromagnetic physics
    //
    emPhysicsList = new G4EmStandardPhysics_option4();
    
}

/////////////////////////////////////////////////////////////////////////////
BNCTPhysicsList::~BNCTPhysicsList()
{
    delete pMessenger;
    delete emPhysicsList;
    delete decay_List;
    //delete radioactiveDecay_List;
    hadronPhys.clear();
    for(size_t i=0; i<hadronPhys.size(); i++)
    {
        delete hadronPhys[i];
    }
}

/////////////////////////////////////////////////////////////////////////////
void BNCTPhysicsList::ConstructParticle()
{
    decay_List -> ConstructParticle(); 
}

/////////////////////////////////////////////////////////////////////////////
void BNCTPhysicsList::ConstructProcess()
{
    // Transportation
    //
    AddTransportation();
    
    decay_List -> ConstructProcess();
    emPhysicsList -> ConstructProcess();
    
    
    //em_config.AddModels();
    
    // Hadronic physics
    //
    for(size_t i=0; i < hadronPhys.size(); i++)
    {
        hadronPhys[i] -> ConstructProcess();
    }
    
    // step limitation (as a full process)
    //
    AddStepMax();
    
    //Parallel world sensitivity
    //
    G4ParallelWorldPhysics* pWorld = new G4ParallelWorldPhysics("DetectorROGeometry");
    pWorld->ConstructProcess();
    
    return;
}

/////////////////////////////////////////////////////////////////////////////
void BNCTPhysicsList::AddPhysicsList(const G4String& name)
{
    if (verboseLevel>1) {
        G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
    }
    if (name == emName) return;
    
    ///////////////////////////////////
    //   ELECTROMAGNETIC MODELS
    ///////////////////////////////////
    if (name == "standard_opt4") {
        emName = name;
        delete emPhysicsList;
        hadronPhys.clear();
        emPhysicsList = new G4EmStandardPhysics_option4();
        G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
        G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmStandardPhysics_option4" << G4endl;
        
        ////////////////////////////////////////
        //   ELECTROMAGNETIC + HADRONIC MODELS
        ////////////////////////////////////////
        
    }  else if (name == "HADRONTHERAPY_1") {
        
        AddPhysicsList("standard_opt4");
        hadronPhys.push_back( new G4DecayPhysics());
        hadronPhys.push_back( new G4RadioactiveDecayPhysics());
        hadronPhys.push_back( new G4IonBinaryCascadePhysics());
        hadronPhys.push_back( new G4EmExtraPhysics());
        hadronPhys.push_back( new G4HadronElasticPhysicsHP());
        hadronPhys.push_back( new G4StoppingPhysics());
        hadronPhys.push_back( new G4HadronPhysicsQGSP_BIC_HP());
        //hadronPhys.push_back( new G4NeutronTrackingCut());
        
        G4cout << "HADRONTHERAPY_1 PHYSICS LIST has been activated" << G4endl;
    }
    
    else if (name == "BNCT") {
        AddPhysicsList("standard_opt4");
        G4EmParameters::Instance()->SetMinEnergy(10*eV);
        hadronPhys.push_back( new G4DecayPhysics());
        hadronPhys.push_back( new G4RadioactiveDecayPhysics());
        hadronPhys.push_back( new G4IonBinaryCascadePhysics());
        hadronPhys.push_back( new G4EmExtraPhysics());
        hadronPhys.push_back( new BNCTHadronElasticPhysicsHP());
        hadronPhys.push_back( new G4HadronPhysicsQGSP_BIC_AllHP());
        hadronPhys.push_back( new G4StoppingPhysics());
        
       // hadronPhys.push_back( new G4NeutronTrackingCut());
    
    G4cout << "BNCT PHYSICS LIST has been activated" << G4endl;
}

    else {
        G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
        << " is not defined"
        << G4endl;
    }
    
}

/////////////////////////////////////////////////////////////////////////////
void BNCTPhysicsList::AddStepMax()
{
    // Step limitation seen as a process
    // This process must exist in all threads.
    //
    BNCTStepMax* stepMaxProcess  = new BNCTStepMax();
    
    auto particleIterator = GetParticleIterator();
    particleIterator->reset();
    while ((*particleIterator)()){
        G4ParticleDefinition* particle = particleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();
        
        if (stepMaxProcess->IsApplicable(*particle) && pmanager)
        {
            pmanager ->AddDiscreteProcess(stepMaxProcess);
        }
    }
}
