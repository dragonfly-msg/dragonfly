#include "NamedPipe.h"

/*
 *	Useful for sockets only: allows to re-accept on a listening socket to handle another client connection
 *  Returns a PIPE_HANDLE that includes the accepted socket that can be used to communicate with the client
 */
//PIPE_HANDLE AcceptAnotherClient(PIPE_HANDLE listening_handle)
//{
//	char errorInfo[ERROR_TXT_LEN];
//	PIPE_HANDLE handle;
//
//	if(listening_handle->type != SOCKET_PIPE)
//		return NULL;
//
//	handle = (PIPE_HANDLE) malloc(sizeof(PIPE_HANDLE_STRUCT));
//	if(handle == NULL){
//		sprintf(errorInfo,"AcceptAnotherClient: Could not allocate pipe handle- Malloc error\n");
//		ReportPipeError(PIPE_NO_ERROR_CODE, errorInfo);
//		return NULL;
//	}
//	
//	handle->type = listening_handle->type;
//
//	if(listening_handle->listening_socket_open)
//	{
//		handle->socket = WaitForClient(listening_handle->listening_socket, PIPE_DISABLE_NAGLE);
//		//check if WaitForClient returned NULL because of a socket error
//		if(handle->socket == NULL)
//		{
//			free(handle);
//			handle = NULL;
//		}
//	}else{
//		sprintf(errorInfo,"AcceptAnotherClient: Got a handle with listening socket NOT open\n");
//		ReportPipeError(PIPE_NO_ERROR_CODE, errorInfo);
//		free(handle);
//		return NULL;
//	}
//
//	return handle;
//}

/*
 * Opens a client pipe
 *
 * char* pipe_descriptor: should be of the form: "PipeName" for named pipes, or "host:port" for sockets
 * char read_write      : should be either "r" or "w" to indicate if the pipe is for reading or writing (meaningless for sockets)
 * 
 * Returns:
 *		If NAMED_PIPE : Will open the existing pipe and return a handle
 *		If SOCKET_PIPE: Will create a TCP connection to the specified host name, and return a socket to it
 *		On failure    : NULL
 *
 * Note: if you are a WINDOWS pipe server - do not overwrite your original HANDLE with the one returned from this function. Just free the returned handle.
 * 
 */
//#ifdef _WINDOWS_C
//PIPE_HANDLE OpenPipe( const char* pipe_descriptor,  const char* read_write){
//#else
//PIPE_HANDLE OpenPipe( char* pipe_descriptor,  char* read_write){
//#endif
//	short port=0;
//	int open_flags;
//	int status;
//	PIPE_HANDLE handle;
//	char errorInfo[ERROR_TXT_LEN]={0};
//	
//	//allocate PIPE_HANDLE struct
//	handle = (PIPE_HANDLE) malloc(sizeof(PIPE_HANDLE_STRUCT));
//	if(handle == NULL){
//		sprintf(errorInfo,"MakePipe: Could not allocate pipe handle- Malloc error\n");
//		ReportPipeError(PIPE_NO_ERROR_CODE, errorInfo);
//		return NULL;
//	}
//	handle->type = ParsePipeName(pipe_descriptor, handle->pipe_name, handle->raw_pipe_name, &port, read_write, &open_flags);
//
//	switch(handle->type){
//		case NAMED_PIPE:
//			#ifdef _WINDOWS_C
//				if( DoesSharedMemFileExist(handle, 's') && (read_write[0]=='r') ){
//					//server wants to connect to the same pipe that it created for reading (server side of the pipe)
//					//get saved server handle value from the shared file and connect to it
//					if(ReadHandleValueFromFile(handle) == -1){
//						free(handle);
//						return NULL;
//					}
//
//					//wait for a client to connect
//					if(WinServerConnectNamedPipe(handle) == -1){
//						free(handle);
//						return NULL;
//					}else{
//						return handle;
//					}					
//				}//end of if(DoesSharedMemFileExist..)
//
//
//				//Either the client process called OpenPipe, or the server did- to open a write handle to itself
//				if( !DoesSharedMemFileExist(handle, 's')  || ( DoesSharedMemFileExist(handle, 's') && (read_write[0]=='w') )  ){
//
//					if( DoesSharedMemFileExist(handle, 'c') ){
//						//the handle to the pipe was already created by another client
//						//duplicate the value of the shared client pipe handle
//						status = DuplicateHandleValueFromFile(handle);
//						if(status == -1){
//							free(handle);
//							return NULL;
//						}else{
//							return handle;
//						}
//					}else{//we are the first client that is trying to open a handle to the pipe
//						return OpenClientPipe(handle);						
//					}//end of if(DoesSharedMemFileExist('c'))
//
//				}else{//we should never get here
//					free(handle);
//					return NULL; 
//				}//end of if( !DoesSharedMemFileExist('s'))
//
//			#else
//
//				handle->id = open( handle->pipe_name, open_flags);
//				if(handle->id < 0){
//					status = CheckErrno(errorInfo);
//					ReportPipeError(status, errorInfo);
//					free(handle);
//					return NULL;
//				}else{
//					return handle;
//				}
//
//			#endif
//		break;
//		
//		case SOCKET_PIPE:
//			handle->socket =  OpenTCPConnection(handle->pipe_name,port);
//			if(handle->socket == NULL)
//			{
//				free(handle);
//				return NULL;
//			}else{
//				//set socket options
//				int socket_options = 0;
//				if(PIPE_DISABLE_NAGLE) socket_options |= NL_DISBALE_NAGLE; 
//				SetSocketOptions(handle->socket, socket_options);
//				
//				handle->listening_socket_open = 0;
//				return handle;
//			}
//		break;
//		
//		default:
//			return NULL;
//		break;
//	}
//}
//


/*
 * Reads n_bytes from the pipe into buffer
 * Returns the number of bytes read, or -1 if failed
 */
//int ReadPipe(PIPE_HANDLE handle, void* data_buffer, int n_bytes){
//	int bytes_read=0;
//	int total_bytes_read=0;
//	char *buffer;
//#ifdef _WINDOWS_C
//	int status, error_code;
//	char errorInfo[ERROR_TXT_LEN]={0};
//#endif
//	buffer = (char*) data_buffer;
//	switch(handle->type){
//		case NAMED_PIPE:
//				while(total_bytes_read < n_bytes){
//					#ifdef _WINDOWS_C
//						status = ReadFile( handle->id, (void *) buffer, n_bytes - total_bytes_read, &bytes_read, NULL);
//						if(!status){
//							error_code = GetLastError();
//							sprintf(errorInfo,"ReadPipe: ReadFile failed to read from the pipe. GetLastError() returned %i\n", error_code);
//							ReportPipeError(error_code, errorInfo);
//							bytes_read = -1;
//						}
//					#else
//						bytes_read = read( handle->id, (void *) buffer, n_bytes - total_bytes_read);
//					#endif
//
//					if(bytes_read < 0) return -1;
//					
//					buffer += bytes_read;
//					total_bytes_read += bytes_read;
//				}
//				return total_bytes_read;
//		break;
//		
//		case SOCKET_PIPE:
//			bytes_read = RecvData(handle->socket, buffer, n_bytes);
//			bytes_read = (bytes_read < 0)? -1 : bytes_read;
//			return bytes_read;
//		break;
//
//		default:
//			return -1;
//		break;
//	}
//}

/*
 * Writes n_bytes into the pipe
 * Returns the number of bytes written, or -1 if failed
 */
//int WritePipe(PIPE_HANDLE handle, void* data, int n_bytes){
//	int bytes_written=0;
//#ifdef _WINDOWS_C	
//	int status, error_code;
//	char errorInfo[ERROR_TXT_LEN]={0};
//#endif
//	switch(handle->type){
//		case NAMED_PIPE:
//			#ifdef _WINDOWS_C
//				status = WriteFile( handle->id, data, n_bytes, &bytes_written, NULL);
//				if(!status){
//					bytes_written = -1;
//					error_code = GetLastError();
//					sprintf(errorInfo,"WritePipe: Could not write to pipe: WriteFile returned %d, last system error code: %d\n",status, error_code );
//					ReportPipeError(status, errorInfo);
//				}
//			#else
//				bytes_written = write( handle->id, data, n_bytes);
//			#endif
//
//				bytes_written = ( (bytes_written < 0) || (bytes_written < n_bytes) ) ? -1 : bytes_written;
//				return bytes_written;
//		break;
//		
//		case SOCKET_PIPE:
//			bytes_written = SendData(handle->socket, (char*)data, n_bytes);
//			bytes_written = ( (bytes_written < 0) || (bytes_written < n_bytes) )? -1 : bytes_written;
//			return bytes_written;
//		break;
//
//		default:
//			return -1;
//		break;
//	}
//}

/*
 * Closes the pipe.
 * options (meaningful only for sockets): 0- nothing, 1-for WINDOWS only: will clean up Winsock (use when closing the last socket!)
 * Returns 0 on success, -1 on failure
 */
//int ClosePipe(PIPE_HANDLE handle){
//	int status=0;
//	int options = 0;
//	
//	if(handle == NULL){
//		ReportPipeError(PIPE_NO_ERROR_CODE, "ClosePipe: got a NULL handle to close\n");
//		return -1;
//	}
//
//	switch(handle->type){
//		case NAMED_PIPE:
//			#ifdef _WINDOWS_C
//				status = CloseHandle(handle->id);
//				if(!status){
//					ReportPipeError(PIPE_NO_ERROR_CODE,"ClosePipe: call to CloseHandle failed\n");
//					free(handle);
//					return -1;
//				}
//				//clear the shared file that was used
//				remove(handle->shared_file_name);
//			#else
//				status = close(handle->id);
//				if(status < 0)
//				{
//					free(handle);
//					return -1;
//				}
//				
//			#endif
//
//		break;
//		
//		case SOCKET_PIPE:
//			if(handle->listening_socket_open == 1)
//				CloseConnection(handle->listening_socket, options);
//			if(!CloseConnection(handle->socket,options))
//			{
//				ReportPipeError(PIPE_NO_ERROR_CODE, "ClosePipe: ERROR: call to CloseConnection failed\n");
//				free(handle);
//				return -1;
//			}
//		break;
//
//		default:
//			return -1;
//		break;
//	}
//	
//	free(handle);
//	return 0;
//}

/*
 * Closes the pipe and:
 * - On Windows does nothing more
 * - On UNIX tries to remove the pipe file after closing it
 * Returns 0 on success, -1 on failure
 */
//int RemovePipe(PIPE_HANDLE handle)
//{
//	#ifdef _WINDOWS_C
//		return ClosePipe(handle);
//	#else
//        int status;
//		if( (handle == NULL) || (handle->pipe_name == NULL) )
//		{
//			ReportPipeError(PIPE_NO_ERROR_CODE, "RemovePipe: got NULL handle\n");
//			return -1;
//		}
//		
//		ClosePipe(handle);
//		
//		status = unlink(handle->pipe_name);
//		if(status == -1)
//		{
//			char errorInfo[ERROR_TXT_LEN]={0};
//			sprintf(errorInfo, "RemovePipe: Failed to remove pipe file %s\n", handle->pipe_name);
//			CheckErrno(&errorInfo[strlen(errorInfo)]);
//			errorInfo[strlen(errorInfo)] = 0;
//			ReportPipeError(errno, errorInfo);
//		}
//		return status;
//	#endif
//}

/*
 * Returns the error code for the last known system error, or PIPE_NO_ERROR_CODE if no error code is available
 * Returns PIPE_NO_ERROR if no error occured since the last time the function was called
 * Fills in a text error message (if applicable) in errstr
 * Once the error is read, it is cleared, so the following calls will not be able to read the same error
 */
//int GetLastPipeError(char* errstr, unsigned int buffer_size){
//	int last_error_code, itr;
//
//	if(PipeErrorOccured == PIPE_NO_ERROR){
//		sprintf(errstr, "No pipe error\n");
//		return PIPE_NO_ERROR;
//	}else{
//		//copy error text & code to return variables
//		last_error_code = PipeErrorCode;		
//		//if given buffer is too small- copy only part of the error text
//		if(buffer_size <= strlen(PipeErrorInfo) ){
//			strncpy(errstr, PipeErrorInfo, buffer_size);
//		}else{//copy all of the error text
//			strncpy(errstr, PipeErrorInfo, strlen(PipeErrorInfo)+1);
//		}
//
//		//clear current error code & text
//		PipeErrorOccured = PIPE_NO_ERROR;
//		PipeErrorCode    = PIPE_NO_ERROR;
//		for(itr=0; itr < ERROR_TXT_LEN; itr++)
//			PipeErrorInfo[itr]=0;
//
//		return last_error_code;
//	}
//}

/*
 * Fills in the given error text in PipeErrorInfo, error code in PipeErrorCode, and sets PipeErrorOccured
 */
//void ReportPipeError(int error_code, char* error_text){
//	unsigned int error_len;
//	PipeErrorOccured  = PIPE_ERROR;
//	PipeErrorCode     = error_code;
//
//	if(error_text != NULL){
//		error_len = (unsigned int)strlen(error_text);
//		if( error_len >= ERROR_TXT_LEN ){//if the given error is longer than our buffer -copy part of it
//			strncpy(PipeErrorInfo, error_text, ERROR_TXT_LEN);
//		}else{
//			strncpy(PipeErrorInfo, error_text, error_len+1);
//		}
//	}
//
//}



/*
 *  Reads the HANDLE from the shared file to handle->id
 *  Returns pid of creator on sucess, -1 on failure
 */
//int ReadHandleValueFromFile(PIPE_HANDLE handle){
//	FILE* f;
//	char  errorInfo[ERROR_TXT_LEN]={0};
//	unsigned int   status=0, pid=0;
//
//	f      = fopen(handle->shared_file_name, "rb");
//	if(f == NULL){
//		sprintf(errorInfo, "ReadHandleValueFromFile: could not open the file %s", handle->shared_file_name);
//		ReportPipeError(PIPE_NO_ERROR_CODE, errorInfo);
//		return -1;
//	}
//
//	status = (unsigned int)fread((void*)&handle->id,sizeof(HANDLE), 1, f);
//	if(status != 1){
//		sprintf(errorInfo, "ReadHandleValueFromFile: could not read handle from the file %s", handle->shared_file_name);
//		ReportPipeError(PIPE_NO_ERROR_CODE, errorInfo);
//		return -1;
//	}
//
//	status = (unsigned int)fread((void*)&pid,sizeof(int), 1 , f);
//	if(status != 1){
//		sprintf(errorInfo, "ReadHandleValueFromFile: could not read pid from the file %s", handle->shared_file_name);
//		ReportPipeError(PIPE_NO_ERROR_CODE, errorInfo);
//		return -1;
//	}
//
//	fclose(f);
//	return pid;
//}

/*
 *  Reads the HANDLE from the shared file, and DUPLICATES it to handle->id
 *  Returns 0 on sucess, -1 on failure
 */
//int DuplicateHandleValueFromFile(PIPE_HANDLE handle){
//	char  errorInfo[ERROR_TXT_LEN]={0};
//	int   status, client_pid;
//	HANDLE source_handle;
//
//	client_pid = ReadHandleValueFromFile(handle);
//	if( client_pid == -1)
//		return -1;
//
//	source_handle = handle->id;
//
//	status = DuplicateHandle(
//				OpenProcess(PROCESS_ALL_ACCESS, TRUE, (DWORD) client_pid), //Handle to the process with the handle to duplicate
//				source_handle,				   //Handle to duplicate. This is an open object handle that is valid in the context of the source process
//				GetCurrentProcess(),       //Handle to the process that is to receive the duplicated handle (self)
//				&handle->id,                //Pointer to a variable that receives the duplicate handle
//				0,						   //Access requested for the new handle (ignored because DUPLICATE_SAME_ACCESS is used)
//				TRUE,                      //Indicates whether the handle is inheritable
//				DUPLICATE_SAME_ACCESS      //The duplicate handle has the same access as the source handle
//				);
//	if(status == 0){
//		status = GetLastError();
//		sprintf(errorInfo, "DuplicatePipeHandle: Could not duplicate the handle. Last system error code: %d\n", status );
//		ReportPipeError(status, errorInfo);
//		return -1;
//	}else{
//		return 0;
//	}
//}

/*
 * Creates a Windows PIPE, and updates the handle to it
 * Returns 0 on success, -1 otherwise
 * 
 * parameters:
 * open_flags: a DWORD set by ParsePipeName(...): 
 * 
 * tip: Windows pipe names must have the form: \\.\pipe\PIPE_NAME
 */
//int MakeWinPipe(PIPE_HANDLE handle, DWORD open_flags){
//	char errorInfo[ERROR_TXT_LEN]={0};
//	DWORD pipe_mode, max_instances, out_buffer_size, in_buffer_size, default_timeout;
//	DWORD error_code;
//	int itr=0;
//
//	//set pipe configuration (see PipeIO.h for values)
//	pipe_mode = PIPE_TYPE_MESSAGE | PIPE_WAIT; //Data is written to the pipe as a stream of messages | Blocking mode is enabled
//	max_instances    = WINPIPE_MAX_INSTANCES;
//	out_buffer_size  = WINPIPE_OUT_BUFFER_SIZE;
//	in_buffer_size   = WINPIPE_IN_BUFFER_SIZE;
//	default_timeout  = WINPIPE_TIMEOUT;
//		
//	handle->id = CreateNamedPipe( handle->pipe_name, // pipe name 
//									open_flags,      // read/write access 
//									pipe_mode,       // message type pipe 
//									max_instances,   // max. instances  
//									out_buffer_size, // output buffer size 
//									in_buffer_size,  // input buffer size 
//									default_timeout, // client time-out 
//									NULL);           // default security attribute (handle cannot be inherited)
//	if(handle->id == INVALID_HANDLE_VALUE){
//		error_code = GetLastError();
//		sprintf(errorInfo,"MakeWinPipe: Failed to create named pipe. Last system error code: %i\n",error_code);
//		ReportPipeError(error_code, errorInfo);
//		CloseHandle(handle->id);
//		return -1;
//	}
//
//	return CreateSharedMemFile(handle, 's');
//}

/* 
 * BLOCKING function: causes the server to wait until a client connect to the other end of the pipe
 *
 * Returns 0 on success, -1 on error
 */
//int WinServerConnectNamedPipe(PIPE_HANDLE handle){
//	char errorInfo[ERROR_TXT_LEN]={0};
//	DWORD error_code;
//
//	if(	!ConnectNamedPipe( handle->id, NULL)){
//		error_code = GetLastError();
//		//if there is already someone connnected to the pipe- then it should be ok to proceed
//		if(error_code == 535)
//			return 0;
//		sprintf(errorInfo,"WinServerConnectNamedPipe: Failed on ConnectNamedPipe. Last error was: %i\n",error_code);
//		ReportPipeError(error_code, errorInfo);
//		return -1;
//	}else{
//		return 0;
//	}
//}

/*
 * Opens the client side of the pipe. 
 * This function should be called by the FIRST client that is trying to open the client pipe.
 *
 * Returns a PIPE_HANDLE, or NULL on failure
 */
//PIPE_HANDLE OpenClientPipe(PIPE_HANDLE handle){
//	DWORD pipe_mode = PIPE_READMODE_MESSAGE;//pipe mode setting for Messages only
//	DWORD error_code;
//	char errorInfo[ERROR_TXT_LEN]={0};
//	char pid_var_name[PIPE_NAME_SIZE]={0};
//	char handle_var_name[PIPE_NAME_SIZE]={0};
//
//	while(1){		
//		handle->id = CreateFile(handle->pipe_name, // pipe name 
//			GENERIC_WRITE, //| GENERIC_READ, // read and write access 
//			FILE_SHARE_WRITE,// no sharing =0?
//			NULL,           // default security attributes
//			OPEN_EXISTING,  // opens existing pipe 
//			0,              // default attributes 
//			NULL);          // no template file
//		//if we have a valid pipe handle - then break the while
//		if(handle->id != INVALID_HANDLE_VALUE)
//			break;
//		//if we have an error other than a busy pipe- quit
//		error_code = GetLastError();
//		if (error_code != ERROR_PIPE_BUSY){
//			sprintf(errorInfo, "OpenClientPipe: Could not open pipe: Last error was %i\n",(int)error_code);
//			ReportPipeError((int)error_code, errorInfo);
//			return NULL;
//		}
//		//if we are here- then the pipe is busy- wait for it
//		if (!WaitNamedPipe(handle->pipe_name, WINPIPE_OPEN_WAIT_TIME)){ 
//			sprintf(errorInfo,"OpenClientPipe: Could not open pipe- pipe was busy for more than 3 seconds\n"); 
//			ReportPipeError(PIPE_NO_ERROR_CODE, errorInfo);
//			return NULL;
//		} 
//		
//	}//end while
//
//	//set pipe mode to messages (same as MakePipe does)
//	if( !SetNamedPipeHandleState( handle->id,    // pipe handle 
//		&pipe_mode  ,  // new pipe mode 
//		NULL,          // don't set maximum bytes 
//		NULL) ){       // don't set maximum time 
//		sprintf(errorInfo, "OpenPipe: SetNamedPipeHandleState failed");
//		ReportPipeError(PIPE_NO_ERROR_CODE, errorInfo);
//		return NULL;
//	}
//
//	//make sure that other clients know that the handle already exists
//	if( CreateSharedMemFile(handle, 'c') == -1){
//		//error is reported by the called fuction
//		return NULL;
//	}else{
//		return handle;
//	}
//	
//}

/*
 * Returns the PID of the current process
 */
//int GetSelfPid(){
//	return _getpid();
//}

/* ----------------------------------------------------------------------------      |
   |              End of   _WINDOWS_C only functions                          |------
   ----------------------------------------------------------------------------       */



/* ----------------------------------------------------------------------------
   |                       _UNIX_C only functions                             |------
   ----------------------------------------------------------------------------      |*/
#ifdef _UNIX_C
/* 
 * Checks the extern errono value, prints an appropriate text error into errorInfo, and returns its value
 */
int CheckErrno(char* errorInfo){
	switch(errno){
		case EPERM:
			sprintf(errorInfo,"[EPERM]= The invoking process does not have appropriate privileges and the file type is not FIFO-special\n");
		break;
		case ENOTDIR:
			sprintf(errorInfo,"[ENOTDIR]= A component of the path prefix is not a directory\n");
		break;
		case ENOENT:
			sprintf(errorInfo,"[ENOENT]= A component of the path prefix specified by path does not name an existing directory or path is an empty string\n");
		break;
		case EROFS:
			sprintf(errorInfo,"[EROFS]= The directory in which the file is to be created is located on a read-only file system\n");
		break;
		case EEXIST:
			sprintf(errorInfo,"[EEXIST]= The named file exists\n");
		break;
		case EIO:
			sprintf(errorInfo,"[EIO]= An I/O error occurred while accessing the file system. Usually used for physical read or write errors\n");
		break;
		case EINVAL:
			sprintf(errorInfo,"[EINVAL]= An invalid argument exists\n");
		break;
		case ENOSPC:
			sprintf(errorInfo,"[ENOSPC]= The directory that would contain the new file cannot be extended or the file system is out of file allocation resources\n");
		break;
		case ELOOP:
			sprintf(errorInfo,"[ELOOP]= Too many symbolic links were encountered in resolving path\n");
		break;
		case ENAMETOOLONG:
			sprintf(errorInfo,"[ENAMETOOLONG]= The length of a pathname exceeds {PATH_MAX}, or pathname component is longer than {NAME_MAX} OR\nPathname resolution of a symbolic link produced an intermediate result whose length exceeds {PATH_MAX}\n");
		break;
		case ESRCH:
			sprintf(errorInfo,"[ESRCH]= No process matches the specified process ID\n");
		break;
		case EINTR:
			sprintf(errorInfo,"[EINTR]= Interrupted function call; an asynchronous signal occurred and prevented completion of the call. When this happens, you should try the call again.\n");
			break;
		case ENXIO:
			sprintf(errorInfo,"[ENXIO]= No such device or address. The system tried to use the device represented by a file you specified, and it couldn't find the device. This can mean that the device file was installed incorrectly, or that the physical device is missing or not correctly attached to the computer.\n");
		break;
		case E2BIG:
			sprintf(errorInfo,"[E2BIG]= Argument list too long; used when the arguments passed to a new program being executed with one of the exec functions occupy too much memory space. This condition never arises in the GNU system.\n");
		break;
		case ENOEXEC:
			sprintf(errorInfo,"[ENOEXEC]= Invalid executable file format. This condition is detected by the exec functions\n");
		break;
		case EBADF:
			sprintf(errorInfo,"[EBADF]= Bad file descriptor; for example, I/O on a descriptor that has been closed or reading from a descriptor open only for writing (or vice versa).\n");
		break;
		case ENOMEM:
			sprintf(errorInfo,"[ENOMEM]= No memory available. The system cannot allocate more virtual memory because its capacity is full.\n");
		break;
		case EACCES:
			sprintf(errorInfo,"[EACCES]= Permission denied; the file permissions do not allow the attempted operation.\n");
		break;
		default:
			sprintf(errorInfo,"[]= An unknown error occured, could not find an appropriate error value for errno- using (-1) as default\n");
			return -1;
		break;
	}
	return errno;
}
#endif
/* ----------------------------------------------------------------------------      |
   |                 End of   _UNIX_C only functions                          |------
   ----------------------------------------------------------------------------       */
