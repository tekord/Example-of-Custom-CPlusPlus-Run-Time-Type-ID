#pragma once

#include <cstddef>
#include <map>
#include <string>

struct RttiTypeId;

/// 
struct ITypeIdProvider {
	/// Returns runtime type ID of this instance
	virtual RttiTypeId *getTypeId() const;
};

/// Represents a type information in runtime
struct RttiTypeId {
	int runtimeId; // Auto-generated runtime ID (for internal usage)
	std::string name; // Type name
	std::size_t size; // Amount of bytes for this type in memory

	RttiTypeId() = default;

	// In C++ standard less than 17 calling this function may cause compile-time error if the specified
	// type don't have RTTI_HAS_TYPE_ID macro
	template <typename T>
	static const RttiTypeId* get() {
#if __cplusplus >= 201703L
		if constexpr (std::is_base_of<ITypeIdProvider, T>()) {
			return T::__typeId;
		}

		return nullptr;
#else
		return T::__typeId;
#endif
	}

	static const RttiTypeId* get(ITypeIdProvider* object) {
		return object->getTypeId();
	}

	static const RttiTypeId* get(ITypeIdProvider& object) {
		return object.getTypeId();
	}
};

/// Contains methods for operating with runtime type entries
class RttiContext final {
public:
	/// Returns global instance of RTTI context
	static RttiContext& instance();

	/// Creates RttiTypeId and registers it in internal registry
	RttiTypeId * createTypeId(const std::string& name, std::size_t size);

	/// Creates RttiTypeId and registers it in internal registry. Template is used for
	/// automatic size calculation
	template <class T>
	RttiTypeId * createTypeId(const std::string& name) {
		return createTypeId(name, sizeof(T));
	}

	/// Finds a type ID entry by the name
	RttiTypeId *findTypeByName(const std::string& name);

private:
	// Prevent external instantiating
	RttiContext();

	/// Returns next free runtime ID
	int nextRuntimeId();

	/// Runtime ID counter
	int _nextFreeRuntimeId;

	// Registry of types. RttiTypeId stored by value instead of pointer because
	// we want to avoid dynamic memory allocation
	std::map<std::string, RttiTypeId> rttiTypeMap;
};

/// Put this macro inside class declaration
#define RTTI_HAS_TYPE_ID \
	public: \
	\
	static RttiTypeId *__typeId; \
	\
	static RttiTypeId *typeId(); \
	\
	RttiTypeId *getTypeId() const override { \
		return __typeId; \
	}

/// Put this macro in CPP file
#define RTTI_DEFINE_TYPE_ID(cppClassName) \
	RttiTypeId *cppClassName::__typeId = RttiContext::instance().createTypeId<cppClassName>(#cppClassName); \
	\
	RttiTypeId *cppClassName::typeId() { \
		return cppClassName::__typeId; \
	}

/// Put this macro in CPP file
#define RTTI_DEFINE_CUSTOM_TYPE_ID(cppClassName, typeName) \
	RttiTypeId *cppClassName::__typeId = RttiContext::instance().createTypeId<cppClassName>(typeName); \
	\
	RttiTypeId *cppClassName::typeId() { \
		return cppClassName::__typeId; \
	}

/// Utility macro for getting type ID of the specified class
#define RTTI_GET_TYPE(cppClass) \
	(RttiTypeId::get<cppClass>())

/// Utility macro for getting type ID of the specified class. Use this version when you need maximum performance
#define RTTI_GET_TYPE_UNSAFE(cppClass) \
	(cppClass::__typeId)
