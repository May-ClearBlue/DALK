#include "DalkTypeDef.h"

//std::stringと呼び出しを共通化するクラス

class _STRING{
public:
	_STRING(const char* str) {
		@autoreleasepool {
		 m_string = [[NSMutableString alloc] initWithCString:str encoding:NSUTF8StringEncoding];
		}
	}
	_STRING(std::string& str) {
		@autoreleasepool {
		m_string = [[NSMutableString alloc] initWithCString:str.data() encoding:NSUTF8StringEncoding];
		}
	}
	_STRING(NSString* str) {
		@autoreleasepool {
		m_string = [[NSMutableString alloc] initWithString:str];
		}
	}
	_STRING() {
		@autoreleasepool {
		m_string = [[NSMutableString alloc] initWithString:@""];
		}
	}
    virtual ~_STRING() {
//        [m_string release];
    }

protected:
	NSMutableString*	m_string;
public:
    NSString* GetNSString(){ return m_string; }
    inline	void operator= (const char* str)		{ [m_string setString:[NSString stringWithCString:str encoding:NSUTF8StringEncoding]]; }
    inline	void operator= (const string& str)      { [m_string setString:[NSString stringWithCString:str.data() encoding:NSUTF8StringEncoding]];}
    inline	void operator= (NSString* str)          { [m_string setString:str]; }
    inline	void operator+=(const char* str)		{ [m_string appendString:[NSString stringWithCString:str encoding:NSUTF8StringEncoding]]; }
    inline	void operator+=(const string& str)      { [m_string appendString:[NSString stringWithCString:str.data() encoding:NSUTF8StringEncoding]];}
    inline	void operator+=(NSString* str)			{ [m_string appendString:str]; }
	inline	bool operator==(NSString* str)			{ return [m_string compare:str] == NSOrderedSame; }
    inline	bool operator==(const char* str)		{ return strcmp([m_string UTF8String], str) == 0 ? true : false; }
    inline	bool operator==(const string& str)		{ return str.data() == [m_string UTF8String]; }
	inline	const char* data()	{ return (char*)[m_string UTF8String]; }
    inline	const char* str()	{ return (char*)[m_string UTF8String]; }
};
