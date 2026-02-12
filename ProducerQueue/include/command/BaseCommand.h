#pragma once
#include <memory>

enum CommandType {
	CreateObject,
	DeleteObject,
	SetObjectProperty,
	SetObjectPropertyPlaceholder
};

/**
* @brief Stores information about a command saved in history.
*/
class BaseCommand {
public:
	enum CommandType type;
	int objectId;

	BaseCommand(CommandType type, int id)
		: type(type), objectId(id) {
	}

	virtual ~BaseCommand() = default;

	/**
	* @brief Creates a copy of the object.
	* 
	* Used for simpler copying of commands from the history to the Commands struct inside GetCommands().
	* 
	* @note Exception:
	* In the SetObjectPropertyCommandPlaceholder class this function does not make a true copy.
	* For ease of use in GetCommand() it replaces its std::shared_ptr<const string> name member to the actual string pointed by the shared pointer.
	* 
	* @return A std::unique_ptr owning the object copy.
	*/
	virtual std::unique_ptr<BaseCommand> clone() = 0;
};