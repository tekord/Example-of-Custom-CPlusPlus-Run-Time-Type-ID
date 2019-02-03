#include <cassert>
#include <iostream>
#include <vector>

#include "RTTI.h"

struct NonRttiObject {
	int x;
	int y;
};

struct FooObject : public ITypeIdProvider {
	RTTI_HAS_TYPE_ID

	int x, y;
};

struct BarObject : public FooObject {
	RTTI_HAS_TYPE_ID

	float m[4];
	float n[4];
};

struct SomeObject : public FooObject {
	RTTI_HAS_TYPE_ID
};

RTTI_DEFINE_TYPE_ID(FooObject);
RTTI_DEFINE_TYPE_ID(BarObject);
RTTI_DEFINE_CUSTOM_TYPE_ID(SomeObject, "CustomObject");

void dumpTypeId(const RttiTypeId *instance) {
	if (instance == nullptr) {
		std::cout << "No type provided" << std::endl;

		return;
	}

	std::cout << "TYPE " << instance->name << ":" << std::endl
		<< "   SIZE: " << instance->size << std::endl
		<< "   RUNTIME ID: " << instance->runtimeId << std::endl;
}

class MethodBasedVisitor {
public:
	void visit(ITypeIdProvider &object) {
		auto type = object.getTypeId();

#define _CASE(cppClass) \
	else if (type == cppClass::typeId()) \
		visit(reinterpret_cast<cppClass&>(object))

		if (0)
			;
		_CASE(FooObject);
		_CASE(BarObject);
		else {
			std::cout << "Fallback for " << object.getTypeId()->name << std::endl;
		}

#undef _CASE
	}

	void visit(FooObject node) {
		std::cout << "Special visit for FooObject" << std::endl;
	}

	void visit(BarObject node) {
		std::cout << "Special visit for BarObject" << std::endl;
	}
};

class LambdaBasedVisitor {
public:
	void visit(ITypeIdProvider &object) {

		auto type = object.getTypeId();

#define _CASE(cppClass, lambda) \
	else if (type == cppClass::typeId()) \
		lambda(reinterpret_cast<cppClass&>(object))

		if (0)
			;
		_CASE(FooObject, [&](FooObject &node) {
			std::cout << "Special visit for FooObject" << std::endl;
		});
		_CASE(SomeObject, [&](SomeObject &node) {
			std::cout << "Special visit for SomeObject" << std::endl;
		});
		else {
			std::cout << "Fallback for " << object.getTypeId()->name << std::endl;
		}

#undef _CASE
	}
};

int main()
{
	std::cout << "__cplusplus: " << __cplusplus << std::endl;

	assert(FooObject::typeId() == FooObject::typeId());
	assert(FooObject::typeId() != BarObject::typeId());
	assert(FooObject::typeId()->name == "FooObject");
	assert(BarObject::typeId()->name == "BarObject");

	std::vector<ITypeIdProvider *> objects;
	objects.push_back(new BarObject());
	objects.push_back(new FooObject());
	objects.push_back(new SomeObject());
	objects.push_back(new BarObject());
	objects.push_back(new FooObject());
	objects.push_back(new SomeObject());

	dumpTypeId(FooObject::typeId());
	dumpTypeId(RttiTypeId::get(objects[0]));
	dumpTypeId(RttiTypeId::get<BarObject>());
	dumpTypeId(RTTI_GET_TYPE(SomeObject));

#if __cplusplus >= 201703L
	dumpTypeId(RttiTypeId::get<NonRttiObject>()); // expected nullptr
#endif

	dumpTypeId(RTTI_GET_TYPE_UNSAFE(BarObject));

	MethodBasedVisitor visitor1;

	std::cout << std::endl << "MethodBasedVisitor:" << std::endl;

	for (auto &i : objects) {
		visitor1.visit(*i);
	}

	LambdaBasedVisitor visitor2;

	std::cout << std::endl << "LambdaBasedVisitor:" << std::endl;

	for (auto &i : objects) {
		visitor2.visit(*i);
	}
}
