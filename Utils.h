#pragma once

#include <unordered_set>
#include <algorithm>
#include <string>
#include <ostream>
#include <map>
#include <unordered_map>
#include <sstream>
#include <set>
#include <format>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Utils { // put some of these into their own files

	enum class ErrorReturn {
		NONE,
		ALREADY_PRESENT,
		ALL_ALREADY_PRESENT,
		SOME_ALREADY_PRESENT,
		NOT_PRESENT,
		NONE_PRESENT,
		SOME_NOT_PRESENT,
	};

	enum class Direction {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	struct Rect {
		int X;
		int Y;
		unsigned int Width;
		unsigned int Height;

		std::string ToString() const {
			return std::format("Rect({}, {}, {}, {})", X, Y, Width, Height);
		}
	};

	template<typename T, typename U>
	class OrderedMap {

	public:
		const U& At(const T& key) const {
			return AtIndex(Indices.at(key));
		}

		U& At(const T& key) {
			return AtIndex(Indices.at(key));
		}

		const U& AtIndex(unsigned int index) const {
			return Elements.at(index);
		}

		U& AtIndex(unsigned int index) {
			return Elements.at(index);
		}

	private:
		std::vector<U> Elements;
		std::unordered_map<T, unsigned int> Indices;
	};

	static bool PointInRect(int px, int py, int x, int y, int w, int h) {
		return (px >= x) && (px <= x + w) && (py >= y) && (py <= y + h);
	}

	static bool PointInRect(glm::vec2 p, Rect rect) {
		return (p.x >= rect.X) && (p.x <= rect.X + rect.Width) && (p.y >= rect.Y) && (p.y <= rect.Y + rect.Height);
	}

	static size_t FindFirstCharLoopingBackwards(std::string_view str, char c, unsigned int start) {
		// loop backwards to find first previous char
		size_t i = start;
		while (i > 0 && str.at(i) != c) {
			i--;
		}

		return i;
	}

	static size_t FindFirstCharLoopingBackwards(std::string_view str, char c, std::string_view::const_iterator it) {
		return FindFirstCharLoopingBackwards(str, c, it - str.begin());
	}

	static std::vector<std::string> SeparateTextLines(const std::string& text) {
		std::stringstream textstream(text);
		std::string piece;

		std::vector<std::string> texts;

		while (std::getline(textstream, piece, '\n'))
			texts.push_back(piece);

		return texts;
	}

	template<typename T>
	static void Insert(std::unordered_set<T>& original, const std::unordered_set<T>& added) {
		original.insert(added.begin(), added.end());
	}

	template<typename T>
	static std::unordered_set<T> InFirstNotSecond(const std::unordered_set<T>& first, const std::unordered_set<T>& second) {

		std::unordered_set<T> result;

		for (const T& element : first)
			if (!second.contains(element))
				result.insert(element);

		return result;
	}

	template<typename T, size_t S>
	static glm::vec<S, T> ToVec(const std::array<T, S>& arr) {
		glm::vec<S, T> vec{ 0 };

		for (size_t i = 0; i < arr.size(); i++) {
			vec[i] = arr.at(i);
		}

		return vec;
	}

	template<typename T, size_t N, size_t S>
	std::ostream& operator<<(std::ostream& stream, const glm::mat<N, S, T> mat) {
		stream << glm::to_string(mat);
		return stream;
	}

	/*template<typename T, typename U>
	static std::string ToString(const std::pair<T, U>& pair) {
		return pair.first + ": " + pair.second;
	}*/

	/*template<typename T>
	static std::string ContainerToString(T con) {
		if (con.empty())
			return "(empty)";
		auto pen = std::prev(con.end());
		std::string s = "(";
		for (auto it = con.begin(); it != pen; it++) {
			s += ToString(*it) + ", ";
		}
		return s + ToString(*pen) + ")";
	}*/

	

	//template<typename T>
	//std::ostream& operator<<(std::ostream& stream, const )

	//template<typename T>
	//static std::string ToString(const std::vector<T>& vec) {
	//	return ContainerToString(vec);
	//}

	//template<typename T>
	//static std::string ToString(const std::set<T>& set) {
	//	return ContainerToString(set);
	//}

	//template<typename T>
	//static std::string ToString(const std::unordered_set<T>& set) {
	//	return ContainerToString(set);
	//}

	//template<typename T, typename U>
	//static std::string ToString(const std::unordered_map<T, U>& map) {
	//	return ContainerToString(map);
	//}

	//template<typename T, typename U>
	//static std::string ToString(const std::map<T, U>& map) {
	//	return ContainerToString(map);
	//}

	//static std::string ToString(const std::ostream& out) {
	//	std::stringstream ss;
	//	ss << out.rdbuf();
	//	return ss.str();
	//}
}