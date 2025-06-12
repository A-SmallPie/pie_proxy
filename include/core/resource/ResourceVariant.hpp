#include <variant>
#include "ProxyThreadResource.hpp"
#include "ServerThreadResource.hpp"

using ResourceVariant = std::variant<ProxyThreadResource*, ServerThreadResource*>;