#include "HTMLParser.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static HTMLTag* TagFactory(HTMLTagType theTagType) { return new HTMLTag(theTagType); }
static HTMLTag* TagBodyFactory(HTMLTagType theTagType) { return new HTMLBodyTag; }
static HTMLTag* TagBlockElemFactory(HTMLTagType theTagType) { return new HTMLTag(theTagType,true); }
static HTMLTag* TagLineBreakFactory(HTMLTagType theTagType) { return new HTMLLineBreakTag; }
static HTMLTag* TagHorizontalRuleFactory(HTMLTagType theTagType) { return new HTMLHorizontalRuleTag; }
static HTMLTag* TagHeadingFactory(HTMLTagType theTagType) { return new HTMLHeadingTag(theTagType); }
static HTMLTag* TagAnchorFactory(HTMLTagType theTagType) { return new HTMLAnchorTag; }
static HTMLTag* TagFontFactory(HTMLTagType theTagType) { return new HTMLFontTag(theTagType); }
static HTMLTag* TagBlockFactory(HTMLTagType theTagType) { return new HTMLBlockTag(theTagType); }
static HTMLTag* TagImageMapFactory(HTMLTagType theTagType) { return new HTMLImageMapTag; }
static HTMLTag* TagAreaFactory(HTMLTagType theTagType) { return new HTMLAreaTag; }
static HTMLTag* TagImageFactory(HTMLTagType theTagType) { return new HTMLImageTag; }
static HTMLTag* TagListFactory(HTMLTagType theTagType) { return new HTMLListTag(theTagType); }
static HTMLTag* TagListItemFactory(HTMLTagType theTagType) { return new HTMLListItemTag; }
static HTMLTag* TagTableFactory(HTMLTagType theTagType) { return new HTMLTableTag; }
static HTMLTag* TagTableRowFactory(HTMLTagType theTagType) { return new HTMLTableRowTag; }
static HTMLTag* TagTableDataFactory(HTMLTagType theTagType) { return new HTMLTableDataTag(theTagType); }
static HTMLTag* TagInputFactory(HTMLTagType theTagType) { return new HTMLInputTag; }
static HTMLTag* TagSelectFactory(HTMLTagType theTagType) { return new HTMLInputTag(HTMLInputType_Select); }
static HTMLTag* TagSkipFactory(HTMLTagType theTagType) { return new HTMLTag(theTagType,HTMLTagFlag_SkipToEndTag); }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLParser::TagNameMap& HTMLParser::GetTagNameMap()
{
	static TagNameMap mTagNameMap;
	if(mTagNameMap.empty())
	{
		mTagNameMap["BODY"]		=		TagLookupInfo(TagBodyFactory,HTMLTagType_Body);

		mTagNameMap["BR"]		=		TagLookupInfo(TagLineBreakFactory,HTMLTagType_LineBreak);
		mTagNameMap["HR"]		=		TagLookupInfo(TagHorizontalRuleFactory,HTMLTagType_HorizontalRule);

		mTagNameMap["P"]		=		TagLookupInfo(TagBlockFactory,HTMLTagType_Paragraph);
		mTagNameMap["DIV"]		=		TagLookupInfo(TagBlockFactory,HTMLTagType_Div);

		mTagNameMap["TABLE"]	=		TagLookupInfo(TagTableFactory,HTMLTagType_Table);
		mTagNameMap["TR"]		=		TagLookupInfo(TagTableRowFactory,HTMLTagType_TableRow);
		mTagNameMap["TD"]		=		TagLookupInfo(TagTableDataFactory,HTMLTagType_TableData);
		mTagNameMap["TH"]		=		TagLookupInfo(TagTableDataFactory,HTMLTagType_TableHeader);

		mTagNameMap["H1"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H1);
		mTagNameMap["H2"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H2);
		mTagNameMap["H3"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H3);
		mTagNameMap["H4"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H4);
		mTagNameMap["H5"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H5);
		mTagNameMap["H6"]		=		TagLookupInfo(TagHeadingFactory,HTMLTagType_H6);

		mTagNameMap["UL"]		=		TagLookupInfo(TagListFactory,HTMLTagType_UnorderedList);
		mTagNameMap["OL"]		=		TagLookupInfo(TagListFactory,HTMLTagType_OrderedList);
		mTagNameMap["LI"]		=		TagLookupInfo(TagListItemFactory,HTMLTagType_ListItem);

		mTagNameMap["PRE"]		=		TagLookupInfo(TagBlockElemFactory,HTMLTagType_Pre);
		mTagNameMap["CENTER"]	=		TagLookupInfo(TagBlockElemFactory,HTMLTagType_Center);

		mTagNameMap["A"]		=		TagLookupInfo(TagAnchorFactory,HTMLTagType_Anchor);

		mTagNameMap["B"]		=		TagLookupInfo(TagFactory,HTMLTagType_Bold);
		mTagNameMap["STRONG"]	=		TagLookupInfo(TagFactory,HTMLTagType_Bold);
		mTagNameMap["I"]		=		TagLookupInfo(TagFactory,HTMLTagType_Italic);
		mTagNameMap["EM"]		=		TagLookupInfo(TagFactory,HTMLTagType_Italic);
		mTagNameMap["U"]		=		TagLookupInfo(TagFactory,HTMLTagType_Underline);
		mTagNameMap["CODE"]		=		TagLookupInfo(TagFactory,HTMLTagType_Code);
		mTagNameMap["TT"]		=		TagLookupInfo(TagFactory,HTMLTagType_Code);
		mTagNameMap["KBD"]		=		TagLookupInfo(TagFactory,HTMLTagType_Code);
		mTagNameMap["SAMP"]		=		TagLookupInfo(TagFactory,HTMLTagType_Code);
		mTagNameMap["SMALL"]	=		TagLookupInfo(TagFactory,HTMLTagType_Small);
		mTagNameMap["BIG"]		=		TagLookupInfo(TagFactory,HTMLTagType_Big);

		mTagNameMap["IMG"]		=		TagLookupInfo(TagImageFactory,HTMLTagType_Image);
		mTagNameMap["MAP"]		=		TagLookupInfo(TagImageMapFactory,HTMLTagType_ImageMap);
		mTagNameMap["AREA"]		=		TagLookupInfo(TagAreaFactory,HTMLTagType_Area);
		
		mTagNameMap["FONT"]		=		TagLookupInfo(TagFontFactory,HTMLTagType_Font);
		mTagNameMap["BASEFONT"]	=		TagLookupInfo(TagFontFactory,HTMLTagType_BaseFont);

		mTagNameMap["NOBR"]		=		TagLookupInfo(TagFactory,HTMLTagType_NoBreak);
		
		mTagNameMap["INPUT"]	=		TagLookupInfo(TagInputFactory,HTMLTagType_Input);
		mTagNameMap["SELECT"]	=		TagLookupInfo(TagSelectFactory,HTMLTagType_Select);
		mTagNameMap["OPTION"]	=		TagLookupInfo(TagFactory,HTMLTagType_Option);
		
		mTagNameMap["SCRIPT"]	=		TagLookupInfo(TagSkipFactory,HTMLTagType_Script);
		mTagNameMap["STYLE"]	=		TagLookupInfo(TagSkipFactory,HTMLTagType_Style);
	}

	return mTagNameMap;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLParser::HTMLParser() : mTagNameMap(GetTagNameMap())
{
	mLineNum = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::CloseFile()
{
	mBackStr.erase();
	mFileReader.Close();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::EndOfFile()
{	
	if(mFileReader.EndOfFile())
		return mBackStr.empty();
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int GetWesternChar(int theChar)
{
	switch(theChar)
	{
		case 130:   return 8218;  //     Single Low-9 Quotation Mark
		case 131:   return 402;  //      Latin Small Letter F With Hook
		case 132:   return 8222;  //     Double Low-9 Quotation Mark
		case 133:   return 8230;  //     Horizontal Ellipsis
		case 134:   return 8224;  //     Dagger
		case 135:   return 8225;  //     Double Dagger
		case 136:   return 710;  //      Modifier Letter Circumflex Accent
		case 137:   return 8240;  //     Per Mille Sign
		case 138:   return 352;  //      Latin Capital Letter S With Caron
		case 139:   return 8249;  //     Single Left-Pointing Angle Quotation Mark
		case 140:   return 338;  //      Latin Capital Ligature OE
		case 145:   return 8216;  //     Left Single Quotation Mark
		case 146:   return 8217;  //     Right Single Quotation Mark
		case 147:   return 8220;  //     Left Double Quotation Mark
		case 148:   return 8221;  //     Right Double Quotation Mark
		case 149:   return 8226;  //     Bullet
		case 150:   return 8211;  //     En Dash
		case 151:   return 8212;  //     Em Dash
		case 152:   return 732;  //      Small Tilde
		case 153:   return 8482;  //     Trade Mark Sign
		case 154:   return 353;  //      Latin Small Letter S With Caron
		case 155:   return 8250;  //     Single Right-Pointing Angle Quotation Mark
		case 156:   return 339;  //      Latin Small Ligature OE
		case 159:   return 376;  //      Latin Capital Letter Y With Diaeresis
	}

	return theChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser::GetChar()
{
	int aChar = 0;
	if(mBackStr.empty())
	{
		aChar = mFileReader.TryReadByte();
		if(aChar>=130 && aChar<=159)
			aChar = GetWesternChar(aChar);
	}
	else
	{
		aChar = mBackStr.at(mBackStr.length()-1);
		mBackStr.resize(mBackStr.length()-1);
	}

	if(aChar=='\n')
		mLineNum++;

	return aChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser::PeekChar()
{
	if(EndOfFile())
		return -1;


	int aChar = GetChar();
	UngetChar(aChar);
	return aChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::UngetChar(int theChar)
{ 
	if(theChar==EOF)
		return;

	if(theChar=='\n')
		mLineNum--;

	mBackStr.append(theChar);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::UngetString(const GUIString &theStr)
{
	for(int i=theStr.length()-1; i>=0; i--)
	{
		UngetChar(theStr.at(i));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::UngetString(const GUIString &theStr, int theStartPos)
{
	for(int i=theStr.length()-1; i>=theStartPos; i--)
	{
		UngetChar(theStr.at(i));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::SkipWhitespace()
{
	bool hadSpace = false;
	while(!EndOfFile())
	{
		int aChar = GetChar();
		if(!safe_isspace(aChar))
		{
			UngetChar(aChar);
			break;
		}
		else
			hadSpace = true;
	}

	return hadSpace;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser::ReadEntity()
{
	GUIString anEntity;
	int i;
	for(i=0; i<20 && !EndOfFile(); i++)
	{
		int aChar = GetChar();
		if(aChar==';')
			break;

		anEntity.append(aChar);
	}

	if(i==20)
	{
		UngetString(anEntity);
		return -1;
	}

	if(anEntity.at(0)=='#')
	{
		int anAsciiValue = anEntity.atoi(1);

		if(anAsciiValue>=130 && anAsciiValue<=159)
			anAsciiValue = GetWesternChar(anAsciiValue);

		if(anAsciiValue==0)
			anAsciiValue = -1;

		return anAsciiValue;
	}

	struct EntityDefine { const char* mName; int mEntity; };
	static EntityDefine anEntityTable[] = {
		{"nbsp", 160},
		{"iexcl", 161},
		{"cent", 162},
		{"pound", 163},
		{"curren", 164},
		{"yen", 165},
		{"brvbar", 166},
		{"sect", 167},
		{"uml", 168},
		{"copy", 169},
		{"ordf", 170},
		{"laquo", 171},
		{"not", 172},
		{"shy", 173},
		{"reg", 174},
		{"macr", 175},
		{"deg", 176},
		{"plusmn", 177},
		{"sup2", 178},
		{"sup3", 179},
		{"acute", 180},
		{"micro", 181},
		{"para", 182},
		{"middot", 183},
		{"cedil", 184},
		{"sup1", 185},
		{"ordm", 186},
		{"raquo", 187},
		{"frac14", 188},
		{"frac12", 189},
		{"frac34", 190},
		{"iquest", 191},
		{"Agrave", 192},
		{"Aacute", 193},
		{"Acirc", 194},
		{"Atilde", 195},
		{"Auml", 196},
		{"Aring", 197},
		{"AElig", 198},
		{"Ccedil", 199},
		{"Egrave", 200},
		{"Eacute", 201},
		{"Ecirc", 202},
		{"Euml", 203},
		{"Igrave", 204},
		{"Iacute", 205},
		{"Icirc", 206},
		{"Iuml", 207},
		{"ETH", 208},
		{"Ntilde", 209},
		{"Ograve", 210},
		{"Oacute", 211},
		{"Ocirc", 212},
		{"Otilde", 213},
		{"Ouml", 214},
		{"times", 215},
		{"Oslash", 216},
		{"Ugrave", 217},
		{"Uacute", 218},
		{"Ucirc", 219},
		{"Uuml", 220},
		{"Yacute", 221},
		{"THORN", 222},
		{"szlig", 223},
		{"agrave", 224},
		{"aacute", 225},
		{"acirc", 226},
		{"atilde", 227},
		{"auml", 228},
		{"aring", 229},
		{"aelig", 230},
		{"ccedil", 231},
		{"egrave", 232},
		{"eacute", 233},
		{"ecirc", 234},
		{"euml", 235},
		{"igrave", 236},
		{"iacute", 237},
		{"icirc", 238},
		{"iuml", 239},
		{"eth", 240},
		{"ntilde", 241},
		{"ograve", 242},
		{"oacute", 243},
		{"ocirc", 244},
		{"otilde", 245},
		{"ouml", 246},
		{"divide", 247},
		{"oslash", 248},
		{"ugrave", 249},
		{"uacute", 250},
		{"ucirc", 251},
		{"uuml", 252},
		{"yacute", 253},
		{"thorn", 254},
		{"yuml", 255},

		{"quot", 34},
		{"amp", 38},
		{"lt", 60},
		{"gt", 62},
		{"OElig", 338},
		{"oelig", 339},
		{"Scaron", 352},
		{"scaron", 353},
		{"Yuml", 376},
		{"circ", 710},
		{"tilde", 732},
		{"ensp", 8194},
		{"emsp", 8195},
		{"thinsp", 8201},
		{"zwnj", 8204},
		{"zwj", 8205},
		{"lrm", 8206},
		{"rlm", 8207},
		{"ndash", 8211},
		{"mdash", 8212},
		{"lsquo", 8216},
		{"rsquo", 8217},
		{"sbquo", 8218},
		{"ldquo", 8220},
		{"rdquo", 8221},
		{"bdquo", 8222},
		{"dagger", 8224},
		{"Dagger", 8225},
		{"permil", 8240},
		{"lsaquo", 8249},
		{"rsaquo", 8250},
		{"euro", 8364},

		{"fnof", 402},
		{"Alpha", 913},
		{"Beta", 914},
		{"Gamma", 915},
		{"Delta", 916},
		{"Epsilon", 917},
		{"Zeta", 918},
		{"Eta", 919},
		{"Theta", 920},
		{"Iota", 921},
		{"Kappa", 922},
		{"Lambda", 923},
		{"Mu", 924},
		{"Nu", 925},
		{"Xi", 926},
		{"Omicron", 927},
		{"Pi", 928},
		{"Rho", 929},
		{"Sigma", 931},
		{"Tau", 932},
		{"Upsilon", 933},
		{"Phi", 934},
		{"Chi", 935},
		{"Psi", 936},
		{"Omega", 937},
		{"alpha", 945},
		{"beta", 946},
		{"gamma", 947},
		{"delta", 948},
		{"epsilon", 949},
		{"zeta", 950},
		{"eta", 951},
		{"theta", 952},
		{"iota", 953},
		{"kappa", 954},
		{"lambda", 955},
		{"mu", 956},
		{"nu", 957},
		{"xi", 958},
		{"omicron", 959},
		{"pi", 960},
		{"rho", 961},
		{"sigmaf", 962},
		{"sigma", 963},
		{"tau", 964},
		{"upsilon", 965},
		{"phi", 966},
		{"chi", 967},
		{"psi", 968},
		{"omega", 969},
		{"thetasym", 977},
		{"upsih", 978},
		{"piv", 982},
		{"bull", 8226},
		{"hellip", 8230},
		{"prime", 8242},
		{"Prime", 8243},
		{"oline", 8254},
		{"frasl", 8260},
		{"weierp", 8472},
		{"image", 8465},
		{"real", 8476},
		{"trade", 8482},
		{"alefsym", 8501},
		{"larr", 8592},
		{"uarr", 8593},
		{"rarr", 8594},
		{"darr", 8595},
		{"harr", 8596},
		{"crarr", 8629},
		{"lArr", 8656},
		{"uArr", 8657},
		{"rArr", 8658},
		{"dArr", 8659},
		{"hArr", 8660},
		{"forall", 8704},
		{"part", 8706},
		{"exist", 8707},
		{"empty", 8709},
		{"nabla", 8711},
		{"isin", 8712},
		{"notin", 8713},
		{"ni", 8715},
		{"prod", 8719},
		{"sum", 8721},
		{"minus", 8722},
		{"lowast", 8727},
		{"radic", 8730},
		{"prop", 8733},
		{"infin", 8734},
		{"ang", 8736},
		{"and", 8743},
		{"or", 8744},
		{"cap", 8745},
		{"cup", 8746},
		{"int", 8747},
		{"there4", 8756},
		{"sim", 8764},
		{"cong", 8773},
		{"asymp", 8776},
		{"ne", 8800},
		{"equiv", 8801},
		{"le", 8804},
		{"ge", 8805},
		{"sub", 8834},
		{"sup", 8835},
		{"nsub", 8836},
		{"sube", 8838},
		{"supe", 8839},
		{"oplus", 8853},
		{"otimes", 8855},
		{"perp", 8869},
		{"sdot", 8901},
		{"lceil", 8968},
		{"rceil", 8969},
		{"lfloor", 8970},
		{"rfloor", 8971},
		{"lang", 9001},
		{"rang", 9002},
		{"loz", 9674},
		{"spades", 9824},
		{"clubs", 9827},
		{"hearts", 9829},
		{"diams", 9830}
	};
	typedef std::map<GUIString,int,StringLessNoCase> EntityMap; 
	static EntityMap anEntityMap;
	if(anEntityMap.empty())
	{
		int aNumEntities = sizeof(anEntityTable)/sizeof(EntityDefine);
		for(int i=0; i<aNumEntities; i++)
			anEntityMap[anEntityTable[i].mName] = anEntityTable[i].mEntity;
	}
	EntityMap::iterator anItr = anEntityMap.find(anEntity);
	if(anItr!=anEntityMap.end())
		return anItr->second;	
	else
	{
		UngetChar(';');
		UngetString(anEntity);
		return -1;
	}


	/*if(anEntity.compareNoCase("GT;")==0)
		return '>';
	if(anEntity.compareNoCase("LT;")==0)
		return '<';
	if(anEntity.compareNoCase("AMP;")==0)
		return '&';
	if(anEntity.compareNoCase("QUOT;")==0)
		return '"';
	if(anEntity.compareNoCase("NBSP;")==0)
		return 0xa0; 
		
	UngetString(anEntity);
	return -1;*/
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::ReadWord(GUIString &theWord,  const char *theStopChars, bool stopAtSpace, bool skip, bool erase)
{
	if(erase)
	{
		theWord.resize(0);
		theWord.reserve(32);
	}

	if(stopAtSpace)
		SkipWhitespace();

	while(true)
	{
		int aChar = GetChar();
		if(aChar==-1)
			break;

		if(aChar<256 && ((stopAtSpace && safe_isspace(aChar))  || (theStopChars!=NULL && strchr(theStopChars,aChar))))
		{
			if(aChar!=0)
			{
				UngetChar(aChar);
				break;
			}
		}

		if(aChar=='&')
		{
			aChar = ReadEntity();
			if(!skip)
			{
				if(aChar!=-1)
					theWord.append(aChar);
				else
					theWord.append('&');
			}
		}
		else
		{
			if(!skip)
				theWord.append(aChar);
		}
	}

	return !theWord.empty();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::SkipTag(const GUIString &theName)
{
	const int BUFFER_SIZE = 512;
	GUIString aSearch = "</";
	aSearch.append(theName);
	aSearch.append('>');
	aSearch.toUpperCase();

	GUIString aBuf(BUFFER_SIZE+1);
	GUIString aCapBuf(BUFFER_SIZE+1);

	while(!EndOfFile())
	{
		aBuf.resize(0);
		aCapBuf.resize(0);
		for(int i=0; i<BUFFER_SIZE; i++)
		{
			int aChar = GetChar();
			if(aChar<0)
				break;
			
			aCapBuf.append(toupper(aChar));
			aBuf.append(aChar);
		}
		
		int anEndPos = aCapBuf.find(aSearch);
		if(anEndPos!=-1)
		{
			UngetString(aBuf,anEndPos+aSearch.length());
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::GetActualTag()
{
	mTagAttributes.Clear();
	bool isEndTag = false;
	GUIString aTagName;

	if(PeekChar()=='/')
	{
		GetChar(); // skip '/'
		isEndTag = true;
	}

	ReadWord(aTagName,">");
	if(aTagName.size()>=3 && aTagName.at(0)=='!' && aTagName.at(1)=='-' && aTagName.at(2)=='-') // comment
	{
		// Skip comment
		if(aTagName.size()>2 && aTagName.at(aTagName.length()-2)=='-' && aTagName.at(aTagName.length()-1)=='-' && PeekChar()=='>')
		{
			GetChar(); // skip >
			return true;
		}

		while(!EndOfFile())
		{
			int char1 = GetChar();
			int char2 = GetChar();
			int char3 = GetChar();
			if(char1=='-' && char2=='-' && char3=='>')
				return true;

			UngetChar(char3);
			UngetChar(char2);
		}

		return false;
	}

	TagNameMap::iterator anItr = mTagNameMap.find(aTagName);
	if(anItr==mTagNameMap.end()) // unrecognized tag... just skip it
	{
		GUIString aSkip;
		ReadWord(aSkip,">",false,true);
		GetChar(); // skip '>'
		return true;
	}

	TagLookupInfo &anInfo = anItr->second;	
	HTMLTagPtr aTag = anInfo.mFactoryFunc(anInfo.mType);
	aTag->SetLineNum(mLineNum);

	if(isEndTag) // end tag has no parameters
	{
		aTag->SetFlags(HTMLTagFlag_IsEndTag,true);
		GUIString aSkip;
		ReadWord(aSkip,">",false,true);
		GetChar(); // skip '>'
		mDocument->AddTag(aTag);
		return true;
	}

	while(!EndOfFile())
	{
		if(PeekChar()=='>')
		{
			GetChar(); // skip '>'
			break;
		}

		GUIString anAttribute;
		GUIString aValue;

		ReadWord(anAttribute,">=");
		SkipWhitespace();
		if(PeekChar()=='=') // read attribute value
		{
			GetChar(); // skip '=' sign
			SkipWhitespace();
			if(PeekChar()=='"')
			{
				GetChar(); // Skip '"' sign
				ReadWord(aValue,"\"",false);
				GetChar(); // Skip '"' sign
			}
			else if(PeekChar()=='\'')
			{
				GetChar(); // Skip ' sign
				ReadWord(aValue,"'",false);
				GetChar(); // Skip ' sign
			}
			else
				ReadWord(aValue,">");
		}
		mTagAttributes.AddAttribute(anAttribute,aValue);

	}

	if(aTag->FlagSet(HTMLTagFlag_SkipToEndTag))
		SkipTag(aTagName);
	else
		aTag->ParseAttributes(mTagAttributes);

	mDocument->AddTag(aTag);
	return true;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::GetNextTag()
{
	GUIString aWord;
	ReadWord(aWord,"<",false);
	if(!aWord.empty())
	{
		HTMLTagPtr aTag = new HTMLTextTag(aWord);
		aTag->SetLineNum(mLineNum);
		mDocument->AddTag(aTag);
	}
	if(PeekChar()!='<')
		return false;

	GetChar();
	bool aVal = GetActualTag();
	if(aVal)
	{
	// in HTML supposedly a carriage return directly after a tag is supposed to be ignored
	// (as well as one directly before a tag)
	//		int aChar = PeekChar();
	//		if(aChar=='\n')
	//			GetChar();
	}
	return aVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::Parse()
{
	mLineNum = 1;
	if(mDocument.get()==NULL)
		mDocument = new HTMLDocument;
	else
		mDocument->Clear();

	while(!EndOfFile())
	{
		if(!GetNextTag())
			break;
	}

	CloseFile();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::ParseFromFile(const GUIString &theFileName)
{
	CloseFile();

	if(theFileName.empty())
		return false;

	std::string aFileName = theFileName;
	if(!mFileReader.Open(aFileName.c_str()))
		return false;

	return Parse();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::ParseFromText(const GUIString &theText)
{
	CloseFile();

	mBackStr.resize(0);
	mBackStr.reserve(theText.length());
	for(int i = theText.length()-1; i>=0; i--)
		mBackStr.append(theText.at(i));

	return Parse();
}
