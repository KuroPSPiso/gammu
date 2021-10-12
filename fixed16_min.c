#define FULLBITS 16
#define LOWBITS 4
#define HIGHBITS (FULLBITS - LOWBITS)
#define SIGNMASKINT32 0x80000000
#define FULLMASKF16 0xFFFF
#define SIGNMASKF16 0x8000

typedef INT16 fixed16;
const fixed16 fractionMaskF16 = (0xFFFF >> HIGHBITS);

#define SIGNISSET(x) (x < 0)
#define SIGNINT32(x) (x & SIGNMASKINT32)
#define SIGNF16(x) (x & SIGNMASKF16)
#define BASEF16(x) (x & (-1 ^ (0xFFFF >> HIGHBITS)))
#define FRACTIONF16(x) (x & fractionMaskF16)
#define PI 0x0032
#define INT16TOF16(x) (x << LOWBITS) //quick standard int16 to fixed16, whole numbers
#define F16TOINT16(x) (x >> LOWBITS) //quick standard int16 to fixed16, whole numbers

// shouldn't be used frequently
const int intMultiplier = 10000; //update along with lowbits
#define BASEF16TOINT(x) ((BASEF16(x) >> LOWBITS) * intMultiplier) 
#define BOOL unsigned char

INT16 powInt16(INT16 base, UINT8 exp)
{
    if (exp == 0) return 1;
    if (exp == 1) return base;
    INT16 ret = base;
    for (; exp > 1; exp--)
    {
        ret *= base;
    }
    return ret;
}

fixed16 intToF16(int x)
{
    if (x == 0) return 0x0000;

    BOOL sign = SIGNISSET(x); //is negative?

    //base calculation
    INT16 base = 0;
    base = ((x / intMultiplier) << LOWBITS);

    //fraction calculation
    INT16 fracValue = 0;
    UINT8 frac = 0;
    fracValue = (x - ((base >> LOWBITS) * intMultiplier));

    INT16 lowBitMultiplier;
    char loopLowBits;

    if (sign)
    {
        for (loopLowBits = 1; loopLowBits <= LOWBITS && fracValue < 0; loopLowBits++)
        {
            lowBitMultiplier = intMultiplier / powInt16(2, loopLowBits);
            if (fracValue + lowBitMultiplier <= 0)
            {
                fracValue += lowBitMultiplier;
                frac |= (0x01 << (LOWBITS - loopLowBits));
            }
        }

        return base - (frac & fractionMaskF16);
    }
    else
    {
        for (loopLowBits = 1; loopLowBits <= LOWBITS && fracValue > 0; loopLowBits++)
        {
            lowBitMultiplier = intMultiplier / powInt16(2, loopLowBits);
            if (fracValue - lowBitMultiplier >= 0)
            {
                fracValue -= lowBitMultiplier;
                frac |= (0x01 << (LOWBITS - loopLowBits));
            }
        }

        return base + (frac & fractionMaskF16);
    }
}

//fixed16 operators
fixed16 absF16(fixed16 x)
{
    if (!SIGNISSET(x)) return x;
    return 0 - x;
}

fixed16 mulF16(fixed16 x, fixed16 mul)
{
    if (mul == 0 || x == 0) return 0;
    if ((x & fractionMaskF16) == 0 && (mul & fractionMaskF16) == 0) return ((x >> LOWBITS) * (mul >> LOWBITS)) << LOWBITS;
    signed int res = ((signed int)x) * ((signed int)mul);
    if ((res & 0xF) >= ((fractionMaskF16 / 2) + 0x01))
    {
        res &= 0xFFFFFFF0;
        res += 0x10;
    }
    return (res >> 4) & FULLMASKF16;
}

fixed16 divF16(fixed16 x, fixed16 div)
{
    if (div == 0 || x == 0) return x;
    return ((x << (LOWBITS - 1)) / (div >> 1));
}

fixed16 floorF16(fixed16 x)
{
    return (x & (FULLMASKF16 - fractionMaskF16));
}

fixed16 ceilF16(fixed16 x)
{
    if (FRACTIONF16(x) <= 0) return x;

    return (x & (FULLMASKF16 - fractionMaskF16)) + (0x0001 << LOWBITS);
}

fixed16 sinF16(fixed16 x) {

    fixed16 res = 0;
    fixed16 term = x;
    int k = 1;

    while ((res + term) != res) {
        res += term;
        k += 2;
        term = mulF16(term, -x);
        term = mulF16(term, x);
        term = divF16(term, INT16TOF16(k));
        term = divF16(term, INT16TOF16(k - 1));
    }

    return res;
}

fixed16 cosF16(fixed16 x) {
    fixed16 res = 0;
    fixed16 term = 0x10;
    int k = 0;

    while ((res + term) != res) {
        res += term;
        k += 2;
        term = mulF16(term, -0x10);
        term = mulF16(term, x);
        term = mulF16(term, x);
        term = divF16(term, INT16TOF16(k));
        term = divF16(term, INT16TOF16(k - 1));
    }

    return res;
}

/*
fixed16 sinF16(fixed16 x) {
    fixed16 term = x;
    fixed16 res = x;
    unsigned char n;
    for (n = 1; n < 5; n++)
    {
        term = mulF16(term, -x);
        term = mulF16(term, x);
        term = divF16(term, INT16TOF16(2 * n + 1));
        term = divF16(term, INT16TOF16(2 * n));
        res += term;
    }
    return res;
}

double cos(double x)
{
    double sum = 1.0;
    double term = 1.0;

    int n;
    for(n = 1; n <= 3; n++)
    {
        term *= -1 * x * x / (2 * n * (2 * n - 1));
        sum += term;
    }

    return sum;
}
*/