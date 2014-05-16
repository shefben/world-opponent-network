// WON_AuthFamilyBuffer

// Base interface class for Auth buffers within the WONAuth library.  AuthFamilyBuffer
// is an abstract base class, all constructors are protected.

// This class simply makes use of the AuthFamilyBuffer class.  It eliminates the STL
// and VC5 enhancements from the interface.

// *IMPORTANT**
// Implementation relies on the fact that mBufP will NEVER be NULL after construction.

// *NOTE*
// This header is compatible with VC4.  Ergo, no using bool, string, etc.


// We're in WON Source!
#define _WON_SOURCE_

#include "common/won.h"
#include <iostream>
#include "crypt/EGPublicKey.h"
#include "crypt/EGPrivateKey.h"
#include "AuthFamilyBuffer.h"
#include "WON_AuthFamilyBuffer.h"

// Private namespace for using and constants
namespace {
	using WONCrypt::EGPublicKey;
	using WONCrypt::EGPrivateKey;
	using WONAuth::AuthFamilyBuffer;
};


// ** Constructors / Destructor

// Default constructor, allocates nothing (must be done by derived classes)
WON_AuthFamilyBuffer::WON_AuthFamilyBuffer() :
	mBufP(NULL)
	{	MessageBox(NULL, "WON_AuthFamilyBuffer", "1", MB_OK);
}


// Copy Constructor, allocates nothing (must be done by derived classes to prevent
// slicing)
WON_AuthFamilyBuffer::WON_AuthFamilyBuffer(const WON_AuthFamilyBuffer& theBufR) :
	mBufP(NULL)
{MessageBox(NULL, "WON_AuthFamilyBuffer", "copy", MB_OK);}

	
// Destructor
WON_AuthFamilyBuffer::~WON_AuthFamilyBuffer(void)
{
	MessageBox(NULL, "WON_AuthFamilyBuffer", "delete", MB_OK);
	delete mBufP;
}


// ** Public Methods **

// Assignment operator, does nothing.  Actual assign must be done by the derived
// classes to prevent slicing.
WON_AuthFamilyBuffer&
WON_AuthFamilyBuffer::operator=(const WON_AuthFamilyBuffer& theBufR)
{
	MessageBox(NULL, "WON_AuthFamilyBuffer", "1", MB_OK);
	return *this;
}


// Check validity
int
WON_AuthFamilyBuffer::IsValid() const
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "is valid", MB_OK);
	return (mBufP->IsValid() ? RETURN_TRUE : RETURN_FALSE);
}


// Compare method
int
WON_AuthFamilyBuffer::Compare(const WON_AuthFamilyBuffer& theBufR) const
{
	MessageBox(NULL, "WON_AuthFamilyBuffer", "copy", MB_OK);
	return mBufP->Compare(*theBufR.mBufP);
}


// WON_AuthFamilyBuffer::Pack
// Pack into raw form and sign with specified raw private key.
int
WON_AuthFamilyBuffer::Pack(const unsigned char* thePrivKeyP, unsigned short theLen)
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "pack", MB_OK);
	EGPrivateKey aKey(theLen, thePrivKeyP);
	return (mBufP->Pack(aKey) ? RETURN_TRUE : RETURN_FALSE);
}


// WON_AuthFamilyBuffer::Unpack
// Unpack data from raw form into buffer.
int
WON_AuthFamilyBuffer::Unpack(const unsigned char* theRawP, unsigned short theLen)
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "unpack", MB_OK);
	return (mBufP->Unpack(theRawP, theLen) ? RETURN_TRUE : RETURN_FALSE);
}


// WON_AuthFamilyBuffer::Verify
// Veriy signature using specified raw public key.
int
WON_AuthFamilyBuffer::Verify(const unsigned char* thePubKeyP, unsigned short theLen) const
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "verify", MB_OK);
	EGPublicKey aKey(theLen, thePubKeyP);
	return (mBufP->Verify(aKey) ? RETURN_TRUE : RETURN_FALSE);
}


// ** Accessor Methods **
// All methods simply pass through to real buffer object

unsigned short
WON_AuthFamilyBuffer::GetFamily() const
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "getfamily", MB_OK);
	return mBufP->GetFamily();
}

time_t
WON_AuthFamilyBuffer::GetIssueTime() const
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "issuetime", MB_OK);
	return mBufP->GetIssueTime();
}

time_t
WON_AuthFamilyBuffer::GetExpireTime() const
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "expire", MB_OK);
	return mBufP->GetExpireTime();
}

unsigned long
WON_AuthFamilyBuffer::GetLifespan() const
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "lifespan", MB_OK);
	return mBufP->GetLifespan();
}

void
WON_AuthFamilyBuffer::SetLifespan(time_t theIssueTime, unsigned long theLifespan)
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "setlifespan", MB_OK);
	mBufP->SetLifespan(theIssueTime, theLifespan);
}

void
WON_AuthFamilyBuffer::SetIssueTime(time_t theTime)
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "set issue", MB_OK);
	mBufP->SetIssueTime(theTime);
}

void
WON_AuthFamilyBuffer::SetExpireTime(time_t theTime)
{
	mBufP->SetExpireTime(theTime);
}

const unsigned char*
WON_AuthFamilyBuffer::GetRaw() const
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "getraw", MB_OK);
	return mBufP->GetRaw();
}

unsigned short
WON_AuthFamilyBuffer::GetRawLen() const
{
	return mBufP->GetRawLen();
}

const unsigned char*
WON_AuthFamilyBuffer::GetData() const
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "getdata", MB_OK);
	return mBufP->GetData();
}

unsigned short
WON_AuthFamilyBuffer::GetDataLen() const
{
	return mBufP->GetDataLen();
}

const unsigned char*
WON_AuthFamilyBuffer::GetSignature() const
{
		MessageBox(NULL, "WON_AuthFamilyBuffer", "getsig", MB_OK);
	return mBufP->GetSignature();
}

unsigned short
WON_AuthFamilyBuffer::GetSignatureLen() const
{
	return mBufP->GetSignatureLen();
}

