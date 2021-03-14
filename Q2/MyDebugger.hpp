#ifndef MYDEBUGGER_H
#define MYDEBUGGER_H

#include <iostream>
#include <tuple>
#include <vector>
#include <set>
#include <map>

// REFER: https://github.com/fenilgmehta/CS744-PA4-Key-Value-Store/blob/master/src/MyDebugger.hpp

// DEBUG macros, to set the flag from commnad line, comment the below line and use `g++ name.cpp -DPRINT_DEBUG`
#define PRINT_DEBUG
#ifdef PRINT_DEBUG
#define printFunction(outStream, functionName, argDelimiter, lineDelimiter) template <typename Arg, typename... Args> inline void functionName(Arg&& arg, Args&&... args) { outStream << arg; (void)(int[]){0, (void(outStream << argDelimiter << args),0)...}; outStream << lineDelimiter; }
printFunction(std::cerr, printErr, " "<<"\033[1;31m"<<"●"<<"\033[0m"<<" ", '\n');
#define db(...) dbg(__LINE__, #__VA_ARGS__, __VA_ARGS__);
template<class T, class... U> void dbg(int32_t lineNo, const char *sdbg, T h, U... a) {std::cerr<<"\033[1;31m"<<"Debug (" << lineNo << ") : "<<"\033[0m"; std::cerr<<sdbg; std::cerr<<" "<<"\033[1;41m"<<"="<<"\033[0m"<<" "; printErr(h, a...); std::cout.flush(); std::cerr.flush();}
template <class S, class T>std::ostream& operator <<(std::ostream& os, const std::pair<S, T>& p) {return os << "pair(" << p.first << "\033[1;31m" << ", " << "\033[0m" << p.second << ")";}
template <class T>std::ostream& operator <<(std::ostream& os, const std::vector<T>& p) {os << "\033[1;32m" << "vector[ " << "\033[0m"; for (const auto& it : p) os << it << "\033[1;31m" << ", " << "\033[0m"; return os << "\033[1;32m" << "]" << "\033[0m";}
template <class T>std::ostream& operator <<(std::ostream& os, const std::set<T>& p) {os << "\033[1;32m" << "set[ "; for (const auto& it : p) os << it << "\033[1;31m" << ", " << "\033[0m"; return os << "\033[1;32m" << "]" << "\033[0m";}
template <class S, class T>std::ostream& operator <<(std::ostream& os, const std::map<S, T>& p) {os << "\033[1;32m" << "map[ " << "\033[0m"; for (const auto& it : p) os << it << "\033[1;31m" << ", " << "\033[0m"; return os << "\033[1;32m" << "]" << "\033[0m";}
#define dbiter(...) dbgIter(#__VA_ARGS__, __VA_ARGS__);
template <class T> void dbgIter(const char *sdbg, T a, T b) {std::cerr<<"\033[1;31m"<<"Debug: "<<"\033[0m"; std::cerr<<sdbg; std::cerr<<"\033[1;31m"<<" = "<<"\033[0m"; std::cerr << "["; for (T i = a; i != b; ++i) {if (i != a) std::cerr << ", "; std::cerr << *i;} std::cerr << "]\n"; std::cout.flush(); std::cerr.flush();}
#else
#define db(...) ;
#define dbiter(...) ;
#endif

// REFER: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
enum Code {
    FG_RED      = 31,
    FG_GREEN    = 32,
    FG_YELLOW   = 33,
    FG_BLUE     = 34,
    FG_DEFAULT  = 39,

    BG_RED      = 41,
    BG_GREEN    = 42,
    BG_YELLOW   = 43,
    BG_BLUE     = 44,
    BG_DEFAULT  = 49
};

#define color(enum_code) "\033[" << enum_code << "m"

// NOTE: TID is Thread ID
// REFER: https://www.geeksforgeeks.org/thread-get_id-function-in-c/

// TODO: comment this line when doing performance testing and when using IDE CmakeLists.txt
// #define DEBUGGING_ON

#ifdef DEBUGGING_ON

// REFER: https://stackoverflow.com/questions/7432100/how-to-get-integer-thread-id-in-c11
// (std::this_thread::get_id())

    std::mutex loggerMutex;

    /* Blue message */
    template <typename T>
    void log_info(const T& msg, bool prependNewLine = false, bool appendExtraNewLine = false) {
        loggerMutex.lock();
        if(prependNewLine) std::cout << '\n';
        std::cout << color(FG_BLUE) << "INFO : " << color(FG_DEFAULT) << msg << '\n';
        if(appendExtraNewLine) std::cout << '\n';
        std::cout.flush();
        loggerMutex.unlock();
    }

    /* Yellow message */
    template <typename T>
    void log_warning(const T& msg, bool prependNewLine = false, bool appendExtraNewLine = false) {
        if(prependNewLine) std::cerr << '\n';
        std::cerr << color(FG_YELLOW) << "WARNING : " << color(FG_DEFAULT) << msg << '\n';
        if(appendExtraNewLine) std::cerr << '\n';
        std::cerr.flush();
    }

#else

    // template<typename T>
    // void log_info(const T &msg, bool prependNewLine = false, bool appendExtraNewLine = false) {}

    #define log_info(...) ;
    #define log_warning(...) ;

#endif


/* Yellow message */
template <typename T>
void log_error_warning(const T& msg, bool prependNewLine = false, bool appendExtraNewLine = false) {
    if(prependNewLine) std::cerr << '\n';
    std::cerr << color(FG_YELLOW) << "WARNING : " << color(FG_DEFAULT) << msg << '\n';
    if(appendExtraNewLine) std::cerr << '\n';
    std::cerr.flush();
}

/* Green message */
template <typename T>
void log_success(const T& msg, bool prependNewLine = false, bool appendExtraNewLine = false) {
    if(prependNewLine) std::cout << '\n';
    std::cout << color(FG_GREEN) << "SUCCESS : " << color(FG_DEFAULT) << msg << '\n';
    if(appendExtraNewLine) std::cout << '\n';
    std::cout.flush();
}

/* Red message */
template <typename T>
void log_error(const T& msg, bool prependNewLine = false, bool appendExtraNewLine = false){
    if(prependNewLine) std::cerr << '\n';
    std::cerr << color(FG_RED) << "ERROR : " << color(FG_DEFAULT) << msg << std::endl;
    if(appendExtraNewLine) std::cerr << '\n';
    std::cerr.flush();
}

#undef color

#endif // MYDEBUGGER_H