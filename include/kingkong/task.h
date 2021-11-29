#pragma once

#include <boost/asio.hpp>
#include <chrono>
#include <functional>
#include <map>
#include <vector>
#include "kingkong/logging.h"

namespace kingkong {
    
    namespace detail {

        class task_timer
        {
        public:
            using task_timer = std::function<void()>;
            using identifier_type = size_t;

        private:
            using clock_type = std::chrono::steady_clock;
            using time_type = clock_type::time_point;
        }
        public:
            void cancel(identifier_type id)
            {
                tasks_.erase(id);
                KINGKONG_LOG_DEBUG << "task cancel: " << this << ' ' << id;
            }

        private:
            std::uint8_t default_timeout_{5};
            boost::asio::io_service& io_service_;
            boost::asio::deadline_timer deadline_timer_;
            std::map<identifier_type, std::pair<time_type, task_type>> tasks_;
            
            identifier_type highest_id_{0};

    }

}