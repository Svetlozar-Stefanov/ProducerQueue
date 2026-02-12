#include "ProducerQueue.h"

#include <assert.h>

void create_object_test() {
	ProducerQueue queue;
	assert(queue.createObject(1) == true);

	Commands commands;
	queue.getCommand(0, 1, commands);

	assert(commands.commands.size() == 1);
	assert(commands.commands[0]->type == CommandType::CreateObject);
	assert(commands.commands[0]->objectId == 1);
}

void duplicate_create_object_test_01() {
	ProducerQueue queue;
	assert(queue.createObject(1) == true);
	assert(queue.createObject(1) == false);
}

void duplicate_create_object_test_02() {
	ProducerQueue queue;
	assert(queue.createObject(1) == true);
	assert(queue.deleteObject(1) == true);
	assert(queue.createObject(1) == true);
	assert(queue.deleteObject(1) == true);
	assert(queue.createObject(1) == true);
	assert(queue.createObject(1) == false);
}

int main() {
	create_object_test();
	duplicate_create_object_test_01();
	duplicate_create_object_test_02();

	return 0;
}