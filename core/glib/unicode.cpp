// Unicode.cpp : Defines the entry point for the console application.
//

/////////////////////////////////////////////////
// Includes
//#include "unicode.h"

//-----------------------------------------------------------------------------
// Private declarations of this module
//-----------------------------------------------------------------------------

namespace {

class TVectorBuilder2
{
public:
	TIntV v;
	TVectorBuilder2(int i) { v.Add(i); }
	operator TIntV() const { return v; }
	TVectorBuilder2& operator ,(int i) { v.Add(i); return *this; }
};

class TVectorBuilder
{
public:
	operator TIntV() const { return TIntV(); }
	TVectorBuilder2 operator ,(int i) { return TVectorBuilder2(i); }
};

TVectorBuilder VB;

TStr CombinePath(const TStr& s, const TStr& t)
{
	int n = s.Len(); if (n <= 0) return t;
	if (s[n - 1] == '\\' || s[n - 1] == '/' || s[n - 1] == ':') return s + t;
	return s + "\\" + t;
}

void AssertEq(const TIntV& v1, const TIntV& v2, const TStr& explanation, FILE *f)
{
	const int n = v1.Len();
	bool ok = (n == v2.Len());
	if (ok) for (int i = 0; i < n && ok; i++) ok = ok && (v1[i] == v2[i]);
	if (! ok)
	{
		if (! f) f = stderr;
		fprintf(f, "%s: [", explanation.CStr());
		for (int i = 0; i < v1.Len(); i++) fprintf(f, "%s%04x", (i == 0 ? "" : " "), int(v1[i]));
		fprintf(f, "] != [");
		for (int i = 0; i < v2.Len(); i++) fprintf(f, "%s%04x", (i == 0 ? "" : " "), int(v2[i]));
		fprintf(f, "]\n");
		Fail;
	}
}

};

//-----------------------------------------------------------------------------
// TUniCodec -- miscellaneous declarations
//-----------------------------------------------------------------------------

uint TUniCodec::GetRndUint(TRnd& rnd)
{
	uint u = rnd.GetUniDevUInt(256) & 0xff;
	u <<= 8; u |= (rnd.GetUniDevUInt(256) & 0xff);
	u <<= 8; u |= (rnd.GetUniDevUInt(256) & 0xff);
	u <<= 8; u |= (rnd.GetUniDevUInt(256) & 0xff);
	return u;
}

uint TUniCodec::GetRndUint(TRnd& rnd, uint minVal, uint maxVal)
{
	if (minVal == TUInt::Mn && maxVal == TUInt::Mx) return GetRndUint(rnd);
	uint range = maxVal - minVal + 1;
	if (range > (uint(1) << (8 * sizeof(uint) - 1)))
		while (true) { uint u = GetRndUint(rnd); if (u < range) return minVal + u; }
	uint mask = 1;
	while (mask < range) mask <<= 1;
	mask -= 1;
	while (true) { uint u = GetRndUint(rnd) & mask; if (u < range) return minVal + u; }
}

bool TUniCodec::IsMachineLittleEndian()
{
	static bool isLE, initialized = false;
	if (initialized) return isLE;
	int i = 0x0201;
	char *p = (char *) (&i);
	char c1, c2;
	memcpy(&c1, p, 1); memcpy(&c2, p + 1, 1);
	if (c1 == 1 && c2 == 2) isLE = true;
	else if (c1 == 2 && c2 == 1) isLE = false;
	else {
		FailR(("TUniCodec::IsMachineLittleEndian: c1 = " + TInt::GetStr(int(uchar(c1)), "%02x") + ", c2 = " + TInt::GetStr(int(uchar(c2)), "%02x") + ".").CStr());
		isLE = true; }
	initialized = true; return isLE;
}

//-----------------------------------------------------------------------------
// TUniCodec -- UTF-8 test driver
//-----------------------------------------------------------------------------

void TUniCodec::TestUtf8(bool decode, size_t expectedRetVal, bool expectedThrow, const TIntV& src, const TIntV& expectedDest, FILE *f)
{
	TIntV dest;
	if (f) {
		fprintf(f, "Settings: %s  %s  %s   replacementChar = %x\n",
			(errorHandling == uehAbort ? "abort" : errorHandling == uehThrow ? "throw" : errorHandling == uehIgnore ? "ignore" : errorHandling == uehReplace ? "replace" : "????"),
			(strict ? "STRICT" : ""), (skipBom ? "skipBom" : ""), uint(replacementChar));
		fprintf(f, "src: "); for (int i = 0; i < src.Len(); i++) fprintf(f, (decode ? " %02x" : " %x"), uint(src[i])); }
	try
	{
		size_t retVal = (decode ? DecodeUtf8(src, 0, src.Len(), dest, true) : EncodeUtf8(src, 0, src.Len(), dest, true));
		if (f) {
			fprintf(f, "\n -> dest:    "); for (int i = 0; i < dest.Len(); i++) fprintf(f, (decode ? " %x" :  " %02x"), uint(dest[i]));
			fprintf(f, "\n    expDest  "); for (int i = 0; i < expectedDest.Len(); i++) fprintf(f, (decode ? " %x" :  " %02x"), uint(expectedDest[i]));
			fprintf(f, "\n    retVal = %d (expected %d)\n", retVal, expectedRetVal); }
		if (retVal != expectedRetVal)
			printf("!!!");
		IAssert(retVal == expectedRetVal); IAssert(! expectedThrow);
		if (dest.Len() != expectedDest.Len())
			printf("!!!");
		IAssert(dest.Len() == expectedDest.Len());
		for (int i = 0; i < dest.Len(); i++) IAssert(dest[i] == expectedDest[i]);
	}
	catch (TUnicodeException e)
	{
		if (f) {
			fprintf(f, "\n -> expDest  "); for (int i = 0; i < expectedDest.Len(); i++) fprintf(f, " %x", uint(expectedDest[i]));
			fprintf(f, "\n    exception \"%s\" at %d (char 0x%02x)\n", e.message.CStr(), int(e.srcIdx), uint(e.srcChar)); }
		IAssert(expectedThrow);
	}
}

// Generates a random UTF-8-encoded stream according to the specifications in 'testCaseDesc',
// then calls TestUtf8 to make sure that DecodeUtf8 reacts as expected.
void TUniCodec::TestDecodeUtf8(TRnd& rnd, const TStr& testCaseDesc)
{
	TIntV src; TIntV expectedDest; int expectedRetVal = 0;
	bool expectedAbort = false;
	FILE *f = 0; // stderr
	// testCaseDesc should consist of pairs or triples of characters, 'cd[e]', where:
	// - 'c' defines the range from which the codepoint should be taken ('A'..'H', 'X'..'Z');
	// - 'd' defines how many bytes the codepoint should be encoded with ('1'..'6');
	// - 'e' defines how many bytes will be removed from the end of the encoded sequence for this codepoint.
	//   (absent = 0, 'a' = 1, 'b' = 2 and so on).
	for (int i = 0; i < testCaseDesc.Len(); )
	{
		IAssert(i + 2 <= testCaseDesc.Len());
		const char c = testCaseDesc[i], d = testCaseDesc[i + 1]; i += 2;
		uint cp; int nBytes = -1, minBytes = -1; bool eighties = false;
		IAssert('1' <= d && d <= '6'); nBytes = d - '0';
		if (c == 'A') { cp = GetRndUint(rnd, 0u, 0x7fu); minBytes = 1; } // 1 byte
		else if (c == 'B') { cp = GetRndUint(rnd, 0x80u, 0x7ffu); minBytes = 2; } // 2 bytes
		else if (c == 'C') { cp = GetRndUint(rnd, 0x800u, 0xffffu); minBytes = 3; } // 3 bytes
		else if (c == 'D') { cp = GetRndUint(rnd, 0x10000u, 0x10ffffu); minBytes = 4; } // 4 bytes, valid Unicode
		else if (c == 'E') { cp = GetRndUint(rnd, 0x110000u, 0x1fffffu); minBytes = 4; } // 4 bytes, invalid Unicode
		else if (c == 'F') { cp = GetRndUint(rnd, 0x200000u, 0x3ffffffu); minBytes = 5; } // 5 bytes
		else if (c == 'G') { cp = GetRndUint(rnd, 0x4000000u, 0x7fffffffu); minBytes = 6; } // 6 bytes, 31 bits
		else if (c == 'H') { cp = GetRndUint(rnd, 0x80000000u, 0xffffffffu); minBytes = 6; } // 6 bytes, 32 bits
		else if (c == 'X') { cp = 0xfffe; minBytes = 3; }
		else if (c == 'Y') { cp = 0xfeff; minBytes = 3; }
		else if (c == 'Z') { eighties = true; minBytes = 1; } // insert several random 10xxxxxx bytes (= 0x80 | random(0..0x3f))
		else Fail;
		IAssert(nBytes >= minBytes);
		// Process 'e'.
		int nToDel = 0;
		if (i < testCaseDesc.Len()) {
			const char e = testCaseDesc[i];
			if (e >= 'a' && e <= 'e') { i += 1; nToDel = e - 'a' + 1; }}
		IAssert(nToDel < nBytes);
		// Will an error occur during the decoding of this codepoint?
		bool errHere = false;
		if (eighties) errHere = true;
		else if (nToDel > 0) errHere = true;
		else if (strict && (cp >= 0x10ffff || nBytes > minBytes)) errHere = true;
		// Update 'expectedDest' and 'expetedRetVal'.
		if (! expectedAbort) {
			if (! errHere) {
				if (src.Len() == 0 && (cp == 0xfffe || cp == 0xfeff) && skipBom) { }
				else { expectedDest.Add(cp); expectedRetVal += 1; } }
			else if (errorHandling == uehReplace) {
				if (eighties) for (int j = 0; j < nBytes; j++) expectedDest.Add(replacementChar);
				else expectedDest.Add(replacementChar); }
			if (errHere && (errorHandling == uehAbort || errorHandling == uehThrow)) expectedAbort = true; }
		// Update 'src'.
		if (eighties) for (int j = 0; j < nBytes; j++) src.Add(GetRndUint(rnd, 0x80, 0xff));
		else if (nBytes == 1) src.Add(cp);
		else {
			int mask = (1 << nBytes) - 1; mask <<= (8 - nBytes);
			src.Add(mask | (uint(cp) >> (6 * (nBytes - 1))));
			for (int j = 1; j < nBytes - nToDel; j++) src.Add(0x80 | ((cp >> (6 * (nBytes - j - 1))) & _0011_1111)); }
	}
	if (f) fprintf(f, "Test case: \"%s\"\n", testCaseDesc.CStr());
	TestUtf8(true, expectedRetVal, expectedAbort && (errorHandling == uehThrow), src, expectedDest, f);
}

void TUniCodec::TestUtf8()
{
	TIntV utf8ReplCh; EncodeUtf8((TVectorBuilder(), replacementChar).v, 0, 1, utf8ReplCh, true);
	for (int skipBom_ = 0; skipBom_ < 2; skipBom_++)
	for (int strict_ = 0; strict_ < 2; strict_++)
	for (int errMode_ = 0; errMode_ < 4; errMode_++)
	{
		strict = (strict_ == 1); errorHandling = TUnicodeErrorHandling(errMode_); skipBom = (skipBom_ == 1);
		TRnd rnd = TRnd(123);
		// Test DecodeUtf8 on various random UTF-8-encoded sequences.
		for (int i = 0; i < 10; i++)
		{
			TestDecodeUtf8(rnd, "X3A1A2A3A4A5A6B2B3B4B5B6C3C4C5C6D4D5D6E5E6F6G6");
			TestDecodeUtf8(rnd, "X3A5dA6d");
			TestDecodeUtf8(rnd, "X3A1B2C3D4E4F5A1G6H6Y3X3A1");
			TestDecodeUtf8(rnd, "X3A1B2C3D4E4F5A2G6H6Y3X3A1");
			TestDecodeUtf8(rnd, "Y3A1B2C3D4E4F5A1G6H6Y3X3A1");
			TestDecodeUtf8(rnd, "A1B2C3D4E4F5A1G6H6Y3X3A1");
			TestDecodeUtf8(rnd, "G6A1A1D4E4A1B2");
			TestDecodeUtf8(rnd, "D4A1A1C3A1B2A1B2");
			TestDecodeUtf8(rnd, "D4A1A1C3A1B2A1B2D4a");
			TestDecodeUtf8(rnd, "X3A1B2C3D5E4F5A1G6H6Y3X3A1");
			TestDecodeUtf8(rnd, "X3A1B2C3D4E5F5A1G6H6Y3X3A1");
			TestDecodeUtf8(rnd, "X3A1B2C3D4aE4F5A1G6H6Y3X3A1");
			TestDecodeUtf8(rnd, "X3A1B2C3D4bE4F5A1G6H6Y3X3A1");
			TestDecodeUtf8(rnd, "X3A2aA3aA4aA5aA6aB2aB3aB4aB5aB6aC3aC4aC5aC6aD4aD5aD6aE5aE6aF6aG6a");
			TestDecodeUtf8(rnd, "X3A3bA4bA5bA6aB3bB4bB5bB6bC3bC4bC5bC6bD4bD5bD6bE5bE6bF6bG6b");
			TestDecodeUtf8(rnd, "X3A4cA5cA6cB4cB5cB6cC4cC5cC6cD4cD5cD6cE5cE6cF6cG6c");
			TestDecodeUtf8(rnd, "X3A5dA6dB5dB6dC5dC6dD5dD6dE5dE6dF6dG6d");
			TestDecodeUtf8(rnd, "X3A6eB6eC6eD6eE6eF6eG6e");
		}
		// Test both DecodeUtf8 and EncodeUtf8 systematically on various characters
		// close to powers of 2.
		TIntV src, expectedDest, src2;
		expectedDest.Gen(1); src.Reserve(6); src2.Gen(1);
		for (int pow = 8; pow <= 32; pow++)
		{
			uint uFrom, uTo;
			if (pow == 8) uFrom = 0, uTo = 1u << pow;
			else if (pow == 32) uFrom = TUInt::Mx - (1u << 8), uTo = TUInt::Mx;
			else uFrom = (1u << pow) - (1u << 8), uTo = (1u << pow) + (1u << 8);
			printf("%u..%u          \r", uFrom, uTo);
			for (uint u = uFrom; ; u++)
			{
				int nBytes = 0;
				if (u < (1u << 7)) nBytes = 1;
				else if (u < (1u << 11)) nBytes = 2;
				else if (u < (1u << 16)) nBytes = 3;
				else if (u < (1u << 21)) nBytes = 4;
				else if (u < (1u << 26)) nBytes = 5;
				else nBytes = 6;
				src.Gen(6, nBytes);
				if (nBytes == 1) src[0] = u;
				else {
					src[0] = (((1 << nBytes) - 1) << (8 - nBytes)) | (u >> (6 * (nBytes - 1)));
					for (int i = 1; i < nBytes; i++) src[i] = 0x80 | ((u >> (6 * (nBytes - i - 1))) & _0011_1111); }
				bool err = (strict && u > 0x10ffff);
				expectedDest.Reserve(1, 0);
				if (err && errorHandling == uehReplace) expectedDest.Add(replacementChar);
				else if (! err) expectedDest.Add(u);
				int erv = (err ? 0 : 1);
				if (skipBom && (u == 0xfeff || u == 0xfffe)) expectedDest.Clr(), erv = 0;
				TestUtf8(true, erv, (err && errorHandling == uehThrow), src, expectedDest, 0);
				// We can also test the UTF-8 encoder.
				src2[0] = u;
				if (err) {
					if (errorHandling == uehReplace) src = utf8ReplCh;
					else src.Clr(false); }
				TestUtf8(false, (err ? 0 : 1), (err && errorHandling == uehThrow), src2, src, 0);
				//
				if (u == uTo) break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// TUniCodec -- UTF-16 test driver
//-----------------------------------------------------------------------------

void TUniCodec::WordsToBytes(const TIntV& src, TIntV& dest)
{
	dest.Clr();
	bool isLE = IsMachineLittleEndian();
	for (int i = 0; i < src.Len(); i++) {
		int c = src[i] & 0xffff;
		if (isLE) { dest.Add(c & 0xff); dest.Add((c >> 8) & 0xff); }
		else { dest.Add((c >> 8) & 0xff); dest.Add(c & 0xff); } }
}

void TUniCodec::TestUtf16(bool decode, size_t expectedRetVal, bool expectedThrow, const TIntV& src, const TIntV& expectedDest,
	const TUtf16BomHandling bomHandling, const TUniByteOrder defaultByteOrder, const bool insertBom,
	FILE *f)
{
	TIntV srcBytes, expectedDestBytes;
	WordsToBytes(src, srcBytes); WordsToBytes(expectedDest, expectedDestBytes);
	TIntV dest;
	if (f) {
		fprintf(f, "Settings: %s  %s  %s  %s  %s replacementChar = %x  \n",
			(errorHandling == uehAbort ? "abort" : errorHandling == uehThrow ? "throw" : errorHandling == uehIgnore ? "ignore" : errorHandling == uehReplace ? "replace" : "????"),
			(strict ? "STRICT" : ""), (decode ? (skipBom ? "skipBom" : "") : (insertBom ? "insrtBom" : "")),
			(bomHandling == bomAllowed ? "bomAllowed" : bomHandling == bomRequired ? "bomRequired" : "bomIgnored"),
			(defaultByteOrder == boBigEndian ? "boBigEndian" : defaultByteOrder == boLittleEndian ? "boLittleEndian" : "boMachineEndian"),
			uint(replacementChar));
		fprintf(f, "src: "); for (int i = 0; i < src.Len(); i++) fprintf(f, (decode ? " %04x" : " %x"), uint(src[i])); }
	for (int useBytes = 0; useBytes < 2; useBytes++)
	{
		const char *fmt = (useBytes ? " %02x" : " %04x");
		try
		{
			dest.Clr();
			size_t retVal;
			if (! useBytes) {
				if (decode) retVal = DecodeUtf16FromWords(src, 0, src.Len(), dest, true, bomHandling, defaultByteOrder);
				else retVal = EncodeUtf16ToWords(src, 0, src.Len(), dest, true, insertBom, defaultByteOrder); }
			else {
				if (decode) retVal = DecodeUtf16FromBytes(srcBytes, 0, srcBytes.Len(), dest, true, bomHandling, defaultByteOrder);
				else retVal = EncodeUtf16ToBytes(src, 0, src.Len(), dest, true, insertBom, defaultByteOrder); }
			const TIntV& ed = (useBytes && ! decode ? expectedDestBytes : expectedDest);
			if (f) {
				fprintf(f, "\n -> dest:    "); for (int i = 0; i < dest.Len(); i++) fprintf(f, (decode ? " %x" :  fmt), uint(dest[i]));
				fprintf(f, "\n    expDest  "); for (int i = 0; i < ed.Len(); i++) fprintf(f, (decode ? " %x" :  fmt), uint(ed[i]));
				fprintf(f, "\n    retVal = %d (expected %d)\n", retVal, expectedRetVal); }
			bool ok = true;
			if (retVal != expectedRetVal) ok = false;
			if (dest.Len() != ed.Len()) ok = false;
			if (ok) for (int i = 0; i < dest.Len(); i++) if (dest[i] != ed[i]) ok = false;
			if (! ok)
			{
				printf("!!!\n");
			}
			IAssert(retVal == expectedRetVal); IAssert(! expectedThrow);
			IAssert(dest.Len() == ed.Len());
			for (int i = 0; i < dest.Len(); i++) IAssert(dest[i] == ed[i]);
		}
		catch (TUnicodeException e)
		{
			if (f) {
				fprintf(f, "\n -> expDest  "); for (int i = 0; i < expectedDest.Len(); i++) fprintf(f, (decode ? " %x" : fmt), uint(expectedDest[i]));
				fprintf(f, "\n    exception \"%s\" at %d (char 0x%02x)\n", e.message.CStr(), int(e.srcIdx), uint(e.srcChar)); }
			IAssert(expectedThrow);
		}
	}
}

// Generates a random UTF-16-encoded stream according to the specifications in 'testCaseDesc',
// then calls TestUtf16 to make sure that DecodeUtf16 reacts as expected.
void TUniCodec::TestDecodeUtf16(TRnd& rnd, const TStr& testCaseDesc,
	const TUtf16BomHandling bomHandling,
	const TUniByteOrder defaultByteOrder,
	const bool insertBom)
{
	TIntV src; TIntV expectedDest; int expectedRetVal = 0;
	bool expectedAbort = false;
	FILE *f = 0;
	bool isMachineLe = IsMachineLittleEndian();
	bool isDefaultLe = (defaultByteOrder == boLittleEndian || (defaultByteOrder == boMachineEndian && isMachineLe));
	bool swap = (isMachineLe != isDefaultLe);
	if (insertBom) {
		src.Add(swap ? 0xfffe : 0xfeff);
		if (! skipBom) { expectedRetVal += 1; expectedDest.Add(0xfeff); } }
	else if (bomHandling == bomRequired) {
		expectedAbort = true; expectedRetVal = -1; }
	// testCaseDesc should consist single characters or pairs of characters, 'c[e]', where:
	// - 'c' defines the range from which the codepoint should be taken ('A'..'E', 'X'..'Y');
	// - 'e' defines how many words will be removed from the end of the encoded sequence for this codepoint.
	//   (absent = 0, 'a' = 1).
	for (int i = 0; i < testCaseDesc.Len(); )
	{
		const char c = testCaseDesc[i++];
		uint cp; int nWords = -1;
		if (c == 'X' || c == 'Y') IAssert(i > 1); // if you want a BOM at the beginning of your data, use insertBom -- if we permit X and Y here, predicting the expectedDest and expectedRetVal gets more complicated
		if (c == 'A') { cp = GetRndUint(rnd, 0u, Utf16FirstSurrogate - 1); nWords = 1; } // characters below the first surrogate range
		else if (c == 'B') { cp = GetRndUint(rnd, Utf16FirstSurrogate, Utf16FirstSurrogate + 1023); nWords = 1; } // the first surrogate range
		else if (c == 'C') { cp = GetRndUint(rnd, Utf16SecondSurrogate, Utf16SecondSurrogate + 1023); nWords = 1; } // the second surrogate range
		else if (c == 'D') { do { cp = GetRndUint(rnd, Utf16SecondSurrogate + 1024, 0xffffu); } while (cp == 0xfffe || cp == 0xfeff); nWords = 1; } // above the second surrogate range, but still in the BMP
		else if (c == 'E') { cp = GetRndUint(rnd, 0x10000u, 0x10ffffu); nWords = 2; } // above the BMP, but still within the range for UTF-16
		else if (c == 'X') { cp = 0xfffe; nWords = 1; }
		else if (c == 'Y') { cp = 0xfeff; nWords = 1; }
		else Fail;
		if (c == 'B' && i < testCaseDesc.Len()) IAssert(testCaseDesc[i] != 'C');
		// Process 'e'.
		int nToDel = 0;
		if (i < testCaseDesc.Len()) {
			const char e = testCaseDesc[i];
			if (e >= 'a') { i += 1; nToDel = 1; }}
		IAssert((nWords == 1 && nToDel == 0) || (nWords == 2 && (nToDel == 0 || nToDel == 1)));
		if (nWords == 2 && nToDel == 1 && i < testCaseDesc.Len()) IAssert(testCaseDesc[i] != 'C');
		// Will an error occur during the decoding of this codepoint?
		bool errHere = false;
		if (Utf16FirstSurrogate <= cp && cp <= Utf16FirstSurrogate + 1023) errHere = true;
		else if (cp > 0x10ffff) { Fail; errHere = true; }
		else if (nToDel > 0) errHere = true;
		else if (strict && (Utf16SecondSurrogate <= cp && cp <= Utf16SecondSurrogate + 1023)) errHere = true;
		// Update 'expectedDest' and 'expectedRetVal'.
		if (! expectedAbort) {
			if (! errHere) {
				if (src.Len() == 0 && (cp == 0xfffe || cp == 0xfeff) && skipBom) { }
				else { expectedDest.Add(cp); expectedRetVal += 1; } }
			else if (errorHandling == uehReplace) {
				expectedDest.Add(replacementChar); }
			if (errHere && (errorHandling == uehAbort || errorHandling == uehThrow)) expectedAbort = true; }
		// Update 'src'.
		if (nWords == 1) src.Add(swap ? SwapBytes(cp) : cp);
		else {
			int c1 = ((cp - 0x10000) >> 10) & 1023; c1 += Utf16FirstSurrogate;
			int c2 = (cp - 0x10000) & 1023; c2 += Utf16SecondSurrogate;
			src.Add(swap ? SwapBytes(c1) : c1);
			if (nToDel == 0) src.Add(swap ? SwapBytes(c2) : c2); }
	}
	if (f) fprintf(f, "Test case: \"%s\"\n", testCaseDesc.CStr());
	TestUtf16(true, expectedRetVal, expectedAbort && (errorHandling == uehThrow), src, expectedDest, bomHandling, defaultByteOrder, false, f);
}

void TUniCodec::TestUtf16()
{
	TIntV utf16ReplCh; utf16ReplCh.Add(replacementChar);
	for (int skipBom_ = 0; skipBom_ < 2; skipBom_++)
	for (int strict_ = 0; strict_ < 2; strict_++)
	for (int errMode_ = 0; errMode_ < 4; errMode_++)
	for (int bomHandling_ = 0; bomHandling_ < 3; bomHandling_++)
	for (int byteOrder_ = 0; byteOrder_ < 3; byteOrder_++)
	for (int insertBom_ = 0; insertBom_ < 2; insertBom_++)
	{
		strict = (strict_ == 1); errorHandling = TUnicodeErrorHandling(errMode_); skipBom = (skipBom_ == 1);
		bool insertBom = (insertBom_ == 1);
		TUniByteOrder byteOrder = (TUniByteOrder) byteOrder_;
		TUtf16BomHandling bomHandling = (TUtf16BomHandling) bomHandling_;
		TRnd rnd = TRnd(123);
		// Test DecodeUtf16 on various random UTF-16-encoded sequences.
		for (int i = 0; i < 10; i++)
		{
			TestDecodeUtf16(rnd, "A", bomHandling, byteOrder, insertBom);
			TestDecodeUtf16(rnd, "AAA", bomHandling, byteOrder, insertBom);
			TestDecodeUtf16(rnd, "B", bomHandling, byteOrder, insertBom);
			TestDecodeUtf16(rnd, "DDAADADAAADDDAA", bomHandling, byteOrder, insertBom);
			TestDecodeUtf16(rnd, "DEEEDAAEEDADEEAAEEADEEDDAA", bomHandling, byteOrder, insertBom);
			TestDecodeUtf16(rnd, "DEaEaEDAAEaEDADEaEAAEEADEEDDAA", bomHandling, byteOrder, insertBom);
			TestDecodeUtf16(rnd, "CABDEBACCEaB", bomHandling, byteOrder, insertBom);
			TestDecodeUtf16(rnd, "EaEEEEaBBACABXABYXXEaYDDXBDCEA", bomHandling, byteOrder, insertBom);
			TestDecodeUtf16(rnd, "EaEEEEaBDCAAXADYXXEaYDDXDCEA", bomHandling, byteOrder, insertBom);
		}
		//continue;
		// Test both DecodeUtf16 and EncodeUtf16 systematically on various characters
		// close to powers of 2.
		TIntV src, expectedDest, src2;
		expectedDest.Gen(1); src.Reserve(6); src2.Gen(1);
		for (int pow = 8; pow <= 32; pow++)
		{
			uint uFrom, uTo;
			if (pow == 8) uFrom = 0, uTo = 1u << pow;
			else if (pow == 32) uFrom = TUInt::Mx - (1u << 8), uTo = TUInt::Mx;
			else uFrom = (1u << pow) - (1u << 8), uTo = (1u << pow) + (1u << 8);
			printf("%u..%u          \r", uFrom, uTo);
			for (uint u = uFrom; ; u++)
			{
				int nWords = 0;
				if (u < 0x10000) nWords = 1;
				else nWords = 2;
				bool isMachineLe = IsMachineLittleEndian(), isDestLe = (byteOrder == boLittleEndian || (byteOrder == boMachineEndian && isMachineLe));
				bool swap = (isMachineLe != isDestLe);
				bool err = (u > 0x10ffff) || (Utf16FirstSurrogate <= u && u <= Utf16FirstSurrogate + 1023) || (strict && Utf16SecondSurrogate <= u && u <= Utf16SecondSurrogate + 1023);
				src.Gen(3, (err ? 0 : nWords) + (insertBom ? 1 : 0));
				if (insertBom) src[0] = (swap ? 0xfffe : 0xfeff);
				if (! ((u > 0x10ffff) || (Utf16FirstSurrogate <= u && u <= Utf16FirstSurrogate + 1023)))
				{
					// Try to encode 'u' and see if it gets decoded correctly.
					if (nWords == 1) src[insertBom ? 1 : 0] = (swap ? SwapBytes(u) : u);
					else {
						int u1 = Utf16FirstSurrogate + (((u - 0x10000) >> 10) & 1023);
						int u2 = Utf16SecondSurrogate + ((u - 0x10000) & 1023);
						src[insertBom ? 1 : 0] = (swap ? SwapBytes(u1) : u1);
						src[insertBom ? 2 : 1] = (swap ? SwapBytes(u2) : u2); }
					if (! ((u == 0xfffe || u == 0xfeff) && bomHandling == bomAllowed && ! insertBom)) // this will just create a mess when decoding
					{
						expectedDest.Reserve(2, 0);
						if (insertBom && ! skipBom) expectedDest.Add(0xfeff);
						if (err && errorHandling == uehReplace) expectedDest.Add(replacementChar);
						else if (! err) expectedDest.Add(u);
						int erv = (err ? 0 : expectedDest.Len());
						if (skipBom && (u == 0xfeff || u == 0xfffe) && ! insertBom) expectedDest.Clr(), erv = 0;
						bool errD = err;
						if (bomHandling == bomRequired && ! insertBom) {
							expectedDest.Clr(false);
							if (u == 0xfeff || u == 0xfffe) { erv = (skipBom ? 0 : 1); if (! skipBom) expectedDest.Add(0xfeff); }
							else { erv = -1; errD = true;
								/*if (errorHandling == uehReplace) expectedDest.Add(replacementChar);*/ }}
						TestUtf16(true, erv, (errD && errorHandling == uehThrow), src, expectedDest, bomHandling, byteOrder, insertBom, 0);
					}
				}
				// We can also test the UTF-16 encoder.
				src2[0] = u;
				if (err) {
					src.Clr(false); if (insertBom) src.Add(swap ? 0xfffe : 0xfeff);
					if (errorHandling == uehReplace) {
						src.Add(swap ? SwapBytes(replacementChar) : replacementChar);
						/*if (byteOrder == boBigEndian || (byteOrder == boMachineEndian && ! TUniCodec::IsMachineLittleEndian())) { src.Add((replacementChar >> 8) & 0xff); src.Add(replacementChar & 0xff); }
						else { src.Add(replacementChar & 0xff); src.Add((replacementChar >> 8) & 0xff); } */
					}}
				TestUtf16(false, (err ? 0 : 1) + (insertBom ? 1 : 0), (err && errorHandling == uehThrow), src2, src, bomHandling, byteOrder, insertBom, 0);
				//
				if (u == uTo) break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// TUniCaseFolding
//-----------------------------------------------------------------------------

void TUniCaseFolding::LoadTxt(const TStr& fileName)
{
	Clr();
	TUniChDb::TUcdFileReader reader; reader.Open(fileName);
	TStrV fields;
	while (reader.GetNextLine(fields))
	{
		int cp = reader.ParseCodePoint(fields[0]);
		const TStr status = fields[1], mapsTo = fields[2];
		if (status == "C" || status == "S" || status == "T") {
			TIntH &dest = (status == "C" ? cfCommon : status == "S" ? cfSimple : cfTurkic);
			IAssert(! dest.IsKey(cp));
			int cp2 = reader.ParseCodePoint(mapsTo);
			dest.AddDat(cp, cp2); }
		else if (status == "F") {
			TIntIntVH &dest = cfFull;
			IAssert(! dest.IsKey(cp));
			TIntV cps; reader.ParseCodePointList(mapsTo, cps); IAssert(cps.Len() > 0);
			dest.AddDat(cp, cps); }
		else
			FailR(status.CStr());
	}
	printf("TUniCaseFolding(\"%s\"): %d common, %d simple, %d full, %d Turkic.\n",
		fileName.CStr(), cfCommon.Len(), cfSimple.Len(), cfFull.Len(), cfTurkic.Len());
}

void TUniCaseFolding::Test(const TIntV& src, const TIntV& expectedDest, const bool full, const bool turkic, FILE *f)
{
	fprintf(f, "TUniCaseFolding(%s%s): ", (full ? "full" : "simple"), (turkic ? ", turkic" : ""));
	for (int i = 0; i < src.Len(); i++) fprintf(f, " %04x", int(src[i]));
	TIntV dest; Fold(src, 0, src.Len(), dest, true, full, turkic);
	fprintf(f, "\n  -> ");
	for (int i = 0; i < dest.Len(); i++) fprintf(f, " %04x", int(dest[i]));
	fprintf(f, "\n");
	IAssert(dest.Len() == expectedDest.Len());
	for (int i = 0; i < dest.Len(); i++) IAssert(dest[i] == expectedDest[i]);
}

/*
void TUniCaseFolding::Test(const TIntV& src, FILE *f) {
	Test(src, false, false, f); Test(src, false, true, f);
	Test(src, true, false, f); Test(src, true, true, f); }
*/

void TUniCaseFolding::Test()
{
	FILE *f = stderr;
	TVectorBuilder VB;
	// simple
	Test((VB, 0x41, 0x62, 0x49, 0x43, 0xdf), (VB, 0x61, 0x62, 0x69, 0x63, 0xdf), false, false, f);
	// simple + turkic
	Test((VB, 0x41, 0x62, 0x49, 0x43, 0xdf), (VB, 0x61, 0x62, 0x131, 0x63, 0xdf), false, true, f);
	// full
	Test((VB, 0x41, 0x62, 0x49, 0x43, 0xdf), (VB, 0x61, 0x62, 0x69, 0x63, 0x73, 0x73), true, false, f);
	// full + turkic
	Test((VB, 0x41, 0x62, 0x49, 0x43, 0xdf), (VB, 0x61, 0x62, 0x131, 0x63, 0x73, 0x73), true, true, f);
}

//-----------------------------------------------------------------------------
// TUniChInfo
//-----------------------------------------------------------------------------

// UAX #14
const ushort TUniChInfo::LineBreak_Unknown = TUniChInfo::GetLineBreakCode('X', 'X');
const ushort TUniChInfo::LineBreak_ComplexContext = TUniChInfo::GetLineBreakCode('S', 'A');
const ushort TUniChInfo::LineBreak_Numeric = TUniChInfo::GetLineBreakCode('N', 'U');
const ushort TUniChInfo::LineBreak_InfixNumeric = TUniChInfo::GetLineBreakCode('I', 'S');
const ushort TUniChInfo::LineBreak_Quotation = TUniChInfo::GetLineBreakCode('Q', 'U');

//-----------------------------------------------------------------------------
// TUniChDb -- word breaking
//-----------------------------------------------------------------------------

// Test driver for WbFind*NonIgnored.
void TUniChDb::TestWbFindNonIgnored(const TIntV& src) const
{
	int n = src.Len();
	TBoolV isIgnored; isIgnored.Gen(n);
	for (int i = 0; i < n; i++) isIgnored[i] = IsWbIgnored(src[i]);
	TIntV prevNonIgnored, nextNonIgnored, curOrNextNonIgnored;
	prevNonIgnored.Gen(n); nextNonIgnored.Gen(n); curOrNextNonIgnored.Gen(n);
	FILE *f = 0; // stderr;
	for (int srcIdx = 0; srcIdx < n; srcIdx++) for (int srcLen = 1; srcLen < n - srcIdx; srcLen++)
	{
		int prev = -1;
		for (int i = 0; i < srcLen; i++) {
			prevNonIgnored[i] = prev;
			if (! isIgnored[srcIdx + i]) prev = srcIdx + i; }
		int next = srcIdx + srcLen;
		for (int i = srcLen - 1; i >= 0; i--) {
			nextNonIgnored[i] = next;
			if (! isIgnored[srcIdx + i]) next = srcIdx + i;
			curOrNextNonIgnored[i] = next; }
		if (f) {
			fprintf(f, "\nIndex:     "); for (int i = 0; i < srcLen; i++) fprintf(f, " %2d", srcIdx + i);
			fprintf(f, "\nNonIgn:    "); for (int i = 0; i < srcLen; i++) fprintf(f, " %s", (isIgnored[srcIdx + i] ? " ." : " Y"));
			fprintf(f, "\nPrevNI:    "); for (int i = 0; i < srcLen; i++) fprintf(f, " %2d", int(prevNonIgnored[i]));
			fprintf(f, "\nNextNI:    "); for (int i = 0; i < srcLen; i++) fprintf(f, " %2d", int(nextNonIgnored[i]));
			fprintf(f, "\nCurNextNI: "); for (int i = 0; i < srcLen; i++) fprintf(f, " %2d", int(curOrNextNonIgnored[i]));
			fprintf(f, "\n"); }
		for (int i = 0; i < srcLen; i++)
		{
			size_t s;
			s = size_t(srcIdx + i); WbFindNextNonIgnored(src, s, size_t(srcIdx + srcLen));
			IAssert(s == size_t(nextNonIgnored[i]));
			s = size_t(srcIdx + i); WbFindCurOrNextNonIgnored(src, s, size_t(srcIdx + srcLen));
			IAssert(s == size_t(curOrNextNonIgnored[i]));
			s = size_t(srcIdx + i); bool ok = WbFindPrevNonIgnored(src, size_t(srcIdx), s);
			if (prevNonIgnored[i] < 0) { IAssert(! ok); IAssert(s == size_t(srcIdx)); }
			else { IAssert(ok); IAssert(s == size_t(prevNonIgnored[i])); }
		}
	}
}

void TUniChDb::TestWbFindNonIgnored() const
{
	TIntV chIgnored, chNonIgnored;
	FILE *f = 0; // stderr;
	for (int i = h.FFirstKeyId(); h.FNextKeyId(i); ) {
		const int cp = h.GetKey(i); const TUniChInfo& ci = h[i];
		if (f) fprintf(f, "%04x: flags %08x props %08x %08x script \"%s\"\n", cp,
			ci.flags, ci.properties, ci.propertiesX, GetScriptName(ci.script).CStr());
		(IsWbIgnored(h[i]) ? chIgnored : chNonIgnored).Add(h.GetKey(i));
	}
	chIgnored.Sort(); chNonIgnored.Sort();
	printf("TUniChDb::TestWbNonIgnored: %d ignored, %d nonignored chars.\n", chIgnored.Len(), chNonIgnored.Len());
	TRnd rnd = TRnd(123);
	for (int iter = 0; iter <= 50; iter++)
	{
		int percIgnored = 2 * iter;
		for (int n = 0; n <= 20; n++)
		{
			// Prepare a random sequence of 'n' codepoints.
			TIntV v; v.Gen(n);
			for (int i = 0; i < n; i++) {
				TIntV& chars = (rnd.GetUniDevInt(100) < percIgnored) ? chIgnored : chNonIgnored;
				int j = rnd.GetUniDevInt(chars.Len());
				v.Add(chars[j]); }
			// Run the tests with this sequence.
			TestWbFindNonIgnored(v);
		}
	}
}

void TUniChDb::TestFindNextWordOrSentenceBoundary(const TStr& basePath, bool sentence)
{
	TUcdFileReader reader; TStrV fields;
	reader.Open(CombinePath(CombinePath(basePath, GetAuxiliaryDir()), (sentence ? GetSentenceBreakTestFn() : GetWordBreakTestFn())));
	int nLines = 0; TRnd rnd = TRnd(123);
	while (reader.GetNextLine(fields))
	{
		nLines += 1;
		IAssert(fields.Len() == 1);
		TStrV parts; fields[0].SplitOnWs(parts);
		const int n = parts.Len(); IAssert((n % 2) == 1);
		TIntV chars; TBoolV isBreak, isPredicted, isPredicted2;
		// Each line is a sequence of codepoints, with a \times or \div in between each
		// pair of codepoints (as well as at the beginning and the end of the sequence) to
		// indicate whether a boundary exists there or not.
		for (int i = 0; i < n; i++)
		{
			const TStr& s = parts[i];
			if ((i % 2) == 0) {
				if (s == "\xc3\x97") // multiplication sign (U+00D7) in UTF-8
					isBreak.Add(false);
				else if (s == "\xc3\xb7") // division sign (U+00F7) in UTF-8
					isBreak.Add(true);
				else FailR(s.CStr()); }
			else chars.Add(reader.ParseCodePoint(s));
		}
		const int m = n / 2; IAssert(chars.Len() == m); IAssert(isBreak.Len() == m + 1);
		IAssert(isBreak[0]); IAssert(isBreak[m]);
		isPredicted.Gen(m + 1); isPredicted.PutAll(false);
		if (AlwaysFalse()) { printf("%3d", nLines); for (int i = 0; i < m; i++) printf(" %04x", int(chars[i])); printf("\n"); }
		// We'll insert a few random characters at the beginning of the sequence
		// so that srcPos doesn't always begin at 0.
		for (int nBefore = 0; nBefore < 5; nBefore++)
		{
			TIntV chars2; for (int i = 0; i < nBefore; i++) chars2.Add(0, rnd.GetUniDevInt(0x10ffff + 1));
			chars2.AddV(chars);
			// Use FindNextBoundary to find all the word boundaries.
			size_t position = (nBefore > 0 ? nBefore - 1 : nBefore); size_t prevPosition = position;
			while (sentence ? FindNextSentenceBoundary(chars2, nBefore, m, position) : FindNextWordBoundary(chars2, nBefore, m, position))
			{
				IAssert(prevPosition < position);
				IAssert(position <= size_t(nBefore + m));
				isPredicted[int(position) - nBefore] = true;
				prevPosition = position;
			}
			IAssert(position == size_t(nBefore + m));
			if (sentence) FindSentenceBoundaries(chars2, nBefore, m, isPredicted2);
			else FindWordBoundaries(chars2, nBefore, m, isPredicted2);
			IAssert(isPredicted2.Len() == m + 1);
			bool ok = true;
			// If we start at 0, the word boundary at the beginning of the sequence was
			// not found explicitly, so we'll add it now.
			if (nBefore == 0) isPredicted[0] = true;
			// Compare the predicted and the true boundaries.
			for (int i = 0; i <= m; i++) {
				if (isBreak[i] != isPredicted[i]) ok = false;
				IAssert(isPredicted2[i] == isPredicted[i]); }
			FILE *f = stderr;
			if (! ok)
			{
				fprintf(f, "\nError in line %d:\n", nLines);
				fprintf(f, "True:      ");
				for (int i = 0; i <= m; i++) {
					fprintf(f, "%s ", (isBreak[i] ? "|" : "."));
					if (i < m) fprintf(f, "%04x ", int(chars[i + nBefore])); }
				fprintf(f, "\nPredicted: ");
				for (int i = 0; i <= m; i++) {
					fprintf(f, "%s ", (isPredicted[i] ? "|" : "."));
					if (i < m) {
						const int cp = chars[i + nBefore];
						TStr s = sentence ? TUniChInfo::GetSbFlagsStr(GetSbFlags(cp)) : TUniChInfo::GetWbFlagsStr(GetWbFlags(cp));
						if (IsWbIgnored(cp)) s = "*" + s;
						fprintf(f, "%4s ", s.CStr()); }}
				fprintf(f, "\n");
				Fail;
			}
			// Test FindNextBoundary if we start in the middle of the sequence,
			// i.e. not at an existing boundary.
			for (int i = 0; i < m; i++) {
				position = i + nBefore; bool ok = sentence ? FindNextSentenceBoundary(chars2, nBefore, m, position) : FindNextWordBoundary(chars2, nBefore, m, position);
				IAssert(ok); // at the very least, there should be the 'boundary' at nBefore + m
				IAssert(size_t(i + nBefore) < position); IAssert(position <= size_t(nBefore + m));
				position -= nBefore;
				for (int j = i + 1; j < int(position); j++)
					IAssert(! isBreak[j]);
				IAssert(isBreak[int(position)]); }
		}
	}
	reader.Close();
	printf("TUniChDb::TestFindNext%sBoundary: %d lines processed.\n", (sentence ? "Sentence" : "Word"), nLines);
}

//-----------------------------------------------------------------------------
// TUniChDb -- composition and decomposition
//-----------------------------------------------------------------------------

void TUniChDb::TestComposition(const TStr& basePath)
{
	TUcdFileReader reader; TStrV fields; int nLines = 0;
	reader.Open(CombinePath(basePath, GetNormalizationTestFn()));
	bool inPart1 = false; TIntH testedInPart1;
	while (reader.GetNextLine(fields))
	{
		nLines += 1;
		if (fields.Len() == 1) {
			IAssert(fields[0].IsPrefix("@Part"));
			inPart1 = (fields[0] == "@Part1"); continue; }
		IAssert(fields.Len() == 6);
		IAssert(fields[5].Len() == 0);
		TIntV c1, c2, c3, c4, c5;
		reader.ParseCodePointList(fields[0], c1);
		reader.ParseCodePointList(fields[1], c2);
		reader.ParseCodePointList(fields[2], c3);
		reader.ParseCodePointList(fields[3], c4);
		reader.ParseCodePointList(fields[4], c5);
		TIntV v;
#define AssE_(v1, v2, expl) AssertEq(v1, v2, TStr(expl) + " (line " + TInt::GetStr(nLines) + ")", 0)
#define NFC_(cmpWith, operand) DecomposeAndCompose(operand, 0, operand.Len(), v, false); AssE_(cmpWith, v, #cmpWith " == NFC(" #operand ")")
#define NFD_(cmpWith, operand) Decompose(operand, 0, operand.Len(), v, false); AssE_(cmpWith, v, #cmpWith " == NFD(" #operand ")")
#define NFKC_(cmpWith, operand) DecomposeAndCompose(operand, 0, operand.Len(), v, true); AssE_(cmpWith, v, #cmpWith " == NFKC(" #operand ")")
#define NFKD_(cmpWith, operand) Decompose(operand, 0, operand.Len(), v, true); AssE_(cmpWith, v, #cmpWith " == NFKD(" #operand ")")
		// NFD:
		NFD_(c3, c1);   // c3 == NFD(c1)
		NFD_(c3, c2);   // c3 == NFD(c2)
		NFD_(c3, c3);   // c3 == NFD(c3)
		NFD_(c5, c4);   // c5 == NFD(c4)
		NFD_(c5, c5);   // c5 == NFD(c5)
		// NFC:
		NFC_(c2, c1);   // c2 == NFC(c1)
		NFC_(c2, c2);   // c2 == NFC(c2)
		NFC_(c2, c3);   // c2 == NFC(c3)
		NFC_(c4, c4);   // c4 == NFC(c4)
		NFC_(c4, c5);   // c4 == NFC(c5)
		// NFKD:
		NFKD_(c5, c1);   // c5 == NFKD(c1)
		NFKD_(c5, c2);   // c5 == NFKD(c2)
		NFKD_(c5, c3);   // c5 == NFKD(c3)
		NFKD_(c5, c4);   // c5 == NFKD(c4)
		NFKD_(c5, c5);   // c5 == NFKD(c5)
		// NFKC:
		NFKC_(c4, c1);   // c4 == NFKC(c1)
		NFKC_(c4, c2);   // c4 == NFKC(c2)
		NFKC_(c4, c3);   // c4 == NFKC(c3)
		NFKC_(c4, c4);   // c4 == NFKC(c4)
		NFKC_(c4, c5);   // c4 == NFKC(c5)
		//
		if (inPart1) {
			IAssert(c1.Len() == 1);
			testedInPart1.AddKey(c1[0]); }
	}
	reader.Close();
	// Test other individual codepoints that were not mentioned in part 1.
	int nOther = 0;
	for (int i = h.FFirstKeyId(); h.FNextKeyId(i); )
	{
		const int cp = h.GetKey(i), nLines = -1;
		if (testedInPart1.IsKey(cp)) continue;
		TIntV x, v; x.Add(cp);
		NFC_(x, x);    // x == NFC(x)
		NFD_(x, x);    // x == NFD(x)
		NFKC_(x, x);   // x == NFKC(x)
		NFKD_(x, x);   // x == NFKD(x)
		nOther += 1;
	}
#undef AssE_
#undef NFC_
#undef NFD_
#undef NFKC_
#undef NFKD_
	printf("TUniChDb::TestComposition: %d lines processed + %d other individual codepoints.\n", nLines, nOther);
}

//-----------------------------------------------------------------------------
// TUniChDb -- case conversion tests
//-----------------------------------------------------------------------------

void TUniChDb::TestCaseConversion(const TStr& source, const TStr& trueLc,
								  const TStr& trueTc, const TStr& trueUc,
								  bool turkic, bool lithuanian)
{
	TIntV src;
	TUcdFileReader::ParseCodePointList(source, src);
	FILE *f = stderr;
	for (int i = 0; i < 3; i++)
	{
		TCaseConversion how = (i == 0) ? ccLower : (i == 1) ? ccTitle : ccUpper;
		const TStr &trueDestS = (how == ccLower ? trueLc : how == ccTitle ? trueTc : trueUc);
		TIntV trueDest; TUcdFileReader::ParseCodePointList(trueDestS, trueDest);
		TIntV dest;
		GetCaseConverted(src, 0, src.Len(), dest, true, how, turkic, lithuanian);
		bool ok = (dest.Len() == trueDest.Len());
		if (ok) for (int i = 0; i < dest.Len() && ok; i++) ok = ok && (dest[i] == trueDest[i]);
		if (ok) continue;
		fprintf(f, "%s(", (how == ccLower ? "toLowercase" : how == ccTitle ? "toTitlecase" : "toUppercase"));
		for (int i = 0; i < src.Len(); i++) fprintf(f, "%s%04x", (i == 0 ? "" : " "), int(src[i]));
		fprintf(f, ")\nCorrect:   (");
		for (int i = 0; i < trueDest.Len(); i++) fprintf(f, "%s%04x", (i == 0 ? "" : " "), int(trueDest[i]));
		fprintf(f, ")\nOur output:(");
		for (int i = 0; i < dest.Len(); i++) fprintf(f, "%s%04x", (i == 0 ? "" : " "), int(dest[i]));
		fprintf(f, ")\n");
		IAssert(ok);
	}
}

void TUniChDb::TestCaseConversions()
{
	// Because no thorough case-conversion test files have been provided as part
	// of the Unicode standard, we'll have to test things on a few test cases of our own.
	// - First, test some unconditional special mappings, such as 'ss', 'ffl', 'dz', etc.
	const TStr F = "0046 ", L = "004C ", S = "0053 ", T = "0054 ", W = "0057 ";
	const TStr f = "0066 ", l = "006c ", s = "0073 ", t = "0074 ", w = "0077 ";
	const TStr ss = "00df ", ffl = "fb04 ", longs = "017f ", longst = "fb05 ", wRing = "1e98 ", Ring = "030a ";
	const TStr DZ = "01c4 ", Dz = "01c5 ", dz = "01c6 ";
	const TStr space = "0020 ", Grave = "0300 ";
	TestCaseConversion(
		F + L + s + t + space + Dz + w + T + ss + wRing + space + longs + DZ + space + dz + longst,  // source
		f + l + s + t + space + dz + w + t + ss + wRing + space + longs + dz + space + dz + longst,  // lowercase
		F + l + s + t + space + Dz + w + t + ss + wRing + space + S + dz + space + Dz + longst,      // titlecase
		F + L + S + T + space + DZ + W + T + S + S + W + Ring + space + S + DZ + space + DZ + S + T, // uppercase
		false, false);
	// - Dotted I, dotless i, etc., but with turkic == false.
	const TStr I = "0049 ", J = "004a ", i = "0069 ", j = "006a ", iDotless = "0131 ", IDot = "0130 ", DotA = "0307 ";
	TestCaseConversion(
		s + I + t + i + w + iDotless + f + IDot + l + space + iDotless + DotA + f + I + DotA + s, // source
		s + i + t + i + w + iDotless + f + i + DotA + l + space + iDotless + DotA + f + i + DotA + s, // lowercase
		S + i + t + i + w + iDotless + f + i + DotA + l + space + I + DotA + f + i + DotA + s, // titlecase
		S + I + T + I + W + I + F + IDot + L + space + I + DotA + F + I + DotA + S, // uppercase
		false, false);
	// - Sigma (final vs. non-final forms).
	const TStr Sigma = "03a3 ", sigma = "03c3 ", fsigma = "03c2 ";
	TestCaseConversion(
		Sigma + s + space + s + Sigma  + space + s + Sigma + s + space + Sigma + S + Sigma  + space + Sigma, // source
		sigma + s + space + s + fsigma + space + s + sigma + s + space + sigma + s + fsigma + space + sigma, // lowercase
		Sigma + s + space + S + fsigma + space + S + sigma + s + space + Sigma + s + fsigma + space + Sigma, // titlecase
		Sigma + S + space + S + Sigma  + space + S + Sigma + S + space + Sigma + S + Sigma  + space + Sigma, // uppercase
		false, false);
	TestCaseConversion(
		sigma + s + space + s + sigma  + space + s + sigma + s + space + sigma + S + sigma  + space + sigma, // source
		sigma + s + space + s + sigma  + space + s + sigma + s + space + sigma + s + sigma  + space + sigma, // lowercase
		Sigma + s + space + S + sigma  + space + S + sigma + s + space + Sigma + s + sigma  + space + Sigma, // titlecase
		Sigma + S + space + S + Sigma  + space + S + Sigma + S + space + Sigma + S + Sigma  + space + Sigma, // uppercase
		false, false);
	TestCaseConversion(
		fsigma + s + space + s + fsigma + space + s + fsigma + s + space + fsigma + S + fsigma  + space + fsigma, // source
		fsigma + s + space + s + fsigma + space + s + fsigma + s + space + fsigma + s + fsigma  + space + fsigma, // lowercase
		Sigma  + s + space + S + fsigma + space + S + fsigma + s + space + Sigma  + s + fsigma  + space + Sigma, // titlecase
		Sigma  + S + space + S + Sigma  + space + S + Sigma  + S + space + Sigma  + S + Sigma   + space + Sigma, // uppercase
		false, false);
	const TStr nonSA = "0315 0321 0322 "; // characters that are neither ccStarter nor ccAbove
	// Special case mappings for Turkic languages:
	// - After_I
	TestCaseConversion(
		s + I + t + i + w + iDotless + f + IDot + l + space + iDotless + DotA + f + I + DotA + J + DotA + I + Grave + DotA + I + DotA + DotA + I + nonSA + DotA + s, // source
		s + iDotless + t + i + w + iDotless + f + i + l + space + iDotless + DotA + f + i + j + DotA + iDotless + Grave + DotA + i + DotA + i + nonSA + s, // lowercase
		S + iDotless + t + i + w + iDotless + f + i + l + space + I + DotA + f + i + j + DotA + iDotless + Grave + DotA + i + DotA + i + nonSA + s, // titlecase
		S + I + T + IDot + W + I + F + IDot + L + space + I + DotA + F + I + DotA + J + DotA + I + Grave + DotA + I + DotA + DotA + I + nonSA + DotA + S, // uppercase
		true, false); // turkic
	// - Not_Before_Dot
	TestCaseConversion(
		I + Grave + t + I + DotA + f + I + nonSA + DotA + j + space + I + nonSA + DotA + space + I + Grave + t, // source
		iDotless + Grave + t + i + f + i + nonSA + j + space + i + nonSA + space + iDotless + Grave + t, // lowercase
		I + Grave + t + i + f + i + nonSA + j + space + I + nonSA + DotA + space + I + Grave + t, // titlecase
		I + Grave + T + I + DotA + F + I + nonSA + DotA + J + space + I + nonSA + DotA + space + I + Grave + T, // uppercase
		true, false); // turkic
	// Special case mappings for Lithuanian:
	// - After_Soft_Dotted  [note: I + DotA turns into i + DotA + DotA when lowercasing due to More_Above]
	TestCaseConversion(
		i + DotA + t + i + Grave + DotA + f + i + DotA + DotA + f + i + nonSA + DotA + I + DotA + t + DotA + i + DotA + Grave, // source
		i + DotA + t + i + Grave + DotA + f + i + DotA + DotA + f + i + nonSA + DotA + i + DotA + DotA + t + DotA + i + DotA + Grave, // lowercase
		I + t + i + Grave + DotA + f + i + DotA + DotA + f + i + nonSA + DotA + i + DotA + DotA + t + DotA + i + DotA + Grave, // titlecase
		I + T + I + Grave + DotA + F + I + DotA + F + I + nonSA + I + DotA + T + DotA + I + Grave, // uppercase
		false, true); // lithuanian
	// - More_Above  [note: j + DotA turns into just J when uppercasing due to After_Soft_Dotted]
	TestCaseConversion(
		J +        Grave + space + J +        nonSA + DotA + space + j + Grave + space + j + DotA + space + J + nonSA + J +        nonSA + Grave + space + j + nonSA, // source
		j + DotA + Grave + space + j + DotA + nonSA + DotA + space + j + Grave + space + j + DotA + space + j + nonSA + j + DotA + nonSA + Grave + space + j + nonSA, // lowercase
		J +        Grave + space + J +        nonSA + DotA + space + J + Grave + space + J +        space + J + nonSA + j + DotA + nonSA + Grave + space + J + nonSA, // titlecase
		J +        Grave + space + J +        nonSA + DotA + space + J + Grave + space + J +        space + J + nonSA + J +        nonSA + Grave + space + J + nonSA, // uppercase
		false, true); // lithuanian
	// SoftDotted [^ Starter Above]* 0307   --(uc,tc)-->  brez 0307
	// SoftDotted [^ Starter Above]* 0307   --(
	//TestCaseConversion("", "", "", "", false, false);
}

//-----------------------------------------------------------------------------
// TUniChDb -- initialization from the text files
//-----------------------------------------------------------------------------

void TUniChDb::LoadTxt_ProcessDecomposition(TUniChInfo& ci, TStr s)
{
	if (s.Empty()) return;
	if (s[0] == '<') {
		int i = s.SearchCh('>'); IAssert(i > 0);
		ci.flags |= ucfCompatibilityDecomposition;
		s = s.GetSubStr(i + 1, s.Len() - 1); s.ToTrunc(); }
	TIntV dec; TUcdFileReader::ParseCodePointList(s, dec);
	IAssert(dec.Len() > 0);
	ci.decompOffset = decompositions.Len();
	decompositions.AddV(dec); decompositions.Add(-1);
}

void TUniChDb::InitPropList(const TStr& basePath)
{
	TUcdFileReader reader; TStrV fields; int nCps = 0, nLines = 0;
	reader.Open(CombinePath(basePath, GetPropListFn()));
	TSubcatHelper helper(*this);
	while (reader.GetNextLine(fields))
	{
		IAssert(fields.Len() == 2);
		int from, to; reader.ParseCodePointRange(fields[0], from, to);
		TStr s = fields[1];
		TUniChProperties prop = TUniChProperties(0); TUniChPropertiesX propx = TUniChPropertiesX(0);
		if (s == "White_Space") prop = ucfPrWhiteSpace;
		else if (s == "Bidi_Control") prop = ucfPrBidiControl;
		else if (s == "Join_Control") prop = ucfPrJoinControl;
		else if (s == "Dash") prop = ucfPrDash;
		else if (s == "Hyphen") prop = ucfPrHyphen;
		else if (s == "Quotation_Mark") prop = ucfPrQuotationMark;
		else if (s == "Terminal_Punctuation") prop = ucfPrTerminalPunctuation;
		else if (s == "Other_Math") propx = ucfPxOtherMath;
		else if (s == "Hex_Digit") prop = ucfPrHexDigit;
		else if (s == "ASCII_Hex_Digit") prop = ucfPrAsciiHexDigit;
		else if (s == "Other_Alphabetic") propx = ucfPxOtherAlphabetic;
		else if (s == "Ideographic") prop = ucfPrIdeographic;
		else if (s == "Diacritic") prop = ucfPrDiacritic;
		else if (s == "Extender") prop = ucfPrExtender;
		else if (s == "Other_Lowercase") propx = ucfPxOtherLowercase;
		else if (s == "Other_Uppercase") propx = ucfPxOtherUppercase;
		else if (s == "Noncharacter_Code_Point") prop = ucfPrNoncharacterCodePoint;
		else if (s == "Other_Grapheme_Extend") propx = ucfPxOtherGraphemeExtend;
		else if (s == "IDS_Binary_Operator") propx = ucfPxIdsBinaryOperator;
		else if (s == "IDS_Trinary_Operator") propx = ucfPxIdsTrinaryOperator;
		else if (s == "Radical") propx = ucfPxRadical;
		else if (s == "Unified_Ideograph") propx = ucfPxUnifiedIdeograph;
		else if (s == "Other_Default_Ignorable_Code_Point") propx = ucfPxOtherDefaultIgnorableCodePoint;
		else if (s == "Deprecated") prop = ucfPrDeprecated;
		else if (s == "Soft_Dotted") prop = ucfPrSoftDotted;
		else if (s == "Logical_Order_Exception") prop = ucfPrLogicalOrderException;
		else if (s == "Other_ID_Start") propx = ucfPxOtherIdStart;
		else if (s == "Other_ID_Continue") propx = ucfPxOtherIdContinue;
		else if (s == "STerm") prop = ucfPrSTerm;
		else if (s == "Variation_Selector") prop = ucfPrVariationSelector;
		else if (s == "Pattern_White_Space") prop = ucfPrPatternWhiteSpace;
		else if (s == "Pattern_Syntax") prop = ucfPrPatternSyntax;
		else FailR(s.CStr());
		helper.ProcessComment(reader);
		for (int cp = from; cp <= to; cp++) {
			int i = h.GetKeyId(cp); if (i < 0) { i = h.AddKey(cp); helper.SetCat(cp); }
			TUniChInfo &ci = h[i]; helper.TestCat(cp);
			if (prop) { IAssert(! ci.IsProperty(prop)); ci.SetProperty(prop); }
			if (propx) { IAssert(! ci.IsPropertyX(propx)); ci.SetPropertyX(propx); }
			nCps++; }
		nLines++;
	}
	reader.Close();
	printf("TUniChDb::InitPropList: %d lines, %d code points.\n", nLines, nCps);
}

void TUniChDb::InitDerivedCoreProperties(const TStr& basePath)
{
	TUcdFileReader reader; TStrV fields; int nCps = 0, nLines = 0;
	reader.Open(CombinePath(basePath, GetDerivedCorePropsFn()));
	TSubcatHelper helper(*this);
	while (reader.GetNextLine(fields))
	{
		IAssert(fields.Len() == 2);
		int from, to; reader.ParseCodePointRange(fields[0], from, to);
		TStr s = fields[1];
		TUniChFlags flag;
		if (s == "Math") flag = ucfDcpMath;
		else if (s == "Alphabetic") flag = ucfDcpAlphabetic;
		else if (s == "Lowercase") flag = ucfDcpLowercase;
		else if (s == "Uppercase") flag = ucfDcpUppercase;
		else if (s == "ID_Start") flag = ucfDcpIdStart;
		else if (s == "ID_Continue") flag = ucfDcpIdContinue;
		else if (s == "XID_Start") flag = ucfDcpXidStart;
		else if (s == "XID_Continue") flag = ucfDcpXidContinue;
		else if (s == "Default_Ignorable_Code_Point") flag = ucfDcpDefaultIgnorableCodePoint;
		else if (s == "Grapheme_Extend") flag = ucfDcpGraphemeExtend;
		else if (s == "Grapheme_Base") flag = ucfDcpGraphemeBase;
		else if (s == "Grapheme_Link") continue; // this flag is deprecated; test for combClass == Virama instead
		else FailR(s.CStr());
		// If we add new codepoints to the hash table, we should also set their category.
		// This is supposed to be provided in the comment, e.g. "# Cf       SOFT HYPHEN".
		helper.ProcessComment(reader);
		//
		for (int cp = from; cp <= to; cp++) {
			int i = h.GetKeyId(cp); if (i < 0) { i = h.AddKey(cp); helper.SetCat(cp); }
			helper.TestCat(cp);
			TUniChInfo &ci = h[i]; IAssert(! ci.IsDcpFlag(flag));
			ci.SetDcpFlag(flag); nCps++; }
		nLines++;
	}
	reader.Close();
	printf("TUniChDb::InitDerivedCoreProperties: %d lines, %d code points.\n", nLines, nCps);
}

void TUniChDb::InitLineBreaks(const TStr& basePath)
{
	// Clear old linebreak values.
	ushort xx = TUniChInfo::LineBreak_Unknown;
	for (int i = h.FFirstKeyId(); h.FNextKeyId(i); ) h[i].lineBreak = xx;
	// Read LineBreak.txt.
	TUcdFileReader reader; TStrV fields;
	reader.Open(CombinePath(basePath, GetLineBreakFn()));
	int nLines = 0, nCps = 0;
	while (reader.GetNextLine(fields))
	{
		IAssert(fields.Len() == 2);
		int from, to; reader.ParseCodePointRange(fields[0], from, to);
		TStr s = fields[1]; IAssert(s.Len() == 2);
		ushort us = TUniChInfo::GetLineBreakCode(s[0], s[1]);
		if (us == xx) continue;
		for (int cp = from; cp <= to; cp++) {
			int i = h.GetKeyId(cp); if (i < 0) { i = h.AddKey(cp);
				printf("TUniChDb::InitLineBreaks: warning, adding codepoint %d, its category will remain unknown.\n", cp); }
			IAssert(h[i].lineBreak == xx);
			h[i].lineBreak = us; nCps++; }
		nLines++;
	}
	reader.Close();
	printf("TUniChDb::InitLineBreaks: %d lines, %d codepoints processed (excluding \'xx\' values).\n", nLines, nCps);
}

void TUniChDb::InitScripts(const TStr& basePath)
{
	TUcdFileReader reader; TStrV fields;
	reader.Open(CombinePath(basePath, GetScriptsFn()));
	TSubcatHelper helper(*this);
	while (reader.GetNextLine(fields))
	{
		int from, to; reader.ParseCodePointRange(fields[0], from, to);
		TStr scriptName = fields[1];
		int scriptNo = scripts.GetKeyId(scriptName);
		if (scriptNo < 0) { scriptNo = scripts.AddKey(scriptName); scripts[scriptNo] = 0; }
		IAssert(scriptNo >= 0 && scriptNo < SCHAR_MAX); // because TUniChInfo.script is a signed char
		scripts[scriptNo] += 1;
		helper.ProcessComment(reader);
		for (int cp = from; cp <= to; cp++) {
			int i = h.GetKeyId(cp); if (i < 0) { i = h.AddKey(cp); helper.SetCat(cp); }
			helper.TestCat(cp);
			TUniChInfo &ci = h[i]; ci.script = scriptNo; }
	}
	reader.Close();
	scripts.AddDat(GetScriptNameUnknown()) = 0;
	printf("TUniChDb::InitScripts: %d scripts: ", scripts.Len());
	if (AlwaysFalse()) for (int i = scripts.FFirstKeyId(); scripts.FNextKeyId(i); )
		printf("  %d:%s (%d)", i, scripts.GetKey(i).CStr(), int(scripts[i]));
	printf("\n");
}

void TUniChDb::InitWordAndSentenceBoundaryFlags(const TStr& basePath)
{
	// UAX #29, sec. 4.1 and 5.1.
	// Note: these flags can also be initialized from auxiliary\\WordBreakProperty.txt.
	int katakana = GetScriptByName(GetScriptNameKatakana()); IAssert(katakana >= 0);
	int hiragana = GetScriptByName(GetScriptNameHiragana()); IAssert(hiragana >= 0);
	// Clear any existing word-boundary flags and initialize them again.
	for (int i = h.FFirstKeyId(); h.FNextKeyId(i); )
	{
		const int cp = h.GetKey(i); TUniChInfo& ci = h[i];
		ci.ClrWbAndSbFlags();
		// Word-boundary flags.
		if (ci.subCat  == ucOtherFormat && cp != 0x200c && cp != 0x200d) ci.SetWbFlag(ucfWbFormat);
		if (ci.script == katakana) ci.SetWbFlag(ucfWbKatakana);
		if (ci.lineBreak == TUniChInfo::LineBreak_InfixNumeric && cp != 0x3a) ci.SetWbFlag(ucfWbMidNum);
		if (ci.lineBreak == TUniChInfo::LineBreak_Numeric) ci.SetWbFlag(ucfWbNumeric);
		if (ci.subCat == ucPunctuationConnector) ci.SetWbFlag(ucfWbExtendNumLet);
		// Sentence-boundary flags.  Some are identical to some word-boundary flags.
		if (cp == 0xa || cp == 0xd || cp == 0x85 || cp == 0x2028 || cp == 0x2029) ci.SetSbFlag(ucfSbSep);
		if (ci.subCat == ucOtherFormat && cp != 0x200c && cp != 0x200d) ci.SetSbFlag(ucfSbFormat);
		if (ci.IsWhiteSpace() && ! ci.IsSbFlag(ucfSbSep) && cp != 0xa0) ci.SetSbFlag(ucfSbSp);
		if (ci.IsLowercase() && ! ci.IsGraphemeExtend()) ci.SetSbFlag(ucfSbLower);
		if (ci.IsUppercase() || ci.subCat == ucLetterTitlecase) ci.SetSbFlag(ucfSbUpper);
		if ((ci.IsAlphabetic() || cp == 0xa0 || cp == 0x5f3) && ! ci.IsSbFlag(ucfSbLower) && ! ci.IsSbFlag(ucfSbUpper) && ! ci.IsGraphemeExtend()) ci.SetSbFlag(ucfSbOLetter);
		if (ci.lineBreak == TUniChInfo::LineBreak_Numeric) ci.SetSbFlag(ucfSbNumeric);
		if (cp == 0x2e) ci.SetSbFlag(ucfSbATerm);
		// Note: UAX #29 says that if the property STerm = true, then the character should belong to the STerm class for
		// the purposes of sentence-boundary detection.  Now in PropList.txt there is no doubt that 002E has the STerm
		// property; thus, it should also belong to the STerm sentence-boundary class.  However, in
		// SentenceBreakProperty.txt, 002E is only listed in the ATerm class, but not in the STerm class.
		if (ci.IsSTerminal() && cp != 0x2e) ci.SetSbFlag(ucfSbSTerm);
		if ((ci.subCat == ucPunctuationOpen || ci.subCat == ucPunctuationClose || ci.lineBreak == TUniChInfo::LineBreak_Quotation) && cp != 0x5f3 && ! ci.IsSbFlag(ucfSbATerm) && ! ci.IsSbFlag(ucfSbSTerm)) ci.SetSbFlag(ucfSbClose);
	}
	// Some additional characters for Katakana and MidLetter.
	TIntV v = (VB, 0x3031, 0x3032, 0x3033, 0x3034, 0x3035, 0x309b, 0x309c, 0x30a0, 0x30fc, 0xff70, 0xff9e, 0xff9f);
	for (int i = 0; i < v.Len(); i++) h.GetDat(v[i]).SetWbFlag(ucfWbKatakana);
	v = (VB, 0x27, 0xb7, 0x5f4, 0x2019, 0x2027, 0x3a);
	for (int i = 0; i < v.Len(); i++) h.GetDat(v[i]).SetWbFlag(ucfWbMidLetter);
	// WbALetter depends on Katakana, so it cannot be initialized earlier.
	for (int i = h.FFirstKeyId(); h.FNextKeyId(i); )
	{
		const int cp = h.GetKey(i); TUniChInfo& ci = h[i];
		if ((ci.IsAlphabetic() || cp == 0x5f3) && ! ci.IsIdeographic() && ! ci.IsWbFlag(ucfWbKatakana) && ci.lineBreak != TUniChInfo::LineBreak_ComplexContext && ci.script != hiragana && ! ci.IsGraphemeExtend())
			ci.SetWbFlag(ucfWbALetter);
	}
	// An alternative is to extract the flags from WordBreakProperty.txt.
	// The results should be the same.
	{TUcdFileReader reader; TStrV fields;
	reader.Open(CombinePath(CombinePath(basePath, GetAuxiliaryDir()), GetWordBreakPropertyFn()));
	THash<TInt, TInt> hh;
	while (reader.GetNextLine(fields))
	{
		IAssert(fields.Len() == 2);
		int from, to; reader.ParseCodePointRange(fields[0], from, to);
		TStr s = fields[1]; TUniChFlags flag;
		if (s == "Format") flag = ucfWbFormat;
		else if (s == "Katakana") flag = ucfWbKatakana;
		else if (s == "ALetter") flag = ucfWbALetter;
		else if (s == "MidLetter") flag = ucfWbMidLetter;
		else if (s == "MidNum") flag = ucfWbMidNum;
		else if (s == "Numeric") flag = ucfWbNumeric;
		else if (s == "ExtendNumLet") flag = ucfWbExtendNumLet;
		else FailR(s.CStr());
		for (int c = from; c <= to; c++) {
			int i = hh.GetKeyId(c); if (i < 0) hh.AddDat(c, flag);
			else hh[i].Val |= flag; }
	}
	reader.Close();
	TIntV cps; for (int i = h.FFirstKeyId(); h.FNextKeyId(i); ) cps.Add(h.GetKey(i));
	for (int i = hh.FFirstKeyId(); hh.FNextKeyId(i); ) cps.Add(hh.GetKey(i));
	cps.Sort(); cps.Merge();
	for (int i = 0; i < cps.Len(); i++)
	{
		int cp = cps[i];
		int flags1 = 0; if (h.IsKey(cp)) flags1 = h.GetDat(cp).GetWbFlags();
		int flags2 = 0; if (hh.IsKey(cp)) flags2 = hh.GetDat(cp);
		flags1 &= ~ucfSbSep; flags2 &= ~ucfSbSep;
		if (flags1 != flags2) {
			printf("cp = %04x: flags1 = %08x flags2 = %08x xor = %08x\n", cp, flags1, flags2, flags1 ^ flags2);
			Fail; }
	}}
	// Likewise, for sentence boundary flags we have SentenceBreakProperty.txt.
	{TUcdFileReader reader; TStrV fields;
	reader.Open(CombinePath(CombinePath(basePath, GetAuxiliaryDir()), GetSentenceBreakPropertyFn()));
	THash<TInt, TInt> hh;
	while (reader.GetNextLine(fields))
	{
		IAssert(fields.Len() == 2);
		int from, to; reader.ParseCodePointRange(fields[0], from, to);
		TStr s = fields[1]; TUniChFlags flag;
		if (s == "Sep") flag = ucfSbSep;
		else if (s == "Format") flag = ucfSbFormat;
		else if (s == "Sp") flag = ucfSbSp;
		else if (s == "Lower") flag = ucfSbLower;
		else if (s == "Upper") flag = ucfSbUpper;
		else if (s == "OLetter") flag = ucfSbOLetter;
		else if (s == "Numeric") flag = ucfSbNumeric;
		else if (s == "ATerm") flag = ucfSbATerm;
		else if (s == "STerm") flag = ucfSbSTerm;
		else if (s == "Close") flag = ucfSbClose;
		else FailR(s.CStr());
		for (int c = from; c <= to; c++) {
			int i = hh.GetKeyId(c); if (i < 0) hh.AddDat(c, flag);
			else hh[i].Val |= flag; }
	}
	reader.Close();
	TIntV cps; for (int i = h.FFirstKeyId(); h.FNextKeyId(i); ) cps.Add(h.GetKey(i));
	for (int i = hh.FFirstKeyId(); hh.FNextKeyId(i); ) cps.Add(hh.GetKey(i));
	cps.Sort(); cps.Merge();
	for (int i = 0; i < cps.Len(); i++)
	{
		int cp = cps[i];
		int flags1 = 0; if (h.IsKey(cp)) flags1 = h.GetDat(cp).GetSbFlags();
		int flags2 = 0; if (hh.IsKey(cp)) flags2 = hh.GetDat(cp);
		if (flags1 != flags2) {
			printf("cp = %04x: flags1 = %08x [%s] flags2 = %08x [%s] xor = %08x\n", cp,
				flags1, TUniChInfo::GetSbFlagsStr(flags1).CStr(),
				flags2, TUniChInfo::GetSbFlagsStr(flags2).CStr(),
				flags1 ^ flags2);
			Fail; }
	}}
}

void TUniChDb::InitSpecialCasing(const TStr& basePath)
{
	TUcdFileReader reader; TStrV fields;
	reader.Open(CombinePath(basePath, GetSpecialCasingFn()));
	while (reader.GetNextLine(fields))
	{
		IAssert(fields.Len() == 5 || fields.Len() == 6);
		IAssert(fields.Last().Empty());
		// Skip conditional mappings -- they will be hardcoded in the GetCaseConverted method.
		TStr conditions = "";
		if (fields.Len() == 6) conditions = fields[4];
		conditions.ToTrunc(); if (! conditions.Empty()) continue;
		// Keep the other mappings.
		const int cp = reader.ParseCodePoint(fields[0]);
		TIntV v; reader.ParseCodePointList(fields[1], v);
		specialCasingLower.AddDat(cp, v);
		reader.ParseCodePointList(fields[2], v);
		specialCasingTitle.AddDat(cp, v);
		reader.ParseCodePointList(fields[3], v);
		specialCasingUpper.AddDat(cp, v);
	}
	reader.Close();
}

void TUniChDb::LoadTxt(const TStr& basePath)
{
	Clr();
	// Set up a hash table with enough ports that there will be more or less no chains longer than 1 element.
	h = THash<TInt, TUniChInfo>(196613, true);
	//
	caseFolding.LoadTxt(CombinePath(basePath, GetCaseFoldingFn()));
	//
	TUcdFileReader reader; TStrV fields; TIntH seen;
	reader.Open(CombinePath(basePath, GetUnicodeDataFn()));
	while (reader.GetNextLine(fields))
	{
		// Codepoint.
		int cp = reader.ParseCodePoint(fields[0]);
		IAssert(! seen.IsKey(cp)); seen.AddKey(cp);
		TUniChInfo& ci = h.AddDat(cp);
		// Name.
		ci.nameOffset = charNames.AddStr(fields[1]);
		// Category.
		TStr& s = fields[2]; IAssert(s.Len() == 2);
		ci.chCat = s[0]; ci.chSubCat = s[1];
		// Canonical combining class.
		s = fields[3]; IAssert(s.Len() > 0);
		int i; bool ok = s.IsInt(true, TUCh::Mn, TUCh::Mx, i); IAssertR(ok, s);
		ci.combClass = (uchar) i;
		// Decomposition type and mapping.
		LoadTxt_ProcessDecomposition(ci, fields[5]);
		// Simple case mappings.
		s = fields[12]; ci.simpleUpperCaseMapping = (! s.Empty() ? reader.ParseCodePoint(s) : -1);
		s = fields[13]; ci.simpleLowerCaseMapping = (! s.Empty() ? reader.ParseCodePoint(s) : -1);
		s = fields[14]; ci.simpleTitleCaseMapping = (! s.Empty() ? reader.ParseCodePoint(s) : -1);
		//
		ci.InitAfterLoad(); // initializes ci.cat, ci.subCat
	}
	reader.Close();
	//
	InitScripts(basePath);
	//
	InitPropList(basePath);
	InitDerivedCoreProperties(basePath);
	InitLineBreaks(basePath);
	InitSpecialCasing(basePath);
	// Process the composition exclusions (UAX #15, sec. 6).
	for (int i = h.FFirstKeyId(); h.FNextKeyId(i); )
	{
		TUniChInfo& ci = h[i];
		int ofs = ci.decompOffset; if (ofs < 0) continue;
		int n = 0; while (decompositions[ofs + n] >= 0) n++;
		IAssert(n > 0);
		// Singleton decompositions.
		if (n == 1) { ci.flags |= ucfCompositionExclusion; continue; }
		// Non-starter decompositions.
		int cp1 = decompositions[ofs];
		IAssert(h.IsKey(cp1));
		uchar ccc = h.GetDat(cp1).combClass;
		if (ccc != TUniChInfo::ccStarter) { ci.flags |= ucfCompositionExclusion; continue; }
	}
	// Process the composition exclusion table.
	reader.Open(CombinePath(basePath, GetCompositionExclusionsFn()));
	int nExclusionTable = 0;
	while (reader.GetNextLine(fields))
	{
		IAssert(fields.Len() == 1);
		int cp = reader.ParseCodePoint(fields[0]);
		int i = h.GetKeyId(cp); IAssert(i >= 0);
		h[i].flags |= ucfCompositionExclusion;
		nExclusionTable++;
	}
	reader.Close();
	// Prepare the inverted index for composition pairs.
	for (int i = h.FFirstKeyId(); h.FNextKeyId(i); )
	{
		int cp = h.GetKey(i);
		TUniChInfo& ci = h[i];
		int ofs = ci.decompOffset; if (ofs < 0) continue;
		if (ci.IsCompositionExclusion()) continue;
		if (ci.IsCompatibilityDecomposition()) continue;
		int n = 0; while (decompositions[ofs + n] >= 0) n++;
		if (n != 2) continue;
		TIntPr pr = TIntPr(decompositions[ofs], decompositions[ofs + 1]);
		IAssert(! inverseDec.IsKey(pr));
		IAssert(ci.combClass == TUniChInfo::ccStarter);
		inverseDec.AddDat(pr, cp);
	}
	printf("TUniChDb(%s): %d chars in h, %d in decomp inverse index; %d in decomp vector; %d in exclusion table\n",
		basePath.CStr(), h.Len(), inverseDec.Len(), decompositions.Len(), nExclusionTable);
	// Before calling InitWordBoundaryFlags(), scripts must have been initialized, as well as
	// flags such as Alphabetic, Word_Break, and Grapheme_Extend.
	InitWordAndSentenceBoundaryFlags(basePath); // Note: scripts must have been initialized by this point.
	// Make sure that Hangul combined characters are treated as stareters.
	for (int cp = HangulSBase; cp < HangulSBase + HangulSCount; cp++)
	{
		int j = h.GetKeyId(cp); if (j < 0) continue;
		TUniChInfo& ci = h[j];
		if (ci.combClass == TUniChInfo::ccInvalid) ci.combClass = TUniChInfo::ccStarter;
		IAssert(ci.combClass == TUniChInfo::ccStarter);
	}
	// There should be no more additions to 'h' beyond this point.
	const int oldHLen = h.Len();
	// Provide default (identity) case mappings if any were missing from UnicodeData.txt
	// (or if any entirely new characters were added later, e.g. while reading LineBreaks.txt).
	int scriptUnknown = GetScriptByName(GetScriptNameUnknown());
	for (int i = h.FFirstKeyId(); h.FNextKeyId(i); )
	{
		int cp = h.GetKey(i); TUniChInfo &ci = h[i];
		if (ci.simpleLowerCaseMapping < 0) ci.simpleLowerCaseMapping = cp;
		if (ci.simpleUpperCaseMapping < 0) ci.simpleUpperCaseMapping = cp;
		if (ci.simpleTitleCaseMapping < 0) ci.simpleTitleCaseMapping = cp;
		if (ci.script < 0) ci.script = scriptUnknown;
	}
	IAssert(h.Len() == oldHLen);
}

void TUniChDb::SaveBin(const TStr& fnBinUcd)
{
	PSOut SOut=TFOut::New(fnBinUcd);
	Save(*SOut);
}

void TUniChDb::InitAfterLoad()
{
	scriptUnknown = GetScriptByName(GetScriptNameUnknown()); IAssert(scriptUnknown >= 0);
}

//-----------------------------------------------------------------------------
// TUniChDb -- main test driver
//-----------------------------------------------------------------------------

void TUniChDb::Test(const TStr& basePath)
{
	TStr fnBin = CombinePath(basePath, GetBinFn());
	if (true || ! TFile::Exists(fnBin))
	{
		// Test LoadTxt.
		LoadTxt(basePath);
		// Test Save.
		{PSOut SOut = TFOut::New(fnBin);
		Save(*SOut);}
	}
	// Test Load.
	this->~TUniChDb();
	new(this) TUniChDb();
	{PSIn SIn = TFIn::New(fnBin);
	Load(*SIn);}
	// Test the case folding.
	caseFolding.Test();
	// Test the word breaking.
	TestWbFindNonIgnored();
	// Test the sentence breaking.
	TestFindNextWordOrSentenceBoundary(basePath, true);
	TestFindNextWordOrSentenceBoundary(basePath, false);
	// Test composition and decomposition.
	TestComposition(basePath);
	// Test the case conversions.
	TestCaseConversions();
}

//-----------------------------------------------------------------------------
// T8BitCodec -- a class for converting between 8-bit encodings and Unicode
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ISO-8859-2
//-----------------------------------------------------------------------------

const int TEncoding_ISO8859_2::toUnicodeTable[6 * 16] =
{
	/* 0xa0 */ 0x00a0, 0x0104, 0x02d8, 0x0141, 0x00a4, 0x013d, 0x015a, 0x00a7, 0x00a8, 0x0160, 0x015e, 0x0164, 0x0179, 0x00ad, 0x017d, 0x017b,
	/* 0xb0 */ 0x00b0, 0x0105, 0x02db, 0x0142, 0x00b4, 0x013e, 0x015b, 0x02c7, 0x00b8, 0x0161, 0x015f, 0x0165, 0x017a, 0x02dd, 0x017e, 0x017c,
	/* 0xc0 */ 0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7, 0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e,
	/* 0xd0 */ 0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7, 0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df,
	/* 0xe0 */ 0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7, 0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f,
	/* 0xf0 */ 0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7, 0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9
};

const int TEncoding_ISO8859_2::fromUnicodeTable1[14 * 16] = {
	/* U+00a0 */ 0x00a0,     -1,     -1,     -1, 0x00a4,     -1,     -1, 0x00a7, 0x00a8,     -1,     -1,     -1,     -1, 0x00ad,     -1,     -1,
	/* U+00b0 */ 0x00b0,     -1,     -1,     -1, 0x00b4,     -1,     -1,     -1, 0x00b8,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+00c0 */     -1, 0x00c1, 0x00c2,     -1, 0x00c4,     -1,     -1, 0x00c7,     -1, 0x00c9,     -1, 0x00cb,     -1, 0x00cd, 0x00ce,     -1,
	/* U+00d0 */     -1,     -1,     -1, 0x00d3, 0x00d4,     -1, 0x00d6, 0x00d7,     -1,     -1, 0x00da,     -1, 0x00dc, 0x00dd,     -1, 0x00df,
	/* U+00e0 */     -1, 0x00e1, 0x00e2,     -1, 0x00e4,     -1,     -1, 0x00e7,     -1, 0x00e9,     -1, 0x00eb,     -1, 0x00ed, 0x00ee,     -1,
	/* U+00f0 */     -1,     -1,     -1, 0x00f3, 0x00f4,     -1, 0x00f6, 0x00f7,     -1,     -1, 0x00fa,     -1, 0x00fc, 0x00fd,     -1,     -1,
	/* U+0100 */     -1,     -1, 0x00c3, 0x00e3, 0x00a1, 0x00b1, 0x00c6, 0x00e6,     -1,     -1,     -1,     -1, 0x00c8, 0x00e8, 0x00cf, 0x00ef,
	/* U+0110 */ 0x00d0, 0x00f0,     -1,     -1,     -1,     -1,     -1,     -1, 0x00ca, 0x00ea, 0x00cc, 0x00ec,     -1,     -1,     -1,     -1,
	/* U+0120 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1 /* blank */,
	/* U+0130 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00c5, 0x00e5,     -1,     -1, 0x00a5, 0x00b5,     -1,
	/* U+0140 */     -1, 0x00a3, 0x00b3, 0x00d1, 0x00f1,     -1,     -1, 0x00d2, 0x00f2,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+0150 */ 0x00d5, 0x00f5,     -1,     -1, 0x00c0, 0x00e0,     -1,     -1, 0x00d8, 0x00f8, 0x00a6, 0x00b6,     -1,     -1, 0x00aa, 0x00ba,
	/* U+0160 */ 0x00a9, 0x00b9, 0x00de, 0x00fe, 0x00ab, 0x00bb,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00d9, 0x00f9,
	/* U+0170 */ 0x00db, 0x00fb,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00ac, 0x00bc, 0x00af, 0x00bf, 0x00ae, 0x00be,     -1
};

const int TEncoding_ISO8859_2::fromUnicodeTable2[2 * 16] = {
	/* U+02c0 */     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00b7,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+02d0 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00a2, 0x00ff,     -1, 0x00b2,     -1, 0x00bd,     -1,     -1
};

//-----------------------------------------------------------------------------
// ISO-8859-3
//-----------------------------------------------------------------------------

const int TEncoding_ISO8859_3::toUnicodeTable[6 * 16] = {
	/* 0xa0 */ 0x00a0, 0x0126, 0x02d8, 0x00a3, 0x00a4,     -1, 0x0124, 0x00a7, 0x00a8, 0x0130, 0x015e, 0x011e, 0x0134, 0x00ad,     -1, 0x017b,
	/* 0xb0 */ 0x00b0, 0x0127, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x0125, 0x00b7, 0x00b8, 0x0131, 0x015f, 0x011f, 0x0135, 0x00bd,     -1, 0x017c,
	/* 0xc0 */ 0x00c0, 0x00c1, 0x00c2,     -1, 0x00c4, 0x010a, 0x0108, 0x00c7, 0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
	/* 0xd0 */     -1, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x0120, 0x00d6, 0x00d7, 0x011c, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x016c, 0x015c, 0x00df,
	/* 0xe0 */ 0x00e0, 0x00e1, 0x00e2,     -1, 0x00e4, 0x010b, 0x0109, 0x00e7, 0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
	/* 0xf0 */     -1, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x0121, 0x00f6, 0x00f7, 0x011d, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x016d, 0x015d, 0x02d9
};

const int TEncoding_ISO8859_3::fromUnicodeTable1[14 * 16] = {
	/* U+00a0 */ 0x00a0,     -1,     -1, 0x00a3, 0x00a4,     -1,     -1, 0x00a7, 0x00a8,     -1,     -1,     -1,     -1, 0x00ad,     -1,     -1,
	/* U+00b0 */ 0x00b0,     -1, 0x00b2, 0x00b3, 0x00b4, 0x00b5,     -1, 0x00b7, 0x00b8,     -1,     -1,     -1,     -1, 0x00bd,     -1,     -1,
	/* U+00c0 */ 0x00c0, 0x00c1, 0x00c2,     -1, 0x00c4,     -1,     -1, 0x00c7, 0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
	/* U+00d0 */     -1, 0x00d1, 0x00d2, 0x00d3, 0x00d4,     -1, 0x00d6, 0x00d7,     -1, 0x00d9, 0x00da, 0x00db, 0x00dc,     -1,     -1, 0x00df,
	/* U+00e0 */ 0x00e0, 0x00e1, 0x00e2,     -1, 0x00e4,     -1,     -1, 0x00e7, 0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
	/* U+00f0 */     -1, 0x00f1, 0x00f2, 0x00f3, 0x00f4,     -1, 0x00f6, 0x00f7,     -1, 0x00f9, 0x00fa, 0x00fb, 0x00fc,     -1,     -1,     -1,
	/* U+0100 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00c6, 0x00e6, 0x00c5, 0x00e5,     -1,     -1,     -1,     -1,
	/* U+0110 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00d8, 0x00f8, 0x00ab, 0x00bb,
	/* U+0120 */ 0x00d5, 0x00f5,     -1,     -1, 0x00a6, 0x00b6, 0x00a1, 0x00b1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+0130 */ 0x00a9, 0x00b9,     -1,     -1, 0x00ac, 0x00bc,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+0140 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1 /* blank */,
	/* U+0150 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00de, 0x00fe, 0x00aa, 0x00ba,
	/* U+0160 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00dd, 0x00fd,     -1,     -1,
	/* U+0170 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00af, 0x00bf,     -1,     -1,     -1,
};
const int TEncoding_ISO8859_3::fromUnicodeTable2[2] = {
	/* U+02d8 */ 0x00a2, 0x00ff
};

//-----------------------------------------------------------------------------
// ISO-8859-4
//-----------------------------------------------------------------------------

const int TEncoding_ISO8859_4::toUnicodeTable[6 * 16] = {
	/* 0xa0 */ 0x00a0, 0x0104, 0x0138, 0x0156, 0x00a4, 0x0128, 0x013b, 0x00a7, 0x00a8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00ad, 0x017d, 0x00af,
	/* 0xb0 */ 0x00b0, 0x0105, 0x02db, 0x0157, 0x00b4, 0x0129, 0x013c, 0x02c7, 0x00b8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014a, 0x017e, 0x014b,
	/* 0xc0 */ 0x0100, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x012e, 0x010c, 0x00c9, 0x0118, 0x00cb, 0x0116, 0x00cd, 0x00ce, 0x012a,
	/* 0xd0 */ 0x0110, 0x0145, 0x014c, 0x0136, 0x00d4, 0x00d5, 0x00d6, 0x00d7, 0x00d8, 0x0172, 0x00da, 0x00db, 0x00dc, 0x0168, 0x016a, 0x00df,
	/* 0xe0 */ 0x0101, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x012f, 0x010d, 0x00e9, 0x0119, 0x00eb, 0x0117, 0x00ed, 0x00ee, 0x012b,
	/* 0xf0 */ 0x0111, 0x0146, 0x014d, 0x0137, 0x00f4, 0x00f5, 0x00f6, 0x00f7, 0x00f8, 0x0173, 0x00fa, 0x00fb, 0x00fc, 0x0169, 0x016b, 0x02d9
};

const int TEncoding_ISO8859_4::fromUnicodeTable1[14 * 16] = {
	/* U+00a0 */ 0x00a0,     -1,     -1,     -1, 0x00a4,     -1,     -1, 0x00a7, 0x00a8,     -1,     -1,     -1,     -1, 0x00ad,     -1, 0x00af,
	/* U+00b0 */ 0x00b0,     -1,     -1,     -1, 0x00b4,     -1,     -1,     -1, 0x00b8,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+00c0 */     -1, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6,     -1,     -1, 0x00c9,     -1, 0x00cb,     -1, 0x00cd, 0x00ce,     -1,
	/* U+00d0 */     -1,     -1,     -1,     -1, 0x00d4, 0x00d5, 0x00d6, 0x00d7, 0x00d8,     -1, 0x00da, 0x00db, 0x00dc,     -1,     -1, 0x00df,
	/* U+00e0 */     -1, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6,     -1,     -1, 0x00e9,     -1, 0x00eb,     -1, 0x00ed, 0x00ee,     -1,
	/* U+00f0 */     -1,     -1,     -1,     -1, 0x00f4, 0x00f5, 0x00f6, 0x00f7, 0x00f8,     -1, 0x00fa, 0x00fb, 0x00fc,     -1,     -1,     -1,
	/* U+0100 */ 0x00c0, 0x00e0,     -1,     -1, 0x00a1, 0x00b1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00c8, 0x00e8,     -1,     -1,
	/* U+0110 */ 0x00d0, 0x00f0, 0x00aa, 0x00ba,     -1,     -1, 0x00cc, 0x00ec, 0x00ca, 0x00ea,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+0120 */     -1,     -1, 0x00ab, 0x00bb,     -1,     -1,     -1,     -1, 0x00a5, 0x00b5, 0x00cf, 0x00ef,     -1,     -1, 0x00c7, 0x00e7,
	/* U+0130 */     -1,     -1,     -1,     -1,     -1,     -1, 0x00d3, 0x00f3, 0x00a2,     -1,     -1, 0x00a6, 0x00b6,     -1,     -1,     -1,
	/* U+0140 */     -1,     -1,     -1,     -1,     -1, 0x00d1, 0x00f1,     -1,     -1,     -1, 0x00bd, 0x00bf, 0x00d2, 0x00f2,     -1,     -1,
	/* U+0150 */     -1,     -1,     -1,     -1,     -1,     -1, 0x00a3, 0x00b3,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+0160 */ 0x00a9, 0x00b9,     -1,     -1,     -1,     -1, 0x00ac, 0x00bc, 0x00dd, 0x00fd, 0x00de, 0x00fe,     -1,     -1,     -1,     -1,
	/* U+0170 */     -1,     -1, 0x00d9, 0x00f9,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00ae, 0x00be,     -1,
};

const int TEncoding_ISO8859_4::fromUnicodeTable2[2 * 16] = {
	/* U+02c0 */     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00b7,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+02d0 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00ff,     -1, 0x00b2,     -1,     -1,     -1,     -1
};

//-----------------------------------------------------------------------------
// CP 437
//-----------------------------------------------------------------------------

const int TEncoding_CP437::toUnicodeTable[8 * 16] = {
	/* 0x80 */ 0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7, 0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
	/* 0x90 */ 0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9, 0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x20a7, 0x0192,
	/* 0xa0 */ 0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba, 0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
	/* 0xb0 */ 0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
	/* 0xc0 */ 0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
	/* 0xd0 */ 0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b, 0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
	/* 0xe0 */ 0x03b1, 0x00df, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x00b5, 0x03c4, 0x03a6, 0x0398, 0x03a9, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229,
	/* 0xf0 */ 0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248, 0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0, 0x00a0
};

const int TEncoding_CP437::fromUnicodeTable1[6 * 16] = {
	/* U+00a0 */ 0x00ff, 0x00ad, 0x009b, 0x009c,     -1, 0x009d,     -1,     -1,     -1,     -1, 0x00a6, 0x00ae, 0x00aa,     -1,     -1,     -1,
	/* U+00b0 */ 0x00f8, 0x00f1, 0x00fd,     -1,     -1, 0x00e6,     -1, 0x00fa,     -1,     -1, 0x00a7, 0x00af, 0x00ac, 0x00ab,     -1, 0x00a8,
	/* U+00c0 */     -1,     -1,     -1,     -1, 0x008e, 0x008f, 0x0092, 0x0080,     -1, 0x0090,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+00d0 */     -1, 0x00a5,     -1,     -1,     -1,     -1, 0x0099,     -1,     -1,     -1,     -1,     -1, 0x009a,     -1,     -1, 0x00e1,
	/* U+00e0 */ 0x0085, 0x00a0, 0x0083,     -1, 0x0084, 0x0086, 0x0091, 0x0087, 0x008a, 0x0082, 0x0088, 0x0089, 0x008d, 0x00a1, 0x008c, 0x008b,
	/* U+00f0 */     -1, 0x00a4, 0x0095, 0x00a2, 0x0093,     -1, 0x0094, 0x00f6,     -1, 0x0097, 0x00a3, 0x0096, 0x0081,     -1,     -1, 0x0098,
};

const int TEncoding_CP437::fromUnicodeTable2[4 * 16] = {
	/* U+0390 */     -1,     -1,     -1, 0x00e2,     -1,     -1,     -1,     -1, 0x00e9,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+03a0 */     -1,     -1,     -1, 0x00e4,     -1,     -1, 0x00e8,     -1,     -1, 0x00ea,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+03b0 */     -1, 0x00e0,     -1,     -1, 0x00eb, 0x00ee,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+03c0 */ 0x00e3,     -1,     -1, 0x00e5, 0x00e7,     -1, 0x00ed,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
};

const int TEncoding_CP437::fromUnicodeTable3[6 * 16] = {
	/* U+2210 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00f9, 0x00fb,     -1,     -1,     -1, 0x00ec,     -1,
	/* U+2220 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00ef,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+2230 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1 /* blank */,
	/* U+2240 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00f7,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+2250 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1 /* blank */,
	/* U+2260 */     -1, 0x00f0,     -1,     -1, 0x00f3, 0x00f2,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
};

const int TEncoding_CP437::fromUnicodeTable4[11 * 16] = {
	/* U+2500 */ 0x00c4,     -1, 0x00b3,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00da,     -1,     -1,     -1,
	/* U+2510 */ 0x00bf,     -1,     -1,     -1, 0x00c0,     -1,     -1,     -1, 0x00d9,     -1,     -1,     -1, 0x00c3,     -1,     -1,     -1,
	/* U+2520 */     -1,     -1,     -1,     -1, 0x00b4,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00c2,     -1,     -1,     -1,
	/* U+2530 */     -1,     -1,     -1,     -1, 0x00c1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00c5,     -1,     -1,     -1,
	/* U+2540 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1 /* blank */,
	/* U+2550 */ 0x00cd, 0x00ba, 0x00d5, 0x00d6, 0x00c9, 0x00b8, 0x00b7, 0x00bb, 0x00d4, 0x00d3, 0x00c8, 0x00be, 0x00bd, 0x00bc, 0x00c6, 0x00c7,
	/* U+2560 */ 0x00cc, 0x00b5, 0x00b6, 0x00b9, 0x00d1, 0x00d2, 0x00cb, 0x00cf, 0x00d0, 0x00ca, 0x00d8, 0x00d7, 0x00ce,     -1,     -1,     -1,
	/* U+2570 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1 /* blank */,
	/* U+2580 */ 0x00df,     -1,     -1,     -1, 0x00dc,     -1,     -1,     -1, 0x00db,     -1,     -1,     -1, 0x00dd,     -1,     -1,     -1,
	/* U+2590 */ 0x00de, 0x00b0, 0x00b1, 0x00b2,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+25a0 */ 0x00fe,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1
};
//	/* U+0190 */     -1,     -1, 0x009f,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
//	/* U+2070 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00fc,
//	/* U+20a0 */     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x009e,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
//	/* U+2310 */ 0x00a9,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
//	/* U+2320 */ 0x00f4, 0x00f5,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,

//-----------------------------------------------------------------------------
// CP 852
//-----------------------------------------------------------------------------

const int TEncoding_CP852::toUnicodeTable[8 * 16] = {
	/* 0x80 */ 0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x016f, 0x0107, 0x00e7, 0x0142, 0x00eb, 0x0150, 0x0151, 0x00ee, 0x0179, 0x00c4, 0x0106,
	/* 0x90 */ 0x00c9, 0x0139, 0x013a, 0x00f4, 0x00f6, 0x013d, 0x013e, 0x015a, 0x015b, 0x00d6, 0x00dc, 0x0164, 0x0165, 0x0141, 0x00d7, 0x010d,
	/* 0xa0 */ 0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x0104, 0x0105, 0x017d, 0x017e, 0x0118, 0x0119, 0x00ac, 0x017a, 0x010c, 0x015f, 0x00ab, 0x00bb,
	/* 0xb0 */ 0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x00c1, 0x00c2, 0x011a, 0x015e, 0x2563, 0x2551, 0x2557, 0x255d, 0x017b, 0x017c, 0x2510,
	/* 0xc0 */ 0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x0102, 0x0103, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x00a4,
	/* 0xd0 */ 0x0111, 0x0110, 0x010e, 0x00cb, 0x010f, 0x0147, 0x00cd, 0x00ce, 0x011b, 0x2518, 0x250c, 0x2588, 0x2584, 0x0162, 0x016e, 0x2580,
	/* 0xe0 */ 0x00d3, 0x00df, 0x00d4, 0x0143, 0x0144, 0x0148, 0x0160, 0x0161, 0x0154, 0x00da, 0x0155, 0x0170, 0x00fd, 0x00dd, 0x0163, 0x00b4,
	/* 0xf0 */ 0x00ad, 0x02dd, 0x02db, 0x02c7, 0x02d8, 0x00a7, 0x00f7, 0x00b8, 0x00b0, 0x00a8, 0x02d9, 0x0171, 0x0158, 0x0159, 0x25a0, 0x00a0
};

const int TEncoding_CP852::fromUnicodeTable1[14 * 16] = {
	/* U+00a0 */ 0x00ff,     -1,     -1,     -1, 0x00cf,     -1,     -1, 0x00f5, 0x00f9,     -1,     -1, 0x00ae, 0x00aa, 0x00f0,     -1,     -1,
	/* U+00b0 */ 0x00f8,     -1,     -1,     -1, 0x00ef,     -1,     -1,     -1, 0x00f7,     -1,     -1, 0x00af,     -1,     -1,     -1,     -1,
	/* U+00c0 */     -1, 0x00b5, 0x00b6,     -1, 0x008e,     -1,     -1, 0x0080,     -1, 0x0090,     -1, 0x00d3,     -1, 0x00d6, 0x00d7,     -1,
	/* U+00d0 */     -1,     -1,     -1, 0x00e0, 0x00e2,     -1, 0x0099, 0x009e,     -1,     -1, 0x00e9,     -1, 0x009a, 0x00ed,     -1, 0x00e1,
	/* U+00e0 */     -1, 0x00a0, 0x0083,     -1, 0x0084,     -1,     -1, 0x0087,     -1, 0x0082,     -1, 0x0089,     -1, 0x00a1, 0x008c,     -1,
	/* U+00f0 */     -1,     -1,     -1, 0x00a2, 0x0093,     -1, 0x0094, 0x00f6,     -1,     -1, 0x00a3,     -1, 0x0081, 0x00ec,     -1,     -1,
	/* U+0100 */     -1,     -1, 0x00c6, 0x00c7, 0x00a4, 0x00a5, 0x008f, 0x0086,     -1,     -1,     -1,     -1, 0x00ac, 0x009f, 0x00d2, 0x00d4,
	/* U+0110 */ 0x00d1, 0x00d0,     -1,     -1,     -1,     -1,     -1,     -1, 0x00a8, 0x00a9, 0x00b7, 0x00d8,     -1,     -1,     -1,     -1,
	/* U+0120 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1 /* blank */,
	/* U+0130 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x0091, 0x0092,     -1,     -1, 0x0095, 0x0096,     -1,
	/* U+0140 */     -1, 0x009d, 0x0088, 0x00e3, 0x00e4,     -1,     -1, 0x00d5, 0x00e5,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+0150 */ 0x008a, 0x008b,     -1,     -1, 0x00e8, 0x00ea,     -1,     -1, 0x00fc, 0x00fd, 0x0097, 0x0098,     -1,     -1, 0x00b8, 0x00ad,
	/* U+0160 */ 0x00e6, 0x00e7, 0x00dd, 0x00ee, 0x009b, 0x009c,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00de, 0x0085,
	/* U+0170 */ 0x00eb, 0x00fb,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x008d, 0x00ab, 0x00bd, 0x00be, 0x00a6, 0x00a7,     -1
};

const int TEncoding_CP852::fromUnicodeTable2[2* 16] = {
	/* U+02c0 */     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00f3,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+02d0 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00f4, 0x00fa,     -1, 0x00f2,     -1, 0x00f1,     -1,     -1
};

const int TEncoding_CP852::fromUnicodeTable3[11 * 16] = {
	/* U+2500 */ 0x00c4,     -1, 0x00b3,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00da,     -1,     -1,     -1,
	/* U+2510 */ 0x00bf,     -1,     -1,     -1, 0x00c0,     -1,     -1,     -1, 0x00d9,     -1,     -1,     -1, 0x00c3,     -1,     -1,     -1,
	/* U+2520 */     -1,     -1,     -1,     -1, 0x00b4,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00c2,     -1,     -1,     -1,
	/* U+2530 */     -1,     -1,     -1,     -1, 0x00c1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00c5,     -1,     -1,     -1,
	/* U+2540 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1 /* blank */,
	/* U+2550 */ 0x00cd, 0x00ba,     -1,     -1, 0x00c9,     -1,     -1, 0x00bb,     -1,     -1, 0x00c8,     -1,     -1, 0x00bc,     -1,     -1,
	/* U+2560 */ 0x00cc,     -1,     -1, 0x00b9,     -1,     -1, 0x00cb,     -1,     -1, 0x00ca,     -1,     -1, 0x00ce,     -1,     -1,     -1,
	/* U+2570 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1 /* blank */,
	/* U+2580 */ 0x00df,     -1,     -1,     -1, 0x00dc,     -1,     -1,     -1, 0x00db,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+2590 */     -1, 0x00b0, 0x00b1, 0x00b2,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+25a0 */ 0x00fe,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1
};

//-----------------------------------------------------------------------------
// Windows-1250
//-----------------------------------------------------------------------------

const int TEncoding_CP1250::toUnicodeTable[8 * 16] = {
	/* 0x80 */ 0x20ac,     -1, 0x201a,     -1, 0x201e, 0x2026, 0x2020, 0x2021,     -1, 0x2030, 0x0160, 0x2039, 0x015a, 0x0164, 0x017d, 0x0179,
	/* 0x90 */     -1, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,     -1, 0x2122, 0x0161, 0x203a, 0x015b, 0x0165, 0x017e, 0x017a,
	/* 0xa0 */ 0x00a0, 0x02c7, 0x02d8, 0x0141, 0x00a4, 0x0104, 0x00a6, 0x00a7, 0x00a8, 0x00a9, 0x015e, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x017b,
	/* 0xb0 */ 0x00b0, 0x00b1, 0x02db, 0x0142, 0x00b4, 0x00b5, 0x00b6, 0x00b7, 0x00b8, 0x0105, 0x015f, 0x00bb, 0x013d, 0x02dd, 0x013e, 0x017c,
	/* 0xc0 */ 0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7, 0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e,
	/* 0xd0 */ 0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7, 0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df,
	/* 0xe0 */ 0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7, 0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f,
	/* 0xf0 */ 0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7, 0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9
};

const int TEncoding_CP1250::fromUnicodeTable1[14 * 16] = {
	/* U+00a0 */ 0x00a0,     -1,     -1,     -1, 0x00a4,     -1, 0x00a6, 0x00a7, 0x00a8, 0x00a9,     -1, 0x00ab, 0x00ac, 0x00ad, 0x00ae,     -1,
	/* U+00b0 */ 0x00b0, 0x00b1,     -1,     -1, 0x00b4, 0x00b5, 0x00b6, 0x00b7, 0x00b8,     -1,     -1, 0x00bb,     -1,     -1,     -1,     -1,
	/* U+00c0 */     -1, 0x00c1, 0x00c2,     -1, 0x00c4,     -1,     -1, 0x00c7,     -1, 0x00c9,     -1, 0x00cb,     -1, 0x00cd, 0x00ce,     -1,
	/* U+00d0 */     -1,     -1,     -1, 0x00d3, 0x00d4,     -1, 0x00d6, 0x00d7,     -1,     -1, 0x00da,     -1, 0x00dc, 0x00dd,     -1, 0x00df,
	/* U+00e0 */     -1, 0x00e1, 0x00e2,     -1, 0x00e4,     -1,     -1, 0x00e7,     -1, 0x00e9,     -1, 0x00eb,     -1, 0x00ed, 0x00ee,     -1,
	/* U+00f0 */     -1,     -1,     -1, 0x00f3, 0x00f4,     -1, 0x00f6, 0x00f7,     -1,     -1, 0x00fa,     -1, 0x00fc, 0x00fd,     -1,     -1,
	/* U+0100 */     -1,     -1, 0x00c3, 0x00e3, 0x00a5, 0x00b9, 0x00c6, 0x00e6,     -1,     -1,     -1,     -1, 0x00c8, 0x00e8, 0x00cf, 0x00ef,
	/* U+0110 */ 0x00d0, 0x00f0,     -1,     -1,     -1,     -1,     -1,     -1, 0x00ca, 0x00ea, 0x00cc, 0x00ec,     -1,     -1,     -1,     -1,
	/* U+0120 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1 /* blank */,
	/* U+0130 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00c5, 0x00e5,     -1,     -1, 0x00bc, 0x00be,     -1,
	/* U+0140 */     -1, 0x00a3, 0x00b3, 0x00d1, 0x00f1,     -1,     -1, 0x00d2, 0x00f2,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+0150 */ 0x00d5, 0x00f5,     -1,     -1, 0x00c0, 0x00e0,     -1,     -1, 0x00d8, 0x00f8, 0x008c, 0x009c,     -1,     -1, 0x00aa, 0x00ba,
	/* U+0160 */ 0x008a, 0x009a, 0x00de, 0x00fe, 0x008d, 0x009d,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00d9, 0x00f9,
	/* U+0170 */ 0x00db, 0x00fb,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x008f, 0x009f, 0x00af, 0x00bf, 0x008e, 0x009e,     -1,
};

const int TEncoding_CP1250::fromUnicodeTable2[2 * 16] = {
	/* U+02c0 */     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00a1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+02d0 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x00a2, 0x00ff,     -1, 0x00b2,     -1, 0x00bd,     -1,     -1,
};

const int TEncoding_CP1250::fromUnicodeTable3[3 * 16] = {
	/* U+2010 */     -1,     -1,     -1, 0x0096, 0x0097,     -1,     -1,     -1, 0x0091, 0x0092, 0x0082,     -1, 0x0093, 0x0094, 0x0084,     -1,
	/* U+2020 */ 0x0086, 0x0087, 0x0095,     -1,     -1,     -1, 0x0085,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
	/* U+2030 */ 0x0089,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x008b, 0x009b,     -1,     -1,     -1,     -1,     -1,
};
//	/* U+20a0 */     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1, 0x0080,     -1,     -1,     -1,
//	/* U+2120 */     -1,     -1, 0x0099,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1

//-----------------------------------------------------------------------------
// YU-ASCII
//-----------------------------------------------------------------------------

//                                                C acute c acute C caron c caron S caron s caron Z caron z caron D stroke d stroke
const int TEncoding_YuAscii::uniChars[10] =     {  0x106,  0x107,  0x10c,  0x10d,  0x160,  0x161,  0x17d,  0x17e,   0x110,  0x111  };
const int TEncoding_YuAscii::yuAsciiChars[10] = {   0x5d,   0x7d,   0x5e,   0x7e,   0x5b,   0x7b,   0x40,   0x60,    0x5c,   0x7c  };
//                                                   ']'     '}'     '^'    '~'     '['     '{'     '@'     '`'      '\\'    '|'


//-----------------------------------------------------------------------------
// TUnicode - codec registry
//-----------------------------------------------------------------------------

void TUnicode::InitCodecs()
{
	ClrCodecs();
	RegisterCodec("ISO-8859-1 ISO_8859-1 ISO_8859-1:1987 ISO-IR-100 CP819 IBM819 LATIN1 L1 csISOLatin1 ISO8859-1 ISO8859_1 CP28591", TCodecBase::New<TCodec_ISO8859_1>());
	RegisterCodec("ISO-8859-2 ISO_8859-2 ISO_8859-2:1987 ISO-IR-101 LATIN2 L2 csISOLatin2 ISO8859-2 ISO8859_2 CP28592", TCodecBase::New<TCodec_ISO8859_2>());
	RegisterCodec("ISO-8859-3 ISO_8859-3 ISO_8859-3:1988 ISO-IR-109 LATIN3 L3 csISOLatin3 ISO8859-3 ISO8859_3 CP28593", TCodecBase::New<TCodec_ISO8859_3>());
	RegisterCodec("ISO-8859-4 ISO_8859-4 ISO_8859-4:1988 ISO-IR-110 LATIN4 L4 csISOLatin4 ISO8859-4 ISO8859_4 CP28594", TCodecBase::New<TCodec_ISO8859_4>());
	RegisterCodec("YUASCII YU-ASCII YU_ASCII", TCodecBase::New<TCodec_YuAscii>());
	RegisterCodec("CP1250 Windows-1250 MS-EE", TCodecBase::New<TCodec_CP1250>());
	RegisterCodec("CP852 cp852_DOSLatin2 DOSLatin2", TCodecBase::New<TCodec_CP852>());
	RegisterCodec("CP437 cp437_DOSLatinUS DOSLatinUS", TCodecBase::New<TCodec_CP437>());
}
