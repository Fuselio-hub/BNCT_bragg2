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

#include "BNCTDetectorMessenger.hh"
#include "BNCTDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithABool.hh"

/////////////////////////////////////////////////////////////////////////////
BNCTDetectorMessenger::BNCTDetectorMessenger(BNCTDetectorConstruction* detector)
:BNCTDetector(detector)
{
    // Change Phantom size
    changeThePhantomDir = new G4UIdirectory("/changePhantom/");
    changeThePhantomDir -> SetGuidance("Command to change the Phantom Size/position");
    changeThePhantomSizeCmd = new G4UIcmdWith3VectorAndUnit("/changePhantom/size", this);
    changeThePhantomSizeCmd -> SetGuidance("Insert sizes X Y and Z"
                                           "\n   0 or negative values mean <<Don't change it!>>");
    changeThePhantomSizeCmd -> SetParameterName("PhantomSizeAlongX",
                                                "PhantomSizeAlongY",
                                                "PhantomSizeAlongZ", false);
    changeThePhantomSizeCmd -> SetDefaultUnit("mm");
    changeThePhantomSizeCmd -> SetUnitCandidates("nm um mm cm");
    changeThePhantomSizeCmd -> AvailableForStates(G4State_Idle);
    
    
    // Change Phantom material
    changeThePhantomMaterialCmd = new G4UIcmdWithAString("/changePhantom/material", this);
    changeThePhantomMaterialCmd -> SetGuidance("Change the Phantom and the B10 material");
    changeThePhantomMaterialCmd -> SetParameterName("PhantomMaterial", false);
    changeThePhantomMaterialCmd -> SetDefaultValue("G4_WATER");
    changeThePhantomMaterialCmd -> AvailableForStates(G4State_Idle);
    
    // Change B10 material
    changeTheB10MaterialCmd = new G4UIcmdWithAString("/changeB10/material", this);
    changeTheB10MaterialCmd -> SetGuidance("Change the B10 material");
    changeTheB10MaterialCmd -> SetParameterName("B10Material", false);
    changeTheB10MaterialCmd -> SetDefaultValue("G4_WATER");
    changeTheB10MaterialCmd -> AvailableForStates(G4State_Idle);

    // Change Phantom position
    changeThePhantomPositionCmd = new G4UIcmdWith3VectorAndUnit("/changePhantom/position", this);
    changeThePhantomPositionCmd -> SetGuidance("Insert X Y and Z dimensions for the position of the center of the Phantom"
                                               " respect to that of the \"World\"");
    changeThePhantomPositionCmd -> SetParameterName("PositionAlongX",
                                                    "PositionAlongY",
                                                    "PositionAlongZ", false);
    changeThePhantomPositionCmd -> SetDefaultUnit("mm");
    changeThePhantomPositionCmd -> SetUnitCandidates("um mm cm m");
    changeThePhantomPositionCmd -> AvailableForStates(G4State_Idle);
    
    
    updateCmd = new G4UIcmdWithoutParameter("/changePhantom/update",this);
    updateCmd->SetGuidance("Update Phantom/B10 geometry.");
    updateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
    updateCmd->SetGuidance("if you changed geometrical value(s).");
    updateCmd->AvailableForStates(G4State_Idle);
    
    //  Change B10 size
    changeTheB10Dir = new G4UIdirectory("/changeB10/");
    changeTheB10Dir -> SetGuidance("Command to change the B10's Size/position/Voxels");
    
    changeTheB10SizeCmd = new G4UIcmdWith3VectorAndUnit("/changeB10/size",this);
    changeTheB10SizeCmd -> SetGuidance("Insert sizes for X Y and Z dimensions of the B10"
                                            "\n   0 or negative values mean <<Don't change it>>");
    changeTheB10SizeCmd -> SetParameterName("B10SizeAlongX", "B10SizeAlongY", "B10SizeAlongZ", false);
    changeTheB10SizeCmd -> SetDefaultUnit("mm");
    changeTheB10SizeCmd -> SetUnitCandidates("nm um mm cm");
    changeTheB10SizeCmd -> AvailableForStates(G4State_Idle);
    
    //  Change the B10 to phantom displacement
    changeTheB10ToPhantomPositionCmd = new G4UIcmdWith3VectorAndUnit("/changeB10/displacement",this);
    changeTheB10ToPhantomPositionCmd -> SetGuidance("Insert X Y and Z displacements between B10 and Phantom"
                                                         "\nNegative values mean <<Don't change it!>>");
    changeTheB10ToPhantomPositionCmd -> SetParameterName("DisplacementAlongX",
                                                              "DisplacementAlongY",
                                                              "DisplacementAlongZ", false);
    changeTheB10ToPhantomPositionCmd -> SetDefaultUnit("mm");
    changeTheB10ToPhantomPositionCmd -> SetUnitCandidates("nm um mm cm");
    changeTheB10ToPhantomPositionCmd -> AvailableForStates(G4State_Idle);
    
    // Change voxels by its size
    changeTheDetectorVoxelCmd = new G4UIcmdWith3VectorAndUnit("/changeB10/voxelSize",this);
    changeTheDetectorVoxelCmd -> SetGuidance("Insert Voxel sizes for X Y and Z dimensions"
                                             "\n   0 or negative values mean <<Don't change it!>>");
    changeTheDetectorVoxelCmd -> SetParameterName("VoxelSizeAlongX", "VoxelSizeAlongY", "VoxelSizeAlongZ", false);
    changeTheDetectorVoxelCmd -> SetDefaultUnit("mm");
    changeTheDetectorVoxelCmd -> SetUnitCandidates("nm um mm cm");
    changeTheDetectorVoxelCmd -> AvailableForStates(G4State_Idle);
    
    
    changeTheSource = new G4UIdirectory("/changeTheSource/");
    changeTheSource -> SetGuidance("Command to change the source");
    
    
    
    
    /////////////////////////////////////////////////////////////////////////////
    
    VirtualLayer = new G4UIcmdWithABool("/changeTheSource/VirtualLayer",this);
    VirtualLayer -> SetParameterName("VirtualLayer ", false);
    VirtualLayer -> SetDefaultValue("false");
    VirtualLayer -> SetGuidance("Set if you want place a VirtualLayer"
                                "\n[usage]: /changeTheSource/VirtualLayer [true/false]");
    VirtualLayer -> AvailableForStates(G4State_Idle, G4State_PreInit);
    
    
    VirtualLayerPosition = new G4UIcmdWith3VectorAndUnit("/changeTheSource/VirtualLayerPosition", this);
    VirtualLayerPosition -> SetGuidance("Insert X Y and Z dimensions for the position of the center of the Virtual Layer"
                                        " respect to that of the \"World\"");
    VirtualLayerPosition -> SetParameterName("PositionAlongX",
                                             "PositionAlongY",
                                             "PositionAlongZ", false);
    
    VirtualLayerPosition -> SetDefaultUnit("cm");
    VirtualLayerPosition -> SetUnitCandidates("um mm cm m");
    VirtualLayerPosition -> AvailableForStates(G4State_Idle);
    
    
    
}

/////////////////////////////////////////////////////////////////////////////
BNCTDetectorMessenger::~BNCTDetectorMessenger()
{
    delete changeThePhantomDir;
    delete changeThePhantomSizeCmd;
    delete changeThePhantomPositionCmd;
    delete changeThePhantomMaterialCmd;
    delete changeTheB10MaterialCmd;
    delete updateCmd;
    delete changeTheB10Dir;
    delete changeTheB10SizeCmd;
    delete changeTheB10ToPhantomPositionCmd;
    delete changeTheDetectorVoxelCmd;
    delete VirtualLayer;
    delete VirtualLayerPosition;
}

/////////////////////////////////////////////////////////////////////////////
void BNCTDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
    
    if( command == changeThePhantomSizeCmd)
    {
        G4ThreeVector size = changeThePhantomSizeCmd -> GetNew3VectorValue(newValue);
        BNCTDetector -> SetPhantomSize(size.getX(),size.getY(),size.getZ());
    }
    else if (command == changeThePhantomPositionCmd )
    {
        
        G4ThreeVector size = changeThePhantomPositionCmd -> GetNew3VectorValue(newValue);
        BNCTDetector -> SetPhantomPosition(size);
    }
    else if (command == changeThePhantomMaterialCmd)
    {
        BNCTDetector -> SetPhantomMaterial(newValue);
    }
    else if (command == changeTheB10MaterialCmd)
    {
        BNCTDetector -> SetB10Material(newValue);
    }
    else if (command == changeTheB10SizeCmd)
    {
        G4ThreeVector size = changeTheB10SizeCmd  -> GetNew3VectorValue(newValue);
        BNCTDetector -> SetB10Size(size.getX(),size.getY(),size.getZ());
    }
    else if (command == changeTheB10ToPhantomPositionCmd)
    {
        G4ThreeVector size = changeTheB10ToPhantomPositionCmd-> GetNew3VectorValue(newValue);
        BNCTDetector -> SetB10ToPhantomPosition(size);
        
    }
    else if (command == changeTheDetectorVoxelCmd)
    {
        G4ThreeVector size = changeTheDetectorVoxelCmd  -> GetNew3VectorValue(newValue);
        BNCTDetector -> SetVoxelSize(size.getX(),size.getY(),size.getZ());
    }
    else if (command == updateCmd)
    {
        BNCTDetector -> UpdateGeometry();
    }
    
}
