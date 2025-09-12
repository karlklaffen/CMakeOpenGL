#pragma once

#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

class Logger {

	template<typename T>
	static std::ostream& operator << (std::ostream& out, const std::set<T>& set) {
		const std::set<T>::const_iterator& pen = std::prev(set.end());
		out << "(";
		for (std::set<T>::const_iterator it = set.begin(); it != pen; it++) {
			out << (*it) << ", ";
		}
		return out << (*pen) << ")";
	}

	template<typename T>
	static std::ostream& operator << (std::ostream& out, const std::unordered_set<T>& set) {
		const std::unordered_set<T>::const_iterator& pen = std::prev(set.end());
		out << "(";
		for (std::unordered_set<T>::const_iterator it = set.begin(); it != pen; it++) {
			out << (*it) << ", ";
		}
		return out << (*pen) << ")";
	}

	template<typename T, typename U>
	static std::ostream& operator << (std::ostream& out, const std::pair<T, U>& pair) {
		return out << pair.first << ": " << pair.second << std::endl;
	}

	template<typename T, typename U>
	static std::ostream& operator << (std::ostream& out, const std::unordered_map<T, U>& map) {
		const std::unordered_map<T, U>::const_iterator& pen = std::prev(map.end());
		out << "(";
		for (std::unordered_map<T, U>::const_iterator it = map.begin(); it != pen; it++) {
			out << (*it) << ", ";
		}
		return out << (*pen) << ")";
	}

	template<typename T, typename U>
	static std::ostream& operator << (std::ostream& out, const std::map<T, U>& map) {
		const std::map<T, U>::const_iterator& pen = std::prev(map.end());
		out << "(";
		for (std::map<T, U>::const_iterator it = map.begin(); it != pen; it++) {
			out << (*it) << ", ";
		}
		return out << (*pen) << ")";
	}
};