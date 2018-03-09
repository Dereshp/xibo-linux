#include "XlfParser.hpp"

#include "Image.hpp"
#include "Video.hpp"
#include "WebView.hpp"
#include "VideoHandler.hpp"

XlfParser::XlfParser(const std::string& full_path)
{
    spdlog::get(LOGGER)->debug(full_path);

    boost::property_tree::ptree tree;
    boost::property_tree::read_xml(full_path, tree);

    m_tree = tree.get_child("layout");
}

std::unique_ptr<MainLayout> XlfParser::parse_layout()
{
    spdlog::get(LOGGER)->debug("parse layout");

    auto attrs = m_tree.get_child("<xmlattr>");

    int schemaVersion = attrs.get<int>("schemaVersion");
    int width = attrs.get<int>("width");
    int height = attrs.get<int>("height");
    std::string backgroundImage = attrs.get_optional<std::string>("background").value_or(std::string{});
    std::string backgroundColor = attrs.get_optional<std::string>("bgcolor").value_or(std::string{});

    m_layout = std::make_unique<MainLayout>(schemaVersion, width, height, backgroundImage, backgroundColor);

    parse_xml_tree();

    return std::move(m_layout);
}

void XlfParser::parse_region(const boost::property_tree::ptree& region_node)
{
    spdlog::get(LOGGER)->debug("parse region");

    auto attrs = region_node.get_child("<xmlattr>");
    auto options = region_node.get_child("options");

    static int available_zindex = 0;

    int id = attrs.get<int>("id");
    int width = static_cast<int>(attrs.get<float>("width"));
    int height = static_cast<int>(attrs.get<float>("height"));
    int top = static_cast<int>(attrs.get<float>("top"));
    int left = static_cast<int>(attrs.get<float>("left"));
    auto zindex_optional = attrs.get_optional<int>("zindex");
    int zindex = zindex_optional ? zindex_optional.value() : available_zindex++;

    bool loop = options.get_optional<bool>("loop").value_or(false);
    auto type = options.get_optional<std::string>("transitionType").value_or("");
    auto direction = options.get_optional<std::string>("transitionDirection").value_or("");
    int duration = options.get_optional<int>("transitionDuration").value_or(0);

    m_layout->add_region(id, Size{width, height}, Point{left, top}, zindex, loop, Transition{type, direction, duration});
}

void XlfParser::parse_xml_tree()
{
    for(auto&& layout : m_tree)
    {
        if(layout.first == "region")
        {
            auto region_node = layout.second;
            parse_region(region_node);

            for(auto&& region : region_node)
            {
                if(region.first == "media")
                {
                    auto media_node = region.second;
                    auto type = media_node.get_child("<xmlattr>").get<std::string>("type");
                    auto render = media_node.get_child("<xmlattr>").get<std::string>("render");

                    if(type == "image")
                    {
                        parse_media<Image>(media_node);
                    }
                    else if(type == "video")
                    {
                        parse_media<Video>(media_node);
                    }
                    // NOTE DataSetView, Embedded, Text and Ticker can be rendered via webview
                    else if(render == "html" || type == "text" || type == "ticker" || type == "datasetview" || type == "embedded")
                    {
                        parse_media<WebView>(media_node);
                    }
                }
            }
        }
    }
}

// FIXME temporary workaround
std::string XlfParser::get_path(int id, const boost::optional<std::string>& uri, const std::string& type)
{
    if(!uri || type == "ticker" || type == "forecastio")
    {
        boost::property_tree::ptree tree;
        boost::property_tree::read_xml(XiboApp::example_dir() + "/requiredFiles.xml", tree);

        auto required_files = tree.get_child("RequiredFiles").get_child("RequiredFileList");
        for(auto&& required_file : required_files)
        {
            auto file_info = required_file.second;
            if(file_info.get<std::string>("FileType") == "resource" && file_info.get<int>("MediaId") == id)
            {
                return file_info.get<std::string>("Path");
            }
        }
        return std::string{};
    }
    return uri.value();
}
