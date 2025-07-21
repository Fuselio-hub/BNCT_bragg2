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

#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4NistManager.hh"
#include "G4NistElementBuilder.hh"
#include "BNCTDetectorConstruction.hh"
#include "PassiveProtonBeamLine.hh"


//G4bool PassiveProtonBeamLine::doCalculation = false;
/////////////////////////////////////////////////////////////////////////////
PassiveProtonBeamLine::PassiveProtonBeamLine():
 physicalTreatmentRoom(0),BNCTDetectorConstruction1(0)
{
        
    //***************************** PW ***************************************
    static G4String ROGeometryName = "DetectorROGeometry";
    RO = new BNCTDetectorROGeometry(ROGeometryName);
    
    G4cout << "Going to register Parallel world...";
    RegisterParallelWorld(RO);
    G4cout << "... done" << G4endl;

}
/////////////////////////////////////////////////////////////////////////////
PassiveProtonBeamLine::~PassiveProtonBeamLine()
{

    delete BNCTDetectorConstruction1;

}

/////////////////////////////////////////////////////////////////////////////
G4VPhysicalVolume* PassiveProtonBeamLine::Construct()
{
    // Construct the whole Passive Beam Line
    ConstructPassiveProtonBeamLine();
    
    //***************************** PW ***************************************
    if (!BNCTDetectorConstruction1)
        
        //***************************** PW ***************************************
        
        // BNCTDetectorConstruction builds ONLY the phantom and the detector with its associated ROGeometry
        BNCTDetectorConstruction1 = new BNCTDetectorConstruction(physicalTreatmentRoom);
    
    
    //***************************** PW ***************************************
    
    BNCTDetectorConstruction1->InitializeDetectorROGeometry(RO,BNCTDetectorConstruction1->GetDetectorToWorldPosition());
    
    //***************************** PW ***************************************

    return physicalTreatmentRoom;
}

// In the following method the DEFAULTS used in the geometry of
// passive beam line are provided
// HERE THE USER CAN CHANGE THE GEOMETRY CHARACTERISTICS OF BEAM
// LINE ELEMENTS, ALTERNATIVELY HE/SHE CAN USE THE MACRO FILE (IF A
// MESSENGER IS PROVIDED)
//
// DEFAULT MATERIAL ARE ALSO PROVIDED
// and COLOURS ARE ALSO DEFINED
// ----------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
void PassiveProtonBeamLine::ConstructPassiveProtonBeamLine()
{
    // -----------------------------
    // Treatment room - World volume
    //------------------------------
    // Treatment room sizes
    const G4double worldX = 400.0 *cm;
    const G4double worldY = 400.0 *cm;
    const G4double worldZ = 400.0 *cm;
    G4bool isotopes = false;
    
    G4Material* airNist =  G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic", isotopes);
    G4Box* treatmentRoom = new G4Box("TreatmentRoom",worldX,worldY,worldZ);
    G4LogicalVolume* logicTreatmentRoom = new G4LogicalVolume(treatmentRoom,
                                                              airNist,
                                                              "logicTreatmentRoom",
                                                              0,0,0);
    physicalTreatmentRoom = new G4PVPlacement(0,
                                              G4ThreeVector(),
                                              "physicalTreatmentRoom",
                                              logicTreatmentRoom,
                                              0,false,0);
    
    
    // The treatment room is invisible in the Visualisation
    //logicTreatmentRoom -> SetVisAttributes(G4VisAttributes::GetInvisible());
    
 }


