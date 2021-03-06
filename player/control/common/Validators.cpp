#include "Validators.hpp"

#include "utils/Resources.hpp"
#include "utils/ColorToHexConverter.hpp"
#include "common/FileSystem.hpp"

Uri Validators::validateUri(const boost::optional<std::string>& uri)
{
    if(uri)
    {
        auto filesystem = std::make_unique<FileSystem>();
        auto fullPath = Resources::resDirectory() / uri.value();

        if(!FileSystem::isRegularFile(fullPath))
            throw std::runtime_error("Not valid path");

        return Uri{Uri::Scheme::File, fullPath};
    }
    return {};
}

uint32_t Validators::validateColor(const std::string& color)
{
    ColorToHexConverter converter;
    return converter.colorToHex(color);
}
