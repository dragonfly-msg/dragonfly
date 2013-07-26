// CMessageBuffer - a class that contains a binary memory buffer of Dragonfly messages,
// allows messages to be added, and then saved to a binary file (the intention being
// to make message logging be lightning fast in an Dragonfly application.
//
// Meel Velliste 10/29/2008

#ifndef _MESSAGE_BUFFER_H_
#define _MESSAGE_BUFFER_H_

#define MESSAGE_BUFFER_FILE_FORMAT_VERSION  1

typedef struct {
	unsigned int FileFormatVersion;
	unsigned int FileTotalBytes;
	unsigned int NumMessages;
	unsigned int MessageHeaderSize;
	unsigned int DataBlockOffsetSize;
	unsigned int NumDataBytes;
} MESSAGE_BUFFER_FILE_HEADER;

class CMessageBuffer {
protected:
	unsigned int NumPreallocMessages;
	unsigned int MessageHeaderBytes;
	unsigned int NumPreallocDataBytes;

	unsigned int NumMessages;
	char *Header;                   // Dynamically allocated array of message headers
	unsigned int *DataBlockOffsets; // Dynamically allocated array of data block offsets
	char *Data;                     // Dynamically allocated buffer of message data
	unsigned int TotalDataBytes;

	void DeallocateBuffer( void) {
		if( Header != NULL)           delete [] Header;
		if( DataBlockOffsets != NULL) delete [] DataBlockOffsets;
		if( Data != NULL)             delete [] Data;
	}
public:
	CMessageBuffer( ) {
		NumPreallocMessages = 0;
		MessageHeaderBytes = 0;
		NumPreallocDataBytes = 0;

		NumMessages = 0;
		Header = NULL;
		DataBlockOffsets = NULL;
		Data = NULL;
		TotalDataBytes = 0;
	}
	~CMessageBuffer( ) {
		DeallocateBuffer( );
	}
	void PreallocateBuffer( unsigned int num_prealloc_messages, unsigned int message_header_bytes, unsigned int num_prealloc_databytes)
	{
		DeallocateBuffer( );

		NumPreallocMessages = num_prealloc_messages;
		MessageHeaderBytes = message_header_bytes;
		NumPreallocDataBytes = num_prealloc_databytes;

		Header = new char[NumPreallocMessages * MessageHeaderBytes];
		if( Header == NULL) throw MyCException( "CMessageBuffer::PreallocateBuffer(): Out of memory while trying to allocate buffer for message headers");

		DataBlockOffsets = new unsigned int[NumPreallocMessages];
		if( DataBlockOffsets == NULL) throw MyCException( "CMessageBuffer::PreallocateBuffer(): Out of memory while trying to allocate buffer for data block offsets");

		Data = new char[NumPreallocDataBytes];
		if( Data == NULL) throw MyCException( "CMessageBuffer::PreallocateBuffer(): Out of memory while trying to allocate buffer for message data");

		NumMessages = NumPreallocMessages;
		TotalDataBytes = NumPreallocDataBytes;
		ClearBuffer( );
	}
	void ClearBuffer( void)
	{
		memset( Header, 0, MessageHeaderBytes * NumMessages);
		memset( DataBlockOffsets, 0, sizeof(*DataBlockOffsets) * NumMessages);
		memset( Data, 0, TotalDataBytes);
		NumMessages = 0;
		TotalDataBytes = 0;
	}
	unsigned int GetNumPreallocMessages( void)
	{
		return NumPreallocMessages;
	}
	unsigned int GetMessageHeaderBytes( void)
	{
		return MessageHeaderBytes;
	}
	unsigned int GetNumPreallocDataBytes( void)
	{
		return NumPreallocDataBytes;
	}
	unsigned int GetNumMessages( void)
	{
		return NumMessages;
	}
	unsigned int GetNumDataBytes( void)
	{
		return TotalDataBytes;
	}
	char* GetHeaderPtr( void) // Get pointer to the entire array of message headers
	{
		return Header;
	}
	char* GetDataPtr( unsigned int i) // Get pointer to i-th data block
	{
		if( i >= NumMessages) throw MyCException( "CMessageBuffer::GetDataPtr(): index exceeds number of stored messages");
		if( DataBlockOffsets[i] >= TotalDataBytes) throw MyCException( "CMessageBuffer::GetDataPtr(): Data block address would exceed amount of data in buffer");
		return Data + DataBlockOffsets[i];
	}
	void SaveDatafile( char *filename)
	{
		FILE *f = fopen( filename, "wb");
		if( f == NULL) throw MyCException( (MyCString) "CMessageBuffer::SaveDatafile: Unable to open data file '" + filename + "'");
		unsigned int num_items_written;
		// Write a file header, containing, number of messages and number of data bytes etc.
		MESSAGE_BUFFER_FILE_HEADER h;
		h.FileFormatVersion = MESSAGE_BUFFER_FILE_FORMAT_VERSION;
		h.NumMessages = NumMessages;
		h.MessageHeaderSize = MessageHeaderBytes;
		h.DataBlockOffsetSize = sizeof(*DataBlockOffsets);
		h.NumDataBytes = TotalDataBytes;
		h.FileTotalBytes = sizeof(h) + h.NumMessages*(h.MessageHeaderSize+h.DataBlockOffsetSize) + h.NumDataBytes;
		num_items_written = fwrite( &h, sizeof(h), 1, f);
		if( num_items_written != 1) throw MyCException( "CMessageBuffer::SaveDatafile: Could not write file header");
		// Write the buffer of headers
		num_items_written = fwrite( Header, MessageHeaderBytes, NumMessages, f);
		if( num_items_written != NumMessages) throw MyCException( "CMessageBuffer::SaveDatafile: Could not write message headers to file");
		// Write the buffer of data block offsets
		num_items_written = fwrite( DataBlockOffsets, sizeof(*DataBlockOffsets), NumMessages, f);
		if( num_items_written != NumMessages) throw MyCException( "CMessageBuffer::SaveDatafile: Could not write data block offsets to file");
		// Write the data blocks buffer
		num_items_written = fwrite( Data, 1, TotalDataBytes, f);
		if( num_items_written != TotalDataBytes) throw MyCException( "CMessageBuffer::SaveDatafile: Could not write data blocks to file");
		fclose( f);
	}
	void LoadDatafile( char *filename)
	{
		FILE *f = fopen( filename, "rb");
		if( f == NULL) throw MyCException( (MyCString) "CMessageBuffer::LoadDatafile: Unable to open data file '" + filename + "'");
		unsigned int num_items_read;
		// Read file header, containing, number of messages and number of data bytes etc.
		MESSAGE_BUFFER_FILE_HEADER h;
		num_items_read = fread( &h, sizeof(h), 1, f);
		if( num_items_read != 1) throw MyCException( "CMessageBuffer::LoadDatafile: Could not read file header");
		// Check that file format version matches
		if( h.FileFormatVersion != MESSAGE_BUFFER_FILE_FORMAT_VERSION) throw MyCException( (MyCString) "CMessageBuffer::LoadDataFile: File format version (" + h.FileFormatVersion + ") does not match this reader's version (" + MESSAGE_BUFFER_FILE_FORMAT_VERSION + ")");
		// Do sanity checks on header information
		if( h.DataBlockOffsetSize != sizeof(*DataBlockOffsets)) throw MyCException( MyCString( "CMessageBuffer::LoadDataFile: Number of data block offset bytes in file (") + h.DataBlockOffsetSize + ") does not match size of data block offset in reader (" + (int)sizeof(*DataBlockOffsets) + ")");
		unsigned int DoubleCheckFileTotalBytes = sizeof(h) + h.NumMessages*(h.MessageHeaderSize+h.DataBlockOffsetSize) + h.NumDataBytes;
		if( h.FileTotalBytes != DoubleCheckFileTotalBytes) throw MyCException( MyCString( "CMessageBuffer::LoadDataFile: Number of file total bytes in file header (") + h.FileTotalBytes + ") does not match number of total bytes calculated from other header fields (" + DoubleCheckFileTotalBytes + ")");
		unsigned int FileLength = GetFileLength( f);
		if( FileLength != h.FileTotalBytes) throw MyCException( MyCString( "CMessageBuffer::LoadDataFile: Actual file length (") + FileLength + ") does not match file length field in header (" + h.FileTotalBytes + ")");
		// Allocate memory buffer
		PreallocateBuffer( h.NumMessages, h.MessageHeaderSize, h.NumDataBytes);
		// Read the buffer of headers
		num_items_read = fread( Header, h.MessageHeaderSize, h.NumMessages, f);
		if( num_items_read != h.NumMessages) throw MyCException( "CMessageBuffer::LoadDatafile: Could not read message headers from file");
		NumMessages = h.NumMessages;
		// Read the buffer of data block offsets
		num_items_read = fread( DataBlockOffsets, sizeof(*DataBlockOffsets), h.NumMessages, f);
		if( num_items_read != h.NumMessages) throw MyCException( "CMessageBuffer::LoadDatafile: Could not read data block offsets from file");
		// Read the data blocks buffer
		num_items_read = fread( Data, 1, h.NumDataBytes, f);
		if( num_items_read != h.NumDataBytes) throw MyCException( "CMessageBuffer::LoadDatafile: Could not read data blocks from file");
		TotalDataBytes = h.NumDataBytes;
		fclose( f);
	}
private:
	unsigned int GetFileLength( FILE *f)
	{
		long current_pos = ftell( f);
		fseek( f, 0, SEEK_END);
		long end_pos = ftell( f);
		fseek( f, current_pos, SEEK_SET);
		if( end_pos > (unsigned int) 0xFFFFFFFF) throw MyCException( "CMessageBuffer::GetFileLength(): File is too long");
		return (unsigned int) end_pos;
	}
};

#endif
