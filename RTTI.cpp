#include "RTTI.h"

static RttiContext* s_rttiContext;

RttiTypeId * ITypeIdProvider::getTypeId() const
{
	return nullptr;
}

RttiContext & RttiContext::instance()
{
	if (s_rttiContext == nullptr) {
		s_rttiContext = new RttiContext();
	}

	return *s_rttiContext;
}

RttiContext::RttiContext()
{
	_nextFreeRuntimeId = 0;
}

RttiTypeId* RttiContext::createTypeId(const std::string& name, std::size_t size)
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

RttiTypeId* RttiContext::findTypeByName(const std::string& name)
{
	auto it = rttiTypeMap.find(name);

	if (it != rttiTypeMap.end())
		return &it->second;

	return nullptr;
}

int RttiContext::nextRuntimeId()
{
	return ++_nextFreeRuntimeId;
}
