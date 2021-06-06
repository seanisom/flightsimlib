#include "FlightSimLib.h"

#include "CglModule.h"

#include <memory>


class CFlightSimLib final : public IFlightSimLib
{
public:
	CFlightSimLib();
	
	FslResult GetModule(FslModuleId module, void **ppv) override;

private:
	std::unique_ptr<flightsimlib::cgl::CCglModuleV1> m_module_cgl;
};

CFlightSimLib::CFlightSimLib()
{
	m_module_cgl = std::make_unique<flightsimlib::cgl::CCglModuleV1>();
}

FslResult CFlightSimLib::GetModule(FslModuleId module, void** ppv)
{
	if (!ppv)
	{
		return 1;
	}

	*ppv = nullptr;

	if (module == Module_ICglModuleV1)
	{
		*ppv = m_module_cgl.get();
	}

	if (!ppv)
	{
		return 1;
	}

	return 0;
}

FSLCALL IFlightSimLib* FSLAPI GetFlightSimLib()
{
	return new CFlightSimLib();
}
