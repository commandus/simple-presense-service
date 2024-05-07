#include <string>
#include <iostream>
#include <csignal>
#include <climits>

#ifdef _MSC_VER
#include <direct.h>
#include <sstream>

#define PATH_MAX MAX_PATH
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

#include "argtable3/argtable3.h"

#include "uv-listener.h"

#define DEF_DB_GATEWAY_JSON  "gateway.json"


#include "err-msg.h"
#include "log.h"
#include "daemonize.h"
#include "ip-address.h"

// i18n
// #include <libintl.h>
// #define _(String) gettext (String)
#define _(String) (String)

const char* programName = "simple-presense-service";
        
enum IP_PROTO {
    PROTO_UDP,
    PROTO_TCP,
    PROTO_UDP_N_TCP
};

static std::string IP_PROTO2string(
    enum IP_PROTO value
)
{
    switch (value) {
    case PROTO_UDP:
        return "UDP";
    case PROTO_TCP:
        return "TCP";
    default:
        return "TCP, UDP";
    }
}

// global parameters and descriptors
class CliServiceDescriptorNParams {
public:
    UVListener server;
    enum IP_PROTO proto;
    std::string intf;
    uint16_t port;
    int32_t code;
    uint64_t accessCode;
    bool runAsDaemon;
    std::string pidfile;
    int verbose;
    std::string db;
    std::string dbGatewayJson;
    int32_t retCode;
    CliServiceDescriptorNParams()
        : proto(PROTO_UDP), port(4205), code(0), accessCode(0), verbose(0), retCode(0),
        runAsDaemon(false)
    {

    }

    std::string toString() const {
        std::stringstream ss;
        ss
            << _("Service: ") << intf << ":" << port << " " << IP_PROTO2string(proto) << ".\n"
            << _("Code: ") << std::hex << code << _(", access code: ") << accessCode << " " << "\n";
        if (!db.empty())
            ss << _("database file name: ") << db << "\n";
        return ss.str();
    }
};

CliServiceDescriptorNParams svc;

static void done() {
    std::cerr << MSG_GRACEFULLY_STOPPED << std::endl;
    exit(svc.retCode);
}

static void stop() {
    svc.server.stop();
}

void signalHandler(int signal)
{
    if (signal == SIGINT) {
        std::cerr << MSG_INTERRUPTED << std::endl;
        done();
    }
}

void setSignalHandler()
{
#ifdef _MSC_VER
#else
    struct sigaction action = {};
    action.sa_handler = &signalHandler;
    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGHUP, &action, nullptr);
#endif
}

void run() {
    svc.server.setAddress(svc.intf, svc.port);
    svc.retCode = svc.server.run();
    if (svc.retCode)
        std::cerr << ERR_MESSAGE << svc.retCode << ": "
            << appStrError(svc.retCode)
            << std::endl;
}

int main(int argc, char** argv) {
    struct arg_str* a_interface_n_port = arg_str0(nullptr, nullptr, _("ipaddr:port"), _("Default *:2024"));
    struct arg_int* a_code = arg_int0("c", "code", _("<number>"), _("Default 42. 0x - hex number prefix"));
    struct arg_str* a_access_code = arg_str0("a", "access", _("<hex>"), _("Default 2a (42 decimal)"));
    struct arg_lit* a_daemonize = arg_lit0("d", "daemonize", _("run daemon"));
    struct arg_str* a_pidfile = arg_str0("p", "pidfile", _("<file>"), _("Check whether a process has created the file pidfile"));
    struct arg_lit* a_verbose = arg_litn("v", "verbose", 0, 2, _("-v - verbose, -vv - debug"));
    struct arg_lit* a_help = arg_lit0("h", "help", _("Show this help"));
    struct arg_end* a_end = arg_end(20);

    void* argtable[] = {
        a_interface_n_port,
        a_code, a_access_code, a_verbose, a_daemonize, a_pidfile,
        a_help, a_end
    };

    // verify the argtable[] entries were allocated successfully
    if (arg_nullcheck(argtable) != 0) {
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return ERR_CODE_COMMAND_LINE;
    }
    // Parse the command line as defined by argtable[]
    int nerrors = arg_parse(argc, argv, argtable);

    svc.runAsDaemon = a_daemonize->count > 0;
    if (a_pidfile->count)
        svc.pidfile = *a_pidfile->sval;
    else
        svc.pidfile = "";

    svc.verbose = a_verbose->count;

    if (a_interface_n_port->count) {
        splitAddress(svc.intf, svc.port, std::string(*a_interface_n_port->sval));
    }
    else {
        svc.intf = "*";
        svc.port = 2024;
    }

    if (a_code->count)
        svc.code = *a_code->ival;
    else
        svc.code = 42;
    if (a_access_code->count)
        svc.accessCode = strtoull(*a_access_code->sval, nullptr, 16);
    else
        svc.accessCode = 42;

    // special case: '--help' takes precedence over error reporting
    if ((a_help->count) || nerrors) {
        if (nerrors)
            arg_print_errors(stderr, a_end, programName);
        std::cerr << _("Usage: ") << programName << std::endl;
        arg_print_syntax(stderr, argtable, "\n");
        std::cerr << _("LoRaWAN gateway storage service") << std::endl;
        arg_print_glossary(stderr, argtable, "  %-27s %s\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return ERR_CODE_COMMAND_LINE;
    }
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

#ifdef _MSC_VER
    WSADATA wsaData;
    int r = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (r) {
        std::cerr << ERR_WSA_STARTUP << std::endl;
        exit(r);
    }
#endif

    if (svc.runAsDaemon) {
        char workDir[PATH_MAX];
        std::string programPath = getcwd(workDir, PATH_MAX);
        if (svc.verbose)
            std::cerr << MSG_LISTENER_DAEMON_RUN
            << "(" << programPath << "/" << programName << "). "
            << MSG_CHECK_SYSLOG << std::endl;
        OPEN_SYSLOG(programName)
            Daemonize daemon(programName, programPath, run, stop, done, 0, svc.pidfile);
        // CLOSESYSLOG()
    }
    else {
        setSignalHandler();
        if (svc.verbose > 1)
            std::cerr << svc.toString();
        run();
        done();
    }
}
