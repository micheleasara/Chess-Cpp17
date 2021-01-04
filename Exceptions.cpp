#include "Exceptions.hpp"

namespace Chess {

 InvalidMove::ErrorCode InvalidMove::errorCode() const {
   return m_errorCode;
 }
 
 InvalidMove::InvalidMove(std::string const& what, ErrorCode code): 
   std::logic_error(what), m_errorCode(code) {}
}

