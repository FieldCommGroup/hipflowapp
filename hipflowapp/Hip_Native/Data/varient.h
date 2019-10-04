/*************************************************************************************************
 *
 * $Workfile: varient.h $
 * 16Mar06 - stevev
 *     Revision, Date and Author have been removed due to the incredible merge
 *     overhead required to reconcile the Revision diffs from a branch.  
 *     Most CVS support tools (like Tortoise) can obtain & report this
 *     information for you.
 *
 *************************************************************************************************
 * The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c)2002 - 2008, HART Communication Foundation, All Rights Reserved
 *************************************************************************************************
 *
 * Description:
 *		home of the varient class
 *		4/5/2	sjv	created
 *      6/25/08 sjv moved code to after the header
 *
 * #include "varient.h"
 */

#ifndef _VARIENT_H
#define _VARIENT_H
#ifdef INC_DEBUG
#pragma message("In varient.h")
#endif

#include <string>
#include <vector>
using namespace std;

#ifndef LINUX
#define  INT_LL    __int64
#include "ddbGeneral.h"
#include "char.h"
#include "ddbdefs.h"
#include "logging.h"
#include "ddbStrings.h"
#include "QA+Debug.h"
#else
#define  INT_LL  long long
#endif // LINUX

#ifdef INC_DEBUG
#pragma message("    Finished Includes::varient.h")
#endif

#define VARIENTTYPESTRING {"invalid"},{"Bool"},{"Opcode"},{"IntConst"},{"FloatConst"},\
		{"DependIndex"},{"SymbolID"},{"LongLong"},{"String"},{"WideString"}
#define VARIENTYPECOUNT  10
#define VARIENTTYPE_LEN  13

extern char varientTypeStrings[VARIENTYPECOUNT][VARIENTTYPE_LEN];

#define BOOLTSIZE	1
#define OPCODESIZE	(sizeof(int))
#define INTSIZE		(sizeof(int))
#define DOUBLESIZE	(sizeof(double))
#define FLOATSIZE	(sizeof(float))
#define IDSIZE		(sizeof(int))
#define LLSIZE		(sizeof(INT_LL))

#define CVV			CValueVarient

class CValueVarient
{
	char tmpBuf[40];// for weird float conversions
public:
	typedef enum valueType_e
	{
		invalid,
		isBool,
		isIntConst,
		isFloatConst,
		isDepIndex,
		isVeryLong,
		isString,
		isWideString
	}valueType_t;

	union vValue_u
	{	
		bool				bIsTrue;    // constant T/F
		int					iIntConst;	// type 22
		double				fFloatConst;// type 23 - float	
										// FLOATCHANGE no change required [1/20/2016 tjohnston]
		int					depIndex;   // type 24 through 29
		INT_LL				longlongVal;// undefined in DDs for now
	} 
	/* union */  vValue;
	string				    sStringVal;
	wstring				    sWideStringVal;
	valueType_t  vType;
	int	         vSize;
	unsigned long vIndex;   /* only for bit-enum'd bit reference resolution */
							/* also holds 'which' for attribute id resolution*/
	bool		vIsValid;
	bool        vIsUnsigned;
	bool        vIsDouble;
	bool        vIsBit;	/* set when bit-Enum SymbolID:: vIndex is Valid */

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	CValueVarient();
	CValueVarient(const CValueVarient& src);
	virtual ~CValueVarient();

	void clear(void);

	bool valueIsZero(void);

	bool isNumeric();

	// Cast operators
	/////////////////////////////////////////////////////////////////////////////////////////////
    operator bool(void);
	operator char(void);
	operator unsigned char(void);
	operator short(void);
	operator unsigned short(void);
    operator int(void);
    operator unsigned int(void);	
    operator long(void);	
    operator unsigned long(void);
	operator INT_LL(void);
	operator unsigned INT_LL(void);
	operator double(void);
	operator float(void);	
	//operator itemID_t(void);  // use unsigned int for this
	operator string(void);
	operator wstring(void);

	//  Equal operators
	/////////////////////////////////////////////////////////////////////////////////////////////
	CValueVarient& operator=(const CValueVarient& src);

	CValueVarient& operator=(const unsigned char src);
	CValueVarient& operator=(const char src);
	CValueVarient& operator=(const unsigned short src);
	CValueVarient& operator=(const short src);
	CValueVarient& operator=(const unsigned int src);
	CValueVarient& operator=(const int src);
	CValueVarient& operator=(const double src);
	CValueVarient& operator=(const float src);
	CValueVarient& operator=(const unsigned INT_LL src);
	CValueVarient& operator=(const INT_LL src);
	CValueVarient& operator=(const string src);
	CValueVarient& operator=(const wstring src);

	//  Relational operators
	/////////////////////////////////////////////////////////////////////////////////////////////

	bool operator<=(const CValueVarient& src); // are we <= src
	bool operator>=(const CValueVarient& src);	
	bool operator==(const CValueVarient& src);/* new 06oct06 - stevev */

private:		
	// for internal use only
	static  valueType_t promote( CVV& inOne, CVV& outOne, const CVV& inTwo, CVV& outTwo);
	wstring getAsWString();
};

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
 *  The Content code is now in varient.cpp
 *+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

typedef vector<CValueVarient>  varientList_t;
typedef vector<CValueVarient*> varientPtrList_t;

#ifdef _OSTREAM_

inline ostream &operator<<(ostream& ostr, CValueVarient& vv )
{
	if (vv.vIsValid)
	{
		switch (vv.vType)
		{
		case CValueVarient::invalid:
			ostr << "*INVALID*";	                    break;
		case CValueVarient::isBool:
			ostr << (vv.vValue.bIsTrue)?"TRUE":"FALSE";	break;
		case CValueVarient::isOpcode:
			{
				switch ((expressElemType_t)(vv.vValue.iOpCode))
				{
				case eet_NOT:			//_OPCODE 1
					ostr << "NOT";		break;
				case eet_NEG:			//_OPCODE 2
					ostr << "NEG";		break;
				case eet_BNEG:			//_OPCODE 3
					ostr << "BNEG";		break;
				case eet_ADD:			//_OPCODE 4
					ostr << "ADD";		break;
				case eet_SUB:			//_OPCODE 5
					ostr << "SUB";		break;
				case eet_MUL:			//_OPCODE 6
					ostr << "MUL";		break;
				case eet_DIV:			//_OPCODE 7
					ostr << "DIV";		break;
				case eet_MOD:			//_OPCODE 8
					ostr << "MOD";		break;
				case eet_LSHIFT:		//_OPCODE 9
					ostr << "LSHIFT";		break;
				case eet_RSHIFT:		//_OPCODE 10
					ostr << "RSHIFT";		break;
				case eet_AND:			//_OPCODE 11
					ostr << "AND";		break;
				case eet_OR:			//_OPCODE 12
					ostr << "OR";		break;
				case eet_XOR:			//_OPCODE 13
					ostr << "XOR";		break;
				case eet_LAND:			//_OPCODE 14
					ostr << "LOGICALAND";		break;
				case eet_LOR:			//_OPCODE 15
					ostr << "LOGICALOR";		break;
				case eet_LT:			//_OPCODE 16
					ostr << "LESSTHAN";		break;
				case eet_GT:			//_OPCODE 17
					ostr << "GREATERTHAN";		break;
				case eet_LE:			//_OPCODE 18
					ostr << "LESSTHANOREQUAL";		break;
				case eet_GE:			//_OPCODE 19
					ostr << "GREATERTHANOREQUAL";		break;
				case eet_EQ:			//_OPCODE 20
					ostr << "EQUAL";		break;
				case eet_NEQ:			//_OPCODE 21
					ostr << "NOTEQUAL";		break;
				default:
					LOGIT(CERR_LOG,"VARIENT: Tried to output an unknown OpCode: %d\n" 
						 ,vv.vValue.iOpCode);
					ostr << "*UNKNOWN_OPCODE*";	
					break;
				}
			}
			break;
		case CValueVarient::isIntConst:
			ostr << "const " << vv.vValue.iIntConst  << " (0x" << hex <<vv.vValue.iIntConst<<dec<<")";	break;
		case CValueVarient::isFloatConst:
			{
				float y  = (float)(vv.vValue.fFloatConst);
				double z = vv.vValue.fFloatConst;
			//	char tmpbuff[64];
				// note 'ostr << "const d " << z << ' and 'ostr << "const f " << y <<' generate the EXACT same strings
			
				if (vv.vIsDouble)
				{
			//	sprintf(tmpbuff,"const d %.15f (0x%a)",z,z);
			//	ostr << tmpbuff; break;
			//ostr << "const d " << z << " (0x" << hex <<vv.vValue.fFloatConst<<dec<<")";break;
			ostr << "const d " << z ;break;
				}
				else
				{
			//	sprintf(tmpbuff,"const f %.6f (0x%a)",y,y);
			//	ostr << tmpbuff; break;
			//ostr << "const f " << y << " (0x" << hex <<vv.vValue.fFloatConst<<dec<<")";break;
			ostr << "const f " << y ;break;
				}
			}
		case CValueVarient::isDepIndex:
			ostr << "*DEPIDX(" << vv.vValue.depIndex << ")*";break;
		case CValueVarient::isString:
			{
				string lang = vv.sStringVal.substr(0, 4);
				if (QANDA && lang.compare("|en|") == 0)
				{
					ostr << "String const |" << vv.sStringVal.substr(4) << "|" ;break;
				}
				else
				{
					ostr << "String const |" << vv.sStringVal<< "|" ;break;
				}
			}
		case CValueVarient::isSymID:
			ostr << "Symbol(0x"<<hex<<vv.vValue.varSymbolID<<dec<<")";
			if (vv.vIsBit) ostr << "[0x"<<hex<<vv.vIndex<<dec<<"]";break;
		case CValueVarient::isVeryLong:						// added 2sept08 stevev
#if _MSC_VER < 1400
			ostr << "const " << (ulong)/*PAW 03/03/09*/vv.vValue.longlongVal  
				 << " (0x" << hex <<(ulong)/*PAW 03/03/09*/vv.vValue.longlongVal<<dec
				 <<")";
#else
			ostr << "const " << vv.vValue.longlongVal  
				 << " (0x" << hex << vv.vValue.longlongVal <<dec<<")";	
#endif
			break;
		case CValueVarient::isWideString:						// added 2sept08 stevev
			{
				// wouldn't take a straight wstring, We'll just output ascii for now
				//stevev Txt:  string astr = TStr2AStr(vv.sWideStringVal);
				string astr = Unicode2UTF8(vv.sWideStringVal);
				{
					string lang = astr.substr(0, 4);
					if (QANDA_8Vold8 && lang.compare("|en|") == 0)
					{
						astr = astr.substr(4);
						std::size_t found = astr.find("|");
						if (found != std::string::npos)
							astr = astr.substr(0, found);

						ostr << "WideString const |" << astr << "|" ;break;
					}
					else if (QANDA && lang.compare("|en|") == 0)
					{
						ostr << "WideString const |" << astr.substr(4) << "|"; break;
					}
					else
					{
						ostr << "WideString const |" << astr << "|" ;break;
					}
				}
			}
		default:
			LOGIT(CERR_LOG,"VARIENT: Tried to output an unknown type:%d\n",vv.vType);
			//ostr << "*UNKNOWN_TYPE*";	
			break;
		}
	}
	else
	{
		ostr << "*FLAGGED_INVALID*";
	}
	return ostr;
}
#endif //_OSTREAM_

#endif// _VARIENT_H

/*************************************************************************************************
 *
 *   $History: varient.h $
 * 
 * *****************  Version 4  *****************
 * User: Stevev       Date: 5/02/03    Time: 11:02a
 * Updated in $/DD Tools/DDB/Common
 * Added unsigned long cast operator that is essensially the unsigned int
 * (removes errors during compile).
 * 
 * *****************  Version 3  *****************
 * User: Stevev       Date: 4/09/03    Time: 7:39a
 * Updated in $/DD Tools/DDB/Common
 * Update Header and footer to HART cod standard
 * 
 *************************************************************************************************
 */
