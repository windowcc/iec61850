/**
 * 该文件是针对log4cpp第三方日志库做的二次封装
 * 依赖 pattern/Singleton.h 头文件以及log4cpp三方开源库
 * 
 * 用法:
 *     该文件引用了4个外部全局变量:
 *     1.log_name:日志输出文件(带路径).
 *     2.log_ins:日志的输出实例,可以通过不同的实例区分同一个应用,不同进程的日志
 *     3.log_level:日志输出级别
 *     4.log_tag:日志输出输出对象,目前支持回卷日志文件和控制台输出.(1:回卷日志,2:控制台日志)
 * 
 **/

#ifndef IEC61850_Log_H
#define IEC61850_Log_H

#include <log4cpp/Priority.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Appender.hh>
#include "singleton.h"

extern std::string log_name;            // 输出日志名称
extern std::string log_ins;             // 输出日志实例标志
extern std::string log_level;           // 日志输出等级
extern bool log_tag;                    // 日志输出标志

using namespace std;

#define LOG_DEBUG_ARGS(formatStr, ...) {\
    INSTANCE_SINGLETON(Log)->info(log4cpp::Priority::DEBUG, formatStr,##__VA_ARGS__); \
}

#define LOG_INFO_ARGS(formatStr, ...) {\
        INSTANCE_SINGLETON(Log)->info(log4cpp::Priority::INFO, formatStr,##__VA_ARGS__); \
    }

#define LOG_NOTICE_ARGS(formatStr, ...) {\
    INSTANCE_SINGLETON(Log)->info(log4cpp::Priority::NOTICE, formatStr,##__VA_ARGS__); \
}

#define LOG_WARN_ARGS(formatStr, ...) {\
    INSTANCE_SINGLETON(Log)->info(log4cpp::Priority::WARN, formatStr,##__VA_ARGS__); \
}

#define LOG_ERROR_ARGS(formatStr, ...) {\
    INSTANCE_SINGLETON(Log)->info(log4cpp::Priority::ERROR, formatStr,##__VA_ARGS__); \
}

#define LOG_CRIT_ARGS(formatStr, ...) {\
    INSTANCE_SINGLETON(Log)->info(log4cpp::Priority::CRIT, formatStr,##__VA_ARGS__); \
}


#define LOG_ALTER_ARGS(formatStr, ...) {\
    INSTANCE_SINGLETON(Log)->info(log4cpp::Priority::ALERT, formatStr,##__VA_ARGS__); \
}

#define LOG_FATAL_ARGS(formatStr, ...) {\
    INSTANCE_SINGLETON(Log)->info(log4cpp::Priority::FATAL, formatStr,##__VA_ARGS__); \
}

#define LOG_DEBUG(msg) {\
    INSTANCE_SINGLETON(Log)->getLogCategory() << \
        log4cpp::Priority::DEBUG << "[" << __FILE__ << "["<< __LINE__ << "]]: " << msg; \
}

#define LOG_INFO(msg) {\
    INSTANCE_SINGLETON(Log)->getLogCategory() << \
        log4cpp::Priority::INFO << "[" << __FILE__ << "["<< __LINE__ << "]]: " << msg; \
}


#define LOG_NOTICE(msg) {\
    INSTANCE_SINGLETON(Log)->getLogCategory() << \
        log4cpp::Priority::NOTICE << "[" << __FILE__ << "["<< __LINE__ << "]]: " << msg; \
}

#define LOG_WARN(msg) {\
    INSTANCE_SINGLETON(Log)->getLogCategory() << \
        log4cpp::Priority::WARN << "[" << __FILE__ << "["<< __LINE__ << "]]: " << msg; \
}

#define LOG_ERROR(msg) {\
    INSTANCE_SINGLETON(Log)->getLogCategory() << \
        log4cpp::Priority::ERROR << "[" << __FILE__ << "["<< __LINE__ << "]]: " << msg; \
}

#define LOG_CRIT(msg) {\
    INSTANCE_SINGLETON(Log)->getLogCategory() << \
        log4cpp::Priority::CRIT << "[" << __FILE__ << "["<< __LINE__ << "]]: " << msg; \
}

#define LOG_ALERT(msg) {\
    INSTANCE_SINGLETON(Log)->getLogCategory() << \
        log4cpp::Priority::ALERT << "[" << __FILE__ << "["<< __LINE__ << "]]: " << msg; \
}

#define LOG_FATAL(msg) {\
    INSTANCE_SINGLETON(Log)->getLogCategory() << \
        log4cpp::Priority::FATAL << "[" << __FILE__ << "["<< __LINE__ << "]]: " << msg; \
}


class Log : public Singleton<Log>
{
    public:
        log4cpp::Category& getLogCategory()
        {
            return log4cpp::Category::getInstance(log_ins);
        }
        void info(log4cpp::Priority::Value priority,
                 const char * stringFormat,...)
        {
            va_list va;
            va_start(va, stringFormat);
            getLogCategory().logva(priority,stringFormat,va);
            va_end(va);
        }
    private:
        Log()
        {
            try
            {
                log4cpp::Appender *appender = nullptr;
                if(log_tag)
                {
                    appender = new log4cpp::RollingFileAppender("default",log_name);
                }
                else
                {
                    appender = new log4cpp::OstreamAppender("default",&std::cout);
                }
                log4cpp::PatternLayout *patternlayout = new log4cpp::PatternLayout();
                patternlayout->setConversionPattern("[%d{%Y-%m-%d %H:%M:%S}][%p][%c]%m%n");
                appender->setLayout(patternlayout);
                log4cpp::Category& log = getLogCategory();
                log4cpp::Priority::Value priority = log4cpp::Priority::INFO;
                if(log_level == "DEBUG")
                {
                    priority = log4cpp::Priority::DEBUG;
                }
                else if(log_level == "INFO")
                {
                    priority = log4cpp::Priority::INFO;
                }
                else if(log_level == "NOTICE")
                {
                    priority = log4cpp::Priority::NOTICE;
                }
                else if(log_level == "WARN")
                {
                    priority = log4cpp::Priority::WARN;
                }
                else if(log_level == "ERROR")
                {
                    priority = log4cpp::Priority::ERROR;
                }
                else if(log_level == "CRIT")
                {
                    priority = log4cpp::Priority::CRIT;
                }
                else if(log_level == "ALERT")
                {
                    priority = log4cpp::Priority::ALERT;
                }
                else if(log_level == "FATAL")
                {
                    priority = log4cpp::Priority::FATAL;
                }
                else
                {
                    priority = log4cpp::Priority::INFO;
                }
                log.setPriority(priority);
                log.addAppender(appender);
            }
            catch (log4cpp::ConfigureFailure &e)
            {
                std::cout << "log module init failed." << std::endl;
                return ;
            }
        }
        ~Log()
        {
            log4cpp::Category::shutdown();
        }
        Log(const Log &) = delete;
        Log& operator=(const Log&) = delete;
        friend Singleton<Log>;
};


#endif // IEC104_LOG_H
