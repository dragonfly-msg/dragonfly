// Serializer.NET.h
//
// Serializes and de-serializes .NET data structures for transmission as a byte stream.
//
// Meel Velliste 8/20/2008
// Copyright (c) 2008 by Meel Velliste, University of Pittsburgh. All rights reserved.

#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Reflection;

namespace Dragonfly {

/*
#define TRY try
#define RETHROW_EXCEPTION( ErrMessage) \
		catch( MyCException &E) { \
			MyCString err( (ErrMessage)); \
			E.AppendTraceToString(err); \
			throw MyCException( err); \
		} \
		catch( Exception ^e) { \
		} \
		catch( ...) { \
			MyCString err( (ErrMessage)); \
			throw MyCException( err + "Unknown exception"); \
		}
*/

	public ref class Serializer
	{
	public:
		//
		// CountDataBytes - Returns how many bytes this data object would take up in serial form.
		//                  Use this to pre-allocate a buffer for serializing the data.
		//
		int CountDataBytes( Object ^Data)
		{
		//TRY {

			int NumElements, ElementSize, NumDataBytes;
			Object ^Field;
			Object ^Element;
			int NumFields;
			int i;
			int TotalDataBytes = 0;

			// If data is an intrinsic type then we get its size from a look-up table
			Type ^dataType = Data->GetType( );
			if( dataType->IsPrimitive) {

				TotalDataBytes = SizeOf( dataType);
		        
			} else { // Else data is a tricky type

				// Tricky types come in two flavors: either strings or general objects
				TypeCode ^typeCode = Type::GetTypeCode( dataType);
				switch( *typeCode) {
					// If data is a string, size is the length of the string (unicode gets translated to ASCII)
					case TypeCode::String: {
						String ^s = safe_cast<String^>( Data);
						NumElements = s->Length;
						TotalDataBytes = sizeof(char) * NumElements;
						break; }
					// If data is a general object, it can be an array or a struct
					case TypeCode::Object:
						if( dataType->IsArray) {
							Type ^elementType = dataType->GetElementType( );
							Array ^a = (Array^) Data;
							if( a->Rank > 1) throw gcnew Exception( "Multi-dimensional arrays not implemented yet");
							NumElements = a->Length;
							if( elementType->IsPrimitive) {
							// If data is an array with intrinsic elements, multiply element size by number of elements
								ElementSize = SizeOf( elementType);
								TotalDataBytes = NumElements * ElementSize;
							} else { 
							// If data is an array of objects, then get size of each element and add them up
								cli::array<Object^> ^objArray = safe_cast<cli::array<Object^>^>( a);
								for( i = 0; i < NumElements; i++) {
									Element = objArray[i];
									NumDataBytes = CountDataBytes( Element);
									TotalDataBytes += NumDataBytes;
								}
							}
						} else { // If data is an non-array object, then treat it as a struct and get the size of each field and add them up (non-data fields such as methods and pointers are ignored)
							cli::array<FieldInfo^> ^fieldInfo = dataType->GetFields();
							NumFields = fieldInfo->Length;
							for( i = 0; i < NumFields; i++) {
								Type ^fieldType = fieldInfo[i]->GetType();
								if( fieldType->IsPrimitive) {
									NumDataBytes = SizeOf(fieldType);
								} else {
									Field = fieldInfo[i]->GetValue( Data);
									NumDataBytes = CountDataBytes( Field);
								}
								TotalDataBytes += NumDataBytes;
							}
						}
						break;
					default:
						// If data is not a string or an object, then it is an unsupported type
						throw gcnew Exception( "Unsupported data type");
						break;
				}

			}
			return TotalDataBytes;

		//} RETHROW_EXCEPTION( "Error in Serializer::CountDataBytes(): ")
		}

		//
		// Pull out data from a .NET object and write it serially into a contiguous memory buffer
		// and check that the number of bytes extracted matches the expected NumDataBytes.
		//
		void SerializeData( Object ^Data, void *pOutputBuffer, int NumDataBytes)
		{
			void *pEndData = SerializeData( Data, pOutputBuffer);

			int CheckSum = (char *) pEndData - (char *) pOutputBuffer;
			if( CheckSum != NumDataBytes) throw gcnew Exception( "Wrong number of bytes serialized");
		}


		//
		// Fill a .NET object with data from a serial memory buffer
		// and check that the number of bytes filled matches the expected NumDataBytes.
		// The Output object serves as its own template, i.e. it needs to have the correct
		// structure when passed as an input. This function merely replaces the values in it.
		//
		void DeserializeData( Object ^%Output, void *pInput, int NumDataBytes)
		{
			int NumTemplateBytes;
			void *pEnd;
		    
			NumTemplateBytes = CountDataBytes( Output);
			if( NumTemplateBytes != NumDataBytes)
			{
				String ^err = "Number of template bytes [" + NumTemplateBytes.ToString() + "] does not match number of serial data bytes [" + NumDataBytes.ToString() + "]";
				throw gcnew Exception( err);
			}
			pEnd = DeserializeData( Output, pInput);

			int CheckSum = (char *) pEnd - (char *) pInput;
			if( CheckSum != NumDataBytes) throw gcnew Exception( "Wrong number of bytes de-serialized (" + CheckSum.ToString() + " instead of the expected " + NumDataBytes.ToString() + ")");
		}

	private:
		int SizeOf( Type ^primitiveType)
		{
			TypeCode ^typeCode = Type::GetTypeCode( primitiveType);
			switch( *typeCode) {
				case TypeCode::Double: return sizeof(System::Double);
				case TypeCode::Single: return sizeof(System::Single);
				case TypeCode::SByte: return sizeof(System::SByte);
				case TypeCode::Int16: return sizeof(System::Int16);
				case TypeCode::Int32: return sizeof(System::Int32);
				case TypeCode::Int64: return sizeof(System::Int64);
				case TypeCode::Byte: return sizeof(System::Byte);
				case TypeCode::UInt16: return sizeof(System::UInt16);
				case TypeCode::UInt32: return sizeof(System::UInt32);
				case TypeCode::UInt64: return sizeof(System::UInt64);
				case TypeCode::Boolean: return sizeof(System::Boolean);
				case TypeCode::Char: return sizeof(System::Char);
				case TypeCode::String: throw gcnew Exception( "Attempting to get size of non-primitive type 'String'");
				case TypeCode::Object: throw gcnew Exception( "Attempting to get size of non-primitive type 'Object'");
				default: throw gcnew Exception( "Unsupported data type");
			}
		}

		//
		// Pull out data from a .NET object and write it serially into a contiguous memory buffer.
		// Returns a pointer to the next byte after the last byte written.
		//
		void* SerializeData( Object ^Data, void *pOutput)
		{
			int NumElements;
			int NumFields;
			int i;
			int numBytes;
			void *pEnd;

			// If data is an intrinsic type then we just copy its value
			Type ^dataType = Data->GetType( );
			if( dataType->IsPrimitive) {

				pEnd = SerializePrimitiveValue( Data, dataType, pOutput);

			} else { // Else data is a tricky type

				// Tricky types come in two flavors: either strings or general objects
				TypeCode ^typeCode = Type::GetTypeCode( dataType);
				switch( *typeCode) {
					// If data is a string, we copy character by character and convert each one from unicode to ascii
					case TypeCode::String: {
						String ^s = safe_cast<String^>( Data);
						NumElements = s->Length;
						cli::array<Char> ^charArray = s->ToCharArray( );
						char *AsciiString = (char*) pOutput;
						for( int i = 0; i < NumElements; i++) {
							unsigned short UnicodeChar = charArray[i];
							if( UnicodeChar > 255) throw gcnew Exception( "Char value greater than 255 while trying to convert Unicode to Ascii");
							AsciiString[i] = (char) UnicodeChar;
						}
						numBytes = NumElements;
						pEnd = (void *) ((char *) pOutput + numBytes);
						break; }
					// If data is a general object, it can be an array or a struct
					case TypeCode::Object:
						if( dataType->IsArray) {
							Type ^elementType = dataType->GetElementType( );
							Array ^a = (Array^) Data;
							if( a->Rank > 1) throw gcnew Exception( "Multi-dimensional arrays not implemented yet");
							NumElements = a->Length;
							if( elementType->IsPrimitive) {
								// If data is an array with intrinsic elements, we copy its content as a contiguous memory block
								pEnd = SerializePrimitiveArray( Data, elementType, NumElements, pOutput);
							} else {
								// If data is an array of objects, then we copy each element by calling this function recursively
								cli::array<Object^> ^objArray = safe_cast<cli::array<Object^>^>( a);
								pEnd = pOutput;
								for( i = 0; i < NumElements; i++) {
									Object ^Element = objArray[i];
									pEnd = SerializeData( Element, pEnd);
								}
							}
						} else { // If data is an non-array object, then treat it as a struct and copy each field by calling this function recursively (non-data fields such as methods and pointers are ignored)
							cli::array<FieldInfo^> ^fieldInfo = dataType->GetFields();
							NumFields = fieldInfo->Length;
							pEnd = pOutput;
							for( i = 0; i < NumFields; i++) {
								Object ^Field = fieldInfo[i]->GetValue( Data);
								pEnd = SerializeData( Field, pEnd);
							}
						}
						break;
					default:
						// If data is not a string or an object, then it is an unsupported type
						throw gcnew Exception( "Unsupported data type");
						break;
				}

			}
			return pEnd;
		}

		//
		// Pull out data from a .NET object that is known to contain a single value of a primitive type.
		//
		void* SerializePrimitiveValue( Object ^Data, Type ^primitiveType, void *pOutput)
		{
			void *pEnd;
			int numBytes = SizeOf( primitiveType);
			TypeCode ^typeCode = Type::GetTypeCode( primitiveType);
			switch( *typeCode) {
				case TypeCode::Double:  {Double *pOut = (Double*) pOutput; *pOut = safe_cast<Double>( Data); break;}
				case TypeCode::Single:  {Single *pOut = (Single*) pOutput; *pOut = safe_cast<Single>( Data); break;}
				case TypeCode::SByte:   {SByte *pOut = (SByte*) pOutput; *pOut = safe_cast<SByte>( Data); break;}
				case TypeCode::Int16:   {Int16 *pOut = (Int16*) pOutput; *pOut = safe_cast<Int16>( Data); break;}
				case TypeCode::Int32:   {Int32 *pOut = (Int32*) pOutput; *pOut = safe_cast<Int32>( Data); break;}
				case TypeCode::Int64:   {Int64 *pOut = (Int64*) pOutput; *pOut = safe_cast<Int64>( Data); break;}
				case TypeCode::Byte:    {Byte *pOut = (Byte*) pOutput; *pOut = safe_cast<Byte>( Data); break;}
				case TypeCode::UInt16:  {UInt16 *pOut = (UInt16*) pOutput; *pOut = safe_cast<UInt16>( Data); break;}
				case TypeCode::UInt32:  {UInt32 *pOut = (UInt32*) pOutput; *pOut = safe_cast<UInt32>( Data); break;}
				case TypeCode::UInt64:  {UInt64 *pOut = (UInt64*) pOutput; *pOut = safe_cast<UInt64>( Data); break;}
				case TypeCode::Boolean: {Boolean *pOut = (Boolean*) pOutput; *pOut = safe_cast<Boolean>( Data); break;}
				case TypeCode::Char:    {Char *pOut = (Char*) pOutput; *pOut = safe_cast<Char>( Data); break;}
				case TypeCode::String: throw gcnew Exception( "Attempting to access non-primitive type 'String' as primitive");
				case TypeCode::Object: throw gcnew Exception( "Attempting to access non-primitive type 'Object' as primitive");
				default: throw gcnew Exception( "Unsupported data type");
			}
			pEnd = (void *) ((char *) pOutput + numBytes);
			return pEnd;
		}

		//
		// Pull out data from a .NET object that is known to contain an array of a primitive type.
		//
		void* SerializePrimitiveArray( Object ^Data, Type ^primitiveType, int numElements, void *pOutput)
		{
			void *pEnd;
			int elementSize = SizeOf( primitiveType);
			int numBytes = numElements * elementSize;
			TypeCode ^typeCode = Type::GetTypeCode( primitiveType);
			switch( *typeCode) {
				case TypeCode::Double:  {cli::array<Double> ^source = safe_cast<cli::array<Double>^>( Data); pin_ptr<Double> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::Single:  {cli::array<Single> ^source = safe_cast<cli::array<Single>^>( Data); pin_ptr<Single> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::SByte:   {cli::array<SByte> ^source = safe_cast<cli::array<SByte>^>( Data); pin_ptr<SByte> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::Int16:   {cli::array<Int16> ^source = safe_cast<cli::array<Int16>^>( Data); pin_ptr<Int16> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::Int32:   {cli::array<Int32> ^source = safe_cast<cli::array<Int32>^>( Data); pin_ptr<Int32> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::Int64:   {cli::array<Int64> ^source = safe_cast<cli::array<Int64>^>( Data); pin_ptr<Int64> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::Byte:    {cli::array<Byte> ^source = safe_cast<cli::array<Byte>^>( Data); pin_ptr<Byte> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::UInt16:  {cli::array<UInt16> ^source = safe_cast<cli::array<UInt16>^>( Data); pin_ptr<UInt16> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::UInt32:  {cli::array<UInt32> ^source = safe_cast<cli::array<UInt32>^>( Data); pin_ptr<UInt32> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::UInt64:  {cli::array<UInt64> ^source = safe_cast<cli::array<UInt64>^>( Data); pin_ptr<UInt64> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::Boolean: {cli::array<Boolean> ^source = safe_cast<cli::array<Boolean>^>( Data); pin_ptr<Boolean> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::Char:    {cli::array<Char> ^source = safe_cast<cli::array<Char>^>( Data); pin_ptr<Char> pSource = &source[0]; memcpy( pOutput, pSource, numBytes); break;}
				case TypeCode::String: throw gcnew Exception( "Attempting to access array of 'String' objects as primitive array");
				case TypeCode::Object: throw gcnew Exception( "Attempting to access array of 'Object' type as primitive array");
				default: throw gcnew Exception( "Unsupported data type");
			}
			pEnd = (void *) ((char *) pOutput + numBytes);
			return pEnd;
		}


		//
		// Fill a .NET object with data from a serial memory buffer.
		// The Output object serves as its own template, i.e. it needs to have the correct
		// structure when passed as an input. This function merely replaces the values in it.
		// Returns a pointer to the next byte after the last byte read from the input buffer.
		//
		void* DeserializeData( Object ^%Output, void *pInput)
		{
			int NumElements;
			int NumFields;
			int i;
			void *pEnd;

			// If data is an intrinsic type then we assign its value
			Type ^dataType = Output->GetType( );
			if( dataType->IsPrimitive) {

				pEnd = DeserializePrimitiveValue( Output, dataType, pInput);

			} else { // Else data is a tricky type

				// Tricky types come in two flavors: either strings or general objects
				TypeCode ^typeCode = Type::GetTypeCode( dataType);
				switch( *typeCode) {
					// If data is a string, we copy character by character and convert each one from ascii to unicode
					case TypeCode::String: {
						String ^template_str = safe_cast<String^>( Output);
						int numChars = template_str->Length;
						String ^output_str = gcnew String( (const char*)pInput, 0, numChars);
						pEnd = (void *) ((char *) pInput + numChars);
						break; }
					// If data is a general object, it can be an array or a struct
					case TypeCode::Object:
						if( dataType->IsArray) {
							Type ^elementType = dataType->GetElementType( );
							Array ^a = safe_cast<Array^>( Output);
							if( a->Rank > 1) throw gcnew Exception( "Multi-dimensional arrays not implemented yet");
							NumElements = a->Length;
							if( elementType->IsPrimitive) {
								// If data is an array with intrinsic elements, we copy content into it directly as a memory block copy
								pEnd = DeserializePrimitiveArray( Output, elementType, NumElements, pInput);
							} else {
								// If data is an array of objects, then we copy each element by calling this function recursively
								cli::array<Object^> ^%objArray = safe_cast<cli::array<Object^>^%>( a);
								pEnd = pInput;
								for( i = 0; i < NumElements; i++) {
									Object ^Element = objArray[i];
									pEnd = DeserializeData( Element, pEnd);
									objArray[i] = Element;
								}
							}
						} else { // If data is an non-array object, then treat it as a struct and copy each field by calling this function recursively (non-data fields such as methods and pointers are ignored)
							cli::array<FieldInfo^> ^fieldInfo = dataType->GetFields();
							NumFields = fieldInfo->Length;
							pEnd = pInput;
							for( i = 0; i < NumFields; i++) {
								Object ^Field = fieldInfo[i]->GetValue( Output);
								pEnd = DeserializeData( Field, pEnd);
								fieldInfo[i]->SetValue( Output, Field);
							}
						}
						break;
					default:
						// If data is not a string or an object, then it is an unsupported type
						throw gcnew Exception( "Unsupported data type");
						break;
				}

			}

			return pEnd;
		}

		//
		// Put data into a .NET object that is known to contain a single value of a primitive type.
		//
		void* DeserializePrimitiveValue( Object ^%Output, Type ^primitiveType, void *pInput)
		{
			void *pEnd;
			int numBytes = SizeOf( primitiveType);
			TypeCode ^typeCode = Type::GetTypeCode( primitiveType);
			switch( *typeCode) {
				case TypeCode::Double:  {Double *pIn = (Double*) pInput; Output = *pIn; break;}
				case TypeCode::Single:  {Single *pIn = (Single*) pInput; Output = *pIn; break;}
				case TypeCode::SByte:   {SByte *pIn = (SByte*) pInput; Output = *pIn; break;}
				case TypeCode::Int16:   {Int16 *pIn = (Int16*) pInput; Output = *pIn; break;}
				case TypeCode::Int32:   {Int32 *pIn = (Int32*) pInput; Output = *pIn; break;}
				case TypeCode::Int64:   {Int64 *pIn = (Int64*) pInput; Output = *pIn; break;}
				case TypeCode::Byte:    {Byte *pIn = (Byte*) pInput; Output = *pIn; break;}
				case TypeCode::UInt16:  {UInt16 *pIn = (UInt16*) pInput; Output = *pIn; break;}
				case TypeCode::UInt32:  {UInt32 *pIn = (UInt32*) pInput; Output = *pIn; break;}
				case TypeCode::UInt64:  {UInt64 *pIn = (UInt64*) pInput; Output = *pIn; break;}
				case TypeCode::Boolean: {Boolean *pIn = (Boolean*) pInput; Output = *pIn; break;}
				case TypeCode::Char:    {Char *pIn = (Char*) pInput; Output = *pIn; break;}
				case TypeCode::String: throw gcnew Exception( "Attempting to access non-primitive type 'String' as primitive");
				case TypeCode::Object: throw gcnew Exception( "Attempting to access non-primitive type 'Object' as primitive");
				default: throw gcnew Exception( "Unsupported data type");
			}
			pEnd = (void *) ((char *) pInput + numBytes);
			return pEnd;
		}

		//
		// Pull data into a .NET object that is known to contain an array of a primitive type.
		//
		void* DeserializePrimitiveArray( Object ^%Output, Type ^primitiveType, int numElements, void *pInput)
		{
			void *pEnd;
			int elementSize = SizeOf( primitiveType);
			int numBytes = numElements * elementSize;
			TypeCode ^typeCode = Type::GetTypeCode( primitiveType);
			switch( *typeCode) {
				case TypeCode::Double:   {cli::array<Double> ^output = safe_cast<cli::array<Double>^>( Output); pin_ptr<Double> pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::Single:   {cli::array<Single> ^output = safe_cast<cli::array<Single>^>( Output); pin_ptr<Single> pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::SByte:    {cli::array<SByte>  ^output = safe_cast<cli::array<SByte>^>( Output);  pin_ptr<SByte>  pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::Int16:    {cli::array<Int16>  ^output = safe_cast<cli::array<Int16>^>( Output);  pin_ptr<Int16>  pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::Int32:    {cli::array<Int32>  ^output = safe_cast<cli::array<Int32>^>( Output);  pin_ptr<Int32>  pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::Int64:    {cli::array<Int64>  ^output = safe_cast<cli::array<Int64>^>( Output);  pin_ptr<Int64>  pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::Byte:     {cli::array<Byte>   ^output = safe_cast<cli::array<Byte>^>( Output);   pin_ptr<Byte>   pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::UInt16:   {cli::array<UInt16> ^output = safe_cast<cli::array<UInt16>^>( Output); pin_ptr<UInt16> pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::UInt32:   {cli::array<UInt32> ^output = safe_cast<cli::array<UInt32>^>( Output); pin_ptr<UInt32> pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::UInt64:   {cli::array<UInt64> ^output = safe_cast<cli::array<UInt64>^>( Output); pin_ptr<UInt64> pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::Boolean:  {cli::array<Boolean> ^output = safe_cast<cli::array<Boolean>^>( Output); pin_ptr<Boolean> pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::Char:     {cli::array<Char>   ^output = safe_cast<cli::array<Char>^>( Output);   pin_ptr<Char>   pOutput = &output[0]; memcpy( pOutput, pInput, numBytes); break;}
				case TypeCode::String: throw gcnew Exception( "Attempting to access array of 'String' objects as primitive array");
				case TypeCode::Object: throw gcnew Exception( "Attempting to access array of 'Object' type as primitive array");
				default: throw gcnew Exception( "Unsupported data type");
			}
			pEnd = (void *) ((char *) pInput + numBytes);
			return pEnd;
		}

	};
}
