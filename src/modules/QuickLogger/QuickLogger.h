/*
 *  QuickLogger - Logs Dragonfly messages in an attempt to save data for the experiment
 *
 *  Meel Velliste 10/28/2008
 *  Emrah Diril  05/04/2012
 */

/* ----------------------------------------------------------------------------
   |                              INCLUDES                                    |
   ----------------------------------------------------------------------------*/
#include "Dragonfly.h"
#include "MessageBufferer.h"

/* ----------------------------------------------------------------------------
   |                       GENERAL DEFINES                                    |
   ----------------------------------------------------------------------------*/
// Default size of message buffers to pre-allocate
// (can be overridden by command-line arguments)
#define QL_NUM_PREALLOC_MESSAGES   100000   // 100,000 messages
#define QL_NUM_PREALLOC_DATABYTES  40000000 // 40 MB of data, should be enough for a few minutes

/* ----------------------------------------------------------------------------
   |                       QUICK Logger MODULE CODE                             |
   ----------------------------------------------------------------------------*/

class CQuickLogger : public Dragonfly_Module
{
private:
	CMessageBufferer _MessageBufrr;
	bool _logging;
public:
	CQuickLogger();
	~CQuickLogger();

	void MainFunction( char *cmd_line_options);

private:
	void Status(const MyCString& msg);
};
