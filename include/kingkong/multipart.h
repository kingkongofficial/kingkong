#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "kingkong/http_request.h"
#include "kingkong/returnable.h"

namespace kingkong {
    namespace multipart
    {
<<<<<<< HEAD
        const std::string dd = "--";
        const std::string crlf = "\r\n";

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
                        str << "; " << it.first << '=' << pad(it.second);
                    }
                    str << crlf;
                }
                str << crlf;
                str << item.body << crlf;
                return str.str();
            }

            message(const ci_map& headers, const std::string& boundary, const std::vector<part>& sections):
              returnable("multipart/form-data"), headers(headers), boundary(boundary), parts(sections) {}

            message(const request& req):
              returnable("multipart/form-data"),
              headers(req.headers),
              boundary(get_boundary(get_header_value("Content-Type"))),
              parts(parse_body(req.body))
            {}

        private:
            std::string get_boundary(const std::string& header) const
            {
                size_t found = header.find("boundary=");
                if (found)
                    return header.substr(found + 9);
                return std::string();
            }

            std::vector<part> parse_body(std::string body)
            {

                std::vector<part> sections;

                std::string delimiter = dd + boundary;

                while (body != (crlf))
                {
                    size_t found = body.find(delimiter);
                    std::string section = body.substr(0, found);

                    body.erase(0, found + delimiter.length() + 2);
                    if (!section.empty())
                    {
                        sections.emplace_back(parse_section(section));
                    }
                }
                return sections;
            }

            part parse_section(std::string& section)
            {
                struct part to_return;

                size_t found = section.find(crlf + crlf);
                std::string head_line = section.substr(0, found + 2);
                section.erase(0, found + 4);

                parse_section_head(head_line, to_return);
                to_return.body = section.substr(0, section.length() - 2);
                return to_return;
            }

            void parse_section_head(std::string& lines, part& part)
            {
                while (!lines.empty())
                {
                    header to_add;

                    size_t found = lines.find(crlf);
                    std::string line = lines.substr(0, found);
                    lines.erase(0, found + 2);
                    if (!line.empty())
                    {
                        size_t found = line.find("; ");
                        std::string header = line.substr(0, found);
                        if (found != std::string::npos)
                            line.erase(0, found + 2);
                        else
                            line = std::string();

                        size_t header_split = header.find(": ");

                        to_add.value = std::pair<std::string, std::string>(header.substr(0, header_split), header.substr(header_split + 2));
                    }

                    while (!line.empty())
                    {
                        size_t found = line.find("; ");
                        std::string param = line.substr(0, found);
                        if (found != std::string::npos)
                            line.erase(0, found + 2);
                        else
                            line = std::string();

                        size_t param_split = param.find('=');

                        std::string value = param.substr(param_split + 1);

                        to_add.params.emplace(param.substr(0, param_split), trim(value));
                    }
                    part.headers.emplace_back(to_add);
                }
            }

            inline std::string trim(std::string& string, const char& excess = '"') const
            {
                if (string.length() > 1 && string[0] == excess && string[string.length() - 1] == excess)
                    return string.substr(1, string.length() - 2);
                return string;
            }

            inline std::string pad(std::string& string, const char& padding = '"') const
            {
                return (padding + string + padding);
            }
        };
=======
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
      
>>>>>>> 26ca80576ea68b32b8f3ff4e99690da4d7a307fa
    } 
} 