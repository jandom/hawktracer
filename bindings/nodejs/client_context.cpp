//
// Created by Hojin Ghim on 23/03/2020.
//

#include "client_context.hpp"

#include "hawktracer/client_utils/stream_factory.hpp"
#include <utility>

namespace HawkTracer
{
namespace Nodejs
{

std::unique_ptr<ClientContext> ClientContext::create(const std::string &source, const EventCallback &event_callback)
{
    std::unique_ptr<parser::Stream> stream = ClientUtils::make_stream_from_string(source);
    if (!stream) {
        return nullptr;
    }

    std::unique_ptr<parser::KlassRegister> klass_register{new parser::KlassRegister()};
    std::unique_ptr<parser::ProtocolReader>
        reader{new parser::ProtocolReader(klass_register.get(), std::move(stream), true)};
    std::unique_ptr<ClientContext>
        context{new ClientContext(std::move(reader), std::move(klass_register), event_callback)};
    return context;
}

ClientContext::ClientContext(std::unique_ptr<parser::ProtocolReader> reader,
                             std::unique_ptr<parser::KlassRegister> klass_register,
                             const EventCallback &event_callback)
    : _reader(std::move(reader)), _klass_register(std::move(klass_register))
{
    _reader->register_events_listener([event_callback](const parser::Event &event)
                                      {
                                          std::vector<const parser::Event *> events{&event}; // TODO
                                          event_callback(events);
                                      });
    _reader->start();
}

ClientContext::~ClientContext()
{
    _reader->stop();
}

} // namespace Nodejs
} // namespace HawkTracer

