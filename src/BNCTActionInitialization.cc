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

#include "BNCTActionInitialization.hh"
#include "BNCTPrimaryGeneratorAction.hh"
#include "BNCTRunAction.hh"
#include "BNCTSteppingAction.hh"
#include "BNCTDetectorConstruction.hh"
#include "G4GeneralParticleSource.hh"

#include "BNCTEventAction.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BNCTActionInitialization::BNCTActionInitialization(/*G4VUserDetectorConstruction* detConstruction*/)
: G4VUserActionInitialization()//,
 // fDetectorConstruction(detConstruction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BNCTActionInitialization::~BNCTActionInitialization()
{//delete masterGPS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BNCTActionInitialization::BuildForMaster() const
{
	// In MT mode, to be clearer, the RunAction class for the master thread might be
	// different than the one used for the workers.
	// This RunAction will be called before and after starting the
	// workers.
	// For more details, please refer to :
	// https://twiki.cern.ch/twiki/bin/view/Geant4/Geant4MTForApplicationDevelopers
	//
	// BNCTRunAction* runAction= new BNCTRunAction(fDetectorConstruction);
	// SetUserAction(runAction);
  BNCTRunAction* pRunAction = new BNCTRunAction();
  SetUserAction(pRunAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BNCTActionInitialization::Build() const
{       
 // Initialize the primary particles
  BNCTPrimaryGeneratorAction *pPrimaryGenerator = new BNCTPrimaryGeneratorAction();
  SetUserAction(pPrimaryGenerator);
	
  // Optional UserActions: run, event, stepping
  BNCTRunAction* pRunAction = new BNCTRunAction();
  SetUserAction(pRunAction);

	
  BNCTEventAction* pEventAction = new BNCTEventAction();
  SetUserAction(pEventAction);
	
  BNCTSteppingAction* steppingAction = new BNCTSteppingAction(pRunAction); 
  SetUserAction(steppingAction);  

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
