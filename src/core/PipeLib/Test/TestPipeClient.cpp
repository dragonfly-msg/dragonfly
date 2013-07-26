#include "TestDefs.h"

char gDefaultServerName[] = "localhost:7111";
//char gDefaultServerName[] = "BlackIronPipe";

class ClientTest
{
private:
	static const int NUM_CLIENTS = 10;
	UPipeClient *_clients[NUM_CLIENTS];
	UPipe *_pipes[NUM_CLIENTS];
public:
	ClientTest() {
		for( int i = 0; i < NUM_CLIENTS; i++) {
			_clients[i] = NULL;
			_pipes[i] = NULL;
		}
	}

	~ClientTest() {
		for( int i = 0; i < NUM_CLIENTS; i++) {
			if( _clients[i] != NULL) _clients[i]->Disconnect();
		}
	}

	void TestConnectingToServer() {
		for( int i = 0; i < NUM_CLIENTS; i++) {
			std::cout << "Connecting to server <" << gDefaultServerName << ">" << std::endl;
			_clients[i] = UPipeFactory::CreateClient( gDefaultServerName);
			_pipes[i] = _clients[i]->Connect();
		}
	}

	void TestDisconnectingFromServer() {
		for( int i = 0; i < NUM_CLIENTS; i++) {
			_clients[i]->Disconnect();
		}
	}

	void TestReading() {
		for( int i = 0; i < NUM_CLIENTS; i++) {
			int value;
			int nbytes = sizeof(value);
			void *buffer = (void*) &value;
			std::cout << "Reading from pipe for client #" << i << "... ";
			_pipes[i]->Read( buffer, nbytes);
			std::cout << "value = " << value << std::endl;
		}
	}

	void TestWriting() {
		for( int i = 0; i < NUM_CLIENTS; i++) {
			int value = 44;
			int nbytes = sizeof(value);
			void *buffer = (void*) &value;
			std::cout << "Writing to pipe for client #" << i << ", value = " << value << "... ";
			_pipes[i]->Write( buffer, nbytes);
			std::cout << "Written!" << std::endl;
		}
	}

	void TestReadingOneByOne() {
		const int ARRAY_LENGTH = 1000000;
		int *value = new int[ARRAY_LENGTH];
		int nbytes = ARRAY_LENGTH * sizeof(value[0]);
		void *buffer = (void*) &value[0];
		while( true) {
			//std::cout << "Press ENTER to read value... ";
			//getchar();
			_pipes[0]->Read( buffer, nbytes);
			std::cout << "read value = " << value[0] << std::endl;
		}
	}
};

// A simple test module that connects to the MiniMessageManager server and
// exchanges some data for test purposes
class MiniModule
{
private:
	UPipeClient *_client;
	UPipe *_pipe;
public:
	MiniModule() {
		_client = NULL;
		_pipe = NULL;
	}
	~MiniModule() {
		if( _client != NULL) {
			_client->Disconnect();
			delete _client;
		}
		std::cout << "Disconnected" << std::endl;
	}

	void Run( void) {
		// Connect to server
		std::cout << "Connecting to server " << gDefaultServerName << "... ";
		_client = UPipeFactory::CreateClient( gDefaultServerName);
		_pipe = _client->Connect();
		std::cout << " CONNECTED!" << std::endl;

		while( true) {
			try {
				// Loop forever, exchanging data periodically
				Communicate();
			} catch( UPipeClosedException) {
				// If the connection to the server fails, try to re-connect
				std::cout << "Server went down! ";
				_client->Disconnect();
			}
			ReConnect();
		}
	}

	// Communicate - i.e. keep sending and receiving test data
	void Communicate( void) {
		int random_value = UPipeServer::RandomIndex(10);
		while( true) {
			Data data;
			void *buffer = (void*) &data;
			int nbytes = sizeof(data);
			// Write data to server
			data.a = 1;
			data.b = random_value;
			data.x = 3.14;
			_pipe->Write( buffer, nbytes);
			std::cout << "Send data: a = " << data.a << ", b = " << data.b << ", x = " << data.x << std::endl;
			// Read response from server
			_pipe->Read( buffer, nbytes);
			std::cout << "Received response: a = " << data.a << ", b = " << data.b << ", x = " << data.x << std::endl;
			Sleep( 1000);
		}
	}

	// Keeps trying to reconnect to the server
	void ReConnect( void) {
		std::cout << "Trying to re-connect to server...";
		bool keep_trying_to_connect = true;
		while( keep_trying_to_connect) {
			try {
				_pipe = _client->Connect();
				std::cout << " CONNECTED!" << std::endl;
				keep_trying_to_connect = false;
			} catch( UPipeException) {
				std::cout << ".";
				Sleep( 500);
			}
		}
	}

};

class AutoClientTest : public UPipeAutoClient
{
public:
	void Main( UPipe *pServerPipe) {
		int random_value = UPipeServer::RandomIndex(10);
		while( true) {
			Data data;
			void *buffer = (void*) &data;
			int nbytes = sizeof(data);
			// Write data to server
			data.a = 1;
			data.b = random_value;
			data.x = 3.14;
			pServerPipe->Write( buffer, nbytes);
			std::cout << "Send data: a = " << data.a << ", b = " << data.b << ", x = " << data.x << std::endl;
			// Read response from server
			pServerPipe->Read( buffer, nbytes);
			std::cout << "Received response: a = " << data.a << ", b = " << data.b << ", x = " << data.x << std::endl;
			Sleep( 1000);
		}
	}
};

int main( int argc, char *argv[])
{
	try
	{
		//ClientTest T;
		//T.TestConnectingToServer();
		//T.TestReadingOneByOne();
		//T.TestReading();
		//Sleep(1000);
		//T.TestWriting();
		//T.TestDisconnectingFromServer();

		//MiniModule MM;
		//MM.Run();

		AutoClientTest ACT;
		if( argc > 1) {
			ACT.Run( argv[1]); // Use command-line specified host address if available
		} else {
			ACT.Run( gDefaultServerName); // Otherwise use default server address
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
