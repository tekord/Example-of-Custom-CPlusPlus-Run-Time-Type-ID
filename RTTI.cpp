#include <map>
#include "RTTI.h"

int Rtti::_nextFreeRuntimeId = 0;

// Global registry of types. RttiTypeId stored by value instead of pointer because
// we want to avoid dynamic memory allocation
std::map<std::string, RttiTypeId> rttiTypeMap;

RttiTypeId* Rtti::createTypeId(const std::string& name, std::size_t size)
{
	RttiTypeId result;
	result.runtimeId = nextRuntimeId();
	result.name = name;
	result.size = size;

#if __cplusplus >= 201703L
	auto entry = rttiTypeMap.insert_or_assign(name, result);

	return &entry.first->second;
#else
	rttiTypeMap[name] = result;

	return &rttiTypeMap[name];
#endif
}

RttiTypeId* Rtti::findTypeByName(const std::string& name)
{
	auto it = rttiTypeMap.find(name);

	if (it != rttiTypeMap.end())
		return &it->second;

	return nullptr;
}

int Rtti::nextRuntimeId()
{
	return ++Rtti::_nextFreeRuntimeId;
}
