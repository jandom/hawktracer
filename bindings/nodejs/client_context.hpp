//
// Created by Hojin Ghim on 23/03/2020.
//

#ifndef CLIENT_CONTEXT_HPP
#define CLIENT_CONTEXT_HPP

#include <string>

#include "hawktracer/parser/protocol_reader.hpp"
#include "hawktracer/parser/klass_register.hpp"

namespace HawkTracer
{
namespace Nodejs
{

class ClientContext
{
public:
    static std::unique_ptr<ClientContext> create(const std::string &source);

private:
    ClientContext(std::unique_ptr<parser::ProtocolReader> reader,
                  std::unique_ptr<parser::KlassRegister> klass_register);

    std::unique_ptr<parser::ProtocolReader> _reader;
    std::unique_ptr<parser::KlassRegister> _klass_register;
};

} // namespace Nodejs
} // namespace HawkTracer

#endif //CLIENT_CONTEXT_HPP
