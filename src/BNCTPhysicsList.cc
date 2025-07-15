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

// EM, decay
#include "G4EmStandardPhysics_option3.hh"

// Particles
#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "G4GenericIon.hh"

// Elastic
#include "G4ChipsElasticModel.hh"

// Proton inelastic
#include "G4BinaryCascade.hh"

// Neutron inelastic & capture
#include "G4NeutronInelasticProcess.hh"
#include "G4NeutronHPInelastic.hh"
#include "G4NeutronCaptureProcess.hh"
#include "G4NeutronHPCapture.hh"
#include "G4NeutronRadCapture.hh"

// Ion inelastic
#include "G4IonInelasticProcess.hh"
#include "G4BinaryLightIonReaction.hh"
#include "G4QMDModel.hh"
#include "G4FTFModel.hh"


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
    else if (name == "CUSTOM_NCEPT") {
    G4cout << "CUSTOM_NCEPT PHYSICS LIST has been activated" << G4endl;

    // === Fisica elettromagnetica accurata ===
    RegisterPhysics(new G4EmStandardPhysics_option3());

    // === Decadimento e decadimento radioattivo ===
    RegisterPhysics(new G4DecayPhysics());
    RegisterPhysics(new G4RadioactiveDecayPhysics());

    // === Proton Inelastic: 0 – 9.9 GeV con BinaryCascade ===
    {
        auto proton = G4Proton::ProtonDefinition();
        auto pmanager = proton->GetProcessManager();

        auto pInel = new G4ProtonInelasticProcess();
        auto pModel = new G4BinaryCascade();
        pModel->SetMinEnergy(0.0);
        pModel->SetMaxEnergy(9.9 * GeV);
        pInel->RegisterMe(pModel);

        pmanager->AddDiscreteProcess(pInel);
    }

    // === Neutron Elastic ===
    {
        auto neutron = G4Neutron::NeutronDefinition();
        auto pmanager = neutron->GetProcessManager();

        auto nelastic = new G4HadronElasticProcess();

        auto hpElastic = new G4NeutronHPElastic();
        hpElastic->SetMinEnergy(0.0);
        hpElastic->SetMaxEnergy(20.0 * MeV);

        auto chipsElastic = new G4ChipsElasticModel();
        chipsElastic->SetMinEnergy(20.0 * MeV);
        chipsElastic->SetMaxEnergy(100.0 * TeV);

        nelastic->RegisterMe(hpElastic);
        nelastic->RegisterMe(chipsElastic);

        pmanager->AddDiscreteProcess(nelastic);
    }

    // === Neutron Inelastic ===
    {
        auto neutron = G4Neutron::NeutronDefinition();
        auto pmanager = neutron->GetProcessManager();

        auto ninel = new G4NeutronInelasticProcess();

        auto hpInel = new G4NeutronHPInelastic();
        hpInel->SetMinEnergy(0.0);
        hpInel->SetMaxEnergy(20.0 * MeV);

        auto bicModel = new G4BinaryCascade();
        bicModel->SetMinEnergy(19.9 * MeV);
        bicModel->SetMaxEnergy(9.9 * GeV);

        ninel->RegisterMe(hpInel);
        ninel->RegisterMe(bicModel);

        pmanager->AddDiscreteProcess(ninel);
    }

    // === Neutron Capture ===
    {
        auto neutron = G4Neutron::NeutronDefinition();
        auto pmanager = neutron->GetProcessManager();

        auto ncapture = new G4NeutronCaptureProcess();

        auto hpCap = new G4NeutronHPCapture();
        hpCap->SetMinEnergy(0.0);
        hpCap->SetMaxEnergy(20.0 * MeV);

        auto radCap = new G4NeutronRadCapture();
        radCap->SetMinEnergy(19.9 * MeV);
        radCap->SetMaxEnergy(100.0 * TeV);

        ncapture->RegisterMe(hpCap);
        ncapture->RegisterMe(radCap);

        pmanager->AddDiscreteProcess(ncapture);
    }

    // === Ion Inelastic ===
    {
        auto ions = G4GenericIon::GenericIonDefinition();
        auto pmanager = ions->GetProcessManager();

        auto ionInel = new G4IonInelasticProcess();

        auto bicModel = new G4BinaryLightIonReaction();
        bicModel->SetMinEnergy(0.0);
        bicModel->SetMaxEnergy(110.0 * MeV);

        auto qmdModel = new G4QMDModel();
        qmdModel->SetMinEnergy(100.0 * MeV);
        qmdModel->SetMaxEnergy(10.0 * GeV);

        auto ftfpModel = new G4FTFModel();
        ftfpModel->SetMinEnergy(9.99 * GeV);
        ftfpModel->SetMaxEnergy(1.0 * TeV);

        ionInel->RegisterMe(bicModel);
        ionInel->RegisterMe(qmdModel);
        ionInel->RegisterMe(ftfpModel);

        pmanager->AddDiscreteProcess(ionInel);
    }
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
