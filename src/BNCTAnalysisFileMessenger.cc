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

#include "BNCTAnalysisFileMessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"
#include "G4SystemOfUnits.hh"

#include "BNCTMatrix.hh"
#include "BNCTLet.hh"

/////////////////////////////////////////////////////////////////////////////
BNCTAnalysisFileMessenger::BNCTAnalysisFileMessenger(BNCTAnalysis* amgr)
:AnalysisManager(amgr)
{
    secondaryCmd = new G4UIcmdWithABool("/analysis/secondary",this);
    secondaryCmd -> SetParameterName("secondary", true);
    secondaryCmd -> SetDefaultValue("true");
    secondaryCmd -> SetGuidance("Set if dose/fluence for the secondary particles will be written"
                                "\n[usage]: /analysis/secondary [true/false]");
    secondaryCmd -> AvailableForStates(G4State_Idle, G4State_PreInit);
    
    DoseMatrixCmd = new G4UIcmdWithAString("/analysis/writeDoseFile",this);
    DoseMatrixCmd->SetGuidance("Write the dose/fluence to an ASCII file");
    DoseMatrixCmd->SetDefaultValue("Dose.csv");
    DoseMatrixCmd->SetParameterName("choice",true);
    
    LetCmd = new G4UIcmdWithABool("/analysis/computeLet",this);
    LetCmd  -> SetParameterName("choice",true);
    LetCmd  -> SetDefaultValue(true);
    LetCmd  -> SetGuidance("Set if Let must be computed and write the ASCII filename for the Let");
    LetCmd  -> AvailableForStates(G4State_Idle, G4State_PreInit);
    
}

/////////////////////////////////////////////////////////////////////////////
BNCTAnalysisFileMessenger::~BNCTAnalysisFileMessenger()
{
    delete secondaryCmd;
    delete DoseMatrixCmd;
    delete LetCmd;
}

/////////////////////////////////////////////////////////////////////////////
void BNCTAnalysisFileMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == secondaryCmd)
    {
        if (BNCTMatrix::GetInstance())
        {
            BNCTMatrix::GetInstance() -> secondary = secondaryCmd -> GetNewBoolValue(newValue);
        }
    }
    
    else if (command == DoseMatrixCmd) // Filename can be passed here TODO
    {
        if ( BNCTMatrix * pMatrix = BNCTMatrix::GetInstance() )
        {
            //pMatrix -> TotalEnergyDeposit();
            pMatrix -> StoreDoseFluenceAscii(newValue);
        }
    }
    
    else if (command == LetCmd)
    {
        if (BNCTLet::GetInstance())
            BNCTLet::GetInstance() -> doCalculation = LetCmd -> GetNewBoolValue(newValue);
    }
}

