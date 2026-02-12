#pragma once
#include "BaseCommand.h"

class DeleteObjectCommand : public BaseCommand {
public:
	DeleteObjectCommand(int id)
		: BaseCommand(CommandType::DeleteObject, id) {
	}

	virtual std::unique_ptr<BaseCommand> clone() override {
		return std::unique_ptr<DeleteObjectCommand>(new DeleteObjectCommand(objectId));
	}
};