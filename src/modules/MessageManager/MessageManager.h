#ifndef _MESSAGEMANAGER_H_
#define _MESSAGEMANAGER_H_

#include "Dragonfly.h"
#include "bit_operations.h"
#include "Debug.h"

#ifdef _UNIX_C
	#include <pthread.h>
	#include <limits.h> //where PIPE_BUF is located
#endif

class CModuleRecord
{
public:
	MODULE_ID   ModuleID;
	UPipe       *pModulePipe;
	short       LoggerStatus;
	short		DaemonStatus;
	
	CModuleRecord( ) {
		Reset();
	}

	void
	Reset( void) {
		ModuleID     = -1;
		pModulePipe  = NULL;
		LoggerStatus = 0;
		DaemonStatus = 0;
	}
};

class CListItem
{
friend class CList;
private:
	CListItem *next;
	CListItem *prev;

public:
	int data;
	int flags;

	CListItem( ) {
		CListItem( 0);
	}
	CListItem( int i) {
		data = i;
		flags = 0;
		next = NULL;
		prev = NULL;
	}
};

class CList
{
private:
	CListItem head;
	CListItem tail;

public:

	CList( ) {
		head.next = &tail;
		head.prev = NULL;
		tail.next = NULL;
		tail.prev = &head;
	}

	~CList( ) {
		CListItem *item = GetFirstItem();
		while( item != NULL) {
			CListItem *next_item = GetNextItem( item);
			delete item;
			item = next_item;
		}
	}

	void
	InsertItemBefore( CListItem *new_item, CListItem *item) {
		new_item->prev = item->prev;
		new_item->next = item;
		new_item->prev->next = new_item;
		item->prev = new_item;
	}

	void
	AppendItem( CListItem *item) {
		InsertItemBefore( item, &tail);
	}

	void
	RemoveItem( CListItem *item) {
		item->prev->next = item->next;
		item->next->prev = item->prev;
		delete item;
	}

	CListItem *
	GetFirstItem( void) {
		if( head.next == &tail) return NULL;
		else return head.next;
	}

	CListItem *
	GetNextItem( CListItem *current_item) {
		if( current_item == NULL) return NULL;
		if( current_item->next == &tail) return NULL;
		else return current_item->next;
	}

	bool
	DoesItemExist( CListItem *search_item) {
		CListItem *item = GetFirstItem();
		while( item != NULL) {			
			if(item->data == search_item->data) 
				return true;
			item = GetNextItem( item);
		}
		return false;
	}
};

#define SUBSCRIBER_FLAG_PAUSE 0x01

class CSubscriberList : protected CList
{
private:

	CListItem *m_CurrentItem;

public:

	CSubscriberList( ) {
		m_CurrentItem = NULL;
	}

	void
	AddSubscriber( MODULE_ID ModuleID) {
		CListItem *subscriber = new CListItem( ModuleID);
		AppendItem( subscriber);
	}

	void
	RemoveSubscriber( MODULE_ID ModuleID) {
		CListItem *current_item = GetFirstItem();
		while( current_item != NULL) {
			if( current_item->data == ModuleID) {
				RemoveItem( current_item);
				break;
			}
			current_item = GetNextItem( current_item);
		}
	}

	void
	PauseSubscriber( MODULE_ID ModuleID) {
		CListItem *current_item = GetFirstItem();
		while( current_item != NULL) {
			if( current_item->data == ModuleID) {
				set_flag_bits( current_item->flags, SUBSCRIBER_FLAG_PAUSE);
				break;
			}
			current_item = GetNextItem( current_item);
		}
	}

	void
	ResumeSubscriber( MODULE_ID ModuleID) {
		CListItem *current_item = GetFirstItem();
		while( current_item != NULL) {
			if( current_item->data == ModuleID) {
				clear_flag_bits( current_item->flags, SUBSCRIBER_FLAG_PAUSE);
				break;
			}
			current_item = GetNextItem( current_item);
		}
	}

	int
	SubscriptionPaused( void) {
		int is_paused = 0;
		if( m_CurrentItem != NULL) {
			if( check_flag_bits( m_CurrentItem->flags, SUBSCRIBER_FLAG_PAUSE)) {
				is_paused = 1;
			}
		}
		return is_paused;
	}

	MODULE_ID
	GetFirstSubscriber( void) {
		m_CurrentItem = GetFirstItem();
		if( m_CurrentItem == NULL) {
			return -1;
		} else {
			return m_CurrentItem->data;
		}
	}

	MODULE_ID
	GetNextSubscriber( void) {
		m_CurrentItem = GetNextItem( m_CurrentItem);
		if( m_CurrentItem == NULL) {
			return -1;
		} else {
			return m_CurrentItem->data;
		}
	}
	
	bool
	IsSubscribed(MODULE_ID ModuleID){
		CListItem subscriber = CListItem( ModuleID);
		return DoesItemExist(&subscriber);
	}

};


class CMessageManager : public UPipeAutoServer
{
public:

	CMessageManager( );
	~CMessageManager( );

	void
	MainLoop( char *cmd_line_options);

private:
	MODULE_ID		m_NextDynamicModIdOffset;
	CModuleRecord   m_ConnectedModules[MAX_MODULES];
	CSubscriberList m_SubscribersToMessageType[MAX_MESSAGE_TYPES];
	CSubscriberList m_EmptySubscriberList;
	MyCString       m_Version;

	MODULE_ID GetDynamicModuleId()
	{
		for(MODULE_ID id=0; id < (MAX_MODULES - DYN_MOD_ID_START); id++)
		{
			MODULE_ID curr_id = m_NextDynamicModIdOffset + DYN_MOD_ID_START;

			// update offset to next available dynamic module ID
			m_NextDynamicModIdOffset++;
			if (m_NextDynamicModIdOffset == MAX_MODULES-DYN_MOD_ID_START)
				m_NextDynamicModIdOffset = 0;

			if (m_ConnectedModules[curr_id].ModuleID != curr_id)
				return curr_id;
		}
		DEBUG_TEXT("CMessageBuffer::GetDynamicModuleId(): All dynamic IDs are in use");
		return 0;
	}
	
	void
	HandleData( UPipe *pClientPipe);
	// Override of abstract base class method. Gets called whenever data is ready on any input pipe.

	void
	HandleDisconnect( UPipe *pClientPipe);
	// Override of virtual base class method to handle unexpected module disconnect cleanly

	void
	ProcessMessage( CMessage *M, UPipe *pSourcePipe);

	void
	DistributeMessage( CMessage *M);
	//Forwards the message to all subscribers and Logger modules, without changing the headers

	void
	DispatchMessage( CMessage *M);
	//Sends the message to all subscribers and Logger modules, headers specifying the message came from MM

	void
	DispatchMessage( CMessage *M, MODULE_ID mod_id);
	//Sends the message only to the specified mod_id and Logger modules, headers specifying the message came from MM

	void
	DispatchSignal( MSG_TYPE sig);
	//Sends the signal to all subscribers and Logger modules, headers specifying the signal came from MM

	void
	DispatchSignal( MSG_TYPE sig, MODULE_ID mod_id);
	//Sends the signal only to the specified mod_id and Logger modules, headers specifying the message came from MM

	void
	DispatchMessageToAll( CMessage *M);
	//Dispatches a message to each connected module (and each Logger module)

	void
	DispatchSignalToAll( MSG_TYPE sig);
	//Dispatches a signal to each connected module (and each Logger module)

	MODULE_ID
	ConnectModule( MODULE_ID module_id, UPipe *pSourcePipe, short logger_status, short daemon_status);

	void
	DisconnectModule( MODULE_ID module_id);

	void
	CleanUpModuleRecord( MODULE_ID module_id);

	void
	ShutdownModule(  MODULE_ID mod_id);

	void
	ShutdownAllModules( int shutdown_Dragonfly=1, int shutdown_daemons=1);

	void
	ShutdownStatusModule( void);

	void 
	ShutdownLoggerModule( void);

	void
	AddSubscription( MODULE_ID module_id, MSG_TYPE message_type);

	void
	RemoveSubscription( MODULE_ID module_id, MSG_TYPE message_type);
	
	void
	PauseSubscription( MODULE_ID mod_id, MSG_TYPE msg_type_to_pause);

	void
	ResumeSubscription( MODULE_ID mod_id, MSG_TYPE msg_type_to_resume);

	CSubscriberList *
	GetSubscriberList( MSG_TYPE message_type);

	bool
	IsModuleSubscribed( MODULE_ID mod_id, MSG_TYPE message_type);

	void
	SendAcknowledge( MODULE_ID mod_id);
	
	int
	SendMessage( CMessage *m, MODULE_ID mod_id);
	
	int
	SendSignal( MSG_TYPE sig, MODULE_ID dest_mod_id);
	//dest_mod_id= module id that should be put in dest_mod_id in message header

	int
	ForwardMessage( CMessage *m, MODULE_ID mod_id);

	int
	ModuleIsConnected( MODULE_ID mod_id);

	UPipe*
	GetModPipe( MODULE_ID mod_id);

	int
	IsDaemon( MODULE_ID mod_id);

	void
	ReportLoad( void);
	//sends an MT_MM_LOAD_REPORT message with the load data of all connected modules

	void
	AskUsageReport( void);
	//sends MT_MOD_REPORT_USAGE to all connected modules

	void
	ReportMMStatus( void);
	//sends MT_MM_STATUS_REPORT message

	void
	LogFailedMessage( CMessage *m, MODULE_ID mod_id);
	//sends a message to logger modules indicating that a message failed to be forwarded to one of the modules
};


#endif //_MESSAGEMANAGER_H_
