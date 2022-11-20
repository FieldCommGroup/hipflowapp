/*************************************************************************************************
 * Copyright 2019-2021 FieldComm Group, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description:
 *		This holds the implementation code for the HCF variant CValueVarient 
 *	
 *		25jun08	sjv	creation from the varient.h file
*/
#define  __cplusplus  201103L   /* temp */
#include "varient.h"
#include <wchar.h>
#include <math.h>
#include <limits>
#include <cmath>
#include <locale>
#include <codecvt>

#define SCAN_FUNC  		sscanf	/* no safe function for this, manually inspect usage */

#define FORTH_PARAM  

#ifndef UINT64
#define UINT64  unsigned long long
#endif
#ifndef INT64
#define INT64  long long
#endif
#ifndef FLT_DIG
#define FLT_DIG          6                       // # of decimal digits of precision
#endif

// 0x36a0000000000000 is one float...1.40130e-045...3,936,146,074,321,813,504 double Ulps
// 4.9406564584125e-323 is 10 double epsilons
bool AlmostEqual2sComplement(double A, double B, int maxUlps)
{   // Make sure maxUlps is non-negative and small enough that the

	// default NAN won't compare as equal to anything.

	if (!(maxUlps > 0 && maxUlps < 4 * 1024))// make sure we are sane
		throw 5;

	long long aInt = *(long long*)&A;	// float to int (see bit pattern above)

	// Make aInt lexicographically ordered using a twos-complement int
	if (aInt < 0)
		aInt = 0x8000000000000000 - aInt;

	long long bInt = *(long long*)&B;	// float to int (see bit pattern above)

	// Make bInt lexicographically ordered using a twos-complement int
	if (bInt < 0)
		bInt = 0x8000000000000000 - bInt;

	// aInt and bInt are conceptually the number of FLT_EPSILONs in the values

#if 1 /*sjv 09may07  was:: _MSC_VER >= 1300  // HOMZ - port to 2003, VS 7 */
	// error C2668: 'abs' : ambiguous call to overloaded function
	// SOLUTION - added double logic
	double  dblDiff = double(aInt - bInt);// WS - 9apr07 - VS2005 checkin
	long long intDiff = (long long)fabs(dblDiff);  // sjv 09may07 abs to fabs.
											   // ...absolute value of the difference
#else
	long long intDiff = abs(aInt - bInt);
#endif

	if (intDiff <= maxUlps)// number of FLT_EPSILONs difference is within spec'ed difference
		return true;

	return false;
}

// utf8 to wide
wstring s2ws(const std::string& str)
{
	wstring retval;
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	try
	{
		retval = converterX.from_bytes(str);
	}
	catch (range_error &e)
	{// this can happen inside a LOGIT so logging again doesn't work very well...
		//LOGIT(CERR_LOG|CLOG_LOG,"ERROR: Caught exception in s2ws():%s\n",e.what());
		string h = " ERROR: Caught exception in String to Wide String:";
		h += e.what();
		//#ifdef TOKENIZER
		fprintf(stderr, "%s.\nString:%s\n", h.c_str(), str.c_str());	// missing .c_str() caused a crash [12/20/2018 tjohnston]
//#else
//	this can happen within a logout, giving a mutex deadlock	LOGIT(CERR_LOG,"%s.\n",h.c_str());
//#endif	
	}
	return retval;
}

// wide to utf8
string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}


CValueVarient::CValueVarient() : vSize(0),vIsValid(false),vType(invalid),vIsUnsigned(false),
												   vIsDouble(false),vIndex(0),vIsBit(false)	
{ vValue.fFloatConst=0.0;sWideStringVal.clear();sStringVal.clear();
};
CValueVarient::CValueVarient(const CValueVarient& src) : vSize(0),vIsValid(false),vType(invalid),
			vIsUnsigned(false), vIsDouble(false),vIndex(0),vIsBit(false)	 { 
				(*this) = src; };
CValueVarient::~CValueVarient(){clear();};

/*----------------------------------------------------------------------------------------------*/

void CValueVarient::clear(void)
{
	vType     = invalid;	vValue.fFloatConst = 0.0;
	vSize     = 0;			vIsValid           = false;		vIsUnsigned = false;
	vIsDouble = false;		vIndex             = 0;			vIsBit      = false;
	// erasing empty strings cause crashes
	if (! sWideStringVal.empty())
		sWideStringVal.erase();
	if (! sStringVal.empty())	
		sStringVal.erase();
}

bool CValueVarient::valueIsZero(void) 
{   
	if      (vType == isBool)       return ((vValue.bIsTrue)          ?false:true);
	else if (vType == isIntConst)   return ((vValue.iIntConst   == 0) ? true:false);
	else if (vType == isVeryLong)   return ((vValue.longlongVal == 0) ? true:false);
	else if (vType == isFloatConst) 
					return (AlmostEqual2sComplement(vValue.fFloatConst,(double)0.0, 4)?true:false);
	else return (true);
}

bool CValueVarient::isNumeric()
{	
	if ( (vType == isBool)       || (vType == isIntConst)  || 
		 (vType == isFloatConst) || (vType == isVeryLong)  )
	{	return true;
	}
	else
	{	return false;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//  CAST operators
/////////////////////////////////////////////////////////////////////////////////////////////////
CValueVarient::operator bool(void)
{	bool bVal;

	switch(vType)
	{
	case isBool:
		{	bVal = vValue.bIsTrue;  }							break;
	case isIntConst:
		{	bVal = ((vValue.iIntConst)? true : false);}			break;
	case isVeryLong:
		{	bVal = ((vValue.longlongVal)? true : false); }		break;
	case isFloatConst:
		{		bVal = ((vValue.fFloatConst)? true : false); }	break;
	case isString:
		{	bVal = ((sStringVal.size())? true : false); }		break;
	case isWideString:
		{	bVal = ((sWideStringVal.size())? true : false); }	break;
	case invalid:
	case isDepIndex:
	default:
		{
			bVal = false; // error.
		}
	}
	return bVal;
}

CValueVarient::operator char(void)
{	
	char cVal;

	switch(vType)
	{
	case isBool:
		{	cVal = ((vValue.bIsTrue)? 1 : 0); } break;
	case isIntConst:
		{
			if (vIsUnsigned)
			{
				cVal = ((unsigned int)vValue.iIntConst);      // auto cast to char
			}
			else
			{
				cVal = vValue.iIntConst;      // auto cast to char
			}
		} break;
	case isVeryLong:
		{
			if ( vIsUnsigned )
			{
				cVal = (unsigned int)vValue.longlongVal;  // auto cast to char
			}
			else
			{
				cVal = (int) vValue.longlongVal;    // auto cast to char
			}
		}	
		break;
	case isFloatConst:
		{	cVal =  (char) vValue.fFloatConst;} break;
	case isString:
		{
			// No vulnerability by inspection: no user-accessible parameters
			if ( SCAN_FUNC( sStringVal.c_str(), "%hhd", &cVal FORTH_PARAM ) <= 0) 
			{  cVal = 0;  }
		} break;
	case isWideString:
		{
			// No vulnerability by inspection: user cannot enter the format
			if ( swscanf(sWideStringVal.c_str(),L"%hhd",&cVal) <= 0) {  cVal = 0;  }
		} break;

	case invalid:
	case isDepIndex:
	default:
		{
			cVal = 0; // error.
		}
	}
	return cVal;
}

CValueVarient::operator unsigned char(void)
{	
	unsigned char uVal;

	switch(vType)
	{
	case isBool:
		{	uVal = ((vValue.bIsTrue)? 1 : 0); }			break;
	case isIntConst:
		{	uVal = (unsigned char)  vValue.iIntConst;}	break;
	case isVeryLong:
		{
			if (vIsUnsigned)
			{
				uVal = (unsigned int)vValue.longlongVal; // auto cast to unsigned char
			}
			else
			{
				uVal = (int) vValue.longlongVal;  // auto cast to unsigned
			}
		}
		break;
	case isFloatConst:
		{	uVal =  (unsigned char) vValue.fFloatConst;} break;
	case isString:
		{
			// No vulnerability by inspection: no user-accessible parameters
			if ( SCAN_FUNC( sStringVal.c_str(), "%hhu", &uVal FORTH_PARAM ) <= 0) 
			{  uVal = 0;  }
		} break;
	case isWideString:
		{
			// No vulnerability by inspection: user cannot enter the format
			if ( swscanf(sWideStringVal.c_str(),L"%hhu",&uVal) <= 0) {  uVal = 0;  }
		} break;

	case invalid:
	case isDepIndex:
	default:
		{
			uVal = 0; // error.
		}
	}
	return uVal;
}

CValueVarient::operator short(void)
{	
	short cVal;

	switch(vType)
	{
	case isBool:
		{	cVal = ((vValue.bIsTrue)? 1 : 0); } break;
	case isIntConst:
		{
			if (vIsUnsigned)
			{
				cVal = ((unsigned int)vValue.iIntConst);      // auto cast to short
			}
			else
			{
				cVal = vValue.iIntConst;      // auto cast to short
			}
		} break;
	case isVeryLong:
		{
			if ( vIsUnsigned)
			{
				cVal = (unsigned int)vValue.longlongVal;  // auto cast to short
			}
			else
			{
				cVal = (int) vValue.longlongVal;    // auto cast to short
			}
		}
		break;
	case isFloatConst:
		{	cVal =  (short) vValue.fFloatConst;} break;
	case isString:
		{ 
			// No vulnerability by inspection: no user-accessible parameters
			if ( SCAN_FUNC( sStringVal.c_str(), "%hd", &cVal FORTH_PARAM ) <= 0) 
			{  cVal = 0;  }
		} break;
	case isWideString:
		{
			// user cannot enter the format
			if ( swscanf(sWideStringVal.c_str(),L"%hd",&cVal) <= 0) {  cVal = 0;  }
		} break;

	case invalid:
	case isDepIndex:
	default:
		{
			cVal = 0; // error.
		}
	}
	return cVal;
}

CValueVarient::operator unsigned short(void)
{	
	unsigned short uVal;

	switch(vType)
	{
	case isBool:
		{	uVal = ((vValue.bIsTrue)? 1 : 0); } break;
	case isIntConst:
		{	uVal = (unsigned short)  vValue.iIntConst;  } break;
	case isVeryLong:
		{
			if (vIsUnsigned)
			{
				uVal = (unsigned int)vValue.longlongVal; // auto cast to unsigned short
			}
			else
			{
				uVal = (int) vValue.longlongVal;  // auto cast to unsigned
			}
		}
		break;
	case isFloatConst:
		{	uVal =  (unsigned short) vValue.fFloatConst;} break;
	case isString:
		{
			// No vulnerability by inspection: no user-accessible parameters
			if ( SCAN_FUNC( sStringVal.c_str(), "%hu", &uVal FORTH_PARAM ) <= 0) 
			{  uVal = 0;  }
		} break;
	case isWideString:
		{
			// No vulnerability by inspection: user cannot enter the format
			if ( swscanf(sWideStringVal.c_str(),L"%hu",&uVal) <= 0) {  uVal = 0;  }
		} break;

	case invalid:
	case isDepIndex:
	default:
		{
			uVal = 0; // error.
		}
	}
	return uVal;
}

CValueVarient::operator double(void)
{	
	double fVal;

	switch(vType)
	{
	case isBool:
		{	fVal = ((vValue.bIsTrue)? 1.0 : 0.0); } break;
	case isIntConst:
		{
			if (vIsUnsigned)
			{
				fVal = (double) ((unsigned int)vValue.iIntConst);  
			}
			else
			{
				fVal = (double) vValue.iIntConst;  
			}
		} 	break;
	case isVeryLong:
		{
			if ( vIsUnsigned)
			{// bill doesn't support unsigned long long to double conversion (in VS6)
				fVal  = (double) ( (INT_LL)(vValue.longlongVal & 0x7fffffffffffffff) );

				if ( (vValue.longlongVal & 0x8000000000000000) != 0 )
				{
					fVal += 0x7fffffffffffffff;// highest positive value
					fVal += 1;// but we need another...
				}
			}
			else
			{
				fVal = (double) vValue.longlongVal;  
			}
		}
		break;
	case isFloatConst:
		{	fVal =          vValue.fFloatConst;} break;
	case isString:
		{
			// No vulnerability by inspection: no user-accessible parameters
			if ( SCAN_FUNC( sStringVal.c_str(), "%lf", &fVal FORTH_PARAM ) <= 0) 
			{  fVal = 0.0;  }
		} break;
	case isWideString:
		{
			// No vulnerability by inspection: user cannot enter the format
			if ( swscanf(sWideStringVal.c_str(),L"%lf",&fVal) <= 0) {  fVal = 0;  }
		} break;


	case invalid:
	case isDepIndex:
	default:
		{
			fVal = 0.0; // error.
		}
	}
	return fVal;
}

CValueVarient::operator float(void)
{	
	float fVal;

	switch(vType)
	{
	case isBool:
		{	fVal = (float)((vValue.bIsTrue)? 1.0 : 0.0); } break; // warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
	case isIntConst:
		{
			if (vIsUnsigned)
			{
				fVal = (float) ((unsigned int)vValue.iIntConst);  
			}
			else
			{
				fVal = (float) vValue.iIntConst;  
			}
		}	break;
	case isVeryLong:
		{
			if (vIsUnsigned)
			{
				fVal = (float) vValue.longlongVal;  
				if ( (vValue.longlongVal & 0x8000000000000000) && fVal < 0 )
				{	fVal *= -1;    fVal += 0x8000000000000000;   }
			}
			else
			{
				fVal = (float) vValue.longlongVal;  
			}
		}
		break;
	case isFloatConst:
		{	fVal =  (float)vValue.fFloatConst;} break;  // warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
	case isString:
		{// never works::	if ( sscanf_s(sStringVal.c_str(),"%f",&fVal) <= 0) {  fVal = 0.0;  }
			fVal = (float)atof(sStringVal.c_str()); // warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
		} break;
	case isWideString:
		{
			// windows  fVal = (float)_wtof(sWideStringVal.c_str());
			fVal = (float)wcstof(sWideStringVal.c_str(), NULL);
		} break;

	case invalid:
	case isDepIndex:
	default:
		{
			fVal = 0.0; // error.
		}
	}
	return fVal;
}

CValueVarient::operator int(void)
{	
	int iVal;

	switch(vType)
	{
	case isBool:
		{	iVal = ((vValue.bIsTrue)? 1 : 0); } break;
	case isIntConst:
		{
			if (vIsUnsigned)
			{
				iVal = ((unsigned int)vValue.iIntConst);  
			}
			else
			{
				iVal = vValue.iIntConst;  
			}
		} break;
	case isVeryLong:
		{
			if (vIsUnsigned)
			{
				iVal = (unsigned int)vValue.longlongVal;  
			}
			else
			{
				iVal = (int) vValue.longlongVal;  
			}
		}
		break;
	case isFloatConst:
		{	iVal =  (int) vValue.fFloatConst;} break;
	case isString:
		{
			// No vulnerability by inspection: no user-accessible parameters
			if ( SCAN_FUNC( sStringVal.c_str(), "%d", &iVal FORTH_PARAM ) <= 0) 
			{  iVal = 0;  }
		} break;
	case isWideString:
		{
			// No vulnerability by inspection: user cannot enter the format
			if ( swscanf(sWideStringVal.c_str(),L"%d",&iVal) <= 0) {  iVal = 0;  }
		} break;

	case invalid:
	case isDepIndex:
	default:
		{
			iVal = 0; // error.
		}
	}
	return iVal;
}

CValueVarient::operator unsigned int(void)
{	
	unsigned int uVal;

	switch(vType)
	{
	case isBool:
		{	uVal = ((vValue.bIsTrue)? 1 : 0); } break;
	case isIntConst:
		{
			uVal = (unsigned int)  vValue.iIntConst;  
		} break;
	case isVeryLong:
		{
			if (vIsUnsigned)
			{
				uVal = (unsigned int)vValue.longlongVal;  
			}
			else
			{
				uVal = (int) vValue.longlongVal;  // auto cast to unsigned
			}
		}
		break;
	case isFloatConst:
		{	uVal =  (unsigned int) vValue.fFloatConst;} break;
	case isString:
		{
			// No vulnerability by inspection: no user-accessible parameters
			if ( SCAN_FUNC( sStringVal.c_str(), "%u", &uVal FORTH_PARAM ) <= 0) 
			{  uVal = 0;  }
		} break;
	case isWideString:
		{
			// No vulnerability by inspection: user cannot enter the format
			if ( swscanf(sWideStringVal.c_str(),L"%u",&uVal) <= 0) {  uVal = 0;  }
		} break;

	case invalid:
	case isDepIndex:
	default:
		{
			uVal = 0; // error.
		}
	}
	return uVal;
}

CValueVarient::operator long(void)
{	
	long uVal = (int)(*this);
	return uVal;
}

CValueVarient::operator unsigned long(void)
{	
	unsigned long uVal = (unsigned int)(*this);
	return uVal;
}

CValueVarient::operator INT_LL(void)
{
	INT_LL iVal;

	switch(vType)
	{
	case isBool:
		{	iVal = ((vValue.bIsTrue)? 1 : 0); } break;
	case isIntConst:
		{
			if (vIsUnsigned)
			{
				iVal = ((unsigned int)vValue.iIntConst);   // autocast 2 longlong
			}
			else
			{
				iVal = vValue.iIntConst;    // autocast
			}
		} break;
	case isVeryLong:
		{	iVal = vValue.longlongVal;  // autocast
		}
		break;
	case isFloatConst:
		{	iVal =  (INT_LL) vValue.fFloatConst;} break;
	case isString:
		// user cannot enter the format
		{	if ( sscanf(sStringVal.c_str(),"%lld",&iVal) <= 0) {  iVal = 0;  }
		} break;
	case isWideString:
		{
			// No vulnerability by inspection: user cannot enter the format
			if ( swscanf(sWideStringVal.c_str(),L"%lld",&iVal) <= 0) {  iVal = 0;  }
		} break;

	case invalid:
	case isDepIndex:
	default:
		{
			iVal = 0; // error.
		}
	}
	return iVal;
}

CValueVarient::operator unsigned INT_LL(void)
{	
	unsigned INT_LL uVal;

	switch(vType)
	{
	case isBool:
		{	uVal = ((vValue.bIsTrue)? 1 : 0); } break;
	case isIntConst:
		{
			uVal = vValue.iIntConst;  
			//stevev 26jan09 - ALWAYS sign extend when up converting (even when unsigned)
			if (vValue.iIntConst & 0x80000000)
			{
				uVal |= 0xffffffff00000000;
			}
		} break;
	case isVeryLong:
		{
			if (vIsUnsigned)
			{
				uVal = ((unsigned INT_LL)vValue.longlongVal);   // autocast 2 longlong
			}
			else
			{
				uVal = vValue.longlongVal;    // autocast
			}
		}
		break;

	case isFloatConst:
		{	uVal =  (unsigned INT_LL) vValue.fFloatConst;} break;
	case isString:
		// No vulnerability by inspection: user cannot enter the format
		{	if ( sscanf(sStringVal.c_str(),"%llu",&uVal) <= 0) {  uVal = 0;  }
		} break;
	case isWideString:
		// user cannot enter the format
		{
			// No vulnerability by inspection: user cannot enter the format
			if ( swscanf(sWideStringVal.c_str(),L"%llu",&uVal) <= 0) {  uVal = 0;  }
		} break;

	case invalid:
	case isDepIndex:
	default:
		{
			uVal = 0; // error.
		}
	}
	return uVal;
}


CValueVarient::operator string(void)
{
	string sVal;
	sVal = "";
	if (vType == isString)
	{
		sVal = sStringVal;
		// we won't deal with converting numerics to strings right now
	}
	// ws 12mar08
	else if (vType == isWideString)
	{
		// stevev Txt: sVal = TStr2AStr(sWideStringVal);
		//sVal = Unicode2UTF8(sWideStringVal);
		sVal = ws2s(sWideStringVal);
		// we won't deal with converting numerics to strings right now
	}
	// end ws 
	if ( sVal.size() == 0 )
		sVal = "";
	return sVal;
}

CValueVarient::operator wstring(void)
{
	wstring sVal;
	sVal = L"";
	if (vType == isWideString)
	{
		sVal = sWideStringVal;
		// we won't deal with converting numerics to strings right now
	}
	else if (vType == isString)
	{
		//sVal = AStr2TStr(sStringVal);// UTF8 to Unicode conversion
		//sVal = UTF82Unicode(sStringVal.c_str());// stevev 15sept2017

		sVal = s2ws(sStringVal.c_str());

		// we won't deal with converting numerics to strings right now
	}
	else
	{
		sVal = getAsWString();
	}
	if ( sVal.size() == 0 )
		sVal = L"";
	return sVal;
}


wstring CValueVarient::getAsWString()
{
	wstring wS;
	wchar_t tmpBuff[68];

	if (vIsValid)
	{
		switch (vType)
		{
		case CValueVarient::invalid:
			wS = L"*INVALID*";	                    break;
		case CValueVarient::isBool:
			wS =  (vValue.bIsTrue)?L"TRUE":L"FALSE";	break;
		case CValueVarient::isIntConst:
			swprintf(tmpBuff,68,L"const %d (0x%02x)",vValue.iIntConst,vValue.iIntConst);
			wS  = tmpBuff ;
			break;
		case CValueVarient::isFloatConst:
			swprintf(tmpBuff,68,L"const %f (0x%02x)",vValue.fFloatConst,vValue.iIntConst);
			wS  = tmpBuff ;
			break;
		case CValueVarient::isDepIndex:
			swprintf(tmpBuff,68,L"*DEPIDX( %d (0x%02x))*",vValue.depIndex, vValue.depIndex);
			wS  = tmpBuff ;
			break;
		case CValueVarient::isString:
			wS  =  L"String const |";
			// stevev Txt:  wS +=  AStr2TStr(sStringVal);
			//wS +=  UTF82Unicode(sStringVal);
			wS = s2ws(sStringVal);
			wS +=  L"|";
			break;
		case CValueVarient::isVeryLong:	// added 2sept08 stevev
			swprintf(tmpBuff,68,L"const %I64d(0x%04I64x)",vValue.longlongVal,vValue.longlongVal);
			wS  = tmpBuff ;
			break;
		case CValueVarient::isWideString:						// added 2sept08 stevev
			// wouldn't take a straight wstring, We'll just output the first char for now
			wS  =  L"WideString const |";
			wS += sWideStringVal.c_str();
			wS += L"|" ;
			break;
		default:
			//LOGIT(CERR_LOG,"VARIENT: Tried to output an unknown type:%d\n",vType);
			wS =  L"*UNKNOWN_TYPE*";	
			break;
		}
	}
	else
	{
		wS = L"*FLAGGED_INVALID*";
	}
	return wS;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
// EQUAL Operators
/////////////////////////////////////////////////////////////////////////////////////////////////
CValueVarient& CValueVarient::operator=(const CValueVarient& src) 
{
	vType       = src.vType;		vValue    = src.vValue; 
	vIsUnsigned = src.vIsUnsigned;	vIsDouble = src.vIsDouble;

	if ( ! src.sStringVal.empty() ) 
	{ 
		sStringVal = (src.sStringVal.c_str());
	}
	else
	{
		sStringVal.erase();
	}

	if ( ! src.sWideStringVal.empty() )
	{ 
		sWideStringVal = (src.sWideStringVal.c_str());
	}
	else
	{
		sWideStringVal.erase();
	}

	vSize  = src.vSize;		vIsValid = src.vIsValid;
	vIndex = src.vIndex;	vIsBit   = src.vIsBit;
	return *this;
}// end operator=()



CValueVarient& CValueVarient::operator=(const unsigned char src) 
{// sign extend all for storage
	int l = 0; l |= src;
// this throws off the sizeandScale routines	if ( src & 0x80 ) l |= 0xffffff00;
	vType=isIntConst;
	vValue.iIntConst= l; 
	sStringVal.erase();
	vIsUnsigned = true;
	vIsDouble   = false;
	vIsValid    = true;
	vSize       = 1; 
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}


CValueVarient& CValueVarient::operator=(const char src) 
{// sign extend all for storage
	int l = 0; l |= src;
	if ( src & 0x80 ) l |= 0xffffff00;
	vType=isIntConst;
	vValue.iIntConst= l; 
	sStringVal.erase();
	vIsUnsigned = false;
	vIsDouble   = false;
	vIsValid    = true;
	vSize       = 1;  
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}

CValueVarient& CValueVarient::operator=(const unsigned short src) 
{// sign extend all for storage
	int l = 0; l |= src;
	if ( src & 0x8000 ) l |= 0xffff0000;
	vType=isIntConst;
	vValue.iIntConst= l; 
	sStringVal.erase();
	vIsUnsigned = true;
	vIsDouble   = false;
	vIsValid    = true;
	vSize       = 2;  
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}


CValueVarient& CValueVarient::operator=(const short src) 
{// sign extend all for storage
	int l = 0; l |= src;
	if ( src & 0x80 ) l |= 0xffff0000;
	vType=isIntConst;
	vValue.iIntConst= l; 
	sStringVal.erase();
	vIsUnsigned = false;
	vIsDouble   = false;
	vIsValid    = true;
	vSize       = 1;  
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}

CValueVarient& CValueVarient::operator=(const unsigned int src) 
{
	vType=isIntConst;
	vValue.iIntConst= (int)src; 
	sStringVal.erase();
	vIsUnsigned = true;
	vIsDouble   = false;
	vIsValid    = true;
	vSize       = 4;  
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}

CValueVarient& CValueVarient::operator=(const int src) 
{
	vType = isIntConst;
	vValue.iIntConst= src; 
	sStringVal.erase();
	vIsUnsigned = false;
	vIsDouble   = false;
	vIsValid    = true;
	vSize       = 4;  
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}

CValueVarient& CValueVarient::operator=(const double src) 
{
	vType = isFloatConst;
	vValue.fFloatConst= src; 
	sStringVal.erase();
	vIsUnsigned = false;
	vIsDouble   = true;
	vIsValid    = true;
	vSize       = 8;  
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}

CValueVarient& CValueVarient::operator=(const float src) 
{
	vType = isFloatConst;
//		vValue.fFloatConst= src; 
	// stevev 16feb06 - the only way I have seen to cast a float to a double
	//	without introducing an error that is smaller than FLT_EPSILON but
	//	much greater than DBL_EPSILON
	// stevev 24feb06 - the following will NOT convert nans!!!!!!!!!!!!!
	// windowsif (_isnan(src))
	if (std::isnan(src) )
	{
		vValue.fFloatConst = src;
	}
	else
	if ( src <= -numeric_limits<float>::infinity() )
	{
		vValue.fFloatConst = -numeric_limits<double>::infinity();
	}
	else
	if ( src >= numeric_limits<float>::infinity() )
	{
		vValue.fFloatConst = numeric_limits<double>::infinity();
	}
	else
	{	  
#if _MSC_VER >= 1400	/* WS 9apr07 VS2005 checkin */	
  #ifndef _WIN32_WCE
		/*This appears to be a V&V issue only
		v8 and va did not match in output
		'000013\0009\0103.fma' 
		See email with tim
		*/
		_gcvt_s( tmpBuf, _countof(tmpBuf), src, FLT_DIG+1);	// sv increased to +2 26mar15
  #else														// sv put it back 10apr15
		_gcvt(src,FLT_DIG+1,tmpBuf);	// PAW 24/04/09
  #endif
#else			
		//windows _gcvt(src,FLT_DIG+1,tmpBuf);
		gcvt(src, FLT_DIG + 1, tmpBuf);
#endif			
		vValue.fFloatConst = atof(tmpBuf);
	}
	sStringVal.erase();
	vIsUnsigned = false;
	vIsDouble   = false;
	vIsValid    = true;
	vSize       = 4;  
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}

CValueVarient& CValueVarient::operator=(const unsigned INT_LL src)
{
	vType=isVeryLong;
	vValue.longlongVal= (INT_LL)src;
	sStringVal.erase();
	vIsUnsigned = true;
	vIsDouble   = false;
	vIsValid    = true;
	vSize       = 8;  
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}

CValueVarient& CValueVarient::operator=(const INT_LL src)
{
	vType=isVeryLong;
	vValue.longlongVal= src; 
	sStringVal.erase();
	vIsUnsigned = false;
	vIsDouble   = false;
	vIsValid    = true;
	vSize       = 8;  
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}

CValueVarient& CValueVarient::operator=(const string src) 
{
const char* temp = src.c_str();
	vType = isString;
	sStringVal= src; 
temp = sStringVal.c_str();
	vIsUnsigned = false;
	vIsDouble   = false;
	vSize       = (int)sStringVal.size(); // WS 9apr07 VS2005 checkin 
	vIsValid    =true; 
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}

CValueVarient& CValueVarient::operator=(const wstring src)
{
	vType = isWideString;
	sWideStringVal= src;
	vIsUnsigned = false;
	vIsDouble   = false;
	vSize       = (int)sWideStringVal.size(); // WS 9apr07 VS2005 checkin
	vIsValid    =true;
	vIndex      = 0;
	vIsBit      = false;
	return *this;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//  Relational operators
/////////////////////////////////////////////////////////////////////////////////////////////////

bool CValueVarient::operator<=(const CValueVarient& src) // are we <= src
{
	CValueVarient newMe, newSrc;
	valueType_t workingType = promote(*this, newMe, src, newSrc);

	switch(workingType)
	{
	case isBool:
		{
			return (newMe.vValue.bIsTrue == newSrc.vValue.bIsTrue);// there is no relativity
		}
		break;
	case isIntConst:
		{
			if (newMe.vIsUnsigned)// modified 24jan06 - previously, 0 was never < 0xffffffff
			{
				return ( ((unsigned int)newMe. vValue.iIntConst) <= 
					     ((unsigned int)newSrc.vValue.iIntConst));
			}
			else
			{
				return (newMe. vValue.iIntConst <= newSrc.vValue.iIntConst);
			}
		}
		break;
	case isFloatConst:
		{			
			return (newMe. vValue.fFloatConst <= newSrc.vValue.fFloatConst);
		}
		break;
	case isVeryLong:
		{
			if ( newMe.vIsUnsigned )
			{// we are unsigned - compare magnitudes
				return ( ((unsigned INT_LL)newMe.vValue.longlongVal) <=
						 ((unsigned INT_LL)newSrc.vValue.longlongVal) );
			}
			else
				return (newMe.vValue.longlongVal <= newSrc.vValue.longlongVal); 
		}
		break;
	default:
		return false; // an error
		break;
	}// endswitch
/* original code::::>>>>> removed on the hcf_UTF8 branch 25jun08 <<<<<<::::************/
}

	
bool CValueVarient::operator>=(const CValueVarient& src) 
{
	CValueVarient newMe, newSrc;
	valueType_t workingType = promote(*this, newMe, (CVV)src, newSrc);

	switch(workingType)
	{
	case isBool:
		{
			return (newMe.vValue.bIsTrue == newSrc.vValue.bIsTrue);// there is no relativity
		}
		break;
	case isIntConst:
		{
			if (newMe.vIsUnsigned)// modified 24jan06 - previously, 0 was never < 0xffffffff
			{
				return ( ((unsigned int)newMe. vValue.iIntConst) >= 
					     ((unsigned int)newSrc.vValue.iIntConst));
			}
			else
			{
				return (newMe. vValue.iIntConst >= newSrc.vValue.iIntConst);
			}
		}
		break;
	case isFloatConst:
		{			
			return (newMe. vValue.fFloatConst >= newSrc.vValue.fFloatConst);
		}
		break;
	case isVeryLong:
		{
			if ( newMe.vIsUnsigned )
			{// we are unsigned - compare magnitudes
				return ( ((unsigned INT_LL)newMe.vValue.longlongVal) >=
						 ((unsigned INT_LL)newSrc.vValue.longlongVal) );
			}
			else
				return (newMe.vValue.longlongVal >= newSrc.vValue.longlongVal); 
		}
		break;
	default:
		return false; // an error
		break;
	}// endswitch
/* original code::::>>>>> removed on the hcf_UTF8 branch 25jun08 <<<<<<::::************/
}

bool CValueVarient::operator==(const CValueVarient& src) 
{	
	bool ret = false;
	CValueVarient old_Me = *this, oldSrc = src, local;

#ifdef TOKENIZER
	if ( /*my*/vType == isDepIndex  && src.vType == isDepIndex  && vValue.depIndex == src.vValue.depIndex )
	{
		ret = true;
	}
	else
#endif // TOKENIZER
	if (/*my*/vType != isDepIndex && /*my*/vType != invalid 
	 &&   src.vType != isDepIndex &&   src.vType != invalid  )
	{
		if ( old_Me.isNumeric() && oldSrc.isNumeric() )
		{
			CValueVarient newMe, newSrc, oldMe = *this;
			valueType_t workingType = promote(old_Me, newMe, oldSrc, newSrc);

			switch (workingType)
			{
			case isBool:
				{
					ret = (newMe.vValue.bIsTrue   == newSrc.vValue.bIsTrue);
				}
				break;
			case isIntConst:
				{
					ret = (newMe.vValue.iIntConst == newSrc.vValue.iIntConst);
				}
				break;
			case isFloatConst:
				{
					ret = AlmostEqual2sComplement(newSrc.vValue.fFloatConst, 
												  newMe. vValue.fFloatConst, 4);
				}
				break;
			case isVeryLong:
				{
					ret = (newMe.vValue.longlongVal == newSrc.vValue.longlongVal);
				}
				break;
			default:
				ret = false;  // very much an error
				break;
			}//endswitch

		}
		else
		if ( (old_Me.vType == isString || old_Me.vType == isWideString) &&
			 (oldSrc.vType == isString || oldSrc.vType == isWideString)   )
		{//isString || isWideString or unknown error
			if (old_Me.vType == isString && oldSrc.vType == isString)
			{
				ret = (old_Me.sStringVal == oldSrc.sStringVal);
			}
			else
			if (old_Me.vType == isWideString && oldSrc.vType == isWideString)
			{
				ret = (old_Me.sWideStringVal == oldSrc.sWideStringVal);
			}
			else // they are strings but mismatched
			{
				wstring oneSide;
				if (old_Me.vType == isString)//oldSrc MUST be isWideString
				{
					oneSide = (wstring)old_Me;
					ret = (oldSrc.sWideStringVal == oneSide);
				}
				else // oldSrc MUST be isString and old_Me MUST be isWideString
				{
					oneSide = (wstring)oldSrc;
					ret = (old_Me.sWideStringVal == oneSide);
				}
			} 
		}
		else
		{// unknown type or mixed string and numeric types	
			ret = false; // not comparable
		}
	}
	// else these (invalid, isDepIndex) cannot be compared so cannot equal ever..
	// leave ret false
	return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Helper (private) Functions
//////////////////////////////////////////////////////////////////////////////////////////////////

/*** stevev 25jun08 - add promotions to cut down on the number of cases in the math portion *****/
struct INTEGER_RANK{ int rank; bool is_unsigned; };

/* promotes the two types according to C rules, returns type of result */
CValueVarient::valueType_t CValueVarient::promote(  CVV& inOne, CVV& outOne, const CVV& inTwo, CVV& outTwo)
{
	valueType_t  retType = invalid;
	CVV          local;

	outOne.clear(); outOne = inOne;
	outTwo.clear(); outTwo = inTwo;

	// they both have to be numeric to be here
	if ( (! inOne.isNumeric()) || (! ((CVV)inTwo).isNumeric()) )
	{	
		return retType; // an error 
	}

	INTEGER_RANK oneRank, twoRank;
	INTEGER_RANK oneCnvt={0,false},twoCnvt= {0,false};
	
/*	First, if the corresponding real type of either operand is long double, the other
operand is converted, without change of type domain, to a type whose
corresponding real type is long double.   */
	/*
	--- We don't support long double at this time 
	*/ 
/*  Otherwise, if the corresponding real type of either operand is double, the other
operand is converted, to a double. */
	if ( (outOne.vType == isFloatConst &&  outOne.vIsDouble) && 
		 (outTwo.vType != isFloatConst || !outTwo.vIsDouble)   )
	{	
		local.clear();  
		local = (double)outTwo;  
		outTwo.clear(); 
		outTwo = local;
		retType = isFloatConst;
	}
	else 
	if ( (outOne.vType != isFloatConst || !outOne.vIsDouble) && 
		 (outTwo.vType == isFloatConst &&  outTwo.vIsDouble)    )
	{	
		local.clear(); 
		local = (double)outOne;  
		outOne.clear(); 
		outOne = local;
		retType = isFloatConst;
	}
	else 
	if ( (outOne.vType == isFloatConst &&  outOne.vIsDouble) && 
		 (outTwo.vType == isFloatConst &&  outTwo.vIsDouble)   )
	{	
		local.clear(); 
		local = (double)outOne;  
		outOne.clear(); 
		outOne = local;
		local.clear(); 
		local = (double)outTwo;  
		outTwo.clear(); 
		outTwo = local;
		retType = isFloatConst;
	}

/*  Otherwise, if the corresponding real type of either operand is float, the other
operand is converted to a float.  */
	else 
	if ( (outOne.vType == isFloatConst && !outOne.vIsDouble) && 
		 (outTwo.vType != isFloatConst ||  outTwo.vIsDouble)   )
	{	local.clear(); local = (float)outTwo;  outTwo.clear(); outTwo = local;
		retType = isFloatConst;
	}
	else 
	if ( (outOne.vType != isFloatConst ||  outOne.vIsDouble) && 
		 (outTwo.vType == isFloatConst && !outTwo.vIsDouble)   )
	{	local.clear(); local = (float)outOne;  outOne.clear(); outOne = local;
		retType = isFloatConst;
	}
	else 
	if ( (outOne.vType == isFloatConst && !outOne.vIsDouble) && 
		 (outTwo.vType == isFloatConst && !outTwo.vIsDouble)   )
	{	local.clear(); local = outOne;  outOne.clear(); outOne = local;
		local.clear(); local = outTwo;  outTwo.clear(); outTwo = local;
		retType = isFloatConst;
	}
	// else: neither is double nor float so fall thru to int handling

/*  Otherwise, the integer promotions are performed on both operands. Then the
following rules are applied to the promoted operands:   */
	if ( retType == invalid ) // no float types...
	{
		if (outOne.vType == isBool )
		{	
			oneRank.is_unsigned = false; oneRank.rank = 1;
		}
		else 
		if ( outOne.vType == isIntConst )
		{
			if ( outOne.vIsUnsigned ) 
				oneRank.is_unsigned = true;
			else // not unsigned => is signed
				oneRank.is_unsigned = false;

			switch (outOne.vSize)
			{
			case 1:		// char & unsigned char
				oneRank.rank = 2;
				break;
			case 2:		// short & unsigned short
				oneRank.rank = 3;
				break;
			case 4:		// int & unsigned int
				oneRank.rank = 4;
				break;
			default:		//0,3
				return invalid; // error return
				break;
			}
		}
		else 
		if ( outOne.vType == isVeryLong)
		{
			if ( outOne.vIsUnsigned ) 
				oneRank.is_unsigned = true;
			else // not unsigned => is signed
				oneRank.is_unsigned = false;

			if (outOne.vSize == 8)
			{
				oneRank.rank = 5;
			}
			else // size 5,6,7 & > 8
			{	return invalid; // error return
			}
		}
		else
		{	return invalid; // error return
		}

		
		if (outTwo.vType == isBool )
		{	
			twoRank.is_unsigned = false; twoRank.rank = 1;
		}
		else 
		if ( outTwo.vType == isIntConst )
		{
			if ( outTwo.vIsUnsigned ) 
				twoRank.is_unsigned = true;
			else // not unsigned => is signed
				twoRank.is_unsigned = false;

			switch (outTwo.vSize)
			{
			case 1:		// char & unsigned char
				twoRank.rank = 2;
				break;
			case 2:		// short & unsigned short
				twoRank.rank = 3;
				break;
			case 4:		// int & unsigned int
				twoRank.rank = 4;
				break;
			default:		//0,3
				return invalid; // error return
				break;
			}
		}
		else 
		if ( outTwo.vType == isVeryLong)
		{
			if ( outTwo.vIsUnsigned ) 
				twoRank.is_unsigned = true;
			else // not unsigned => is signed
				twoRank.is_unsigned = false;

			if (outTwo.vSize == 8)
			{
				twoRank.rank = 5;
			}
			else // size 5,6,7 & > 8
			{	return invalid; // error return
			}
		}
		else
		{	return invalid; // error return
		}

	/* If both operands have the same type, then no further conversion is needed. */
		if ( outOne.vType       == outTwo.vType        &&  
			 outOne.vIsUnsigned == outTwo.vIsUnsigned  &&  
			 outOne.vIsDouble   == outTwo.vIsDouble      )
		{	return outTwo.vType;// done
		}

	/* Otherwise, if both operands have signed integer types or both have unsigned
	integer types, the operand with the type of lesser integer conversion rank is
	converted to the type of the operand with greater rank. */
		if ( (( oneRank.is_unsigned) && ( twoRank.is_unsigned)) || 
			 ((!oneRank.is_unsigned) && (!twoRank.is_unsigned))  )
		{// lower to higher
			//oneCnvt,twoCnvt;
			if (oneRank.rank > twoRank.rank)
			{
				twoCnvt = oneRank;// other stays empty
			}
			else
			{
				oneCnvt = twoRank;// other stays empty
			}
		}
		else // one is signed, the other is unsigned
	/* Otherwise, if the operand that has unsigned integer type has rank greater or
	equal to the rank of the type of the other operand, then the operand with
	signed integer type is converted to the type of the operand with unsigned
	integer type.*/
		if ( oneRank.is_unsigned && oneRank.rank >= twoRank.rank )
		{// two converted to one's type
			twoCnvt = oneRank;// other stays empty
		}
		else
		if ( twoRank.is_unsigned && twoRank.rank >= oneRank.rank )
		{// one converted to two's type
			oneCnvt = twoRank;// other stays empty
		}
		else
	/* Otherwise, if the type of the operand with signed integer type can represent
	all of the values of the type of the operand with unsigned integer type, then
	the operand with unsigned integer type is converted to the type of the
	operand with signed integer type.  */
		if ( (!oneRank.is_unsigned) && oneRank.rank > twoRank.rank)
		{//two converted to one's type
			twoCnvt = oneRank;// other stays empty
		}
		else
		if ( (!twoRank.is_unsigned) && twoRank.rank > oneRank.rank )
		{// one converted to two's type
			oneCnvt = twoRank;// other stays empty
		}
		else
	/* Otherwise, both operands are converted to the unsigned integer type
	corresponding to the type of the operand with signed integer type.	*/
		if ( oneRank.is_unsigned )// two is SIGNED
		{//both to twoRank.rank and unsigned
			twoCnvt = oneCnvt   = twoRank;// other stays empty
			twoCnvt.is_unsigned = true;
			oneCnvt.is_unsigned = true;
		}
		else // one is SIGNED
		{//both to oneRank.rank and unsigned
			twoCnvt = oneCnvt   = oneRank;// other stays empty
			twoCnvt.is_unsigned = true;
			oneCnvt.is_unsigned = true;
		}

		// do the conversion(s)
		if (oneCnvt.rank > 0 )
		{// convert oneOut to oneCnvt type
			switch (oneCnvt.rank)
			{
			case 1:	// bool
				{	local.clear();  local  = (bool)outOne;  
					outOne.clear(); outOne = local;
					retType = isBool;
				}
				break;
			case 2:	// char
				{
					if (oneCnvt.is_unsigned)
					{	local.clear();  local = (unsigned char)outOne;  
						outOne.clear(); outOne = local;
						retType = isIntConst;//RUL_UNSIGNED_CHAR;
					}
					else//signed
					{	local.clear();  local = (char)outOne;  
						outOne.clear(); outOne = local;
						retType = isIntConst;//RUL_CHAR;
					}
				}
				break;
			case 3:	// short
				{
					if (oneCnvt.is_unsigned)
					{	local.clear();  local = (unsigned short)outOne;  
						outOne.clear(); outOne = local;
						retType = isIntConst;//RUL_USHORT;
					}
					else//signed
					{	local.clear();  local = (short)outOne;  
						outOne.clear(); outOne = local;
						retType = isIntConst;//RUL_SHORT;
					}
				}
				break;
			case 4:	// int
				{
					if (oneCnvt.is_unsigned)
					{	local.clear();  local = (unsigned int)outOne;  
						outOne.clear(); outOne = local;
						retType = isIntConst;//RUL_UINT;
					}
					else//signed
					{	local.clear();  local = (int)outOne;  
						outOne.clear(); outOne = local;
						retType = isIntConst;//RUL_INT;
					}
				}
				break;
			case 5:	// long long
				{
					if (oneCnvt.is_unsigned)
					{	local.clear();  local = (UINT64)outOne;  
						outOne.clear(); outOne = local;
						retType = isVeryLong;//RUL_ULONGLONG;
					}
					else//signed
					{	local.clear();  local = (INT64)outOne;  
						outOne.clear(); outOne = local;
						retType = isVeryLong;//RUL_LONGLONG;
					}
				}
				break;
			default:
				outOne.clear();// error
				retType = invalid;//RUL_NULL;
				break;
			}// endswitch
		}// else no conversion on one

		if (twoCnvt.rank > 0 )
		{// convert twoOut to twoCnvt type
			switch (twoCnvt.rank)
			{
			case 1:	// bool
				{	local.clear();   local = (bool)outTwo;  
					outTwo.clear(); outTwo = local;
					retType = isBool;//RUL_BOOL;
				}
				break;
			case 2:	// char
				{
					if (twoCnvt.is_unsigned)
					{	local.clear();  local = (unsigned char)outTwo;  
						outTwo.clear(); outTwo = local;
						retType = isIntConst;//RUL_UNSIGNED_CHAR;
					}
					else//signed
					{	local.clear();  local = (char)outTwo;  
						outTwo.clear(); outTwo = local;
						retType = isIntConst;//RUL_CHAR;
					}
				}
				break;
			case 3:	// short
				{
					if (twoCnvt.is_unsigned)
					{	local.clear();  local = (unsigned short)outTwo;  
						outTwo.clear(); outTwo = local;
						retType = isIntConst;//RUL_USHORT;
					}
					else//signed
					{	local.clear();  local = (short)outTwo;  
						outTwo.clear(); outTwo = local;
						retType = isIntConst;//RUL_SHORT;
					}
				}
				break;
			case 4:	// int
				{
					if (twoCnvt.is_unsigned)
					{	local.clear();  local = (unsigned int)outTwo;  
						outTwo.clear(); outTwo = local;
						retType = isIntConst;//RUL_UINT;
					}
					else//signed
					{	local.clear();  local = (int)outTwo;  
						outTwo.clear(); outTwo = local;
						retType = isIntConst;//RUL_INT;
					}
				}
				break;
			case 5:	// long long
				{
					if (twoCnvt.is_unsigned)
					{	local.clear();  local = (UINT64)outTwo;  
						outTwo.clear(); outTwo = local;
						retType = isVeryLong;//RUL_ULONGLONG;
					}
					else//signed
					{	local.clear();  local = (INT64)outTwo;  
						outTwo.clear(); outTwo = local;
						retType = isVeryLong;//RUL_LONGLONG;
					}
				}
				break;
			default:
				outTwo.clear();// error
				retType = invalid;//RUL_NULL;
				break;
			}// endswitch
		}// else no conversion on two

	}//else let the float types stay
	return retType;
}// end of promote