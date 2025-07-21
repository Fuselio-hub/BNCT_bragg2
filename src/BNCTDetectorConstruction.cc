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

#include "G4UnitsTable.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4Colour.hh"
#include "G4UserLimits.hh"
#include "G4UnitsTable.hh"
#include "G4VisAttributes.hh"
#include "G4NistManager.hh"
#include "BNCTDetectorConstruction.hh"
#include "BNCTDetectorROGeometry.hh"
#include "BNCTDetectorMessenger.hh"
#include "BNCTDetectorSD.hh"
#include "BNCTMatrix.hh"
#include "BNCTLet.hh"
#include "PassiveProtonBeamLine.hh"
#include "BNCTMatrix.hh"

#include "BNCTRBE.hh"
#include "G4SystemOfUnits.hh"

#include <cmath>



BNCTDetectorConstruction* BNCTDetectorConstruction::instance = 0;
/////////////////////////////////////////////////////////////////////////////
BNCTDetectorConstruction::BNCTDetectorConstruction(G4VPhysicalVolume* physicalTreatmentRoom)
: motherPhys(physicalTreatmentRoom), // pointer to WORLD volume
detectorSD(0), detectorROGeometry(0), matrix(0),
phantom(0), B10(0),
phantomLogicalVolume(0), B10LogicalVolume(0),
phantomPhysicalVolume(0), B10PhysicalVolume(0),
aRegion(0)
{
    
    
    /* NOTE! that the BNCTDetectorConstruction class
     * does NOT inherit from G4VUserDetectorConstruction G4 class
     * So the Construct() mandatory virtual method is inside another geometric class
     * like the passiveProtonBeamLIne, ...
     */
    
    // Messenger to change parameters of the phantom/detector geometry
    detectorMessenger = new BNCTDetectorMessenger(this);
    
    // Default detector voxels size
    // 200 slabs along the beam direction (X)
    sizeOfVoxelAlongX = 0.1 * cm;  // se cambi, anche in RunAction
    sizeOfVoxelAlongY = 20 *cm;
    sizeOfVoxelAlongZ = 20 *cm;
    
    DefineMaterials();
    // Define here the material of the water phantom and of the detector
    SetPhantomMaterial("soft_tissue");
    SetB10Material("BoroTissue"); 
    
    // Construct geometry (messenger commands)
    // SetDetectorSize(4.*cm, 4.*cm, 4.*cm);
    // dimensioni e posizioni cambiate nella macro 
    SetB10Size(1. *cm, 20. *cm, 20. *cm);
    SetPhantomSize(20. *cm, 20. *cm, 20. *cm);
    
    SetPhantomPosition(G4ThreeVector(10. *cm, 0. *cm, 0. *cm));
    SetB10ToPhantomPosition(G4ThreeVector(4. *cm, 0. *cm, 0. *cm));  // relativo al phantom 
    SetB10Position();
    //GetDetectorToWorldPosition();
    
    // Write virtual parameters to the real ones and check for consistency
    UpdateGeometry();
    
}

/////////////////////////////////////////////////////////////////////////////
BNCTDetectorConstruction::~BNCTDetectorConstruction()
{
    delete detectorROGeometry;
    delete matrix;
    delete detectorMessenger;
}

/////////////////////////////////////////////////////////////////////////////
BNCTDetectorConstruction* BNCTDetectorConstruction::GetInstance()
{
    return instance;
}

void BNCTDetectorConstruction::DefineMaterials()
{
  G4NistManager* man = G4NistManager::Instance();

  man->FindOrBuildMaterial("G4_Al");
  man->FindOrBuildMaterial("G4_Si");
  man->FindOrBuildMaterial("G4_Fe");
  man->FindOrBuildMaterial("G4_Cu");
  man->FindOrBuildMaterial("G4_Ge");
  man->FindOrBuildMaterial("G4_Mo");
  man->FindOrBuildMaterial("G4_Ta");
  man->FindOrBuildMaterial("G4_W");
  man->FindOrBuildMaterial("G4_Au");
  man->FindOrBuildMaterial("G4_Pb");
  man->FindOrBuildMaterial("G4_PbWO4");
  man->FindOrBuildMaterial("G4_SODIUM_IODIDE");

  man->FindOrBuildMaterial("G4_AIR");
  //man->FindOrBuildMaterial("G4_WATER");

  
// definizione degli elementi: G4Element(nome , simbolo , numero atomico, massa atomica);
    G4Element* H1 = man->FindOrBuildElement("H");
//  G4Element* O = man->FindOrBuildElement("O");
    // pressurized water
    G4Element* H  = new G4Element("TS_H_of_Water" ,"H" , 1., 1.0079*g/mole);
    G4Element* O  = new G4Element("Oxygen"        ,"O" , 8., 16.00*g/mole);
    G4Element* elC = new G4Element("Carbon","C",6.,12.011*g/mole);
    G4Element* elN = new G4Element("Nitrogen","N", 7.,14.01*g/mole);
    G4Element* elO = new G4Element("Oxygen","O", 8.,16.00*g/mole);
    G4Element* elNa = new G4Element("Sodium","Na", 11.,22.99*g/mole);
    G4Element* elMg = new G4Element("Magnesium","Mg", 12.,24.305*g/mole);
    G4Element* elP = new G4Element("Phosphorus","P", 15.,30.974*g/mole);
    G4Element* elS = new G4Element("Sulfur","S", 16.,32.064*g/mole);
    G4Element* elCl = new G4Element("Chlorine","Cl", 17.,35.453*g/mole);
    G4Element* elK = new G4Element("Potassium","K", 19.,39.098*g/mole);
    G4Element* elCa = new G4Element("Calcium","Ca", 20.,40.08*g/mole);
    G4Element* elFe  = new G4Element("Iron","Fe", 26.,55.85*g/mole);
    G4Element* elZn = new G4Element("Zinc","Zn", 30.,65.38*g/mole);
    G4Element* elRb = new G4Element("Rb","Rb", 37.,85.47 *g/mole);
    G4Element* elSr = new G4Element("Sr","Sr", 38.,87.62 *g/mole);
    G4Element* elZr = new G4Element("Zr","Zr", 40.,91.22 *g/mole);
    G4Element* elPb = new G4Element("Lead","Pb",  82.,207.19 *g/mole);
    
    // qui c'era definizione dell'acqua pressurizzata  --->  tolta perché inutile
    
    // definizione del boro
    G4double fractionmass;
    G4Isotope* B10 = new G4Isotope("B10", 5, 10, 10.811*g/mole);
    G4Isotope* B11 = new G4Isotope("B11", 5, 11, 11.0093*g/mole);
//
    G4Element* eB10 = new G4Element("Borone","B",1);
//  eB->AddIsotope(B10,19.1*perCent);
//  eB->AddIsotope(B11,80.9*perCent);
    eB10->AddIsotope(B10,100.*perCent);  // 100% boro-10!!!
//
    G4Material * Boro10 = new G4Material("Boro10",2.46*g/cm3,1);
    Boro10->AddElement(eB10, 1.);
//
    //preso da esempio Human_Phantom: MIRD soft tissue. vengono aggiunti 
    G4double d = 0.9869 *g/cm3;
    G4Material* soft = new G4Material("soft_tissue",d,16);
    soft->AddElement(H,0.1047);
    soft->AddElement(elC,0.2302);
    soft->AddElement(elN,0.0234);
    soft->AddElement(elO,0.6321);
    soft->AddElement(elNa,0.0013);
    soft->AddElement(elMg,0.00015);
    soft->AddElement(elP,0.0024);
    soft->AddElement(elS,0.0022);
    soft->AddElement(elCl,0.0014);
    soft->AddElement(elK,0.0021);
    soft->AddElement(elFe,0.000063);
    soft->AddElement(elZn,0.000032);
    soft->AddElement(elRb,0.0000057);
    soft->AddElement(elSr,0.00000034);
    soft->AddElement(elZr,0.000008);
    soft->AddElement(elPb,0.00000016);
    
    
    G4double density = 1.*g/cm3;
    G4Material * B10Water = new G4Material("WaterB10",d,2); //2 sta per due componenti (tessuto molle e boro) 
    B10Water-> AddMaterial (soft ,fractionmass=99*perCent); //qui ho messo soft al posto di H2O
    B10Water-> AddMaterial (Boro10 ,fractionmass=1*perCent);  // quantità esagerata di boro ma solo per provare
        
    G4Material * BoroTissue = new G4Material("BoroTissue",d,2);
    BoroTissue -> AddMaterial (soft, fractionmass = 99.99*perCent);
    BoroTissue -> AddMaterial (Boro10, fractionmass = 0.01*perCent ); //100 ppm di boro

    // soft tissue con boro11 per confronto
    G4Element* eB11 = new G4Element("Borone11", "B", 1);
    eB11->AddIsotope(B11, 100.*perCent);  // 100% B11

    // Crea materiale puro di B11 (densità identica a Boron)
    G4Material* Boron11 = new G4Material("Boron11", 2.46*g/cm3, 1);
    Boron11->AddElement(eB11, 1.);

    // Crea Boro11Tissue come miscela di soft tissue e B11
    G4Material* Boro11Tissue = new G4Material("Boro11Tissue", d, 2);
    Boro11Tissue->AddMaterial(soft, 99.99*perCent);
    Boro11Tissue->AddMaterial(Boron11, 0.01*perCent);

  //  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}
/////////////////////////////////////////////////////////////////////////////
// ConstructPhantom() is the method that construct a water box (called phantom
// (or water phantom) in the usual Medical physicists slang).
// A water phantom can be considered a good approximation of a an human body.
void BNCTDetectorConstruction::ConstructPhantom()
{
    // Definition of the solid volume of the Phantom
    phantom = new G4Box("Phantom",
                        phantomSizeX/2,
                        phantomSizeY/2,
                        phantomSizeZ/2);
    
    // Definition of the logical volume of the Phantom
    phantomLogicalVolume = new G4LogicalVolume(phantom,
                                               phantomMaterial,
                                               "phantomLog", 0, 0, 0);
    
    // Definition of the physics volume of the Phantom
    phantomPhysicalVolume = new G4PVPlacement(0,
                                              phantomPosition,
                                              "phantomPhys",
                                              phantomLogicalVolume,
                                              motherPhys,
                                              false,
                                              0);
    
    // Visualisation attributes of the phantom
    //red = new G4VisAttributes(G4Colour(255/255., 0/255. ,0/255.));
    red = new G4VisAttributes(G4Colour(204/255., 119/255. ,34/255.));  // non è rosso ma arancione
    red -> SetVisibility(true);
    red -> SetForceSolid(true);
    //red -> SetForceWireframe(true);
    red -> SetForceWireframe(false);
    phantomLogicalVolume -> SetVisAttributes(red);
}

/////////////////////////////////////////////////////////////////////////////
// ConstructDetector() is the method the reconstruct a detector region
// inside the water phantom. It is a volume, located inside the water phantom.
//
//           **************************
//           *    water phantom       *
//           *                        *
//           *                        *
//           *---------------         *
//  Beam     *              -         *
//  ----->   * detector     -         *
//           *              -         *
//           *---------------         *
//           *                        *
//           *                        *
//           *                        *
//           **************************
//
// The detector can be dived in slices or voxelized
// and inside it different quantities (dose distribution, fluence distribution, LET, etc)
// can be stored.
void BNCTDetectorConstruction::ConstructB10()

{
        // Definition of the solid volume of the Detector
    B10 = new G4Box("B10",
                         
                         phantomSizeX/2,
                         
                         phantomSizeY/2,
                         
                         phantomSizeZ/2);
    
    // Definition of the logic volume of the Phantom
    B10LogicalVolume = new G4LogicalVolume(B10,
                                                B10Material,
                                                "B10Log",
                                                0,0,0);
    // Definition of the physical volume of the Phantom
    B10PhysicalVolume = new G4PVPlacement(0,
                                               B10Position, // Setted by displacement
                                               "B10Phys",
                                               B10LogicalVolume,
                                               phantomPhysicalVolume,
                                               false,0);
    
    // Visualisation attributes of the detector
    skyBlue = new G4VisAttributes( G4Colour(128/255. , 0/255. ,  32/255. ));  // non è skyblue ma rosso scuro
    //skyBlue = new G4VisAttributes( G4Colour(0/255. , 255/255. ,  0/255. ));
    skyBlue -> SetVisibility(true);
    skyBlue -> SetForceSolid(true);
    //skyBlue -> SetForceWireframe(true);
    B10LogicalVolume -> SetVisAttributes(skyBlue);
    
    // **************
    // **************
    // Cut per Region
    // **************
    //
    // A smaller cut is fixed in the phantom to calculate the energy deposit with the
    // required accuracy
    
}

///////////////////////////////////////////////////////////////////////
void BNCTDetectorConstruction::InitializeDetectorROGeometry(BNCTDetectorROGeometry* RO,
                                                            G4ThreeVector detectorToWorldPosition)
{
    RO->Initialize(detectorToWorldPosition,
                   phantomSizeX/2,
                   phantomSizeY/2,
                   phantomSizeZ/2,
                   numberOfVoxelsAlongX,
                   numberOfVoxelsAlongY,
                   numberOfVoxelsAlongZ);
}

///////////////////////////////////////////////////////////////////////
void  BNCTDetectorConstruction::ParametersCheck()
{
    // Check phantom/detector sizes & relative position
    if (!IsInside(B10SizeX,
                  B10SizeY,
                  B10SizeZ,
                  phantomSizeX,
                  phantomSizeY,
                  phantomSizeZ,
                  B10ToPhantomPosition
                  ))
        G4Exception("BNCTDetectorConstruction::ParametersCheck()", "BNCT0001", FatalException, "Error: Detector is not fully inside Phantom!");
    
    // Check Detector sizes respect to the voxel ones
    
    if ( B10SizeX < sizeOfVoxelAlongX) {
        G4Exception("BNCTDetectorConstruction::ParametersCheck()", "BNCT0002", FatalException, "Error:  Detector X size must be bigger or equal than that of Voxel X!");
    }
    if ( B10SizeY < sizeOfVoxelAlongY) {
        G4Exception(" BNCTDetectorConstruction::ParametersCheck()", "BNCT0003", FatalException, "Error:  Detector Y size must be bigger or equal than that of Voxel Y!");
    }
    if ( B10SizeZ < sizeOfVoxelAlongZ) {
        G4Exception(" BNCTDetectorConstruction::ParametersCheck()", "BNCT0004", FatalException, "Error:  Detector Z size must be bigger or equal than that of Voxel Z!");
    }
}

///////////////////////////////////////////////////////////////////////
G4bool BNCTDetectorConstruction::SetPhantomMaterial(G4String material)
{
    
    if (G4Material* pMat = G4NistManager::Instance()->FindOrBuildMaterial(material, false) )
    {
        phantomMaterial  = pMat;
        if (phantomLogicalVolume)
        {
            phantomLogicalVolume ->  SetMaterial(pMat);
            G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
            G4RunManager::GetRunManager() -> GeometryHasBeenModified();
            G4cout << "The material of Phantom has been changed to " << material << G4endl;
        }
    }
    else
    {
        G4cout << "WARNING: material \"" << material << "\" doesn't exist in NIST elements/materials"
        " table [located in $G4INSTALL/source/materials/src/G4NistMaterialBuilder.cc]" << G4endl;
        G4cout << "Use command \"/parameter/nist\" to see full materials list!" << G4endl;
        return false;
    }
    
    return true;
}
G4bool BNCTDetectorConstruction::SetB10Material(G4String material)
{
    
    if (G4Material* pMat = G4NistManager::Instance()->FindOrBuildMaterial(material, false) )
    {
        B10Material = pMat;
        if (B10LogicalVolume)
        {
            B10LogicalVolume -> SetMaterial(pMat);
            G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
            G4RunManager::GetRunManager() -> GeometryHasBeenModified();
            G4cout << "The material of activeLayer has been changed to " << material << G4endl;
        }
    }
    else
    {
        G4cout << "WARNING: material \"" << material << "\" doesn't exist in NIST elements/materials"
        " table [located in $G4INSTALL/source/materials/src/G4NistMaterialBuilder.cc]" << G4endl;
        G4cout << "Use command \"/parameter/nist\" to see full materials list!" << G4endl;
        return false;
    }
    
    return true;
}
/////////////////////////////////////////////////////////////////////////////
void BNCTDetectorConstruction::SetPhantomSize(G4double sizeX, G4double sizeY, G4double sizeZ)
{
    if (sizeX > 0.) phantomSizeX = sizeX;
    if (sizeY > 0.) phantomSizeY = sizeY;
    if (sizeZ > 0.) phantomSizeZ = sizeZ;
}

/////////////////////////////////////////////////////////////////////////////
void BNCTDetectorConstruction::SetB10Size(G4double sizeX, G4double sizeY, G4double sizeZ)
{
    if (sizeX > 0.) {B10SizeX = sizeX;}
    if (sizeY > 0.) {B10SizeY = sizeY;}
    if (sizeZ > 0.) {B10SizeZ = sizeZ;}
    SetVoxelSize(sizeOfVoxelAlongX, sizeOfVoxelAlongY, sizeOfVoxelAlongZ);
}

/////////////////////////////////////////////////////////////////////////////
void BNCTDetectorConstruction::SetVoxelSize(G4double sizeX, G4double sizeY, G4double sizeZ)
{
    if (sizeX > 0.) {sizeOfVoxelAlongX = sizeX;}
    if (sizeY > 0.) {sizeOfVoxelAlongY = sizeY;}
    if (sizeZ > 0.) {sizeOfVoxelAlongZ = sizeZ;}
}

///////////////////////////////////////////////////////////////////////
void BNCTDetectorConstruction::SetPhantomPosition(G4ThreeVector pos)
{
    phantomPosition = pos;
}

/////////////////////////////////////////////////////////////////////////////
void BNCTDetectorConstruction::SetB10ToPhantomPosition(G4ThreeVector displ)
{
    B10ToPhantomPosition = displ;
}


/////////////////////////////////////////////////////////////////////////////
void BNCTDetectorConstruction::UpdateGeometry()
{
    /*
     * Check parameters consistency
     */
    ParametersCheck();
    
    G4GeometryManager::GetInstance() -> OpenGeometry();
    if (phantom)
    {
        phantom -> SetXHalfLength(phantomSizeX/2);
        phantom -> SetYHalfLength(phantomSizeY/2);
        phantom -> SetZHalfLength(phantomSizeZ/2);
        
        phantomPhysicalVolume -> SetTranslation(phantomPosition);
    }
    else   ConstructPhantom();
    
    
    // Get the center of the B10 layer
    SetB10Position();
    if (B10)
    {
        
        B10 -> SetXHalfLength(B10SizeX/2);
        B10 -> SetYHalfLength(B10SizeY/2);
        B10 -> SetZHalfLength(B10SizeZ/2);
        
        B10PhysicalVolume -> SetTranslation(B10Position);
    }
    else    ConstructB10();
    
     
    
    // Round to nearest integer number of voxel
    
    numberOfVoxelsAlongX = G4lrint(phantomSizeX / sizeOfVoxelAlongX);
    sizeOfVoxelAlongX = ( phantomSizeX / numberOfVoxelsAlongX );
    numberOfVoxelsAlongY = G4lrint(phantomSizeY / sizeOfVoxelAlongY);
    sizeOfVoxelAlongY = ( phantomSizeY / numberOfVoxelsAlongY );
    numberOfVoxelsAlongZ = G4lrint(phantomSizeZ / sizeOfVoxelAlongZ);
    sizeOfVoxelAlongZ = ( phantomSizeZ / numberOfVoxelsAlongZ );
    PassiveProtonBeamLine *ppbl= (PassiveProtonBeamLine*)
    
    G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    
    BNCTDetectorROGeometry* RO = (BNCTDetectorROGeometry*) ppbl->GetParallelWorld(0);
    
    //Set parameters, either for the Construct() or for the UpdateROGeometry()
    RO->Initialize(GetDetectorToWorldPosition(),
                   phantomSizeX/2,
                   phantomSizeY/2,
                   phantomSizeZ/2,
                   numberOfVoxelsAlongX,
                   numberOfVoxelsAlongY,
                   numberOfVoxelsAlongZ);
    
    //This method below has an effect only if the RO geometry is already built.
    RO->UpdateROGeometry();
    
    
    
    volumeOfVoxel = sizeOfVoxelAlongX * sizeOfVoxelAlongY * sizeOfVoxelAlongZ;
    ///!!!ATTENZIONE CONTROLLARE LA MASSA DEL VOXEL
    massOfVoxel = phantomMaterial -> GetDensity() * volumeOfVoxel;
    //  This will clear the existing matrix (together with all data inside it)!
    matrix = BNCTMatrix::GetInstance(numberOfVoxelsAlongX,
                                              numberOfVoxelsAlongY,
                                              numberOfVoxelsAlongZ,
                                              massOfVoxel);
    
    
    // Initialize RBE
    BNCTRBE::CreateInstance(numberOfVoxelsAlongX, numberOfVoxelsAlongY, numberOfVoxelsAlongZ, massOfVoxel);
    
    // Comment out the line below if let calculation is not needed!
    // Initialize LET with energy of primaries and clear data inside
    if ( (let = BNCTLet::GetInstance(this)) )
    {
        BNCTLet::GetInstance() -> Initialize();
    }
    
    
    // Initialize analysis
    // Inform the kernel about the new geometry
    G4RunManager::GetRunManager() -> GeometryHasBeenModified();
    G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
    
    PrintParameters();
    
    // CheckOverlaps();
}

/////////////////////////////////////////////////////////////////////////////
//Check of the geometry
/////////////////////////////////////////////////////////////////////////////
void BNCTDetectorConstruction::CheckOverlaps()
{
    G4PhysicalVolumeStore* thePVStore = G4PhysicalVolumeStore::GetInstance();
    G4cout << thePVStore->size() << " physical volumes are defined" << G4endl;
    G4bool overlapFlag = false;
    G4int res=1000;
    G4double tol=0.; //tolerance
    for (size_t i=0;i<thePVStore->size();i++)
    {
        //overlapFlag = (*thePVStore)[i]->CheckOverlaps(res,tol,fCheckOverlapsVerbosity) | overlapFlag;
        overlapFlag = (*thePVStore)[i]->CheckOverlaps(res,tol,true) | overlapFlag;    }
    if (overlapFlag)
        G4cout << "Check: there are overlapping volumes" << G4endl;
}

/////////////////////////////////////////////////////////////////////////////
void BNCTDetectorConstruction::PrintParameters()
{
    
    G4cout << "The (X,Y,Z) dimensions of the phantom are : (" <<
    G4BestUnit( phantom -> GetXHalfLength()*2., "Length") << ',' <<
    G4BestUnit( phantom -> GetYHalfLength()*2., "Length") << ',' <<
    G4BestUnit( phantom -> GetZHalfLength()*2., "Length") << ')' << G4endl;
    
    G4cout << "The (X,Y,Z) dimensions of the detector are : (" <<
    G4BestUnit( B10 -> GetXHalfLength()*2., "Length") << ',' <<
    G4BestUnit( B10 -> GetYHalfLength()*2., "Length") << ',' <<
    G4BestUnit( B10 -> GetZHalfLength()*2., "Length") << ')' << G4endl;
    
    G4cout << "Displacement between Phantom and World is: ";
    G4cout << "DX= "<< G4BestUnit(phantomPosition.getX(),"Length") <<
    "DY= "<< G4BestUnit(phantomPosition.getY(),"Length") <<
    "DZ= "<< G4BestUnit(phantomPosition.getZ(),"Length") << G4endl;
    
    G4cout << "The (X,Y,Z) sizes of the Voxels are: (" <<
    G4BestUnit(sizeOfVoxelAlongX, "Length")  << ',' <<
    G4BestUnit(sizeOfVoxelAlongY, "Length")  << ',' <<
    G4BestUnit(sizeOfVoxelAlongZ, "Length") << ')' << G4endl;
    
    G4cout << "The number of Voxels along (X,Y,Z) is: (" <<
    numberOfVoxelsAlongX  << ',' <<
    numberOfVoxelsAlongY  <<','  <<
    numberOfVoxelsAlongZ  << ')' << G4endl;
}