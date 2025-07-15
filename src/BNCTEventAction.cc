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

#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4VVisManager.hh"

#include "BNCTEventAction.hh"
#include "BNCTDetectorHit.hh"
#include "BNCTDetectorSD.hh"
#include "BNCTDetectorConstruction.hh"
#include "BNCTMatrix.hh"
#include "BNCTEventActionMessenger.hh"

/////////////////////////////////////////////////////////////////////////////
BNCTEventAction::BNCTEventAction() :
drawFlag("all" ),printModulo(10), pointerEventMessenger(0)
{
    hitsCollectionID = -1;
    pointerEventMessenger = new BNCTEventActionMessenger(this);
}

/////////////////////////////////////////////////////////////////////////////
BNCTEventAction::~BNCTEventAction()
{
    delete pointerEventMessenger;
}

/////////////////////////////////////////////////////////////////////////////
void BNCTEventAction::BeginOfEventAction(const G4Event*)
{
    G4SDManager* pSDManager = G4SDManager::GetSDMpointer();
    if(hitsCollectionID == -1)
        hitsCollectionID = pSDManager -> GetCollectionID("BNCTDetectorHitsCollection");
}

/////////////////////////////////////////////////////////////////////////////
void BNCTEventAction::EndOfEventAction(const G4Event* evt)
{
    if(hitsCollectionID < 0)
        return;
    G4HCofThisEvent* HCE = evt -> GetHCofThisEvent();
    
    // Clear voxels hit list
    BNCTMatrix* matrix = BNCTMatrix::GetInstance();
    if (matrix) matrix -> ClearHitTrack();
    
    if(HCE)
    {
        BNCTDetectorHitsCollection* CHC = (BNCTDetectorHitsCollection*)(HCE -> GetHC(hitsCollectionID));
        if(CHC)
        {
            if(matrix)
            {
                // Fill the matrix with the information: voxel and associated energy deposit
                // in the detector at the end of the event
                
                size_t HitCount = CHC -> entries();
                for (size_t h=0; h<HitCount; h++)
                {
                    G4int i = ((*CHC)[h]) -> GetXID();
                    G4int j = ((*CHC)[h]) -> GetYID();
                    G4int k = ((*CHC)[h]) -> GetZID();
                    G4double energyDeposit = ((*CHC)[h]) -> GetEdep();
                    matrix -> Fill(i, j, k, energyDeposit/MeV);
                }
            }
        }
    }
}
