/*
 * Exception class to be used with C++ code
 * Written by Sagi Perel, 03/21/06
 */

#include "MyCException.h"

MyCException::MyCException() : m_TraceIndex(0), m_ErrorCode(0)
{
}

MyCException::MyCException(const char* err_str, int error_code): m_TraceIndex(1), m_ErrorCode(error_code)
{
	m_StackTrace[0].SetContent(err_str);
}

MyCException::MyCException(const MyCString& str, int error_code): m_TraceIndex(1), m_ErrorCode(error_code)
{
	m_StackTrace[0].SetContent(str.GetContent());
}

MyCException::MyCException(const MyCException &E) : m_TraceIndex(E.m_TraceIndex), m_ErrorCode(E.m_ErrorCode)
{
	int itr;
	for(itr=0; itr < MYCEXCEPTION_STACK_TRACE_SIZE; itr++){
		m_StackTrace[itr].SetContent(E.m_StackTrace[itr]);
	}
}

MyCException::~MyCException( )
{
}

void MyCException::Set(const char* err_str, int error_code)
{
	m_TraceIndex = 1;
	m_ErrorCode  = error_code;
	m_StackTrace[0].SetContent(err_str);

}

void MyCException::Set(const MyCString& str, int error_code)
{
	m_TraceIndex = 1;
	m_ErrorCode  = error_code;
	m_StackTrace[0].SetContent(str);
}

void MyCException::Set(const MyCException &E)
{
	m_TraceIndex = E.m_TraceIndex;
	m_ErrorCode  = E.m_ErrorCode;
	int itr;
	for(itr=0; itr < MYCEXCEPTION_STACK_TRACE_SIZE; itr++){
		m_StackTrace[itr].SetContent(E.m_StackTrace[itr]);
	}

}

void MyCException::ReportToFile(const char* file_name)
{
	int itr;
	ofstream ReportFile;
	MyCString time;

	if(file_name != NULL)
		ReportFile.open(MYCEXCEPTION_STACK_FILE_NAME, ios::app);
	else
		ReportFile.open(file_name, ios::app);
	//ReportFile << "--------------------------------------" << endl << "Stack trace created " << __DATE__ << " " << __TIME__ << endl << "--------------------------------------" << endl;
	
	ReportFile << endl << "--------------------------------------" << endl << "Stack trace" << endl << time << endl << "--------------------------------------" << endl;
	for(itr=0; itr < m_TraceIndex; itr++){
		ReportFile << "(" << itr+1 << ") ";
		ReportFile << m_StackTrace[itr].GetContent() << endl;
	}
	ReportFile.close();
	
}

void MyCException::AddToStack(const MyCString &err_str)
{
	/*char tmp[1000]={0};
	char file[1000] = {__FILE__};
	int line        =  __LINE__;
#ifdef _UNIX_C
    //__FUNCTION__ not defined for VisualC++ compiler
	//char func[1000] = {__FUNCTION__};
	//sprintf(tmp, "file %s, line %d: function %s: ",file, line, func );
	sprintf(tmp, "file %s, line %d: function %s: ",file, line );
#else
	sprintf(tmp, "file %s, line %d: ",file, line );
#endif
	m_StackTrace[m_TraceIndex].SetContent(tmp);
	m_StackTrace[m_TraceIndex].AppendContent(err_str);
	m_TraceIndex++;*/

	m_StackTrace[m_TraceIndex].SetContent(err_str);
	m_TraceIndex++;
}

void MyCException::AddToStack(const char* err_str)
{
	/*char tmp[1000]={0};
	char file[1000] = {__FILE__};
	int line        =  __LINE__;
#ifdef _UNIX_C
    //__FUNCTION__ not defined for VisualC++ compiler
	//char func[1000] = {__FUNCTION__};
	//sprintf(tmp, "file %s, line %d: function %s: ",file, line, func );
	sprintf(tmp, "file %s, line %d: function %s: ",file, line );
#else
	sprintf(tmp, "file %s, line %d: ",file, line );
#endif
	m_StackTrace[m_TraceIndex].SetContent(tmp);
	m_StackTrace[m_TraceIndex].AppendContent(err_str);
	m_TraceIndex++;*/

	m_StackTrace[m_TraceIndex].SetContent(err_str);
	m_TraceIndex++;
}

char* MyCException::GetTrace(int trace_level) const
{
	return (this->m_StackTrace[trace_level].GetContent());
}

void MyCException::AppendTraceToString(MyCString& str) const
{
	int itr=0;

	str +=  MyCString("\n") + "--------------------------------------" +  "\n" + "Stack trace " + "\n" + "--------------------------------------" + "\n";
	str +=  MyCString("Reported error code: ") + m_ErrorCode + "\n";
	for(itr=0; itr < m_TraceIndex; itr++){
		str += MyCString("[") + itr + "] " + m_StackTrace[itr] + "\n";
	}

}

void MyCException::Reset()
{
	for(int i=0; i<MYCEXCEPTION_STACK_TRACE_SIZE; i++)
		m_StackTrace[i].Reset();

	m_TraceIndex=0;
	m_ErrorCode=0;
}


#ifdef _WINDOWS_C
/*
 * Handles unexpected exception types
 * Should be assigned from the main by calling:  set_unexpected(HandleUnexpectedExceptions);
 */
void HandleUnexpectedExceptions(){
	MyCException E("HandleUnexpectedExceptions got unknown exception type");
	E.ReportToFile();
}
#endif

