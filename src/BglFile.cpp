//******************************************************************************
//
// The MIT License (MIT)
//
// Copyright (c) 2020 Sean Isom
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//******************************************************************************

//******************************************************************************
//
// File:     BglFile.cpp
//
// Summary:  BGL File structure parsing and utilities
//
// Author:   Sean Isom
//
// TODO list:
// * Factories need to be refactored to standalone
// * Layers and Tiles should conform to IBglSerializable
// * Observable model
// * History Stack
//
//******************************************************************************

// ReSharper disable CppClangTidyCppcoreguidelinesProTypeStaticCastDowncast
#include "BglFile.h"
// #include "BglData.h"

namespace flightsimlib
{

    namespace io
    {

        //******************************************************************************
        // CBglData
        //******************************************************************************

        template <typename... Args>
        std::unique_ptr<CBglData> FactoryImpl(
            EBglLayerType type, IBglSceneryObject::ESceneryObjectType child_type, Args&&... args)
        {
            std::unique_ptr<IBglSerializable> data = nullptr;

            constexpr auto no_copy = sizeof...(Args) != 0;

            using TLayer = std::underlying_type<EBglLayerType>::type;
            const auto layer_value = static_cast<TLayer>(type);
            constexpr auto season_lower = static_cast<TLayer>(EBglLayerType::TerrainSeasonJan);
            constexpr auto season_upper = static_cast<TLayer>(EBglLayerType::TerrainSeasonDec);
            constexpr auto photo_lower = static_cast<TLayer>(EBglLayerType::TerrainPhotoJan);
            constexpr auto photo_upper = static_cast<TLayer>(EBglLayerType::TerrainPhotoNight);
            constexpr auto photo32_lower = static_cast<TLayer>(EBglLayerType::TerrainPhoto32Jan);
            constexpr auto photo32_upper = static_cast<TLayer>(EBglLayerType::TerrainPhoto32Night);

            if (layer_value >= season_lower && layer_value <= season_upper)
            {
                data = no_copy ? std::make_unique<CBglTerrainSeason>()
                               : std::make_unique<CBglTerrainSeason>(*static_cast<const CBglTerrainSeason*>(args)...);
                return std::make_unique<CBglData>(type, std::move(data));
            }

            if (layer_value >= photo_lower && layer_value <= photo_upper)
            {
                data = no_copy ? std::make_unique<CBglTerrainPhoto>()
                               : std::make_unique<CBglTerrainPhoto>(*static_cast<const CBglTerrainPhoto*>(args)...);
                return std::make_unique<CBglData>(type, std::move(data));
            }

            if (layer_value >= photo32_lower && layer_value <= photo32_upper)
            {
                data = no_copy ? std::make_unique<CBglTerrainPhoto32>()
                               : std::make_unique<CBglTerrainPhoto32>(*static_cast<const CBglTerrainPhoto32*>(args)...);
                return std::make_unique<CBglData>(type, std::move(data));
            }

            switch (type) // NOLINT(clang-diagnostic-switch-enum)
            {
            case EBglLayerType::Airport:
                data = no_copy ? std::make_unique<CBglAirport>() : // NOLINT(bugprone-branch-clone)
                           std::make_unique<CBglAirport>(*static_cast<const CBglAirport*>(args)...);
                break;
            case EBglLayerType::Nav:
                data = no_copy ? std::make_unique<CBglNav>() : // NOLINT(bugprone-branch-clone)
                           std::make_unique<CBglNav>(*static_cast<const CBglNav*>(args)...);
                break;
            case EBglLayerType::Ndb:
                data = no_copy ? std::make_unique<CBglNdb>() : // NOLINT(bugprone-branch-clone)
                           std::make_unique<CBglNdb>(*static_cast<const CBglNdb*>(args)...);
                break;
            case EBglLayerType::Marker:
                data = no_copy ? std::make_unique<CBglMarker>() : // NOLINT(bugprone-branch-clone)
                           std::make_unique<CBglMarker>(*static_cast<const CBglMarker*>(args)...);
                break;
            case EBglLayerType::Boundary:
                data = no_copy ? std::make_unique<CBglBoundary>() : // NOLINT(bugprone-branch-clone)
                           std::make_unique<CBglBoundary>(*static_cast<const CBglBoundary*>(args)...);
                break;
            case EBglLayerType::Waypoint:
                data = no_copy ? std::make_unique<CBglWaypoint>() : // NOLINT(bugprone-branch-clone)
                           std::make_unique<CBglWaypoint>(*static_cast<const CBglWaypoint*>(args)...);
                break;
            case EBglLayerType::Geopol:
                data = no_copy ? std::make_unique<CBglGeopol>() : // NOLINT(bugprone-branch-clone)
                           std::make_unique<CBglGeopol>(*static_cast<const CBglGeopol*>(args)...);
                break;
            case EBglLayerType::SceneryObject:
            {
                switch (child_type)
                {
                case IBglSceneryObject::ESceneryObjectType::GenericBuilding:
                    data = no_copy ? std::make_unique<CBglGenericBuilding>() : // NOLINT(bugprone-branch-clone)
                               std::make_unique<CBglGenericBuilding>(*static_cast<const CBglGenericBuilding*>(args)...);
                    break;
                case IBglSceneryObject::ESceneryObjectType::LibraryObject:
                    data = no_copy ? std::make_unique<CBglLibraryObject>() : // NOLINT(bugprone-branch-clone)
                               std::make_unique<CBglLibraryObject>(*static_cast<const CBglLibraryObject*>(args)...);
                    break;
                case IBglSceneryObject::ESceneryObjectType::Windsock:
                    data = no_copy ? std::make_unique<CBglWindsock>() : // NOLINT(bugprone-branch-clone)
                               std::make_unique<CBglWindsock>(*static_cast<const CBglWindsock*>(args)...);
                    break;
                case IBglSceneryObject::ESceneryObjectType::Effect:
                    data = no_copy ? std::make_unique<CBglEffect>() : // NOLINT(bugprone-branch-clone)
                               std::make_unique<CBglEffect>(*static_cast<const CBglEffect*>(args)...);
                    break;
                case IBglSceneryObject::ESceneryObjectType::TaxiwaySigns:
                    data = no_copy ? std::make_unique<CBglTaxiwaySigns>() : // NOLINT(bugprone-branch-clone)
                               std::make_unique<CBglTaxiwaySigns>(*static_cast<const CBglTaxiwaySigns*>(args)...);
                    break;
                case IBglSceneryObject::ESceneryObjectType::Trigger:
                    data = no_copy ? std::make_unique<CBglTrigger>() : // NOLINT(bugprone-branch-clone)
                               std::make_unique<CBglTrigger>(*static_cast<const CBglTrigger*>(args)...);
                    break;
                case IBglSceneryObject::ESceneryObjectType::Beacon:
                    data = no_copy ? std::make_unique<CBglBeacon>() : // NOLINT(bugprone-branch-clone)
                               std::make_unique<CBglBeacon>(*static_cast<const CBglBeacon*>(args)...);
                    break;
                case IBglSceneryObject::ESceneryObjectType::ExtrusionBridge:
                    data = no_copy ? std::make_unique<CBglExtrusionBridge>() : // NOLINT(bugprone-branch-clone)
                               std::make_unique<CBglExtrusionBridge>(*static_cast<const CBglExtrusionBridge*>(args)...);
                    break;
                default:
                    break;
                }
            }
            break;

            case EBglLayerType::ModelData:
                data = no_copy ? std::make_unique<CBglModelData>() : // NOLINT(bugprone-branch-clone)
                           std::make_unique<CBglModelData>(*static_cast<const CBglModelData*>(args)...);
                break;
            case EBglLayerType::AirportSummary:
                data = no_copy ? std::make_unique<CBglAirportSummary>() : // NOLINT(bugprone-branch-clone)
                           std::make_unique<CBglAirportSummary>(*static_cast<const CBglAirportSummary*>(args)...);
                break;
            case EBglLayerType::TerrainVectorDb:
                break;
            case EBglLayerType::TerrainElevation:
                data = no_copy
                           ? std::make_unique<CBglTerrainElevation>()
                           : std::make_unique<CBglTerrainElevation>(*static_cast<const CBglTerrainElevation*>(args)...);
                break;
            case EBglLayerType::TerrainLandClass:
                data = no_copy
                           ? std::make_unique<CBglTerrainLandClass>()
                           : std::make_unique<CBglTerrainLandClass>(*static_cast<const CBglTerrainLandClass*>(args)...);
                break;
            case EBglLayerType::TerrainWaterClass:
                data = no_copy ? std::make_unique<CBglTerrainWaterClass>()
                               : std::make_unique<CBglTerrainWaterClass>(
                                     *static_cast<const CBglTerrainWaterClass*>(args)...);
                break;
            case EBglLayerType::TerrainRegion:
                data = no_copy ? std::make_unique<CBglTerrainRegion>()
                               : std::make_unique<CBglTerrainRegion>(*static_cast<const CBglTerrainRegion*>(args)...);
                break;
            case EBglLayerType::PopulationDensity:
                data = no_copy ? std::make_unique<CBglPopulationDensity>()
                               : std::make_unique<CBglPopulationDensity>(
                                     *static_cast<const CBglPopulationDensity*>(args)...);
                break;
            case EBglLayerType::AutogenAnnotation:
                break;
            case EBglLayerType::TerrainIndex:
                data = no_copy ? std::make_unique<CBglTerrainIndex>()
                               : std::make_unique<CBglTerrainIndex>(*static_cast<const CBglTerrainIndex*>(args)...);
                break;
            case EBglLayerType::TerrainTextureLookup:
                break;
            case EBglLayerType::Tacan:
                data = no_copy ? std::make_unique<CBglTacan>() : // NOLINT(bugprone-branch-clone)
                           std::make_unique<CBglTacan>(*static_cast<const CBglTacan*>(args)...);
                break;
            default:
                return nullptr;
                /*
                auto AsTerrainSeason()->IBglTerrainSeason* override;
                auto AsTerrainPhoto()->IBglTerrainPhoto* override;
                auto AsTerrainPhoto32()->IBglTerrainPhoto32* override;
                */
            }

            return std::make_unique<CBglData>(type, std::move(data));
        }

        CBglData::CBglData(EBglLayerType type, std::unique_ptr<IBglSerializable> data) :
            m_type(type), m_data(std::move(data))
        {
        }

        std::unique_ptr<CBglData> CBglData::Factory(
            EBglLayerType type, IBglSceneryObject::ESceneryObjectType child_type)
        {
            return FactoryImpl(type, child_type);
        }

        std::unique_ptr<CBglData> CBglData::Clone() const
        {
            auto child_type = IBglSceneryObject::ESceneryObjectType::Unknown;
            if (GetType() == EBglLayerType::SceneryObject)
            {
                const auto* scenery = static_cast<const CBglSceneryObject*>(m_data.get());
                child_type = scenery->GetSceneryObjectType();
            }
            return FactoryImpl(GetType(), child_type, m_data.get());
        }

        auto CBglData::GetType() const -> EBglLayerType { return m_type; }

        auto CBglData::SetType(EBglLayerType value) -> void { m_type = value; }

        auto CBglData::AsRasterQuad1() -> ITerrainRasterQuad1*
        {
            using TLayer = std::underlying_type<EBglLayerType>::type;

            constexpr auto season_lower = static_cast<TLayer>(EBglLayerType::TerrainSeasonJan);
            constexpr auto season_upper = static_cast<TLayer>(EBglLayerType::TerrainSeasonDec);
            constexpr auto photo_lower = static_cast<TLayer>(EBglLayerType::TerrainPhotoJan);
            constexpr auto photo_upper = static_cast<TLayer>(EBglLayerType::TerrainPhotoNight);
            constexpr auto photo32_lower = static_cast<TLayer>(EBglLayerType::TerrainPhoto32Jan);
            constexpr auto photo32_upper = static_cast<TLayer>(EBglLayerType::TerrainPhoto32Night);

            const auto layer_value = static_cast<TLayer>(m_type);
            if (layer_value >= season_lower && layer_value <= season_upper)
            {
                return static_cast<CTerrainRasterQuad1*>(m_data.get());
            }

            if (layer_value >= photo_lower && layer_value <= photo_upper)
            {
                return static_cast<CTerrainRasterQuad1*>(m_data.get());
            }

            if (layer_value >= photo32_lower && layer_value <= photo32_upper)
            {
                return static_cast<CTerrainRasterQuad1*>(m_data.get());
            }

            switch (m_type)
            {
            case EBglLayerType::TerrainElevation:
            case EBglLayerType::TerrainLandClass:
            case EBglLayerType::TerrainWaterClass:
            case EBglLayerType::TerrainRegion:
            case EBglLayerType::PopulationDensity:
            case EBglLayerType::TerrainIndex:
                return static_cast<CTerrainRasterQuad1*>(m_data.get());
            default:
                break;
            }

            return nullptr;
        }

        auto CBglData::AsAirport() -> IBglAirport*
        {
            if (m_type == EBglLayerType::Airport)
            {
                return static_cast<CBglAirport*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsNav() -> IBglNav*
        {
            if (m_type == EBglLayerType::Nav)
            {
                return static_cast<CBglNav*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsNdb() -> IBglNdb*
        {
            if (m_type == EBglLayerType::Ndb)
            {
                return static_cast<CBglNdb*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsMarker() -> IBglMarker*
        {
            if (m_type == EBglLayerType::Marker)
            {
                return static_cast<CBglMarker*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsBoundary() -> IBglBoundary*
        {
            if (m_type == EBglLayerType::Boundary)
            {
                return static_cast<CBglBoundary*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsWaypoint() -> IBglWaypoint*
        {
            if (m_type == EBglLayerType::Waypoint)
            {
                return static_cast<CBglWaypoint*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsGeopol() -> IBglGeopol*
        {
            if (m_type == EBglLayerType::Geopol)
            {
                return static_cast<CBglGeopol*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsSceneryObject() -> IBglSceneryObject*
        {
            if (m_type == EBglLayerType::SceneryObject)
            {
                return static_cast<CBglSceneryObject*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsModelData() -> IBglModelData*
        {
            if (m_type == EBglLayerType::ModelData)
            {
                return static_cast<CBglModelData*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsAirportSummary() -> IBglAirportSummary*
        {
            if (m_type == EBglLayerType::AirportSummary)
            {
                return static_cast<CBglAirportSummary*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsTerrainVectorDb() -> IBglTerrainVectorDb*
        {
            // if (m_type == EBglLayerType::TerrainVectorDb)
            // {
            //     return dynamic_cast<IBglTerrainVectorDb*>(m_data.get());
            // }
            return nullptr;
        }

        auto CBglData::AsTerrainElevation() -> IBglTerrainElevation*
        {
            if (m_type == EBglLayerType::TerrainElevation)
            {
                return static_cast<CBglTerrainElevation*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsTerrainLandClass() -> IBglTerrainLandClass*
        {
            if (m_type == EBglLayerType::TerrainLandClass)
            {
                return static_cast<CBglTerrainLandClass*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsTerrainWaterClass() -> IBglTerrainWaterClass*
        {
            if (m_type == EBglLayerType::TerrainWaterClass)
            {
                return static_cast<CBglTerrainWaterClass*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsTerrainRegion() -> IBglTerrainRegion*
        {
            if (m_type == EBglLayerType::TerrainRegion)
            {
                return static_cast<CBglTerrainRegion*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsPopulationDensity() -> IBglPopulationDensity*
        {
            if (m_type == EBglLayerType::PopulationDensity)
            {
                return static_cast<CBglPopulationDensity*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsAutogenAnnotation() -> IBglAutogenAnnotation*
        {
            // if (m_type == EBglLayerType::AutogenAnnotation)
            // {
            //     return dynamic_cast<IBglAutogenAnnotation*>(m_data.get());
            // }
            return nullptr;
        }

        auto CBglData::AsTerrainIndex() -> IBglTerrainIndex*
        {
            if (m_type == EBglLayerType::TerrainIndex)
            {
                return static_cast<CBglTerrainIndex*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsTerrainTextureLookup() -> IBglTerrainTextureLookup*
        {
            // if (m_type == EBglLayerType::TerrainTextureLookup)
            // {
            //     return dynamic_cast<IBglTerrainTextureLookup*>(m_data.get());
            // }
            return nullptr;
        }

        auto CBglData::AsTerrainSeason() -> IBglTerrainSeason*
        {
            using TLayer = std::underlying_type<EBglLayerType>::type;

            constexpr auto lower_bound = static_cast<TLayer>(EBglLayerType::TerrainSeasonJan);
            constexpr auto upper_bound = static_cast<TLayer>(EBglLayerType::TerrainSeasonDec);
            const auto layer_type = static_cast<TLayer>(m_type);

            if (layer_type >= lower_bound && layer_type <= upper_bound)
            {
                return static_cast<CBglTerrainSeason*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsTerrainPhoto() -> IBglTerrainPhoto*
        {
            using TLayer = std::underlying_type<EBglLayerType>::type;

            constexpr auto lower_bound = static_cast<TLayer>(EBglLayerType::TerrainPhotoJan);
            constexpr auto upper_bound = static_cast<TLayer>(EBglLayerType::TerrainPhotoNight);
            const auto layer_type = static_cast<TLayer>(m_type);

            if (layer_type >= lower_bound && layer_type <= upper_bound)
            {
                return static_cast<CBglTerrainPhoto*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsTerrainPhoto32() -> IBglTerrainPhoto32*
        {
            using TLayer = std::underlying_type<EBglLayerType>::type;

            constexpr auto lower_bound = static_cast<TLayer>(EBglLayerType::TerrainPhoto32Jan);
            constexpr auto upper_bound = static_cast<TLayer>(EBglLayerType::TerrainPhoto32Night);
            const auto layer_type = static_cast<TLayer>(m_type);

            if (layer_type >= lower_bound && layer_type <= upper_bound)
            {
                return static_cast<CBglTerrainPhoto32*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::AsTacan() -> IBglTacan*
        {
            if (m_type == EBglLayerType::Tacan)
            {
                return static_cast<CBglTacan*>(m_data.get());
            }
            return nullptr;
        }

        auto CBglData::CalculateSize() const -> int
        {
            if (m_data == nullptr)
            {
                return 0;
            }
            return m_data->CalculateSize();
        }

        auto CBglData::Validate() const -> bool
        {
            if (m_data == nullptr)
            {
                return false;
            }
            return m_data->Validate();
        }

        auto CBglData::ReadBinary(BinaryFileStream& in) -> bool
        {
            if (m_data == nullptr)
            {
                return false;
            }

            m_data->ReadBinary(in);

            if (!in)
            {
                return false;
            }
            return true;
        }

        auto CBglData::WriteBinary(BinaryFileStream& out) -> bool
        {
            if (m_data == nullptr)
            {
                return false;
            }

            m_data->WriteBinary(out);

            if (!out)
            {
                return false;
            }
            return true;
        }

        //******************************************************************************
        // CBglLayer
        //******************************************************************************

        CBglLayer::CBglLayer(EBglLayerType type, EBglLayerClass layer_class, const SBglLayerPointer& data) :
            m_layer_pointer(data), m_type(type), m_class(layer_class)
        {
        }

        std::unique_ptr<CBglLayer> CBglLayer::Factory(const SBglLayerPointer& data)
        {
            std::unique_ptr<CBglLayer> layer = nullptr;
            const auto type = static_cast<EBglLayerClass>(data.DataClass);

            switch (type)
            {
            case EBglLayerClass::DirectQmid:
                layer = std::make_unique<CBglDirectQmidLayer>(data, data.Type);
                break;
            case EBglLayerClass::IndirectQmid:
                layer = std::make_unique<CBglIndirectQmidLayer>(data, data.Type);
                break;
            case EBglLayerClass::AirportNameIndex:
                break;
            case EBglLayerClass::IcaoIndex:
                break;
            case EBglLayerClass::GuidIndex:
                layer = std::make_unique<CBglGuidLayer>(data, data.Type);
                break;
            case EBglLayerClass::Exclusion:
                layer = std::make_unique<CBglExclusionLayer>(data);
                break;
            case EBglLayerClass::TimeZone:
                layer = std::make_unique<CBglTimeZoneLayer>(data);
                break;
            case EBglLayerClass::Unknown:
                break;
            }
            return layer;
        }

        auto CBglLayer::GetType() const -> EBglLayerType { return m_type; }

        auto CBglLayer::SetType(EBglLayerType value) -> void { m_type = value; }

        auto CBglLayer::GetClass() const -> EBglLayerClass { return m_class; }

        auto CBglLayer::SetClass(EBglLayerClass value) -> void { m_class = value; }

        auto CBglLayer::GetLayerPointer() const -> const SBglLayerPointer* { return &m_layer_pointer.read(); }

        auto CBglLayer::AsIndirectQmidLayer() -> IBglIndirectQmidLayer*
        {
            if (GetClass() == EBglLayerClass::IndirectQmid)
            {
                return static_cast<CBglIndirectQmidLayer*>(this);
            }
            return nullptr;
        }

        auto CBglLayer::AsDirectQmidLayer() -> IBglDirectQmidLayer*
        {
            if (GetClass() == EBglLayerClass::DirectQmid)
            {
                return static_cast<CBglDirectQmidLayer*>(this);
            }
            return nullptr;
        }

        auto CBglLayer::AsNameListLayer() -> IBglNameListLayer*
        {
            if (GetClass() == EBglLayerClass::AirportNameIndex)
            {
                return static_cast<CBglNameListLayer*>(this);
            }
            return nullptr;
        }

        auto CBglLayer::AsIcaoLayer() -> IBglIcaoLayer*
        {
            if (GetClass() == EBglLayerClass::IcaoIndex)
            {
                return static_cast<CBglIcaoLayer*>(this);
            }
            return nullptr;
        }

        auto CBglLayer::AsGuidLayer() -> IBglGuidLayer*
        {
            if (GetClass() == EBglLayerClass::GuidIndex)
            {
                return static_cast<CBglGuidLayer*>(this);
            }
            return nullptr;
        }

        auto CBglLayer::AsExclusionLayer() -> IBglExclusionLayer*
        {
            if (GetClass() == EBglLayerClass::Exclusion)
            {
                return static_cast<CBglExclusionLayer*>(this);
            }
            return nullptr;
        }

        auto CBglLayer::AsTimeZoneLayer() -> IBglTimeZoneLayer*
        {
            if (GetClass() == EBglLayerClass::TimeZone)
            {
                return static_cast<CBglTimeZoneLayer*>(this);
            }
            return nullptr;
        }

        bool CBglLayer::IsTrq1BglLayer(EBglLayerType layer_type)
        {
            if (layer_type == EBglLayerType::TerrainIndex || layer_type == EBglLayerType::TerrainLandClass ||
                layer_type == EBglLayerType::TerrainWaterClass || layer_type == EBglLayerType::TerrainRegion ||
                layer_type == EBglLayerType::PopulationDensity ||
                (layer_type >= EBglLayerType::TerrainSeasonJan && layer_type <= EBglLayerType::TerrainSeasonDec) ||
                (layer_type >= EBglLayerType::TerrainPhotoJan && layer_type <= EBglLayerType::TerrainPhotoNight) ||
                (layer_type >= EBglLayerType::TerrainPhoto32Jan && layer_type <= EBglLayerType::TerrainPhoto32Night))
            {
                return true;
            }
            return false;
        }

        bool CBglLayer::IsRcs1BglLayer(EBglLayerType layer_type)
        {
            if (layer_type == EBglLayerType::TerrainElevation)
            {
                return true;
            }
            return false;
        }

        //******************************************************************************
        // CBglDirectQmidLayer
        //******************************************************************************

        CBglDirectQmidLayer::CBglDirectQmidLayer(const SBglLayerPointer& pointer, EBglLayerType type) :
            CBglLayer(type, EBglLayerClass::DirectQmid, pointer)
        {
        }

        CBglDirectQmidLayer::CBglDirectQmidLayer(const CBglDirectQmidLayer& other) :
            CBglLayer(other.GetType(), other.GetClass(), *other.GetLayerPointer()), m_pointers(other.m_pointers.size())
        {
            for (const auto& tile : other.m_tiles)
            {
                auto& it = m_tiles[tile.first] = std::vector<std::unique_ptr<CBglData>>(tile.second.size());

                for (const auto& data : tile.second)
                {
                    it.emplace_back(data->Clone());
                }
            }

            for (auto i = 0u; i < other.m_pointers.size(); ++i)
            {
                m_pointers[i] = std::make_unique<SBglTilePointer>(*other.m_pointers[i]);
            }
        }

        auto CBglDirectQmidLayer::ReadBinary(BinaryFileStream& in) -> bool
        {
            const auto* layer_pointer = CBglLayer::GetLayerPointer();
            if (!in)
            {
                return false;
            }
            const auto next_position = in.GetPosition();
            in.SetPosition(layer_pointer->StreamOffset);
            if (!in)
            {
                return false;
            }

            // TODO - This needs to become a factory method
            // ReSharper disable once CppInitializedValueIsAlwaysRewritten
            auto layer_type = EBglLayerType::None;
            switch (layer_pointer->Type)
            {
            case EBglLayerType::TerrainPhoto32Jan:
                layer_type = EBglLayerType::Tacan;
                break;
            case EBglLayerType::TerrainPhoto32Feb:
                layer_type = EBglLayerType::TacanIndex;
                break;
            default:
                layer_type = layer_pointer->Type;
                break;
            }
            const auto qmid_count = static_cast<int>(layer_pointer->TileCount);

            auto existing = false;
            // TODO - merge layers when pointer match - maybe handle outside of layer code?
            /*
            for (const auto& other_layer : layers)
            {
                if (other_layer.second->m_data->StreamOffset == layer_pointer->StreamOffset)
                {
                    existing = true;
                    for (const auto& tile : other_layer.second->Tiles())
                    {
                        const auto tile_pointer(tile.second);
                        layer->AddTile(tile.first, tile_pointer);
                    }
                    break;
                }
            }
            */
            if (existing == false)
            {
                m_pointers.resize(qmid_count);
                const bool is_terrain_layer =
                    CBglLayer::IsTrq1BglLayer(layer_pointer->Type) || CBglLayer::IsRcs1BglLayer(layer_pointer->Type);
                const auto entry_size =
                    (layer_pointer->TileCount > 0) ? (layer_pointer->SizeBytes / layer_pointer->TileCount) : 0u;
                for (auto i = 0; i < qmid_count; ++i)
                {
                    m_pointers[i] = std::make_unique<SBglTilePointer>();
                    if (is_terrain_layer && (entry_size == 16 || entry_size == 20))
                    {
                        in >> m_pointers[i]->QmidLow >> m_pointers[i]->QmidHigh;
                        if (entry_size == 20)
                        {
                            in >> m_pointers[i]->RecordCount;
                        }
                        else
                        {
                            m_pointers[i]->RecordCount = 1;
                        }
                        in >> m_pointers[i]->StreamOffset >> m_pointers[i]->SizeBytes;
                    }
                    else
                    {
                        m_pointers[i]->ReadBinary(in, layer_pointer->HasQmidHigh != 0);
                    }
                    if (!in)
                    {
                        return false;
                    }
                }
                if (in.GetPosition() != static_cast<int>(layer_pointer->StreamOffset + layer_pointer->SizeBytes))
                {
                    return false;
                }
                for (const auto& tile_pointer : m_pointers)
                {
                    const auto data_count = static_cast<int>(tile_pointer->RecordCount);
                    if (data_count <= 0)
                    {
                        return false;
                    }

                    in.SetPosition(tile_pointer->StreamOffset);
                    if (!in)
                    {
                        return false;
                    }

                    auto tile =
                        m_tiles.emplace(std::make_pair(CPackedQmid{tile_pointer->QmidLow, tile_pointer->QmidHigh},
                            std::vector<std::unique_ptr<CBglData>>{}));
                    auto& data_list = tile.first->second;

                    data_list.reserve(data_count);

                    for (auto i = 0; i < data_count; ++i)
                    {
                        const auto pos = in.GetPosition();
                        const auto type = GetType();
                        auto child_type = IBglSceneryObject::ESceneryObjectType::Unknown;
                        if (type == EBglLayerType::SceneryObject)
                        {
                            child_type = GetSceneryObjectType(in);
                        }
                        auto data = CBglData::Factory(layer_type, child_type);
                        if (data == nullptr)
                        {
                            in.SetPosition(pos + static_cast<int>(tile_pointer->SizeBytes));
                            continue;
                        }
                        if (!data->ReadBinary(in))
                        {
                            return false;
                        }
                        const auto size = data->CalculateSize();
                        if (!data->Validate())
                        {
                            return false;
                        }
                        data_list.emplace_back(std::move(data));
                        if (is_terrain_layer)
                        {
                            in.SetPosition(pos + static_cast<int>(tile_pointer->SizeBytes));
                        }
                        else
                        {
                            in.SetPosition(pos + static_cast<int>(size));
                        }
                        if (!in)
                        {
                            return false;
                        }
                    }
                }
            }

            in.SetPosition(next_position);
            if (!in)
            {
                return false;
            }
            return true;
        }

        auto CBglDirectQmidLayer::CalculateSize() const -> int
        {
            auto data_size = 0;
            for (const auto& tile : m_tiles)
            {
                for (const auto& data : tile.second)
                {
                    data_size += data->CalculateSize();
                }
            }
            return data_size;
        }

        auto CBglDirectQmidLayer::CalculateDataPointersSize() const -> int
        {
            auto tile_pointer_size = 16;
            for (const auto& pointer : m_pointers)
            {
                if (pointer->QmidHigh > 0)
                {
                    tile_pointer_size = 20;
                    break;
                }
            }
            return static_cast<int>(m_layer_pointer->TileCount) * tile_pointer_size;
        }

        auto CBglDirectQmidLayer::WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile, int offset_to_layer)
            -> bool
        {
            const auto tile_pointers_size = CalculateDataPointersSize();

            auto type = CBglLayer::GetType();
            switch (type) // NOLINT(clang-diagnostic-switch-enum)
            {
            case EBglLayerType::Tacan:
                type = EBglLayerType::TerrainPhoto32Jan;
                break;
            case EBglLayerType::TacanIndex:
                type = EBglLayerType::TerrainPhoto32Feb;
                break;
            default:
                break;
            }

            auto& data = m_layer_pointer.write();
            data.Type = type;

            data.TileCount = static_cast<uint32_t>(m_tiles.size());
            data.DataClass = 1;
            if (m_layer_pointer->TileCount && tile_pointers_size / static_cast<int>(m_layer_pointer->TileCount) == 20)
            {
                data.HasQmidHigh = 1;
            }
            else
            {
                data.HasQmidHigh = 0;
            }
            data.SizeBytes = tile_pointers_size;
            data.StreamOffset = offset_to_tile;

            m_layer_pointer->WriteBinary(out);
            return true;
        }

        auto CBglDirectQmidLayer::WriteBinaryData(BinaryFileStream& out) -> bool
        {
            for (const auto& pointer : m_pointers)
            {
                const auto& tile = m_tiles[CPackedQmid{pointer->QmidLow, pointer->QmidHigh}];

                pointer->StreamOffset = out.GetPosition();
                pointer->RecordCount = static_cast<int>(tile.size());

                auto data_size = 0;

                for (const auto& data : tile)
                {
                    if (!data->WriteBinary(out))
                    {
                        return false;
                    }
                    data_size += data->CalculateSize();
                }

                pointer->SizeBytes = data_size;
            }
            return true;
        }

        auto CBglDirectQmidLayer::WriteBinaryDataPointers(BinaryFileStream& out) -> bool
        {
            for (const auto& pointer : m_pointers)
            {
                pointer->WriteBinary(out, m_layer_pointer->HasQmidHigh);
                if (!out)
                {
                    return false;
                }
            }
            return true;
        }

        auto CBglDirectQmidLayer::GetQmidCount() const -> int
        {
            assert(m_pointers.size() == m_tiles.size());
            return static_cast<int>(m_pointers.size());
        }

        auto CBglDirectQmidLayer::HasQmid(CPackedQmid qmid) const -> bool
        {
            const auto& it = m_tiles.find(qmid);
            if (it == m_tiles.end())
            {
                return false;
            }
            return true;
        }

        auto CBglDirectQmidLayer::GetDataPointerAtIndex(int index) const -> const SBglTilePointer*
        {
            return m_pointers[index].get();
        }

        auto CBglDirectQmidLayer::GetDataCountAtQmid(CPackedQmid qmid) -> int
        {
            const auto& it = m_tiles.find(qmid);
            if (it == m_tiles.end())
            {
                return 0;
            }
            return static_cast<int>(it->second.size());
        }

        auto CBglDirectQmidLayer::GetDataAtQmid(CPackedQmid qmid, int index) -> IBglData*
        {
            const auto& it = m_tiles.find(qmid);
            if (it == m_tiles.end())
            {
                return nullptr;
            }
            return it->second[index].get();
        }

        auto CBglDirectQmidLayer::AddDataAtQmid(CPackedQmid qmid, const IBglData* data) -> void
        {
            auto& record = m_tiles[qmid];
            if (record.empty())
            {
                auto pointer = std::make_unique<SBglTilePointer>();
                pointer->QmidLow = qmid.Low();
                pointer->QmidHigh = qmid.High();
                pointer->RecordCount = 1u;

                auto it = m_pointers.begin();
                for (; it != m_pointers.end(); ++it)
                {
                    auto record_qmid = CPackedQmid{it->get()->QmidLow, it->get()->QmidHigh};
                    if (record_qmid > qmid) // map is ordered, so only check one direction
                    {
                        break;
                    }
                }

                m_pointers.emplace(it, std::move(pointer));
            }
            record.emplace_back(static_cast<const CBglData*>(data)->Clone());
        }

        auto CBglDirectQmidLayer::RemoveQmid(CPackedQmid qmid) -> void
        {
            const auto it_tile = m_tiles.find(qmid);
            if (it_tile != m_tiles.end())
            {
                m_tiles.erase(it_tile);

                for (auto it = m_pointers.begin(); it != m_pointers.end(); ++it)
                {
                    if (it->get()->QmidLow == qmid.Low() && it->get()->QmidHigh == qmid.High())
                    {
                        m_pointers.erase(it);
                        break;
                    }
                }
            }
        }

        auto CBglDirectQmidLayer::RemoveDataAtQmid(CPackedQmid qmid, int index) -> void
        {
            const auto it = m_tiles.find(qmid);
            if (it != m_tiles.end())
            {
                it->second.erase(it->second.begin() + index);
                if (it->second.empty())
                {
                    RemoveQmid(qmid);
                }
            }
        }

        //******************************************************************************
        // CBglIndirectQmidLayer
        //******************************************************************************

        CBglIndirectQmidLayer::CBglIndirectQmidLayer(const SBglLayerPointer& pointer, EBglLayerType type) :
            CBglLayer(type, EBglLayerClass::GuidIndex, pointer)
        {
        }

        CBglIndirectQmidLayer::CBglIndirectQmidLayer(const CBglIndirectQmidLayer& other) :
            CBglLayer(other.GetType(), other.GetClass(), *other.GetLayerPointer()), m_tiles(other.m_tiles),
            m_pointers(other.m_pointers), m_offsets(other.m_offsets)
        {
            for (const auto& data : other.m_data)
            {
                m_data.emplace_back(data->Clone());
            }
        }

        auto CBglIndirectQmidLayer::ReadBinary(BinaryFileStream& in) -> bool
        {
            const auto* layer_pointer = CBglLayer::GetLayerPointer();
            in.SetPosition(layer_pointer->StreamOffset);

            const auto qmid_count = static_cast<int>(layer_pointer->TileCount);
            const auto base_position = in.GetPosition();

            auto& tiles = m_tiles.write();
            auto max_index = 0;
            for (auto i = 0; i < qmid_count; ++i)
            {
                SBglTilePointer pointer{};
                pointer.ReadBinary(in, false);
                if (!in)
                {
                    return false;
                }
                const auto new_index = static_cast<int>(pointer.StreamOffset + pointer.RecordCount);
                if (new_index > max_index)
                {
                    max_index = new_index;
                }
                tiles[CPackedQmid{pointer.QmidLow, 0}] = pointer;
            }

            if (max_index < 0)
            {
                return false;
            }

            auto& pointers = m_pointers.write();
            pointers.resize(max_index);
            std::map<int, int> offsets_and_sizes;

            for (auto& pointer : pointers)
            {
                pointer.ReadBinary(in);
                if (!in)
                {
                    return false;
                }
                offsets_and_sizes[static_cast<int>(pointer.StreamOffset)] = static_cast<int>(pointer.SizeBytes);
            }

            auto pos = in.GetPosition();
            if (pos != base_position + static_cast<int>(layer_pointer->SizeBytes))
            {
                return false;
            }

            // Now read data
            m_data.resize(offsets_and_sizes.size());
            in.SetPosition(offsets_and_sizes.begin()->first);

            auto i = 0;
            for (auto it = offsets_and_sizes.begin(); it != offsets_and_sizes.end(); ++it)
            {
                pos = in.GetPosition();
                if (pos != it->first)
                {
                    return false;
                }

                auto& data = m_data[i++];
                data = CBglData::Factory(GetType(), IBglSceneryObject::ESceneryObjectType::Unknown);
                if (data == nullptr)
                {
                    continue;
                }

                data->ReadBinary(in);

                if (!data->Validate() || data->CalculateSize() != it->second)
                {
                    return false;
                }
            }

            return true;
        }

        auto CBglIndirectQmidLayer::CalculateSize() const -> int
        {
            auto data_size = 0;
            for (const auto& data : m_data)
            {
                data_size += data->CalculateSize();
            }

            return data_size;
        }

        auto CBglIndirectQmidLayer::CalculateDataPointersSize() const -> int
        {
            return static_cast<int>(sizeof(SBglIndirectPointer) * m_pointers->size()) +
                   static_cast<int>(16 * m_tiles->size()); // TODO Constant
        }

        auto CBglIndirectQmidLayer::WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile, int offset_to_layer)
            -> bool
        {
            auto& data = m_layer_pointer.write();
            data.Type = GetType();
            data.TileCount = GetQmidCount();
            data.DataClass = static_cast<std::underlying_type<EBglLayerType>::type>(EBglLayerClass::IndirectQmid);
            data.HasQmidHigh = 0;
            data.SizeBytes = CalculateDataPointersSize();
            data.StreamOffset = offset_to_tile;

            m_layer_pointer->WriteBinary(out);
            return true;
        }

        auto CBglIndirectQmidLayer::WriteBinaryData(BinaryFileStream& out) -> bool
        {
            auto position = out.GetPosition();
            position += static_cast<int>(sizeof(SBglIndirectPointer) * m_pointers->size());

            // First recalculate indirect pointers to data
            std::vector<std::pair<int, int>> indirect_pointers;
            indirect_pointers.reserve(m_data.size());

            for (const auto& data : m_data)
            {
                const auto data_size = data->CalculateSize();
                indirect_pointers.emplace_back(position, data_size);
                position += data_size;
            }

            for (auto i = 0; i < static_cast<int>(m_offsets->size()); ++i)
            {
                const auto index = m_offsets.read()[i];
                const auto& indirect_pointer = indirect_pointers[index];
                auto& existing_pointer = m_pointers.write()[i];
                existing_pointer.StreamOffset = static_cast<uint32_t>(indirect_pointer.first);
                existing_pointer.SizeBytes = static_cast<uint32_t>(indirect_pointer.second);
            }

            // Then write the data
            for (const auto& data : m_data)
            {
                if (!data->WriteBinary(out))
                {
                    return false;
                }
            }

            return true;
        }

        auto CBglIndirectQmidLayer::WriteBinaryDataPointers(BinaryFileStream& out) -> bool
        {
            // First write tile pointers (which use relative offsets, so must be at front)
            for (const auto& tile : m_tiles.write())
            {
                tile.second.WriteBinary(out, false);
                if (!out)
                {
                    return false;
                }
            }

            // Then write the indirect pointers
            for (auto& pointer : m_pointers.write())
            {
                pointer.WriteBinary(out);
                if (!out)
                {
                    return false;
                }
            }

            if (!out)
            {
                return false;
            }
            return true;
        }

        auto CBglIndirectQmidLayer::GetQmidCount() const -> int { return static_cast<int>(m_tiles->size()); }

        auto CBglIndirectQmidLayer::HasQmid(CPackedQmid qmid) const -> bool
        {
            const auto it = m_tiles->find(qmid);
            if (it != m_tiles->end())
            {
                return true;
            }
            return false;
        }

        auto CBglIndirectQmidLayer::GetDataPointerAtQmid(CPackedQmid qmid) const -> const SBglTilePointer*
        {
            const auto it = m_tiles->find(qmid);
            if (it != m_tiles->end())
            {
                return &it->second;
            }
            return nullptr;
        }

        auto CBglIndirectQmidLayer::GetDataCountAtQmid(CPackedQmid qmid) const -> int
        {
            const auto it = m_tiles->find(qmid);
            if (it != m_tiles->end())
            {
                return static_cast<int>(it->second.RecordCount);
            }
            return 0;
        }

        auto CBglIndirectQmidLayer::GetIndirectPointerCount() const -> int
        {
            return static_cast<int>(m_pointers->size());
        }

        auto CBglIndirectQmidLayer::GetIndirectPointerAtIndex(int index) const -> const SBglIndirectPointer*
        {
            return &m_pointers.read()[index];
        }

        auto CBglIndirectQmidLayer::GetIndirectPointerAtQmid(CPackedQmid qmid, int index) const
            -> const SBglIndirectPointer*
        {
            const auto it = m_tiles->find(qmid);
            if (it != m_tiles->end())
            {
                return &m_pointers.read()[static_cast<int>(it->second.StreamOffset) + index];
            }
            return nullptr;
        }

        auto CBglIndirectQmidLayer::GetDataIndexAtQmid(CPackedQmid qmid, int index) const -> int
        {
            const auto it = m_tiles->find(qmid);
            if (it == m_tiles->end())
            {
                return 0;
            }

            const auto offset = static_cast<int>(it->second.StreamOffset) + index;
            return m_offsets.read()[offset];
        }

        auto CBglIndirectQmidLayer::GetDataCount() const -> int { return static_cast<int>(m_data.size()); }

        auto CBglIndirectQmidLayer::GetDataAtIndex(int index) -> IBglData* { return m_data[index].get(); }

        auto CBglIndirectQmidLayer::AddDataAtQmids(CPackedQmid* qmids, int count, const IBglData* data) -> void
        {
            auto offset = static_cast<int>(m_offsets->size());
            m_data.emplace_back(static_cast<const CBglData*>(data)->Clone());

            for (auto i = 0; i < count; ++i)
            {
                auto& qmid = qmids[i];
                auto it = m_tiles.write().find(qmid);
                if (it == m_tiles->end())
                {
                    m_tiles.write().emplace(qmid, SBglTilePointer{
                                                      qmid.Low(),
                                                      0,
                                                      1,
                                                      static_cast<uint32_t>(m_pointers->size()),
                                                      0,

                                                  });

                    const auto size = static_cast<const CBglData*>(data)->CalculateSize();
                    m_pointers.write().emplace_back(SBglIndirectPointer{0, static_cast<uint32_t>(size)});

                    m_offsets.write().emplace_back(offset);

                    continue;
                }

                auto& tile_pointer = it->second;
                const auto index = tile_pointer.StreamOffset + tile_pointer.RecordCount++;

                while (++it != m_tiles->end())
                {
                    ++it->second.StreamOffset;
                }

                const auto size = static_cast<const CBglData*>(data)->CalculateSize();
                m_pointers.write().emplace(
                    m_pointers.write().begin() + index, SBglIndirectPointer{0, static_cast<uint32_t>(size)});

                m_offsets.write().emplace(m_offsets.write().begin() + index, offset);
            }
        }

        auto CBglIndirectQmidLayer::RemoveDataAtIndex(int index) -> void
        {
            m_data.erase(m_data.begin() + index);

            std::vector<int> offsets;

            for (auto i = 0; i < static_cast<int>(m_offsets->size()); ++i)
            {
                if (m_offsets.read()[i] == index)
                {
                    offsets.emplace_back(i);
                }
            }

            for (auto offset : offsets)
            {
                m_pointers.write().erase(m_pointers.write().begin() + offset);
                m_offsets.write().erase(m_offsets.write().begin() + offset);

                // This is super duper inefficient, but we wouldn't do this often??
                auto found = false;
                for (auto it = m_tiles.write().begin(); it != m_tiles.write().end(); /* no increment */)
                {
                    if (!found)
                    {
                        const auto record_offset = static_cast<int>(it->second.StreamOffset);
                        const auto record_count = static_cast<int>(it->second.RecordCount);

                        if (record_offset >= offset && record_offset + record_count < offset)
                        {
                            found = true;
                            if (--it->second.RecordCount == 0)
                            {
                                it = m_tiles.write().erase(it);
                                continue;
                            }
                        }
                    }
                    else
                    {
                        --it->second.RecordCount;
                    }
                    ++it;
                }
            }
        }

        auto CBglDirectQmidLayer::GetSceneryObjectType(BinaryFileStream& in) -> IBglSceneryObject::ESceneryObjectType
        {
            const auto pos = in.GetPosition();
            auto child_type = uint16_t{};
            auto child_size = uint16_t{};
            in >> child_type;
            in >> child_size;
            in.SetPosition(pos);

            return static_cast<IBglSceneryObject::ESceneryObjectType>(child_type);
        }

        //******************************************************************************
        // CBglGuidLayer
        //******************************************************************************

        CBglGuidLayer::CBglGuidLayer(const SBglLayerPointer& pointer, EBglLayerType type) :
            CBglLayer(type, EBglLayerClass::GuidIndex, pointer)
        {
        }

        CBglGuidLayer::CBglGuidLayer(const CBglGuidLayer& other) :
            CBglLayer(other.GetType(), other.GetClass(), *other.GetLayerPointer()), m_pointer(other.m_pointer),
            m_offsets(other.m_offsets), m_guids(other.m_guids)
        {
            for (const auto& data : other.m_data)
            {
                m_data.emplace_back(data->Clone());
            }
        }

        auto CBglGuidLayer::ReadBinary(BinaryFileStream& in) -> bool
        {
            assert(m_data.empty());

            if (GetClass() != EBglLayerClass::GuidIndex)
            {
                return false;
            }

            in.SetPosition(GetLayerPointer()->StreamOffset);

            m_pointer.write().ReadBinary(in, GetLayerPointer()->HasQmidHigh != 0);

            if (!in)
            {
                return false;
            }

            in.SetPosition(m_pointer->StreamOffset);

            if (!in)
            {
                return false;
            }

            const auto pos = in.GetPosition();

            const auto count = static_cast<int>(m_pointer->RecordCount);
            m_guids.write().resize(count);
            m_data.resize(count);

            for (auto i = 0; i < count; ++i)
            {
                auto& guid_pointer = m_guids.write()[i];
                guid_pointer.ReadBinary(in);

                m_offsets[guid_pointer.Name] = i;
            }

            if (!in)
            {
                return false;
            }

            for (auto i = 0; i < count; ++i)
            {
                const auto& guid_pointer = m_guids.read()[i];
                auto& data = m_data[i];

                data = CBglData::Factory(GetType(), IBglSceneryObject::ESceneryObjectType::Unknown);
                if (data == nullptr)
                {
                    continue;
                }

                in.SetPosition(pos + static_cast<int>(guid_pointer.StreamOffset));
                data->ReadBinary(in);

                if (!data->Validate())
                {
                    return false;
                }
            }

            if (!in)
            {
                return false;
            }

            return true;
        }

        auto CBglGuidLayer::CalculateSize() const -> int
        {
            auto data_size = 0;
            for (const auto& data : m_data)
            {
                data_size += data->CalculateSize();
            }
            data_size += static_cast<int>(sizeof(SBglGuidPointer) * m_guids->size());
            return data_size;
        }

        auto CBglGuidLayer::CalculateDataPointersSize() const -> int
        {
            return 16; // TODO Constant
        }

        auto CBglGuidLayer::WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile, int offset_to_layer) -> bool
        {
            auto& data = m_layer_pointer.write();

            // TODO need to_integral in common header

            data.Type = GetType();
            data.TileCount = 1;
            data.DataClass = static_cast<std::underlying_type<EBglLayerType>::type>(EBglLayerClass::GuidIndex);
            data.HasQmidHigh = 0;
            data.SizeBytes = CalculateDataPointersSize();
            data.StreamOffset = offset_to_tile;
            data.WriteBinary(out);
            return true;
        }

        auto CBglGuidLayer::WriteBinaryData(BinaryFileStream& out) -> bool
        {
            auto& tile_pointer = m_pointer.write();
            const auto count = static_cast<int>(m_offsets.size());
            const auto base_position = out.GetPosition();

            tile_pointer.QmidLow = 0x0;
            tile_pointer.QmidHigh = 0x0;
            tile_pointer.RecordCount = count;
            tile_pointer.StreamOffset = base_position;
            tile_pointer.SizeBytes = CalculateSize();

            // auto guid_offset = base_position + static_cast<int>(sizeof(SBglGuidPointer) * m_guids->size());
            auto guid_offset = static_cast<int>(sizeof(SBglGuidPointer) * m_guids->size());

            for (auto i = 0; i < count; ++i)
            {
                auto& guid_pointer = m_guids.write()[i];
                const auto size = m_data[i]->CalculateSize();

                guid_pointer.StreamOffset = guid_offset;
                guid_pointer.SizeBytes = size;

                guid_offset += size;

                guid_pointer.WriteBinary(out);
            }

            for (auto& data : m_data)
            {
                data->WriteBinary(out);
            }

            if (!out)
            {
                return false;
            }
            return true;
        }

        auto CBglGuidLayer::WriteBinaryDataPointers(BinaryFileStream& out) -> bool
        {
            m_pointer->WriteBinary(out, m_layer_pointer->HasQmidHigh);

            if (!out)
            {
                return false;
            }

            return true;
        }

        auto CBglGuidLayer::GetGuidCount() const -> int { return static_cast<int>(m_offsets.size()); }

        auto CBglGuidLayer::GetDataPointer() const -> const SBglTilePointer* { return &m_pointer.read(); }

        auto CBglGuidLayer::GetGuidPointerAt(int index) const -> const SBglGuidPointer*
        {
            return &m_guids.read()[index];
        }

        auto CBglGuidLayer::HasGuid(_GUID guid) const -> bool
        {
            const auto it = m_offsets.find(guid);
            if (it != m_offsets.end())
            {
                return true;
            }
            return false;
        }

        auto CBglGuidLayer::GetData(_GUID guid) const -> const IBglData*
        {
            // TODO - need a non-const version
            const auto it = m_offsets.find(guid);
            if (it != m_offsets.end())
            {
                return m_data[it->second].get();
            }
            return nullptr;
        }

        auto CBglGuidLayer::AddData(_GUID guid, const IBglData* data) -> void
        {
            const auto it = m_offsets.find(guid);
            if (it == m_offsets.end())
            {
                const auto offset = static_cast<int>(m_offsets.size());
                m_offsets[guid] = offset;
                m_guids.write().emplace_back(SBglGuidPointer{guid, 0, 0});
                m_data.emplace_back(static_cast<const CBglData*>(data)->Clone());
                ++m_pointer.write().RecordCount;
            }
        }

        auto CBglGuidLayer::RemoveData(_GUID guid) -> void
        {
            const auto& it = m_offsets.find(guid);
            if (it != m_offsets.end())
            {
                m_guids.write().erase(m_guids->begin() + it->second);
                m_data.erase(m_data.begin() + it->second);
                m_offsets.erase(it);
                --m_pointer.write().RecordCount;
            }
        }

        //******************************************************************************
        // CBglExclusionLayer
        //******************************************************************************

        CBglExclusionLayer::CBglExclusionLayer(const SBglLayerPointer& pointer) :
            CBglLayer(EBglLayerType::Exclusion, EBglLayerClass::Exclusion, pointer)
        {
        }

        CBglExclusionLayer::CBglExclusionLayer(const CBglExclusionLayer& other) :
            CBglLayer(other.GetType(), other.GetClass(), *other.GetLayerPointer()), m_pointer(other.m_pointer),
            m_exclusions(other.m_exclusions)
        {
        }

        auto CBglExclusionLayer::ReadBinary(BinaryFileStream& in) -> bool
        {
            assert(m_exclusions->empty());

            if (CBglLayer::GetType() != EBglLayerType::Exclusion)
            {
                return false;
            }

            in.SetPosition(GetLayerPointer()->StreamOffset);

            m_pointer.write().ReadBinary(in, CBglLayer::GetLayerPointer()->HasQmidHigh != 0);

            if (!in)
            {
                return false;
            }

            const auto count = static_cast<int>(m_pointer->RecordCount);
            m_exclusions.write().resize(count);

            for (auto i = 0; i < count; ++i)
            {
                auto& exclusion = m_exclusions.write()[i];
                exclusion.ReadBinary(in);
                if (!exclusion.Validate())
                {
                    return false;
                }
            }

            if (!in)
            {
                return false;
            }

            return true;
        }

        auto CBglExclusionLayer::CalculateSize() const -> int
        {
            auto data_size = 0;
            for (const auto& tile : m_exclusions.read())
            {
                data_size += tile.CalculateSize();
            }
            return data_size;
        }

        auto CBglExclusionLayer::CalculateDataPointersSize() const -> int
        {
            return 16; // TODO Constant
        }

        auto CBglExclusionLayer::WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile, int offset_to_layer)
            -> bool
        {
            auto& data = m_layer_pointer.write();

            // TODO need to_integral in common header

            data.Type = EBglLayerType::Exclusion;
            data.TileCount = 1;
            data.DataClass = static_cast<std::underlying_type<EBglLayerType>::type>(EBglLayerClass::Exclusion);
            data.HasQmidHigh = 0;
            data.SizeBytes = CalculateDataPointersSize();
            data.StreamOffset = offset_to_tile;
            data.WriteBinary(out);
            return true;
        }

        auto CBglExclusionLayer::WriteBinaryData(BinaryFileStream& out) -> bool
        {
            auto& pointer = m_pointer.write();

            pointer.QmidLow = 0x2;
            pointer.QmidHigh = 0x0;
            pointer.RecordCount = static_cast<int>(m_exclusions->size());
            pointer.StreamOffset = out.GetPosition();
            pointer.SizeBytes = CalculateSize();

            for (auto& tile : m_exclusions.write())
            {
                tile.WriteBinary(out);
            }

            if (!out)
            {
                return false;
            }
            return true;
        }

        auto CBglExclusionLayer::WriteBinaryDataPointers(BinaryFileStream& out) -> bool
        {
            m_pointer->WriteBinary(out, m_layer_pointer->HasQmidHigh);

            if (!out)
            {
                return false;
            }

            return true;
        }

        auto CBglExclusionLayer::GetExclusionCount() const -> int { return static_cast<int>(m_exclusions->size()); }

        auto CBglExclusionLayer::GetDataPointer() const -> const SBglTilePointer* { return &m_pointer.read(); }

        auto CBglExclusionLayer::GetExclusionAt(int index) -> IBglExclusion* { return &m_exclusions.write()[index]; }

        auto CBglExclusionLayer::AddExclusion(const IBglExclusion* exclusion) -> void
        {
            m_exclusions.write().emplace_back(*static_cast<const CBglExclusion*>(exclusion));
        }

        auto CBglExclusionLayer::RemoveExclusion(const IBglExclusion* exclusion) -> void
        {
            // This uses a unique_ptr - need to find a way to pass these safely via interface
            const auto iter = m_exclusions.read().begin() +
                              std::distance(m_exclusions.read().data(), static_cast<const CBglExclusion*>(exclusion));
            m_exclusions.write().erase(iter);
        }

        //******************************************************************************
        // CBglTimezoneLayer
        //******************************************************************************

        CBglTimeZoneLayer::CBglTimeZoneLayer(const SBglLayerPointer& pointer) :
            CBglLayer(EBglLayerType::TimeZone, EBglLayerClass::TimeZone, pointer)
        {
        }

        CBglTimeZoneLayer::CBglTimeZoneLayer(const CBglTimeZoneLayer& other) :
            CBglLayer(other.GetType(), other.GetClass(), *other.GetLayerPointer()), m_pointer(other.m_pointer),
            m_timezones(other.m_timezones)
        {
        }

        auto CBglTimeZoneLayer::ReadBinary(BinaryFileStream& in) -> bool
        {
            assert(m_timezones->empty());

            if (CBglLayer::GetType() != EBglLayerType::TimeZone)
            {
                return false;
            }

            in.SetPosition(GetLayerPointer()->StreamOffset);

            m_pointer.write().ReadBinary(in, CBglLayer::GetLayerPointer()->HasQmidHigh != 0);

            if (!in)
            {
                return false;
            }

            const auto count = static_cast<int>(m_pointer->RecordCount);
            m_timezones.write().resize(count);

            // TODO - hardcoded FSX
            assert(m_pointer->SizeBytes / 24 == count);

            for (auto i = 0; i < count; ++i)
            {
                auto& timezone = m_timezones.write()[i];
                timezone.ReadBinary(in);
                if (!timezone.Validate())
                {
                    return false;
                }
            }

            if (!in)
            {
                return false;
            }

            return true;
        }

        auto CBglTimeZoneLayer::CalculateSize() const -> int
        {
            auto data_size = 0;
            for (const auto& tile : m_timezones.read())
            {
                data_size += tile.CalculateSize();
            }
            return data_size;
        }

        auto CBglTimeZoneLayer::CalculateDataPointersSize() const -> int
        {
            return 16; // TODO Constant
        }

        auto CBglTimeZoneLayer::WriteBinaryPointer(BinaryFileStream& out, int offset_to_tile, int offset_to_layer)
            -> bool
        {
            auto& data = m_layer_pointer.write();

            // TODO need to_integral in common header

            data.Type = EBglLayerType::TimeZone;
            data.TileCount = 1;
            data.DataClass = static_cast<std::underlying_type<EBglLayerType>::type>(EBglLayerClass::TimeZone);
            data.HasQmidHigh = 0;
            data.SizeBytes = CalculateDataPointersSize();
            data.StreamOffset = offset_to_tile;
            data.WriteBinary(out);
            return true;
        }

        auto CBglTimeZoneLayer::WriteBinaryData(BinaryFileStream& out) -> bool
        {
            auto& pointer = m_pointer.write();

            pointer.QmidLow = 0x2;
            pointer.QmidHigh = 0x0;
            pointer.RecordCount = static_cast<int>(m_timezones->size());
            pointer.StreamOffset = out.GetPosition();
            pointer.SizeBytes = CalculateSize();

            for (auto& tile : m_timezones.write())
            {
                tile.WriteBinary(out);
            }

            if (!out)
            {
                return false;
            }
            return true;
        }

        auto CBglTimeZoneLayer::WriteBinaryDataPointers(BinaryFileStream& out) -> bool
        {
            m_pointer->WriteBinary(out, m_layer_pointer->HasQmidHigh);

            if (!out)
            {
                return false;
            }

            return true;
        }

        auto CBglTimeZoneLayer::GetTimeZoneCount() const -> int { return static_cast<int>(m_timezones->size()); }

        auto CBglTimeZoneLayer::GetDataPointer() const -> const SBglTilePointer* { return &m_pointer.read(); }

        auto CBglTimeZoneLayer::GetTimeZoneAt(int index) -> IBglTimeZone* { return &m_timezones.write()[index]; }

        auto CBglTimeZoneLayer::AddTimeZone(const IBglTimeZone* timezone) -> void
        {
            m_timezones.write().emplace_back(*static_cast<const CBglTimeZone*>(timezone));
        }

        auto CBglTimeZoneLayer::RemoveTimeZone(const IBglTimeZone* timezone) -> void
        {
            // This uses a unique_ptr - need to find a way to pass these safely via interface
            const auto iter = m_timezones.read().begin() +
                              std::distance(m_timezones.read().data(), static_cast<const CBglTimeZone*>(timezone));
            m_timezones.write().erase(iter);
        }

        //******************************************************************************
        // CBglFile
        //******************************************************************************

        CBglFile::CBglFile() : CBglFile(L"") { }

        CBglFile::CBglFile(std::wstring file_name) :
            m_file_name(std::move(file_name)), m_file_size(0), m_header(), m_dirty(false), m_stream(file_name)
        {
        }

        auto CBglFile::GetLayerCount() const -> int { return static_cast<int>(m_layers.size()); }

        auto CBglFile::HasLayer(EBglLayerType type) const -> bool
        {
            const auto it = m_layer_offsets.find(type);
            if (it != m_layer_offsets.end())
            {
                return true;
            }
            return false;
        }

        auto CBglFile::GetLayerAt(int index) -> IBglLayer* { return m_layers[index].get(); }

        auto CBglFile::GetIndirectQmidLayer(EBglLayerType type) -> IBglIndirectQmidLayer*
        {
            const auto it = m_layer_offsets.find(type);
            if (it == m_layer_offsets.end())
            {
                return nullptr;
            }
            return m_layers[it->second]->AsIndirectQmidLayer();
        }

        auto CBglFile::GetDirectQmidLayer(EBglLayerType type) -> IBglDirectQmidLayer*
        {
            const auto it = m_layer_offsets.find(type);
            if (it == m_layer_offsets.end())
            {
                return nullptr;
            }
            return m_layers[it->second]->AsDirectQmidLayer();
        }

        auto CBglFile::GetNameListLayer() -> IBglNameListLayer*
        {
            const auto it = m_layer_offsets.find(EBglLayerType::NameList);
            if (it == m_layer_offsets.end())
            {
                return nullptr;
            }
            return m_layers[it->second]->AsNameListLayer();
        }

        auto CBglFile::GetIcaoLayer(EBglLayerType type) -> IBglIcaoLayer*
        {
            const auto it = m_layer_offsets.find(type);
            if (it == m_layer_offsets.end())
            {
                return nullptr;
            }
            return m_layers[it->second]->AsIcaoLayer();
        }

        auto CBglFile::GetGuidLayer(EBglLayerType type) -> IBglGuidLayer*
        {
            const auto it = m_layer_offsets.find(type);
            if (it == m_layer_offsets.end())
            {
                return nullptr;
            }
            return m_layers[it->second]->AsGuidLayer();
        }

        auto CBglFile::GetExclusionLayer() -> IBglExclusionLayer*
        {
            const auto it = m_layer_offsets.find(EBglLayerType::Exclusion);
            if (it == m_layer_offsets.end())
            {
                return nullptr;
            }
            return m_layers[it->second]->AsExclusionLayer();
        }

        auto CBglFile::GetTimeZoneLayer() -> IBglTimeZoneLayer*
        {
            const auto it = m_layer_offsets.find(EBglLayerType::TimeZone);
            if (it == m_layer_offsets.end())
            {
                return nullptr;
            }
            return m_layers[it->second]->AsTimeZoneLayer();
        }

        bool CBglFile::Open()
        {
            if (m_stream.IsOpen())
            {
                return true;
            }
            m_stream.Open(m_file_name);
            if (!m_stream)
            {
                return false;
            }
            m_stream.SetPosition(0, std::fstream::end);
            m_file_size = m_stream.GetPosition();
            m_stream.SetPosition(0, std::fstream::beg);
            return true;
        }

        bool CBglFile::Close()
        {
            m_stream.Close();
            return m_stream ? true : false;
        }

        bool CBglFile::Write()
        {
            if (m_stream.IsOpen())
            {
                m_stream.Close();
            }
            m_stream.Open(
                m_file_name, std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::trunc);
            if (!m_stream)
            {
                return false;
            }
            if (!WriteHeader())
            {
                return false;
            }
            if (!WriteAllLayers())
            {
                return false;
            }
            m_dirty = false;
            return true;
        }

        bool CBglFile::Read()
        {
            if (!m_layers.empty())
            {
                return false;
            }
            if (!Open())
            {
                return false;
            }
            if (!ReadHeader())
            {
                return false;
            }
            return ReadAllLayers();
        }

        auto CBglFile::GetFileName() const -> const wchar_t* { return m_file_name.c_str(); }

        void CBglFile::Rename(const wchar_t* file_name)
        {
            m_file_name = file_name;
            m_dirty = true;
        }

        bool CBglFile::TryMergeLayer(IBglLayer* layer)
        {
            const auto it = m_layer_offsets.find(layer->GetType());
            if (it == m_layer_offsets.end())
            {
                // TODO - This currently fails if the layer exists
                //  m_layers[it->second] = ;
                //  m_layers.emplace_back(static_cast<CBglLayer*>(layer));
                m_layers.emplace_back(layer->CloneLayer());
                m_layer_offsets.emplace(layer->GetType(), static_cast<int>(m_layers.size()));
                m_dirty = true;
                return true;
            }
            return false;
        }

        void CBglFile::RemoveLayer(EBglLayerType type)
        {
            const auto it = m_layer_offsets.find(type);
            if (it != m_layer_offsets.end())
            {
                // TODO - this keeps a nullptr in the vector, otherwise we would have to shift all offsets
                m_layers[it->second].reset();
                m_layer_offsets.erase(it);
                m_dirty = true;
            }
        }

        bool CBglFile::IsDirty() const { return m_dirty; }

        int CBglFile::GetFileSize() const { return m_file_size; }

        bool CBglFile::ReadAllLayers()
        {
            if (m_stream.GetPosition() != static_cast<int>(m_header.HeaderSize))
            {
                m_stream.SetPosition(m_header.HeaderSize);
                if (!m_stream)
                {
                    return false;
                }
            }
            const auto count = static_cast<int>(m_header.LayerCount);
            // remove static size to prevent nullptr layers
            // m_layers.resize(count);
            m_layers.reserve(count);

            for (auto i = 0; i < count; ++i)
            {
                auto layer_pointer = SBglLayerPointer{};
                layer_pointer.ReadBinary(m_stream);

                const auto next_position = m_stream.GetPosition();

                auto layer = CBglLayer::Factory(layer_pointer);
                if (layer == nullptr)
                {
                    continue; // TODO - Add an unknown layer type? So we keep the pointer
                }
                if (!layer->ReadBinary(m_stream))
                {
                    return false;
                }
                if (!m_stream)
                {
                    return false;
                }

                m_layer_offsets[layer->GetType()] = static_cast<int>(m_layers.size());
                m_layers.emplace_back(std::move(layer));

                m_stream.SetPosition(next_position);
            }
            return true;
        }

        // File order for resample is currently all layers after header (required),
        // then all data, then all QMID pointers to tile data
        // Although this may just be a compiler constraint. bglcomp for example looks different
        bool CBglFile::WriteAllLayers()
        {
            if (m_layers.size() != m_header.LayerCount ||
                m_stream.GetPosition() != static_cast<int>(m_header.HeaderSize))
            {
                return false;
            }
            auto data_size = HeaderSize();
            data_size += CBglLayer::CalculateLayerPointerSize() * m_header.LayerCount;

            std::vector<int> layer_positions;
            layer_positions.reserve(m_layers.size());
            for (const auto& layer : m_layers)
            {
                layer_positions.emplace_back(data_size);
                const auto size = layer->CalculateSize();
                data_size += size;
            }
            auto i = 0;
            for (const auto& layer : m_layers)
            {
                if (!layer->WriteBinaryPointer(m_stream, data_size, layer_positions[i++]) || !m_stream)
                {
                    return false;
                }
                data_size += layer->CalculateDataPointersSize();
            }
            for (const auto& layer : m_layers)
            {
                if (!layer->WriteBinaryData(m_stream) || !m_stream)
                {
                    return false;
                }
            }
            for (const auto& layer : m_layers)
            {
                if (!layer->WriteBinaryDataPointers(m_stream) && !m_stream)
                {
                    return false;
                }
            }
            return true;
        }

        bool CBglFile::ReadHeader()
        {
            if (!m_stream)
            {
                return false;
            }
            m_stream.SetPosition(0, std::fstream::beg);
            m_header = SBglHeader::ReadBinary(m_stream);
            if (!m_stream)
            {
                return false;
            }
            if (m_header.Version != Version())
            {
                return false;
            }
            if (m_header.FileMagic != FileMagic())
            {
                return false;
            }
            if (m_header.HeaderSize != HeaderSize())
            {
                return false;
            }
            if (m_header.QmidMagic != QmidMagic())
            {
                return false;
            }
            const auto count = static_cast<int>(m_header.LayerCount);
            if (count < 0 || count > std::numeric_limits<int32_t>::max())
            {
                return false;
            }
            return true;
        }

        bool CBglFile::WriteHeader()
        {
            m_stream.SetPosition(0, std::fstream::beg);
            if (!m_stream)
            {
                return false;
            }
            if (m_dirty)
            {
                BuildHeader();
            }
            SBglHeader::WriteBinary(m_stream, m_header);
            return m_stream ? true : false;
        }

        bool CBglFile::BuildHeader()
        {
            m_header.Version = Version();
            m_header.FileMagic = FileMagic();
            m_header.HeaderSize = HeaderSize();
            m_header.FileTime = 0; // TODO FILETIME library
            m_header.QmidMagic = QmidMagic();
            m_header.LayerCount = static_cast<uint32_t>(m_layers.size());
            return ComputeHeaderQmids();
        }

        // TODO Build Qmid Algorithm
        bool CBglFile::ComputeHeaderQmids()
        {
            m_header.PackedQMIDParent0 = 0;
            m_header.PackedQMIDParent1 = 0;
            m_header.PackedQMIDParent2 = 0;
            m_header.PackedQMIDParent3 = 0;
            m_header.PackedQMIDParent4 = 0;
            m_header.PackedQMIDParent5 = 0;
            m_header.PackedQMIDParent6 = 0;
            m_header.PackedQMIDParent7 = 0;
            return true;
        }

    } // namespace io

} // namespace flightsimlib
