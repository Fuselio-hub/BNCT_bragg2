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

#ifndef BNCTDetectorMessenger_h
#define BNCTDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class BNCTDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

class BNCTDetectorMessenger: public G4UImessenger
{
public:
    BNCTDetectorMessenger(BNCTDetectorConstruction* );
    ~BNCTDetectorMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
private:
    
    // Pointer to the phantom/B10
    BNCTDetectorConstruction* BNCTDetector;
    
    G4UIdirectory *changeThePhantomDir,  *changeTheB10Dir;
    
    G4UIcmdWithoutParameter   *updateCmd;
    G4UIcmdWithAString        *changeThePhantomMaterialCmd;
    G4UIcmdWithAString        *changeTheB10MaterialCmd;
    G4UIcmdWith3VectorAndUnit *changeThePhantomSizeCmd,
    *changeThePhantomPositionCmd,
    *changeTheB10SizeCmd,
    *changeTheB10ToPhantomPositionCmd,
    *changeTheDetectorVoxelCmd,
    *VirtualLayerPosition;
    
    G4UIcmdWithABool *VirtualLayer;
    G4UIdirectory* changeTheSource;
    
    
};





#endif

