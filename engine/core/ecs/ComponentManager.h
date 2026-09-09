// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <any>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include "ComponentArray.h"
#include "Log.h"

namespace doriax {

	using ComponentId = unsigned char;


	// Dense index per component type, so an array lookup is a vector subscript instead
	// of hashing typeid().name() on every call.
	class DORIAX_API ComponentTypeRegistry {
	public:
		static size_t indexOf(const char* mangledName);
	};

	template<typename T>
	inline size_t componentTypeIndex(){
		// Cached per module; the shared registry keeps the value equal across them.
		static const size_t index = ComponentTypeRegistry::indexOf(typeid(T).name());
		return index;
	}


	class DORIAX_API ComponentManager {
	private:
		struct ComponentEntry {
			std::shared_ptr<ComponentArrayBase> array;
			ComponentId id = 0;
		};

		// Indexed by componentTypeIndex<T>(); unregistered types keep an empty slot.
		std::vector<ComponentEntry> entries{};
		ComponentId nextComponentIdId{};

		const ComponentEntry* findEntry(size_t index) const {
			if (index >= entries.size() || !entries[index].array){
				return nullptr;
			}
			return &entries[index];
		}

	public:
		template<typename T>
		void registerComponent() {
			const size_t index = componentTypeIndex<T>();

			if (index >= entries.size()){
				entries.resize(index + 1);
			}

			if (entries[index].array){
				Log::error("Registering component type more than once");
				return;
			}

			// Ids stay sequential: they are signature bit positions.
			entries[index].array = std::make_shared<ComponentArray<T>>();
			entries[index].id = nextComponentIdId;

			++nextComponentIdId;
		}

		template<typename T>
		ComponentId getComponentId() const{
			if (const ComponentEntry* entry = findEntry(componentTypeIndex<T>())){
				return entry->id;
			}

			// Not a default: 0 is the first component's signature bit.
			Log::error("Component not registered before use");
			throw std::out_of_range("Component not registered before use");
		}

		// Raw pointer: copying the shared_ptr cost an atomic pair per access. Never
		// null, since every caller dereferences it right away.
		template<typename T>
		ComponentArray<T>* getComponentArray() const {
			if (const ComponentEntry* entry = findEntry(componentTypeIndex<T>())){
				return static_cast<ComponentArray<T>*>(entry->array.get());
			}

			Log::error("Component not registered before use");
			throw std::out_of_range("Component not registered before use");
		}

		template<typename T>
		void addComponent(Entity entity) {
			getComponentArray<T>()->insert(entity);
		}

		template<typename T>
		void addComponent(Entity entity, const T& component) {
			getComponentArray<T>()->insert(entity, component);
		}

		template<typename T>
		void addComponent(Entity entity, T&& component) {
			using Component = std::remove_cv_t<std::remove_reference_t<T>>;
			getComponentArray<Component>()->insert(entity, std::forward<T>(component));
		}

		template<typename T>
		void removeComponent(Entity entity) {
			getComponentArray<T>()->remove(entity);
		}

		template<typename T>
		T* findComponent(Entity entity) const {
			return getComponentArray<T>()->findComponent(entity);
		}

		template<typename T>
		T& getComponent(Entity entity) const {
			return getComponentArray<T>()->getComponent(entity);
		}

		template<typename T>
	    T* findComponentFromIndex(size_t index) const{
		    return getComponentArray<T>()->findComponentFromIndex(index);
	    }

		template<typename T>
	    T& getComponentFromIndex(size_t index) const {
		    return getComponentArray<T>()->getComponentFromIndex(index);
	    }

		void entityDestroyed(Entity entity) {
			for (auto const& entry : entries) {
				if (entry.array){
					entry.array->entityDestroyed(entity);
				}
			}
		}
	};

}

#endif //COMPONENTMANAGER_H
