#include "IObject.h"

void flightsimlib::IObject::Release() const
{
	delete this;
}
