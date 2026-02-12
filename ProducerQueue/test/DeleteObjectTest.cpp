#include "ProducerQueue.h"

#include "command/SetObjectPropertyCommand.h"

#include <assert.h>

void delete_object_test() {
	ProducerQueue queue;
	assert(queue.createObject(1) == true);
	assert(queue.deleteObject(1) == true);

	Commands commands;
	queue.getCommand(0, 2, commands);

	assert(commands.commands.size() == 1);
	assert(commands.commands[0]->type == CommandType::DeleteObject);
	assert(commands.commands[0]->objectId == 1);
}

void double_delete_object_test() {
	ProducerQueue queue;
	assert(queue.createObject(1) == true);
	assert(queue.deleteObject(1) == true);
	assert(queue.deleteObject(1) == false);
}

void delete_object_before_existance_test() {
	ProducerQueue queue;
	assert(queue.deleteObject(1) == false);
}

void remove_redundant_create_delete_object_test_01() {
	ProducerQueue queue;
	assert(queue.createObject(1) == true);
	assert(queue.deleteObject(1) == true);
	assert(queue.createObject(1) == true);
	assert(queue.deleteObject(1) == true);
	assert(queue.createObject(1) == true);

	Commands commands;
	queue.getCommand(0, 5, commands);

	assert(commands.commands.size() == 2);
	assert(commands.commands[0]->type == CommandType::DeleteObject);
	assert(commands.commands[0]->objectId == 1);
	assert(commands.commands[1]->type == CommandType::CreateObject);
	assert(commands.commands[1]->objectId == 1);
}

void remove_redundant_create_delete_object_test_02() {
	ProducerQueue queue;
	assert(queue.createObject(1) == true);
	assert(queue.deleteObject(1) == true);
	assert(queue.createObject(1) == true);
	assert(queue.deleteObject(1) == true);
	assert(queue.createObject(1) == true);
	assert(queue.deleteObject(1) == true);

	Commands commands;
	queue.getCommand(0, 6, commands);

	assert(commands.commands.size() == 1);
	assert(commands.commands[0]->type == CommandType::DeleteObject);
	assert(commands.commands[0]->objectId == 1);
}

void remove_redundant_set_object_property_test() {
	ProducerQueue queue;
	assert(queue.createObject(1) == true);
	assert(queue.setObjectProperty(1, "age", 4) == true);
	assert(queue.setObjectProperty(1, "height", 10) == true);
	assert(queue.createObject(2) == true);
	assert(queue.setObjectProperty(2, "age", 5) == true);
	assert(queue.deleteObject(1) == true);

	Commands commands;
	queue.getCommand(0, 6, commands);

	assert(commands.commands.size() == 3);
	assert(commands.commands[0]->type == CommandType::CreateObject);
	assert(commands.commands[0]->objectId == 2);
	assert(commands.commands[1]->type == CommandType::SetObjectProperty);
	assert(commands.commands[1]->objectId == 2);
	assert(dynamic_cast<SetObjectPropertyCommand&>(*commands.commands[1]).name == "age");
	assert(dynamic_cast<SetObjectPropertyCommand&>(*commands.commands[1]).value == 5);
	assert(commands.commands[2]->type == CommandType::DeleteObject);
	assert(commands.commands[2]->objectId == 1);
}

int main() {
	delete_object_test();
	double_delete_object_test();
	delete_object_before_existance_test();
	remove_redundant_create_delete_object_test_01();
	remove_redundant_create_delete_object_test_02();
	remove_redundant_set_object_property_test();

	return 0;
}