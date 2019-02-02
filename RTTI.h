#pragma once

#include <cstddef>
#include <string>

/// Represents a type information in runtime
struct RttiTypeId {
	int runtimeId; // Auto-generated runtime ID (for internal usage)
	std::string name; // Type name
	std::size_t size; // Amount of bytes for this type in memory

	RttiTypeId() = default;
};

/// Contains methods for operating with runtime type entries
class Rtti {
public:
	/// Creates RttiTypeId and registers it in internal registry
	static RttiTypeId * createTypeId(const std::string& name, std::size_t size);

	/// Creates RttiTypeId and registers it in internal registry. Template is used for
	/// automatic size calculation
	template <class T>
	static RttiTypeId * createTypeId(const std::string& name) {
		return createTypeId(name, sizeof(T));
	}

	/// Finds a type ID entry by the name
	static RttiTypeId *findTypeByName(const std::string& name);

private:
	// Prevent instantiating
	Rtti() {}

	/// Returns next free runtime ID
	static int nextRuntimeId();

private:
	/// Runtime ID counter
	static int _nextFreeRuntimeId;
};

/// Put this macro inside class declaration
#define RTTI_HAS_TYPE_ID \
	static RttiTypeId *__typeId; \
	\
	static RttiTypeId *typeId(); \
	\
	RttiTypeId *getTypeId() const override { \
		return __typeId; \
	}

/// 
#define RTTI_DEFINE_TYPE_ID(cppClassName, typeName) \
	RttiTypeId *cppClassName::__typeId = Rtti::createTypeId<cppClassName>(typeName); \
	\
	RttiTypeId *cppClassName::typeId() { \
		return cppClassName::__typeId; \
	}

/// 
struct TypeIdProvider {
	/// Returns runtime type ID of this instance
	virtual RttiTypeId *getTypeId() const = 0;
};
