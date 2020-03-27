//
// Created by Hojin Ghim on 23/03/2020.
//

#ifndef CLIENT_CONTEXT_HPP
#define CLIENT_CONTEXT_HPP

#include <string>

#include "hawktracer/parser/event.hpp"
#include "hawktracer/parser/protocol_reader.hpp"
#include "hawktracer/parser/klass_register.hpp"

namespace HawkTracer
{
namespace Nodejs
{

class ClientContext
{
public:
    using EventCallback = std::function<void(std::unique_ptr<std::vector<parser::Event>>)>;
    static std::unique_ptr<ClientContext> create(const std::string &source, EventCallback event_callback);

    ~ClientContext();

private:
    ClientContext(std::unique_ptr<parser::ProtocolReader> reader,
                  std::unique_ptr<parser::KlassRegister> klass_register,
                  EventCallback event_callback);

    const std::unique_ptr<parser::ProtocolReader> _reader;
    const std::unique_ptr<const parser::KlassRegister> _klass_register;
    const EventCallback _event_callback;
};

} // namespace Nodejs
} // namespace HawkTracer

#endif //CLIENT_CONTEXT_HPP
