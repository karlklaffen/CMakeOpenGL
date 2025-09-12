#pragma once

#include <unordered_map>
#include <string>

enum class TestEnum {
    APPLE,
    BANANA
};

template<typename E>
class SmartEnum {

public:

    SmartEnum(std::string defaultName = "") : DefaultName(defaultName) {

    }

    SmartEnum(const std::unordered_map<E, std::string>& names, std::string defaultName = "") : Names(names), DefaultName(defaultName) {

    }

    bool AddName(E e, std::string_view name) {

        if (Names.contains(e))
            return true;

        Names.try_emplace(e, name);

        return false;
    }

    bool SpecifyName(E e, std::string_view name) {

        auto it = Names.find(e);

        if (it == Names.end())
            Names.try_emplace(e, name);
        else
            it->second = name;

        return false;
    }

    std::string GetName(E e) const {
        auto it = Names.find(e);

        if (it == Names.end())
            return DefaultName;

        return it->second;
    }

private:
    std::unordered_map<E, std::string> Names;
    std::string DefaultName;
};