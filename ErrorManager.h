#pragma once

#include <string>
#include <iostream>

namespace ErrorManager {

	// return true if error
	static bool Assert(bool success, const std::string& message) {
		if (success)
			return false;

		throw std::exception(("Error: " + message).c_str());

		/*try {
			
		}
		catch (const std::exception& e) {
			
		}*/

		return true;
	}
}