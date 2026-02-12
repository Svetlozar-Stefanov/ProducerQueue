#pragma once
#include "BaseCommand.h"

#include <string>

class SetObjectPropertyCommand : public BaseCommand {
public:
	int value;
	std::string name;

	SetObjectPropertyCommand(int id, const std::string& name, int value)
		: BaseCommand(CommandType::SetObjectProperty, id), name(name), value(value) {}

	virtual std::unique_ptr<BaseCommand> clone() override {
		return std::unique_ptr<SetObjectPropertyCommand>(new SetObjectPropertyCommand(objectId, name, value));
	}
};

class SetObjectPropertyCommandPlaceholder : public BaseCommand {
public:
	int value;
	std::shared_ptr<const std::string> name;

	SetObjectPropertyCommandPlaceholder(int id, std::shared_ptr<const std::string> name, int value)
		: BaseCommand(CommandType::SetObjectPropertyPlaceholder, id), name(name), value(value) {
	}

	virtual std::unique_ptr<BaseCommand> clone() override {
		return std::unique_ptr<SetObjectPropertyCommand>(new SetObjectPropertyCommand(objectId, *name, value));
	}
};