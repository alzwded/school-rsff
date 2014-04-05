#ifndef PARSER_HXX
#define PARSER_HXX

#include "core.hxx"
#include <iostream>

namespace Parser {
void Parse(std::istream& stream, Beam::vector& beams, Sensor::vector& sensors);
} // namespace

#endif
