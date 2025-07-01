#include "error_handler.h"
// Are there any errors?
bool ErrorHandler::HaveError() {
	if (errors_.empty()) {
		return false;
	}
	else {
		return true;
	}
}

// Adds an error to the error list
void ErrorHandler::AddError(int line, ErrorCode error_code) {
	auto err = ListError.find(error_code);
	errors_.push_back({ line, error_code, err->second });
}

// Retrieves the errors
const std::vector<Error>& ErrorHandler::getErrors() const {
	return errors_;
}

// Outputs the errors to a file/stream
void ErrorHandler::print(std::ostream& out) const {
	for (const auto& err : errors_) {
		out << std::dec << "[Line " << err.line << "] Error: " << err.message << '\n';
	}
}

void ErrorHandler::clear() {
	errors_.clear();
}

ErrorHandler GlobalErrorHandler; // global variable to store and access errors