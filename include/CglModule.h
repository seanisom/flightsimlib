#pragma once

#include "ICglModule.h"

namespace flightsimlib::cgl
{


class CCglModuleV1 final : public ICglModuleV1
{
public:
	[[nodiscard]] IVectorTile* CreateVectorTile() const override;
};


}