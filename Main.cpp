#include <cassert>
#include <iostream>
#include <vector>

#include "RTTI.h"

struct FooObject : public TypeIdProvider {
	RTTI_HAS_TYPE_ID

	int x, y;
};

struct BarObject : public FooObject {
	RTTI_HAS_TYPE_ID

	float m[4];
	float n[4];
};

RTTI_DEFINE_TYPE_ID(FooObject, "FooObject");
RTTI_DEFINE_TYPE_ID(BarObject, "BarObject");

void dumpTypeId(const RttiTypeId *instance) {
	std::cout << "TYPE " << instance->name << ":" << std::endl
		<< "   SIZE: " << instance->size << std::endl
		<< "   RUNTIME ID: " << instance->runtimeId << std::endl;
}

int main()
{
	dumpTypeId(FooObject::typeId());
	dumpTypeId(BarObject::typeId());

	assert(FooObject::typeId() == FooObject::typeId());
	assert(FooObject::typeId() != BarObject::typeId());
	assert(FooObject::typeId()->name == "FooObject");
	assert(BarObject::typeId()->name == "BarObject");
}
