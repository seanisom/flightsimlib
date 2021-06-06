#include "CglModule.h"

#include "VectorTile.h"


namespace flightsimlib::cgl
{

IVectorTile* CCglModuleV1::CreateVectorTile() const
{
	// Simple factory - worry about wrapping another day
	return new VectorTile();
}

}
