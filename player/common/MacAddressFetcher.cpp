#include "MacAddressFetcher.hpp"

#include "common/logger/Logging.hpp"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include <boost/format.hpp>

const std::size_t CONFIG_BUFFER_SIZE = 1024;
const std::size_t MAC_ADDRESS_BUFFER = 100;
const int INVALID_SOCKET = -1;

MacAddressError::MacAddressError(std::string_view error) : m_error(error)
{
}

const char* MacAddressError::what() const noexcept
{
    return m_error.data();
}

boost::optional<std::string> MacAddressFetcher::get()
{
    try
    {
        char buffer[CONFIG_BUFFER_SIZE];

        auto socket = openSocket();
        auto interfaceConfig = getInterfaceConfig(socket, buffer);
        auto interfaceRequest = findInterface(socket, interfaceConfig);

        return retrieveMacAddress(socket, interfaceRequest);
    }
    catch (MacAddressError& error)
    {
        Log::error("MAC address was not fetched: {}", error.what());
        return {};
    }
}

SocketDescriptor MacAddressFetcher::openSocket()
{
    SocketDescriptor sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(sock == INVALID_SOCKET)
    {
        throw MacAddressError{"Socket was not opened"};
    }
    return sock;
}

ifconf MacAddressFetcher::getInterfaceConfig(SocketDescriptor socket, char* buffer)
{
    ifconf interfaceConfig;
    interfaceConfig.ifc_len = CONFIG_BUFFER_SIZE;
    interfaceConfig.ifc_buf = buffer;

    if(!ioctl(socket, SIOCGIFCONF, &interfaceConfig))
    {
        return interfaceConfig;
    }

    throw MacAddressError{"Interface Config was not recieved"};
}

ifreq MacAddressFetcher::findInterface(SocketDescriptor socket, ifconf& interfaceConfig)
{
    const std::size_t configSize = static_cast<std::size_t>(interfaceConfig.ifc_len) / sizeof(ifreq);

    ifreq* it = interfaceConfig.ifc_req;
    const ifreq* const end = it + configSize;

    for(; it != end; ++it)
    {
        auto flags = retrieveFlags(socket, *it);
        if(isNotLoopback(flags))
        {
            ifreq interfaceRequest;
            std::strcpy(interfaceRequest.ifr_name, it->ifr_name);
            return interfaceRequest;
        }
    }

    throw MacAddressError{"Interface Request was not found"};
}

std::string MacAddressFetcher::retrieveMacAddress(SocketDescriptor socket, ifreq& interfaceRequest)
{
    if(!ioctl(socket, SIOCGIFHWADDR, &interfaceRequest))
    {
        char macAddress[MAC_ADDRESS_BUFFER] = {0};
        unsigned char* mac = reinterpret_cast<unsigned char*>(interfaceRequest.ifr_hwaddr.sa_data);

        std::sprintf(macAddress, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        return macAddress;
    }

    throw MacAddressError{"MAC Address was not retrived from request"};
}

InterfaceFlags MacAddressFetcher::retrieveFlags(SocketDescriptor socket, ifreq& interfaceRequest)
{
    if(!ioctl(socket, SIOCGIFFLAGS, &interfaceRequest))
    {
        return interfaceRequest.ifr_flags;
    }

    throw MacAddressError{"Flags were not retrived from frequest"};
}

bool MacAddressFetcher::isNotLoopback(InterfaceFlags flags)
{
    return !(flags & IFF_LOOPBACK);
}
