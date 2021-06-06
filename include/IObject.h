#pragma once

namespace flightsimlib
{

class IObject
{
public:
	virtual void Release() const;
	
protected:
	IObject() = default;
	IObject(const IObject&) = default;
	IObject(IObject&&) = default;
	IObject& operator=(const IObject&) = default;
	IObject& operator=(IObject&&) = default;
	virtual ~IObject() = default;
};

}