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
#include "err-msg.h"
#include "log.h"
#include "daemonize.h"
#include "ip-address.h"
#include "uv-client.h"

// i18n
// #include <libintl.h>
// #define _(String) gettext (String)
#define _(String) (String)

const char* programName = "simple-presence-client";

// global parameters and descriptors
class ClientDescriptorNParams {
public:
    UVClient client;
    std::string remoteAddress;
    uint16_t remotePort;
    bool runAsDaemon;
    std::string pidfile;
    int verbose;
    int32_t retCode;
    ClientDescriptorNParams()
        : remotePort(2024), verbose(0), retCode(0),
          runAsDaemon(false)
    {

    }

    std::string toString() const {
        std::stringstream ss;
        ss
            << _("UUID: ") << client.uid.toString() << " "
            << _("Service: ") << remoteAddress << ":" << remotePort << ".\n";
        return ss.str();
    }
};

ClientDescriptorNParams cli;

static void done() {
    std::cerr << MSG_GRACEFULLY_STOPPED << std::endl;
    exit(cli.retCode);
}

static void stop() {
    cli.client.stop();
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
    cli.client.setAddress("*", cli.remotePort + 1);
    cli.client.setRemoteAddress(cli.remoteAddress, cli.remotePort);
    cli.client.verbose = cli.verbose;

    cli.retCode = cli.client.run();
    if (cli.retCode)
        std::cerr << ERR_MESSAGE << cli.retCode << ": "
                  << appStrError(cli.retCode)
                  << std::endl;
}

int main(int argc, char** argv) {
    struct arg_str* a_uid = arg_str0(nullptr, nullptr, _("UUID"), _("cat /proc/sys/kernel/random/uuid"));
    struct arg_str* a_interface_n_port = arg_str0("d", "dest", _("ipaddr:remotePort"), _("Default 127.0.0.1:202"));
    struct arg_lit* a_daemonize = arg_lit0("d", "daemonize", _("run daemon"));
    struct arg_str* a_pidfile = arg_str0("p", "pidfile", _("<file>"), _("Check whether a process has created the file pidfile"));
    struct arg_lit* a_verbose = arg_litn("v", "verbose", 0, 2, _("-v - verbose, -vv - debug"));
    struct arg_lit* a_help = arg_lit0("h", "help", _("Show this help"));
    struct arg_end* a_end = arg_end(20);

    void* argtable[] = {
        a_interface_n_port,
        a_uid,
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
        splitAddress(cli.remoteAddress, cli.remotePort, std::string(*a_interface_n_port->sval));
    }
    else {
        cli.remoteAddress = "127.0.0.1";
        cli.remotePort = 2024;
    }

    if (a_uid->count)
        cli.client.uid = *a_uid->sval;
    else
        cli.client.uid.generateRandom();

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
        setSignalHandler();
        if (cli.verbose > 1)
            std::cerr << cli.toString();
        run();
        done();
    }
}
