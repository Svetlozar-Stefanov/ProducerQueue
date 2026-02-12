#pragma once
#include "BaseCommand.h"

class CreateObjectCommand : public BaseCommand {
public:
	CreateObjectCommand(int id)
		: BaseCommand(CommandType::CreateObject, id) {
	}

	virtual std::unique_ptr<BaseCommand> clone() override {
		return std::unique_ptr<CreateObjectCommand>(new CreateObjectCommand(objectId));
	}
};