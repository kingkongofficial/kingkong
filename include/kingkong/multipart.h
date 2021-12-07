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

          const std::string& get_header_value(const std::string& key) const
          {
              return kingkong::get_header_value(headers, key);
          }

          std::string dump() const override
          {
              std::stringstream str;
              std::string delimiter = dd + boundary;

              for (unsigned i = 0; i < parts.size(); i++)
              {
                  str << delimiter << crlf;
                  str << dump(i);
              }
              str << delimiter << dd << crlf;
              return str.str();
          }

          std::string dump(int part_) const
          {
              std::stringstream str;
              part item = parts[part_];
              for (header item_h : item.headers)
              {
                  str << item_h.value.first << ": " << item_h.value.second;
                  for (auto& it : item_h.params)
                  {
                      str << "; " << it.first << '=' << pad(it.second)
                  }
                  str << crlf
              }
              str << crlf;
              str << item.body << crlf;
              return str.str();
          }
      }
      
    } 
    
    
}
