//#ifndef unicode_h
//#define unicode_h

/////////////////////////////////////////////////
// Includes
//#include "base.h"

typedef int TUniVecIdx;

//-----------------------------------------------------------------------------
// TUniCodec -- an UTF-8 and UTF-16 Encoder/Decoder
//-----------------------------------------------------------------------------

// Error handling modes for the TUniCodec class.
typedef enum TUnicodeErrorHandling_
{
	// What happens when an error occurs:
	uehIgnore = 0,  // - it is silently ignored (nothing is added to the output vector)
	uehThrow = 1,   // - an exception is thrown (TUnicodeException)
	uehReplace = 2, // - the replacement character is added to the output vector
	uehAbort = 3    // - the encoding/decoding process stops immediately
}
TUnicodeErrorHandling;

class TUnicodeException
{
public:
	TStr message;  // error message
	size_t srcIdx; // the position in the source vector where the error occurred
	int srcChar;   // the source character at the position srcIdx
	TUnicodeException(size_t srcIdx_, int srcChar_, const TStr& message_) :
		message(message_), srcIdx(srcIdx_), srcChar(srcChar_) { }
};

typedef enum TUniByteOrder_
{
	boMachineEndian = 0,
	boLittleEndian = 1,
	boBigEndian = 2
}
TUniByteOrder;

typedef enum TUtf16BomHandling_
{
	bomAllowed = 0,   // if a BOM is present, it is used to determine the byte order; otherwise, the default byte order is used
	bomRequired = 1,  // if a BOM is present, it is used to determine the byte order; otherwise, an error is reported
	bomIgnored = 2    // the default byte order is used; if a BOM is present, it is treated like any other character
}
TUtf16BomHandling;

class TUniCodec
{
public:
	// 0xfffd is defined as the replacement character by the Unicode standard.
	// By default, it is rendered as a question mark inside a diamond: "<?>".
	enum { DefaultReplacementChar = 0xfffd };

	// The replacement character is inserted into the destination vector
	// if an error occurs in the source vector.  By default, this is set
	// to DefaultReplacementChar.
	int replacementChar;
	// The error handling mode.
	TUnicodeErrorHandling errorHandling;
	// There are a number of situations where there is strictly speaking an error in
	// the source data although it can still be decoded in a reasonably meaningful way.
	// If strict == true, these situations are treated as errors.  Examples:
	// - when decoding UTF-8:
	//   - a codepoint represented by more bytes than necessary (e.g. one of the characters 0..127
	//     encoded as a two-byte sequence)
	//   - a codepoint > 0x10ffff
	// - when decoding UTF-16:
	//   - a codepoint from the range reserved for the second character of a surrogate pair
	//     is not preceded by a codepoint from the range reserved for the first character of a surrogate pair
	// - when encoding UTF-8:
	//   - a codepoint > 0x10ffff
	// - when encoding UTF-16:
	//   - a codepoint from the range reserved from the second character of a surrogate pair
	//     [note that a codepoint > 0x10ffff, or from the range reserved for the first character of a
	//     surrogate pair, is always an error, even with strict == false]
	bool strict;
	// skipBom == true means: If a byte-order-mark (0xfffe or 0xfeff) occurs at the beginning
	// of the source vector, it is skipped (when decoding).
	// - Note: a BOM is not really useful in UTF-8 encoded data.  However, the .NET UTF8Encoding
	//   emits 0xfeff by default as a kind of preamble.  It gets encoded as 3 bytes, ef bb bf,
	//   and can be helpful to make the data easier to recognize as UTF-8 encoded data.
	bool skipBom;

	TUniCodec() : replacementChar(DefaultReplacementChar), errorHandling(uehIgnore), strict(false), skipBom(true)
	{
	}

	TUniCodec(TUnicodeErrorHandling errorHandling_, bool strict_, int replacementChar_, bool skipBom_) :
		replacementChar(replacementChar_), errorHandling(errorHandling_), strict(strict_), skipBom(skipBom_)
	{
	}

protected:
	enum {
#define DefineByte(b7, b6, b5, b4, b3, b2, b1, b0) _ ## b7 ## b6 ## b5 ## b4 ## _ ## b3 ## b2 ## b1 ## b0 = (b7 << 7) | (b6 << 6) | (b5 << 5) | (b4 << 4) | (b3 << 3) | (b2 << 2) | (b1 << 1) | b0
		DefineByte(1, 0, 0, 0, 0, 0, 0, 0),
		DefineByte(1, 1, 0, 0, 0, 0, 0, 0),
		DefineByte(1, 1, 1, 0, 0, 0, 0, 0),
		DefineByte(1, 1, 1, 1, 0, 0, 0, 0),
		DefineByte(1, 1, 1, 1, 1, 0, 0, 0),
		DefineByte(1, 1, 1, 1, 1, 1, 0, 0),
		DefineByte(1, 1, 1, 1, 1, 1, 1, 0),
		DefineByte(0, 0, 1, 1, 1, 1, 1, 1),
		DefineByte(0, 0, 0, 1, 1, 1, 1, 1),
		DefineByte(0, 0, 0, 0, 1, 1, 1, 1),
		DefineByte(0, 0, 0, 0, 0, 1, 1, 1),
		DefineByte(0, 0, 0, 0, 0, 0, 1, 1)
#undef DefineByte
	};

	typedef TUniVecIdx TVecIdx;
	//friend class TUniChDb;
	friend class TUniCaseFolding;

public:

	//-----------------------------------------------------------------------
	// UTF-8
	//-----------------------------------------------------------------------

	// Returns the number of characters that have been successfully decoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	template<typename TSrcVec, typename TDestCh>
	size_t DecodeUtf8(
		const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, const bool clrDest = true) const;
	template<typename TSrcVec, typename TDestCh>
	size_t DecodeUtf8(const TSrcVec& src, TVec<TDestCh>& dest, const bool clrDest = true) const { return DecodeUtf8(src, 0, src.Len(), dest, clrDest); }

	// Returns the number of characters that have been successfully encoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	template<typename TSrcVec, typename TDestCh>
	size_t EncodeUtf8(
		const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, const bool clrDest = true) const;
	template<typename TSrcVec, typename TDestCh>
	size_t EncodeUtf8(const TSrcVec& src, TVec<TDestCh>& dest, const bool clrDest = true) const { return EncodeUtf8(src, 0, src.Len(), dest, clrDest); }

	// The following wrappers around the UTF-8 encoder return a TStr containing
	// the UTF-8-encoded version of the input string.
	template<typename TSrcVec> TStr EncodeUtf8Str(const TSrcVec& src, size_t srcIdx, const size_t srcCount) const { TVec<char> temp; EncodeUtf8(src, srcIdx, srcCount, temp); TStr retVal = &(temp[0]); return retVal; }
	template<typename TSrcVec> TStr EncodeUtf8Str(const TSrcVec& src) const { TVec<char> temp; EncodeUtf8(src, temp); temp.Add(0); TStr retVal = &(temp[0]); return retVal; }

	//-----------------------------------------------------------------------
	// UTF-16 Decoder
	//-----------------------------------------------------------------------

protected:
	enum {
		Utf16FirstSurrogate = 0xd800,
		Utf16SecondSurrogate = 0xdc00
	};

	static bool IsMachineLittleEndian();

public:

	// Returns the number of characters that have been successfully decoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	// Each element of 'src' is assumed to contain one byte of data.
	// srcCount must be even (though srcIdx doesn't need to be).
	template<typename TSrcVec, typename TDestCh>
	size_t DecodeUtf16FromBytes(
		const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, const bool clrDest,
		const TUtf16BomHandling bomHandling = bomAllowed,
		const TUniByteOrder defaultByteOrder = boMachineEndian) const;

	// Here, each element of 'src' is treated as a 16-bit word.  The byte-order settings
	// are used to determine if the two bytes of each word should be swapped before further
	// processing.  For example, if a BOM is present, it must have the value 0xfeff; if it
	// actually has the value 0xfffe, this means that the two bytes of each word must be swapped.
	// Basically, the combination of the byteOrder parameter and the byte order mark (if present) at the
	// beginning of the source data is used to determine the "original" byte order of the data;
	// if this doesn't match the byte order of the local machine, the two bytes of each word will
	// be swapped during the decoding process.
	template<typename TSrcVec, typename TDestCh>
	size_t DecodeUtf16FromWords(
		const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, bool clrDest,
		const TUtf16BomHandling bomHandling = bomAllowed,
		const TUniByteOrder defaultByteOrder = boMachineEndian) const;

	//-----------------------------------------------------------------------
	// UTF-16 Encoder
	//-----------------------------------------------------------------------

	// Returns the number of characters that have been successfully encoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	//
	// Notes:
	// - UTF-16 *cannot* encode characters above 0x10ffff, so their presence is always
	//   treated as an error, regardless of the value of 'strict'.
	// - Characters from the range Utf16FirstSurrogate through Utf16FirstSurrogate + 1023
	//   cannot be encoded by UTF-16 either, as they would be misinterpreted during decoding
	//   as the first character of a surrogate pair.
	// - Characters from the range Utf16SecondSurrogate through Utf16SecondSurrogate + 1023
	//   can be encoded in principle; however, if strict == true, they are treated as errors.
	template<typename TSrcVec, typename TDestCh>
	size_t EncodeUtf16ToWords(
		const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, const bool clrDest, const bool insertBom,
		const TUniByteOrder destByteOrder = boMachineEndian) const;

	template<typename TSrcVec, typename TDestCh>
	size_t EncodeUtf16ToBytes(
		const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, const bool clrDest, const bool insertBom,
		const TUniByteOrder destByteOrder = boMachineEndian) const;

	//-----------------------------------------------------------------------
	// Helper declarations for the test drivers
	//-----------------------------------------------------------------------

protected:

	static uint GetRndUint(TRnd& rnd);
	static uint GetRndUint(TRnd& rnd, uint minVal, uint maxVal);

	//-----------------------------------------------------------------------
	// UTF-8 Test Driver
	//-----------------------------------------------------------------------

protected:
	void TestUtf8(bool decode, size_t expectedRetVal, bool expectedThrow, const TIntV& src, const TIntV& expectedDest, FILE *f);
	// Generates a random UTF-8-encoded stream according to the specifications in 'testCaseDesc',
	// then calls TestUtf8 to make sure that DecodeUtf8 reacts as expected.
	void TestDecodeUtf8(TRnd& rnd, const TStr& testCaseDesc);
public:
	void TestUtf8();

	//-----------------------------------------------------------------------
	// UTF-16 Test Driver
	//-----------------------------------------------------------------------

protected:
	void WordsToBytes(const TIntV& src, TIntV& dest);
	void TestUtf16(bool decode, size_t expectedRetVal, bool expectedThrow, const TIntV& src, const TIntV& expectedDest,
		// Note: insertBom is only used with the encoder.  When encoding, 'defaultByteOrder' is used as the destination byte order.
		const TUtf16BomHandling bomHandling, const TUniByteOrder defaultByteOrder, const bool insertBom,
		FILE *f);
	static inline int SwapBytes(int x) {
		return ((x >> 8) & 0xff) | ((x & 0xff) << 8); }
	// Generates a random UTF-16-encoded stream according to the specifications in 'testCaseDesc',
	// then calls TestUtf16 to make sure that DecodeUtf16 reacts as expected.
	void TestDecodeUtf16(TRnd& rnd, const TStr& testCaseDesc,
		const TUtf16BomHandling bomHandling,
		const TUniByteOrder defaultByteOrder,
		const bool insertBom);
public:
	void TestUtf16();

};

//-----------------------------------------------------------------------------
// Case folding
//-----------------------------------------------------------------------------
// Note: there's no need to access this class directly.
// Use TUniChDb::GetCaseFolded() instead.

typedef THash<TInt, TIntV> TIntIntVH;

class TUniCaseFolding
{
protected:
	TIntH cfCommon, cfSimple, cfTurkic;
	TIntIntVH cfFull;

	template<typename TSrcDat, typename TDestDat>
	inline static void AppendVector(const TVec<TSrcDat>& src, TVec<TDestDat>& dest) {
		for (int i = 0; i < src.Len(); i++) dest.Add(src[i]); }
	friend class TUniChDb;
	typedef TUniVecIdx TVecIdx;

public:
	TUniCaseFolding() { }
	explicit TUniCaseFolding(TSIn& SIn) : cfCommon(SIn), cfSimple(SIn), cfTurkic(SIn), cfFull(SIn) { SIn.LoadCs(); }
	void Load(TSIn& SIn) { cfCommon.Load(SIn); cfSimple.Load(SIn); cfFull.Load(SIn); cfTurkic.Load(SIn); SIn.LoadCs(); }
	void Save(TSOut& SOut) const { cfCommon.Save(SOut); cfSimple.Save(SOut); cfFull.Save(SOut); cfTurkic.Save(SOut); SOut.SaveCs(); }
	void Clr() { cfCommon.Clr(); cfSimple.Clr(); cfFull.Clr(); cfTurkic.Clr(); }
	void LoadTxt(const TStr& fileName);

	// Use 'turkic' when processing text in a Turkic language (tr, az).  This only affects the uppercase I and I-with-dot-above.
	template<typename TSrcVec, typename TDestCh>
	void Fold(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, const bool clrDest, const bool full, const bool turkic) const
	{
		for (const size_t srcEnd = srcIdx + srcCount; srcIdx < srcEnd; )
		{
			int c = src[TVecIdx(srcIdx)], i; srcIdx++;
			if (turkic && ((i = cfTurkic.GetKeyId(c)) >= 0)) { dest.Add(cfTurkic[i]); continue; }
			if (full && ((i = cfFull.GetKeyId(c)) >= 0)) { AppendVector(cfFull[i], dest); continue; }
			if ((! full) && ((i = cfSimple.GetKeyId(c)) >= 0)) { dest.Add(cfSimple[i]); continue; }
			i = cfCommon.GetKeyId(c); if (i >= 0) dest.Add(cfCommon[i]); else dest.Add(c);
		}
	}

	template<typename TSrcVec>
	void FoldInPlace(TSrcVec& src, size_t srcIdx, const size_t srcCount, const bool turkic) const
	{
		for (const size_t srcEnd = srcIdx + srcCount; srcIdx < srcEnd; srcIdx++)
		{
			int c = src[TVecIdx(srcIdx)], i;
			if (turkic && ((i = cfTurkic.GetKeyId(c)) >= 0)) { src[TVecIdx(srcIdx)] = cfTurkic[i]; continue; }
			if ((i = cfSimple.GetKeyId(c)) >= 0) { src[TVecIdx(srcIdx)] = cfSimple[i]; continue; }
			i = cfCommon.GetKeyId(c); if (i >= 0) src[TVecIdx(srcIdx)] = cfCommon[i];
		}
	}

protected:
	void Test(const TIntV& src, const TIntV& expectedDest, const bool full, const bool turkic, FILE *f);
public:
	void Test();
};

//-----------------------------------------------------------------------------
// TCodecBase -- an abstract base class for codecs
//-----------------------------------------------------------------------------

class TCodecBase;
typedef TPt<TCodecBase> PCodecBase;
typedef TVec<PCodecBase> TCodecBaseV;

class TCodecBase
{
protected:
	TCRef CRef;
	friend class TPt<TCodecBase>;
public:
	virtual ~TCodecBase() { }

	template<class TCodecImpl>
	static PCodecBase New(); /* {
		return new TCodecWrapper<TCodecImpl>(); } */

	virtual TStr GetName() const = 0;
	virtual void Test() const { }

	// Returns the number of characters that have been successfully decoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	virtual size_t ToUnicode(const TIntV& src, size_t srcIdx, const size_t srcCount, TIntV& dest, const bool clrDest = true) const = 0;
	virtual size_t ToUnicode(const TStr& src, size_t srcIdx, const size_t srcCount, TIntV& dest, const bool clrDest = true) const = 0;

	size_t ToUnicode(const TIntV& src, TIntV& dest, const bool clrDest = true) const { return ToUnicode(src, 0, src.Len(), dest, clrDest); }
	size_t ToUnicode(const TStr& src, TIntV& dest, const bool clrDest = true) const { return ToUnicode(src, 0, src.Len(), dest, clrDest); }

	// Returns the number of characters that have been successfully encoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	virtual size_t FromUnicode(const TIntV& src, size_t srcIdx, const size_t srcCount, TIntV& dest, const bool clrDest = true) const = 0;
	virtual size_t FromUnicode(const TIntV& src, size_t srcIdx, const size_t srcCount, TChA& dest, const bool clrDest = true) const = 0;
	virtual size_t FromUnicode(const TIntV& src, size_t srcIdx, const size_t srcCount, TStr& dest, const bool clrDest = true) const = 0;

	size_t FromUnicode(const TIntV& src, TIntV& dest, const bool clrDest = true) const { return FromUnicode(src, 0, src.Len(), dest, clrDest); }
	size_t FromUnicode(const TIntV& src, TChA& dest, const bool clrDest = true) const { return FromUnicode(src, 0, src.Len(), dest, clrDest); }
	size_t FromUnicode(const TIntV& src, TStr& dest, const bool clrDest = true) const { return FromUnicode(src, 0, src.Len(), dest, clrDest); }
};

//-----------------------------------------------------------------------------
// TCodecWrapper -- a descendant of TCodecBase; relies on a template
// parameter class for the actual implementation of the codec.
//-----------------------------------------------------------------------------
// Thus, if you know in advance that you'll need ISO-8859-2, just use
// T8BitCodec<TEncoding_ISO8859_2>.  If you don't know the encoding
// in advance, use a PCodecBase pointing to a suitable specialization
// of TCodecWrapper<...>.  You can TUnicode::GetCodec(TStr& name)
// to obtain a suitable pointer.

template<class TCodecImpl_>
class TCodecWrapper : public TCodecBase
{
public:
	typedef TCodecImpl_ TCodecImpl;
	TCodecImpl impl;
public:

	virtual TStr GetName() const { return impl.GetName(); }

	virtual void Test() const { impl.Test(); }

	virtual size_t ToUnicode(const TIntV& src, size_t srcIdx, const size_t srcCount, TIntV& dest, const bool clrDest = true) const {
		return impl.ToUnicode(src, srcIdx, srcCount, dest, clrDest); }
	virtual size_t ToUnicode(const TStr& src, size_t srcIdx, const size_t srcCount, TIntV& dest, const bool clrDest = true) const {
		return impl.ToUnicode(src, srcIdx, srcCount, dest, clrDest); }

	virtual size_t FromUnicode(const TIntV& src, size_t srcIdx, const size_t srcCount, TIntV& dest, const bool clrDest = true) const {
		return impl.FromUnicode(src, srcIdx, srcCount, dest, clrDest); }
	virtual size_t FromUnicode(const TIntV& src, size_t srcIdx, const size_t srcCount, TChA& dest, const bool clrDest = true) const {
		return impl.FromUnicode(src, srcIdx, srcCount, dest, clrDest); }
	virtual size_t FromUnicode(const TIntV& src, size_t srcIdx, const size_t srcCount, TStr& dest, const bool clrDest = true) const {
		TChA buf; size_t retVal = impl.FromUnicode(src, srcIdx, srcCount, buf, false);
		if (clrDest) dest += buf.CStr(); else dest = buf.CStr();
		return retVal; }
};

template<class TCodecImpl>
PCodecBase TCodecBase::New() {
  return new TCodecWrapper<TCodecImpl>();
}

//-----------------------------------------------------------------------------
// TVecElt -- a template for determining the type of a vector's elements
//-----------------------------------------------------------------------------

template<class TVector_>
class TVecElt
{
};

template<class TDat>
class TVecElt<TVec<TDat> >
{
public:
	typedef TVec<TDat> TVector;
	typedef TDat TElement;
	static inline void Add(TVector& vector, const TElement& element) { vector.Add(element); }
};

template<>
class TVecElt<TChA>
{
public:
	typedef TChA TVector;
	typedef char TElement;
	static inline void Add(TVector& vector, const TElement& element) { vector += element; }
};


//-----------------------------------------------------------------------------
// T8BitCodec -- a class for converting between 8-bit encodings and Unicode
//-----------------------------------------------------------------------------

class TEncoding_ISO8859_1
{
public:
	static inline TStr GetName() { return "ISO-8859-1"; }
	static int ToUnicode(int c) { Assert(0 <= c && c <= 255); return c; }
	static int FromUnicode(int c) { if (0 <= c && c <= 255) return c; else return -1; }
};

class TEncoding_ISO8859_2 // ISO Latin 2
{
public:
	static inline TStr GetName() { return "ISO-8859-2"; }
	static const int toUnicodeTable[6 * 16], fromUnicodeTable1[14 * 16], fromUnicodeTable2[2 * 16];
	static int ToUnicode(int c) { Assert(0 <= c && c <= 255);
		if (c < 0xa0) return c; else return toUnicodeTable[c - 0xa0]; }
	static int FromUnicode(int c) {
		if (0 <= c && c < 0xa0) return c;
		else if (0xa0 <= c && c < 0x180) return fromUnicodeTable1[c - 0xa0];
		else if (0x2c0 <= c && c < 0x2e0) return fromUnicodeTable2[c - 0x2c0];
		else return -1; }
};

class TEncoding_ISO8859_3
{
public:
	static inline TStr GetName() { return "ISO-8859-3"; }
	static const int toUnicodeTable[6 * 16], fromUnicodeTable1[14 * 16], fromUnicodeTable2[2];
	static int ToUnicode(int c) { Assert(0 <= c && c <= 255);
		if (c < 0xa0) return c; else return toUnicodeTable[c - 0xa0]; }
	static int FromUnicode(int c) {
		if (0 <= c && c < 0xa0) return c;
		else if (0xa0 <= c && c < 0x180) return fromUnicodeTable1[c - 0xa0];
		else if (0x2d8 <= c && c < 0x2da) return fromUnicodeTable2[c - 0x2d8];
		else return -1; }
};

class TEncoding_ISO8859_4
{
public:
	static inline TStr GetName() { return "ISO-8859-4"; }
	static const int toUnicodeTable[6 * 16], fromUnicodeTable1[14 * 16], fromUnicodeTable2[2 * 16];
	static int ToUnicode(int c) { Assert(0 <= c && c <= 255);
		if (c < 0xa0) return c; else return toUnicodeTable[c - 0xa0]; }
	static int FromUnicode(int c) {
		if (0 <= c && c < 0xa0) return c;
		else if (0xa0 <= c && c < 0x180) return fromUnicodeTable1[c - 0xa0];
		else if (0x2c0 <= c && c < 0x2e0) return fromUnicodeTable2[c - 0x2c0];
		else return -1; }
};

class TEncoding_YuAscii
{
public:
	static const int uniChars[10], yuAsciiChars[10];
	static inline TStr GetName() { return "YU-ASCII"; }
	static int ToUnicode(int c) { Assert(0 <= c && c <= 255);
		for (int i = 0; i < int(sizeof(yuAsciiChars) / sizeof(yuAsciiChars[0])); i++)
			if (c == yuAsciiChars[i]) return uniChars[i];
		return c; }
	static int FromUnicode(int c) {
		for (int i = 0; i < int(sizeof(uniChars) / sizeof(uniChars[0])); i++)
			if (c == uniChars[i]) return yuAsciiChars[i];
			else if(c == yuAsciiChars[i]) return -1;
		if (0 <= c && c <= 255) return c; else return -1; }
};

class TEncoding_CP437 // DOS US
{
public:
	static inline TStr GetName() { return "CP437"; }
	static const int toUnicodeTable[8 * 16], fromUnicodeTable1[6 * 16], fromUnicodeTable2[4 * 16], fromUnicodeTable3[6 * 16], fromUnicodeTable4[11 * 16];
	static int ToUnicode(int c) { Assert(0 <= c && c <= 255);
		if (c < 0x80) return c; else return toUnicodeTable[c - 0x80]; }
	static int FromUnicode(int c) {
		if (0 <= c && c < 0x80) return c;
		else if (0xa0 <= c && c < 0x100) return fromUnicodeTable1[c - 0xa0];
		else if (0x390 <= c && c < 0x3d0) return fromUnicodeTable2[c - 0x390];
		else if (0x2210 <= c && c < 0x2270) return fromUnicodeTable3[c - 0x2210];
		else if (0x2500 <= c && c < 0x25b0) return fromUnicodeTable4[c - 0x2500];
		else if (c == 0x192) return 0x9f;
		else if (c == 0x207f) return 0xfc;
		else if (c == 0x20a7) return 0x9e;
		else if (c == 0x2310) return 0xa9;
		else if (c == 0x2320) return 0xf4;
		else if (c == 0x2321) return 0xf5;
		else return -1; }
};

class TEncoding_CP852 // DOS Latin 2
{
public:
	static inline TStr GetName() { return "CP852"; }
	static const int toUnicodeTable[8 * 16], fromUnicodeTable1[14 * 16], fromUnicodeTable2[2 * 16], fromUnicodeTable3[11 * 16];
	static int ToUnicode(int c) { Assert(0 <= c && c <= 255);
		if (c < 0x80) return c; else return toUnicodeTable[c - 0x80]; }
	static int FromUnicode(int c) {
		if (0 <= c && c < 0x80) return c;
		else if (0xa0 <= c && c < 0x180) return fromUnicodeTable1[c - 0xa0];
		else if (0x2c0 <= c && c < 0x2e0) return fromUnicodeTable2[c - 0x2c0];
		else if (0x2500 <= c && c < 0x25b0) return fromUnicodeTable3[c - 0x2500];
		else return -1; }
};

class TEncoding_CP1250 // Windows-1250, similar to ISO Latin 2
{
public:
	static inline TStr GetName() { return "CP1250"; }
	static const int toUnicodeTable[8 * 16], fromUnicodeTable1[14 * 16], fromUnicodeTable2[2 * 16], fromUnicodeTable3[3 * 16];
	static int ToUnicode(int c) { Assert(0 <= c && c <= 255);
		if (c < 0x80) return c; else return toUnicodeTable[c - 0x80]; }
	static int FromUnicode(int c) {
		if (0 <= c && c < 0x80) return c;
		else if (0xa0 <= c && c < 0x180) return fromUnicodeTable1[c - 0xa0];
		else if (0x2c0 <= c && c < 0x2e0) return fromUnicodeTable2[c - 0x2c0];
		else if (0x2010 <= c && c < 0x2040) return fromUnicodeTable3[c - 0x2010];
		else if (c == 0x20ac) return 0x80;
		else if (c == 0x2122) return 0x99;
		else return -1; }
};

template<class TEncoding_>
class T8BitCodec
{
protected:
	typedef TUniVecIdx TVecIdx;
public:
	typedef TEncoding_ TEncoding;
	TUnicodeErrorHandling errorHandling;
	int replacementChar;

	T8BitCodec() : errorHandling(uehIgnore), replacementChar(TUniCodec::DefaultReplacementChar) { }
	T8BitCodec(TUnicodeErrorHandling errorHandling_, int replacementChar_ = TUniCodec::DefaultReplacementChar) :
		errorHandling(errorHandling_), replacementChar(replacementChar_) { }
	static TStr GetName() { return TEncoding::GetName(); }

	void Test() const
	{
		int nDecoded = 0;
		for (int c = 0; c <= 255; c++) {
			int cu = TEncoding::ToUnicode(c); if (cu == -1) continue;
			nDecoded++;
			IAssert(0 <= cu && cu < 0x110000);
			int c2 = TEncoding::FromUnicode(cu);
			IAssert(c2 == c); }
		int nEncoded = 0;
		for (int cu = 0; cu < 0x110000; cu++) {
			int c = TEncoding::FromUnicode(cu); if (c == -1) continue;
			nEncoded++;
			IAssert(0 <= c && c <= 255);
			int cu2 = TEncoding::ToUnicode(c);
			IAssert(cu2 == cu); }
		IAssert(nDecoded == nEncoded);
	}

	// Returns the number of characters that have been successfully decoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	template<typename TSrcVec, typename TDestCh>
	size_t ToUnicode(
		const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, const bool clrDest = true) const
	{
		if (clrDest) dest.Clr();
		size_t toDo = srcCount;
		while (toDo-- > 0) {
			int chSrc = ((int) src[TVecIdx(srcIdx)]) & 0xff; srcIdx++;
			int chDest = TEncoding::ToUnicode(chSrc);
			dest.Add(chDest); }
		return srcCount;
	}
	template<typename TSrcVec, typename TDestCh>
	size_t ToUnicode(const TSrcVec& src, TVec<TDestCh>& dest, const bool clrDest = true) const { return ToUnicode(src, 0, src.Len(), dest, clrDest); }

	size_t ToUnicode(const TIntV& src, TIntV& dest, const bool clrDest = true) const { return ToUnicode(src, 0, src.Len(), dest, clrDest); }
	size_t ToUnicode(const TStr& src, TIntV& dest, const bool clrDest = true) const { return ToUnicode(src, 0, src.Len(), dest, clrDest); }

	// Returns the number of characters that have been successfully encoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	template<typename TSrcVec, typename TDestVec>
	size_t FromUnicode(
		const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TDestVec& dest, const bool clrDest = true) const
	{
		typedef typename TVecElt<TDestVec>::TElement TDestCh;
		if (clrDest) dest.Clr();
		size_t toDo = srcCount, nEncoded = 0;
		while (toDo-- > 0) {
			int chSrc = (int) src[TVecIdx(srcIdx)]; srcIdx++;
			int chDest = TEncoding::FromUnicode(chSrc);
			if (chDest < 0) {
				switch (errorHandling) {
				case uehThrow: throw TUnicodeException(srcIdx - 1, chSrc, "Invalid character for encoding into " + GetName() + ".");
				case uehAbort: return nEncoded;
				case uehReplace: TVecElt<TDestVec>::Add(dest, TDestCh(replacementChar)); continue;
				case uehIgnore: continue;
				default: Fail; } }
			TVecElt<TDestVec>::Add(dest, TDestCh(chDest)); nEncoded++; }
		return nEncoded;
	}

	template<typename TSrcVec, typename TDestVec>
	size_t FromUnicode(const TSrcVec& src, TDestVec& dest, const bool clrDest = true) const { return FromUnicode(src, 0, src.Len(), dest, clrDest); }

	size_t UniToStr(const TIntV& src, size_t srcIdx, const size_t srcCount, TStr& dest, const bool clrDest = true) const {
		TChA buf; size_t retVal = FromUnicode(src, srcIdx, srcCount, buf, false);
		if (clrDest) dest += buf.CStr(); else dest = buf.CStr();
		return retVal; }
	size_t UniToStr(const TIntV& src, TStr& dest, const bool clrDest = true) const { return UniToStr(src, 0, src.Len(), dest, clrDest); }
};

typedef T8BitCodec<TEncoding_ISO8859_1> TCodec_ISO8859_1;
typedef T8BitCodec<TEncoding_ISO8859_2> TCodec_ISO8859_2;
typedef T8BitCodec<TEncoding_ISO8859_3> TCodec_ISO8859_3;
typedef T8BitCodec<TEncoding_ISO8859_4> TCodec_ISO8859_4;
typedef T8BitCodec<TEncoding_CP852> TCodec_CP852;
typedef T8BitCodec<TEncoding_CP437> TCodec_CP437;
typedef T8BitCodec<TEncoding_CP1250> TCodec_CP1250;
typedef T8BitCodec<TEncoding_YuAscii> TCodec_YuAscii;

//-----------------------------------------------------------------------------
// Various declarations used by the Unicode Character Database
//-----------------------------------------------------------------------------

typedef enum TUniChCategory_
{
#define DefineUniCat(cat, c) uc ## cat = (int(uchar(c)) & 0xff)
	DefineUniCat(Letter, 'L'),             // ucLetter
	DefineUniCat(Mark, 'M'),
	DefineUniCat(Number, 'N'),
	DefineUniCat(Punctuation, 'P'),
	DefineUniCat(Symbol, 'S'),
	DefineUniCat(Separator, 'Z'),
	DefineUniCat(Other, 'C')
#undef DefineUniCat
}
TUniChCategory;

typedef enum TUniChSubCategory_
{
#define DefineUniSubCat(cat, subCat, c) uc ## cat ## subCat = ((uc ## cat) << 8) | (int(uchar(c)) & 0xff)
	DefineUniSubCat(Letter, Uppercase, 'u'),            // ucLetterUppercase
	DefineUniSubCat(Letter, Lowercase, 'l'),
	DefineUniSubCat(Letter, Titlecase, 't'),
	DefineUniSubCat(Letter, Modifier, 'm'),
	DefineUniSubCat(Letter, Other, 'o'),
	DefineUniSubCat(Mark, Nonspacing, 'n'),
	DefineUniSubCat(Mark, SpacingCombining, 'c'),
	DefineUniSubCat(Mark, Enclosing, 'e'),
	DefineUniSubCat(Number, DecimalDigit, 'd'),
	DefineUniSubCat(Number, Letter, 'l'),
	DefineUniSubCat(Number, Other, 'o'),
	DefineUniSubCat(Punctuation, Connector, 'c'),
	DefineUniSubCat(Punctuation, Dash, 'd'),
	DefineUniSubCat(Punctuation, Open, 's'),
	DefineUniSubCat(Punctuation, Close, 'e'),
	DefineUniSubCat(Punctuation, InitialQuote, 'i'),
	DefineUniSubCat(Punctuation, FinalQuote, 'f'),
	DefineUniSubCat(Punctuation, Other, 'o'),
	DefineUniSubCat(Symbol, Math, 'm'),
	DefineUniSubCat(Symbol, Currency, 'c'),
	DefineUniSubCat(Symbol, Modifier, 'k'),
	DefineUniSubCat(Symbol, Other, 'o'),
	DefineUniSubCat(Separator, Space, 's'),
	DefineUniSubCat(Separator, Line, 'l'),
	DefineUniSubCat(Separator, Paragraph, 'p'),
	DefineUniSubCat(Other, Control, 'c'),
	DefineUniSubCat(Other, Format, 'f'),
	DefineUniSubCat(Other, Surrogate, 's'),
	DefineUniSubCat(Other, PrivateUse, 'o'),
	DefineUniSubCat(Other, NotAssigned, 'n')
}
TUniChSubCategory;

typedef enum TUniChFlags_
{
	ucfCompatibilityDecomposition = 1, // if this flag is not set, the decomposition is canonical
	ucfCompositionExclusion = 1 << 1,       // from CompositionExclusions.txt
	// Flags used when searching for word boundaries.  See UAX #29.
	ucfWbFormat = 1 << 2,
	ucfWbKatakana = 1 << 3,
	ucfWbALetter = 1 << 4,
	ucfWbMidLetter = 1 << 5,
	ucfWbMidNum = 1 << 6,
	ucfWbNumeric = 1 << 7,
	ucfWbExtendNumLet = 1 << 8,
	// Flags used with sentence boundaries (Sep is also used with word boundaries).  See UAX #29.
	ucfSbSep = 1 << 9,
	ucfSbFormat = 1 << 10,
	ucfSbSp = 1 << 11,
	ucfSbLower = 1 << 12,
	ucfSbUpper = 1 << 13,
	ucfSbOLetter = 1 << 14,
	ucfSbNumeric = 1 << 15,
	ucfSbATerm = 1 << 16,
	ucfSbSTerm = 1 << 17,
	ucfSbClose = 1 << 18,
	ucfSbMask = ucfSbSep | ucfSbFormat | ucfSbSp | ucfSbLower | ucfSbUpper | ucfSbOLetter | ucfSbNumeric | ucfSbATerm | ucfSbSTerm | ucfSbClose,
	ucfWbMask = ucfWbFormat | ucfWbKatakana | ucfWbALetter | ucfWbMidLetter | ucfWbMidNum | ucfWbNumeric | ucfWbExtendNumLet | ucfSbSep,
	// Flags from DerivedCoreProperties.txt.
	// [The comments are from UCD.html.]
	// - Characters with the Alphabetic property. For more information, see Chapter 4 in [Unicode].
	//   Generated from: Other_Alphabetic + Lu + Ll + Lt + Lm + Lo + Nl
	ucfDcpAlphabetic = 1 << 19,
	// - For programmatic determination of default-ignorable code points.
	//   New characters that should be ignored in processing (unless explicitly supported)
	//   will be assigned in these ranges, permitting programs to correctly handle the default
	//   behavior of such characters when not otherwise supported.  For more information, see
	//   UAX #29: Text Boundaries [Breaks].
	//   Generated from Other_Default_Ignorable_Code_Point + Cf + Cc + Cs + Noncharacters - White_Space - annotation characters
	//   [Examples: soft hyphen, zero-width space, noncharacters (e.g. U+fffe, U+ffff, U+1fffe, U+1ffff, etc.), surrogates, language tags, variation selectors]
	ucfDcpDefaultIgnorableCodePoint = 1 << 20,
	// - Characters with the Lowercase property.  For more information, see Chapter 4 in [Unicode].
	//   Generated from: Other_Lowercase + Ll
	ucfDcpLowercase = 1 << 21,
	// - For programmatic determination of grapheme cluster boundaries.
	//   For more information, see UAX #29: Text Boundaries [Breaks].
	//   Generated from: [0..10FFFF] - Cc - Cf - Cs - Co - Cn - Zl - Zp - Grapheme_Extend
	ucfDcpGraphemeBase = 1 << 22,
	// - For programmatic determination of grapheme cluster boundaries.
	//   For more information, see UAX #29: Text Boundaries [Breaks].
	//   Generated from: Other_Grapheme_Extend + Me + Mn
	//   Note: depending on an application's interpretation of Co (private use), they may be either
	//         in Grapheme_Base, or in Grapheme_Extend, or in neither.
	ucfDcpGraphemeExtend = 1 << 23,
	// - Used to determine programming identifiers, as described in UAX #31: Identifier and Pattern Syntax.
	ucfDcpIdStart = 1 << 24,
	ucfDcpIdContinue = 1 << 25,
	// - Characters with the Math property. For more information, see Chapter 4 in [Unicode].
	//   Generated from: Sm + Other_Math
	ucfDcpMath = 1 << 26,
	// - Characters with the Uppercase property. For more information, see Chapter 4 in [Unicode].
	//   Generated from: Lu + Other_Uppercase
	ucfDcpUppercase = 1 << 27,
	// - Used to determine programming identifiers, as described in UAX #31: Identifier and Pattern Syntax.
	ucfDcpXidStart = 1 << 28,
	ucfDcpXidContinue = 1 << 29,
	ucfDcpMask = ucfDcpAlphabetic | ucfDcpDefaultIgnorableCodePoint | ucfDcpLowercase | ucfDcpGraphemeBase | ucfDcpGraphemeExtend |
		ucfDcpIdStart | ucfDcpIdContinue | ucfDcpMath | ucfDcpUppercase | ucfDcpXidStart | ucfDcpXidContinue,
}
TUniChFlags;

typedef enum TUniChProperties_
{
	// The flags from PropList.txt.
	// [The comments are from UCD.html.]
	// - ASCII characters commonly used for the representation of hexadecimal numbers.
	//   [= 0123456789abcdefABCDEF]
	ucfPrAsciiHexDigit = 1,
	// - Those format control characters which have specific functions in the Bidirectional Algorithm.
	ucfPrBidiControl = 2,
	// - Those punctuation characters explicitly called out as dashes in the Unicode Standard,
	//   plus compatibility equivalents to those. Most of these have the Pd General Category,
	//   but some have the Sm General Category because of their use in mathematics.
	//     U+0002d  HYPHEN-MINUS
	//     U+0058a  ARMENIAN HYPHEN
	//     U+005be  HEBREW PUNCTUATION MAQAF
	//     U+01806  MONGOLIAN TODO SOFT HYPHEN
	//     U+02010  HYPHEN
	//     U+02011  NON-BREAKING HYPHEN
	//     U+02012  FIGURE DASH
	//     U+02013  EN DASH
	//     U+02014  EM DASH
	//     U+02015  HORIZONTAL BAR
	//     U+02053  SWUNG DASH
	//     U+0207b  SUPERSCRIPT MINUS
	//     U+0208b  SUBSCRIPT MINUS
	//     U+02212  MINUS SIGN
	//     U+02e17  DOUBLE OBLIQUE HYPHEN
	//     U+0301c  WAVE DASH
	//     U+03030  WAVY DASH
	//     U+030a0  KATAKANA-HIRAGANA DOUBLE HYPHEN
	//     U+0fe31  PRESENTATION FORM FOR VERTICAL EM DASH
	//     U+0fe32  PRESENTATION FORM FOR VERTICAL EN DASH
	//     U+0fe58  SMALL EM DASH
	//     U+0fe63  SMALL HYPHEN-MINUS
	//     U+0ff0d  FULLWIDTH HYPHEN-MINUS
	ucfPrDash = 4,
	// - For a machine-readable list of deprecated characters.  No characters will ever be removed
	//   from the standard, but the usage of deprecated characters is strongly discouraged.
	ucfPrDeprecated = 8,
	// - Characters that linguistically modify the meaning of another character to which they apply.
	//   Some diacritics are not combining characters, and some combining characters are not diacritics.
	ucfPrDiacritic = 0x10,
	// - Characters whose principal function is to extend the value or shape of a preceding alphabetic
	//   character.  Typical of these are length and iteration marks.
	ucfPrExtender = 0x20,
	// - Used in determining default grapheme cluster boundaries.  For more information, see UAX #29: Text Boundaries.
	ucfPrGraphemeLink = 0x40,
	// - Characters commonly used for the representation of hexadecimal numbers, plus their compatibility equivalents.
	//   [= AsciiHexDigit + fullwidth digit {0..9} + fullwidth latin {small|capital} letter {a..f}]
	ucfPrHexDigit = 0x80,
	// - Those dashes used to mark connections between pieces of words, plus the Katakana middle dot.
	//   The Katakana middle dot functions like a hyphen, but is shaped like a dot rather than a dash.
	//     U+0002d  HYPHEN-MINUS
	//     U+000ad  SOFT HYPHEN
	//     U+0058a  ARMENIAN HYPHEN
	//     U+01806  MONGOLIAN TODO SOFT HYPHEN
	//     U+02010  HYPHEN
	//     U+02011  NON-BREAKING HYPHEN
	//     U+02e17  DOUBLE OBLIQUE HYPHEN
	//     U+030fb  KATAKANA MIDDLE DOT
	//     U+0fe63  SMALL HYPHEN-MINUS
	//     U+0ff0d  FULLWIDTH HYPHEN-MINUS
	//     U+0ff65  HALFWIDTH KATAKANA MIDDLE DOT
	ucfPrHyphen = 0x100,
	// - Characters considered to be CJKV (Chinese, Japanese, Korean, and Vietnamese) ideographs.
	ucfPrIdeographic = 0x200,
	// - Those format control characters which have specific functions for control of cursive joining and ligation.
	ucfPrJoinControl = 0x400,
	// - There are a small number of characters that do not use logical order.
	//   These characters require special handling in most processing.
	ucfPrLogicalOrderException = 0x800,
	// - Code points that are permanently reserved for internal use.
	ucfPrNoncharacterCodePoint = 0x1000,
	// - Used for pattern syntax as described in UAX #31: Identifier and Pattern Syntax.
	ucfPrPatternSyntax = 0x2000,
	ucfPrPatternWhiteSpace = 0x4000,
	// - Those punctuation characters that function as quotation marks.
	//     U+00022  QUOTATION MARK
	//     U+00027  APOSTROPHE
	//     U+000ab  LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
	//     U+000bb  RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
	//     U+02018  LEFT SINGLE QUOTATION MARK
	//     U+02019  RIGHT SINGLE QUOTATION MARK
	//     U+0201a  SINGLE LOW-9 QUOTATION MARK
	//     U+0201b  SINGLE HIGH-REVERSED-9 QUOTATION MARK
	//     U+0201c  LEFT DOUBLE QUOTATION MARK
	//     U+0201d  RIGHT DOUBLE QUOTATION MARK
	//     U+0201e  DOUBLE LOW-9 QUOTATION MARK
	//     U+0201f  DOUBLE HIGH-REVERSED-9 QUOTATION MARK
	//     U+02039  SINGLE LEFT-POINTING ANGLE QUOTATION MARK
	//     U+0203a  SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
	//     U+0300c  LEFT CORNER BRACKET
	//     U+0300d  RIGHT CORNER BRACKET
	//     U+0300e  LEFT WHITE CORNER BRACKET
	//     U+0300f  RIGHT WHITE CORNER BRACKET
	//     U+0301d  REVERSED DOUBLE PRIME QUOTATION MARK
	//     U+0301e  DOUBLE PRIME QUOTATION MARK
	//     U+0301f  LOW DOUBLE PRIME QUOTATION MARK
	//     U+0fe41  PRESENTATION FORM FOR VERTICAL LEFT CORNER BRACKET
	//     U+0fe42  PRESENTATION FORM FOR VERTICAL RIGHT CORNER BRACKET
	//     U+0fe43  PRESENTATION FORM FOR VERTICAL LEFT WHITE CORNER BRACKET
	//     U+0fe44  PRESENTATION FORM FOR VERTICAL RIGHT WHITE CORNER BRACKET
	//     U+0ff02  FULLWIDTH QUOTATION MARK
	//     U+0ff07  FULLWIDTH APOSTROPHE
	//     U+0ff62  HALFWIDTH LEFT CORNER BRACKET
	//     U+0ff63  HALFWIDTH RIGHT CORNER BRACKET
	ucfPrQuotationMark = 0x8000,
	// - Characters with a "soft dot", like i or j. An accent placed on these characters causes the dot to disappear.
	//   An explicit _dot above_ can be added where required, such as in Lithuanian.
	ucfPrSoftDotted = 0x10000,
	// - Sentence Terminal. Used in UAX #29: Text Boundaries.
	//     U+00021  EXCLAMATION MARK
	//     U+0002e  FULL STOP
	//     U+0003f  QUESTION MARK
	//     U+0203c  DOUBLE EXCLAMATION MARK
	//     U+0203d  INTERROBANG
	//     U+02047  DOUBLE QUESTION MARK
	//     U+02048  QUESTION EXCLAMATION MARK
	//     U+02049  EXCLAMATION QUESTION MARK
	//     U+03002  IDEOGRAPHIC FULL STOP
	//     [plus many characters from other writing systems]
	ucfPrSTerm = 0x20000,
	// - Those punctuation characters that generally mark the end of textual units.
	//   [JB note: this set contains more character than STerm.  For example, it contains
	//   the comma, colon and semicolon, whereas STerm doesn't.]
	//     U+00021  EXCLAMATION MARK
	//     U+0002c  COMMA
	//     U+0002e  FULL STOP
	//     U+0003a  COLON
	//     U+0003b  SEMICOLON
	//     U+0003f  QUESTION MARK
	//     U+0203c  DOUBLE EXCLAMATION MARK
	//     U+0203d  INTERROBANG
	//     U+02047  DOUBLE QUESTION MARK
	//     U+02048  QUESTION EXCLAMATION MARK
	//     U+02049  EXCLAMATION QUESTION MARK
	//     [plus *lots* of charcters from other writing systems]
	ucfPrTerminalPunctuation = 0x40000,
	// - Indicates all those characters that qualify as Variation Selectors.
	//   For details on the behavior of these characters, see StandardizedVariants.html and
	//   Section 16.4, Variation Selectors in [Unicode].
	ucfPrVariationSelector = 0x80000,
	// - Those separator characters and control characters which should be treated by
	//   programming languages as "white space" for the purpose of parsing elements.
	//   Note: ZERO WIDTH SPACE and ZERO WIDTH NO-BREAK SPACE are not included,
	//         since their functions are restricted to line-break control.
	//         Their names are unfortunately misleading in this respect.
	//   Note: There are other senses of "whitespace" that encompass a different set of characters.
	//         [JB note: e.g. there's a BIDI class for whitespace ('WS') in UnicodeData.txt.
	//         There's also a "Sp" class in the sentence boundary algorithm, see UAX #29, sec. 5.1.]
	//   This includes the following characters:
	//     U+0009  <control>
	//     U+000a  <control>
	//     U+000b  <control>
	//     U+000c  <control>
	//     U+000d  <control>
	//     U+0020  SPACE
	//     U+0085  <control>
	//     U+00a0  NO-BREAK SPACE
	//     U+1680  OGHAM SPACE MARK
	//     U+180e  MONGOLIAN VOWEL SEPARATOR
	//     U+2000  EN QUAD
	//     U+2001  EM QUAD
	//     U+2002  EN SPACE
	//     U+2003  EM SPACE
	//     U+2004  THREE-PER-EM SPACE
	//     U+2005  FOUR-PER-EM SPACE
	//     U+2006  SIX-PER-EM SPACE
	//     U+2007  FIGURE SPACE
	//     U+2008  PUNCTUATION SPACE
	//     U+2009  THIN SPACE
	//     U+200a  HAIR SPACE
	//     U+2028  LINE SEPARATOR
	//     U+2029  PARAGRAPH SEPARATOR
	//     U+202f  NARROW NO-BREAK SPACE
	//     U+205f  MEDIUM MATHEMATICAL SPACE
	//     U+3000  IDEOGRAPHIC SPACE
	ucfPrWhiteSpace = 0x100000
}
TUniChProperties;

typedef enum TUniChPropertiesX_
{
	// More properties from PropList.txt.
	// - Used to derive the properties in DerivedCoreProperties.txt.
	ucfPxOtherAlphabetic = 1,
	ucfPxOtherDefaultIgnorableCodePoint = 2,
	ucfPxOtherGraphemeExtend = 4,
	ucfPxOtherIdContinue = 8,
	ucfPxOtherIdStart = 0x10,
	ucfPxOtherLowercase = 0x20,
	ucfPxOtherMath = 0x40,
	ucfPxOtherUppercase = 0x80,
	// - Used in ideographic description sequences.
	ucfPxIdsBinaryOperator = 0x100,
	ucfPxIdsTrinaryOperator = 0x200,
	ucfPxRadical = 0x400,
	ucfPxUnifiedIdeograph = 0x800
}
TUniChPropertiesX;

//-----------------------------------------------------------------------------
// TUniChInfo -- contains information about a single Unicode codepoint
//-----------------------------------------------------------------------------

class TUniChInfo
{
public:
	enum { // combining classes (for 'combClass'); from UnicodeData.txt
		ccStarter = 0, // 0: Spacing, split, enclosing, reordrant, and Tibetan subjoined
		ccOverlaysAndInterior = 1,
		ccNuktas = 7,
		ccHiraganaKatakanaVoicingMarks = 8,
		ccViramas = 9,
		ccFixedPositionStart = 10, // Start of fixed position classes
		ccFixedPositionEnd = 199, // End of fixed position classes
		ccBelowLeftAttached = 200,
		ccBelowAttached = 202,
		ccBelowRightAttached = 204,
		ccLeftAttached = 208, // Left attached (reordrant around single base character)
		ccRightAttached = 210,
		ccAboveLeftAttached = 212,
		ccAboveAttached = 214,
		ccAboveRightAttached = 216,
		ccBelowLeft = 218,
		ccBelow = 220,
		ccBelowRight = 222,
		ccLeft = 224, // Left (reordrant around single base character)
		ccRight = 226,
		ccAboveLeft = 228,
		ccAbove = 230,
		ccAboveRight = 232,
		ccDoubleBelow = 233,
		ccDoubleAbove = 234,
		ccBelowIotaSubscript = 240, // Below (iota subscript)
		ccInvalid = 255 // not defined by Unicode
	};
	char chCat, chSubCat; // chCat + chSubCat together comprise the general category (from UnicodeData.txt)
	uchar combClass; // canonical combining class
	TUniChCategory cat; // = TUniChCategory(chCat)
	TUniChSubCategory subCat; // = TUniChSubCategory(cat << 8 | subCat)
	signed char script; // keyId into 'TUniChDb.scriptNames'; -1 if unknown
	int simpleUpperCaseMapping, simpleLowerCaseMapping, simpleTitleCaseMapping; // from UnicodeData.txt
	int decompOffset; // offset into 'TUniChDb.decompositions'; or -1 if the character doesn't change during decomposition
	int nameOffset; // offset into 'TUniChDb.charNames'
	int flags; // a combination of TUniChFlags
	int properties; // a combination of TUniChProperties
	int propertiesX; // a combination of TUniChPropertiesX
	ushort lineBreak; // from LineBreak.txt

	// Converts a 2-letter linebreak code into a 16-bit integer.
	static inline ushort GetLineBreakCode(char c1, char c2) { return ((ushort(uchar(c1)) & 0xff) << 8) | ((ushort(uchar(c2)) & 0xff)); }
	static const ushort LineBreak_Unknown, LineBreak_ComplexContext, LineBreak_Numeric, LineBreak_InfixNumeric, LineBreak_Quotation;

public:
	void InitAfterLoad() {
		cat = (TUniChCategory) chCat;
		subCat = (TUniChSubCategory) (((int(uchar(chCat)) & 0xff) << 8) | (int(uchar(chSubCat)) & 0xff)); }
	void SetCatAndSubCat(const TUniChSubCategory catAndSubCat) {
		cat = (TUniChCategory) ((int(catAndSubCat) >> 8) & 0xff);
		subCat = catAndSubCat;
		chCat = (char) cat; chSubCat = (char) (int(subCat) & 0xff); }
	friend class TUniChDb;

	// Inexplicably missing from TSIn/TSOut...
	static inline void LoadUShort(TSIn& SIn, ushort& u) { SIn.LoadBf(&u, sizeof(u)); }
	static inline void LoadSChar(TSIn& SIn, signed char& u) { SIn.LoadBf(&u, sizeof(u)); }
	static inline void SaveUShort(TSOut& SOut, ushort u) { SOut.SaveBf(&u, sizeof(u)); }
	static inline void SaveSChar(TSOut& SOut, signed char u) { SOut.SaveBf(&u, sizeof(u)); }

public:
	void Save(TSOut& SOut) const {
		SOut.Save(chCat); SOut.Save(chSubCat); SOut.Save(combClass); SaveSChar(SOut, script);
		SOut.Save(simpleUpperCaseMapping); SOut.Save(simpleLowerCaseMapping); SOut.Save(simpleTitleCaseMapping);
		SOut.Save(decompOffset); SOut.Save(nameOffset);
		SOut.Save(flags); SOut.Save(properties); SOut.Save(propertiesX); SaveUShort(SOut, lineBreak); }
	void Load(TSIn& SIn) {
		SIn.Load(chCat); SIn.Load(chSubCat); SIn.Load(combClass); LoadSChar(SIn, script);
		SIn.Load(simpleUpperCaseMapping); SIn.Load(simpleLowerCaseMapping); SIn.Load(simpleTitleCaseMapping);
		SIn.Load(decompOffset); SIn.Load(nameOffset);
		SIn.Load(flags); SIn.Load(properties); SIn.Load(propertiesX); LoadUShort(SIn, lineBreak); InitAfterLoad(); }
	explicit TUniChInfo(TSIn& SIn) { Load(SIn); }
	TUniChInfo() : chCat(char(ucOther)), chSubCat(char(ucOtherNotAssigned & 0xff)), combClass(ccInvalid),
		script(-1),simpleUpperCaseMapping(-1), simpleLowerCaseMapping(-1), simpleTitleCaseMapping(-1),
		decompOffset(-1), nameOffset(-1), flags(0), properties(0), propertiesX(0), lineBreak(LineBreak_Unknown) {
		InitAfterLoad(); }

	// DerivedCoreProperties flags.
	bool IsDcpFlag(const TUniChFlags flag) const { Assert((flag & ucfDcpMask) == flag); return (flags & flag) == flag; }
	void ClrDcpFlags() { flags = flags & ~ucfDcpMask; }
	void SetDcpFlag(const TUniChFlags flag) { Assert((flag & ucfDcpMask) == flag); flags |= flag; }
	bool IsAlphabetic() const { return IsDcpFlag(ucfDcpAlphabetic); }
	bool IsUppercase() const { return IsDcpFlag(ucfDcpUppercase); }
	bool IsLowercase() const { return IsDcpFlag(ucfDcpLowercase); }
	bool IsMath() const { return IsDcpFlag(ucfDcpMath); }
	bool IsDefaultIgnorable() const { return IsDcpFlag(ucfDcpDefaultIgnorableCodePoint); }
	bool IsGraphemeBase() const { return IsDcpFlag(ucfDcpGraphemeBase); }
	bool IsGraphemeExtend() const { return IsDcpFlag(ucfDcpGraphemeExtend); }
	bool IsIdStart() const { return IsDcpFlag(ucfDcpIdStart); }
	bool IsIdContinue() const { return IsDcpFlag(ucfDcpIdContinue); }
	bool IsXidStart() const { return IsDcpFlag(ucfDcpXidStart); }
	bool IsXidContinue() const { return IsDcpFlag(ucfDcpXidContinue); }

	// PropList.txt flags.
	bool IsProperty(const TUniChProperties flag) const { return (properties & flag) == flag; }
	void SetProperty(const TUniChProperties flag) { properties |= flag; }
	bool IsAsciiHexDigit() const { return IsProperty(ucfPrAsciiHexDigit); }
	bool IsBidiControl() const { return IsProperty(ucfPrBidiControl); }
	bool IsDash() const { return IsProperty(ucfPrDash); }
	bool IsDeprecated() const { return IsProperty(ucfPrDeprecated); }
	bool IsDiacritic() const { return IsProperty(ucfPrDiacritic); }
	bool IsExtender() const { return IsProperty(ucfPrExtender); }
	bool IsGraphemeLink() const { return IsProperty(ucfPrGraphemeLink); }
	bool IsHexDigit() const { return IsProperty(ucfPrHexDigit); }
	bool IsHyphen() const { return IsProperty(ucfPrHyphen); }
	bool IsIdeographic() const { return IsProperty(ucfPrIdeographic); }
	bool IsJoinControl() const { return IsProperty(ucfPrJoinControl); }
	bool IsLogicalOrderException() const { return IsProperty(ucfPrLogicalOrderException); }
	bool IsNoncharacter() const { return IsProperty(ucfPrNoncharacterCodePoint); }
	bool IsQuotationMark() const { return IsProperty(ucfPrQuotationMark); }
	bool IsSoftDotted() const { return IsProperty(ucfPrSoftDotted); }
	bool IsSTerminal() const { return IsProperty(ucfPrSTerm); }
	bool IsTerminalPunctuation() const { return IsProperty(ucfPrTerminalPunctuation); }
	bool IsVariationSelector() const { return IsProperty(ucfPrVariationSelector); }
	bool IsWhiteSpace() const { return IsProperty(ucfPrWhiteSpace); }

	// Additional PropList.txt flags.
	bool IsPropertyX(const TUniChPropertiesX flag) const { return (propertiesX & flag) == flag; }
	void SetPropertyX(const TUniChPropertiesX flag) { propertiesX |= flag; }

	// Miscellaneous flags.
	bool IsCompositionExclusion() const { return (flags & ucfCompositionExclusion) == ucfCompositionExclusion; }
	bool IsCompatibilityDecomposition() const { return (flags & ucfCompatibilityDecomposition) == ucfCompatibilityDecomposition; }

	// Word-boundary flags.
	bool IsWbFlag(const TUniChFlags flag) const { Assert((flag & ucfWbMask) == flag); return (flags & flag) == flag; }
	void ClrWbAndSbFlags() { flags = flags & ~(ucfWbMask | ucfSbMask); }
	void SetWbFlag(const TUniChFlags flag) { Assert((flag & ucfWbMask) == flag); flags |= flag; }
	int GetWbFlags() const { return flags & ucfWbMask; }
	bool IsWbFormat() const { return IsWbFlag(ucfWbFormat); }
	TStr GetWbFlagsStr() const { return GetWbFlagsStr(GetWbFlags()); }
	static TStr GetWbFlagsStr(const int flags) { return TStr("") + (flags & ucfWbALetter ? "A" : "") +
		(flags & ucfWbFormat ? "F" : "") + (flags & ucfWbKatakana ? "K" : "") + (flags & ucfWbMidLetter ? "M" : "") +
		(flags & ucfWbMidNum ? "m" : "") + (flags & ucfWbNumeric ? "N" : "") + (flags & ucfWbExtendNumLet ? "E" : ""); }

	// Sentence-boundary flags.
	bool IsSbFlag(const TUniChFlags flag) const { Assert((flag & ucfSbMask) == flag); return (flags & flag) == flag; }
	void SetSbFlag(const TUniChFlags flag) { Assert((flag & ucfSbMask) == flag); flags |= flag; }
	int GetSbFlags() const { return flags & ucfSbMask; }
	bool IsSbFormat() const { return IsSbFlag(ucfSbFormat); }
	TStr GetSbFlagsStr() const { return GetSbFlagsStr(GetSbFlags()); }
	static TStr GetSbFlagsStr(const int flags) { return TStr("") + (flags & ucfSbSep ? "S" : "") +
		(flags & ucfSbFormat ? "F" : "") + (flags & ucfSbSp ? "_" : "") + (flags & ucfSbLower ? "L" : "") +
		(flags & ucfSbUpper ? "U" : "") + (flags & ucfSbOLetter ? "O" : "") + (flags & ucfSbNumeric ? "N" : "") +
		(flags & ucfSbATerm ? "A" : "") + (flags & ucfSbSTerm ? "T" : "") + (flags & ucfSbClose ? "C" : ""); }

	bool IsSbSep() const { return (flags & ucfSbSep) == ucfSbSep; }

	// Grapheme-boundary flags.
	bool IsGbExtend() const { return IsGraphemeExtend(); }

	// Sec. 3.13, D47: C is cased iff it is uppercase, lowercase, or general_category == titlecase_letter.
	bool IsCased() const { return IsUppercase() || IsLowercase() || (subCat == ucLetterTitlecase); }

	// Character categories.
	TUniChCategory GetCat() const { return (TUniChCategory) cat; }
	TUniChSubCategory GetSubCat() const { return (TUniChSubCategory) subCat; }
	// The following characters belong to the 'symbol/currency' subcategory:
	//     U+00024  DOLLAR SIGN
	//     U+000a2  CENT SIGN
	//     U+000a3  POUND SIGN
	//     U+000a4  CURRENCY SIGN
	//     U+000a5  YEN SIGN
	//     U+020a3  FRENCH FRANC SIGN
	//     U+020a4  LIRA SIGN
	//     U+020ac  EURO SIGN
	//     [and plenty of others]
	bool IsCurrency() const { return subCat == ucSymbolCurrency; }
	// Note: most private-use and surrogate characters aren't listed explicitly in UnicodeData.txt.
	// Thus, it's better to call TUniChDb's versions of these methods, which are aware of
	// the full ranges of private-use and surrogate characters.
	bool IsPrivateUse() const { return subCat == ucOtherPrivateUse; }
	bool IsSurrogate() const { return subCat == ucOtherSurrogate; }

	inline static bool IsValidSubCat(const char chCat, const char chSubCat) {
		static const char s[] = "LuLlLtLmLoMnMcMeNdNlNoPcPdPsPePiPfPoSmScSkSoZsZlZpCcCfCsCoCn";
		for (const char *p = s; *p; p += 2)
			if (chCat == p[0] && chSubCat == p[1]) return true;
		return false; }
};

//-----------------------------------------------------------------------------
// TUniTrie -- a trie for suffixes that should not appear at the end
// of a sentence
//-----------------------------------------------------------------------------

template<typename TItem_>
class TUniTrie
{
public:
	typedef TItem_ TItem;
protected:
	class TNode {
	public:
		TItem item;
		int child, sib;
		bool terminal;
		TNode() : child(-1), sib(-1), terminal(false) { }
		TNode(const TItem& item_, const int child_, const int sib_, const bool terminal_) : item(item_), child(child_), sib(sib_), terminal(terminal_) { }
	};
	typedef TVec<TNode> TNodeV;
	typedef TPair<TItem, TItem> TItemPr;
	typedef TTriple<TItem, TItem, TItem> TItemTr;
	typedef TUniVecIdx TVecIdx;
	THash<TItem, TVoid> singles; //
	THash<TItemPr, TVoid> pairs;
	THash<TItemTr, TInt> roots;
	TNodeV nodes;
public:
	TUniTrie() { }
	void Clr() { singles.Clr(); pairs.Clr(); roots.Clr(); nodes.Clr(); }

	bool Empty() const { return singles.Empty() && pairs.Empty() && roots.Empty(); }

	bool Has1Gram(const TItem& item) const { return singles.IsKey(item); }
	bool Has2Gram(const TItem& last, const TItem& butLast) const { return pairs.IsKey(TItemPr(last, butLast)); }
	int Get3GramRoot(const TItem& last, const TItem& butLast, const TItem& butButLast) const {
		int keyId = roots.GetKeyId(TItemTr(last, butLast, butButLast));
		if (keyId < 0) return 0; else return roots[keyId]; }
	int GetChild(const int parentIdx, const TItem& item) const {
		for (int childIdx = nodes[parentIdx].child; childIdx >= 0; ) {
			const TNode &node = nodes[childIdx];
			if (node.item == item) return childIdx;
			childIdx = node.sib; }
		return -1; }
	bool IsNodeTerminal(const int nodeIdx) const { return nodes[nodeIdx].terminal; }

	// Adds a new string to the trie.  Note that the last characters appear
	// closer to the root of the trie.
	template<typename TSrcVec>
	void Add(const TSrcVec& src, const size_t srcIdx, const size_t srcCount)
	{
		IAssert(srcCount > 0);
		if (srcCount == 1) { singles.AddKey(TItem(src[TVecIdx(srcIdx)])); return; }
		if (srcCount == 2) { pairs.AddKey(TItemPr(TItem(src[TVecIdx(srcIdx + 1)]), TItem(src[TVecIdx(srcIdx)]))); return; }
		size_t srcLast = srcIdx + (srcCount - 1);
		TItemTr tr = TItemTr(TItem(src[TVecIdx(srcLast)]), TItem(src[TVecIdx(srcLast - 1)]), TItem(src[TVecIdx(srcLast - 2)]));
		int keyId = roots.GetKeyId(tr), curNodeIdx = -1;
		if (keyId >= 0) curNodeIdx = roots[keyId];
		else { curNodeIdx = nodes.Add(TNode(TItem(0), -1, -1, false)); roots.AddDat(tr, curNodeIdx); }
		//
		if (srcCount > 3) for (size_t srcPos = srcLast - 3; ; )
		{
			const TItem curItem = src[TVecIdx(srcPos)];
			int childNodeIdx = nodes[curNodeIdx].child;
			while (childNodeIdx >= 0) {
				TNode &childNode = nodes[childNodeIdx];
				if (childNode.item == curItem) break;
				childNodeIdx = childNode.sib; }
			if (childNodeIdx < 0) {
				childNodeIdx = nodes.Add(TNode(curItem, -1, nodes[curNodeIdx].child, false));
				nodes[curNodeIdx].child = childNodeIdx; }
			curNodeIdx = childNodeIdx;
			if (srcPos == srcIdx) break; else srcPos--;
		}
		nodes[curNodeIdx].terminal = true;
	}

	template<typename TSrcVec>
	void Add(const TSrcVec& src) { Add(src, 0, (size_t) src.Len()); }
};

//-----------------------------------------------------------------------------
// TUniChDb -- provides access to the Unicode Character Database
//-----------------------------------------------------------------------------

class TUniChDb
{
protected:
	void InitAfterLoad();
	typedef TUniVecIdx TVecIdx;

public:
	THash<TInt, TUniChInfo> h; // key: codepoint
	TStrPool charNames;
	TStrIntH scripts; // keyID = used in TUniChInfo.script; key = script name; dat = number of characters (informative only)
	TIntV decompositions;
	THash<TIntPr, TInt> inverseDec;
	TUniCaseFolding caseFolding;
	// These hash tables contain only the unconditional mappings from SpecialCasing.txt.
	// The conditional mappings are hardcoded into GetCaseConverted().
	TIntIntVH specialCasingLower, specialCasingUpper, specialCasingTitle;
	int scriptUnknown; // = scripts.GetKey("Unknown")

	TUniChDb() : scriptUnknown(-1) { }
	explicit TUniChDb(TSIn& SIn) { Load(SIn); }
	void Clr() {
		h.Clr(); charNames.Clr(); decompositions.Clr(); inverseDec.Clr(); caseFolding.Clr();
		specialCasingLower.Clr(); specialCasingUpper.Clr(); specialCasingTitle.Clr();
		scripts.Clr(); }
	void Save(TSOut& SOut) const {
		h.Save(SOut); charNames.Save(SOut); decompositions.Save(SOut);
		inverseDec.Save(SOut); caseFolding.Save(SOut); scripts.Save(SOut);
		specialCasingLower.Save(SOut); specialCasingUpper.Save(SOut); specialCasingTitle.Save(SOut);
		SOut.SaveCs(); }
	void Load(TSIn& SIn) {
		h.Load(SIn); charNames.~TStrPool(); new (&charNames) TStrPool(SIn);
		decompositions.Load(SIn);
		inverseDec.Load(SIn); caseFolding.Load(SIn); scripts.Load(SIn);
		specialCasingLower.Load(SIn); specialCasingUpper.Load(SIn); specialCasingTitle.Load(SIn);
		SIn.LoadCs(); InitAfterLoad(); }
	void LoadBin(const TStr& fnBin) {
		PSIn SIn = TFIn::New(fnBin); Load(*SIn); }
	void Test(const TStr& basePath);

	// File names used by LoadTxt() and its subroutines.
	static TStr GetCaseFoldingFn() { return "CaseFolding.txt"; }
	static TStr GetSpecialCasingFn() { return "SpecialCasing.txt"; }
	static TStr GetUnicodeDataFn() { return "UnicodeData.txt"; }
	static TStr GetCompositionExclusionsFn() { return "CompositionExclusions.txt"; }
	static TStr GetScriptsFn() { return "Scripts.txt"; }
	static TStr GetDerivedCorePropsFn() { return "DerivedCoreProperties.txt"; }
	static TStr GetLineBreakFn() { return "LineBreak.txt"; }
	static TStr GetPropListFn() { return "PropList.txt"; }
	static TStr GetAuxiliaryDir() { return "auxiliary"; }
	static TStr GetWordBreakTestFn() { return "WordBreakTest.txt"; }
	static TStr GetWordBreakPropertyFn() { return "WordBreakProperty.txt"; }
	static TStr GetSentenceBreakTestFn() { return "SentenceBreakTest.txt"; }
	static TStr GetSentenceBreakPropertyFn() { return "SentenceBreakProperty.txt"; }
	static TStr GetNormalizationTestFn() { return "NormalizationTest.txt"; }
	static TStr GetBinFn() { return "UniChDb.bin"; } // used only by Test()

	//-------------------------------------------------------------------------
	// Script names
	//-------------------------------------------------------------------------

	// These constants are used when initializing from the text files.
	static TStr GetScriptNameUnknown() { return "Unknown"; }
	static TStr GetScriptNameKatakana() { return "Katakana"; }
	static TStr GetScriptNameHiragana() { return "Hiragana"; }
	//
	const TStr& GetScriptName(const int scriptId) const { return scripts.GetKey(scriptId); }
	int GetScriptByName(const TStr& scriptName) const { return scripts.GetKeyId(scriptName); }
	int GetScript(const TUniChInfo& ci) const { int s = ci.script; if (s < 0) s = scriptUnknown; return s; }
	int GetScript(const int cp) const { int i = h.GetKeyId(cp); if (i < 0) return scriptUnknown; else return GetScript(h[i]); }

	//-------------------------------------------------------------------------
	// Character namesnames
	//-------------------------------------------------------------------------

	// GetCharName returns 0 if the name is unknown; GetCharNameS returns a string of the form "U+1234".
	const char *GetCharName(const int cp) const { int i = h.GetKeyId(cp); if (i < 0) return 0; int ofs = h[i].nameOffset; return ofs < 0 ? 0 : charNames.GetCStr(ofs); }
	TStr GetCharNameS(const int cp) const {
		// ToDo: Add special processing for precomposed Hangul syllables (UAX #15, sec. 16).
		const char *p = GetCharName(cp); if (p) return p;
		char buf[20]; sprintf(buf, "U+%04x", cp); return TStr(buf); }
	template<class TSrcVec> void PrintCharNames(FILE *f, const TSrcVec& src, size_t srcIdx, const size_t srcCount, const TStr& prefix) const {
		if (! f) f = stdout;
		for (const size_t srcEnd = srcIdx + srcCount; srcIdx < srcEnd; srcIdx++) {
			fprintf(f, "%s", prefix.CStr());
			int cp = src[TVecIdx(srcIdx)]; fprintf(f, (cp >= 0x10000 ? "U+%05x" : "U+%04x "), cp);
			fprintf(f, " %s\n", GetCharNameS(cp).CStr()); }}
	template<class TSrcVec> void PrintCharNames(FILE *f, const TSrcVec& src, const TStr& prefix) const { PrintCharNames(f, src, 0, src.Len(), prefix); }

	//-------------------------------------------------------------------------
	// Character information
	//-------------------------------------------------------------------------
	// These methods provide access to a subset of the functionality
	// available in TUniChInfo.

	bool IsGetChInfo(const int cp, TUniChInfo& ChInfo) {
		int i = h.GetKeyId(cp);
		if (i < 0) return false; else { ChInfo=h[i]; return true; }}
	TUniChCategory GetCat(const int cp) const { int i = h.GetKeyId(cp); if (i < 0) return ucOther; else return h[i].cat; }
	TUniChSubCategory GetSubCat(const int cp) const { int i = h.GetKeyId(cp); if (i < 0) return ucOtherNotAssigned; else return h[i].subCat; }

	bool IsWbFlag(const int cp, const TUniChFlags flag) const { int i = h.GetKeyId(cp); if (i < 0) return false; else return h[i].IsWbFlag(flag); }
	int GetWbFlags(const int cp) const { int i = h.GetKeyId(cp); if (i < 0) return 0; else return h[i].GetWbFlags(); }
	bool IsSbFlag(const int cp, const TUniChFlags flag) const { int i = h.GetKeyId(cp); if (i < 0) return false; else return h[i].IsSbFlag(flag); }
	int GetSbFlags(const int cp) const { int i = h.GetKeyId(cp); if (i < 0) return 0; else return h[i].GetSbFlags(); }

#define ___UniFwd1(name) bool name(const int cp) const { int i = h.GetKeyId(cp); if (i < 0) return false; else return h[i].name(); }
#define ___UniFwd2(name1, name2) ___UniFwd1(name1) ___UniFwd1(name2)
#define ___UniFwd3(name1, name2, name3) ___UniFwd2(name1, name2) ___UniFwd1(name3)
#define ___UniFwd4(name1, name2, name3, name4) ___UniFwd3(name1, name2, name3) ___UniFwd1(name4)
#define ___UniFwd5(name1, name2, name3, name4, name5) ___UniFwd4(name1, name2, name3, name4) ___UniFwd1(name5)

#define DECLARE_FORWARDED_PROPERTY_METHODS \
	___UniFwd5(IsAsciiHexDigit, IsBidiControl, IsDash, IsDeprecated, IsDiacritic) \
	___UniFwd5(IsExtender, IsGraphemeLink, IsHexDigit, IsHyphen, IsIdeographic)  \
	___UniFwd5(IsJoinControl, IsLogicalOrderException, IsNoncharacter, IsQuotationMark, IsSoftDotted)  \
	___UniFwd4(IsSTerminal, IsTerminalPunctuation, IsVariationSelector, IsWhiteSpace)  \
	___UniFwd5(IsAlphabetic, IsUppercase, IsLowercase, IsMath, IsDefaultIgnorable)  \
	___UniFwd4(IsGraphemeBase, IsGraphemeExtend, IsIdStart, IsIdContinue)  \
	___UniFwd2(IsXidStart, IsXidContinue)  \
	___UniFwd3(IsCompositionExclusion, IsCompatibilityDecomposition, IsSbSep)  \
	___UniFwd1(IsGbExtend)  \
	___UniFwd2(IsCased, IsCurrency)

	DECLARE_FORWARDED_PROPERTY_METHODS

#undef ___UniFwd1

	bool IsPrivateUse(const int cp) const {
		int i = h.GetKeyId(cp); if (i >= 0) return h[i].IsPrivateUse();
		return (0xe000 <= cp && cp <= 0xf8ff) ||  // plane 0 private-use area
			// Planes 15 and 16 are entirely for private use.
			(0xf0000 <= cp && cp <= 0xffffd) || (0x100000 <= cp && cp <= 0x10fffd); }
	// Note: d800..dbff are high surrogates, dc00..dfff are low surrogates.
	// For db80..dbff it is clear that the surrogate pair containing this high surrogate
	// will refer to a private-use codepoint, but IsPrivateUse nevertheless returns false
	// for db80..dbff.  This is consistent with the category codes assigned in UnicodeData.txt.
	bool IsSurrogate(const int cp) const {
		int i = h.GetKeyId(cp); if (i >= 0) return h[i].IsSurrogate();
		return 0xd800 <= cp && cp <= 0xdcff; }

	// Note: in particular, all Hangul characters (HangulLBase..HangulLBase + HangulLCount - 1
	// and HangulSBase..HangulSBase + HangulSCount - 1) should be treated as starters
	// for composition to work correctly.
	int GetCombiningClass(const int cp) const { int i = h.GetKeyId(cp); if (i < 0) return TUniChInfo::ccStarter; else return h[i].combClass; }

	//-------------------------------------------------------------------------
	// Hangul constants
	//-------------------------------------------------------------------------

	enum {
        HangulSBase = 0xAC00, HangulLBase = 0x1100, HangulVBase = 0x1161, HangulTBase = 0x11A7,
        HangulLCount = 19, HangulVCount = 21, HangulTCount = 28,
        HangulNCount = HangulVCount * HangulTCount,   // 588
        HangulSCount = HangulLCount * HangulNCount   // 11172
	};

	//-------------------------------------------------------------------------
	// Word boundaries (UAX #29)
	//-------------------------------------------------------------------------

protected:
	// UAX #29, rule WB3: ignore Format and Extend characters.
	// [Note: rule SB5 for sentence boundaries is identical, and thus these methods will also be used for sentence-boundary detection.]
	static bool IsWbIgnored(const TUniChInfo& ci) { return ci.IsGbExtend() || ci.IsWbFormat(); }
	bool IsWbIgnored(const int cp) const { int i = h.GetKeyId(cp); if (i < 0) return false; else return IsWbIgnored(h[i]); }
	// Sets 'position' to the smallest index from 'position..srcEnd-1' that contains a non-ignored character.
	template<typename TSrcVec> void WbFindCurOrNextNonIgnored(const TSrcVec& src, size_t& position, const size_t srcEnd) const {
		while (position < srcEnd && IsWbIgnored(src[TVecIdx(position)])) position++; }
	// Sets 'position' to the smallest index from 'position+1..srcEnd-1' that contains a non-ignored character.
	template<typename TSrcVec> void WbFindNextNonIgnored(const TSrcVec& src, size_t& position, const size_t srcEnd) const {
		if (position >= srcEnd) return;
		position++; while (position < srcEnd && IsWbIgnored(src[TVecIdx(position)])) position++; }
	// Sets 'position' to the smallest index from 'position+1..srcEnd-1' that contains a non-ignored character.
	template<typename TSrcVec> void WbFindNextNonIgnoredS(const TSrcVec& src, size_t& position, const size_t srcEnd) const {
		if (position >= srcEnd) return;
		if (IsSbSep(src[TVecIdx(position)])) { position++; return; }
		position++; while (position < srcEnd && IsWbIgnored(src[TVecIdx(position)])) position++; }
	// Sets 'position' to the largest index from 'srcStart..position-1' that contains a non-ignored character.
	template<typename TSrcVec> bool WbFindPrevNonIgnored(const TSrcVec& src, const size_t srcStart, size_t& position) const {
		if (position <= srcStart) return false;
		while (position > srcStart) {
			position--; if (! IsWbIgnored(src[TVecIdx(position)])) return true; }
		return false; }
	// Test driver for WbFind*NonIgnored.
	void TestWbFindNonIgnored(const TIntV& src) const;
	void TestWbFindNonIgnored() const;
public:
	// Finds the next word boundary strictly after 'position'.
	// Note that there is a valid word boundary at 'srcIdx + srcCount'.
	// If there is no such word boundary, it returns 'false' and sets 'position' to 'srcIdx + srcCount'.
	template<typename TSrcVec>
	bool FindNextWordBoundary(const TSrcVec& src, const size_t srcIdx, const size_t srcCount, size_t &position) const;
	// Creates, in 'dest', a vector of 'srcCount + 1' elements, where 'dest[i]' tells if there is a word
	// boundary between 'src[srcIdx + i - 1]' and 'src[srcIdx + i]'.  Note that 'dest[0]' and 'dest[srcCount]' are
	// always set to 'true'.
	template<typename TSrcVec>
	void FindWordBoundaries(const TSrcVec& src, const size_t srcIdx, const size_t srcCount, TBoolV& dest) const;
protected:
	void TestFindNextWordOrSentenceBoundary(const TStr& basePath, bool sentence);

	//-------------------------------------------------------------------------
	// Sentence boundaries (UAX #29)
	//-------------------------------------------------------------------------

protected:
	TUniTrie<TInt> sbExTrie;

	// Checks whether a sentence that ended at src[position - 1]
	// would end in one of the suffixes from sbExTrie.
	template<typename TSrcVec>
	bool CanSentenceEndHere(const TSrcVec& src, const size_t srcIdx, const size_t position) const;

public:
	// Finds the next sentence boundary strictly after 'position'.
	// Note that there is a valid sentence boundary at 'srcIdx + srcCount'.
	// If there is no such sentence boundary, it returns 'false' and sets 'position' to 'srcIdx + srcCount'.
	template<typename TSrcVec>
	bool FindNextSentenceBoundary(const TSrcVec& src, const size_t srcIdx, const size_t srcCount, size_t &position) const;
	// Creates, in 'dest', a vector of 'srcCount + 1' elements, where 'dest[i]' tells if there is a sentence
	// boundary between 'src[srcIdx + i - 1]' and 'src[srcIdx + i]'.  Note that 'dest[0]' and 'dest[srcCount]' are
	// always set to 'true'.
	template<typename TSrcVec>
	void FindSentenceBoundaries(const TSrcVec& src, const size_t srcIdx, const size_t srcCount, TBoolV& dest) const;

	// These methods allow the user to define a set of sentence boundary exceptions.
	// This is a set of strings, stored in 'sbExTrie'.  If the Unicode rules require
	// a sentence boundary in a position that would cause the sentence to end with
	// 'x (STerm | ATerm) Close* Sp* Sep?', where 'x' is a word from 'sbExTrie',
	// we will *not* place a sentence boundary there.
	//
	// NOTE: sbExTrie is not saved or loaded by the Save() and Load() methods.
	// By default, it is empty.  Use SbEx_Clr() to clear it, and SbEx_SetStdEnglish() to obtain
	// a standard set of English-language exceptions.
	void SbEx_Clr() { sbExTrie.Clr(); }
	template<class TSrcVec> void SbEx_Add(const TSrcVec& v) { sbExTrie.Add(v); }
	// template<> void SbEx_Add(const TStr& s) {
	void SbEx_Add(const TStr& s) {
          TIntV v; int n = s.Len(); v.Gen(n); for (int i = 0; i < n; i++) v[i] = int(uchar(s[i])); SbEx_Add(v); }
	void SbEx_AddUtf8(const TStr& s) { TUniCodec codec; TIntV v; codec.DecodeUtf8(s, v); SbEx_Add(v); }
	int SbEx_AddMulti(const TStr& words, const bool wordsAreUtf8 = true) { TStrV vec; words.SplitOnAllCh('|', vec);
		for (int i = 0; i < vec.Len(); i++) if (wordsAreUtf8) SbEx_AddUtf8(vec[i]); else SbEx_Add(vec[i]);
		return vec.Len(); }
	void SbEx_Set(const TUniTrie<TInt>& newTrie) { sbExTrie = newTrie; }
	int SbEx_SetStdEnglish() {
		static const TStr data = "Ms|Mrs|Mr|Rev|Dr|Prof|Gov|Sen|Rep|Gen|Brig|Col|Capt|Lieut|Lt|Sgt|Pvt|Cmdr|Adm|Corp|St|Mt|Ft|e.g|e. g.|i.e|i. e|ib|ibid|s.v|s. v|s.vv|s. vv";
		SbEx_Clr(); return SbEx_AddMulti(data, false); }

	//-------------------------------------------------------------------------
	// Normalization, decomposition, etc. (UAX #15)
	//-------------------------------------------------------------------------

protected:
	// Adds, to 'dest', the decomposition of 'codePoint' (calling itself recursively if necessary).
	// If 'compatibility == false', only canonical decompositions are used.
	template<typename TDestCh>
	void AddDecomposition(const int codePoint, TVec<TDestCh>& dest, const bool compatibility) const;
public:
	// This appends, to 'dest', the decomposed form of the source string.
	// - for normalization form D (NFD), i.e. canonical decomposition: use compatibility == false;
	// - for normalization form KD (NFKD), i.e. compatibility decomposition: use compatibility == true.
	template<typename TSrcVec, typename TDestCh>
	void Decompose(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
			TVec<TDestCh>& dest, bool compatibility, bool clrDest = true) const;
	template<typename TSrcVec, typename TDestCh>
	void Decompose(const TSrcVec& src, TVec<TDestCh>& dest, bool compatibility, bool clrDest = true) const {
		Decompose(src, 0, src.Len(), dest, compatibility, clrDest); }
	// This performs canonical composition on the source string, and appends
	// the result to the destination string.  The source string should be the
	// result of a (canonical or compatibility) decomposition; if this is the
	// case, the composition will lead to a normalization form C (NFC) or
	// normalization form KC (NFKC), depending on whether canonical or compatibility
	// decomposition was used.
	template<typename TSrcVec, typename TDestCh>
	void Compose(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
			TVec<TDestCh>& dest, bool clrDest = true) const;
	template<typename TSrcVec, typename TDestCh>
	void Compose(const TSrcVec& src, TVec<TDestCh>& dest, bool clrDest = true) const {
		Compose(src, 0, src.Len(), dest, clrDest); }
	// Calls Decompose, followed by Compose; thus the result is the NFC (if
	// compatibility == false) or NFKC (if compatibility == true) of the source string.
	// A temporary TIntV is used to contain the intermediate NF(K)D form of the
	// source string.
	template<typename TSrcVec, typename TDestCh>
	void DecomposeAndCompose(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
			TVec<TDestCh>& dest, bool compatibility, bool clrDest = true) const;
	template<typename TSrcVec, typename TDestCh>
	void DecomposeAndCompose(const TSrcVec& src, TVec<TDestCh>& dest, bool compatibility, bool clrDest = true) const {
		DecomposeAndCompose(src, 0, src.Len(), dest, compatibility, clrDest); }
	// Copies the starter characters from 'src' to 'dest'; the other
	// characters are skipped.  'src' should already have been decomposed.
	// Returns the number of characters extracted.
	template<typename TSrcVec, typename TDestCh>
	size_t ExtractStarters(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
			TVec<TDestCh>& dest, bool clrDest = true) const;
	template<typename TSrcVec, typename TDestCh>
	size_t ExtractStarters(const TSrcVec& src, TVec<TDestCh>& dest, bool clrDest = true) const {
		return ExtractStarters(src, 0, src.Len(), dest, clrDest); }
	// Extracts the starters into a temporary vector and then copies it into 'src'.
	template<typename TSrcVec>
	size_t ExtractStarters(TSrcVec& src) const {
		TIntV temp; size_t retVal = ExtractStarters(src, temp);
		src.Clr(); for (int i = 0; i < temp.Len(); i++) src.Add(temp[i]);
		return retVal; }

protected:
	void TestComposition(const TStr& basePath);

	//-------------------------------------------------------------------------
	// Initialization from the text files
	//-------------------------------------------------------------------------

protected:
	void InitWordAndSentenceBoundaryFlags(const TStr& basePath);
	void InitScripts(const TStr& basePath);
	void InitLineBreaks(const TStr& basePath);
	void InitDerivedCoreProperties(const TStr& basePath);
	void InitPropList(const TStr& basePath);
	void InitSpecialCasing(const TStr& basePath);
	void LoadTxt_ProcessDecomposition(TUniChInfo& ci, TStr s);
public:
	void LoadTxt(const TStr& basePath);
	void SaveBin(const TStr& fnBinUcd);

	//-------------------------------------------------------------------------
	// Case conversions
	//-------------------------------------------------------------------------

public:
	typedef enum TCaseConversion_ { ccLower = 0, ccUpper = 1, ccTitle = 2, ccMax = 3 } TCaseConversion;
	// Appends the case-converted form of 'src' to 'dest'.
	// 'how' defines what kind of case conversion is required.
	// 'turkic' should be set to true iff the text is in Turkic ('tr') or Azeri ('ar').
	// 'lithuanian' should be set to true iff the text is in Lithuanian ('lt').
	template<typename TSrcVec, typename TDestCh> void GetCaseConverted(const TSrcVec& src, size_t srcIdx, const size_t srcCount, TVec<TDestCh>& dest, const bool clrDest, const TCaseConversion how, const bool turkic, const bool lithuanian) const;
	template<typename TSrcVec, typename TDestCh> void GetLowerCase(const TSrcVec& src, size_t srcIdx, const size_t srcCount, TVec<TDestCh>& dest, const bool clrDest = true, const bool turkic = false, const bool lithuanian = false) const { GetCaseConverted(src, srcIdx, srcCount, dest, clrDest, ccLower, turkic, lithuanian); }
	template<typename TSrcVec, typename TDestCh> void GetUpperCase(const TSrcVec& src, size_t srcIdx, const size_t srcCount, TVec<TDestCh>& dest, const bool clrDest = true, const bool turkic = false, const bool lithuanian = false) const { GetCaseConverted(src, srcIdx, srcCount, dest, clrDest, ccUpper, turkic, lithuanian); }
	template<typename TSrcVec, typename TDestCh> void GetTitleCase(const TSrcVec& src, size_t srcIdx, const size_t srcCount, TVec<TDestCh>& dest, const bool clrDest = true, const bool turkic = false, const bool lithuanian = false) const { GetCaseConverted(src, srcIdx, srcCount, dest, clrDest, ccTitle, turkic, lithuanian); }
	template<typename TSrcVec, typename TDestCh> void GetLowerCase(const TSrcVec& src, TVec<TDestCh>& dest, const bool clrDest = true, const bool turkic = false, const bool lithuanian = false) const { GetLowerCase(src, 0, src.Len(), dest, clrDest, turkic, lithuanian); }
	template<typename TSrcVec, typename TDestCh> void GetUpperCase(const TSrcVec& src, TVec<TDestCh>& dest, const bool clrDest = true, const bool turkic = false, const bool lithuanian = false) const { GetUpperCase(src, 0, src.Len(), dest, clrDest, turkic, lithuanian); }
	template<typename TSrcVec, typename TDestCh> void GetTitleCase(const TSrcVec& src, TVec<TDestCh>& dest, const bool clrDest = true, const bool turkic = false, const bool lithuanian = false) const { GetTitleCase(src, 0, src.Len(), dest, clrDest, turkic, lithuanian); }

	// GetSimpleCaseConverted uses only the simple case mappings (from UnicodeData.txt).
	// This is simpler and faster.  Since each character now maps into exactly one
	// character, case conversion can also be done in place (see ToSimpleCaseConverted, etc.).
	template<typename TSrcVec, typename TDestCh> void GetSimpleCaseConverted(const TSrcVec& src, size_t srcIdx, const size_t srcCount, TVec<TDestCh>& dest, const bool clrDest, const TCaseConversion how) const;
	template<typename TSrcVec, typename TDestCh> void GetSimpleLowerCase(const TSrcVec& src, size_t srcIdx, const size_t srcCount, TVec<TDestCh>& dest, const bool clrDest = true) const { GetSimpleCaseConverted(src, srcIdx, srcCount, dest, clrDest, ccLower); }
	template<typename TSrcVec, typename TDestCh> void GetSimpleUpperCase(const TSrcVec& src, size_t srcIdx, const size_t srcCount, TVec<TDestCh>& dest, const bool clrDest = true) const { GetSimpleCaseConverted(src, srcIdx, srcCount, dest, clrDest, ccUpper); }
	template<typename TSrcVec, typename TDestCh> void GetSimpleTitleCase(const TSrcVec& src, size_t srcIdx, const size_t srcCount, TVec<TDestCh>& dest, const bool clrDest = true) const { GetSimpleCaseConverted(src, srcIdx, srcCount, dest, clrDest, ccTitle); }
	template<typename TSrcVec, typename TDestCh> void GetSimpleLowerCase(const TSrcVec& src, TVec<TDestCh>& dest, const bool clrDest = true) const { GetSimpleLowerCase(src, 0, src.Len(), dest, clrDest); }
	template<typename TSrcVec, typename TDestCh> void GetSimpleUpperCase(const TSrcVec& src, TVec<TDestCh>& dest, const bool clrDest = true) const { GetSimpleUpperCase(src, 0, src.Len(), dest, clrDest); }
	template<typename TSrcVec, typename TDestCh> void GetSimpleTitleCase(const TSrcVec& src, TVec<TDestCh>& dest, const bool clrDest = true) const { GetSimpleTitleCase(src, 0, src.Len(), dest, clrDest); }

	template<typename TSrcVec> void ToSimpleCaseConverted(TSrcVec& src, size_t srcIdx, const size_t srcCount, const TCaseConversion how) const;
	template<typename TSrcVec> void ToSimpleUpperCase(TSrcVec& src, size_t srcIdx, const size_t srcCount) const { ToSimpleCaseConverted(src, srcIdx, srcCount, ccUpper); }
	template<typename TSrcVec> void ToSimpleLowerCase(TSrcVec& src, size_t srcIdx, const size_t srcCount) const { ToSimpleCaseConverted(src, srcIdx, srcCount, ccLower); }
	template<typename TSrcVec> void ToSimpleTitleCase(TSrcVec& src, size_t srcIdx, const size_t srcCount) const { ToSimpleCaseConverted(src, srcIdx, srcCount, ccTitle); }
	template<typename TSrcVec> void ToSimpleUpperCase(TSrcVec& src) const { ToSimpleUpperCase(src, 0, src.Len()); }
	template<typename TSrcVec> void ToSimpleLowerCase(TSrcVec& src) const { ToSimpleLowerCase(src, 0, src.Len()); }
	template<typename TSrcVec> void ToSimpleTitleCase(TSrcVec& src) const { ToSimpleTitleCase(src, 0, src.Len()); }

public:
	friend class TUniCaseFolding;

	// Case folding is an alternative to the above functions.  It is intended primarily
	// to produce strings that are suitable for comparisons.  For example,
	// ToLowerCase(sigma) = sigma, ToLowerCase(final-sigma) = final-sigma;
	// but ToCaseFolder(sigma) = sigma, ToCaseFolded(final-sigma) = sigma.
	// - 'turkic' enables special processing for Turkic languages (I-dot and i-dotless).
	// - 'full' enables full case mappings -- i.e. sometimes a character may be mapped
	//   into a string of two or more characters.
	// - Note: For best results, perform NFD(CaseFold(NFD(x)) or NFKD(CaseFold(NFKD(x)) on
	//   each string before comparing them (see sec. 3.13 of the standard).
	template<typename TSrcVec, typename TDestCh>
	void GetCaseFolded(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, const bool clrDest, const bool full, const bool turkic = false) const { caseFolding.Fold(src, srcIdx, srcCount, dest, clrDest, full, turkic); }
	template<typename TSrcVec, typename TDestCh>
	void GetCaseFolded(const TSrcVec& src, TVec<TDestCh>& dest, const bool clrDest = true, const bool full = true, const bool turkic = false) const {
		GetCaseFolded(src, 0, src.Len(), dest, clrDest, full, turkic); }
	// ToCaseFolded folds the string in place.  However, this means that only the simple
	// case foldings can be used (the full ones could increase the length of the string).
	template<typename TSrcVec> void ToCaseFolded(TSrcVec& src, size_t srcIdx, const size_t srcCount, const bool turkic = false) const { caseFolding.FoldInPlace(src, srcIdx, srcCount, turkic); }
	template<typename TSrcVec> void ToCaseFolded(TSrcVec& src, const bool turkic = false) const { ToCaseFolded(src, 0, src.Len(), turkic); }

protected:
	void TestCaseConversion(const TStr& source, const TStr& trueLc, const TStr& trueTc, const TStr& trueUc, bool turkic, bool lithuanian);
	void TestCaseConversions();

	//-------------------------------------------------------------------------
	// Text file reader for the Unicode character database
	//-------------------------------------------------------------------------

protected:

	class TUcdFileReader
	{
	protected:
		TChA buf;
	public:
		TChA comment; // contains '#' and everything after it
	protected:
		FILE *f;
		int putBackCh;
		int GetCh() {
			if (putBackCh >= 0) { int c = putBackCh; putBackCh = EOF; return c; }
			return fgetc(f); }
		void PutBack(int c) { Assert(putBackCh == EOF); putBackCh = c; }
		// Returns 'false' iff the EOF was encountered before anything was read.
		bool ReadNextLine() {
			buf.Clr(); comment.Clr();
			bool inComment = false, first = true;
			while (true) {
				int c = GetCh();
				if (c == EOF) return ! first;
				else if (c == 13) {
					c = GetCh(); if (c != 10) PutBack(c);
					return true; }
				else if (c == 10) return true;
				else if (c == '#') inComment = true;
				if (! inComment) buf += char(c);
				else comment += char(c); }
				/*first = false;*/}
	private:
		TUcdFileReader& operator = (const TUcdFileReader& r) { Fail; return *((TUcdFileReader *) 0); }
		TUcdFileReader(const TUcdFileReader& r) { Fail; }
	public:
		TUcdFileReader() : f(0) { }
		TUcdFileReader(const TStr& fileName) : f(0), putBackCh(EOF) { Open(fileName); }
		void Open(const TStr& fileName) { Close(); f = fopen(fileName.CStr(), "rt"); IAssertR(f, fileName); putBackCh = EOF; }
		void Close() { putBackCh = EOF; if (f) { fclose(f); f = 0; }}
		~TUcdFileReader() { Close(); }
		bool GetNextLine(TStrV& dest) {
			dest.Clr();
			while (true) {
				if (! ReadNextLine()) return false;
				TStr line = buf; line.ToTrunc();
				if (line.Len() <= 0) continue;
				line.SplitOnAllCh(';', dest, false);
				for (int i = 0; i < dest.Len(); i++) dest[i].ToTrunc();
				return true; }}
		static int ParseCodePoint(const TStr& s) {
			int c; bool ok = s.IsHexInt(true, 0, 0x10ffff, c); IAssertR(ok, s); return c; }
		static void ParseCodePointList(const TStr& s, TIntV& dest, bool ClrDestP = true) { // space-separated list
			if (ClrDestP) dest.Clr();
			TStrV parts; s.SplitOnWs(parts);
			for (int i = 0; i < parts.Len(); i++) {
				int c; bool ok = parts[i].IsHexInt(true, 0, 0x10ffff, c); IAssertR(ok, s);
				dest.Add(c); } }
		static void ParseCodePointRange(const TStr& s, int& from, int &to) { // xxxx or xxxx..yyyy
			int i = s.SearchStr(".."); if (i < 0) { from = ParseCodePoint(s); to = from; return; }
			from = ParseCodePoint(s.GetSubStr(0, i - 1));
			to = ParseCodePoint(s.GetSubStr(i + 2, s.Len() - 1)); }
	};

	//-------------------------------------------------------------------------
	// Helper class for processing the text files
	//-------------------------------------------------------------------------
	// Files such as DerivedCoreProps.txt often refer to ranges of codepoints,
	// and not all codepoints from the range have also been listed in
	// UnicodeData.txt.  Thus, new TUniChInfo instances will be created
	// when processing DerivedCoreProps.txt and similar files.
	// To assign the correct (sub)categories to these new codepoints,
	// the following class will extract the subcategory info from the
	// comments in DerivedCoreProps.txt and similar files.

	class TSubcatHelper
	{
	public:
		bool hasCat; TUniChSubCategory subCat;
		TStrH invalidCatCodes;
		TUniChDb &owner;

		TSubcatHelper(TUniChDb &owner_) : owner(owner_) { }

		void ProcessComment(TUniChDb::TUcdFileReader &reader)
		{
			hasCat = false; subCat = ucOtherNotAssigned;
			if (reader.comment.Len() > 3)
			{
				IAssert(reader.comment[0] == '#');
				IAssert(reader.comment[1] == ' ');
				char chCat = reader.comment[2], chSubCat = reader.comment[3];
				if (reader.comment.Len() > 4) IAssert(isspace(uchar(reader.comment[4])));
				if (TUniChInfo::IsValidSubCat(chCat, chSubCat)) {
					hasCat = true; subCat = (TUniChSubCategory) ((int(uchar(chCat)) << 8) | (int(uchar(chSubCat)))); }
				else invalidCatCodes.AddKey(TStr(chCat) + TStr(chSubCat));
			}
		}

		void SetCat(const int cp) {
			int i = owner.h.GetKeyId(cp); IAssert(i >= 0);
			IAssert(owner.h[i].subCat == ucOtherNotAssigned);
			IAssert(hasCat);
			owner.h[i].SetCatAndSubCat(subCat); }
		void TestCat(const int cp) {
			if (! hasCat) return;
			int i = owner.h.GetKeyId(cp); IAssert(i >= 0);
			IAssert(owner.h[i].subCat == subCat); }

		~TSubcatHelper()
		{
			if (invalidCatCodes.IsKey("L&")) invalidCatCodes.DelKey("L&");
			// Output any unexpected ones (there shouldn't be any).
			if (! invalidCatCodes.Empty()) {
				printf("Invalid cat code(s) in the comments: ");
				for (int i = invalidCatCodes.FFirstKeyId(); invalidCatCodes.FNextKeyId(i); )
					printf(" \"%s\"", invalidCatCodes.GetKey(i).CStr());
				printf("\n"); }
		}
	};
};

//-----------------------------------------------------------------------------
// TUnicode -- a sadly emasculated wrapper around TUniCodec and TUniChDb
//-----------------------------------------------------------------------------

class TUnicode
{
public:
	TUniCodec codec;
	TUniChDb ucd;

	TUnicode() { Init(); }
	explicit TUnicode(const TStr& fnBinUcd) { ucd.LoadBin(fnBinUcd); Init(); }
	void Init() { InitCodecs(); }

	//-----------------------------------------------------------------------
	// UTF-8
	//-----------------------------------------------------------------------

	// Returns the number of characters that have been successfully decoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	int DecodeUtf8(const TIntV& src, TIntV& dest) const { return (int) codec.DecodeUtf8(src, dest); }
	int DecodeUtf8(const TStr& src, TIntV& dest) const { return (int) codec.DecodeUtf8(src, dest); }

	// Returns the number of characters that have been successfully encoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	int EncodeUtf8(const TIntV& src, TIntV& dest) const { return (int) codec.EncodeUtf8(src, dest); }

	// The following wrapper around the UTF-8 encoder returns a TStr containing
	// the UTF-8-encoded version of the input string.
	TStr EncodeUtf8Str(const TIntV& src) const { return codec.EncodeUtf8Str(src); }

	//-----------------------------------------------------------------------
	// UTF-16 Decoder
	//-----------------------------------------------------------------------

	// Returns the number of characters that have been successfully decoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	// Each element of 'src' is assumed to contain one byte of data.
	// srcCount must be even (though srcIdx doesn't need to be).
	int DecodeUtf16FromBytes(const TIntV& src, TIntV& dest,
		const TUtf16BomHandling bomHandling = bomAllowed,
		const TUniByteOrder defaultByteOrder = boMachineEndian) const {
			return (int) codec.DecodeUtf16FromBytes(src, 0, src.Len(), dest, true, bomHandling, defaultByteOrder); }

	// Here, each element of 'src' is treated as a 16-bit word.  The byte-order settings
	// are used to determine if the two bytes of each word should be swapped before further
	// processing.  For example, if a BOM is present, it must have the value 0xfeff; if it
	// actually has the value 0xfffe, this means that the two bytes of each word must be swapped.
	// Basically, the combination of the byteOrder parameter and the byte order mark (if present) at the
	// beginning of the source data is used to determine the "original" byte order of the data;
	// if this doesn't match the byte order of the local machine, the two bytes of each word will
	// be swapped during the decoding process.
	int DecodeUtf16FromWords(const TIntV& src, TIntV& dest,
		const TUtf16BomHandling bomHandling = bomAllowed,
		const TUniByteOrder defaultByteOrder = boMachineEndian) const {
			return (int) codec.DecodeUtf16FromWords(src, 0, src.Len(), dest, true, bomHandling, defaultByteOrder); }

	//-----------------------------------------------------------------------
	// UTF-16 Encoder
	//-----------------------------------------------------------------------

	// Returns the number of characters that have been successfully encoded.
	// This does not include any replacement characters that may have been inserted into 'dest'.
	int EncodeUtf16ToWords(const TIntV& src, TIntV& dest, const bool insertBom,
		const TUniByteOrder destByteOrder = boMachineEndian) const {
			return (int) codec.EncodeUtf16ToWords(src, 0, src.Len(), dest, true, insertBom, destByteOrder); }

	int EncodeUtf16ToBytes(const TIntV& src, TIntV& dest, const bool insertBom,
		const TUniByteOrder destByteOrder = boMachineEndian) const {
			return (int) codec.EncodeUtf16ToBytes(src, 0, src.Len(), dest, true, insertBom, destByteOrder); }

	//-----------------------------------------------------------------------
	// 8-bit codecs
	//-----------------------------------------------------------------------

	T8BitCodec<TEncoding_ISO8859_1> iso8859_1;
	T8BitCodec<TEncoding_ISO8859_2> iso8859_2;
	T8BitCodec<TEncoding_ISO8859_3> iso8859_3;
	T8BitCodec<TEncoding_ISO8859_4> iso8859_4;
	T8BitCodec<TEncoding_YuAscii> yuAscii;
	T8BitCodec<TEncoding_CP1250> cp1250;
	T8BitCodec<TEncoding_CP852> cp852;
	T8BitCodec<TEncoding_CP437> cp437;

	//-----------------------------------------------------------------------
	// Codec registry
	//-----------------------------------------------------------------------
	// If you know you'll need ISO-8859-2, just use
	//   TUnicode unicode;
	//   unicode.iso8859_2.Encode(...);
	// If you don't know what you'll need, use:
	//   TUnicode unicode;
	//   PCodecBase myCodec = unicode.GetCodec(myCodecName);
	//   myCodec->Encode(...);
	// Note that the first approach is slightly more efficient because there
	// aren't any virtual method calls involved.

protected:
	THash<TStr, PCodecBase> codecs;
	static inline TStr NormalizeCodecName(const TStr& name) {
		TStr s = name.GetLc(); s.ChangeStrAll("_", ""); s.ChangeStrAll("-", ""); return s; }
public:
	void RegisterCodec(const TStr& nameList, const PCodecBase& codec) {
		TStrV names; nameList.SplitOnWs(names);
		for (int i = 0; i < names.Len(); i++)
			codecs.AddDat(NormalizeCodecName(names[i]), codec); }
	void UnregisterCodec(const TStr& nameList) {
		TStrV names; nameList.SplitOnWs(names);
		for (int i = 0; i < names.Len(); i++)
			codecs.DelKey(NormalizeCodecName(names[i])); }
	void ClrCodecs() { codecs.Clr(); }
	void InitCodecs();
	PCodecBase GetCodec(const TStr& name) const {
		TStr s = NormalizeCodecName(name);
		PCodecBase p; if (! codecs.IsKeyGetDat(s, p)) p.Clr();
		return p; }
	void GetAllCodecs(TCodecBaseV& dest) const {
		dest.Clr();
		for (int i = codecs.FFirstKeyId(); codecs.FNextKeyId(i); ) {
			PCodecBase codec = codecs[i]; bool found = false;
			for (int j = 0; j < dest.Len(); j++) if (dest[j]() == codec()) { found = true; break; }
			if (! found) dest.Add(codec); }}

	//-------------------------------------------------------------------------
	// Word boundaries (UAX #29)
	//-------------------------------------------------------------------------

	// Finds the next word boundary strictly after 'position'.
	// Note that there are valid word boundaries at 0 and at 'src.Len()'.
	// If there is no such word boundary, it returns 'false' and sets 'position' to 'src.Len()'.
	bool FindNextWordBoundary(const TIntV& src, int &position) const {
		if (position < 0) { position = 0; return true; }
		size_t position_; bool retVal = ucd.FindNextWordBoundary(src, 0, src.Len(), position_); position = int(position_); return retVal; }
	// Creates, in 'dest', a vector of 'src.Len() + 1' elements, where 'dest[i]' tells if there is a word
	// boundary between 'src[i - 1]' and 'src[i]'.  Note that 'dest[0]' and 'dest[src.Len()]' are
	// always set to 'true'.
	void FindWordBoundaries(const TIntV& src, TBoolV& dest) const { ucd.FindWordBoundaries(src, 0, src.Len(), dest); }

	//-------------------------------------------------------------------------
	// Sentence boundaries (UAX #29)
	//-------------------------------------------------------------------------

	// Finds the next sentence boundary strictly after 'position'.
	// Note that there are valid sentence boundaries at 0 and at 'src.Len()'.
	// If there is no such sentence boundary, it returns 'false' and sets 'position' to 'src.Len()'.
	bool FindNextSentenceBoundary(const TIntV& src, int &position) const {
		if (position < 0) { position = 0; return true; }
		size_t position_; bool retVal = ucd.FindNextSentenceBoundary(src, 0, src.Len(), position_); position = int(position_); return retVal; }
	// Creates, in 'dest', a vector of 'src.Len() + 1' elements, where 'dest[i]' tells if there is a sentence
	// boundary between 'src[i - 1]' and 'src[i]'.  Note that 'dest[0]' and 'dest[src.Len()]' are
	// always set to 'true'.
	void FindSentenceBoundaries(const TIntV& src, TBoolV& dest) const { ucd.FindSentenceBoundaries(src, 0, src.Len(), dest); }

	void ClrSentenceBoundaryExceptions() { ucd.SbEx_Clr(); }
	void UseEnglishSentenceBoundaryExceptions() { ucd.SbEx_SetStdEnglish(); }

	//-------------------------------------------------------------------------
	// Normalization, decomposition, etc. (UAX #15)
	//-------------------------------------------------------------------------

	// This sets 'dest' to the decomposed form of the source string.
	// - for normalization form D (NFD), i.e. canonical decomposition: use compatibility == false;
	// - for normalization form KD (NFKD), i.e. compatibility decomposition: use compatibility == true.
	void Decompose(const TIntV& src, TIntV& dest, bool compatibility) const { ucd.Decompose(src, dest, compatibility, true); }
	// This performs canonical composition on the source string, and stores
	// the result in the destination vector.  The source string should be the
	// result of a (canonical or compatibility) decomposition; if this is the
	// case, the composition will lead to a normalization form C (NFC) or
	// normalization form KC (NFKC), depending on whether canonical or compatibility
	// decomposition was used.
	void Compose(const TIntV& src, TIntV& dest) const { return ucd.Compose(src, dest, true); }
	// Calls Decompose, followed by Compose; thus the result is the NFC (if
	// compatibility == false) or NFKC (if compatibility == true) of the source string.
	// A temporary TIntV is used to contain the intermediate NF(K)D form of the
	// source string.
	void DecomposeAndCompose(const TIntV& src, TIntV& dest, bool compatibility) const { return ucd.DecomposeAndCompose(src, dest, compatibility); }
	// Copies the starter characters from 'src' to 'dest'; the other
	// characters are skipped.  'src' should already have been decomposed.
	// Returns the number of characters extracted.  This function can be
	// used to remove diacritical marks from a string (after it has been decomposed!).
	int ExtractStarters(const TIntV& src, TIntV& dest) const { return (int) ucd.ExtractStarters(src, dest); }
	// Extracts the starters into a temporary vector and then copies it into 'src'.
	int ExtractStarters(TIntV& src) const { return (int) ucd.ExtractStarters(src); }

	//-------------------------------------------------------------------------
	// Case conversions
	//-------------------------------------------------------------------------
	// NOTE: if you will be dealing with Turkish, Azeri or Lithuanian text,
	// use the case-conversion methods in TUniChDb, which allow the caller
	// to request language-specific case mappings for these languages.

public:
	typedef TUniChDb::TCaseConversion TCaseConversion;
	// Sets 'dest' to the case-converted form of 'src'.
	void GetLowerCase(const TIntV& src, TIntV& dest) const { ucd.GetLowerCase(src, dest, true, false, false); }
	void GetUpperCase(const TIntV& src, TIntV& dest) const { ucd.GetUpperCase(src, dest, true, false, false); }
	void GetTitleCase(const TIntV& src, TIntV& dest) const { ucd.GetTitleCase(src, dest, true, false, false); }

	// GetSimpleCaseConverted uses only the simple case mappings (from UnicodeData.txt).
	// This is simpler and faster.  Since each character now maps into exactly one
	// character, case conversion can also be done in place (see ToSimpleCaseConverted, etc.).
	void GetSimpleLowerCase(const TIntV& src, TIntV& dest) const { ucd.GetSimpleLowerCase(src, dest, true); }
	void GetSimpleUpperCase(const TIntV& src, TIntV& dest) const { ucd.GetSimpleUpperCase(src, dest, true); }
	void GetSimpleTitleCase(const TIntV& src, TIntV& dest) const { ucd.GetSimpleTitleCase(src, dest, true); }

	// These functions perform simple case-conversions in-place.
	void ToSimpleUpperCase(TIntV& src) const { ucd.ToSimpleUpperCase(src); }
	void ToSimpleLowerCase(TIntV& src) const { ucd.ToSimpleLowerCase(src); }
	void ToSimpleTitleCase(TIntV& src) const { ucd.ToSimpleTitleCase(src); }

	// Case folding is an alternative to the above functions.  It is intended primarily
	// to produce strings that are suitable for comparisons.  For example,
	// ToLowerCase(sigma) = sigma, ToLowerCase(final-sigma) = final-sigma;
	// but ToCaseFolded(sigma) = sigma, ToCaseFolded(final-sigma) = sigma.
	// - 'full' enables full case mappings -- i.e. sometimes a character may be mapped
	//   into a string of two or more characters.
	// - Note: For best results, perform NFD(CaseFold(NFD(x)) or NFKD(CaseFold(NFKD(x)) on
	//   each string before comparing them (see sec. 3.13 of the standard).
	void GetCaseFolded(const TIntV& src, TIntV& dest, const bool full = true) const { return ucd.GetCaseFolded(src, dest, true, full, false); }
	// ToCaseFolded folds the string in place.  However, this means that only the simple
	// case foldings can be used (the full ones could increase the length of the string).
	void ToCaseFolded(TIntV& src) const { return ucd.ToCaseFolded(src, false); }

	TStr GetUtf8CaseFolded(const TStr& s) const {
		bool isAscii = true;
		for (int i = 0, n = s.Len(); i < n; i++) if (uchar(s[i]) >= 128) { isAscii = false; break; }
		if (isAscii) return s.GetLc();
		TIntV src; DecodeUtf8(s, src);
		TIntV dest; GetCaseFolded(src, dest);
		return EncodeUtf8Str(dest); }

	//-------------------------------------------------------------------------
	// Character properties
	//-------------------------------------------------------------------------
	// These methods simply call the corresponding TUniChDb method
	// (which typically calls the corresponding method of TUniChInfo).
	// See the declaration for DECLARE_FORWARDED_PROPERTY_METHODS for a complete list.
	// They are all of the form        bool IsXxxx(const int cp) const
	// Some of the more notable ones include:
	// - IsAlphabetic, IsUppercase, IsLowercase, IsMath, IsAsciiHexDigit
	//   IsDash, IsDeprecated, IsDiacritic, IsHexDigit, IsHyphen, IsIdeographic
	//   IsNoncharacter, IsQuotationMark, IsSoftDotted, IsTerminalPunctuation, IsWhiteSpace

#define ___UniFwd1(name) bool name(const int cp) const { return ucd.name(cp); }
	DECLARE_FORWARDED_PROPERTY_METHODS
#undef DECLARE_FORWARDED_PROPERTY_METHODS
#undef __UniFwd1
	___UniFwd2(IsPrivateUse, IsSurrogate)

	TUniChCategory GetCat(const int cp) const { return ucd.GetCat(cp); }
	TUniChSubCategory GetSubCat(const int cp) const { return ucd.GetSubCat(cp); }

	// GetCharName returns 0 if the name is unknown; GetCharNameS returns a string of the form "U+1234".
	const char *GetCharName(const int cp) const { return ucd.GetCharName(cp); }
	TStr GetCharNameS(const int cp) const { return ucd.GetCharNameS(cp); }

};

//-----------------------------------------------------------------------------
// TUniCodec -- UTF-8 Decoder
//-----------------------------------------------------------------------------

// Returns the number of characters that have been successfully decoded.
// This does not include any replacement characters that may have been inserted into 'dest'.
template<typename TSrcVec, typename TDestCh>
size_t TUniCodec::DecodeUtf8(
	const TSrcVec& src, size_t srcIdx, const size_t srcCount,
	TVec<TDestCh>& dest, const bool clrDest) const
{
	size_t nDecoded = 0;
	if (clrDest) dest.Clr();
	const size_t origSrcIdx = srcIdx;
	const size_t srcEnd = srcIdx + srcCount;
	while (srcIdx < srcEnd)
	{
		const size_t charSrcIdx = srcIdx;
		uint c = src[TVecIdx(srcIdx)] & 0xff; srcIdx++;
		if ((c & _1000_0000) == 0) {
			// c is one of the characters 0..0x7f, encoded as a single byte.
			dest.Add(TDestCh(c)); nDecoded++; continue; }
		else if ((c & _1100_0000) == _1000_0000) {
			// No character in a valid UTF-8-encoded string should begin with a byte of the form 10xxxxxx.
			// We must have been thrown into the middle of a multi-byte character.
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(charSrcIdx, c, "Invalid character: 10xxxxxx.");
			case uehAbort: return nDecoded;
			case uehReplace: dest.Add(TDestCh(replacementChar)); continue;
			case uehIgnore: continue;
			default: Fail; } }
		else
		{
			// c introduces a sequence of 2..6 bytes, depending on how many
			// of the most significant bits of c are set.
			uint nMoreBytes = 0, nBits = 0, minVal = 0;
			if ((c & _1110_0000) == _1100_0000) nMoreBytes = 1, nBits = 5, minVal = 0x80;
			else if ((c & _1111_0000) == _1110_0000) nMoreBytes = 2, nBits = 4, minVal = 0x800;
			else if ((c & _1111_1000) == _1111_0000) nMoreBytes = 3, nBits = 3, minVal = 0x10000;
			else if ((c & _1111_1100) == _1111_1000) nMoreBytes = 4, nBits = 2, minVal = 0x200000;
			else if ((c & _1111_1110) == _1111_1100) nMoreBytes = 5, nBits = 1, minVal = 0x4000000;
			else {
				// c is of the form 1111111x, which is invalid even in the early definitions of UTF-8
				// (which allowed the encoding of codepoints up to 2^31 - 1).  However, in principle this
				// could be used to encode 32-bit integers with the msb set: 1aaabbbbccccddddeeeeffffgggghhhh
				// could be encoded as 1111111a 10aabbbb 10ccccdd 10ddeeee 10ffffgg 10gghhhh.
				if (strict)  {
					switch (errorHandling) {
					case uehThrow: throw TUnicodeException(charSrcIdx, c, "Invalid character: 1111111x.");
					case uehAbort: return nDecoded;
					// In the case of uehReplace and uehIgnore, we'll read the next 5 bytes
					// and try to decode the character.  Then, since 'strict' is true and
					// the codepoint is clearly >= 2^31, we'll notice this as an error later
					// and (in the case of uehReplace) insert a replacement character then.
					// This is probably better than inserting a replacement character right
					// away and then trying to read the next byte as if a new character
					// was beginning there -- if the current byte is really followed by five
					// 10xxxxxx bytes, we'll just get six replacement characters in a row.
					case uehReplace: break; //dest.Add(TDestCh(replacementChar)); continue;
					case uehIgnore: break; // continue;
					default: Fail; } }
				nMoreBytes = 5; nBits = 2; minVal = 0x80000000u; }
			// Decode this multi-byte sequence.
			uint cOut = c & ((1 << nBits) - 1); // First extract the nBits least significant bits from c.
			bool cancel = false;
			for (uint i = 0; i < nMoreBytes && ! cancel; i++) {
				// See if there are enough bytes left in the source vector.
				if (! (srcIdx < srcEnd)) {
					switch (errorHandling) {
					case uehThrow: throw TUnicodeException(charSrcIdx, c, TInt::GetStr(nMoreBytes) + " more bytes expected, only " + TInt::GetStr(int(srcEnd - charSrcIdx - 1)) + " available.");
					case uehAbort: return nDecoded;
					case uehReplace: dest.Add(TDestCh(replacementChar)); cancel = true; continue;
					case uehIgnore: cancel = true; continue;
					default: Fail; } }
				// Read the next byte.
				c = src[TVecIdx(srcIdx)] & 0xff; srcIdx++;
				if ((c & _1100_0000) != _1000_0000) { // Each subsequent byte should be of the form 10xxxxxx.
					switch (errorHandling) {
					case uehThrow: throw TUnicodeException(charSrcIdx, c, "Byte " + TInt::GetStr(i) + " of " + TInt::GetStr(nMoreBytes) + " extra bytes should begin with 10xxxxxx.");
					case uehAbort: return nDecoded;
					case uehReplace: dest.Add(TDestCh(replacementChar)); srcIdx--; cancel = true; continue;
					case uehIgnore: srcIdx--; cancel = true; continue;
					default: Fail; } }
				cOut <<= 6; cOut |= (c & _0011_1111); }
			if (cancel) continue;
			if (strict) {
				// err1: This codepoint has been represented by more bytes than it should have been.
				// For example, cOut in the range 0..127 should be represented by a single byte,
				// not by two or more bytes.
				// - For example, this may happen in the "modified UTF-8" sometimes used for Java
				// serialization, where the codepoint 0 is encoded as 11000000 10000000 to avoid
				// the appearance of null bytes in the encoded stream.
				bool err1 = (cOut < minVal);
				// err2: Early definitions of UTF-8 allowed any 31-bit integer to be encoded, using up to 6 bytes.
				// However, later this was restricted to the codepoints 0..0x10ffff only, because only these
				// are valid Unicode codepoints.  Thus, no more than 4 bytes are ever necessary.
				bool err2 = (nMoreBytes > 3 || (nMoreBytes == 3 && cOut > 0x10ffff));
				if (err1 || err2) switch (errorHandling) {
					case uehThrow:
						if (err1) throw TUnicodeException(charSrcIdx, c, "The codepoint 0x" + TInt::GetStr(cOut, "%08x") + " has been represented by too many bytes (" + TInt::GetStr(nMoreBytes + 1) + ").");
						else if (err2) throw TUnicodeException(charSrcIdx, c, "Invalid multibyte sequence: it decodes into 0x" + TInt::GetStr(cOut, "%08x") + ", but only codepoints 0..0x10ffff are valid.");
						else { Fail; break; }
					case uehAbort: return nDecoded;
					case uehReplace: dest.Add(TDestCh(replacementChar)); continue;
					case uehIgnore: continue;
					default: Fail; } }
			// Add the decoded codepoint to the destination vector.
			// If this is the first decoded character, and it's one of the byte-order marks
			// (0xfffe and 0xfeff), we will skip it (unless skipBom is false).
			if (! (skipBom && (cOut == 0xfffe || cOut == 0xfeff) && charSrcIdx == origSrcIdx)) {
				dest.Add(cOut); nDecoded++; }
		} // else (multi-byte sequence)
	} // while
	return nDecoded;
}

//-----------------------------------------------------------------------
// TUniCodec -- UTF-8 Encoder
//-----------------------------------------------------------------------

// Returns the number of characters that have been successfully encoded.
// This does not include any replacement characters that may have been inserted into 'dest'.
template<typename TSrcVec, typename TDestCh>
size_t TUniCodec::EncodeUtf8(
	const TSrcVec& src, size_t srcIdx, const size_t srcCount,
	TVec<TDestCh>& dest, const bool clrDest) const
{
	size_t nEncoded = 0;
	for (const size_t srcEnd = srcIdx + srcCount; srcIdx < srcEnd; srcIdx++)
	{
		uint c = uint(src[TVecIdx(srcIdx)]);
		bool err = false;
		if (strict && c > 0x10ffff) {
			err = true;
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(srcIdx, c, "Invalid character (0x" + TInt::GetStr(c, "%x") + "; only characters in the range 0..0x10ffff are allowed).");
			case uehAbort: return nEncoded;
			case uehReplace: c = replacementChar; break;
			case uehIgnore: continue;
			default: Fail; } }
		if (c < 0x80u)
			dest.Add(TDestCh(c & 0xffu));
		else if (c < 0x800u) {
			dest.Add(TDestCh(_1100_0000 | ((c >> 6) & _0001_1111)));
			dest.Add(TDestCh(_1000_0000 | (c & _0011_1111))); }
		else if (c < 0x10000u) {
			dest.Add(TDestCh(_1110_0000 | ((c >> 12) & _0000_1111)));
			dest.Add(TDestCh(_1000_0000 | ((c >> 6) & _0011_1111)));
			dest.Add(TDestCh(_1000_0000 | (c & _0011_1111))); }
		else if (c < 0x200000u) {
			dest.Add(TDestCh(_1111_0000 | ((c >> 18) & _0000_0111)));
			dest.Add(TDestCh(_1000_0000 | ((c >> 12) & _0011_1111)));
			dest.Add(TDestCh(_1000_0000 | ((c >> 6) & _0011_1111)));
			dest.Add(TDestCh(_1000_0000 | (c & _0011_1111))); }
		else if (c < 0x4000000u) {
			dest.Add(TDestCh(_1111_1000 | ((c >> 24) & _0000_0011)));
			dest.Add(TDestCh(_1000_0000 | ((c >> 18) & _0011_1111)));
			dest.Add(TDestCh(_1000_0000 | ((c >> 12) & _0011_1111)));
			dest.Add(TDestCh(_1000_0000 | ((c >> 6) & _0011_1111)));
			dest.Add(TDestCh(_1000_0000 | (c & _0011_1111))); }
		else {
			dest.Add(TDestCh(_1111_1100 | ((c >> 30) & _0000_0011)));
			dest.Add(TDestCh(_1000_0000 | ((c >> 24) & _0011_1111)));
			dest.Add(TDestCh(_1000_0000 | ((c >> 18) & _0011_1111)));
			dest.Add(TDestCh(_1000_0000 | ((c >> 12) & _0011_1111)));
			dest.Add(TDestCh(_1000_0000 | ((c >> 6) & _0011_1111)));
			dest.Add(TDestCh(_1000_0000 | (c & _0011_1111))); }
		if (! err) nEncoded++;
	}
	return nEncoded;
}

//-----------------------------------------------------------------------
// TUniCodec -- UTF-16 Encoder
//-----------------------------------------------------------------------

// Returns the number of characters that have been successfully decoded.
// This does not include any replacement characters that may have been inserted into 'dest'.
// Each element of 'src' is assumed to contain one byte of data.
// srcCount must be even (though srcIdx doesn't need to be).
template<typename TSrcVec, typename TDestCh>
size_t TUniCodec::DecodeUtf16FromBytes(
	const TSrcVec& src, size_t srcIdx, const size_t srcCount,
	TVec<TDestCh>& dest, const bool clrDest,
	const TUtf16BomHandling bomHandling,
	const TUniByteOrder defaultByteOrder) const
{
	IAssert(srcCount % 2 == 0);
	IAssert(bomHandling == bomAllowed || bomHandling == bomRequired || bomHandling == bomIgnored);
	IAssert(defaultByteOrder == boMachineEndian || defaultByteOrder == boBigEndian || defaultByteOrder == boLittleEndian);
	if (clrDest) dest.Clr();
	size_t nDecoded = 0;
	if (srcCount <= 0) return nDecoded;
	const size_t origSrcIdx = srcIdx, srcEnd = srcIdx + srcCount;
	bool littleEndian = false;
  bool leDefault = (defaultByteOrder == boLittleEndian || (defaultByteOrder == boMachineEndian && IsMachineLittleEndian()));
	if (bomHandling == bomIgnored) littleEndian = leDefault;
	else if (bomHandling == bomAllowed || bomHandling == bomRequired)
	{
		int byte1 = uint(src[TVecIdx(srcIdx)]) & 0xff, byte2 = uint(src[TVecIdx(srcIdx + 1)]) & 0xff;
		if (byte1 == 0xfe && byte2 == 0xff) { littleEndian = false; if (skipBom) srcIdx += 2; }
		else if (byte1 == 0xff && byte2 == 0xfe) { littleEndian = true; if (skipBom) srcIdx += 2; }
		else if (bomHandling == bomAllowed) littleEndian = leDefault;
		else { // Report an error.
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(srcIdx, byte1, "BOM expected at the beginning of the input vector (" + TInt::GetStr(byte1, "%02x") + " " + TInt::GetStr(byte2, "%02x") + " found instead).");
			case uehAbort: case uehReplace: case uehIgnore: return size_t(-1);
			default: Fail; } }
	}
	else Fail;
	while (srcIdx < srcEnd)
	{
		const size_t charSrcIdx = srcIdx;
		uint byte1 = uint(src[TVecIdx(srcIdx)]) & 0xff, byte2 = uint(src[TVecIdx(srcIdx + 1)]) & 0xff; srcIdx += 2;
		uint c = littleEndian ? (byte1 | (byte2 << 8)) : (byte2 | (byte1 << 8));
		if (Utf16FirstSurrogate <= c && c <= Utf16FirstSurrogate + 1023)
		{
			// c is the first character in a surrogate pair.  Read the next character.
			if (! (srcIdx + 2 <= srcEnd)) {
				switch (errorHandling) {
				case uehThrow: throw TUnicodeException(charSrcIdx, c, "The second character of a surrogate pair is missing.");
				case uehAbort: return nDecoded;
				case uehReplace: dest.Add(TDestCh(replacementChar)); continue;
				case uehIgnore: continue;
				default: Fail; } }
			uint byte1 = uint(src[TVecIdx(srcIdx)]) & 0xff, byte2 = uint(src[TVecIdx(srcIdx + 1)]) & 0xff; srcIdx += 2;
			uint c2 = littleEndian ? (byte1 | (byte2 << 8)) : (byte2 | (byte1 << 8));
			// c2 should be the second character of the surrogate pair.
			if (c2 < Utf16SecondSurrogate || Utf16SecondSurrogate + 1023 < c2) {
				switch (errorHandling) {
				case uehThrow: throw TUnicodeException(charSrcIdx + 2, c2, "The second character of a surrogate pair should be in the range " + TInt::GetStr(Utf16SecondSurrogate, "%04x") + ".." + TInt::GetStr(Utf16SecondSurrogate + 1023, "%04x") + ", not " + TInt::GetStr(c2, "04x") + ".");
				case uehAbort: return nDecoded;
				// with uehReplace and uehIgnore, we'll just skip the first character of the surrogate pair; we'll process the second one during the next iteration, this time as an ordinary character
				case uehReplace: dest.Add(TDestCh(replacementChar)); srcIdx -= 2; continue;
				case uehIgnore: srcIdx -= 2; continue;
				default: Fail; } }
			// c and c2 each contain 10 bits of information.
			uint cc = ((c - Utf16FirstSurrogate) << 10) | (c2 - Utf16SecondSurrogate);
			cc += 0x10000;
			dest.Add(TDestCh(cc)); nDecoded++; continue;
		}
		else if (strict && Utf16SecondSurrogate <= c && c <= Utf16SecondSurrogate + 1023) {
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(charSrcIdx, c, "This 16-bit value should be used only as the second character of a surrogate pair.");
			case uehAbort: return nDecoded;
			case uehReplace: dest.Add(TDestCh(replacementChar)); continue;
			case uehIgnore: continue;
			default: Fail; } }
		// If 'c' is the first character in the input stream, and it's a BOM, we might have to skip it.
		if (charSrcIdx == origSrcIdx && (c == 0xfffeu || c == 0xfeffu) && skipBom) continue;
		// Otherwise, store 'c' to the destination vector.
		dest.Add(TDestCh(c)); nDecoded++;
	}
	return nDecoded;
}

// Here, each element of 'src' is treated as a 16-bit word.  The byte-order settings
// are used to determine if the two bytes of each word should be swapped before further
// processing.  For example, if a BOM is present, it must have the value 0xfeff; if it
// actually has the value 0xfffe, this means that the two bytes of each word must be swapped.
// Basically, the combination of the byteOrder parameter and the byte order mark (if present) at the
// beginning of the source data is used to determine the "original" byte order of the data;
// if this doesn't match the byte order of the local machine, the two bytes of each word will
// be swapped during the decoding process.
template<typename TSrcVec, typename TDestCh>
size_t TUniCodec::DecodeUtf16FromWords(
	const TSrcVec& src, size_t srcIdx, const size_t srcCount,
	TVec<TDestCh>& dest, bool clrDest,
	const TUtf16BomHandling bomHandling,
	const TUniByteOrder defaultByteOrder) const
{
	IAssert(bomHandling == bomAllowed || bomHandling == bomRequired || bomHandling == bomIgnored);
	IAssert(defaultByteOrder == boMachineEndian || defaultByteOrder == boBigEndian || defaultByteOrder == boLittleEndian);
	if (clrDest) dest.Clr();
	size_t nDecoded = 0;
	if (srcCount <= 0) return nDecoded;
	const size_t origSrcIdx = srcIdx, srcEnd = srcIdx + srcCount;
	bool swap = false;
  bool isMachineLe = IsMachineLittleEndian();
	bool isDefaultLe = (defaultByteOrder == boLittleEndian || (defaultByteOrder == boMachineEndian && isMachineLe));
	if (bomHandling == bomIgnored) swap = (isDefaultLe != isMachineLe);
	else if (bomHandling == bomAllowed || bomHandling == bomRequired)
	{
		int c = uint(src[TVecIdx(srcIdx)]) & 0xffff;
		if (c == 0xfeff) { swap = false; if (skipBom) srcIdx += 1; }
		else if (c == 0xfffe) { swap = true; if (skipBom) srcIdx += 1; }
		else if (bomHandling == bomAllowed) swap = (isMachineLe != isDefaultLe);
		else { // Report an error.
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(srcIdx, c, "BOM expected at the beginning of the input vector (" + TInt::GetStr(c, "%04x") + " found instead).");
			case uehAbort: case uehReplace: case uehIgnore: return size_t(-1);
			default: Fail; } }
	}
	else Fail;
	while (srcIdx < srcEnd)
	{
		const size_t charSrcIdx = srcIdx;
		uint c = uint(src[TVecIdx(srcIdx)]) & 0xffffu; srcIdx++;
		if (swap) c = ((c >> 8) & 0xff) | ((c & 0xff) << 8);
		if (Utf16FirstSurrogate <= c && c <= Utf16FirstSurrogate + 1023)
		{
			// c is the first character in a surrogate pair.  Read the next character.
			if (! (srcIdx < srcEnd)) {
				switch (errorHandling) {
				case uehThrow: throw TUnicodeException(charSrcIdx, c, "The second character of a surrogate pair is missing.");
				case uehAbort: return nDecoded;
				case uehReplace: dest.Add(TDestCh(replacementChar)); continue;
				case uehIgnore: continue;
				default: Fail; } }
			uint c2 = uint(src[TVecIdx(srcIdx)]) & 0xffffu; srcIdx++;
			if (swap) c2 = ((c2 >> 8) & 0xff) | ((c2 & 0xff) << 8);
			// c2 should be the second character of the surrogate pair.
			if (c2 < Utf16SecondSurrogate || Utf16SecondSurrogate + 1023 < c2) {
				switch (errorHandling) {
				case uehThrow: throw TUnicodeException(charSrcIdx + 1, c2, "The second character of a surrogate pair should be in the range " + TInt::GetStr(Utf16SecondSurrogate, "%04x") + ".." + TInt::GetStr(Utf16SecondSurrogate + 1023, "%04x") + ", not " + TInt::GetStr(c2, "04x") + ".");
				case uehAbort: return nDecoded;
				// with uehReplace and uehIgnore, we'll just skip the first character of the surrogate pair; we'll process the second one during the next iteration, this time as an ordinary character
				case uehReplace: dest.Add(TDestCh(replacementChar)); srcIdx -= 1; continue;
				case uehIgnore: srcIdx -= 1; continue;
				default: Fail; } }
			// c and c2 each contain 10 bits of information.
			uint cc = ((c - Utf16FirstSurrogate) << 10) | (c2 - Utf16SecondSurrogate);
			cc += 0x10000;
			dest.Add(TDestCh(cc)); nDecoded++; continue;
		}
		else if (strict && Utf16SecondSurrogate <= c && c <= Utf16SecondSurrogate + 1023) {
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(charSrcIdx, c, "This 16-bit value should be used only as the second character of a surrogate pair.");
			case uehAbort: return nDecoded;
			case uehReplace: dest.Add(TDestCh(replacementChar)); continue;
			case uehIgnore: continue;
			default: Fail; } }
		// If 'c' is the first character in the input stream, and it's a BOM, we might have to skip it.
		if (charSrcIdx == origSrcIdx && (c == 0xfffeu || c == 0xfeffu) && skipBom) continue;
		// Otherwise, store 'c' to the destination vector.
		dest.Add(TDestCh(c)); nDecoded++;
	}
	return nDecoded;
}

//-----------------------------------------------------------------------
// TUniCodec -- UTF-16 Encoder
//-----------------------------------------------------------------------

// Returns the number of characters that have been successfully encoded.
// This does not include any replacement characters that may have been inserted into 'dest'.
template<typename TSrcVec, typename TDestCh>
size_t TUniCodec::EncodeUtf16ToWords(
	const TSrcVec& src, size_t srcIdx, const size_t srcCount,
	TVec<TDestCh>& dest, const bool clrDest, const bool insertBom,
	const TUniByteOrder destByteOrder) const
{
	bool isMachineLe = IsMachineLittleEndian();
	bool swap = (destByteOrder == boLittleEndian && ! isMachineLe) || (destByteOrder == boBigEndian && isMachineLe);
	size_t nEncoded = 0, srcEnd = srcIdx + srcCount;
	if (insertBom) { dest.Add(TDestCh(swap ? 0xfffeu : 0xfeffu)); nEncoded++; }
	while (srcIdx < srcEnd)
	{
		uint c = uint(src[TVecIdx(srcIdx)]); srcIdx++;
		if (! (c <= 0x10ffffu)) {
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(srcIdx - 1, c, "UTF-16 only supports characters in the range 0..10ffff (not " + TUInt::GetStr(c, "%08x") + ").");
			case uehAbort: return nEncoded;
			case uehReplace: dest.Add(TDestCh(swap ? SwapBytes(replacementChar) : replacementChar)); continue;
			case uehIgnore: continue;
			default: Fail; } }
		if (Utf16FirstSurrogate <= c && c < Utf16FirstSurrogate + 1023) {
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(srcIdx - 1, c, "UTF-16 cannot encode " + TUInt::GetStr(c, "%04x") + " as it belongs to the first surrogate range (" + TUInt::GetStr(Utf16FirstSurrogate, "%04x") + ".." + TUInt::GetStr(Utf16FirstSurrogate + 1023, "%04x") + ").");
			case uehAbort: return nEncoded;
			case uehReplace: dest.Add(TDestCh(swap ? SwapBytes(replacementChar) : replacementChar)); continue;
			case uehIgnore: continue;
			default: Fail; } }
		if (Utf16SecondSurrogate <= c && c < Utf16SecondSurrogate + 1023) {
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(srcIdx - 1, c, "The character " + TUInt::GetStr(c, "%04x") + " belongs to the second surrogate range (" + TUInt::GetStr(Utf16FirstSurrogate, "%04x") + ".." + TUInt::GetStr(Utf16FirstSurrogate + 1023, "%04x") + "), which is not allowed with strict == true.");
			case uehAbort: return nEncoded;
			case uehReplace: dest.Add(TDestCh(swap ? SwapBytes(replacementChar) : replacementChar)); continue;
			case uehIgnore: continue;
			default: Fail; } }
		// If c is <= 0xffff, it can be stored directly.
		if (c <= 0xffffu) {
			if (swap) c = ((c >> 8) & 0xff) | ((c & 0xff) << 8);
			dest.Add(TDestCh(c)); nEncoded++; continue; }
		// Otherwise, represent c by a pair of surrogate characters.
		c -= 0x10000u; IAssert(/*0 <= c &&*/ c <= 0xfffffu);
		uint c1 = (c >> 10) & 1023, c2 = c & 1023;
		c1 += Utf16FirstSurrogate; c2 += Utf16SecondSurrogate;
		if (swap) {
			c1 = ((c1 >> 8) & 0xff) | ((c1 & 0xff) << 8);
			c2 = ((c2 >> 8) & 0xff) | ((c2 & 0xff) << 8); }
		dest.Add(TDestCh(c1));
		dest.Add(TDestCh(c2));
		nEncoded++; continue;
	}
	return nEncoded;
}

template<typename TSrcVec, typename TDestCh>
size_t TUniCodec::EncodeUtf16ToBytes(
	const TSrcVec& src, size_t srcIdx, const size_t srcCount,
	TVec<TDestCh>& dest, const bool clrDest, const bool insertBom,
	const TUniByteOrder destByteOrder) const
{
	bool isDestLe = (destByteOrder == boLittleEndian || (destByteOrder == boMachineEndian && IsMachineLittleEndian()));
	size_t nEncoded = 0, srcEnd = srcIdx + srcCount;
	if (insertBom) { dest.Add(isDestLe ? 0xff : 0xfe); dest.Add(isDestLe ? 0xfe : 0xff); nEncoded++; }
	while (srcIdx < srcEnd)
	{
		uint c = uint(src[TVecIdx(srcIdx)]); srcIdx++;
		if (! (c <= 0x10ffffu)) {
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(srcIdx - 1, c, "UTF-16 only supports characters in the range 0..10ffff (not " + TUInt::GetStr(c, "%08x") + ").");
			case uehAbort: return nEncoded;
#define ___OutRepl if (isDestLe) { dest.Add(replacementChar & 0xff); dest.Add((replacementChar >> 8) & 0xff); } else { dest.Add((replacementChar >> 8) & 0xff); dest.Add(replacementChar & 0xff); }
			case uehReplace: ___OutRepl; continue;
			case uehIgnore: continue;
			default: Fail; } }
		if (Utf16FirstSurrogate <= c && c < Utf16FirstSurrogate + 1023) {
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(srcIdx - 1, c, "UTF-16 cannot encode " + TUInt::GetStr(c, "%04x") + " as it belongs to the first surrogate range (" + TUInt::GetStr(Utf16FirstSurrogate, "%04x") + ".." + TUInt::GetStr(Utf16FirstSurrogate + 1023, "%04x") + ").");
			case uehAbort: return nEncoded;
			case uehReplace: ___OutRepl; continue;
			case uehIgnore: continue;
			default: Fail; } }
		if (Utf16SecondSurrogate <= c && c < Utf16SecondSurrogate + 1023) {
			switch (errorHandling) {
			case uehThrow: throw TUnicodeException(srcIdx - 1, c, "The character " + TUInt::GetStr(c, "%04x") + " belongs to the second surrogate range (" + TUInt::GetStr(Utf16FirstSurrogate, "%04x") + ".." + TUInt::GetStr(Utf16FirstSurrogate + 1023, "%04x") + "), which is not allowed with strict == true.");
			case uehAbort: return nEncoded;
			case uehReplace: ___OutRepl; continue;
			case uehIgnore: continue;
			default: Fail; } }
#undef ___OutRepl
		// If c is <= 0xffff, it can be stored directly.
		if (c <= 0xffffu) {
			if (isDestLe) { dest.Add(c & 0xff); dest.Add((c >> 8) & 0xff); }
			else { dest.Add((c >> 8) & 0xff); dest.Add(c & 0xff); }
			nEncoded++; continue; }
		// Otherwise, represent c by a pair of surrogate characters.
		c -= 0x10000u; IAssert(/*0 <= c &&*/ c <= 0xfffffu);
		uint c1 = (c >> 10) & 1023, c2 = c & 1023;
		c1 += Utf16FirstSurrogate; c2 += Utf16SecondSurrogate;
		if (isDestLe) { dest.Add(c1 & 0xff); dest.Add((c1 >> 8) & 0xff); dest.Add(c2 & 0xff); dest.Add((c2 >> 8) & 0xff); }
		else { dest.Add((c1 >> 8) & 0xff); dest.Add(c1 & 0xff); dest.Add((c2 >> 8) & 0xff); dest.Add(c2 & 0xff); }
		nEncoded++; continue;
	}
	return nEncoded;
}

//-----------------------------------------------------------------------------
// TUniChDb -- word boundaries
//-----------------------------------------------------------------------------

template<typename TSrcVec>
bool TUniChDb::FindNextWordBoundary(const TSrcVec& src, const size_t srcIdx, const size_t srcCount, size_t &position) const
{
	// WB1.  Break at the start of text.
	if (position < srcIdx) { position = srcIdx; return true; }
	// If we are beyond the end of the text, there aren't any word breaks left.
	const size_t srcEnd = srcIdx + srcCount;
	if (position >= srcEnd) return false;
	// If 'position' is currently at an ignored character, move it back to the last nonignored character.
	size_t origPos = position;
	if (IsWbIgnored(src[TVecIdx(position)])) {
		if (! WbFindPrevNonIgnored(src, srcIdx, position))
			position = origPos;
	}
	// Determine the previous nonignored character (before 'position').
	size_t posPrev = position;
	if (! WbFindPrevNonIgnored(src, srcIdx, posPrev)) posPrev = position;
	// Sec 6.2.  Allow a break between Sep and an ignored character.
	if (position == origPos && position + 1 < srcEnd && IsSbSep(src[TVecIdx(position)]) && IsWbIgnored(src[TVecIdx(position + 1)])) { position += 1; return true; }
	// Determine the next nonignored character (after 'position').
	size_t posNext = position; WbFindNextNonIgnored(src, posNext, srcEnd);
	size_t posNext2;
	int cPrev = (posPrev < position ? (int) src[TVecIdx(posPrev)] : -1), cCur = (position < srcEnd ? (int) src[TVecIdx(position)] : -1);
	int cNext = (position < posNext && posNext < srcEnd ? (int) src[TVecIdx(posNext)] : -1);
	int wbfPrev = GetWbFlags(cPrev), wbfCur = GetWbFlags(cCur), wbfNext = GetWbFlags(cNext);
	int cNext2, wbfNext2;
	//
	for ( ; position < srcEnd; posPrev = position, position = posNext, posNext = posNext2,
							   cPrev = cCur, cCur = cNext, cNext = cNext2,
							   wbfPrev = wbfCur, wbfCur = wbfNext, wbfNext = wbfNext2)
	{
		// Should there be a word boundary between 'position' and 'posNext' (or, more accurately,
		// between src[posNext - 1] and src[posNext] --- any ignored characters between 'position'
		// and 'posNext' are considered to belong to the previous character ('position'), not to the next one)?
		posNext2 = posNext; WbFindNextNonIgnored(src, posNext2, srcEnd);
		cNext2 = (posNext < posNext2 && posNext2 < srcEnd ? (int) src[TVecIdx(posNext2)] : -1);
		wbfNext2 = GetWbFlags(cNext2);
#define TestCurNext(curFlag, nextFlag) if ((wbfCur & curFlag) == curFlag && (wbfNext & nextFlag) == nextFlag) continue
#define TestCurNext2(curFlag, nextFlag, next2Flag) if ((wbfCur & curFlag) == curFlag && (wbfNext & nextFlag) == nextFlag && (wbfNext2 & next2Flag) == next2Flag) continue
#define TestPrevCurNext(prevFlag, curFlag, nextFlag) if ((wbfPrev & prevFlag) == prevFlag && (wbfCur & curFlag) == curFlag && (wbfNext & nextFlag) == nextFlag) continue
		// WB3.  Do not break within CRLF.
		if (cCur == 13 && cNext == 10) continue;
		// WB5.  Do not break between most letters.
		TestCurNext(ucfWbALetter, ucfWbALetter);
		// WB6.  Do not break letters across certain punctuation.
		TestCurNext2(ucfWbALetter, ucfWbMidLetter, ucfWbALetter);
		// WB7.  Do not break letters across certain punctuation.
		TestPrevCurNext(ucfWbALetter, ucfWbMidLetter, ucfWbALetter);
		// WB8.  Do not break within sequences of digits, or digits adjacent to letters.
		TestCurNext(ucfWbNumeric, ucfWbNumeric);
		// WB9.  Do not break within sequences of digits, or digits adjacent to letters.
		TestCurNext(ucfWbALetter, ucfWbNumeric);
		// WB10.  Do not break within sequences of digits, or digits adjacent to letters.
		TestCurNext(ucfWbNumeric, ucfWbALetter);
		// WB11.  Do not break within sequences, such as "3.2" or "3.456,789".
		TestPrevCurNext(ucfWbNumeric, ucfWbMidNum, ucfWbNumeric);
		// WB12.  Do not break within sequences, such as "3.2" or "3.456,789".
		TestCurNext2(ucfWbNumeric, ucfWbMidNum, ucfWbNumeric);
		// WB13.  Do not break between Katakana.
		TestCurNext(ucfWbKatakana, ucfWbKatakana);
		// WB13a.  Do not break from extenders.
		if ((wbfCur & (ucfWbALetter | ucfWbNumeric | ucfWbKatakana | ucfWbExtendNumLet)) != 0 &&
			(wbfNext & ucfWbExtendNumLet) == ucfWbExtendNumLet) continue;
		// WB13b.  Do not break from extenders.
		if ((wbfCur & ucfWbExtendNumLet) == ucfWbExtendNumLet &&
			(wbfNext & (ucfWbALetter | ucfWbNumeric | ucfWbKatakana)) != 0) continue;
		// WB14.  Otherwise, break everywhere.
		position = posNext; return true;
#undef TestCurNext
#undef TestCurNext2
#undef TestPrevCurNext
	}
	// WB2.  Break at the end of text.
	IAssert(position == srcEnd);
	return true;
}

// ToDo: provide a more efficient implementation of this.
template<typename TSrcVec>
void TUniChDb::FindWordBoundaries(const TSrcVec& src, const size_t srcIdx, const size_t srcCount, TBoolV& dest) const
{
	if (size_t(dest.Len()) != srcCount + 1) dest.Gen(TVecIdx(srcCount + 1));
	dest.PutAll(false);
	size_t position = srcIdx;
	dest[TVecIdx(position - srcIdx)] = true;
	while (position < srcIdx + srcCount)
	{
		size_t oldPos = position;
		FindNextWordBoundary(src, srcIdx, srcCount, position);
		Assert(oldPos < position); Assert(position <= srcIdx + srcCount);
		dest[TVecIdx(position - srcIdx)] = true;
	}
	Assert(dest[TVecIdx(srcCount)]);
}

//-----------------------------------------------------------------------------
// TUniChDb -- sentence boundaries
//-----------------------------------------------------------------------------

template<typename TSrcVec>
bool TUniChDb::CanSentenceEndHere(const TSrcVec& src, const size_t srcIdx, const size_t position) const
{
	if (sbExTrie.Empty()) return true;
	// We'll move back from the position where a sentence-boundary is being considered.
	size_t pos = position;
	if (! WbFindPrevNonIgnored(src, srcIdx, pos)) return true;
	int c = (int) src[TVecIdx(pos)]; int sfb = GetSbFlags(c);
	// - Skip the Sep, if there is one.
	if ((c & ucfSbSep) == ucfSbSep) {
		if (! WbFindPrevNonIgnored(src, srcIdx, pos)) return true;
		c = (int) src[TVecIdx(pos)]; sfb = GetSbFlags(c); }
	// - Skip any Sp characters.
	while ((sfb & ucfSbSp) == ucfSbSp) {
		if (! WbFindPrevNonIgnored(src, srcIdx, pos)) return true;
		c = (int) src[TVecIdx(pos)]; sfb = GetSbFlags(c); }
	// - Skip any Close characters.
	while ((sfb & ucfSbSp) == ucfSbSp) {
		if (! WbFindPrevNonIgnored(src, srcIdx, pos)) return true;
		c = (int) src[TVecIdx(pos)]; sfb = GetSbFlags(c); }
	// - Skip any ATerm | STerm characters.
	while ((sfb & (ucfSbATerm | ucfSbSTerm)) != 0) {
		if (! WbFindPrevNonIgnored(src, srcIdx, pos)) return true;
		c = (int) src[TVecIdx(pos)]; sfb = GetSbFlags(c); }
	// Now start moving through the trie.
	int cLast = c, cButLast = -1, cButButLast = -1, len = 1, node = -1;
	while (true)
	{
		bool atEnd = (! WbFindPrevNonIgnored(src, srcIdx, pos));
		c = (atEnd ? -1 : (int) src[TVecIdx(pos)]);
		TUniChCategory cat = GetCat(c);
		if (atEnd || ! (cat == ucLetter || cat == ucNumber || cat == ucSymbol)) {
			// Check if the suffix we've read so far is one of those that appear in the trie.
			if (len == 1) return ! sbExTrie.Has1Gram(cLast);
			if (len == 2) return ! sbExTrie.Has2Gram(cLast, cButLast);
			IAssert(len >= 3); IAssert(node >= 0);
			if (sbExTrie.IsNodeTerminal(node)) return false;
			if (atEnd) return true; }
		if (len == 1) { cButLast = c; len++; }
		else if (len == 2) { cButButLast = c; len++;
			// Now we have read the last three characters; start descending the suitable subtrie.
			node = sbExTrie.Get3GramRoot(cLast, cButLast, cButButLast);
			if (node < 0) return true; }
		else {
			// Descend down the trie.
			node = sbExTrie.GetChild(node, c);
			if (node < 0) return true; }
	}
	//return true;
}

template<typename TSrcVec>
bool TUniChDb::FindNextSentenceBoundary(const TSrcVec& src, const size_t srcIdx, const size_t srcCount, size_t &position) const
{
	// SB1.  Break at the start of text.
	if (position < srcIdx) { position = srcIdx; return true; }
	// If we are beyond the end of the text, there aren't any word breaks left.
	const size_t srcEnd = srcIdx + srcCount;
	if (position >= srcEnd) return false;
	// If 'position' is currently at an ignored character, move it back to the last nonignored character.
	size_t origPos = position;
	if (IsWbIgnored(src[TVecIdx(position)])) {
		if (! WbFindPrevNonIgnored(src, srcIdx, position))
			position = origPos;
	}
	// Determine the previous nonignored character (before 'position').
	size_t posPrev = position;
	if (! WbFindPrevNonIgnored(src, srcIdx, posPrev)) posPrev = position;
	// Sec 6.2.  Allow a break between Sep and an ignored character.
	if (position == origPos && position + 1 < srcEnd && IsSbSep(src[TVecIdx(position)]) && IsWbIgnored(src[TVecIdx(position + 1)])) { position += 1; return true; }
	// Determine the next nonignored character (after 'position').
	size_t posNext = position; WbFindNextNonIgnored(src, posNext, srcEnd);
	size_t posNext2;
	int cPrev = (posPrev < position ? (int) src[TVecIdx(posPrev)] : -1), cCur = (position < srcEnd ? (int) src[TVecIdx(position)] : -1);
	int cNext = (position < posNext && posNext < srcEnd ? (int) src[TVecIdx(posNext)] : -1);
	int sbfPrev = GetSbFlags(cPrev), sbfCur = GetSbFlags(cCur), sbfNext = GetSbFlags(cNext);
	int cNext2, sbfNext2;
	// Initialize the state of the peek-back automaton.
	typedef enum { stInit, stATerm, stATermSp, stATermSep, stSTerm, stSTermSp, stSTermSep } TPeekBackState;
	TPeekBackState backState;
	{
		size_t pos = position;
		bool wasSep = false, wasSp = false, wasATerm = false, wasSTerm = false;
		while (true)
		{
			if (! WbFindPrevNonIgnored(src, srcIdx, pos)) break;
			// Skip at most one Sep.
			int cp = (int) src[TVecIdx(pos)]; int sbf = GetSbFlags(cp);
			if ((sbf & ucfSbSep) == ucfSbSep) {
				wasSep = true;
				if (! WbFindPrevNonIgnored(src, srcIdx, pos)) break;
				cp = (int) src[TVecIdx(pos)]; sbf = GetSbFlags(cp); }
			// Skip zero or more Sp's.
			bool stop = false;
			while ((sbf & ucfSbSp) == ucfSbSp) {
				wasSp = true;
				if (! WbFindPrevNonIgnored(src, srcIdx, pos)) { stop = true; break; }
				cp = (int) src[TVecIdx(pos)]; sbf = GetSbFlags(cp); }
			if (stop) break;
			// Skip zero or more Close's.
			while ((sbf & ucfSbClose) == ucfSbClose) {
				if (! WbFindPrevNonIgnored(src, srcIdx, pos)) { stop = true; break; }
				cp = (int) src[TVecIdx(pos)]; sbf = GetSbFlags(cp); }
			if (stop) break;
			// Process an ATerm or STerm.
			wasATerm = ((sbf & ucfSbATerm) == ucfSbATerm);
			wasSTerm = ((sbf & ucfSbSTerm) == ucfSbSTerm);
			break;
		}
		if (wasATerm) backState = (wasSep ? stATermSep : wasSp ? stATermSp : stATerm);
		else if (wasSTerm) backState = (wasSep ? stSTermSep : wasSp ? stSTermSp : stSTerm);
		else backState = stInit;
	}
	// Initialize the state of the peek-ahead automaton.  This state tells us what follows
	// after we skip all contiguous characters from the complement of the set {OLetter, Upper, Lower, Sep, STerm, ATerm}.
	// Thus, the next character is either OLetter, Upper, Lower, Sep, STerm, ATerm, or the end of the input string.
	// Our peek-ahead automaton must tell us whether it is Lower or something else.
	typedef enum { stUnknown, stLower, stNotLower } TPeekAheadState;
	TPeekAheadState aheadState = stUnknown;
	//
	for ( ; position < srcEnd; posPrev = position, position = posNext, posNext = posNext2,
							   cPrev = cCur, cCur = cNext, cNext = cNext2,
							   sbfPrev = sbfCur, sbfCur = sbfNext, sbfNext = sbfNext2)
	{
		// Should there be a word boundary between 'position' and 'posNext' (or, more accurately,
		// between src[posNext - 1] and src[posNext] --- any ignored characters between 'position'
		// and 'posNext' are considered to belong to the previous character ('position'), not to the next one)?
		posNext2 = posNext; WbFindNextNonIgnored(src, posNext2, srcEnd);
		cNext2 = (posNext < posNext2 && posNext2 < srcEnd ? (int) src[TVecIdx(posNext2)] : -1);
		sbfNext2 = GetSbFlags(cNext2);
		// Update the peek-back automaton.
#define TestCur(curFlag) ((sbfCur & ucfSb##curFlag) == ucfSb##curFlag)
#define Trans(curFlag, newState) if (TestCur(curFlag)) { backState = st##newState; break; }
		switch (backState) {
			case stInit: Trans(ATerm, ATerm); Trans(STerm, STerm); break;
			case stATerm: Trans(Sp, ATermSp); Trans(Sep, ATermSep); Trans(ATerm, ATerm); Trans(STerm, STerm); Trans(Close, ATerm); backState = stInit; break;
			case stSTerm: Trans(Sp, STermSp); Trans(Sep, STermSep); Trans(ATerm, ATerm); Trans(STerm, STerm); Trans(Close, STerm); backState = stInit; break;
			case stATermSp: Trans(Sp, ATermSp); Trans(Sep, ATermSep); Trans(ATerm, ATerm); Trans(STerm, STerm); backState = stInit; break;
			case stSTermSp: Trans(Sp, STermSp); Trans(Sep, STermSep); Trans(ATerm, ATerm); Trans(STerm, STerm); backState = stInit; break;
			case stATermSep: Trans(ATerm, ATerm); Trans(STerm, STerm); backState = stInit; break;
			case stSTermSep: Trans(ATerm, ATerm); Trans(STerm, STerm); backState = stInit; break;
			default: IAssert(false); }
#undef Trans
#undef TestCur
		// Update the peek-ahead automaton.
#define IsPeekAheadSkippable(sbf) ((sbf & (ucfSbOLetter | ucfSbUpper | ucfSbLower | ucfSbSep | ucfSbSTerm | ucfSbATerm)) == 0)
		if (! IsPeekAheadSkippable(sbfCur)) {
			bool isLower = ((sbfCur & ucfSbLower) == ucfSbLower);
			if (aheadState == stLower) IAssert(isLower);
			else if (aheadState == stNotLower) IAssert(! isLower);
			// We haven't peaked ahead farther than this so far -- invalidate the state.
			aheadState = stUnknown; }
		if (aheadState == stUnknown)
		{
			// Peak ahead to the next non-peekahead-skippable character.
			size_t pos = posNext;
			while (pos < srcEnd) {
				int cp = (int) src[TVecIdx(pos)]; int sbf = GetSbFlags(cp);
				if (! IsPeekAheadSkippable(sbf)) {
					if ((sbf & ucfSbLower) == ucfSbLower) aheadState = stLower;
					else aheadState = stNotLower;
					break; }
				WbFindNextNonIgnored(src, pos, srcEnd); }
			if (! (pos < srcEnd)) aheadState = stNotLower;
		}
#undef IsPeekAheadSkippable
		//
#define TestCurNext(curFlag, nextFlag) if ((sbfCur & curFlag) == curFlag && (sbfNext & nextFlag) == nextFlag) continue
#define TestCurNext2(curFlag, nextFlag, next2Flag) if ((sbfCur & curFlag) == curFlag && (sbfNext & nextFlag) == nextFlag && (sbfNext2 & next2Flag) == next2Flag) continue
#define TestPrevCurNext(prevFlag, curFlag, nextFlag) if ((sbfPrev & prevFlag) == prevFlag && (sbfCur & curFlag) == curFlag && (sbfNext & nextFlag) == nextFlag) continue
		// SB3.  Do not break within CRLF.
		if (cCur == 13 && cNext == 10) continue;
		// SB4.  Break ater paragraph separators.
		if ((sbfCur & ucfSbSep) == ucfSbSep) {
			if (! CanSentenceEndHere(src, srcIdx, position)) continue;
			position = posNext; return true; }
		// Do not break after ambiguous terminators like period, if they are immediately followed by a number
		// or lowercase letter, if they are between uppercase letters, or if the first following letter
		// (optionally after certain punctuation) is lowercase.  For example, a period may be an abbreviation
		// or numeric period, and thus may not mark the end of a sentence.
		TestCurNext(ucfSbATerm, ucfSbNumeric); // SB6
		TestPrevCurNext(ucfSbUpper, ucfSbATerm, ucfSbUpper); // SB7
		// SB8a.  (STerm | ATerm) Close* Sp* [do not break] (STerm | ATerm)
		if ((backState == stATerm || backState == stATermSp || backState == stSTerm || backState == stSTermSp) &&
			(sbfNext & (ucfSbSTerm | ucfSbATerm)) != 0) continue;
		// SB8*.  ATerm Close* Sp* [do not break] ( ! (OLetter | Upper | Lower | Sep | STerm | ATerm) )* Lower
		if ((backState == stATerm || backState == stATermSp) && aheadState == stLower) continue;
		// Break after sentence terminators, but include closing punctuation, trailing spaces, and a paragraph separator (if present).
		// SB9. ( STerm | ATerm ) Close* [do not break] ( Close | Sp | Sep )
		if ((backState == stATerm || backState == stSTerm) && (sbfNext & (ucfSbClose | ucfSbSp | ucfSbSep)) != 0) continue;
		// SB10. ( STerm | ATerm ) Close* Sp* [do not break] ( Sp | Sep )
		// SB11*. ( STerm | ATerm ) Close* Sp* Sep? [do break]
		if (backState == stATerm || backState == stATermSp || backState == stATermSep || backState == stSTerm || backState == stSTermSp || backState == stSTermSep) {
			if ((sbfNext & (ucfSbSp | ucfSbSep)) != 0) continue; // SB10
			if (! CanSentenceEndHere(src, srcIdx, position)) continue;
			position = posNext; return true; } // SB11
		// WB12.  Otherwise, do not break.
		continue;
#undef TestCurNext
#undef TestCurNext2
#undef TestPrevCurNext
	}
	// WB2.  Break at the end of text.
	IAssert(position == srcEnd);
	return true;
}

// ToDo: provide a more efficient implementation of this.
template<typename TSrcVec>
void TUniChDb::FindSentenceBoundaries(const TSrcVec& src, const size_t srcIdx, const size_t srcCount, TBoolV& dest) const
{
	if (size_t(dest.Len()) != srcCount + 1) dest.Gen(TVecIdx(srcCount + 1));
	dest.PutAll(false);
	size_t position = srcIdx;
	dest[TVecIdx(position - srcIdx)] = true;
	while (position < srcIdx + srcCount)
	{
		size_t oldPos = position;
		FindNextSentenceBoundary(src, srcIdx, srcCount, position);
		Assert(oldPos < position); Assert(position <= srcIdx + srcCount);
		dest[TVecIdx(position - srcIdx)] = true;
	}
	Assert(dest[TVecIdx(srcCount)]);
}

//-----------------------------------------------------------------------------
// TUniChDb -- case conversions
//-----------------------------------------------------------------------------

template<typename TSrcVec, typename TDestCh>
void TUniChDb::GetCaseConverted(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
								TVec<TDestCh>& dest, const bool clrDest,
								const TUniChDb::TCaseConversion how,
								const bool turkic, const bool lithuanian) const
{
	const TIntIntVH &specials = (how == ccUpper ? specialCasingUpper : how == ccLower ? specialCasingLower : how == ccTitle ? specialCasingTitle : *((TIntIntVH *) 0));
	if (clrDest) dest.Clr();
	enum {
		GreekCapitalLetterSigma = 0x3a3,
		GreekSmallLetterSigma = 0x3c3,
		GreekSmallLetterFinalSigma = 0x3c2,
		LatinCapitalLetterI = 0x49,
		LatinCapitalLetterJ = 0x4a,
		LatinCapitalLetterIWithOgonek = 0x12e,
		LatinCapitalLetterIWithGrave = 0xcc,
		LatinCapitalLetterIWithAcute = 0xcd,
		LatinCapitalLetterIWithTilde = 0x128,
		LatinCapitalLetterIWithDotAbove = 0x130,
		LatinSmallLetterI = 0x69,
		CombiningDotAbove = 0x307
	};
	//
	bool seenCased = false, seenTwoCased = false; int cpFirstCased = -1;
	size_t nextWordBoundary = srcIdx;
	TBoolV wordBoundaries; bool wbsKnown = false;
	for (const size_t origSrcIdx = srcIdx, srcEnd = srcIdx + srcCount; srcIdx < srcEnd; )
	{
		int cp = src[TVecIdx(srcIdx)]; srcIdx++;
		//if (turkic && cp == 0x130 && how == ccLower) printf("!");
		// For conversion to titlecase, the first cased character of each word
		// must be converted to titlecase; everything else must be converted
		// to lowercase.
		TUniChDb::TCaseConversion howHere;
		if (how != ccTitle) howHere = how;
		else {
			if (srcIdx - 1 == nextWordBoundary) { // A word starts/ends here.
				seenCased = false; seenTwoCased = false; cpFirstCased = -1;
				size_t next = nextWordBoundary; FindNextWordBoundary(src, origSrcIdx, srcCount, next);
				IAssert(next > nextWordBoundary); nextWordBoundary = next; }
			bool isCased = IsCased(cp);
			if (isCased && ! seenCased) { howHere = ccTitle; seenCased = true; cpFirstCased = cp; }
			else { howHere = ccLower;
				if (isCased && seenCased) seenTwoCased = true; }
		}
		// First, process the conditional mappings from SpecialCasing.txt.
		// These will be processed in code -- they were ignored while
		// we were reading SpecialCasing.txt itself.
		if (cp == GreekCapitalLetterSigma && howHere == ccLower)
		{
			// SpecialCasing.txt mentions the 'FinalSigma' condition, but sec. 3.13 of
			// the standard doesn't define it.  We'll use FinalCased instead.
			// FinalCased: within the closest word boundaries containing C,
			// there is a cased letter before C, and there is no cased letter after C.
			//size_t nextBoundary = srcIdx - 1; FindNextWordBoundary(src, srcIdx, srcCount, nextBoundary);
			if (! wbsKnown) { FindWordBoundaries(src, origSrcIdx, srcCount, wordBoundaries); wbsKnown = true; }
			size_t srcIdx2 = srcIdx; bool casedAfter = false;
			if (how == ccTitle)
				printf("!");
			//while (srcIdx2 < nextBoundary)
			while (! wordBoundaries[TVecIdx(srcIdx2 - origSrcIdx)])
			{
				int cp2 = src[TVecIdx(srcIdx2)]; srcIdx2++;
				if (IsCased(cp2)) { casedAfter = true; break; }
			}
			if (! casedAfter)
			{
				//size_t prevBoundary = srcIdx - 1;
				//FindPreviousWordBoundary(src, srcIdx, srcCount, prevBoundary);
				srcIdx2 = srcIdx - 1; bool casedBefore = false;
				//while (prevBoundary < srcIdx2)
				while (! wordBoundaries[TVecIdx(srcIdx2 - origSrcIdx)])
				{
					--srcIdx2; int cp2 = src[TVecIdx(srcIdx2)];
					if (IsCased(cp2)) { casedBefore = true; break; }
				}
				if (casedBefore) {
					// Now we have a FinalCased character.
					dest.Add(GreekSmallLetterFinalSigma); Assert(howHere == ccLower); continue; }
			}
			// If we got here, add a non-final sigma.
			dest.Add(GreekSmallLetterSigma); continue;
		}
		else if (lithuanian)
		{
			if (howHere == ccLower)
			{
				if (cp == LatinCapitalLetterI || cp == LatinCapitalLetterJ || cp == LatinCapitalLetterIWithOgonek)
				{
					bool moreAbove = false;
					for (size_t srcIdx2 = srcIdx; srcIdx2 < srcEnd; )
					{
						const int cp2 = src[TVecIdx(srcIdx2)]; srcIdx2++;
						const int cc2 = GetCombiningClass(cp2);
						if (cc2 == TUniChInfo::ccStarter) break;
						if (cc2 == TUniChInfo::ccAbove) { moreAbove = true; break; }
					}
					if (moreAbove)
					{
						if (cp == LatinCapitalLetterI) { dest.Add(0x69); dest.Add(0x307); continue; }
						if (cp == LatinCapitalLetterJ) { dest.Add(0x6a); dest.Add(0x307); continue; }
						if (cp == LatinCapitalLetterIWithOgonek) { dest.Add(0x12f); dest.Add(0x307); continue; }
					}
				}
				else if (cp == LatinCapitalLetterIWithGrave) { dest.Add(0x69); dest.Add(0x307); dest.Add(0x300); continue; }
				else if (cp == LatinCapitalLetterIWithAcute) { dest.Add(0x69); dest.Add(0x307); dest.Add(0x301); continue; }
				else if (cp == LatinCapitalLetterIWithTilde) { dest.Add(0x69); dest.Add(0x307); dest.Add(0x303); continue; }
			}
			if (cp == CombiningDotAbove)
			{
				// Lithuanian, howHere != ccLower.
				// AfterSoftDotted := the last preceding character with a combining class
				// of zero before C was Soft_Dotted, and there is no intervening combining
				// character class 230 (ABOVE).
				bool afterSoftDotted = false;
				size_t srcIdx2 = srcIdx - 1; // now srcIdx2 is the index from which we got 'cp'
				while (origSrcIdx < srcIdx2)
				{
					--srcIdx2; int cp2 = src[TVecIdx(srcIdx2)];
					int cc2 = GetCombiningClass(cp2);
					if (cc2 == TUniChInfo::ccAbove) break;
					if (cc2 == TUniChInfo::ccStarter) {
						afterSoftDotted = IsSoftDotted(cp2); break; }
				}
				if (afterSoftDotted)
				{
					Assert(lithuanian);
					// Remove DOT ABOVE after "i" with upper or titlecase.
					// - Note: but this must only be done if that "i" was actually placed into uppercase (if how == ccTitle,
					//   the "i" may have been kept lowercase and thus we shouldn't remove the dot).
					if (how == ccLower) { dest.Add(0x307); continue; }
					if (how == ccUpper) continue;
					Assert(how == ccTitle);
					Assert(howHere == ccLower); // because CombiningDotAbove is not a cased character
					if (seenCased && ! seenTwoCased) continue; // The "i" has been placed into uppercase; thus, remove the dot.
					dest.Add(0x307); continue;
				}
			}
		}
		else if (turkic) // language code 'tr' (Turkish) and 'az' (Azeri)
		{
			// I and i-dotless; I-dot and i are case pairs in Turkish and Azeri
			// The following rules handle those cases.
			if (cp == LatinCapitalLetterIWithDotAbove) {
				dest.Add(howHere == ccLower ? 0x69 : 0x130); continue; }
			// When lowercasing, remove dot_above in the sequence I + dot_above,
			// which will turn into i.  This matches the behavior of the
			// canonically equivalent I-dot_above.
			else if (cp == CombiningDotAbove)
			{
				// AfterI: the last preceding base character was an uppercase I,
				// and there is no intervening combining character class 230 (ABOVE).
				bool afterI = false;
				size_t srcIdx2 = srcIdx - 1; // now srcIdx2 is the index from which we got 'cp'
				while (origSrcIdx < srcIdx2)
				{
					--srcIdx2; int cp2 = src[TVecIdx(srcIdx2)];
					if (cp2 == LatinCapitalLetterI) { afterI = true; break; }
					int cc2 = GetCombiningClass(cp2);
					if (cc2 == TUniChInfo::ccAbove || cc2 == TUniChInfo::ccStarter) break;
				}
				if (afterI) {
					if (how == ccTitle && seenCased && ! seenTwoCased) {
						// Sec. 3.13 defines title-casing in an unusual way: find the first cased character in each word;
						// if found, map it to titlecase; otherwise, map all characters in that word to lowercase.
						// This suggests that if a cased character is found, others in that word should be left alone.
						// This seems unusual; we map all other characters to lowercase instead.
						// But this leads to problems with e.g. I followed by dot-above (U+0307): since the dot-above
						// is not the first cased character (it isn't even cased), we attempt to set it to lowercase;
						// but since afterI is also true here, this would mean deleting it.  Thus our titlecased
						// form of "I followed by dot-above" would be just "I", which is clearly wrong.
						// So we treat this as a special case here.
						IAssert(cpFirstCased == LatinCapitalLetterI);
						dest.Add(0x307); continue; }
					if (howHere != ccLower) dest.Add(0x307);
					continue; }
			}
			// When lowercasing, unless an I is before a dot_above,
			// it turns into a dotless i.
			else if (cp == LatinCapitalLetterI)
			{
				// BeforeDot: C is followed by U+0307 (combining dot above).
				// Any sequence of characters with a combining class that is
				// neither 0 nor 230 may intervene between the current character
				// and the combining dot above.
				bool beforeDot = false;
				for (size_t srcIdx2 = srcIdx; srcIdx2 < srcEnd; )
				{
					const int cp2 = src[TVecIdx(srcIdx2)]; srcIdx2++;
					if (cp2 == 0x307) { beforeDot = true; break; }
					const int cc2 = GetCombiningClass(cp2);
					if (cc2 == TUniChInfo::ccStarter || cc2 == TUniChInfo::ccAbove) break;
				}
				if (! beforeDot) {
					dest.Add(howHere == ccLower ? 0x131 : 0x49); continue; }
			}
			// When uppercasing, i turns into a dotted capital I.
			else if (cp == LatinSmallLetterI)
			{
				dest.Add(howHere == ccLower ? 0x69 : 0x130); continue;
			}
		}
		// Try to use the unconditional mappings.
		const TIntIntVH &specHere = (
			howHere == how ? specials :
			howHere == ccLower ? specialCasingLower :
			howHere == ccTitle ? specialCasingTitle :
			howHere == ccUpper ? specialCasingUpper : *((TIntIntVH *) 0));
		int i = specHere.GetKeyId(cp);
		if (i >= 0) { TUniCaseFolding::AppendVector(specHere[i], dest); continue; }
		// Try to use the simple (one-character) mappings.
		i = h.GetKeyId(cp);
		if (i >= 0) {
			const TUniChInfo &ci = h[i];
			int cpNew = (
				howHere == ccLower ? ci.simpleLowerCaseMapping :
				howHere == ccUpper ? ci.simpleUpperCaseMapping :
									 ci.simpleTitleCaseMapping);
			if (cpNew < 0) cpNew = cp;
			dest.Add(cpNew); continue; }
		// As a final resort, leave 'cp' unchanged.
		dest.Add(cp);
	}
}

template<typename TSrcVec, typename TDestCh>
void TUniChDb::GetSimpleCaseConverted(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
	TVec<TDestCh>& dest, const bool clrDest, const TCaseConversion how) const
{
	if (clrDest) dest.Clr();
	bool seenCased = false; size_t nextWordBoundary = srcIdx;
	for (const size_t origSrcIdx = srcIdx, srcEnd = srcIdx + srcCount; srcIdx < srcEnd; )
	{
		const int cp = src[TVecIdx(srcIdx)]; srcIdx++;
		int i = h.GetKeyId(cp); if (i < 0) { dest.Add(cp); continue; }
		const TUniChInfo &ci = h[i];
		// With titlecasing, the first cased character of each word must be put into titlecase,
		// all others into lowercase.  This is what the howHere variable is for.
		TUniChDb::TCaseConversion howHere;
		if (how != ccTitle) howHere = how;
		else {
			if (srcIdx - 1 == nextWordBoundary) { // A word starts/ends here.
				seenCased = false;
				size_t next = nextWordBoundary; FindNextWordBoundary(src, origSrcIdx, srcCount, next);
				IAssert(next > nextWordBoundary); nextWordBoundary = next; }
			bool isCased = IsCased(cp);
			if (isCased && ! seenCased) { howHere = ccTitle; seenCased = true; }
			else howHere = ccLower;
		}
		int cpNew = (howHere == ccTitle ? ci.simpleTitleCaseMapping : howHere == ccUpper ? ci.simpleUpperCaseMapping : ci.simpleLowerCaseMapping);
		if (cpNew < 0) cpNew = cp;
		dest.Add(cpNew);
	}
}

template<typename TSrcVec>
void TUniChDb::ToSimpleCaseConverted(TSrcVec& src, size_t srcIdx, const size_t srcCount, const TCaseConversion how) const
{
	bool seenCased = false; size_t nextWordBoundary = srcIdx;
	for (const size_t origSrcIdx = srcIdx, srcEnd = srcIdx + srcCount; srcIdx < srcEnd; srcIdx++)
	{
		const int cp = src[TVecIdx(srcIdx)];
		int i = h.GetKeyId(cp); if (i < 0) continue;
		const TUniChInfo &ci = h[i];
		// With titlecasing, the first cased character of each word must be put into titlecase,
		// all others into lowercase.  This is what the howHere variable is for.
		TUniChDb::TCaseConversion howHere;
		if (how != ccTitle) howHere = how;
		else {
			if (srcIdx == nextWordBoundary) { // A word starts/ends here.
				seenCased = false;
				size_t next = nextWordBoundary; FindNextWordBoundary(src, origSrcIdx, srcCount, next);
				IAssert(next > nextWordBoundary); nextWordBoundary = next; }
			bool isCased = IsCased(cp);
			if (isCased && ! seenCased) { howHere = ccTitle; seenCased = true; }
			else howHere = ccLower;
		}
		int cpNew = (howHere == ccTitle ? ci.simpleTitleCaseMapping : howHere == ccUpper ? ci.simpleUpperCaseMapping : ci.simpleLowerCaseMapping);
		if (cpNew >= 0) src[TVecIdx(srcIdx)] = cpNew;
	}
}

//-----------------------------------------------------------------------------
// TUniChDb -- composition, decomposition, normal forms
//-----------------------------------------------------------------------------

template<typename TDestCh>
void TUniChDb::AddDecomposition(const int codePoint, TVec<TDestCh>& dest, const bool compatibility) const
{
	if (HangulSBase <= codePoint && codePoint < HangulSBase + HangulSCount)
	{
		// UAX #15, sec. 16: Hangul decomposition
		const int SIndex = codePoint - HangulSBase;
		const int L = HangulLBase + SIndex / HangulNCount;
		const int V = HangulVBase + (SIndex % HangulNCount) / HangulTCount;
		const int T = HangulTBase + (SIndex % HangulTCount);
		dest.Add(L); dest.Add(V);
		if (T != HangulTBase) dest.Add(T);
		return;
	}
	int i = h.GetKeyId(codePoint); if (i < 0) { dest.Add(codePoint); return; }
	const TUniChInfo &ci = h[i];
	int ofs = ci.decompOffset; if (ofs < 0) { dest.Add(codePoint); return; }
	if ((! compatibility) && ci.IsCompatibilityDecomposition()) { dest.Add(codePoint); return; }
	while (true) {
		int cp = decompositions[ofs++]; if (cp < 0) return;
		AddDecomposition(cp, dest, compatibility); }
}

template<typename TSrcVec, typename TDestCh>
void TUniChDb::Decompose(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, const bool compatibility, bool clrDest) const
{
	if (clrDest) dest.Clr();
	const size_t destStart = dest.Len()/*, srcEnd = srcIdx + srcCount*/;
	// Decompose the string.
	while (srcIdx < srcCount) {
		AddDecomposition(src[TVecIdx(srcIdx)], dest, compatibility); srcIdx++; }
	// Rearrange the decomposed string into canonical order.
	for (size_t destIdx = destStart, destEnd = dest.Len(); destIdx < destEnd; )
	{
		size_t j = destIdx;
		int cp = dest[TVecIdx(destIdx)]; destIdx++;
		int cpCls = GetCombiningClass(cp);
		if (cpCls == TUniChInfo::ccStarter) continue;
		while (destStart < j && GetCombiningClass(dest[TVecIdx(j - 1)]) > cpCls) {
			dest[TVecIdx(j)] = dest[TVecIdx(j - 1)]; j--; }
		dest[TVecIdx(j)] = cp;
	}
}

template<typename TSrcVec, typename TDestCh>
void TUniChDb::DecomposeAndCompose(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, bool compatibility, bool clrDest) const
{
	if (clrDest) dest.Clr();
	TIntV temp;
	Decompose(src, srcIdx, srcCount, temp, compatibility);
	Compose(temp, 0, temp.Len(), dest, clrDest);
}

template<typename TSrcVec, typename TDestCh>
void TUniChDb::Compose(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, bool clrDest) const
{
	if (clrDest) dest.Clr();
	bool lastStarterKnown = false; // has a starter been encountered yet?
	size_t lastStarterPos = size_t(-1);  // the index (in 'dest') of the last starter
	int cpLastStarter = -1; // the codepoint of the last starter (i.e. cpLastStarter == dest[lastStarterPos])
	const size_t srcEnd = srcIdx + srcCount;
	int ccMax = -1; // The highest combining class among the characters since the last starter.
	while (srcIdx < srcEnd)
	{
		const int cp = src[TVecIdx(srcIdx)]; srcIdx++;
		const int cpClass = GetCombiningClass(cp);
		//int cpCombined = -1;
		// If there is a starter with which 'cp' can be combined, and from which it is not blocked
		// by some intermediate character, we can try to combine them.
		if (lastStarterKnown && ccMax < cpClass)
		{
			int j = inverseDec.GetKeyId(TIntPr(cpLastStarter, cp));
			int cpCombined = -1;
			do {
				// Try to look up a composition in the inverseDec table.
				if (j >= 0) { cpCombined = inverseDec[j]; break; }
				// UAX #15, sec. 16: Hangul composition
				// - Try to combine L and V.
				const int LIndex = cpLastStarter - HangulLBase;
				if (0 <= LIndex && LIndex < HangulLCount) {
					const int VIndex = cp - HangulVBase;
					if (0 <= VIndex && VIndex < HangulVCount) {
						cpCombined = HangulSBase + (LIndex * HangulVCount + VIndex) * HangulTCount;
						break; } }
				// - Try to combine LV and T.
				const int SIndex = cpLastStarter - HangulSBase;
				if (0 <= SIndex && SIndex < HangulSCount && (SIndex % HangulTCount) == 0)
				{
					const int TIndex = cp - HangulTBase;
					if (0 <= TIndex && TIndex < HangulTCount) {
						cpCombined = cpLastStarter + TIndex;
						break; }
				}
			} while (false);
			// If a combining character has been found, use it to replace the old cpStarter.
			if (cpCombined >= 0) {
				dest[TVecIdx(lastStarterPos)] = cpCombined;
				Assert(GetCombiningClass(cpCombined) == TUniChInfo::ccStarter);
				// if (cpCombined is not a starter) { starterKnown = false; lastStarterPos = size_t(01); cpLastStarter = -1; } else
				cpLastStarter = cpCombined; continue; }
		}
		if (cpClass == TUniChInfo::ccStarter) { // 'cp' is a starter, remember it for later.  Set ccMax to -1 so that this starter can be combined with another starter.
			lastStarterKnown = true; lastStarterPos = dest.Len(); cpLastStarter = cp; ccMax = cpClass - 1; }
		else if (cpClass > ccMax) // Remember cp's class as the new maximum class since the last starter (for blocking).
			ccMax = cpClass;
		dest.Add(cp);
	}
}

template<typename TSrcVec, typename TDestCh>
size_t TUniChDb::ExtractStarters(const TSrcVec& src, size_t srcIdx, const size_t srcCount,
		TVec<TDestCh>& dest, bool clrDest) const
{
	if (clrDest) dest.Clr();
	size_t retVal = 0;
	for (const size_t srcEnd = srcIdx + srcCount; srcIdx < srcEnd; srcIdx++) {
		const int cp = src[TVecIdx(srcIdx)];
		if (GetCombiningClass(cp) == TUniChInfo::ccStarter)
			{ dest.Add(cp); retVal++; } }
	return retVal;
}

inline bool AlwaysFalse()
{
	int sum = 0;
	for (int i = 0; i < 5; i++) sum += i;
	return sum > 100;
}

inline bool AlwaysTrue()
{
	int sum = 0;
	for (int i = 0; i < 5; i++) sum += i;
	return sum < 100;
}

/*

Notes on decomposition:

- In UnicodeData.txt, there is a field with the decomposition mapping.
  This field may also include a tag, <...>.
  If there is a tag, this is a compatibility mapping.
  Otherwise it is a canonical mapping.
- Canonical decomposition uses only canonical mappings,
  compatibility decomposition uses both canonical and compatibility mappings.
- Decomposition:
  1. Apply the decomposition mappings (canonical or canonical+compatibility), recursively.
  2. Put the string into canonical order, which means:
     while there exists a pair of characters, A immediately followed by B,
	 such that combiningclass(A) > combiningclass(B) > 0  [an "exchangeable pair"]:
	   swap A and B;
  This results in NFD (normalized form D, after canonical decomposition)
  or NFKD (normalized form KD, after compatibility decomposition).
- Canonical composition:
  1. Before composition, the string should have been decomposed
     (using either canonical or compatibility decomposition).
  2. For each character C (from left to right):
     2.1.  Find the last starter S before C (if not found, continue).
	 2.2.  If there is, between S and C, some character with a combining class >= than that of C, then continue.
	 2.3.  If there exists a character L for which the canonical decomposition is S+L
	       and L is not in the composition exclusion table [i.e. L is a "primary composite"],
		   then replace S by L, and remove C.
  This results in NFC (normalized form C, with canonical decomposition followed by canonical composition)
  or NFKC (normalized form KD, with compatibility decomposition followed by canonical composition).
- Composition exclusion table:
  - Anything in CompositionExclusions.txt.
  - Singletons: characters whose canonical decomposition is a single character.
  - Non-starter decompositions: characters whose canonical decomposition begins with a non-starter.

Example:
                 E-grave  (00c8; composition class 0; canonical decomposition: 0045 0300)
				 E-macron (0112; composition class 0;                          0045 0304)
				 grave   (0300; composition class 230)
                 macron  (0304; composition class 230)
  source string: 00c8 0304
  after canonical decomposition (or compatibility decomposition, they would be the same here): 0045 0300 0304
  after canonical composition: 00c8 0304

  cc(horn) = 216
  cc(dot below) = 220
  cc(dot above) = 230

ToDos:
- case folding - je misljen predvsem za primerjanje tako dobljenih nizov.
  Funkcija f(s) = NFC(toCaseFold(s)) je idempotentna.
  Funkcija g(s) = NFKC(toCaseFold(s)) pa ni -- ce hocemo to, moramo pri foldingu
  upostevati se nekaj dodatnih mappingov (glej 5.18, zadnji odstavek; DerivedNormalizationProps.txt).
- Zdi se, da je CaseFolding.txt v bistvu cisto navaden folding v lowercase.
  Ker hocemo imeti tudi ostale foldinge, glejmo raje SpecialCasing.txt
  (+ simple case mappinge v UnicodeData.txt).
  Predlagam, da pri branju SpecialCasing.txt conditional mappinge kar ignoriramo
  in jih potem upostevamo posebej kar v source kodi nasih programov [za
  podrobno definicijo pogojev pa glej tabelo 3.13].
  - Pripis: vseeno se mi zdi, da je CaseFolding.txt nekaj malo drugacnega od navadnega lowercase.
    Na primer, za small final sigma 03c2 je tam navedeno, naj se spremeni v navadno small sigma 03c3.
	To ne sledi niti iz UnicodeData.txt niti iz SpecialCasing.txt, pa ceprav v UCD.html pise,
	da je CaseFolding.txt izpeljan iz njiju.  Glavni namen CaseFolding.txt naj bi bil za
	potrebe "locale-independent case folding" (table 4.1 in sec. 5.18).
  - Preden se zacnes ubadati s case conversioni, si oglej razdelek 3.13
    in se posebej str. 90.
  - Glej str. 91 o kombinaciji N[K]FD + caseFold + N[K]FD
  - definicija cased ipd. na str. 89
- isIdentifierStart(c), isIdentifierEnd(c) -- sec. 5.15
  Glej DerivedCoreProperties.txt, kjer je na podoben nacin definiranih se kup podobnih
  stvari, med drugim isLowerCase in isUpperCase.  Tam je tudi isLetter, isAlphabetic itd. (sec. 4.9).
  To je se najbolje dodati med flagse posameznega characterja.
- general category: sec. 4.5
- motivacija za titlecase: 5.18
- primerjaj nas dosedanji izracun compositionExclusion s tistim, kar je naracunano v DerivedNormalizationProps.txt
  pod Full_Composition_Exclusion
- script names: Scripts.txt in UAX #24.
- block names: Blocks.txt
- space characters: table 6.2 in baje tudi UCD.html
- dash characters: table 6.3
*/

//#endif

