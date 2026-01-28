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

#ifndef FLIGHTSIMLIB_IO_BGLDATA_H
#define FLIGHTSIMLIB_IO_BGLDATA_H

#include "BglTypes.h"
#include "../external/stlab/copy_on_write.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// This is because of the mixins for SceneryObject
#pragma warning(disable : 4250)

namespace flightsimlib
{

    namespace io
    {

        //******************************************************************************
        // IBglSerializable
        //******************************************************************************

        // Forward Declarations
        class BinaryFileStream;
        class CBglSceneryObject;

        class IBglSerializable
        {
          public:
            virtual ~IBglSerializable() = default; // TODO necessary?
            virtual auto ReadBinary(BinaryFileStream& in) -> void = 0;
            virtual auto WriteBinary(BinaryFileStream& out) -> void = 0;
            virtual auto Validate() -> bool = 0;
            virtual auto CalculateSize() const -> int = 0;
        };

        //******************************************************************************
        // Common
        //******************************************************************************

        // TODO: Move to Utility

        class PackedAltitude
        {
          public:
            PackedAltitude(uint32_t value) : m_Value(value) { }

            static double Value(uint32_t value) { return value / 1000.0; }

            static uint32_t FromDouble(double value) { return static_cast<uint32_t>(value * 1000.0); }

            double Value() const { return Value(m_Value); }

          private:
            uint32_t m_Value;
        };

        class ANGLE16
        {
          public:
            ANGLE16(uint16_t value) : m_Value(value) { }

            static double Value(uint16_t value) { return value * 360.0 / 0x10000; }

            static uint16_t FromDouble(double value) { return static_cast<uint16_t>(0x10000 / 360.0 * value); }

            double Value() const { return Value(m_Value); }

          private:
            uint16_t m_Value;
        };

        class Latitude
        {
          public:
            Latitude(uint32_t value) : m_Value(Value(value)) { }

            Latitude(double value) : m_Value(value) { }

            static double Value(uint32_t value) { return 90.0 - value * (180.0 / 0x20000000); }

            static uint32_t ToPacked(double value)
            {
                return static_cast<uint32_t>((90.0 - value) / (180.0 / 0x20000000));
            }

            double Value() const { return m_Value; }

            uint32_t ToPacked() const { return ToPacked(m_Value); }

            double m_Value;
        };

        class Longitude
        {
          public:
            Longitude(uint32_t value) : m_Value(Value(value)) { }

            Longitude(double value) : m_Value(value) { }

            static double Value(uint32_t value) { return (value * (360.0 / 0x30000000)) - 180.0; }

            static uint32_t ToPacked(double value)
            {
                return static_cast<uint32_t>((180.0 + value) / (360.0 / 0x30000000));
            }

            double Value() const { return m_Value; }

            uint32_t ToPacked() const { return ToPacked(m_Value); }

            double m_Value;
        };

        //******************************************************************************
        // CBglFuelAvailability
        //******************************************************************************

        template <typename T> class CBglFuelAvailability : virtual public IBglFuelAvailability
        {
          public:
            explicit CBglFuelAvailability(T& data);

            auto Get73Octane() const -> EFuelAvailability override;
            auto Set73Octane(EFuelAvailability value) -> void override;
            auto Get87Octane() const -> EFuelAvailability override;
            auto Set87Octane(EFuelAvailability value) -> void override;
            auto Get100Octane() const -> EFuelAvailability override;
            auto Set100Octane(EFuelAvailability value) -> void override;
            auto Get130Octane() const -> EFuelAvailability override;
            auto Set130Octane(EFuelAvailability value) -> void override;
            auto Get145Octane() const -> EFuelAvailability override;
            auto Set145Octane(EFuelAvailability value) -> void override;
            auto GetMogas() const -> EFuelAvailability override;
            auto SetMogas(EFuelAvailability value) -> void override;
            auto GetJet() const -> EFuelAvailability override;
            auto SetJet(EFuelAvailability value) -> void override;
            auto GetJetA() const -> EFuelAvailability override;
            auto SetJetA(EFuelAvailability value) -> void override;
            auto GetJetA1() const -> EFuelAvailability override;
            auto SetJetA1(EFuelAvailability value) -> void override;
            auto GetJetAP() const -> EFuelAvailability override;
            auto SetJetAP(EFuelAvailability value) -> void override;
            auto GetJetB() const -> EFuelAvailability override;
            auto SetJetB(EFuelAvailability value) -> void override;
            auto GetJet4() const -> EFuelAvailability override;
            auto SetJet4(EFuelAvailability value) -> void override;
            auto GetJet5() const -> EFuelAvailability override;
            auto SetJet5(EFuelAvailability value) -> void override;
            auto HasAvgas() const -> bool override;
            auto HasJetFuel() const -> bool override;

          private:
            auto UpdateAvgasAvailability(EFuelAvailability value);
            auto UpdateJetFuelAvailability(EFuelAvailability value);

            enum class EFuelBits
            {
                Octane73 = 0,
                Octane87 = 2,
                Octane100 = 4,
                Octane130 = 6,
                Octane145 = 8,
                Mogas = 10,
                Jet = 12,
                JetA = 14,
                JetA1 = 16,
                JetAP = 18,
                JetB = 20,
                Jet4 = 22,
                Jet5 = 24,
                Reserved1 = 26,
                Reserved2 = 28,
                Avgas = 30,
                JetFuel = 31
            };

            static constexpr int s_num_availability_bits = 2;

            T& m_data;
        };

        //******************************************************************************
        // CBglName
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglNameData
        {
            uint16_t Type;
            uint32_t Size;
            std::string Name;
        };

#pragma pack(pop)

        class CBglName : public IBglSerializable, virtual public IBglName
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetName() const -> const char* override;
            auto SetName(const char* value) -> void override;

            auto IsEmpty() const -> bool;

          private:
            auto CalculateRemainingSize() const -> int;

            stlab::copy_on_write<SBglNameData> m_data;
        };

        //******************************************************************************
        // CBglLla
        //******************************************************************************

        template <typename T> class CBglLLA : virtual public IBglLLA
        {
          public:
            explicit CBglLLA(T& data);

            auto GetLongitude() const -> double override;
            auto SetLongitude(double value) -> void override;
            auto GetLatitude() const -> double override;
            auto SetLatitude(double value) -> void override;
            auto GetAltitude() const -> double override;
            auto SetAltitude(double value) -> void override;

          private:
            std::reference_wrapper<T> m_data;
        };

        //******************************************************************************
        // CBglNdb
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglNdbData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t NdbType;
            uint32_t Frequency;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            float Range;
            float MagVar;
            uint32_t Icao;
            uint32_t Region;
        };

#pragma pack(pop)

        class CBglNdb final : public CBglLLA<stlab::copy_on_write<SBglNdbData>>, public CBglName, public IBglNdb
        {
          public:
            CBglNdb() : CBglLLA<stlab::copy_on_write<SBglNdbData>>(m_data) { }

            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetFrequency() const -> uint32_t override;
            auto SetFrequency(uint32_t value) -> void override;
            auto GetRange() const -> float override;
            auto SetRange(float value) -> void override;
            auto GetMagVar() const -> float override;
            auto SetMagVar(float value) -> void override;
            auto GetIcao() const -> uint32_t override;
            auto SetIcao(uint32_t value) -> void override;
            auto GetRegion() const -> uint32_t override;
            auto SetRegion(uint32_t value) -> void override;

          private:
            stlab::copy_on_write<SBglNdbData> m_data;
        };

        //******************************************************************************
        // CBglRunway
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglRunwayData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t SurfaceType;
            uint8_t NumberPrimary;
            uint8_t DesignatorPrimary;
            uint8_t NumberSecondary;
            uint8_t DesignatorSecondary;
            uint32_t IlsIcaoPrimary;
            uint32_t IlsIcaoSecondary;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            float Length;
            float Width;
            float Heading;
            float PatternAltitude;
            uint16_t MarkingFlags;
            uint8_t LightFlags;
            uint8_t PatternFlags;
        };

#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)

        struct SBglRunwayEndData
        {
            uint16_t Position;
            uint32_t Size;
            uint16_t SurfaceType;
            float Length;
            float Width;
        };

#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)

        struct SBglRunwayVasiData
        {
            uint16_t Position;
            uint32_t Size;
            uint16_t Type;
            float BiasX;
            float BiasZ;
            float Spacing;
            float Pitch;
        };

#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)

        struct SBglRunwayApproachLightsData
        {
            uint16_t Position;
            uint32_t Size;
            uint8_t Type;
            uint8_t Strobes;
        };

#pragma pack(pop)

        class CBglRunway final : public CBglLLA<stlab::copy_on_write<SBglRunwayData>>,
                                 public IBglSerializable,
                                 public IBglRunway
        {
          public:
            CBglRunway() : CBglLLA<stlab::copy_on_write<SBglRunwayData>>(m_data) { }

            class CBglRunwayEnd final : public IBglSerializable, public IBglRunwayEnd
            {
              public:
                auto ReadBinary(BinaryFileStream& in) -> void override;
                auto WriteBinary(BinaryFileStream& out) -> void override;
                auto Validate() -> bool override;
                auto CalculateSize() const -> int override;

                auto GetPosition() const -> EPosition override;
                auto GetSurfaceType() const -> ESurfaceType override;
                auto SetSurfaceType(ESurfaceType value) -> void override;
                auto GetLength() const -> float override;
                auto SetLength(float value) -> void override;
                auto GetWidth() const -> float override;
                auto SetWidth(float value) -> void override;

                // public but not part of client interface:
                auto IsEmpty() const -> bool;
                auto SetPosition(EPosition value) -> void;

              private:
                stlab::copy_on_write<SBglRunwayEndData> m_data;
            };

            class CBglRunwayVasi final : public IBglSerializable, public IBglRunwayVasi
            {
              public:
                auto ReadBinary(BinaryFileStream& in) -> void override;
                auto WriteBinary(BinaryFileStream& out) -> void override;
                auto Validate() -> bool override;
                auto CalculateSize() const -> int override;

                auto GetPosition() const -> EPosition override;
                auto GetType() const -> EType override;
                auto SetType(EType value) -> void override;
                auto GetBiasX() const -> float override;
                auto SetBiasX(float value) -> void override;
                auto GetBiasZ() const -> float override;
                auto SetBiasZ(float value) -> void override;
                auto GetSpacing() const -> float override;
                auto SetSpacing(float value) -> void override;
                auto GetPitch() const -> float override;
                auto SetPitch(float value) -> void override;

                // public but not part of client interface:
                auto IsEmpty() const -> bool;
                auto SetPosition(EPosition value) -> void;

              private:
                stlab::copy_on_write<SBglRunwayVasiData> m_data;
            };

            class CBglRunwayApproachLights final : public IBglSerializable, public IBglRunwayApproachLights
            {
              public:
                auto ReadBinary(BinaryFileStream& in) -> void override;
                auto WriteBinary(BinaryFileStream& out) -> void override;
                auto Validate() -> bool override;
                auto CalculateSize() const -> int override;

                auto GetPosition() const -> EPosition override;
                auto GetType() const -> EType override;
                auto SetType(EType value) -> void override;
                auto GetStrobeCount() const -> int override;
                auto SetStrobeCount(int value) -> void override;
                auto HasEndLights() const -> bool override;
                auto SetEndLights(bool value) -> void override;
                auto HasReilLights() const -> bool override;
                auto SetReilLights(bool value) -> void override;
                auto HasTouchdownLights() const -> bool override;
                auto SetTouchdownLights(bool value) -> void override;

                // public but not part of client interface:
                auto IsEmpty() const -> bool;
                auto SetPosition(EPosition value) -> void;

              private:
                stlab::copy_on_write<SBglRunwayApproachLightsData> m_data;
            };

          private:
            enum class EMarkingFlags : uint16_t
            {
                Edges = 0,
                Threshold = 1,
                FixedDistance = 2,
                Touchdown = 3,
                Dashes = 4,
                Ident = 5,
                Precision = 6,
                EdgePavement = 7,
                SingleEnd = 8,
                PrimaryClosed = 9,
                SecondaryClosed = 10,
                PrimaryStol = 11,
                SecondaryStol = 12,
                AlternateThreshold = 13,
                AlternateFixedDistance = 14,
                AlternateTouchdown = 15,
            };

            enum class ELightFlags : uint8_t
            {
                Edge = 0,
                Center = 2,
                CenterRed = 4,
                AlternatePrecision = 5,
                LeadingZeroIdent = 6,
                NoThresholdEndArrows = 7
            };

            enum class EPatternFlags : uint8_t
            {
                PrimaryTakeoff = 0,
                PrimaryLanding = 1,
                PrimaryPattern = 2,
                SecondaryTakeoff = 3,
                SecondaryLanding = 4,
                SecondaryPattern = 5
            };

          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetSurfaceType() const -> ESurfaceType override;
            auto SetSurfaceType(ESurfaceType value) -> void override;
            auto GetPrimaryRunwayNumber() const -> ERunwayNumber override;
            auto SetPrimaryRunwayNumber(ERunwayNumber value) -> void override;
            auto GetPrimaryRunwayDesignator() const -> ERunwayDesignator override;
            auto SetPrimaryRunwayDesignator(ERunwayDesignator value) -> void override;
            auto GetSecondaryRunwayNumber() const -> ERunwayNumber override;
            auto SetSecondaryRunwayNumber(ERunwayNumber value) -> void override;
            auto GetSecondaryRunwayDesignator() const -> ERunwayDesignator override;
            auto SetSecondaryRunwayDesignator(ERunwayDesignator value) -> void override;
            auto GetPrimaryIcaoIdent() const -> uint32_t override;
            auto SetPrimaryIcaoIdent(uint32_t value) -> void override;
            auto GetSecondaryIcaoIdent() const -> uint32_t override;
            auto SetSecondaryIcaoIdent(uint32_t value) -> void override;
            auto GetLength() const -> float override;
            auto SetLength(float value) -> void override;
            auto GetWidth() const -> float override;
            auto SetWidth(float value) -> void override;
            auto GetHeading() const -> float override;
            auto SetHeading(float value) -> void override;
            auto GetPatternAltitude() const -> float override;
            auto SetPatternAltitude(float value) -> void override;
            auto HasEdgeMarkings() const -> bool override;
            auto SetEdgeMarkings(bool value) -> void override;
            auto HasThresholdMarkings() const -> bool override;
            auto SetThresholdMarkings(bool value) -> void override;
            auto HasFixedDistanceMarkings() const -> bool override;
            auto SetFixedDistanceMarkings(bool value) -> void override;
            auto HasTouchdownMarkings() const -> bool override;
            auto SetTouchdownMarkings(bool value) -> void override;
            auto HasDashMarkings() const -> bool override;
            auto SetDashMarkings(bool value) -> void override;
            auto HasIdentMarkings() const -> bool override;
            auto SetIdentMarkings(bool value) -> void override;
            auto HasPrecisionMarkings() const -> bool override;
            auto SetPrecisionMarkings(bool value) -> void override;
            auto HasEdgePavement() const -> bool override;
            auto SetEdgePavement(bool value) -> void override;
            auto IsSingleEnd() const -> bool override;
            auto SetSingleEnd(bool value) -> void override;
            auto IsPrimaryClosed() const -> bool override;
            auto SetPrimaryClosed(bool value) -> void override;
            auto IsSecondaryClosed() const -> bool override;
            auto SetSecondaryClosed(bool value) -> void override;
            auto IsPrimaryStol() const -> bool override;
            auto SetPrimaryStol(bool value) -> void override;
            auto IsSecondaryStol() const -> bool override;
            auto SetSecondaryStol(bool value) -> void override;
            auto HasAlternateThreshold() const -> bool override;
            auto SetAlternateThreshold(bool value) -> void override;
            auto HasAlternateFixedDistance() const -> bool override;
            auto SetAlternateFixedDistance(bool value) -> void override;
            auto HasAlternateTouchDown() const -> bool override;
            auto SetAlternateTouchDown(bool value) -> void override;
            auto HasAlternatePrecision() const -> bool override;
            auto SetAlternatePrecision(bool value) -> void override;
            auto HasLeadingZeroIdent() const -> bool override;
            auto SetLeadingZeroIdent(bool value) -> void override;
            auto HasNoThresholdEndArrows() const -> bool override;
            auto SetNoThresholdEndArrows(bool value) -> void override;
            auto GetEdgeLights() const -> ELightIntensity override;
            auto SetEdgeLights(ELightIntensity value) -> void override;
            auto GetCenterLights() const -> ELightIntensity override;
            auto SetCenterLights(ELightIntensity value) -> void override;
            auto IsCenterRedLights() const -> bool override;
            auto SetCenterRedLights(bool value) -> void override;
            auto IsPrimaryTakeoff() const -> bool override;
            auto SetPrimaryTakeoff(bool value) -> void override;
            auto IsPrimaryLanding() const -> bool override;
            auto SetPrimaryLanding(bool value) -> void override;
            auto IsPrimaryRightPattern() const -> bool override;
            auto SetPrimaryRightPattern(bool value) -> void override;
            auto IsSecondaryTakeoff() const -> bool override;
            auto SetSecondaryTakeoff(bool value) -> void override;
            auto IsSecondaryLanding() const -> bool override;
            auto SetSecondaryLanding(bool value) -> void override;
            auto IsSecondaryRightPattern() const -> bool override;
            auto SetSecondaryRightPattern(bool value) -> void override;

            auto GetPrimaryOffsetThreshold() -> const IBglRunwayEnd* override;
            auto SetPrimaryOffsetThreshold(IBglRunwayEnd* value) -> void override;
            auto GetSecondaryOffsetThreshold() -> const IBglRunwayEnd* override;
            auto SetSecondaryOffsetThreshold(IBglRunwayEnd* value) -> void override;
            auto GetPrimaryBlastPad() -> const IBglRunwayEnd* override;
            auto SetPrimaryBlastPad(IBglRunwayEnd* value) -> void override;
            auto GetSecondaryBlastPad() -> const IBglRunwayEnd* override;
            auto SetSecondaryBlastPad(IBglRunwayEnd* value) -> void override;
            auto GetPrimaryOverrun() -> const IBglRunwayEnd* override;
            auto SetPrimaryOverrun(IBglRunwayEnd* value) -> void override;
            auto GetSecondaryOverrun() -> const IBglRunwayEnd* override;
            auto SetSecondaryOverrun(IBglRunwayEnd* value) -> void override;
            auto GetPrimaryLeftVasi() -> const IBglRunwayVasi* override;
            auto SetPrimaryLeftVasi(IBglRunwayVasi* value) -> void override;
            auto GetPrimaryRightVasi() -> const IBglRunwayVasi* override;
            auto SetPrimaryRightVasi(IBglRunwayVasi* value) -> void override;
            auto GetSecondaryLeftVasi() -> const IBglRunwayVasi* override;
            auto SetSecondaryLeftVasi(IBglRunwayVasi* value) -> void override;
            auto GetSecondaryRightVasi() -> const IBglRunwayVasi* override;
            auto SetSecondaryRightVasi(IBglRunwayVasi* value) -> void override;
            auto GetPrimaryApproachLights() -> const IBglRunwayApproachLights* override;
            auto SetPrimaryApproachLights(IBglRunwayApproachLights* value) -> void override;
            auto GetSecondaryApproachLights() -> const IBglRunwayApproachLights* override;
            auto SetSecondaryApproachLights(IBglRunwayApproachLights* value) -> void override;

          private:
            stlab::copy_on_write<SBglRunwayData> m_data;
            stlab::copy_on_write<CBglRunwayEnd> m_primary_offset_threshold;
            stlab::copy_on_write<CBglRunwayEnd> m_secondary_offset_threshold;
            stlab::copy_on_write<CBglRunwayEnd> m_primary_blast_pad;
            stlab::copy_on_write<CBglRunwayEnd> m_secondary_blast_pad;
            stlab::copy_on_write<CBglRunwayEnd> m_primary_overrun;
            stlab::copy_on_write<CBglRunwayEnd> m_secondary_overrun;
            stlab::copy_on_write<CBglRunwayVasi> m_primary_left_vasi;
            stlab::copy_on_write<CBglRunwayVasi> m_primary_right_vasi;
            stlab::copy_on_write<CBglRunwayVasi> m_secondary_left_vasi;
            stlab::copy_on_write<CBglRunwayVasi> m_secondary_right_vasi;
            stlab::copy_on_write<CBglRunwayApproachLights> m_primary_approach_lights;
            stlab::copy_on_write<CBglRunwayApproachLights> m_secondary_approach_lights;
        };

        //******************************************************************************
        // CBglStart
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglStartData
        {
            uint16_t Type;
            uint32_t Size;
            uint8_t Number;
            uint8_t Designator;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            float Heading;
        };

#pragma pack(pop)

        class CBglStart final : public CBglLLA<stlab::copy_on_write<SBglStartData>>,
                                public IBglSerializable,
                                public IBglStart
        {
          public:
            CBglStart() : CBglLLA<stlab::copy_on_write<SBglStartData>>(m_data) { }

            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetRunwayNumber() const -> IBglRunway::ERunwayNumber override;
            auto SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void override;
            auto GetRunwayDesignator() const -> IBglRunway::ERunwayDesignator override;
            auto SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void override;
            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetHeading() const -> float override;
            auto SetHeading(float value) -> void override;

          private:
            stlab::copy_on_write<SBglStartData> m_data;
        };

        //******************************************************************************
        // CBglCom
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglComData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t ComType;
            uint32_t Frequency;
            std::string Name;
        };

#pragma pack(pop)

        class CBglCom final : public IBglSerializable, public IBglCom
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetFrequency() const -> uint32_t override;
            auto SetFrequency(uint32_t value) -> void override;
            auto GetName() const -> const char* override;
            auto SetName(const char* value) -> void override;

            auto IsEmpty() const -> bool;

          private:
            auto CalculateRemainingSize() const -> int;

            stlab::copy_on_write<SBglComData> m_data;
        };

        //******************************************************************************
        // CBglHelipad
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglHelipadData
        {
            uint16_t Type;
            uint32_t Size;
            uint8_t SurfaceType;
            uint8_t HelipadType;
            uint32_t Color;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            float Length;
            float Width;
            float Heading;
        };

#pragma pack(pop)

        class CBglHelipad final : public CBglLLA<stlab::copy_on_write<SBglHelipadData>>,
                                  public IBglSerializable,
                                  public IBglHelipad
        {
          public:
            CBglHelipad() : CBglLLA<stlab::copy_on_write<SBglHelipadData>>(m_data) { }

            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetSurfaceType() const -> ESurfaceType override;
            auto SetSurfaceType(ESurfaceType value) -> void override;
            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto IsTransparent() const -> bool override;
            auto SetTransparent(bool value) -> void override;
            auto IsClosed() const -> bool override;
            auto SetClosed(bool value) -> void override;
            auto GetColor() const -> uint32_t override;
            auto SetColor(uint32_t value) -> void override;
            auto GetLength() const -> float override;
            auto SetLength(float value) -> void override;
            auto GetWidth() const -> float override;
            auto SetWidth(float value) -> void override;
            auto GetHeading() const -> float override;
            auto SetHeading(float value) -> void override;

          private:
            stlab::copy_on_write<SBglHelipadData> m_data;
        };

        //******************************************************************************
        // CBglRunwayDelete
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglRunwayDeleteData
        {
            uint8_t SurfaceType;
            uint8_t NumberPrimary;
            uint8_t NumberSecondary;
            uint8_t Designator;
        };

#pragma pack(pop)

        class CBglRunwayDelete : public IBglSerializable, public IBglRunwayDelete
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetSurfaceType() const -> ESurfaceType override;
            auto SetSurfaceType(ESurfaceType value) -> void override;
            auto GetPrimaryRunwayNumber() const -> IBglRunway::ERunwayNumber override;
            auto SetPrimaryRunwayNumber(IBglRunway::ERunwayNumber value) -> void override;
            auto GetPrimaryRunwayDesignator() const -> IBglRunway::ERunwayDesignator override;
            auto SetPrimaryRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void override;
            auto GetSecondaryRunwayNumber() const -> IBglRunway::ERunwayNumber override;
            auto SetSecondaryRunwayNumber(IBglRunway::ERunwayNumber value) -> void override;
            auto GetSecondaryRunwayDesignator() const -> IBglRunway::ERunwayDesignator override;
            auto SetSecondaryRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void override;

          private:
            stlab::copy_on_write<SBglRunwayDeleteData> m_data;
        };

        //******************************************************************************
        // CBglStartDelete
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglStartDeleteData
        {
            uint8_t Number;
            uint8_t Designator;
            uint8_t Type;
            uint8_t Pad;
        };

#pragma pack(pop)

        class CBglStartDelete : public IBglSerializable, public IBglStartDelete
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetRunwayNumber() const -> IBglRunway::ERunwayNumber override;
            auto SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void override;
            auto GetRunwayDesignator() const -> IBglRunway::ERunwayDesignator override;
            auto SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void override;
            auto GetType() const -> IBglStart::EType override;
            auto SetType(IBglStart::EType value) -> void override;

          private:
            stlab::copy_on_write<SBglStartDeleteData> m_data;
        };

        //******************************************************************************
        // CBglComDelete
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglComDeleteData
        {
            uint32_t Frequency;
        };

#pragma pack(pop)

        class CBglComDelete : public IBglSerializable, public IBglComDelete
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> IBglCom::EType override;
            auto SetType(IBglCom::EType value) -> void override;
            auto GetFrequency() const -> uint32_t override;
            auto SetFrequency(uint32_t value) -> void override;

          private:
            stlab::copy_on_write<SBglComDeleteData> m_data;
        };

        //******************************************************************************
        // CBglAirportDelete
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglAirportDeleteData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t Flags;
            uint8_t RunwayCount;
            uint8_t StartCount;
            uint8_t ComCount;
            uint8_t Pad;
        };

#pragma pack(pop)

        class CBglAirportDelete : public IBglSerializable, public IBglAirportDelete
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto IsAllApproaches() const -> bool override;
            auto SetAllApproaches(bool value) -> void override;
            auto IsAllApronLights() const -> bool override;
            auto SetAllApronLights(bool value) -> void override;
            auto IsAllAprons() const -> bool override;
            auto SetAllAprons(bool value) -> void override;
            auto IsAllFrequencies() const -> bool override;
            auto SetAllFrequencies(bool value) -> void override;
            auto IsAllHelipads() const -> bool override;
            auto SetAllHelipads(bool value) -> void override;
            auto IsAllRunways() const -> bool override;
            auto SetAllRunways(bool value) -> void override;
            auto IsAllStarts() const -> bool override;
            auto SetAllStarts(bool value) -> void override;
            auto IsAllTaxiways() const -> bool override;
            auto SetAllTaxiways(bool value) -> void override;

            auto GetRunwayDeleteCount() const -> int override;
            auto GetStartDeleteCount() const -> int override;
            auto GetComDeleteCount() const -> int override;
            auto GetRunwayDeleteAt(int index) -> IBglRunwayDelete* override;
            auto AddRunwayDelete(const IBglRunwayDelete* runway) -> void override;
            auto RemoveRunwayDelete(const IBglRunwayDelete* runway) -> void override;
            auto GetStartDeleteAt(int index) -> IBglStartDelete* override;
            auto AddStartDelete(const IBglStartDelete* start) -> void override;
            auto RemoveStartDelete(const IBglStartDelete* start) -> void override;
            auto GetComDeleteAt(int index) -> IBglComDelete* override;
            auto AddComDelete(const IBglComDelete* com) -> void override;
            auto RemoveComDelete(const IBglComDelete* com) -> void override;

            auto IsEmpty() const -> bool;

          private:
            enum class EFlags : uint16_t
            {
                Approaches = 0,
                ApronLights = 1,
                Aprons = 2,
                Frequencies = 3,
                Helipads = 4,
                Runways = 5,
                Starts = 6,
                Taxiways = 7,
            };

            stlab::copy_on_write<std::vector<CBglRunwayDelete>> m_runway_deletes;
            stlab::copy_on_write<std::vector<CBglStartDelete>> m_start_deletes;
            stlab::copy_on_write<std::vector<CBglComDelete>> m_com_deletes;
            stlab::copy_on_write<SBglAirportDeleteData> m_data;
        };

        //******************************************************************************
        // CBglApronEdgeLights
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglApronEdgeLightsData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t Flags; // Are these draw flags with pad? Who knows
            uint16_t VertexCount;
            uint16_t EdgeCount;
            uint32_t Color;
            float Brightness;
            float MaxAltitude;
        };

#pragma pack(pop)

        class CBglApronEdgeLights final : public IBglSerializable, public IBglApronEdgeLights
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetVertexCount() const -> int override;
            auto GetEdgeCount() const -> int override;
            auto GetColor() const -> uint32_t override;
            auto SetColor(uint32_t value) -> void override;
            auto GetBrightness() const -> float override;
            auto SetBrightness(float value) -> void override;
            auto GetMaxAltitude() const -> float override;
            auto SetMaxAltitude(float value) -> void override;
            auto GetVertexAt(int index) -> SBglVertexLL* override;
            auto AddVertex(const SBglVertexLL* vertex) -> void override;
            auto RemoveVertex(const SBglVertexLL* vertex) -> void override;
            auto GetEdgeAt(int index) -> SBglEdge* override;
            auto AddEdge(const SBglEdge* edge) -> void override;
            auto RemoveEdge(const SBglEdge* edge) -> void override;

            auto IsEmpty() const -> bool;

          private:
            stlab::copy_on_write<SBglApronEdgeLightsData> m_data;
            stlab::copy_on_write<std::vector<SBglEdge>> m_edges;
            stlab::copy_on_write<std::vector<SBglVertexLL>> m_vertices;
        };

        //******************************************************************************
        // CBglApron
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglApronData
        {
            uint16_t Type;
            uint32_t Size;
            uint8_t Surface;
            uint16_t VertexCount;
        };

#pragma pack(pop)

        class CBglApron final : public IBglSerializable, public IBglApron
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetSurfaceType() const -> ESurfaceType override;
            auto SetSurfaceType(ESurfaceType value) -> void override;
            auto GetVertexCount() const -> int override;
            auto GetVertexAt(int index) -> SBglVertexLL* override;
            auto AddVertex(const SBglVertexLL* vertex) -> void override;
            auto RemoveVertex(const SBglVertexLL* vertex) -> void override;

          private:
            stlab::copy_on_write<SBglApronData> m_data;
            stlab::copy_on_write<std::vector<SBglVertexLL>> m_vertices;

            static constexpr int s_num_pad = 3;
        };

        //******************************************************************************
        // CBglApronPolygons
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglApronPolygonsData
        {
            uint16_t Type;
            uint32_t Size;
            uint8_t Surface;
            uint8_t Flags;
            uint16_t VertexCount;
            uint16_t IndexCount;
        };

#pragma pack(pop)

        class CBglApronPolygons final : public IBglSerializable, public IBglApronPolygons
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetSurfaceType() const -> ESurfaceType override;
            auto SetSurfaceType(ESurfaceType value) -> void override;
            auto IsDrawSurface() const -> bool override;
            auto SetDrawSurface(bool value) -> void override;
            auto IsDrawDetail() const -> bool override;
            auto SetDrawDetail(bool value) -> void override;
            auto GetVertexCount() const -> int override;
            auto GetIndexCount() const -> int override;
            auto GetVertexAt(int index) -> SBglVertexLL* override;
            auto AddVertex(const SBglVertexLL* vertex) -> void override;
            auto RemoveVertex(const SBglVertexLL* vertex) -> void override;
            auto GetIndexAt(int index) -> SBglIndex* override;
            auto AddIndex(const SBglIndex* index) -> void override;
            auto RemoveIndex(const SBglIndex* index) -> void override;

          private:
            stlab::copy_on_write<SBglApronPolygonsData> m_data;
            stlab::copy_on_write<std::vector<SBglVertexLL>> m_vertices;
            stlab::copy_on_write<std::vector<SBglIndex>> m_indices;
        };

        //******************************************************************************
        // CBglTaxiwayPoint
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTaxiwayPointData
        {
            uint8_t Type;
            uint8_t Orientation;
            uint16_t Pad;
            SBglVertexLL Vertex;
        };

#pragma pack(pop)

        class CBglTaxiwayPoint final : public IBglSerializable, public IBglTaxiwayPoint
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetOrientation() const -> EOrientation override;
            auto SetOrientation(EOrientation value) -> void override;
            auto GetVertex() -> SBglVertexLL* override;
            auto SetVertex(SBglVertexLL* vertex) -> void override;

          private:
            stlab::copy_on_write<SBglTaxiwayPointData> m_data;
        };

        //******************************************************************************
        // CBglTaxiwayPoints
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTaxiwayPointsData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t PointCount;
        };

#pragma pack(pop)

        class CBglTaxiwayPoints final : public IBglSerializable, public IBglTaxiwayPoints
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetPointCount() const -> int override;
            auto GetPointAt(int index) -> IBglTaxiwayPoint* override;
            auto AddPoint(const IBglTaxiwayPoint* point) -> void override;
            auto RemovePoint(const IBglTaxiwayPoint* point) -> void override;

            auto IsEmpty() const -> bool;

          private:
            stlab::copy_on_write<SBglTaxiwayPointsData> m_data;
            stlab::copy_on_write<std::vector<CBglTaxiwayPoint>> m_points;
        };

        //******************************************************************************
        // CBglTaxiwayParking
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTaxiwayParkingData
        {
            uint32_t Flags;
            float Radius;
            float Heading;
            float TeeOffset1;
            float TeeOffset2;
            float TeeOffset3;
            float TeeOffset4;
            SBglVertexLL Vertex;
        };

#pragma pack(pop)

        // TODO - Handle XML Index (and adjust)
        class CBglTaxiwayParking final : public IBglSerializable, public IBglTaxiwayParking
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetAirlineCodeCount() const -> int override;
            auto GetNumber() const -> uint16_t override;
            auto SetNumber(uint16_t value) -> void override;
            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetPushback() const -> EPushback override;
            auto SetPushback(EPushback value) -> void override;
            auto GetName() const -> EName override;
            auto SetName(EName value) -> void override;
            auto GetRadius() const -> float override;
            auto SetRadius(float value) -> void override;
            auto GetHeading() const -> float override;
            auto SetHeading(float value) -> void override;
            auto GetTeeOffset1() const -> float override;
            auto SetTeeOffset1(float value) -> void override;
            auto GetTeeOffset2() const -> float override;
            auto SetTeeOffset2(float value) -> void override;
            auto GetTeeOffset3() const -> float override;
            auto SetTeeOffset3(float value) -> void override;
            auto GetTeeOffset4() const -> float override;
            auto SetTeeOffset4(float value) -> void override;
            auto GetAirlineCodeAt(int index) const -> const char* override;
            auto AddAirlineCode(const char* code) -> void override;
            auto RemoveAirlineCode(const char* code) -> void override;

            auto SetAirlineCodeCount(int value) -> void;

          private:
            stlab::copy_on_write<SBglTaxiwayParkingData> m_data;
            stlab::copy_on_write<std::vector<std::string>> m_codes;
        };

        //******************************************************************************
        // CBglTaxiwayParkings
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTaxiwayParkingsData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t ParkingCount;
        };

#pragma pack(pop)

        class CBglTaxiwayParkings final : public IBglSerializable, public IBglTaxiwayParkings
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetParkingCount() const -> int override;
            auto GetParkingAt(int index) -> IBglTaxiwayParking* override;
            auto AddParking(const IBglTaxiwayParking* parking) -> void override;
            auto RemoveParking(const IBglTaxiwayParking* parking) -> void override;

            auto IsEmpty() const -> bool;

          private:
            stlab::copy_on_write<SBglTaxiwayParkingsData> m_data;
            stlab::copy_on_write<std::vector<CBglTaxiwayParking>> m_parkings;
        };

        //******************************************************************************
        // CBglTaxiwayPath
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTaxiwayPathData
        {
            uint16_t StartIndex;
            uint16_t EndIndex;
            uint8_t TypeFlags;
            uint8_t NameIndex;
            uint8_t MarkingFlags;
            uint8_t Surface;
            float Width;
            float WeightLimit;
            float Unknown;
        };

#pragma pack(pop)

        class CBglTaxiwayPath final : public IBglSerializable, public IBglTaxiwayPath
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetStartIndex() const -> int override;
            auto SetStartIndex(int value) -> void override;
            auto GetEndIndex() const -> int override;
            auto SetEndIndex(int value) -> void override;
            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetRunwayNumber() const -> IBglRunway::ERunwayNumber override;
            auto SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void override;
            auto GetRunwayDesignator() const -> IBglRunway::ERunwayDesignator override;
            auto SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void override;
            auto GetNameIndex() const -> int override;
            auto SetNameIndex(int value) -> void override;
            auto IsDrawSurface() const -> bool override;
            auto SetDrawSurface(bool value) -> void override;
            auto IsDrawDetail() const -> bool override;
            auto SetDrawDetail(bool value) -> void override;
            auto HasCenterLine() const -> bool override;
            auto SetCenterLine(bool value) -> void override;
            auto IsCenterLineLighted() const -> bool override;
            auto SetCenterLineLighted(bool value) -> void override;
            auto IsLeftEdgeLighted() const -> bool override;
            auto SetsLeftEdgeLighted(bool value) -> void override;
            auto IsRightEdgeLighted() const -> bool override;
            auto SetsRightEdgeLighted(bool value) -> void override;
            auto GetLeftEdge() const -> ELightType override;
            auto SetLeftEdge(ELightType value) -> void override;
            auto GetRightEdge() const -> ELightType override;
            auto SetRightEdge(ELightType value) -> void override;
            auto GetSurfaceType() const -> ESurfaceType override;
            auto SetSurfaceType(ESurfaceType value) -> void override;
            auto GetWidth() const -> float override;
            auto SetWidth(float value) -> void override;
            auto GetWeightLimit() const -> float override;
            auto SetWeightLimit(float value) -> void override;

          private:
            stlab::copy_on_write<SBglTaxiwayPathData> m_data;
        };

        //******************************************************************************
        // CBglTaxiwayPaths
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTaxiwayPathsData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t PathCount;
        };

#pragma pack(pop)

        class CBglTaxiwayPaths final : public IBglSerializable, public IBglTaxiwayPaths
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetPathCount() const -> int override;
            auto GetPathAt(int index) -> IBglTaxiwayPath* override;
            auto AddPath(const IBglTaxiwayPath* path) -> void override;
            auto RemovePath(const IBglTaxiwayPath* path) -> void override;

            auto IsEmpty() const -> bool;

          private:
            stlab::copy_on_write<SBglTaxiwayPathsData> m_data;
            stlab::copy_on_write<std::vector<CBglTaxiwayPath>> m_paths;
        };

        //******************************************************************************
        // CBglTaxiwayNames
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTaxiwayNamesData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t NameCount;
        };

#pragma pack(pop)

        class CBglTaxiwayNames final : public IBglSerializable, public IBglTaxiwayNames
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetNameCount() const -> int override;
            auto GetNameAt(int index) const -> const char* override;
            auto AddName(const char* name) -> void override;
            auto RemoveName(const char* name) -> void override;

            auto IsEmpty() const -> bool;

          private:
            stlab::copy_on_write<SBglTaxiwayNamesData> m_data;
            stlab::copy_on_write<std::vector<std::string>> m_names;
        };

        //******************************************************************************
        // CBglJetway
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglJetwayData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t Number;
            uint16_t Name;
            uint32_t SceneryObjectSize;
        };

#pragma pack(pop)

        class CBglJetway final : public IBglSerializable, public IBglJetway
        {
          public:
            // rule of five
            CBglJetway() = default;
            ~CBglJetway() override = default;
            CBglJetway(CBglJetway const& other);
            CBglJetway(CBglJetway&& other) = default;
            CBglJetway& operator=(CBglJetway const& other);
            CBglJetway& operator=(CBglJetway&& other) = default;

            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetNumber() const -> uint16_t override;
            auto SetNumber(uint16_t value) -> void override;
            auto GetName() const -> IBglTaxiwayParking::EName override;
            auto SetName(IBglTaxiwayParking::EName value) -> void override;
            auto GetSceneryObject() -> IBglSceneryObject* override;
            auto GetSceneryObject() const -> const IBglSceneryObject* override;
            auto SetSceneryObject(IBglSceneryObject* value) -> void override;

          private:
            auto CalculatePadSize() const -> int;

            stlab::copy_on_write<SBglJetwayData> m_data;
            std::unique_ptr<CBglSceneryObject> m_scenery_object;
        };

        //******************************************************************************
        // CBglLeg
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglLegData
        {
            uint8_t LegType;
            uint8_t AltitudeDescriptor;
            uint8_t TurnDirection;
            uint8_t Flags;
            uint32_t IcaoIdent;
            uint32_t RegionIdent;
            uint32_t RecommendedIcaoIdent;
            uint32_t RecommendedRegionIdent;
            float Theta;
            float Rho;
            float Course;
            float DistanceTime;
            float Altitude1;
            float Altitude2;
        };

#pragma pack(pop)

        class CBglLeg final : public IBglSerializable, public IBglLeg
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetAltitudeDescriptor() const -> EAltitudeDescriptor override;
            auto SetAltitudeDescriptor(EAltitudeDescriptor value) -> void override;
            auto GetTurnDirection() const -> ETurnDirection override;
            auto SetTurnDirection(ETurnDirection value) -> void override;
            auto IsTrueCourse() const -> bool override;
            auto SetTrueCourse(bool value) -> void override;
            auto IsTime() const -> bool override;
            auto SetIsTime(bool value) -> void override;
            auto IsFlyover() const -> bool override;
            auto SetFlyover(bool value) -> void override;
            auto GetFixType() const -> EFixType override;
            auto SetFixType(EFixType value) -> void override;
            auto GetIcaoIdent() const -> uint32_t override;
            auto SetIcaoIdent(uint32_t value) -> void override;
            auto GetRegionIdent() const -> uint32_t override;
            auto SetRegionIdent(uint32_t value) -> void override;
            auto GetIcaoAirport() const -> uint32_t override;
            auto SetIcaoAirport(uint32_t value) -> void override;
            auto GetRecommendedFixType() const -> EFixType override;
            auto SetRecommendedFixType(EFixType value) -> void override;
            auto GetRecommendedIcaoIdent() const -> uint32_t override;
            auto SetRecommendedIcaoIdent(uint32_t value) -> void override;
            auto GetRecommendedRegionIdent() const -> uint32_t override;
            auto SetRecommendedRegionIdent(uint32_t value) -> void override;
            auto GetTheta() const -> float override;
            auto SetTheta(float value) -> void override;
            auto GetRho() const -> float override;
            auto SetRho(float value) -> void override;
            auto GetTrueCourse() const -> float override;
            auto SetTrueCourse(float value) -> void override;
            auto GetMagneticCourse() const -> float override;
            auto SetMagneticCourse(float value) -> void override;
            auto GetDistance() const -> float override;
            auto SetDistance(float value) -> void override;
            auto GetTime() const -> float override;
            auto SetTime(float value) -> void override;
            auto GetAltitude1() const -> float override;
            auto SetAltitude1(float value) -> void override;
            auto GetAltitude2() const -> float override;
            auto SetAltitude2(float value) -> void override;

          private:
            stlab::copy_on_write<SBglLegData> m_data;
        };

//******************************************************************************
// CBglLegs
//******************************************************************************

// TODO - should we just have a generic Count / list struct
#pragma pack(push)
#pragma pack(1)

        struct SBglLegsData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t LegCount;
        };

#pragma pack(pop)

        class CBglLegs final : public IBglSerializable, public IBglLegs
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetLegCount() const -> int override;
            auto GetLegAt(int index) -> IBglLeg* override;
            auto AddLeg(const IBglLeg* leg) -> void override;
            auto RemoveLeg(const IBglLeg* leg) -> void override;

            auto IsEmpty() const -> bool;

          private:
            stlab::copy_on_write<SBglLegsData> m_data;
            stlab::copy_on_write<std::vector<CBglLeg>> m_legs;
        };

        //******************************************************************************
        // CBglDmeArc
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglDmeArcData
        {
            uint32_t IcaoIdent;
            uint32_t RegionIdent;
            uint32_t Radial;
            float Distance;
        };

#pragma pack(pop)

        class CBglDmeArc final : public IBglSerializable, public IBglDmeArc
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetIcaoIdent() const -> uint32_t override;
            auto SetIcaoIdent(uint32_t value) -> void override;
            auto GetRegionIdent() const -> uint32_t override;
            auto SetRegionIdent(uint32_t value) -> void override;
            auto GetIcaoAirport() const -> uint32_t override;
            auto SetIcaoAirport(uint32_t value) -> void override;
            auto GetRadial() const -> int override;
            auto SetRadial(int value) -> void override;
            auto GetDistance() const -> float override;
            auto SetDistance(float value) -> void override;

            auto IsEmpty() const -> bool;

          private:
            stlab::copy_on_write<SBglDmeArcData> m_data;
        };

        //******************************************************************************
        // CBglTransition
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTransitionData
        {
            uint16_t Type;
            uint32_t Size;
            uint8_t TransitionType;
            uint8_t LegCount;
            uint32_t IcaoIdent;
            uint32_t RegionIdent;
            float Altitude;
        };

#pragma pack(pop)

        class CBglTransition final : public IBglSerializable, public IBglTransition
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetLegCount() const -> int override;
            auto GetFixType() const -> IBglLeg::EFixType override;
            auto SetFixType(IBglLeg::EFixType value) -> void override;
            auto GetIcaoIdent() const -> uint32_t override;
            auto SetIcaoIdent(uint32_t value) -> void override;
            auto GetRegionIdent() const -> uint32_t override;
            auto SetRegionIdent(uint32_t value) -> void override;
            auto GetIcaoAirport() const -> uint32_t override;
            auto SetIcaoAirport(uint32_t value) -> void override;
            auto GetAltitude() const -> float override;
            auto SetAltitude(float value) -> void override;
            auto GetDmeArc() -> IBglDmeArc* override;
            auto SetDmeArc(IBglDmeArc* value) -> void override;
            auto GetTransitionLegs() -> IBglLegs* override;
            auto SetTransitionLegs(IBglLegs* value) -> void override;

          private:
            stlab::copy_on_write<SBglTransitionData> m_data;
            stlab::copy_on_write<CBglDmeArc> m_dme_arc;
            stlab::copy_on_write<CBglLegs> m_legs;
            static constexpr int s_transition_legs_type = 0x2Fu;
        };

        //******************************************************************************
        // CBglApproach
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglApproachData
        {
            uint16_t Type;
            uint32_t Size;
            uint8_t Suffix;
            uint8_t Number;
            uint8_t TypeDesignator;
            uint8_t TransitionCount;
            uint8_t ApproachLegCount;
            uint8_t MissedApproachLegCount;
            uint32_t IcaoIdent;
            uint32_t RegionIdent;
            float Altitude;
            float Heading;
            float MissedAltitude;
        };

#pragma pack(pop)

        class CBglApproach final : public IBglSerializable, public IBglApproach
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetSuffix() const -> char override;
            auto SetSuffix(char value) -> void override;
            auto GetRunwayNumber() const -> IBglRunway::ERunwayNumber override;
            auto SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void override;
            auto GetRunwayDesignator() const -> IBglRunway::ERunwayDesignator override;
            auto SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void override;
            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto HasGpsOverlay() const -> bool override;
            auto SetGpsOverlay(bool value) -> void override;
            auto GetFixType() const -> IBglLeg::EFixType override;
            auto SetFixType(IBglLeg::EFixType value) -> void override;
            auto GetIcaoIdent() const -> uint32_t override;
            auto SetIcaoIdent(uint32_t value) -> void override;
            auto GetRegionIdent() const -> uint32_t override;
            auto SetRegionIdent(uint32_t value) -> void override;
            auto GetIcaoAirport() const -> uint32_t override;
            auto SetIcaoAirport(uint32_t value) -> void override;
            auto GetAltitude() const -> float override;
            auto SetAltitude(float value) -> void override;
            auto GetHeading() const -> float override;
            auto SetHeading(float value) -> void override;
            auto GetMissedAltitude() const -> float override;
            auto SetMissedAltitude(float value) -> void override;
            auto GetTransitionCount() const -> int override;
            auto GetApproachLegCount() const -> int override;
            auto GetMissedApproachLegCount() const -> int override;
            auto GetApproachLegs() -> IBglLegs* override;
            auto SetApproachLegs(IBglLegs* value) -> void override;
            auto GetMissedApproachLegs() -> IBglLegs* override;
            auto SetMissedApproachLegs(IBglLegs* value) -> void override;
            auto GetTransitionAt(int index) -> IBglTransition* override;
            auto AddTransition(const IBglTransition* transition) -> void override;
            auto RemoveTransition(const IBglTransition* transition) -> void override;

          private:
            enum class EChildType : uint16_t
            {
                None = 0x0,
                Transition = 0x2C,
                ApproachLegs = 0x2D,
                MissedApproachLegs = 0x2E,
                TransitionLegs = 0x2F
            };

            stlab::copy_on_write<SBglApproachData> m_data;
            stlab::copy_on_write<CBglLegs> m_approach_legs;
            stlab::copy_on_write<CBglLegs> m_missed_approach_legs;
            stlab::copy_on_write<std::vector<CBglTransition>> m_transitions;
        };

        //******************************************************************************
        // CBglFence
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglFenceData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t VertexCount;
            _GUID InstanceId;
            _GUID Profile;
        };

#pragma pack(pop)

        class CBglFence final : public IBglSerializable, public IBglFence
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetVertexCount() const -> int override;
            auto GetInstanceId() const -> _GUID override;
            auto SetInstanceId(_GUID value) -> void override;
            auto GetProfile() const -> _GUID override;
            auto SetProfile(_GUID value) -> void override;
            auto GetVertexAt(int index) -> SBglVertexLL* override;
            auto AddVertex(const SBglVertexLL* vertex) -> void override;
            auto RemoveVertex(const SBglVertexLL* vertex) -> void override;

          private:
            stlab::copy_on_write<SBglFenceData> m_data;
            stlab::copy_on_write<std::vector<SBglVertexLL>> m_vertices;
        };

        //******************************************************************************
        // CBglAirport
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglAirportData
        {
            uint16_t Type;
            uint32_t Size;
            uint8_t RunwayCount;
            uint8_t FrequencyCount;
            uint8_t StartCount;
            uint8_t ApproachCount;
            uint8_t ApronCount;
            uint8_t HelipadCount;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            uint32_t TowerLongitude;
            uint32_t TowerLatitude;
            uint32_t TowerAltitude;
            float MagVar;
            uint32_t IcaoIdent;
            uint32_t RegionIdent;
            uint32_t FuelAvailability;
            uint16_t TrafficScalar;
            uint16_t Pad;
        };

#pragma pack(pop)

        class CBglAirport final : public CBglFuelAvailability<stlab::copy_on_write<SBglAirportData>>,
                                  public CBglLLA<stlab::copy_on_write<SBglAirportData>>,
                                  public CBglName,
                                  public IBglAirport
        {
          public:
            CBglAirport() :
                CBglFuelAvailability<stlab::copy_on_write<SBglAirportData>>(m_data),
                CBglLLA<stlab::copy_on_write<SBglAirportData>>(m_data)
            {
            }

            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetRunwayCount() const -> int override;
            auto GetFrequencyCount() const -> int override;
            auto GetStartCount() const -> int override;
            auto GetApproachCount() const -> int override;
            auto GetApronCount() const -> int override;
            auto IsDeleteAirport() const -> bool override;
            auto SetDeleteAirport(bool value) -> void override;
            auto GetHelipadCount() const -> int override;
            auto GetTowerLongitude() const -> double override;
            auto SetTowerLongitude(double value) -> void override;
            auto GetTowerLatitude() const -> double override;
            auto SetTowerLatitude(double value) -> void override;
            auto GetTowerAltitude() const -> double override;
            auto SetTowerAltitude(double value) -> void override;
            auto GetMagVar() const -> float override;
            auto SetMagVar(float value) -> void override;
            auto GetIcaoIdent() const -> uint32_t override;
            auto SetIcaoIdent(uint32_t value) -> void override;
            auto GetRegionIdent() const -> uint32_t override;
            auto SetRegionIdent(uint32_t value) -> void override;
            auto GetTrafficScalar() const -> float override;
            auto SetTrafficScalar(float value) -> void override;

            auto GetRunwayAt(int index) -> IBglRunway* override;
            auto AddRunway(const IBglRunway* runway) -> void override;
            auto RemoveRunway(const IBglRunway* runway) -> void override;
            auto GetStartAt(int index) -> IBglStart* override;
            auto AddStart(const IBglStart* start) -> void override;
            auto RemoveStart(const IBglStart* start) -> void override;
            auto GetComAt(int index) -> IBglCom* override;
            auto AddCom(const IBglCom* com) -> void override;
            auto RemoveCom(const IBglCom* com) -> void override;
            auto GetHelipadAt(int index) -> IBglHelipad* override;
            auto AddHelipad(const IBglHelipad* helipad) -> void override;
            auto RemoveHelipad(const IBglHelipad* helipad) -> void override;
            auto GetDelete() -> IBglAirportDelete* override;
            auto SetDelete(IBglAirportDelete* value) -> void override;
            auto GetApronEdgeLights() -> IBglApronEdgeLights* override;
            auto SetApronEdgeLights(IBglApronEdgeLights* value) -> void override;
            auto GetApronAt(int index) -> IBglApron* override;
            auto AddApron(const IBglApron* apron) -> void override;
            auto RemoveApron(const IBglApron* apron) -> void override;
            auto GetApronPolygonsAt(int index) -> IBglApronPolygons* override;
            auto AddApronPolygons(const IBglApronPolygons* polygons) -> void override;
            auto RemoveApronPolygons(const IBglApronPolygons* polygons) -> void override;
            auto GetTaxiwayPoints() -> IBglTaxiwayPoints* override;
            auto SetTaxiwayPoints(IBglTaxiwayPoints* value) -> void override;
            auto GetTaxiwayParkings() -> IBglTaxiwayParkings* override;
            auto SetTaxiwayParkings(IBglTaxiwayParkings* value) -> void override;
            auto GetTaxiwayPaths() -> IBglTaxiwayPaths* override;
            auto SetTaxiwayPaths(IBglTaxiwayPaths* value) -> void override;
            auto GetTaxiwayNames() -> IBglTaxiwayNames* override;
            auto SetTaxiwayNames(IBglTaxiwayNames* value) -> void override;
            auto GetJetwayCount() const -> int override;
            auto GetJetwayAt(int index) -> IBglJetway* override;
            auto AddJetway(const IBglJetway* jetway) -> void override;
            auto RemoveJetway(const IBglJetway* jetway) -> void override;
            auto GetApproachAt(int index) -> IBglApproach* override;
            auto AddApproach(const IBglApproach* approach) -> void override;
            auto RemoveApproach(const IBglApproach* approach) -> void override;
            auto GetBlastFenceCount() const -> int override;
            auto GetBlastFenceAt(int index) -> IBglFence* override;
            auto AddBlastFence(const IBglFence* fence) -> void override;
            auto RemoveBlastFence(const IBglFence* fence) -> void override;
            auto GetBoundaryFenceCount() const -> int override;
            auto GetBoundaryFenceAt(int index) -> IBglFence* override;
            auto AddBoundaryFence(const IBglFence* fence) -> void override;
            auto RemoveBoundaryFence(const IBglFence* fence) -> void override;

          private:
            stlab::copy_on_write<std::vector<CBglRunway>> m_runways;
            stlab::copy_on_write<std::vector<CBglStart>> m_starts;
            stlab::copy_on_write<std::vector<CBglCom>> m_coms;
            stlab::copy_on_write<std::vector<CBglHelipad>> m_helipads;
            stlab::copy_on_write<CBglAirportDelete> m_delete;
            stlab::copy_on_write<CBglApronEdgeLights> m_apron_edge_lights;
            stlab::copy_on_write<std::vector<CBglApron>> m_aprons;
            stlab::copy_on_write<std::vector<CBglApronPolygons>> m_apron_polygons;
            stlab::copy_on_write<CBglTaxiwayPoints> m_taxiway_points;
            stlab::copy_on_write<CBglTaxiwayParkings> m_taxiway_parkings;
            stlab::copy_on_write<CBglTaxiwayPaths> m_taxiway_paths;
            stlab::copy_on_write<CBglTaxiwayNames> m_taxiway_names;
            stlab::copy_on_write<std::vector<CBglJetway>> m_jetways;
            stlab::copy_on_write<std::vector<CBglApproach>> m_approaches;
            stlab::copy_on_write<std::vector<CBglFence>> m_blast_fences;
            stlab::copy_on_write<std::vector<CBglFence>> m_boundary_fences;
            stlab::copy_on_write<SBglAirportData> m_data;
        };

        //******************************************************************************
        // CBglAirportSummary
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglAirportSummaryData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t ApproachAvailability;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            uint32_t IcaoIdent;
            uint32_t RegionIdent;
            float MagVar;
            float LongestRunwayLength;
            float LongestRunwayHeading;
            uint32_t FuelAvailability;
        };

#pragma pack(pop)

        class CBglAirportSummary final : public CBglFuelAvailability<stlab::copy_on_write<SBglAirportSummaryData>>,
                                         public CBglLLA<stlab::copy_on_write<SBglAirportSummaryData>>,
                                         public IBglSerializable,
                                         public IBglAirportSummary
        {
          public:
            CBglAirportSummary() :
                CBglFuelAvailability<stlab::copy_on_write<SBglAirportSummaryData>>(m_data),
                CBglLLA<stlab::copy_on_write<SBglAirportSummaryData>>(m_data)
            {
            }

            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto HasCom() const -> bool override;
            auto SetCom(bool value) -> void override;
            auto HasPavedRunway() const -> bool override;
            auto SetPavedRunway(bool value) -> void override;
            auto HasOnlyWaterRunway() const -> bool override;
            auto SetOnlyWaterRunway(bool value) -> void override;
            auto HasGpsApproach() const -> bool override;
            auto SetGpsApproach(bool value) -> void override;
            auto HasVorApproach() const -> bool override;
            auto SetVorApproach(bool value) -> void override;
            auto HasNdbApproach() const -> bool override;
            auto SetNdbApproach(bool value) -> void override;
            auto HasIlsApproach() const -> bool override;
            auto SetIlsApproach(bool value) -> void override;
            auto HasLocApproach() const -> bool override;
            auto SetLocApproach(bool value) -> void override;
            auto HasSdfApproach() const -> bool override;
            auto SetSdfApproach(bool value) -> void override;
            auto HasLdaApproach() const -> bool override;
            auto SetLdaApproach(bool value) -> void override;
            auto HasVorDmeApproach() const -> bool override;
            auto SetVorDmeApproach(bool value) -> void override;
            auto HasNdbDmeApproach() const -> bool override;
            auto SetNdbDmeApproach(bool value) -> void override;
            auto HasRnavApproach() const -> bool override;
            auto SetRnavApproach(bool value) -> void override;
            auto HasLocBcApproach() const -> bool override;
            auto SetLocBcApproach(bool value) -> void override;
            auto GetIcaoIdent() const -> uint32_t override;
            auto SetIcaoIdent(uint32_t value) -> void override;
            auto GetRegionIdent() const -> uint32_t override;
            auto SetRegionIdent(uint32_t value) -> void override;
            auto GetMagVar() const -> float override;
            auto SetMagVar(float value) -> void override;
            auto GetLongestRunwayLength() const -> float override;
            auto SetLongestRunwayLength(float value) -> void override;
            auto GetLongestRunwayHeading() const -> float override;
            auto SetLongestRunwayHeading(float value) -> void override;

          private:
            enum class EFlags : uint16_t
            {
                Com = 0,
                PavedRunway = 1,
                OnlyWaterRunway = 2,
                Unknown3 = 3,
                Unknown4 = 4,
                GpsApproach = 5,
                VorApproach = 6,
                NdbApproach = 7,
                IlsApproach = 8,
                LocApproach = 9,
                SdfApproach = 10,
                LdaApproach = 11,
                VorDmeApproach = 12,
                NdbDmeApproach = 13,
                RnavApproach = 14,
                LocBcApproach = 15
            };

            stlab::copy_on_write<SBglAirportSummaryData> m_data;
        };

        //******************************************************************************
        // CBglRoute
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglConnectionData
        {
            uint32_t IcaoIdent;
            uint32_t RegionIdent;
            float AltitudeMinimum;
        };

        struct SBglRouteData
        {
            uint8_t RouteType;
            SBglConnectionData Previous;
            SBglConnectionData Next;
            std::string Name;
        };

#pragma pack(pop)

        class CBglRoute final : public IBglSerializable, public IBglRoute
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetName() const -> const char* override;
            auto SetName(const char* value) -> void override;
            auto GetPreviousType() const -> EConnectionType override;
            auto SetPreviousType(EConnectionType value) -> void override;
            auto GetPreviousIcaoIdent() const -> uint32_t override;
            auto SetPreviousIcaoIdent(uint32_t value) -> void override;
            auto GetPreviousRegionIdent() const -> uint32_t override;
            auto SetPreviousRegionIdent(uint32_t value) -> void override;
            auto GetPreviousIcaoAirport() const -> uint32_t override;
            auto SetPreviousIcaoAirport(uint32_t value) -> void override;
            auto GetPreviousAltitudeMinimum() const -> float override;
            auto SetPreviousAltitudeMinimum(float value) -> void override;
            auto GetNextType() const -> EConnectionType override;
            auto SetNextType(EConnectionType value) -> void override;
            auto GetNextIcaoIdent() const -> uint32_t override;
            auto SetNextIcaoIdent(uint32_t value) -> void override;
            auto GetNextRegionIdent() const -> uint32_t override;
            auto SetNextRegionIdent(uint32_t value) -> void override;
            auto GetNextIcaoAirport() const -> uint32_t override;
            auto SetNextIcaoAirport(uint32_t value) -> void override;
            auto GetNextAltitudeMinimum() const -> float override;
            auto SetNextAltitudeMinimum(float value) -> void override;

          private:
            stlab::copy_on_write<SBglRouteData> m_data;
        };

        //******************************************************************************
        // CBglWaypoint
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglWaypointData
        {
            uint16_t Type;
            uint32_t Size;
            uint8_t WaypointType;
            uint8_t RouteCount;
            uint32_t Longitude;
            uint32_t Latitude;
            float Magvar;
            uint32_t IcaoIdent;
            uint32_t RegionIdent;
        };

#pragma pack(pop)

        class CBglWaypoint final : public IBglSerializable, public IBglWaypoint
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetRouteCount() const -> int override;
            auto GetLongitude() const -> double override;
            auto SetLongitude(double value) -> void override;
            auto GetLatitude() const -> double override;
            auto SetLatitude(double value) -> void override;
            auto GetMagVar() const -> float override;
            auto SetMagVar(float value) -> void override;
            auto GetIcaoIdent() const -> uint32_t override;
            auto SetIcaoIdent(uint32_t value) -> void override;
            auto GetRegionIdent() const -> uint32_t override;
            auto SetRegionIdent(uint32_t value) -> void override;
            auto GetIcaoAirport() const -> uint32_t override;
            auto SetIcaoAirport(uint32_t value) -> void override;
            auto GetRouteAt(int index) -> IBglRoute* override;
            auto AddRoute(const IBglRoute* route) -> void override;
            auto RemoveRoute(const IBglRoute* route) -> void override;

          private:
            auto CalculatePadSize() const -> int;

            stlab::copy_on_write<SBglWaypointData> m_data;
            stlab::copy_on_write<std::vector<CBglRoute>> m_routes;
        };

        //******************************************************************************
        // CBglLocalizer
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglLocalizerData
        {
            uint16_t Type;
            uint32_t Size;
            uint8_t RunwayNumber;
            uint8_t RunwayDesignator;
            float Heading;
            float Width;
        };

#pragma pack(pop)

        class CBglLocalizer final : public IBglSerializable, public IBglLocalizer
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetRunwayNumber() const -> IBglRunway::ERunwayNumber override;
            auto SetRunwayNumber(IBglRunway::ERunwayNumber value) -> void override;
            auto GetRunwayDesignator() const -> IBglRunway::ERunwayDesignator override;
            auto SetRunwayDesignator(IBglRunway::ERunwayDesignator value) -> void override;
            auto GetHeading() const -> float override;
            auto SetHeading(float value) -> void override;
            auto GetWidth() const -> float override;
            auto SetWidth(float value) -> void override;

            auto IsEmpty() const -> bool;

          private:
            stlab::copy_on_write<SBglLocalizerData> m_data;
        };

        //******************************************************************************
        // CBglGlideSlope
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglGlideSlopeData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t Pad;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            float Range;
            float Pitch;
        };

#pragma pack(pop)

        class CBglGlideSlope final : public CBglLLA<stlab::copy_on_write<SBglGlideSlopeData>>,
                                     public IBglSerializable,
                                     public IBglGlideSlope
        {
          public:
            CBglGlideSlope() : CBglLLA<stlab::copy_on_write<SBglGlideSlopeData>>(m_data) { }

            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetPitch() const -> float override;
            auto SetPitch(float value) -> void override;
            auto GetRange() const -> float override;
            auto SetRange(float value) -> void override;

            auto IsEmpty() const -> bool;

          private:
            stlab::copy_on_write<SBglGlideSlopeData> m_data;
        };

        //******************************************************************************
        // CBglDme
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglDmeData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t Pad;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            float Range;
        };

#pragma pack(pop)

        class CBglDme final : public CBglLLA<stlab::copy_on_write<SBglDmeData>>, public IBglSerializable, public IBglDme
        {
          public:
            CBglDme() : CBglLLA<stlab::copy_on_write<SBglDmeData>>(m_data) { }

            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetRange() const -> float override;
            auto SetRange(float value) -> void override;

            auto IsEmpty() const -> bool;

          private:
            stlab::copy_on_write<SBglDmeData> m_data;
        };

        //******************************************************************************
        // CBglNav
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglNavData
        {
            uint16_t Type;
            uint32_t Size;
            uint8_t NavType;
            uint8_t Flags;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            uint32_t Frequency;
            float Range;
            float MagVar;
            uint32_t IcaoIdent;
            uint32_t RegionIdent;
        };

#pragma pack(pop)

        class CBglNav final : public CBglLLA<stlab::copy_on_write<SBglNavData>>, public CBglName, public IBglNav
        {
          public:
            CBglNav() : CBglLLA<stlab::copy_on_write<SBglNavData>>(m_data) { }

            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto IsDmeOnly() const -> bool override;
            auto SetDmeOnly(bool value) -> void override;
            auto HasBackCourse() const -> bool override;
            auto SetBackCourse(bool value) -> void override;
            auto HasGlideSlope() const -> bool override;
            auto SetGlideSlope(bool value) -> void override;
            auto HasDme() const -> bool override;
            auto SetDme(bool value) -> void override;
            auto HasNav() const -> bool override;
            auto SetNav(bool value) -> void override;
            auto GetFrequency() const -> uint32_t override;
            auto SetFrequency(uint32_t value) -> void override;
            auto GetRange() const -> float override;
            auto SetRange(float value) -> void override;
            auto GetMagVar() const -> float override;
            auto SetMagVar(float value) -> void override;
            auto GetIcaoIdent() const -> uint32_t override;
            auto SetIcaoIdent(uint32_t value) -> void override;
            auto GetRegionIdent() const -> uint32_t override;
            auto SetRegionIdent(uint32_t value) -> void override;
            auto GetIcaoAirport() const -> uint32_t override;
            auto SetIcaoAirport(uint32_t value) -> void override;
            auto GetLocalizer() -> IBglLocalizer* override;
            auto SetLocalizer(IBglLocalizer* value) -> void override;
            auto GetGlideSlope() -> IBglGlideSlope* override;
            auto SetGlideSlope(IBglGlideSlope* value) -> void override;
            auto GetDmeRecord() -> IBglDme* override;
            auto SetDmeRecord(IBglDme* value) -> void override;

          private:
            enum class EFlags : uint8_t
            {
                NotDmeOnly = 0,
                BackCourse = 2,
                GlideSlope = 3,
                Dme = 4,
                NavMissing = 5
            };

            stlab::copy_on_write<SBglNavData> m_data;
            stlab::copy_on_write<CBglLocalizer> m_localizer;
            stlab::copy_on_write<CBglGlideSlope> m_glide_slope;
            stlab::copy_on_write<CBglDme> m_dme;
        };

        //******************************************************************************
        // CBglTacan
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTacanData
        {
            uint16_t Type;
            uint32_t Size;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            uint32_t Channel;
            uint8_t Flags;
            float Range;
            float MagVar;
            uint32_t IcaoIdent;
            uint32_t RegionIdent;
        };

#pragma pack(pop)

        class CBglTacan final : public CBglLLA<stlab::copy_on_write<SBglTacanData>>, public CBglName, public IBglTacan
        {
          public:
            CBglTacan() : CBglLLA<stlab::copy_on_write<SBglTacanData>>(m_data) { }

            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto IsTypeY() const -> bool override;
            auto SetTypeY(bool value) -> void override;
            auto IsDmeOnly() const -> bool override;
            auto SetDmeOnly(bool value) -> void override;
            auto GetChannel() const -> uint32_t override;
            auto SetChannel(uint32_t value) -> void override;
            auto GetRange() const -> float override;
            auto SetRange(float value) -> void override;
            auto GetMagVar() const -> float override;
            auto SetMagVar(float value) -> void override;
            auto GetIcaoIdent() const -> uint32_t override;
            auto SetIcaoIdent(uint32_t value) -> void override;
            auto GetRegionIdent() const -> uint32_t override;
            auto SetRegionIdent(uint32_t value) -> void override;
            auto GetIcaoAirport() const -> uint32_t override;
            auto SetIcaoAirport(uint32_t value) -> void override;
            auto GetDmeRecord() -> IBglDme* override;
            auto SetDmeRecord(IBglDme* value) -> void override;

          private:
            stlab::copy_on_write<SBglTacanData> m_data;
            stlab::copy_on_write<CBglDme> m_dme;
        };

        //******************************************************************************
        // CBglBoundaryEdge
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglBoundaryEdgeData
        {
            uint16_t Type;
            uint32_t Longitude;
            uint32_t Latitude;
        };

#pragma pack(pop)

        class CBglBoundaryEdge final : public IBglSerializable, public IBglBoundaryEdge
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetLongitude() const -> double override;
            auto SetLongitude(double value) -> void override;
            auto GetLatitude() const -> double override;
            auto SetLatitude(double value) -> void override;
            auto GetRadius() const -> float override;
            auto SetRadius(float value) -> void override;

          private:
            stlab::copy_on_write<SBglBoundaryEdgeData> m_data;
        };

        //******************************************************************************
        // CBglBoundaryEdges
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglBoundaryEdgesData
        {
            uint16_t Type;
            uint32_t Size;
            uint16_t EdgeCount;
        };

#pragma pack(pop)

        class CBglBoundaryEdges final : public IBglSerializable, public IBglBoundaryEdges
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetEdgeCount() const -> int override;
            auto GetEdgeAt(int index) -> IBglBoundaryEdge* override;
            auto AddEdge(const IBglBoundaryEdge* edge) -> void override;
            auto RemoveEdge(const IBglBoundaryEdge* edge) -> void override;

            auto IsEmpty() const -> bool;

          private:
            stlab::copy_on_write<SBglBoundaryEdgesData> m_data;
            stlab::copy_on_write<std::vector<CBglBoundaryEdge>> m_edges;
        };

        //******************************************************************************
        // CBglBoundary
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglBoundaryData
        {
            uint16_t Type;
            uint32_t Size;
            uint8_t BoundaryType;
            uint8_t AltitudeType;
            uint32_t MinLongitude;
            uint32_t MinLatitude;
            uint32_t MinAltitude;
            uint32_t MaxLongitude;
            uint32_t MaxLatitude;
            uint32_t MaxAltitude;
        };

#pragma pack(pop)

        class CBglBoundary final : public CBglName, public IBglBoundary
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetMaxAltitudeType() const -> EAltitudeType override;
            auto SetMaxAltitudeType(EAltitudeType value) -> void override;
            auto GetMinAltitudeType() const -> EAltitudeType override;
            auto SetMinAltitudeType(EAltitudeType value) -> void override;
            auto GetMinLongitude() const -> double override;
            auto SetMinLongitude(double value) -> void override;
            auto GetMinLatitude() const -> double override;
            auto SetMinLatitude(double value) -> void override;
            auto GetMinAltitude() const -> double override;
            auto SetMinAltitude(double value) -> void override;
            auto GetMaxLongitude() const -> double override;
            auto SetMaxLongitude(double value) -> void override;
            auto GetMaxLatitude() const -> double override;
            auto SetMaxLatitude(double value) -> void override;
            auto GetMaxAltitude() const -> double override;
            auto SetMaxAltitude(double value) -> void override;
            auto GetCom() -> IBglCom* override;
            auto SetCom(IBglCom* value) -> void override;
            auto GetEdges() -> IBglBoundaryEdges* override;
            auto SetEdges(IBglBoundaryEdges* value) -> void override;

          private:
            auto CalculatePadSize() const -> int;

            stlab::copy_on_write<SBglBoundaryData> m_data;
            stlab::copy_on_write<CBglBoundaryEdges> m_edges;
            stlab::copy_on_write<CBglCom> m_com;
        };

        //******************************************************************************
        // CBglExclusion
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        // Exclusions are always 0,0 QMID!
        struct SBglExclusionData
        {
            uint16_t Type;
            uint16_t Size; // TODO - verify this is actually 16
            uint32_t MinLongitude;
            uint32_t MinLatitude;
            uint32_t MaxLongitude;
            uint32_t MaxLatitude;
        };

#pragma pack(pop)

        class CBglExclusion final : public IBglSerializable, public IBglExclusion
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetMinLongitude() const -> double override;
            auto SetMinLongitude(double value) -> void override;
            auto GetMaxLongitude() const -> double override;
            auto SetMaxLongitude(double value) -> void override;
            auto GetMinLatitude() const -> double override;
            auto SetMinLatitude(double value) -> void override;
            auto GetMaxLatitude() const -> double override;
            auto SetMaxLatitude(double value) -> void override;
            auto IsExcludeAll() const -> bool override;
            auto SetAll(bool value) -> void override;
            auto IsBeacon() const -> bool override;
            auto SetBeacon(bool value) -> void override;
            auto IsGenericBuilding() const -> bool override;
            auto SetGenericBuilding(bool value) -> void override;
            auto IsEffect() const -> bool override;
            auto SetEffect(bool value) -> void override;
            auto IsLibraryObject() const -> bool override;
            auto SetLibraryObject(bool value) -> void override;
            auto IsTaxiwaySigns() const -> bool override;
            auto SetTaxiwaySigns(bool value) -> void override;
            auto IsTrigger() const -> bool override;
            auto SetTrigger(bool value) -> void override;
            auto IsWindsock() const -> bool override;
            auto SetWindsock(bool value) -> void override;
            auto IsExtrusionBridge() const -> bool override;
            auto SetExtrusionBridge(bool value) -> void override;

          private:
            enum class EType : uint16_t
            {
                None = 0,
                All = 3,
                Beacon = 4,
                Effect = 5,
                GenericBuilding = 6,
                LibraryObject = 7,
                TaxiwaySigns = 8,
                Trigger = 9,
                Windsock = 10,
                ExtrusionBridge = 11,
            };

            stlab::copy_on_write<SBglExclusionData> m_data;
        };

        //******************************************************************************
        // CBglMarker
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglMarkerData
        {
            uint16_t SectionType;
            uint16_t Size;
            uint8_t UnusedType;
            uint16_t Heading;
            uint8_t MarkerType;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            uint32_t Icao;
            uint32_t Region;
        };

#pragma pack(pop)

        class CBglMarker final : public IBglSerializable, public IBglMarker
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetHeading() const -> float override;
            auto SetHeading(float value) -> void override;
            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetLongitude() const -> double override;
            auto SetLongitude(double value) -> void override;
            auto GetLatitude() const -> double override;
            auto SetLatitude(double value) -> void override;
            auto GetAltitude() const -> double override;
            auto SetAltitude(double value) -> void override;
            auto GetIcaoIdent() const -> uint32_t override;
            auto SetIcaoIdent(uint32_t value) -> void override;
            auto GetRegionIdent() const -> uint32_t override;
            auto SetRegionIdent(uint32_t value) -> void override;

          private:
            stlab::copy_on_write<SBglMarkerData> m_data;
        };

        //******************************************************************************
        // CBglGeopol
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglGeopolData
        {
            uint16_t SectionType;
            uint32_t Size;
            uint16_t GeopolType;
            uint32_t MinLongitude;
            uint32_t MinLatitude;
            uint32_t MaxLongitude;
            uint32_t MaxLatitude;
        };

#pragma pack(pop)

        class CBglGeopol final : public IBglSerializable, public IBglGeopol
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetMinLongitude() const -> double override;
            auto SetMinLongitude(double value) -> void override;
            auto GetMaxLongitude() const -> double override;
            auto SetMaxLongitude(double value) -> void override;
            auto GetMinLatitude() const -> double override;
            auto SetMinLatitude(double value) -> void override;
            auto GetMaxLatitude() const -> double override;
            auto SetMaxLatitude(double value) -> void override;
            auto GetVertexCount() const -> int override;
            auto GetVertexAt(int index) -> SBglVertexLL* override;
            auto AddVertex(const SBglVertexLL* vertex) -> void override;
            auto RemoveVertex(const SBglVertexLL* vertex) -> void override;

          private:
            auto SetVertexCount(int value) -> void;

            stlab::copy_on_write<SBglGeopolData> m_data;
            stlab::copy_on_write<std::vector<SBglVertexLL>> m_vertices;
        };

        //******************************************************************************
        // CBglSceneryObject
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglSceneryObjectData
        {
            uint16_t SectionType;
            uint16_t Size;
            uint32_t Longitude;
            uint32_t Latitude;
            uint32_t Altitude;
            uint16_t Flags;
            uint16_t Pitch;
            uint16_t Bank;
            uint16_t Heading;
            uint32_t ImageComplexity;
            _GUID InstanceId; // TODO: Assumes FSX, need to add FS9 types
        };

#pragma pack(pop)

        class CBglSceneryObject : public IBglSerializable, virtual public IBglSceneryObject
        {
          public:
            void ReadBinary(BinaryFileStream& in) override;
            void WriteBinary(BinaryFileStream& out) override;
            bool Validate() override;
            int CalculateSize() const override;

            ESceneryObjectType GetSceneryObjectType() const override;
            void SetSceneryObjectType(ESceneryObjectType value) override;
            double GetLongitude() const override;
            void SetLongitude(double value) override;
            double GetLatitude() const override;
            void SetLatitude(double value) override;
            double GetAltitude() const override;
            void SetAltitude(double value) override;
            bool IsAboveAgl() const override;
            void SetAboveAgl(bool value) override;
            bool IsNoAutogenSuppression() const override;
            void SetNoAutogenSuppression(bool value) override;
            bool IsNoCrash() const override;
            void SetNoCrash(bool value) override;
            bool IsNoFog() const override;
            void SetNoFog(bool value) override;
            bool IsNoShadow() const override;
            void SetNoShadow(bool value) override;
            bool IsNoZWrite() const override;
            void SetNoZWrite(bool value) override;
            bool IsNoZTest() const override;
            void SetNoZTest(bool value) override;
            float GetPitch() const override;
            void SetPitch(float value) override;
            float GetBank() const override;
            void SetBank(float value) override;
            float GetHeading() const override;
            void SetHeading(float value) override;
            EImageComplexity GetImageComplexity() const override;
            void SetImageComplexity(EImageComplexity value) override;
            _GUID GetInstanceId() const override;
            void SetInstanceId(_GUID value) override;
            IBglGenericBuilding* GetGenericBuilding() override;
            IBglLibraryObject* GetLibraryObject() override;
            IBglWindsock* GetWindsock() override;
            IBglEffect* GetEffect() override;
            IBglTaxiwaySigns* GetTaxiwaySigns() override;
            IBglTrigger* GetTrigger() override;
            IBglBeacon* GetBeacon() override;
            IBglExtrusionBridge* GetExtrusionBridge() override;

            auto IsEmpty() const -> bool;
            auto Clone() const { return std::unique_ptr<CBglSceneryObject>(CloneImpl()); }

          protected:
            int RecordSize() const;
            virtual CBglSceneryObject* CloneImpl() const = 0;

          private:
            enum class Flags : uint16_t
            {
                None = 0,
                AboveAgl = 1 << 0,
                NoAutogenSuppression = 1 << 1,
                NoCrash = 1 << 2,
                NoFog = 1 << 3,
                NoShadow = 1 << 4,
                NoZWrite = 1 << 5,
                NoZTest = 1 << 6
            };

            stlab::copy_on_write<SBglSceneryObjectData> m_data;
        };

        //******************************************************************************
        // CBglGenericBuilding
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglGenericBuildingData
        {
            float Scale;
            uint16_t SubrecordStart;
            uint16_t SubrecordSize;
            uint16_t SubrecordType;
            uint16_t BuildingSides;
            uint16_t SizeX;
            uint16_t SizeZ;
            uint16_t SizeTopX;
            uint16_t SizeTopZ;
            uint16_t BottomTexture;
            uint16_t SizeBottomY;
            uint16_t TextureIndexBottomX;
            uint16_t TextureIndexBottomZ;
            uint16_t WindowTexture;
            uint16_t SizeWindowY;
            uint16_t TextureIndexWindowX;
            uint16_t TextureIndexWindowY;
            uint16_t TextureIndexWindowZ;
            uint16_t TopTexture;
            uint16_t SizeTopY;
            uint16_t TextureIndexTopX;
            uint16_t TextureIndexTopZ;
            uint16_t RoofTexture;
            uint16_t TextureIndexRoofX;
            uint16_t TextureIndexRoofZ;
            uint16_t SizeRoofY;
            uint16_t TextureIndexGableY;
            uint16_t GableTexture;
            uint16_t TextureIndexGableZ;
            uint16_t FaceTexture;
            uint16_t TextureIndexFaceX;
            uint16_t TextureIndexFaceY;
            uint16_t TextureIndexRoofY;
            uint16_t SubrecordEnd;
            // Smoothing not implemented
        };

#pragma pack(pop)

        class CBglGenericBuilding final : public CBglSceneryObject, public IBglGenericBuilding
        {
          public:
            void ReadBinary(BinaryFileStream& in) override;
            void WriteBinary(BinaryFileStream& out) override;
            bool Validate() override;
            int CalculateSize() const override;

            float GetScale() const override;
            void SetScale(float value) override;
            EType GetType() const override;
            void SetType(EType value) override;
            uint16_t GetBuildingSides() const override;
            void SetBuildingSides(uint16_t value) override;
            uint16_t GetSizeX() const override;
            void SetSizeX(uint16_t value) override;
            uint16_t GetSizeZ() const override;
            void SetSizeZ(uint16_t value) override;
            uint16_t GetSizeTopX() const override;
            void SetSizeTopX(uint16_t value) override;
            uint16_t GetSizeTopZ() const override;
            void SetSizeTopZ(uint16_t value) override;
            uint16_t GetBottomTexture() const override;
            void SetBottomTexture(uint16_t value) override;
            uint16_t GetSizeBottomY() const override;
            void SetSizeBottomY(uint16_t value) override;
            uint16_t GetTextureIndexBottomX() const override;
            void SetTextureIndexBottomX(uint16_t value) override;
            uint16_t GetTextureIndexBottomZ() const override;
            void SetTextureIndexBottomZ(uint16_t value) override;
            uint16_t GetWindowTexture() const override;
            void SetWindowTexture(uint16_t value) override;
            uint16_t GetSizeWindowY() const override;
            void SetSizeWindowY(uint16_t value) override;
            uint16_t GetTextureIndexWindowX() const override;
            void SetTextureIndexWindowX(uint16_t value) override;
            uint16_t GetTextureIndexWindowY() const override;
            void SetTextureIndexWindowY(uint16_t value) override;
            uint16_t GetTextureIndexWindowZ() const override;
            void SetTextureIndexWindowZ(uint16_t value) override;
            uint16_t GetTopTexture() const override;
            void SetTopTexture(uint16_t value) override;
            uint16_t GetSizeTopY() const override;
            void SetSizeTopY(uint16_t value) override;
            uint16_t GetTextureIndexTopX() const override;
            void SetTextureIndexTopX(uint16_t value) override;
            uint16_t GetTextureIndexTopZ() const override;
            void SetTextureIndexTopZ(uint16_t value) override;
            uint16_t GetRoofTexture() const override;
            void SetRoofTexture(uint16_t value) override;
            uint16_t GetTextureIndexRoofX() const override;
            void SetTextureIndexRoofX(uint16_t value) override;
            uint16_t GetTextureIndexRoofZ() const override;
            void SetTextureIndexRoofZ(uint16_t value) override;
            uint16_t GetSizeRoofY() const override;
            void SetSizeRoofY(uint16_t value) override;
            uint16_t GetTextureIndexGableY() const override;
            void SetTextureIndexGableY(uint16_t value) override;
            uint16_t GetGableTexture() const override;
            void SetGableTexture(uint16_t value) override;
            uint16_t GetTextureIndexGableZ() const override;
            void SetTextureIndexGableZ(uint16_t value) override;
            uint16_t GetFaceTexture() const override;
            void SetFaceTexture(uint16_t value) override;
            uint16_t GetTextureIndexFaceX() const override;
            void SetTextureIndexFaceX(uint16_t value) override;
            uint16_t GetTextureIndexFaceY() const override;
            void SetTextureIndexFaceY(uint16_t value) override;
            uint16_t GetTextureIndexRoofY() const override;
            void SetTextureIndexRoofY(uint16_t value) override;

          private:
            int CalculateSubrecordSize() const;

          protected:
            auto CloneImpl() const -> CBglSceneryObject* override { return new CBglGenericBuilding(*this); }

          private:
            stlab::copy_on_write<SBglGenericBuildingData> m_data;
        };

        //******************************************************************************
        // CBglLibraryObject
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglLibraryObjectData
        {
            _GUID Name;
            float Scale;
        };

#pragma pack(pop)

        class CBglLibraryObject final : public CBglSceneryObject, public IBglLibraryObject
        {
          public:
            void ReadBinary(BinaryFileStream& in) override;
            void WriteBinary(BinaryFileStream& out) override;
            bool Validate() override;
            int CalculateSize() const override;

            _GUID GetName() const override;
            void SetName(_GUID value) override;
            float GetScale() const override;
            void SetScale(float value) override;

          protected:
            auto CloneImpl() const -> CBglSceneryObject* override { return new CBglLibraryObject(*this); }

          private:
            stlab::copy_on_write<SBglLibraryObjectData> m_data;
        };

        //******************************************************************************
        // CBglWindsock
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglWindsockData
        {
            float PoleHeight;
            float WindsockLength;
            uint32_t PoleColor;
            uint32_t SockColor;
            uint16_t Lighted;
        };

#pragma pack(pop)

        class CBglWindsock final : public CBglSceneryObject, public IBglWindsock
        {
          public:
            void ReadBinary(BinaryFileStream& in) override;
            void WriteBinary(BinaryFileStream& out) override;
            bool Validate() override;
            int CalculateSize() const override;

            float GetPoleHeight() const override;
            void SetPoleHeight(float value) override;
            float GetSockLength() const override;
            void SetSockLength(float value) override;
            uint32_t GetPoleColor() const override;
            void SetPoleColor(uint32_t value) override;
            uint32_t GetSockColor() const override;
            void SetSockColor(uint32_t value) override;
            bool IsLighted() const override;
            void SetLighted(bool value) override;

          protected:
            auto CloneImpl() const -> CBglSceneryObject* override { return new CBglWindsock(*this); }

          private:
            stlab::copy_on_write<SBglWindsockData> m_data;
        };

        //******************************************************************************
        // CBglEffect
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglEffectData
        {
            std::string Name;
            std::string Params;
        };

#pragma pack(pop)

        class CBglEffect final : public CBglSceneryObject, public IBglEffect
        {
          public:
            void ReadBinary(BinaryFileStream& in) override;
            void WriteBinary(BinaryFileStream& out) override;
            bool Validate() override;
            int CalculateSize() const override;

            const char* GetName() const override;
            void SetName(const char* value) override;
            const char* GetParams() const override;
            void SetParams(const char* value) override;

          protected:
            auto CloneImpl() const -> CBglSceneryObject* override { return new CBglEffect(*this); }

          private:
            stlab::copy_on_write<SBglEffectData> m_data;
            static constexpr int s_name_size = 80;
        };

        //******************************************************************************
        // CBglTaxiwaySign
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTaxiwaySignData
        {
            float LongitudeBias;
            float LatitudeBias;
            uint16_t Heading;
            uint8_t Size;
            uint8_t Justification;
            std::string Label;
        };

#pragma pack(pop)

        class CBglTaxiwaySign final : public IBglSerializable, public IBglTaxiwaySign
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetLongitudeBias() const -> float override;
            auto SetLongitudeBias(float value) -> void override;
            auto GetLatitudeBias() const -> float override;
            auto SetLatitudeBias(float value) -> void override;
            auto GetHeading() const -> float override;
            auto SetHeading(float value) -> void override;
            auto GetSize() const -> ESize override;
            auto SetSize(ESize value) -> void override;
            auto GetJustification() const -> EJustification override;
            auto SetJustification(EJustification value) -> void override;
            auto GetLabel() const -> const char* override;
            auto SetLabel(const char* value) -> void override;

          private:
            stlab::copy_on_write<SBglTaxiwaySignData> m_data;
            static constexpr int s_record_size = 12;
        };

        //******************************************************************************
        // CBglTaxiwaySigns
        //******************************************************************************

        class CBglTaxiwaySigns final : public CBglSceneryObject, public IBglTaxiwaySigns
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetSignCount() const -> int override;
            auto GetSignAt(int index) -> IBglTaxiwaySign* override;
            auto AddSign(const IBglTaxiwaySign* sign) -> void override;
            auto RemoveSign(const IBglTaxiwaySign* sign) -> void override;

          protected:
            auto CloneImpl() const -> CBglSceneryObject* override { return new CBglTaxiwaySigns(*this); }

          private:
            stlab::copy_on_write<std::vector<CBglTaxiwaySign>> m_signs;
        };

        //******************************************************************************
        // CBglTriggerRefuelRepair
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTriggerRefuelRepairData
        {
            uint32_t FuelAvailability;
            uint32_t PointCount;
        };

#pragma pack(pop)

        class CBglTriggerRefuelRepair final
            : public CBglFuelAvailability<stlab::copy_on_write<SBglTriggerRefuelRepairData>>,
              public IBglSerializable,
              public IBglTriggerRefuelRepair
        {
          public:
            CBglTriggerRefuelRepair() : CBglFuelAvailability<stlab::copy_on_write<SBglTriggerRefuelRepairData>>(m_data)
            {
            }

            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetVertexCount() const -> int override;
            auto GetVertexAt(int index) -> SBglVertexBias* override;
            auto AddVertex(const SBglVertexBias* point) -> void override;
            auto RemoveVertex(const SBglVertexBias* point) -> void override;

          private:
            stlab::copy_on_write<SBglTriggerRefuelRepairData> m_data;
            stlab::copy_on_write<std::vector<SBglVertexBias>> m_vertices;
        };

        //******************************************************************************
        // CBglTriggerWeather
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTriggerWeatherData
        {
            uint16_t Type;
            float Heading;
            float Scalar;
            uint32_t PointCount;
        };

#pragma pack(pop)

        class CBglTriggerWeather final : public IBglSerializable, public IBglTriggerWeather
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetTriggerHeading() const -> float override;
            auto SetTriggerHeading(float value) -> void override;
            auto GetScalar() const -> float override;
            auto SetScalar(float value) -> void override;
            auto GetVertexCount() const -> int override;
            auto GetVertexAt(int index) -> SBglVertexBias* override;
            auto AddVertex(const SBglVertexBias* point) -> void override;
            auto RemoveVertex(const SBglVertexBias* point) -> void override;

          private:
            stlab::copy_on_write<SBglTriggerWeatherData> m_data;
            stlab::copy_on_write<std::vector<SBglVertexBias>> m_vertices;
        };

        //******************************************************************************
        // CBglTrigger
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglTriggerData
        {
            uint16_t Type;
            float Height;
        };

#pragma pack(pop)

        class CBglTrigger final : public CBglSceneryObject, public IBglTrigger
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetType() const -> EType override;
            auto SetType(EType value) -> void override;
            auto GetHeight() const -> float override;
            auto SetHeight(float value) -> void override;
            auto GetRepairRefuel() const -> const IBglTriggerRefuelRepair* override;
            auto SetRepairRefuel(const IBglTriggerRefuelRepair* value) -> void override;
            auto GetWeather() const -> const IBglTriggerWeather* override;
            auto SetWeather(const IBglTriggerWeather* value) -> void override;

          protected:
            auto CloneImpl() const -> CBglSceneryObject* override { return new CBglTrigger(*this); }

          private:
            stlab::copy_on_write<SBglTriggerData> m_data;
            stlab::copy_on_write<CBglTriggerRefuelRepair> m_refuel;
            stlab::copy_on_write<CBglTriggerWeather> m_weather;
        };

        //******************************************************************************
        // CBglBeacon
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglBeaconData
        {
            uint8_t BaseType;
            uint8_t Type;
        };

#pragma pack(pop)

        class CBglBeacon final : public CBglSceneryObject, public IBglBeacon
        {
          public:
            void ReadBinary(BinaryFileStream& in) override;
            void WriteBinary(BinaryFileStream& out) override;
            bool Validate() override;
            int CalculateSize() const override;

            EBaseType GetBaseType() const override;
            void SetBaseType(EBaseType value) override;
            EType GetType() const override;
            void SetType(EType value) override;

          protected:
            auto CloneImpl() const -> CBglSceneryObject* override { return new CBglBeacon(*this); }

          private:
            stlab::copy_on_write<SBglBeaconData> m_data;
        };

        //******************************************************************************
        // CBglExtrusionBridge
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglExtrusionBridgeData
        {
            _GUID ExtrusionProfile;
            _GUID MaterialSet;
            uint32_t LongitudeSample1;
            uint32_t LatitudeSample1;
            uint32_t AltitudeSample1;
            uint32_t LongitudeSample2;
            uint32_t LatitudeSample2;
            uint32_t AltitudeSample2;
            float RoadWidth;
            float Probability;
            uint8_t SuppressPlatform;
            uint8_t PlacementCount;
            uint16_t PointCount;
        };

#pragma pack(pop)

        class CBglExtrusionBridge final : public CBglSceneryObject, public IBglExtrusionBridge
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetExtrusionProfile() const -> _GUID override;
            auto SetExtrusionProfile(_GUID value) -> void override;
            auto GetMaterialSet() const -> _GUID override;
            void SetMaterialSet(_GUID value) override;
            auto GetLongitudeSample1() const -> double override;
            void SetLongitudeSample1(double value) override;
            auto GetLatitudeSample1() const -> double override;
            auto SetLatitudeSample1(double value) -> void override;
            auto GetAltitudeSample1() const -> double override;
            auto SetAltitudeSample1(double value) -> void override;
            auto GetLongitudeSample2() const -> double override;
            auto SetLongitudeSample2(double value) -> void override;
            auto GetLatitudeSample2() const -> double override;
            auto SetLatitudeSample2(double value) -> void override;
            auto GetAltitudeSample2() const -> double override;
            auto SetAltitudeSample2(double value) -> void override;
            auto GetRoadWidth() const -> float override;
            auto SetRoadWidth(float value) -> void override;
            auto GetProbability() const -> float override;
            auto SetProbability(float value) -> void override;
            auto IsSuppressPlatform() const -> bool override;
            auto SetSuppressPlatform(bool value) -> void override;
            auto GetPlacementCount() const -> int override;
            auto GetPointCount() const -> int override;
            auto GetPlacementAt(int index) -> _GUID* override;
            auto AddPlacement(const _GUID* placement) -> void override;
            auto RemovePlacement(const _GUID* placement) -> void override;
            auto GetPointAt(int index) -> SBglVertexLLA* override;
            auto AddPoint(const SBglVertexLLA* point) -> void override;
            auto RemovePoint(const SBglVertexLLA* point) -> void override;

          protected:
            auto CloneImpl() const -> CBglSceneryObject* override { return new CBglExtrusionBridge(*this); }

          private:
            stlab::copy_on_write<SBglExtrusionBridgeData> m_data;
            stlab::copy_on_write<std::vector<_GUID>> m_placements;
            stlab::copy_on_write<std::vector<SBglVertexLLA>> m_points;
        };

        //******************************************************************************
        // CBglModelData
        //******************************************************************************

#pragma pack(push)
#pragma pack(1)

        struct SBglModelData
        {
            _GUID Name;
            uint32_t Offset;
            uint32_t Length;
        };

#pragma pack(pop)

        class CBglModelData final : public IBglSerializable, public IBglModelData
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetData() const -> const uint8_t* override;
            auto SetData(const uint8_t* value, int length) -> void override;
            auto GetLength() const -> int override;

          private:
            stlab::copy_on_write<std::vector<uint8_t>> m_model;
            // std::shared_ptr<uint8_t[]> GetCompressedData();
        };

        //******************************************************************************
        // CTerrainRasterQuad1
        //******************************************************************************

        enum class ERasterCompressionType : uint8_t
        {
            None = 0,
            Lz1 = 1,
            Delta = 2,
            DeltaLz1 = 3,
            Lz2 = 4,
            DeltaLz2 = 5,
            BitPack = 6,
            BitPackLz1 = 7,
            SolidBlock = 8,
            BitPackLz2 = 9,
            Ptc = 10,
            Dxt1 = 11,
            Dxt3 = 12,
            Dxt5 = 13,
            Max = 14
        };

        enum class ERasterDataType : uint16_t
        {
            None = 0,
            Photo = 1,
            Elevation = 2,
            LandClass = 3,
            WaterClass = 4,
            Region = 5,
            Season = 6,
            PopulationDensity = 7,
            Reserved = 8,
            TerrainIndex = 9,
            ModifiedElevation = 10,
            PhotoFlight = 11,
            Max = 12
        };

#pragma pack(push)
#pragma pack(1)

        // Exclusions are always 0,0 QMID!
        struct SBglTerrainRasterQuad1Data
        {
            uint32_t Version;
            uint32_t Size;
            ERasterDataType DataType;
            ERasterCompressionType CompressionTypeData;
            ERasterCompressionType CompressionTypeMask;
            uint32_t QmidLow;
            uint32_t QmidHigh;
            uint32_t Variations;
            uint32_t Rows;
            uint32_t Cols;
            uint32_t SizeData;
            uint32_t SizeMask;
        };

#pragma pack(pop)

        // Out-Of-Core compressed raster block
        class CRasterBlock
        {
          public:
            int DataOffset;
            int DataLength;
            int MaskOffset;
            int MaskLength;

            std::shared_ptr<uint8_t[]> GetCompressedData();
        };

        struct SRasterImage
        {
            uint32_t Width = 0;
            uint32_t Height = 0;
            ERasterDataType DataType = ERasterDataType::None;
            int BitDepth = 0;
            int Channels = 0;
            std::unique_ptr<uint8_t[]> Data;
            size_t DataSize = 0;
        };

        class CTerrainRasterQuad1 : public IBglSerializable, ITerrainRasterQuad1
        {
          public:
            void ReadBinary(BinaryFileStream& in) override;
            void WriteBinary(BinaryFileStream& out) override;
            bool Validate() override;
            int CalculateSize() const override;

            int Rows() const override;
            int Cols() const override;
            ERasterDataType GetDataType() const { return m_header->DataType; }
            static bool GetImageFormatForType(ERasterDataType data_type, int& bit_depth, int& num_channels);
            std::unique_ptr<uint8_t[]> DecompressData(ERasterCompressionType compression_type,
                const uint8_t* compressed_data, int compressed_size, int uncompressed_size) const;
            bool DecodeToImage(const uint8_t* compressed_data, int compressed_size, SRasterImage& out_image) const;

            const SBglTerrainRasterQuad1Data& GetHeader() const { return m_header.read(); }

            int GetDataOffset() const { return m_data->DataOffset; }

            int GetDataLength() const { return m_data->DataLength; }

            struct SRcs1Data
            {
                uint32_t Signature = 0;
                float Scale = 1.0f;
                float Base = 0.0f;
            };

            bool ReadCompressedData(
                BinaryFileStream& in, std::vector<uint8_t>& out_data, std::optional<SRcs1Data>& out_rcs1) const;

          private:
            int GetBpp() const
            {
                switch (GetDataType())
                {
                case ERasterDataType::PopulationDensity:
                case ERasterDataType::TerrainIndex:
                case ERasterDataType::LandClass:
                case ERasterDataType::Region:
                case ERasterDataType::Season:
                case ERasterDataType::WaterClass:
                    return 1;

                case ERasterDataType::Elevation:
                case ERasterDataType::ModifiedElevation:
                case ERasterDataType::Photo:
                    return 2;
                case ERasterDataType::PhotoFlight:
                    return 4;
                default:
                    return 0;
                }
            }

            bool GetImageFormat(int& bit_depth, int& num_channels) const;

            int CalculateLength(bool mask) const
            {
                if (mask)
                {
                    return static_cast<int>(sizeof(uint8_t)) * Cols() * Rows();
                }

                return GetBpp() * Cols() * Rows();
            }

            stlab::copy_on_write<SBglTerrainRasterQuad1Data> m_header;
            stlab::copy_on_write<CRasterBlock> m_data;
        };

        class CBglTerrainElevation final : public CTerrainRasterQuad1, public IBglTerrainElevation
        {
        };

        class CBglTerrainIndex final : public CTerrainRasterQuad1, public IBglTerrainIndex
        {
        };

        auto ConvertRasterToRgba(const SRasterImage& image, std::vector<uint8_t>& out_rgba) -> bool;

        //******************************************************************************
        // CBglTimeZone
        //******************************************************************************

        // TODO - FSX Only. Later P3D (and MSFS) changed the format, need to handle
        // In MSFS, they are 0x26 bytes, in P3D, variable with nested vertex
        // Note, daylightSavings enum us not used, see serialization info in:
        // https://www.fsdeveloper.com/forum/threads/bgl2xml-feature-request.188576
        // We need to conditionally serialize

#pragma pack(push)
#pragma pack(1)

        struct SBglTimezoneData
        {
            uint32_t MinLongitude;
            uint32_t MaxLatitude;
            uint32_t MaxLongitude;
            uint32_t MinLatitude;
            int16_t TimeDeviation;
            uint8_t Priority;
            int8_t DstTimeShift;
            uint32_t DstPacked;
        };

#pragma pack(pop)

        class CBglTimeZone final : public IBglSerializable, public IBglTimeZone
        {
          public:
            auto ReadBinary(BinaryFileStream& in) -> void override;
            auto WriteBinary(BinaryFileStream& out) -> void override;
            auto Validate() -> bool override;
            auto CalculateSize() const -> int override;

            auto GetMinLongitude() const -> double override;
            auto SetMinLongitude(double value) -> void override;
            auto GetMaxLatitude() const -> double override;
            auto SetMaxLatitude(double value) -> void override;
            auto GetMaxLongitude() const -> double override;
            auto SetMaxLongitude(double value) -> void override;
            auto GetMinLatitude() const -> double override;
            auto SetMinLatitude(double value) -> void override;
            auto GetTimeDeviation() const -> int16_t override;
            auto SetTimeDeviation(int16_t value) -> void override;
            auto GetPriority() const -> uint8_t override;
            auto SetPriority(uint8_t value) -> void override;
            auto GetDaylightSavingsTimeShift() const -> int8_t override;
            auto SetDaylightSavingsTimeShift(int8_t value) -> void override;
            auto GetDaylightSavingsStartDayOfYear() const -> uint16_t override;
            auto SetDaylightSavingsStartDayOfYear(uint16_t value) -> void override;
            auto GetDaylightSavingsStartDayOfWeek() const -> EStartDayOfWeek override;
            auto SetDaylightSavingsStartDayOfWeek(EStartDayOfWeek value) -> void override;
            auto GetDaylightSavingsEndDayOfYear() const -> uint16_t override;
            auto SetDaylightSavingsEndDayOfYear(uint16_t value) -> void override;
            auto GetDaylightSavingsEndDayOfWeek() const -> EStartDayOfWeek override;
            auto SetDaylightSavingsEndDayOfWeek(EStartDayOfWeek value) -> void override;

          private:
            stlab::copy_on_write<SBglTimezoneData> m_data;
        };

    } // namespace io

} // namespace flightsimlib

#endif
