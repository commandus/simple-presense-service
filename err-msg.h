#ifndef ERR_MSG_H_
#define ERR_MSG_H_	1

#define ERR_MESSAGE						"Error "
#define ERR_DEBUG						"Info "
#define ERR_INFO						"Info "
#define ERR_WARNING						"Warning "

#define CODE_OK							0
#define ERR_CODE_COMMAND_LINE		    (-5000)
#define ERR_CODE_SOCKET_BIND		    (-5014)
#define ERR_CODE_SOCKET_LISTEN		    (-5022)
#define ERR_CODE_STOPPED                (-5175)

#define MSG_GRACEFULLY_STOPPED			"Stopped gracefully"
#define MSG_INTERRUPTED 				"Interrupted "
#define MSG_LISTENER_DAEMON_RUN         "Listener daemon run .."
#define MSG_CHECK_SYSLOG 	            "Check syslog."

// Error en-us description
#define ERR_COMMAND_LINE        		"Wrong parameter(s)"
#define ERR_WSA_STARTUP					"WSA startup error"

const char *appStrError(int code);

#endif
