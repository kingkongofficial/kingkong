#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "kingkong/returnable.h"
#include "kingkong/http_request.h"

namespace kingkong
{

    namespace multipart
    {
      const std::string dd = "--";
      const std::string crlf = "\r\n"  ;

      struct header
      {
          std::pair<std::string, std::string> value;
          std::unordered_map<std::string, std::string> params;
      };

      struct part
      {
          std::vector<header> headers;
          std::string body;
      };

      struct message : public returnable
      {
          ci_map headers;
          std::string boundary;
          std::vector<part> parts;
      }
      
    } 
    
    
}
