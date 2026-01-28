#pragma once

#include "IObject.h"

#if defined(_WIN32)
#if defined(_WIN64)
#define FSLAPI __cdecl
#define MEMBERCALL
#else
#define FSLAPI __stdcall
#define MEMBERCALL __thiscall
#endif
#else
#define FSLAPI
#define MEMBERCALL
#endif

#ifdef FLIGHTSIMLIB_DLL
#define FSLCALL
#else
#ifdef FLIGHTSIMLIB_LIB
#define FSLCALL
#else
#ifdef FLIGHTSIMLIB_EXPORTS
#define FSLCALL __declspec(dllexport)
#else
#define FSLCALL __declspec(dllimport)
#endif
#endif
#endif

typedef int FslModuleId;

/**
 * \brief CGL Module ID for GetModule
 */
static FslModuleId Module_ICglModuleV1 = 1000;

typedef int FslResult;

// COM-Like abstract interface. But without COM!
class IFlightSimLib : public flightsimlib::IObject
{
  public:
    virtual FslResult GetModule(FslModuleId id, void** pp_module) = 0;
};

// Factory function that creates instances of FlightSimLib
extern "C" FSLCALL IFlightSimLib* FSLAPI GetFlightSimLib();