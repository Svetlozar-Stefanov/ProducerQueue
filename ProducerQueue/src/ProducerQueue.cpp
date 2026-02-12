#include "ProducerQueue.h"

#include <memory>

#include "error/GetCommandError.h"
#include "command/CreateObjectCommand.h"
#include "command/DeleteObjectCommand.h"
#include "command/SetObjectPropertyCommand.h"

std::shared_ptr<const std::string> PropertyNameContainer::get(const std::string& name) {
	if (m_PropertyNames.find(name) == m_PropertyNames.end()) {
		std::shared_ptr<const std::string> namePtr = std::shared_ptr<std::string>(new std::string(name));
		m_PropertyNames[name] = namePtr;
		return namePtr;
	}
	
	return m_PropertyNames[name].lock();
}

void PropertyNameContainer::remove(const std::string& name) {
	const auto& nameItr = m_PropertyNames.find(name);
	if (nameItr != m_PropertyNames.end() && (*nameItr).second.expired()) {
		m_PropertyNames.erase(nameItr);
	}
}

bool ProducerQueue::createObject(int n) {
	if (m_State.find(n) == m_State.end()) {
		m_State[n] = ObjectState();
	}
	
	ObjectState& state = m_State[n];
	if (state.createdAt != -1 && (state.deletedAt < state.createdAt)) {
		return false; // Duplicate object creation
	}
	
	state.createdAt = m_Index;
	m_Commands.emplace(m_Index, new CreateObjectCommand(n));
	m_Index++;

	return true;
}

bool ProducerQueue::deleteObject(int n) {
	if (m_State.find(n) == m_State.end()) {
		return false; // Object does not exist
	}

	ObjectState& state = m_State[n];
	if (state.deletedAt > state.createdAt) {
		return false; // Duplicate object deletion
	}

	if (state.createdAt != -1) {
		m_Commands.erase(state.createdAt); // Handle redundant create command
		state.createdAt = -1;
	}
	if (state.deletedAt != -1) {
		m_Commands.erase(state.deletedAt); // Handle redundant delete command
		state.deletedAt = -1;
	}

	std::vector<std::string> redundantProperties;
	for (const auto& property : state.properties) {
		redundantProperties.push_back(*property.first);

		const PropertyState& propertyState = property.second;
		m_Commands.erase(propertyState.setAt); // Handle redundant set command
	}
	state.properties.clear();

	//Check for and erase unused property names
	for (const auto& name : redundantProperties) {
		m_PropertyNameContainer.remove(name);
	}

	state.deletedAt = m_Index;
	m_Commands.emplace(m_Index, new DeleteObjectCommand(n));
	m_Index++;
	
	return true;
}

bool ProducerQueue::setObjectProperty(int n, const char* name, int value) {
	if (m_State.find(n) == m_State.end()) {
		return false; // Object does not exist
	}

	ObjectState& state = m_State[n];
	if (state.deletedAt > state.createdAt) {
		return false; // Object was deleted
	}

	std::shared_ptr<const std::string> namePtr = m_PropertyNameContainer.get(name);
	if (state.properties.find(namePtr) != state.properties.end()) {
		PropertyState& propertyState = state.properties[namePtr];
		
		m_Commands.erase(propertyState.setAt); // Handle redundant set command

		propertyState.setAt = m_Index;
		propertyState.value = value;
	}
	else {
		state.properties[namePtr] = PropertyState(m_Index, value);
	}

	m_Commands.emplace(m_Index, new SetObjectPropertyCommandPlaceholder(n, namePtr, value));
	m_Index++;

	return true;
}

int64_t ProducerQueue::getCommand(int64_t startIndex, int count, Commands& commands) {
	if (startIndex >= m_Index || m_Index - startIndex < count) {
		return INDEX_OVERFLOW;
	}

	// Get the command at startIndex or the closest meaningful command that follows
	std::map<int64_t, std::unique_ptr<BaseCommand>>::iterator start = m_Commands.find(startIndex);
	if (start == m_Commands.end()) {
		start = m_Commands.upper_bound(startIndex);
	}

	int64_t currentIndex = (*start).first;
	while (currentIndex < startIndex + count) {
		BaseCommand* command = (*start).second.get();
		commands.commands.push_back(command->clone());

		start++;
		if (start == m_Commands.end()) {
			break;
		}
		currentIndex = (*start).first;
	}

	return startIndex + count;
}