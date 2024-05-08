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
#include "err-msg.h"
#include "log.h"
#include "daemonize.h"
#include "ip-address.h"

// i18n
// #include <libintl.h>
// #define _(String) gettext (String)
#define _(String) (String)

const char* programName = "simple-presence-service";
        
// global parameters and descriptors
class CliSvcDescriptorNParams {
public:
    UVListener server;
    std::string intf;
    uint16_t port;
    bool runAsDaemon;
    std::string pidfile;
    int verbose;
    std::string db;
    std::string dbGatewayJson;
    int32_t retCode;
    CliSvcDescriptorNParams()
        : port(2024), verbose(0), retCode(0),
        runAsDaemon(false)
    {

    }

    std::string toString() const {
        std::stringstream ss;
        ss
            << _("Service: ") << intf << ":" << port << ".\n";
        if (!db.empty())
            ss << _("database file name: ") << db << "\n";
        return ss.str();
    }
};

CliSvcDescriptorNParams cli;

static void done() {
    std::cerr << MSG_GRACEFULLY_STOPPED << std::endl;
    exit(cli.retCode);
}

static void stop() {
    cli.server.stop();
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

    cli.server.setAddress(cli.intf, cli.port);
    cli.server.verbose = cli.verbose;
    cli.retCode = cli.server.run();
    if (cli.retCode)
        std::cerr << ERR_MESSAGE << cli.retCode << ": "
            << appStrError(cli.retCode)
            << std::endl;
}

int main(int argc, char** argv) {
    struct arg_str* a_interface_n_port = arg_str0(nullptr, nullptr, _("ipaddr:remotePort"), _("Default *:2024"));
    struct arg_lit* a_daemonize = arg_lit0("d", "daemonize", _("run daemon"));
    struct arg_str* a_pidfile = arg_str0("p", "pidfile", _("<file>"), _("Check whether a process has created the file pidfile"));
    struct arg_lit* a_verbose = arg_litn("v", "verbose", 0, 2, _("-v - verbose, -vv - debug"));
    struct arg_lit* a_help = arg_lit0("h", "help", _("Show this help"));
    struct arg_end* a_end = arg_end(20);

    void* argtable[] = {
        a_interface_n_port,
        a_verbose, a_daemonize, a_pidfile,
        a_help, a_end
    };

    // verify the argtable[] entries were allocated successfully
    if (arg_nullcheck(argtable) != 0) {
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return ERR_CODE_COMMAND_LINE;
    }
    // Parse the command line as defined by argtable[]
    int nerrors = arg_parse(argc, argv, argtable);

    cli.runAsDaemon = a_daemonize->count > 0;
    if (a_pidfile->count)
        cli.pidfile = *a_pidfile->sval;
    else
        cli.pidfile = "";

    cli.verbose = a_verbose->count;

    if (a_interface_n_port->count) {
        splitAddress(cli.intf, cli.port, std::string(*a_interface_n_port->sval));
    }
    else {
        cli.intf = "*";
        cli.port = 2024;
    }

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
    setSignalHandler();
    if (cli.runAsDaemon) {
        char workDir[PATH_MAX];
        std::string programPath = getcwd(workDir, PATH_MAX);
        if (cli.verbose)
            std::cerr << MSG_LISTENER_DAEMON_RUN
            << "(" << programPath << "/" << programName << "). "
            << MSG_CHECK_SYSLOG << std::endl;
        OPEN_SYSLOG(programName)
        Daemonize daemon(programName, programPath, run, stop, done, 0, cli.pidfile);
        // CLOSESYSLOG()
    }
    else {
        if (cli.verbose > 1)
            std::cerr << cli.toString();
        run();
        done();
    }
}
