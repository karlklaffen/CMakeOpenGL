#include "NotCopyable.h"

NotCopyable::NotCopyable() : Deprecated(false) {

}

NotCopyable::NotCopyable(NotCopyable&& original) noexcept : Deprecated(false) {
	original.Deprecated = true;
}

bool NotCopyable::IsDeprecated() const {
	return Deprecated;
}