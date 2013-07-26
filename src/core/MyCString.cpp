/*
 * String class to be used with C++ code
 * Written by Sagi Perel, 03/21/06
 */

#include "MyCString.h"

#ifdef _UNIX_C
void itoa(int number, char* buf, int radix)
{
     //did not put implementation for radix yet- for now it is only base 10
     sprintf(buf, "%i", number);
}

void itoa(unsigned int number, char* buf, int radix)
{
     //did not put implementation for radix yet- for now it is only base 10
     sprintf(buf, "%i", number);
}
#else
#endif


MyCString::MyCString()
{
	m_Content   = NULL;
	m_Len       = 0;
	InitTokenizer();
}

MyCString::MyCString(const char* str)
{
	if(str == NULL){
		m_Content = NULL;
		m_Len     = 0;
	}else{
		m_Len = strlen(str);
		m_Content = new char[m_Len+1];
		m_Content[m_Len] = 0;
		strncpy(m_Content, str, m_Len);
	}
	InitTokenizer();
}

MyCString::MyCString(const MyCString &cstring)
{
	if(cstring.IsNull() ){
		m_Content = NULL;
		m_Len     = 0;
	}else{
		m_Len     = cstring.GetLen();
		m_Content = new char[m_Len+1];
		m_Content[m_Len] = 0;
		strncpy( m_Content, cstring.GetContent(), m_Len );
	}
	InitTokenizer();
}

MyCString::MyCString(int num, int radix)
{
	char buf[CSTRING_IN_BUFFER];
	itoa(num, buf, radix);
	m_Len = strlen(buf);
	m_Content = new char[m_Len+1];
	strncpy(m_Content, buf, m_Len);
	m_Content[m_Len] = 0;
	InitTokenizer();
}

MyCString::MyCString(unsigned int num, int radix)
{
	char buf[CSTRING_IN_BUFFER];
	itoa(num, buf, radix);
	m_Len = strlen(buf);
	m_Content = new char[m_Len+1];
	strncpy(m_Content, buf, m_Len);
	m_Content[m_Len] = 0;
	InitTokenizer();
}

MyCString::MyCString(double num)
{
	char buf[CSTRING_IN_BUFFER];
	sprintf(buf, "%f", num);
	m_Len = strlen(buf);
	m_Content = new char[m_Len+1];
	strncpy(m_Content, buf, m_Len);
	m_Content[m_Len] = 0;
	InitTokenizer();
}

MyCString::MyCString(float num)
{
	char buf[CSTRING_IN_BUFFER];
	sprintf(buf, "%f", num);
	m_Len = strlen(buf);
	m_Content = new char[m_Len+1];
	strncpy(m_Content, buf, m_Len);
	m_Content[m_Len] = 0;
	InitTokenizer();
}

MyCString::MyCString(const std::string &cstring)
{
	if(cstring.empty() ){
		m_Content = NULL;
		m_Len     = 0;
	}else{
		m_Len     = cstring.length();
		m_Content = new char[m_Len+1];
		m_Content[m_Len] = 0;
		strncpy( m_Content, cstring.data(), m_Len );
	}
	InitTokenizer();
}

//this ctor copies buf_size bytes from the memory buffer
MyCString::MyCString(void* mem_buf, int buf_size)
{
	if(buf_size <= 0)
	{
		m_Content = NULL;
		m_Len     = 0;
	}else{
		m_Len     = buf_size;
		m_Content = new char[m_Len+1];
		m_Content[m_Len] = 0;
		memcpy( m_Content, mem_buf, m_Len );
	}
	InitTokenizer();
}

MyCString::~MyCString()
{
	if( !this->IsNull() )
		this->Reset();
}


ostream& operator<<(ostream& out, const MyCString& str)
{
	out<< str.GetContent();
	return out;
}

istream& operator>>(istream& in, MyCString& dest)
{
	//we do not know how big the istream will be, so we will do it in chunks
	//TBC!!
	char buf[CSTRING_IN_BUFFER];
	in >> buf;
	dest.AppendContent(buf);
	return in;
}

char MyCString::operator [](int itr) const
{
	if( (itr < 0) || (itr > m_Len-1) )
		return 0;
	else
		return m_Content[itr];
}

const MyCString& MyCString::operator=(const MyCString &str)
{
	if(&str != this)
	{
		this->Reset();
		SetContent(str);
	}

	return *this;
}

const MyCString& MyCString::operator=(const char* str)
{
	this->Reset();
	AppendContent(str);
	return *this;
}

const MyCString& MyCString::operator=(int num)
{
	this->Reset();
	AppendContent(num);
	return *this;
}

const MyCString& MyCString::operator=(unsigned int num)
{
	this->Reset();
	AppendContent(num);
	return *this;
}

const MyCString& MyCString::operator=(float num)
{
	this->Reset();
	AppendContent(num);
	return *this;
}

const MyCString& MyCString::operator=(double num)
{
	this->Reset();
	AppendContent(num);
	return *this;
}


int  MyCString::operator==(const MyCString& str)
{
	if( (IsNull() && !str.IsNull()) || (!IsNull() && str.IsNull()) )
		return 0;
	if( IsNull() && str.IsNull() )
		return 1;

	if(strcmp(GetContent(), str.GetContent()) == 0)
		return 1;
	else
		return 0;
}

int  MyCString::operator==(const char* char_str)
{
	MyCString str(char_str);
	if( (IsNull() && !str.IsNull()) || (!IsNull() && str.IsNull()) )
		return 0;
	if( IsNull() && str.IsNull() )
		return 1;

	if(strcmp(GetContent(), str.GetContent()) == 0)
		return 1;
	else
		return 0;
}

int  MyCString::operator==(int int_num)
{
	MyCString str(int_num);
	if( (IsNull() && !str.IsNull()) || (!IsNull() && str.IsNull()) )
		return 0;
	if( IsNull() && str.IsNull() )
		return 1;

	if(strcmp(GetContent(), str.GetContent()) == 0)
		return 1;
	else
		return 0;
}

int  MyCString::operator!=(const MyCString& str)
{
	if( (IsNull() && !str.IsNull()) || (!IsNull() && str.IsNull()) )
		return 0;
	if( IsNull() && str.IsNull() )
		return 1;

	if(strcmp(GetContent(), str.GetContent()) == 0)
		return 0;
	else
		return 1;
}

int  MyCString::operator!=(const char* char_str)
{
	MyCString str(char_str);
	if( (IsNull() && !str.IsNull()) || (!IsNull() && str.IsNull()) )
		return 0;
	if( IsNull() && str.IsNull() )
		return 1;

	if(strcmp(GetContent(), str.GetContent()) == 0)
		return 0;
	else
		return 1;
}

int  MyCString::operator!=(int int_num)
{
	MyCString str(int_num);
	if( (IsNull() && !str.IsNull()) || (!IsNull() && str.IsNull()) )
		return 0;
	if( IsNull() && str.IsNull() )
		return 1;

	if(strcmp(GetContent(), str.GetContent()) == 0)
		return 0;
	else
		return 1;
}

MyCString MyCString::operator+(const MyCString& str)
{
	MyCString tmp(this->GetContent());
	tmp.AppendContent(str);
	return tmp;
}

MyCString MyCString::operator+(const char* str)
{
	MyCString tmp(this->GetContent());
	tmp.AppendContent(str);
	return tmp;
}

MyCString MyCString::operator+(int num)
{
	MyCString tmp(this->GetContent());
	tmp.AppendContent(num);
	return tmp;
}

MyCString MyCString::operator+(unsigned int num)
{
	MyCString tmp(this->GetContent());
	tmp.AppendContent(num);
	return tmp;
}


MyCString MyCString::operator+(float num)
{
	MyCString tmp(this->GetContent());
	tmp.AppendContent(num);
	return tmp;
}

MyCString MyCString::operator+(double num)
{
	MyCString tmp(this->GetContent());
	tmp.AppendContent(num);
	return tmp;
}

const MyCString& MyCString::operator+=(const MyCString& str)
{
	AppendContent(str);
	return *this;
}

const MyCString& MyCString::operator+=(const char* str)
{
	AppendContent(str);
	return *this;
}

const MyCString& MyCString::operator+=(int num)
{
	AppendContent(num);
	return *this;
}

const MyCString& MyCString::operator+=(unsigned int num)
{
	AppendContent(num);
	return *this;
}

const MyCString& MyCString::operator+=(float num)
{
	AppendContent(num);
	return *this;
}

const MyCString& MyCString::operator+=(double num)
{
	AppendContent(num);
	return *this;
}


int  MyCString::IsNull() const
{
	return (m_Len == 0) ? 1 : 0;	
}

int MyCString::GetLen() const
{
	return m_Len;
}

void  MyCString::SetContent(const MyCString &str)
{
	SetContent(str.GetContent() );	
}

void  MyCString::SetContent(const char* str)
{
	//delete previous content if exists
	this->Reset();
	//set new content
	if(str != NULL){
		m_Len = strlen(str);
		m_Content = new char[m_Len+1];
		m_Content[m_Len] = 0;
		strncpy(m_Content, str, m_Len);
	}
}


void MyCString::AppendContent(const MyCString &str)
{
	int str_len;
	char* new_str=NULL;

	if(str.IsNull() ){
		return;
	}else{
		//this is empty- like creating a new string
		if( this->IsNull() ){
			SetContent(str);
			return;
		}		
		//append to an existing string
		str_len = str.GetLen();	
		new_str = new char[m_Len + str_len + 1];
		strncpy(new_str       , m_Content, m_Len);    //copy current string to new allocated string
		strncpy(new_str+m_Len, str.GetContent() , str_len+1);//append the new string & terminating NULL
	
		delete(m_Content);
		m_Content = new_str; 
		m_Len    += str_len;
	}	
}

void MyCString::AppendContent(const char* str, int len)
{
	int str_len;
	char* new_str=NULL;
	//don't append a NULL string
	if(str == NULL)
		return;
	//"this" string is empty- so this is like creating a new string
	if( this->IsNull() ){
		SetContent(str);
		return;
	}
	//append to an existing string
	if(len < 0)
		str_len = strlen(str);	
	else
		str_len = len;
	new_str = new char[m_Len + str_len + 1];
	strncpy(new_str       , m_Content, m_Len);    //copy current string to new allocated string
	strncpy(new_str+m_Len, str       , str_len);//append the new string & terminating NULL
	new_str[m_Len + str_len] = 0;
	
	delete(m_Content);
	m_Content = new_str; 
	m_Len    += str_len;
}

void MyCString::AppendContent(int num)
{
	char num_str[CSTRING_IN_BUFFER]={0};
	itoa(num, num_str, 10);
	AppendContent(num_str);	
}

void MyCString::AppendContent(unsigned int num)
{
	char num_str[CSTRING_IN_BUFFER]={0};
	itoa(num, num_str, 10);
	AppendContent(num_str);	
}

void MyCString::AppendContent(float num)
{
	MyCString tmp(num);
	AppendContent(tmp);	
}

void MyCString::AppendContent(double num)
{
	MyCString tmp(num);
	AppendContent(tmp);	
}

char* MyCString::GetContent() const
{
	return m_Content;
}


void MyCString::Reset()
{
	if(!this->IsNull() ){
		delete m_Content;
		m_Len = 0;
	}
	ResetTokenizer();
}


void MyCString::DeletePrefix(const MyCString& delims)
{
	char* new_content = NULL;
	int itr=0, itr_d=0, no_match=1;

	if( this->IsNull() || delims.IsNull() )
		return;
	
	//go over m_Content, and compare against all delims until a match is found
	for(itr=0; itr < m_Len; itr++)
	{
		no_match=1;
		itr_d=0;
		while( no_match && itr_d < delims.GetLen() )
		{
			if(m_Content[itr] == delims[itr_d])
				no_match=0;
			itr_d++;
		}
		//no match was found && you reached to the end of the delims array? Then this is a non delim char!
		if(no_match && itr_d == delims.GetLen())
			break;
	}

	//there were no delims in the beginning
	if(itr == 0)
		return;
	//the whole string was composed of delims
	if(itr == m_Len)
	{
		this->Reset();
		return;
	}
	//there were some delims- so copy only part of the string
	new_content = new char[m_Len- itr + 1];
	strncpy(new_content, &m_Content[itr], m_Len - itr);
	new_content[m_Len- itr] = 0;

	delete(m_Content);
	m_Content = new_content;
	m_Len     = m_Len- itr;
}

/*
 *	Will set m_Delims & m_NumDelims with the right data
 *  Returns number of delimiters read (if got valid delims), -1 otherwise
 *
 *  delims should be a NULL terminated string
 */
int MyCString::Tokenize(const char* delims)
{
	if(delims == NULL) 
		return -1;
	
	ResetTokenizer();
	
	while(delims[m_NumDelims] != 0)
		m_NumDelims++;

	m_Delims = new char[m_NumDelims];
	strncpy(m_Delims, delims, m_NumDelims);

	return m_NumDelims;
}


void MyCString::InitTokenizer()
{
	m_Delims       = 0;
	m_NumDelims    = 0;   
	m_NextTokenIdx = 0;
	m_LastToken    = 0;
}

void MyCString::ResetTokenizer()
{
	if(m_NumDelims != 0)
		delete(m_Delims);
	if(m_LastToken != 0)
		delete(m_LastToken);

	m_Delims       = 0;
	m_NumDelims    = 0;   
	m_NextTokenIdx = 0;
	m_LastToken    = 0;
}

/*
 *	Returns a pointer to the next token, NULL if there's none
 */
char* MyCString::GetNextToken()
{
	if(m_Len <= 0 || m_NextTokenIdx == m_Len)
		return NULL;

	/*int found_not_delim = 0;
	int start_index = m_NextTokenIdx;
	int end_index   = m_NextTokenIdx;
	//special case: check if the string does not start with a delimiter
	while(end_index < m_Len && !found_not_delim)
	{
		for(int i=0; i < m_NumDelims; i++)
		{
			if(m_Content[end_index] =! m_Delims[i])
			{
				found_not_delim |= 1;
				break;
			}
		}
		end_index++;
	}

	int found_delim = 0;
	    start_index = end_index;
		*/
	int found_delim = 0;
	int start_index = m_NextTokenIdx;
	int end_index   = m_NextTokenIdx;
	//find the first delimiter
	while(end_index < m_Len && !found_delim)
	{
		for(int i=0; i < m_NumDelims; i++)
		{
			if(m_Content[end_index] == m_Delims[i])
			{
				found_delim = 1;
				break;
			}
		}
		//the character matched none of the delimiters, so move to the next char
		end_index++;
	}

	//when exiting the loop, end_index will be on the delimiter
	int token_len;
	if(end_index == m_Len)
		token_len = end_index - start_index;
	else
		token_len = (end_index-1) - start_index;
	m_NextTokenIdx = end_index;
	if(m_LastToken != NULL) 
		delete(m_LastToken);
	m_LastToken = new char[token_len+1];
	strncpy(m_LastToken, &m_Content[start_index], token_len);
	m_LastToken[token_len] = 0;
	return m_LastToken;
}

int MyCString::DoesContain(const char* pattern)
{
	MyCString tmp(pattern);
	return DoesContain(tmp);
}

int MyCString::DoesContain(const MyCString& pattern)
{
	if(strstr(m_Content, pattern.GetContent()) == NULL)
		return 0;
	else
		return 1;
}
