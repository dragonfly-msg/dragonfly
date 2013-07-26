/*
 * Returns how many bytes the given Matlab array contains
 */
int CountDataBytes( const mxArray *Data)
{
    int NumElements, ElementSize, NumDataBytes;
    const mxArray *Field;
    const mxArray *Element;
    int NumFields;
    int i, el;
    int TotalDataBytes = 0;

    if( mxIsNumeric( Data)) {
        
        NumElements = mxGetNumberOfElements( Data);
        ElementSize = mxGetElementSize( Data);
        TotalDataBytes = NumElements * ElementSize;
        
    } else if( mxIsChar( Data)) {
        
        NumElements = mxGetNumberOfElements( Data);
        TotalDataBytes = sizeof(char) * NumElements;
        
    } else if( mxIsStruct( Data)) {

        NumFields   = mxGetNumberOfFields( Data);
		NumElements = mxGetNumberOfElements( Data);//check if this is an array of structures
		for( el=0; el < NumElements; el++)
		{
            for( i = 0; i < NumFields; i++) {
                Field = mxGetFieldByNumber( Data, el, i);
                NumDataBytes = CountDataBytes( Field);
                TotalDataBytes += NumDataBytes;
            }
        }
        
    } else if( mxIsCell( Data)) { 

        NumElements = mxGetNumberOfElements( Data);
        for( i = 0; i < NumElements; i++) {
            Element = mxGetCell( Data, i);
            NumDataBytes = CountDataBytes( Element);
            TotalDataBytes += NumDataBytes;
        }
        
    } else {
        mexErrMsgTxt("CountDataBytes: Unsupported data type");
    }
    return TotalDataBytes;
}

/*
 * Get data from a C array to a matlab array
 */
void* FillOutputArray( const mxArray *Output, void *pData)
{
    int NumDataBytes;
    const mxArray *Field;
    void *pOutputData;
    int NumFields;
    int i, el;
    unsigned char *C_String;
    unsigned short *MatlabString;
    int NumChars;
    const mxArray *Element;
    int NumElements;

    if( mxIsNumeric( Output)) {

        pOutputData = mxGetData( Output);
        NumDataBytes = CountDataBytes( Output);
        memcpy( pOutputData, pData, NumDataBytes);
        return (void *) ((char *) pData + NumDataBytes);

    } else if( mxIsChar( Output)) {
        
        MatlabString = (unsigned short*) mxGetData( Output);
        NumChars = mxGetNumberOfElements( Output);
        C_String = (unsigned char*) pData;
        for( i = 0; i < NumChars; i++) {
            MatlabString[i] = C_String[i];
        }
        NumDataBytes = CountDataBytes( Output);
        return (void *) ((char *) pData + NumDataBytes);

    } else if( mxIsStruct( Output)) {

        NumFields   = mxGetNumberOfFields( Output);
		NumElements = mxGetNumberOfElements( Output);
		for( el=0; el < NumElements; el++)
		{
			for( i = 0; i < NumFields; i++) {
				Field = mxGetFieldByNumber( Output, el, i);
				pData = FillOutputArray( Field, pData);
			}
		}
        return pData;

    } else if( mxIsCell( Output)) { 

        NumElements = mxGetNumberOfElements( Output);
        for( i = 0; i < NumElements; i++) {
            Element = mxGetCell( Output, i);
            pData = FillOutputArray( Element, pData);
        }
        
    } else {
        mexErrMsgTxt( "FillOutputArray: Unsupported data type");
    }
    return NULL;
}

/*
 * Get data from a matlab array to a C array
 */
void* ExtractData( const mxArray *Data, void *pExtractedData)
{
    int NumDataBytes;
    const mxArray *Field;
    void *pData;
    int NumFields;
    int i;
    unsigned char *C_String;
    unsigned short *MatlabString;
    int NumChars;
    const mxArray *Element;
    int NumElements;

    if( mxIsNumeric( Data)) {

        pData = mxGetData( Data);
        NumDataBytes = CountDataBytes( Data);
        memcpy( pExtractedData, pData, NumDataBytes);
        return (void *) ((char *) pExtractedData + NumDataBytes);

    } else if( mxIsChar( Data)) {
        
        MatlabString = (unsigned short*) mxGetData( Data);
        NumChars = mxGetNumberOfElements( Data);
        C_String = (unsigned char*) pExtractedData;
        for( i = 0; i < NumChars; i++) {
            if( MatlabString[i] > 255) mexErrMsgTxt( "ExtractData: char value greater than 255");
            C_String[i] = MatlabString[i];
        }
        NumDataBytes = CountDataBytes( Data);
        return (void *) ((char *) pExtractedData + NumDataBytes);
        
    } else if( mxIsStruct( Data)) {

        NumFields = mxGetNumberOfFields( Data);
        for( i = 0; i < NumFields; i++) {
            Field = mxGetFieldByNumber( Data, 0, i);
            pExtractedData = ExtractData( Field, pExtractedData);
        }
        return pExtractedData;

    } else if( mxIsCell( Data)) { 

        NumElements = mxGetNumberOfElements( Data);
        for( i = 0; i < NumElements; i++) {
            Element = mxGetCell( Data, i);
            pExtractedData = ExtractData( Element, pExtractedData);
        }
        return pExtractedData;
        
    } else {
        mexErrMsgTxt( "ExtractData: Unsupported data type");
    }
    return NULL;
}

/*
 * Get data from a matlab array to a C array
 */
void* SerializeData( const mxArray *Data, int *pNumDataBytes)
{
    int NumDataBytes, CheckSum;
    void *pExtractedData, *pEndData;

    NumDataBytes = CountDataBytes( Data);

    pExtractedData = mxMalloc( NumDataBytes);
    if( pExtractedData == NULL) mexErrMsgTxt( "SerializeData: Could not allocate memory for data buffer");
    pEndData = ExtractData( Data, pExtractedData);

    CheckSum = (char *) pEndData - (char *) pExtractedData;
    if( CheckSum != NumDataBytes) mexErrMsgTxt( "SerializeData: Looks like a bug, wrong number of bytes copied");

    *pNumDataBytes = NumDataBytes;
    return pExtractedData;
}

mxArray *C2Matlab( const mxArray *Template, void *pData, int NumDataBytes)
{
    int NumTemplateBytes;
    mxArray *ReturnData;
    
    NumTemplateBytes = CountDataBytes( Template);
    if( NumTemplateBytes != NumDataBytes)
	{
		char error_txt[1000]={0};
		sprintf(error_txt, "C2Matlab: number of template bytes [%i] does not match number of C data bytes [%i]",NumTemplateBytes, NumDataBytes );
		mexErrMsgTxt( error_txt);
	}
    ReturnData = mxDuplicateArray( Template);
    if( ReturnData == NULL) mexErrMsgTxt( "Could not make copy of template array");
    FillOutputArray( ReturnData, pData);
    return ReturnData;
}
