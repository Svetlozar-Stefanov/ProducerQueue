#include "ProducerQueue.h"

#include "command/SetObjectPropertyCommand.h"

#include <assert.h>

void set_property_test() {
	ProducerQueue queue;
	assert(queue.createObject(1) == true);
	assert(queue.setObjectProperty(1, "age", 4) == true);

	Commands commands;
	queue.getCommand(0, 2, commands);

	assert(commands.commands.size() == 2);
	assert(commands.commands[0]->type == CommandType::CreateObject);
	assert(commands.commands[0]->objectId == 1);
	assert(commands.commands[1]->type == CommandType::SetObjectProperty);
	assert(commands.commands[1]->objectId == 1);
	assert(dynamic_cast<SetObjectPropertyCommand&>(*commands.commands[1]).name == "age");
	assert(dynamic_cast<SetObjectPropertyCommand&>(*commands.commands[1]).value == 4);
}

void reassign_property_test() {
	ProducerQueue queue;
	assert(queue.createObject(1) == true);
	assert(queue.setObjectProperty(1, "age", 4) == true);
	assert(queue.setObjectProperty(1, "age", 8) == true);

	Commands commands;
	queue.getCommand(0, 3, commands);

	assert(commands.commands.size() == 2);
	assert(commands.commands[0]->type == CommandType::CreateObject);
	assert(commands.commands[0]->objectId == 1);
	assert(commands.commands[1]->type == CommandType::SetObjectProperty);
	assert(commands.commands[1]->objectId == 1);
	assert(dynamic_cast<SetObjectPropertyCommand&>(*commands.commands[1]).name == "age");
	assert(dynamic_cast<SetObjectPropertyCommand&>(*commands.commands[1]).value == 8);
}

void set_property_for_non_existant_test() {
	ProducerQueue queue;
	assert(queue.setObjectProperty(1, "age", 4) == false);
}

int main() {
	set_property_test();
	set_property_for_non_existant_test();
	reassign_property_test();

	return 0;
}