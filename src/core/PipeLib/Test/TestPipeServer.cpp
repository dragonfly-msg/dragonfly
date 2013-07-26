#include "TestDefs.h"

char gDefaultServerName[] = ":7111";
//char gDefaultServerName[] = "localhost:7111";
//char gDefaultServerName[] = "BlackIronPipe";

void TestServerNameParsing( void);

class ServerTest
{
private:
	static const int MAX_CLIENTS = 10;
	UPipeServer *_server;
	UPipe *_pipes[MAX_CLIENTS];
	int _numClients;
public:
	ServerTest() {
		_server = NULL;
		_numClients = 0;
	}
	void TestOneShotAccept() {
		// Open listening pipe
		_server = UPipeFactory::CreateServer( gDefaultServerName);
		// Wait for clients
		int NumClients = 10;
		for( int i = 0; i < NumClients; i++) {
			_pipes[i] = _server->AcceptClient();
		}
	}

	void TestDisconnectingClients() {
		_server->DisconnectAllClients();
	}

	void TestWriting() {
		int num_clients = _server->GetNumClients();
		for( int i = 0; i < num_clients; i++) {
			int value = i+10;
			int nbytes = sizeof(value);
			void *buffer = (void*) &value;
			std::cout << "Writing to pipe for client #" << i << ", value = " << value << "... ";
			_pipes[i]->Write( buffer, nbytes);
			std::cout << "Written!" << std::endl;
		}
	}

	void TestReading() {
		int num_clients = _server->GetNumClients();
		for( int i = 0; i < num_clients; i++) {
			int value;
			int nbytes = sizeof(value);
			void *buffer = (void*) &value;
			std::cout << "Reading from pipe for client #" << i << "... ";
			_pipes[i]->Read( buffer, nbytes);
			std::cout << "value = " << value << std::endl;
		}
	}

	void TestOneShotServer() {
		TestOneShotAccept();
		TestWriting();
		//TestReading();
		//Sleep(1000);
		//TestWritingALot();
		//TestMultiRead_InOrder();
		TestMultiRead_InRandomOrder();
		TestDisconnectingClients();
	}

	void TestInfiniteServer() {
		// Open listening pipe
		_server = UPipeFactory::CreateServer( gDefaultServerName);
		// Wait for clients
		while( true) {
			std::cout << "Waiting for client... ";
			UPipe *p = _server->AcceptClient();
			std::cout << "Accepted client, ";
			int value = 33;
			int nbytes = sizeof(value);
			void *buffer = (void*) &value;
			std::cout << "writing value " << value << "... ";
			p->Write( buffer, nbytes);
			std::cout << "Written!" << std::endl;
			std::cout << "Waiting for response... ";
			p->Read( buffer, nbytes);
			std::cout << "Received response " << value << std::endl;
			_server->DisconnectClient( p);
		}
	}

	void TestMultiRead_InOrder( void) {
		int num_clients = _server->GetNumClients();
		while( num_clients > 0) {
			std::cout << "Waiting for data from any clients... ";
			UPipe *p[UPipeServer::MAX_CLIENTS];
			int num_clients_that_have_data = _server->WaitForData( p, -1);
			std::cout << "Got data from " << num_clients_that_have_data << " clients" << std::endl;
			for( int i = 0; i < num_clients_that_have_data; i++) {
				std::cout << "Reading data from client #" << i << " (" << p[i] << ")... ";
				try {
					int value;
					int nbytes = sizeof(value);
					void *buffer = (void*) &value;
					p[i]->Read( buffer, nbytes);
					std::cout << "got value " << value << std::endl;
				} catch( UPipeClosedException) {
					std::cout << "client pipe has been closed" << std::endl;
					_server->DisconnectClient( p[i]);
					num_clients = _server->GetNumClients();
				}
			}
		}
	}

	void TestMultiRead_InRandomOrder( void) {
		int num_clients = _server->GetNumClients();
		while( num_clients > 0) {
			std::cout << "Waiting for data from any clients... "; std::flush(std::cout);
			UPipe *p[UPipeServer::MAX_CLIENTS];
			int num_clients_that_have_data = _server->WaitForData( p, -1);
			std::cout << "Got data from " << num_clients_that_have_data << " clients" << std::endl;
			int random_order[UPipeServer::MAX_CLIENTS];
			UPipeServer::RandomIndices( num_clients_that_have_data, random_order);
			for( int n = 0; n < num_clients_that_have_data; n++) {
				int i = random_order[n];
				std::cout << "Reading data from client #" << i << " (" << p[i] << ")... ";
				try {
					int value;
					int nbytes = sizeof(value);
					void *buffer = (void*) &value;
					p[i]->Read( buffer, nbytes);
					std::cout << "got value " << value << std::endl;
				} catch( UPipeClosedException) {
					std::cout << "client pipe has been closed" << std::endl;
					_server->DisconnectClient( p[i]);
					num_clients = _server->GetNumClients();
				}
			}
		}
	}

	void TestWritingALot() {
		const int ARRAY_LENGTH = 25000;
		int *value = new int[ARRAY_LENGTH];
		value[0] = 47;
		int nbytes = ARRAY_LENGTH * sizeof(value[0]);
		void *buffer = (void*) &value[0];
		int totalbytes = 0;
		while( true) {
			std::cout << "Writing to pipe for client #0, value = " << value[0] << "... ";
			std::flush(std::cout);
			//Sleep(100);
			_pipes[0]->Write( buffer, nbytes);
			totalbytes += nbytes;
			std::cout << totalbytes << " bytes written!" << std::endl;
			//std::cout << "Value = " << value[0] << ", " << totalbytes << " bytes written" << std::endl;
			value[0]++;
		}
		delete[] value;
	}

	~ServerTest() {
		if( _server != NULL) {
			delete _server;
		}
	}
};

// Implements a simple server that allows clients to connect/disconnect whenever they want
// and exchanges some simple data with the clients
class MiniMessageManager
{
private:
	UPipeServer *_server;
public:
	MiniMessageManager() {
		_server = NULL;
	}
	~MiniMessageManager() {
		if( _server != NULL) delete _server;
	}

	void Run( void) {
		// Open listening pipe
		std::cout << "Creating server " << gDefaultServerName << "... ";
		_server = UPipeFactory::CreateServer( gDefaultServerName);
		std::cout << "CREATED!" << std::endl;

		// Loop infinitely, waiting for data and clients
		while( true) {

			// Wait for data or a new client
			std::cout << "Waiting for clients or data" << std::endl;
			UPipe *pipes[UPipeServer::MAX_CLIENTS];
			bool connection_request = true;
			int num_ready = _server->WaitForDataAndClients( pipes, -1, &connection_request);
			if( num_ready > 0) std::cout << "Received data from " << num_ready << " clients" << std::endl;
			if( connection_request) std::cout << "Received connection request" << std::endl;
			
			// Handle the data (in random order if we got data from more than one client at once)
			int random_order[UPipeServer::MAX_CLIENTS];
			UPipeServer::RandomIndices( num_ready, random_order);
			for( int i = 0; i < num_ready; i++) {
				int r = random_order[i];
				UPipe *p = pipes[r];
				std::cout << "Handling data from client #" << r << " (" << p << ")" << std::endl;
				try {
					HandleData( p);
				} catch( UPipeClosedException) {
					std::cout << "Client pipe has been closed" << std::endl;
					_server->DisconnectClient( p);
				}
			}

			// Accept new client, if any
			if( connection_request) {
				UPipe *client_pipe = _server->AcceptClient();
				std::cout << "Accepted client " << client_pipe << std::endl;
			}
		}
	}

	void HandleData( UPipe *p) {
		Data data;
		void *buffer = (void*) &data;
		int nbytes = sizeof(data);
		// Read data from client
		p->Read( buffer, nbytes);
		std::cout << "Received data from " << p << ", a = " << data.a << ", b = " << data.b << ", x = " << data.x << std::endl;
		// Send response to client
		double multiplier = (double) data.b;
		data.a = _server->GetNumClients();
		data.b = 47;
		data.x = multiplier * data.x;
		p->Write( buffer, nbytes);
		std::cout << "Sent response: a = " << data.a << ", b = " << data.b << ", x = " << data.x << std::endl;
	}
};


class AutoServerTest : public UPipeAutoServer
{
public:
	void HandleData( UPipe *pClientPipe) {
		Data data;
		void *buffer = (void*) &data;
		int nbytes = sizeof(data);
		// Read data from client
		pClientPipe->Read( buffer, nbytes);
		std::cout << "Received data from " << pClientPipe << ", a = " << data.a << ", b = " << data.b << ", x = " << data.x << std::endl;
		// Send response to client
		double multiplier = (double) data.b;
		data.a = _server->GetNumClients();
		data.b = 47;
		data.x = multiplier * data.x;
		pClientPipe->Write( buffer, nbytes);
		std::cout << "Sent response: a = " << data.a << ", b = " << data.b << ", x = " << data.x << std::endl;
	}
};

int main( int argc, char *argv[])
{
	try
	{
		//TestServerNameParsing();

		//ServerTest T;
		//T.TestOneShotServer();
		//T.TestInfiniteServer();
		//T.TestWritingALot();

		//MiniMessageManager MMM;
		//MMM.Run();

		AutoServerTest AST;
		if( argc > 1) {
			AST.Run( argv[1]); // Use command-line specified host address if available
		} else {
			AST.Run( gDefaultServerName); // Otherwise use default server address
		}

		//std::cout << "Press any key to quit!" << std::endl;
		//getchar();
	}
	catch( UPipeClosedException &e)
	{
		MyCString s;
		e.AppendTraceToString( s);
		std::cout << "UPipeClosedException: " << s.GetContent() << std::endl;
	}
	catch( UPipeException &e)
	{
		MyCString s;
		e.AppendTraceToString( s);
		std::cout << "UPipeException: " << s.GetContent() << std::endl;
	}
	catch( MyCException &e)
	{
		MyCString s;
		e.AppendTraceToString( s);
		std::cout << "MyCException: " << s.GetContent() << std::endl;
	}
	catch(...)
	{
		MyCString s;
		std::cout << "Unknown Exception!" << std::endl;
	}
}

void TestServerNameParsing( void)
{
	// To run this test, the private members of UPipeFactory need to be made public, then
	// the code below can be uncommented and compiled

	//char ServerName[1000];
	//short PortNo;
	//UPipeFactory::ServerType t;

	//t = UPipeFactory::ParseServerAddress( gServerName, ServerName, &PortNo);
	//printf("ServerAddress = <%s>, ServerName = <%s>, port no. = %i\n", gServerName, ServerName, PortNo);
	//switch( t) {
	//	case UPipeFactory::NamedPipe: printf("Named Pipe\n"); break;
	//	case UPipeFactory::Socket: printf("Socket\n"); break;
	//	default: printf("Hmmmm...\n");
	//}
}
