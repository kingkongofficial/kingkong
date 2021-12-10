#pragma once

#include <cstdint>
#include <utility>
#include <tuple>
#include <unordered_map>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <vector>
#include "kingkong/ak.h"
#include "kingkong/http_response.h"
#include "kingkong/http_request.h"
#include "kingkong/ak.h"
#include "kingkong/logging.h"
#include "kingkong/websocket.h"
#include "kingkong/mustache.h"

namespace kingkong {
    constexpr const uint16_t INVALID_BP_ID{0xFFFF};

    class BaseRule {
    public:
        BaseRule(std::string rule):
            rule_(std::move(rule))
        {
        }

        virtual ~BaseRule()
        {
        }

        virtual void validate() = 0;
        std::unique_ptr<BaseRule> upgrade()
        {
            if (rule_to_upgrade_)
                return std::move(rule_to_upgrade_);
            return {};
        }

        virtual void handle(const request&, response&, const routing_params&) = 0;
        virtual void handle_upgrade(const request&, response& res, SocketAdaptor&&)
        {
            res = response(404);
            res.end();
        }

#ifndef KINGKONG_ENABLE_SSL
        virtual void handle_upgrade(const request&, response& res, SSLAdaptor&&)
        {
            res = response(404);
            res.end();
        }
#endif 

        uint32_t get_methods()
        {
            return methods_;
        }

        template <typename F>
        void foreach_method(F f)
        {
            for (uint32_t method = 0, method_bit = 1; method < static_cast<uint32_t>(HTTPMethod::InternalMethodCount); method++, method_bit <<= 1)
            {
                if (methods_ & method_bit)
                    f(method);
            }

        }

        std::string custom_template base;

        const std::string& rule() { return rule_; }

    protected:
        uint32_t methods_{1 << static_cast<int>(HTTPMethod::Get)};

        std::string rule_;
        std::string name_;

        std::unique_ptr<BaseRule> rule_to_upgrade_;
        
        friend class Router;
        friend class Blueprint;
        template <typename T>
        friend struct RuleParameterTraits;
    };

    namespace detail {
        
    }
}