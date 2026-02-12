#include "command/BaseCommand.h"

#include <map>
#include <list>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_set>
#include <unordered_map>

/**
* @brief Stores information about a property.
*/
struct PropertyState {
	int64_t setAt;
	int value;

	PropertyState() : setAt(-1), value(-1) {}
	PropertyState(int64_t setAt, int value) : setAt(setAt), value(value) {}
};

/**
* @brief Stores information about an object.
*
* @param properties A map holdin the state of each property. The map key is a shared pointer instead of a string to save space.
*/
struct ObjectState {
	int64_t createdAt;
	int64_t deletedAt;
	
	std::unordered_map<std::shared_ptr<const std::string>, PropertyState> properties;

	ObjectState() : createdAt(-1), deletedAt(-1) {}
};

/**
* @brief A property name container.
* 
* Owns all property name strings. Provides access to them and tracks their lifetime.
*/
class PropertyNameContainer {
private:
	std::unordered_map<std::string, std::weak_ptr<const std::string>> m_PropertyNames;

public:
	/**
	* @brief Provides access to the name string. 
	*
	* @note Creates string before sharing access, if string does not exist.
	* 
	* @return A std::shared_ptr to the name string.
	*/
	std::shared_ptr<const std::string> get(const std::string& name);

	/**
	* @brief Prompts delete check. If name string is not used by anyone deletes it from the container.
	*/
	void remove(const std::string& name);
};

struct Commands {
	std::vector<std::unique_ptr<BaseCommand>> commands;
};

class ProducerQueue {
private:
	int64_t m_Index;

	PropertyNameContainer m_PropertyNameContainer;

	/**
	* @brief Stores each objects latest state. Employs a hash map for O(1) access time.
	*/
	std::unordered_map<int, ObjectState> m_State;

	/**
	* @brief Stores the command history. Employs a red-black BST for O(logN) insert, delete and lookup(upper_bound).
	* 
	* Enables us to assume the missing indices between elements to be a NoOp without keeping any data. 
	* Relies on fast upper_bound to find the closest index to startIndex in getCommands and start from there.
	*/
	std::map<int64_t, std::unique_ptr<BaseCommand>> m_Commands;
public:
	ProducerQueue() : m_Index(0) {}

	/**
	* @brief Creates an object. Does not allow for duplicate create commands.
	*/
	bool createObject(int n);

	/**
	* @brief Deletes an object. Does not allow for duplicate delete commands.
	* 
	* Removes redundant create, delete and setObjectProperty commands from the history.
	*/
	bool deleteObject(int n);

	/**
	* @brief Sets the value of a property.
	*/
	bool setObjectProperty(int n, const char* name, int value);

	/**
	* @brief Returns a range of commands from the command history.
	*/
	int64_t getCommand(int64_t startIndex, int count, Commands& commands);
};
