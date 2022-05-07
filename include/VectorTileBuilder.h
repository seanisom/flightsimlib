#pragma once

#include <unordered_map>
#include <vector>

namespace flightsimlib::cgl
{

class IVectorTile;

struct Vert2U16;

typedef Vert2U16 VectorVertex;
typedef Vert2U16 LandFeature;
typedef Vert2U16 WaterFeature;
typedef Vert2U16 Unknown1Feature;
typedef Vert2U16 Unknown2Feature;
	
struct PowerFeature;
struct RailFeature;
struct RiverFeature;
struct WaterPolygon;
struct RoadFeature;

enum class ERoadType
{
	MotorwayRhd = 0,
	MotorwayLhd = 1,
	MotorwayLinkRhd = 2,
	MotorwayLinkLhd = 3,
	TrunkRhd = 4,
	TrunkLhd = 5,
	TrunkLinkRhd = 6,
	TrunkLinkLhd = 7,
	PrimaryRhd = 8,
	PrimaryLhd = 9,
	PrimaryLinkRhd = 10,
	PrimaryLinkLhd = 11,
	SecondaryRhd = 12,
	SecondaryLhd = 13,
	SecondaryLinkRhd = 14,
	SecondaryLinkLhd = 15,
	TertiaryRhd = 16,
	TertiaryLhd = 17,
	TertiaryLinkRhd = 18,
	TertiaryLinkLhd = 19,
	ResidentialRhd = 20,
	ResidentialLhd = 21,
	ServiceRhd = 22,
	ServiceLhd = 23,
	LivingStreetRhd = 24,
	LivingStreetLhd = 25,
	UnclassifiedRhd = 26,
	UnclassifiedLhd = 27,
	TrackRhd = 28,
	TrackLhd = 29,
	Footway = 30
};

	
enum class ELandType
{
	Vineyard = 0,
	Cemetery = 1,
	PlantNursery = 2,
	Orchard = 3,
	Grass = 4,
	Garden = 5,
	Park = 6,
	Residential = 7,
	CampPitch = 8,
	CaravanSite = 9,
	Parking = 10,
	Beach = 11,
	Glacier = 12,
	Aerodrome = 13,
	Unknown14 = 14,
	Unknown15 = 15,
	Industrial = 16,
	Pitch = 17,
	SolarPlant = 18,
	NuclearPlant = 19,
	Wetland = 20,
	Pedestrian = 21,
	Square = 22,
	Religious = 23,
	Worship = 24,
	Sand = 25,
	Reef = 26,
	Farmland = 27
};

enum class EWaterType
{
	Riverbank = 0,
	Wastewater = 1,
	Unclassified = 2,
	Pond = 3,
	Reservoir = 4,
	Coastline = 5,
	Other = 6
};

enum class EPointType
{
	TreeBroadleaved = 0,
	TreeNeedleleaved = 1,
	TreePalm = 2,
	TreeLeafless = 3,
	Tree = 4,
	WindTurbineHorizontal = 5,
	WindTurbineVertical = 6,
	Windmill = 7,
	WindmillVaneless = 8,
	StorageTank = 9,
	Tower = 10,
	WaterTower = 11,
	Mast = 12,
	MastCommunication = 13,
	TowerCommunication = 14,
	ChimneyCommunication = 15,
	TowerCooling = 16,
	TowerRadar = 17,
	WeatherStation = 18,
	Silo = 19
};


enum class ERailType
{
	Heavy = 0,
	Disused = 1,
	Tram = 2,
	Subway = 3,
	NarrowGauge = 4,
	Light = 5,
	Preserved = 6,
	Monorail = 7,
	Funicular = 8
};


enum class EPowerType
{
	Tower = 0,
	Pole = 1
};


enum class EUnknown1Type
{
	Type0 = 0,
	Type1 = 1,
	Type2 = 2,
	Type3 = 3,
	Type4 = 4,
	Type5 = 5,
	Type6 = 6,
	Type7 = 7,
	Type8 = 8,
	Type9 = 9
};


enum class EUnknown2Type
{
	Type0 = 0,
	Type1 = 1,
	Type2 = 2,
	Type3 = 3,
	Type4 = 4
};
	
	
class VectorTileBuilder
{
public:
	void AddRoad(ERoadType type, RoadFeature& road, const std::vector<VectorVertex>& vertices);
	void AddLand(ELandType type, LandFeature& land, const std::vector<VectorVertex>& vertices);
	void AddWater(EWaterType type, std::vector<std::vector<VectorVertex>>& vertices, const std::vector<float>& heights);
	void AddRiver(RiverFeature river, const std::vector<VectorVertex>& vertices);
	void AddPoints(EPointType type, const std::vector<VectorVertex>& vertices);
	void AddRail(ERailType type, RailFeature& rail, const std::vector<VectorVertex>& vertices);
	void AddPower(EPowerType type, PowerFeature& power, const std::vector<VectorVertex>& vertices);
	void AddUnknown1(EUnknown1Type type, Unknown1Feature& unknown1, const std::vector<VectorVertex>& vertices);
	void AddUnknown2(EUnknown2Type type, Unknown2Feature& unknown2, const std::vector<VectorVertex>& vertices);
	
	IVectorTile* Build();

private:
	std::unordered_map<ERoadType, std::vector<RoadFeature>> m_roads;
	std::unordered_map<ELandType, std::vector<LandFeature>> m_land;
	std::unordered_map<int, EWaterType> m_water_features;
	std::vector<std::vector<WaterPolygon>> m_water_polygons;
	std::vector<RiverFeature> m_rivers;
	std::unordered_map<EPointType, std::vector<VectorVertex>> m_points;
	std::unordered_map<ERailType, std::vector<RailFeature>> m_rails;
	std::unordered_map<EPowerType, std::vector<PowerFeature>> m_power;
	std::unordered_map<EUnknown1Type, std::vector<Unknown1Feature>> m_unknown1;
	std::unordered_map<EUnknown2Type, std::vector<Unknown2Feature>> m_unknown2;

	std::vector<VectorVertex> m_vertices;
};
	
}