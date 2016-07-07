#ifndef ___BASE64_H___
#define ___BASE64_H___
#include "atlstr.h"

using namespace std;

class CBase64
{
public:
	static bool Encode(const unsigned char *pIn, unsigned long uInLen, CStringA& strOut);
	static bool Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen);
	static bool Decode(const CStringA& strIn, unsigned char *pOut, unsigned long *uOutLen);

	static CStringA Encode(CStringA data);
	static CStringA Decode(CStringA data);
	static int GetDecodeBufferLength(CStringA data);
};

#endif // ___BASE64_H___
