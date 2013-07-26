//#ifndef _DRAGONFLY_TYPES_H_
//#define _DRAGONFLY_TYPES_H_

// Types used in the Dragonfly system

typedef short MODULE_ID;
typedef short HOST_ID;
typedef int MSG_TYPE;
typedef int MSG_COUNT;

// Maximums for the entire Dragonfly system
#define MAX_MODULES       200  //maximal number of modules in the system
#define DYN_MOD_ID_START  100  //module ID where pool of dynamic IDs begin
#define MAX_HOSTS         5    //maximal number of hosts in the system
#define MAX_MESSAGE_TYPES 10000 //maximal number of message types in the system
#define MIN_STREAM_TYPE    9000 //minimal number type for a data stream
#define MAX_TIMERS        100  //maximal number of total timers TimerModule can handle
#define MAX_INTERNAL_TIMERS 20 //maximal number of internal timers that can be set by an Dragonfly module

// Maximums for core modules
#define MAX_DRAGONFLY_MSG_TYPE  99 // Message types below 100 are reserved for Dragonfly core
#define MAX_DRAGONFLY_MODULE_ID  9 // Module ID-s below 10 are reserved for Dragonfly core

// Header fields for all messages passed through Dragonfly
#define DF_MSG_HEADER_FIELDS \
	MSG_TYPE	msg_type; \
	MSG_COUNT	msg_count; \
	double	send_time; \
	double	recv_time; \
	HOST_ID		src_host_id; \
	MODULE_ID	src_mod_id; \
	HOST_ID		dest_host_id; \
	MODULE_ID	dest_mod_id; \
	int			num_data_bytes; \
	int			remaining_bytes; \
	int			is_dynamic; \
	int			reserved
// msg_type - Message type ID
// msg_count - Source message count (per source, starting from 1)
// send_time - Time at source when message sent (Seconds since 0:00:00 on Jan 1, 1970)
// recv_time - Time at final destination when message received (Seconds since 0:00:00 on Jan 1, 1970)
// src_host_id - ID number of the host computer where the message originates
// src_mod_id - ID number of the program module where the message originates
// dest_host_id - ID number of the host computer intended to receive this message
// dest_mod_id - ID number of the module ID intended to receive this message
// num_data_bytes - Number of data bytes following this header
// remaining_bytes - If message data is too large to fit in one message, then this value is non-zero
//                   and represents the number of data bytes still to follow in subsequent messages
// is_dynamic - if true, then message data is a serialized form of Dynamic Data.
// reserved - for 64-bit alignment (if you ever change the fields, remember they have to be
//	          16-, 32- and 64-bit aligned, i.e. no single field crosses a 2-, 4-, or 8-byte boundary)

// Header for messages passed through Dragonfly
typedef struct {
	DF_MSG_HEADER_FIELDS;
} DF_MSG_HEADER;

#define MAX_CONTIGUOUS_MESSAGE_DATA 9000

typedef struct {
	DF_MSG_HEADER_FIELDS;
	char data[MAX_CONTIGUOUS_MESSAGE_DATA];
} DF_MESSAGE;


// Module ID-s of core modules
#define MID_MESSAGE_MANAGER     0
#define MID_COMMAND_MODULE      1
#define MID_APPLICATION_MODULE  2
#define MID_NETWORK_RELAY       3
#define MID_STATUS_MODULE       4
#define MID_QUICKLOGGER         5

#define HID_LOCAL_HOST  0
#define HID_ALL_HOSTS   0x7FFF

typedef char STRING_DATA[];   //message data type for variable length string messages
//typedef char* STRING_DATA;   //message data type for variable length string messages

// Used for subscribing to all message types
#define ALL_MESSAGE_TYPES  0x7FFFFFFF

// Messages sent by MessageManager to modules
#define MT_EXIT            0
#define MT_KILL			   1
#define MT_ACKNOWLEDGE     2
#define MT_FAIL_SUBSCRIBE  6
typedef struct { MODULE_ID mod_id; short reserved; MSG_TYPE msg_type;} MDF_FAIL_SUBSCRIBE;
#define MT_FAILED_MESSAGE  7 // Sent by MM when it cannot forward a message to a module
typedef struct { 
	MODULE_ID dest_mod_id;
	short reserved[3];
	double time_of_failure;
	DF_MSG_HEADER msg_header;
} MDF_FAILED_MESSAGE;
#define MT_MM_ERROR			83
typedef STRING_DATA MDF_MM_ERROR;
#define MT_MM_INFO		    84
typedef STRING_DATA MDF_MM_INFO;

// Messages sent by modules to MessageManager
#define MT_CONNECT         13
typedef struct { short logger_status; short daemon_status;} MDF_CONNECT;
#define MT_DISCONNECT     14

// Subscription messages
#define MT_SUBSCRIBE      15
#define MT_UNSUBSCRIBE    16
#define MT_PAUSE_SUBSCRIPTION	85
#define MT_RESUME_SUBSCRIPTION	86
typedef MSG_TYPE MDF_SUBSCRIBE;
typedef MSG_TYPE MDF_UNSUBSCRIBE;
typedef MSG_TYPE MDF_PAUSE_SUBSCRIPTION;
typedef MSG_TYPE MDF_RESUME_SUBSCRIPTION;

#define MT_SHUTDOWN_DRAGONFLY  17
#define MT_SHUTDOWN_APP   18
#define MT_FORCE_DISCONNECT 82
typedef struct { int mod_id; } MDF_FORCE_DISCONNECT;


//Messages sent by all core modules
#define MT_CORE_MODULE_REINIT_ACK 25

//Messages sent by all modules
#define MT_MODULE_READY   26
typedef struct { int pid; } MDF_MODULE_READY;
#define MT_DYNAMIC_DD_READ_ERR 90
typedef STRING_DATA MDF_DYNAMIC_DD_READ_ERR;
#define MT_DEBUG_TEXT     91
typedef STRING_DATA MDF_DEBUG_TEXT;

// Messages sent to ApplicationModule
#define MT_AM_EXIT        30
#define MT_START_APP      31
typedef STRING_DATA MDF_START_APP;
#define MT_STOP_APP       32
typedef STRING_DATA MDF_STOP_APP;
#define MT_RESTART_APP    33
typedef STRING_DATA MDF_RESTART_APP;
#define MT_KILL_APP       34
typedef STRING_DATA MDF_KILL_APP;
#define MT_AM_RE_READ_CONFIG_FILE 89
#define MT_AM_GET_APP_NAME        92
	//sent by MASTER AM to SLAVE AMs
#define MT_SLAVE_START_APP 64		
typedef STRING_DATA MDF_SLAVE_START_APP;
#define MT_SLAVE_START_APP_ACK 65
typedef struct {int num_remote_hosts; } MDF_SLAVE_START_APP_ACK;
#define MT_SLAVE_STOP_APP      66
typedef STRING_DATA MDF_SLAVE_STOP_APP;
#define MT_SLAVE_KILL_APP      67
typedef STRING_DATA MDF_SLAVE_KILL_APP;
#define MT_SLAVE_RESTART_APP   68
typedef STRING_DATA MDF_SLAVE_RESTART_APP;

//Messages sent by ApplicationModule
#define MT_AM_ERROR       35 //sends out if a module started not by application manager
typedef STRING_DATA MDF_AM_ERROR;
#define MT_AM_ACKNOWLEDGE 36 //sent as an immediate reply to MT_START_APP, MT_STOP_APP, MT_KILL_APP
#define MT_FAIL_START_APP 37 //sent if got MT_START_APP and failed to start ANY module
typedef STRING_DATA MDF_FAIL_START_APP;
#define MT_FAIL_STOP_APP  38 //sent if got MT_STOP_APP  and failed to stop  ANY module
typedef STRING_DATA MDF_FAIL_STOP_APP;
#define MT_FAIL_KILL_APP  39 //sent if got MT_KILL_APP  and failed to kill  ANY module
typedef STRING_DATA MDF_FAIL_KILL_APP;
#define MT_APP_START_COMPLETE    40 //a final reply to MT_START_APP    when done starting application
#define MT_APP_SHUTODWN_COMPLETE 41 //a final reply to MT_STOP_APP     when done with application shutdown
#define MT_APP_RESTART_COMPLETE  42 //a final reply to MT_RESTART_APP  when done with application restart
#define MT_APP_KILL_COMPLETE     43 //a final reply to MT_KILL_APP  when done with killing application
#define MT_ALL_MODULES_READY     44 //sent if got MT_MODULE_READY from all modules
#define MT_CORE_MODULE_REINIT    45 //sent after an application is shutdown/killed - to indicate core modules that they have to reinitialize
#define MT_AM_CONFIG_FILE_DATA   46 //data about what was read from the AM config file
typedef STRING_DATA MDF_AM_CONFIG_FILE_DATA;
#define MT_AM_APP_NAME           93
typedef char MDF_AM_APP_NAME[];

//sent by SLAVE AMs
#define MT_SLAVE_ALL_MODULES_READY	69
#define MT_SLAVE_FAIL_START_APP		70
typedef STRING_DATA MDF_SLAVE_FAIL_START_APP;
#define MT_SLAVE_FAIL_STOP_APP		71
#define MT_SLAVE_FAIL_KILL_APP      72
#define MT_SLAVE_APP_SHUTODWN_COMPLETE	74 
#define MT_SLAVE_APP_RESTART_COMPLETE	75 
#define MT_SLAVE_APP_KILL_COMPLETE		76
#define MT_SLAVE_AM_ERROR               77
typedef STRING_DATA MDF_SLAVE_AM_ERROR;


// Error in Module
#define MT_APP_ERROR      47
typedef STRING_DATA MDF_APP_ERROR;

//Messages sent to StatusModule
#define MT_SM_EXIT        48

//Messages sent by TimerModule
#define MT_CLOCK_SYNC     49
#define MT_TIMER_EXPIRED  50
typedef struct { int timer_id; } MDF_TIMER_EXPIRED;
#define MT_TIMED_OUT      73
typedef MDF_TIMER_EXPIRED MDF_TIMED_OUT;
#define MT_SET_TIMER_FAILED 51
typedef struct { MODULE_ID mod_id; int timer_id; int snooze_time; } MDF_SET_TIMER_FAILED;

//typedef struct { int a; int b; double x; } MDF_TEST_DATA;

//Messages sent to TimerModule
#define MT_TM_EXIT        52
#define MT_SET_TIMER      53
typedef struct { int timer_id; int snooze_time; } MDF_SET_TIMER;

#define MT_CANCEL_TIMER   54
typedef struct { int timer_id; } MDF_CANCEL_TIMER;

//Messages sent to LoggerModule
#define MT_LM_EXIT        55

// Messages sent by core modules when they have finished initializing and are ready to serve
#define MT_MM_READY             94
#define MT_LM_READY             96

// Messages for QuickLogger Module
// MT_SAVE_MESSAGE_LOG - Tells QuickLogger to dump its current message buffer
// to the named file
#define MT_SAVE_MESSAGE_LOG        56
#define MAX_LOGGER_FILENAME_LENGTH    256
typedef struct {
    char pathname[MAX_LOGGER_FILENAME_LENGTH];  // File path name where to save data
    int pathname_length;                 // Number of characters in path name
} MDF_SAVE_MESSAGE_LOG;
// Response to MT_MESSAGE_LOG_SAVED after QuickLogger has saved the file
#define MT_MESSAGE_LOG_SAVED       57
// MT_PAUSE_MESSAGE_LOGGING - Tells QuickLogger to not put incoming messages
// in the log until further notice
#define MT_PAUSE_MESSAGE_LOGGING   58
#define MT_RESUME_MESSAGE_LOGGING  59
#define MT_RESET_MESSAGE_LOG       60
#define MT_DUMP_MESSAGE_LOG        61

#define MT_TIMING_TEST 62
#define MAX_TIMING_TEST_TIME_POINTS 12
typedef struct {
  double time[MAX_TIMING_TEST_TIME_POINTS];
} MDF_TIMING_TEST;


//#endif //_DRAGONFLY_TYPES_H_
