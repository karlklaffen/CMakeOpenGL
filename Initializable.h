#pragma once

// for objects that violate RAII (have chance to fail initialization)
class Initializable {

protected:
	Initializable(std::string_view name) : Initialized(false), Name(name) {

	}
	void ConfirmInitialization() {
		Initialized = true;
	}

public:
	bool InitializationIncomplete() const {
		if (!Initialized)
			std::cout << "ERROR: " << Name << " found to have failed during initialization" << std::endl;
		return !Initialized;
	};

private:
	bool Initialized;
	std::string Name;

};