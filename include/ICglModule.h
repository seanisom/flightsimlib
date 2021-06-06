#pragma once

#include "IObject.h"

namespace flightsimlib::cgl
{

class IVectorTile;


class ICglModuleV1 : public IObject
{
public:
	[[nodiscard]] virtual IVectorTile* CreateVectorTile() const = 0;
};

}