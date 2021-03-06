#pragma once

#include "Soap.hpp"
#include "BaseResponseParser.hpp"
#include "BaseRequestSerializer.hpp"

#include "common/Field.hpp"

namespace SubmitScreenShot
{
    struct Result
    {
        bool success;
    };

    struct Request
    {
        Field<std::string> serverKey{"serverKey"};
        Field<std::string> hardwareKey{"hardwareKey"};
        Field<std::string> screenShot{"screenShot"};
    };
}

template<>
class Soap::RequestSerializer<SubmitScreenShot::Request> : public BaseRequestSerializer<SubmitScreenShot::Request>
{
public:
    RequestSerializer(const SubmitScreenShot::Request& request);
    std::string string();

};

template<>
class Soap::ResponseParser<SubmitScreenShot::Result> : public BaseResponseParser<SubmitScreenShot::Result>
{
public:
    ResponseParser(const std::string& soapResponse);

protected:
    SubmitScreenShot::Result doParse(const xml_node& node) override;

};
