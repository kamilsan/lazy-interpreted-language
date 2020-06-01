#pragma once

#include <string>

class Node;

[[noreturn]] void reportError(const std::string& message, const Node& node);
