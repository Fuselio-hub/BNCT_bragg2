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

#ifndef PassiveProtonBeamLine_H
#define PassiveProtonBeamLine_H 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"

class G4VPhysicalVolume;
class BNCTDetectorConstruction;
class BNCTDetectorROGeometry;

class PassiveProtonBeamLine : public G4VUserDetectorConstruction
{
public:

    PassiveProtonBeamLine();
    ~PassiveProtonBeamLine();
  // static G4bool doCalculation;
    
    G4VPhysicalVolume* Construct();
    //***************************** PW **************NON SERVE*************************
    
    static PassiveProtonBeamLine* GetInstance();
    
    //***************************** PW **************NON SERVE*************************
    
    
    
    
private:
    static PassiveProtonBeamLine* instance;
    //passive proton line dimensions
    
    void ConstructPassiveProtonBeamLine();
    
    
    
    G4VPhysicalVolume* physicalTreatmentRoom;
    BNCTDetectorConstruction* BNCTDetectorConstruction1;
    
    
    
    
    
    BNCTDetectorROGeometry* RO;
    
    
};
#endif
