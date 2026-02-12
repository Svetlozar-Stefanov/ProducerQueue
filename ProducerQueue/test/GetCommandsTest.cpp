#include "ProducerQueue.h"

#include "command/SetObjectPropertyCommand.h"

#include <assert.h>


int main() {
	ProducerQueue queue;
	Commands commands;

	assert(queue.createObject(1) == true);
	assert(queue.setObjectProperty(1, "age", 4) == true);

	queue.getCommand(0, 2, commands);
	assert(commands.commands.size() == 2);
	assert(commands.commands[0]->type == CommandType::CreateObject);
	assert(commands.commands[0]->objectId == 1);
	assert(commands.commands[1]->type == CommandType::SetObjectProperty);
	assert(commands.commands[1]->objectId == 1);
	assert(dynamic_cast<SetObjectPropertyCommand&>(*commands.commands[1]).name == "age");
	assert(dynamic_cast<SetObjectPropertyCommand&>(*commands.commands[1]).value == 4);

	assert(queue.deleteObject(1) == true);
	assert(queue.createObject(2) == true);

	commands.commands.clear();
	int64_t next = queue.getCommand(0, 2, commands);
	assert(commands.commands.size() == 0);

	next = queue.getCommand(next, 2, commands);
	assert(commands.commands.size() == 2);
	assert(commands.commands[0]->type == CommandType::DeleteObject);
	assert(commands.commands[0]->objectId == 1);
	assert(commands.commands[1]->type == CommandType::CreateObject);
	assert(commands.commands[1]->objectId == 2);

	assert(queue.setObjectProperty(2, "age", 5) == true);
	assert(queue.createObject(3) == true);

	commands.commands.clear();
	queue.getCommand(next, 2, commands);
	assert(commands.commands.size() == 2);
	assert(commands.commands[0]->type == CommandType::SetObjectProperty);
	assert(commands.commands[0]->objectId == 2);
	assert(dynamic_cast<SetObjectPropertyCommand&>(*commands.commands[0]).name == "age");
	assert(dynamic_cast<SetObjectPropertyCommand&>(*commands.commands[0]).value == 5);
	assert(commands.commands[1]->type == CommandType::CreateObject);
	assert(commands.commands[1]->objectId == 3);

	return 0;
}