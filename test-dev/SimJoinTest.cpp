#include "Snap.h"

int main(){
  TTableContext Context;
  // Case 1: Euclidean Distance
  Schema BuildingS;
  BuildingS.Add(TPair<TStr,TAttrType>("Building", atStr));
  BuildingS.Add(TPair<TStr,TAttrType>("X", atInt));
  BuildingS.Add(TPair<TStr,TAttrType>("Y", atInt));

  // create table
  PTable TBuildings = TTable::LoadSS("Buildings", BuildingS, "tests/buildings.txt", Context, '\t', false);

	TStrV Cols;
	Cols.Add("X");
	Cols.Add("Y");

	// Find all buildings within 5 Euc Distance of each other.
	PTable BuildingJointTable = TBuildings->SelfSimJoin(Cols, "Euclidean_Distance", L2Norm, 5.0);
	BuildingJointTable->SaveSS("tests/buildings.out.txt");

  // Case2 : Haversine distance 
  Schema PlaceS;
  PlaceS.Add(TPair<TStr,TAttrType>("Name", atStr));
  PlaceS.Add(TPair<TStr,TAttrType>("Location", atStr));
  PlaceS.Add(TPair<TStr,TAttrType>("Latitude", atFlt));
  PlaceS.Add(TPair<TStr,TAttrType>("Longitude", atFlt));

  // create table
  PTable TPlaces = TTable::LoadSS("Places", PlaceS, "tests/places.txt", Context, '\t', false);

	Cols.Clr();
	Cols.Add("Latitude");
	Cols.Add("Longitude");

	PTable PlacesJointTable = TPlaces->SelfSimJoin(Cols, "Distance",Haversine, 1000.0);

	TStrV ProjectionV;
	ProjectionV.Add("Places_1.Name");
	ProjectionV.Add("Places_1.Location");	
	ProjectionV.Add("Places_2.Name");
	ProjectionV.Add("Places_2.Location");
	ProjectionV.Add("Distance");
	PlacesJointTable->ProjectInPlace(ProjectionV);
	PlacesJointTable->SelectAtomic("Places_1.Name", "Places_2.Name", NEQ);
	PlacesJointTable->SaveSS("tests/places.out.txt");

	printf("Saved buildings.out.txt and places.out.txt\n");
  return 0;
}
