#include "ReflectionInfo.h"

bool TypeInfo::HasAnnotation(const std::vector<std::string>& names) const
{
	for (const std::string& str : names)
	{
		if (std::find(Annotations.begin(), Annotations.end(), str) != Annotations.end())
		{
			return true;
		}
	}

	return false;
}

bool TypeInfo::HasAnnotation(std::string_view name) const
{
	return std::find(Annotations.begin(), Annotations.end(), name) != Annotations.end();
}
