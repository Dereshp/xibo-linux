#include "RequiredFiles.hpp"

#include "Resources.hpp"
#include "utils/Utilities.hpp"

namespace Resources = XMDSResources::RequiredFiles;

const RegularFiles& RequiredFiles::Result::requiredFiles() const
{
    return m_requiredFiles;
}

const ResourceFiles& RequiredFiles::Result::requiredResources() const
{
    return m_requiredResources;
}

void RequiredFiles::Result::addFile(RegularFile&& file)
{
    m_requiredFiles.emplace_back(std::move(file));
}

void RequiredFiles::Result::addResource(ResourceFile&& resource)
{
    m_requiredResources.emplace_back(std::move(resource));
}

SOAP::RequestSerializer<RequiredFiles::Request>::RequestSerializer(const RequiredFiles::Request& request) : BaseRequestSerializer(request)
{
}

std::string SOAP::RequestSerializer<RequiredFiles::Request>::string()
{
    return createRequest(Resources::Name, request().serverKey, request().hardwareKey);
}

SOAP::ResponseParser<RequiredFiles::Result>::ResponseParser(const std::string& soapResponse) : BaseResponseParser(soapResponse)
{
}

RequiredFiles::Result SOAP::ResponseParser<RequiredFiles::Result>::doParse(const xml_node& node)
{
    auto requiredFilesXml = node.get<std::string>(Resources::RequiredFilesXml);
    auto filesNode = Utils::parseXmlFromString(requiredFilesXml).get_child(Resources::Files);

    RequiredFiles::Result result;

    for(auto [name, fileNode] : filesNode)
    {
        if(name != Resources::File) continue;

        auto fileAttrs = fileNode.get_child(Resources::FileAttrs);
        auto fileType = fileAttrs.get<std::string>(Resources::FileType);

        if(isLayout(fileType) || isMedia(fileType))
        {
            result.addFile(parseRegularFile(fileAttrs));
        }
        else if(isResource(fileType))
        {
            result.addResource(parseResourceFile(fileAttrs));
        }
    }

    return result;
}

RegularFile SOAP::ResponseParser<RequiredFiles::Result>::parseRegularFile(const xml_node& attrs)
{
    auto fileType = attrs.get<std::string>(Resources::FileType);
    auto id = attrs.get<int>(Resources::RegularFile::Id);
    auto size = attrs.get<size_t>(Resources::RegularFile::Size);
    auto md5 = attrs.get<std::string>(Resources::RegularFile::MD5);
    auto downloadType = toDownloadType(attrs.get<std::string>(Resources::RegularFile::DownloadType));
    auto [path, name] = parseFileNameAndPath(downloadType, fileType, attrs);

    return RegularFile{id, size, md5, path, name, fileType, downloadType};
}

ResourceFile SOAP::ResponseParser<RequiredFiles::Result>::parseResourceFile(const xml_node& attrs)
{
    auto layoutId = attrs.get<int>(Resources::ResourceFile::MediaId);
    auto regionId = attrs.get<int>(Resources::ResourceFile::RegionId);
    auto mediaId = attrs.get<int>(Resources::ResourceFile::MediaId);

    return ResourceFile{layoutId, regionId, mediaId};
}

bool SOAP::ResponseParser<RequiredFiles::Result>::isLayout(std::string_view type) const
{
    return type == Resources::LayoutType;
}

bool SOAP::ResponseParser<RequiredFiles::Result>::isMedia(std::string_view type) const
{
    return type == Resources::MediaType;
}

bool SOAP::ResponseParser<RequiredFiles::Result>::isResource(std::string_view type) const
{
    return type == Resources::ResourceType;
}

DownloadType SOAP::ResponseParser<RequiredFiles::Result>::toDownloadType(std::string_view type)
{
    if(type == Resources::RegularFile::HTTPDownload)
        return DownloadType::HTTP;
    else if(type == Resources::RegularFile::XMDSDownload)
        return DownloadType::XMDS;

    return DownloadType::Invalid;
}


// NOTE: workaround because filePath and fileName from RequiredFiles request are a bit clumsy to parse directly
std::pair<std::string, std::string>
SOAP::ResponseParser<RequiredFiles::Result>::parseFileNameAndPath(DownloadType dType, std::string_view fType, const xml_node& attrs)
{
    std::string path, name;

    switch(dType)
    {
        case DownloadType::HTTP:
            path = attrs.get<std::string>(Resources::RegularFile::Path);
            name = attrs.get<std::string>(Resources::RegularFile::Name);
            break;
        case DownloadType::XMDS:
            name = attrs.get<std::string>(Resources::RegularFile::Path);
            if(isLayout(fType))
            {
                name += ".xlf";
            }
            break;
        default:
            break;
    }

    return std::pair{path, name};
}
