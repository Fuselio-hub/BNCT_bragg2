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

#ifndef BNCTDetectorConstruction_H
#define BNCTDetectorConstruction_H 1

#include "G4Box.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "BNCTDetectorROGeometry.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4PVPlacement;
class BNCTDetectorROGeometry;
class BNCTDetectorMessenger;
class BNCTDetectorSD;
class BNCTMatrix;
class BNCTLet;

class BNCTDetectorConstruction
{
public:
    
    BNCTDetectorConstruction(G4VPhysicalVolume*);
    
    ~BNCTDetectorConstruction();
    
public:
    static BNCTDetectorConstruction* GetInstance();
    void InitializeDetectorROGeometry(BNCTDetectorROGeometry*,
                                      G4ThreeVector detectorToWorldPosition);
    G4VPhysicalVolume* motherPhys;
    BNCTDetectorSD*         detectorSD; // Pointer to sensitive detector
    
    //////////////////////////
    void VirtualLayer(G4bool Varbool);
    G4bool NewSource;
    void SetVirtualLayerPosition(G4ThreeVector);
    G4ThreeVector VirtualLayerPosition;
    
    //////////////////////////
private:
    void DefineMaterials();
    void ConstructPhantom();
    void ConstructB10();
    void ParametersCheck();
    void CheckOverlaps();
    
public:
    // Get detector position relative to WORLD
    inline G4ThreeVector GetDetectorToWorldPosition()
    {
        return phantomPosition;
    }
    /////////////////////////////////////////////////////////////////////////////
    // Get displacement between phantom and B10 by B10 position (center of), phantom (center of) and B10 sizes
    inline G4ThreeVector GetB10ToPhantomPosition()
    {
        return G4ThreeVector(phantomSizeX/2 - B10SizeX/2 + B10Position.getX(),
                             phantomSizeY/2 - B10SizeY/2 + B10Position.getY(),
                             phantomSizeZ/2 - B10SizeZ/2 + B10Position.getZ()
                             );
    }
    
    /////////////////////////////////////////////////////////////////////////////
    // Calculate (and set) B10 position by displacement, phantom and B10 sizes
    inline void SetB10Position()
    {
        // Adjust B10 position
        B10Position.setX(B10ToPhantomPosition.getX() - phantomSizeX/2 + B10SizeX/2);
        B10Position.setY(B10ToPhantomPosition.getY() - phantomSizeY/2 + B10SizeY/2);
        B10Position.setZ(B10ToPhantomPosition.getZ() - phantomSizeZ/2 + B10SizeZ/2);
        
    }
    /////////////////////////////////////////////////////////////////////////////
    // Check whether B10 is inside phantom
    inline bool IsInside(G4double B10X,
                         G4double B10Y,
                         G4double B10Z,
                         G4double phantomX,
                         G4double phantomY,
                         G4double phantomZ,
                         G4ThreeVector pos)
    {
        // Dimensions check... X Y and Z
        // Firstly check what dimension we are modifying
        {
            if (B10X > phantomX)
            {
                G4cout << "Error: B10 X dimension must be smaller or equal to the correspondent of the phantom" << G4endl;
                return false;
            }
            if ( (phantomX - B10X) < pos.getX())
            {
                G4cout << "Error: X dimension doesn't fit with B10 to phantom relative position" << G4endl;
                return false;
            }
        }
        
        {
            if (B10Y > phantomY)
            {
                G4cout << "Error: B10 Y dimension must be smaller or equal to the correspondent of the phantom" << G4endl;
                return false;
            }
            if ( (phantomY - B10Y) < pos.getY())
            {
                G4cout << "Error: Y dimension doesn't fit with B10 to phantom relative position" << G4endl;
                return false;
            }
        }
        
        {
            if (B10Z > phantomZ)
            {
                G4cout << "Error: B10 Z dimension must be smaller or equal to the correspondent of the phantom" << G4endl;
                return false;
            }
            if ( (phantomZ - B10Z) < pos.getZ())
            {
                G4cout << "Error: Z dimension doesn't fit with B10 to phantom relative position" << G4endl;
                return false;
            }
        }
        
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////
    
    G4bool  SetPhantomMaterial(G4String material);
    G4bool  SetB10Material(G4String material);
    void SetVoxelSize(G4double sizeX, G4double sizeY, G4double sizeZ);
    void SetB10Size(G4double sizeX, G4double sizeY, G4double sizeZ);
    void SetPhantomSize(G4double sizeX, G4double sizeY, G4double sizeZ);
    void SetPhantomPosition(G4ThreeVector);
    void SetB10ToPhantomPosition(G4ThreeVector B10ToPhantomPosition);
    void UpdateGeometry();
    void PrintParameters();
    G4LogicalVolume* GetB10LogicalVolume(){ return B10LogicalVolume;}
    
private:
    static BNCTDetectorConstruction* instance;
    BNCTDetectorMessenger* detectorMessenger;
    
    G4VisAttributes* skyBlue;
    G4VisAttributes* red;
    G4VisAttributes* green;
    
    BNCTDetectorROGeometry* detectorROGeometry; // Pointer to ROGeometry
    BNCTMatrix*             matrix;
    BNCTLet*                let;
    
    G4Box *phantom , *B10;
    G4LogicalVolume *phantomLogicalVolume, *B10LogicalVolume;
    G4VPhysicalVolume *phantomPhysicalVolume, *B10PhysicalVolume;
    
    G4Box* solidVirtualLayer;
    G4LogicalVolume* logicVirtualLayer;
    G4VPhysicalVolume*  physVirtualLayer;
    
    G4double phantomSizeX;
    G4double phantomSizeY;
    G4double phantomSizeZ;
    
    G4double B10SizeX;
    G4double B10SizeY;
    G4double B10SizeZ;
    
    G4ThreeVector phantomPosition, B10Position, B10ToPhantomPosition; //  phantom center, detector center, detector to phantom relative position
    
    G4double sizeOfVoxelAlongX;
    G4double sizeOfVoxelAlongY;
    G4double sizeOfVoxelAlongZ;
    
    G4int numberOfVoxelsAlongX;
    G4int numberOfVoxelsAlongY;
    G4int numberOfVoxelsAlongZ;
    
    G4double volumeOfVoxel, massOfVoxel;
    
    G4Material *phantomMaterial, *B10Material;
    G4Region* aRegion;
};
#endif
