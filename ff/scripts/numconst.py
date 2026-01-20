# Helper to get IEEE "exact" constants for things to hard code in C++
#
# This script is not designed for an end user. Use the existing code
# to mess around for computing what you want. Parts of it are turned
# on or off with "if 1" or "if 0" conditions.
#
# To be safe, we may want 8 significant digits for float and 17 for double

import ctypes
import decimal
import re
import struct
from decimal import Decimal
print(f'decimal default precision = {decimal.getcontext().prec}') # 28
DEC_PREC_CALC = 75
decimal.getcontext().prec = DEC_PREC_CALC
print(f'set decimal precision to {decimal.getcontext().prec}')
DEC_PREC_OUT = 50 # number of digits to output for high decimal precision
assert DEC_PREC_OUT + 10 <= decimal.getcontext().prec
print()

constantStr = {
    'pi': '3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066',
    'e': '2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427427466391932003059921817413596629043572900334295260'
}
assert all(re.fullmatch(r'(\d+)\.(\d{PREC,999})'.replace(
    'PREC',str(decimal.getcontext().prec+10)),numstr)
    for numstr in constantStr.values())

# TODO can precision be limited to that specified in the context?
constantDec = { key: Decimal(constantStr[key]) for key in constantStr }

if 1: # print constants
    print('hard coded decimal constants')
    for key in sorted(constantDec.keys()):
        print(f'{key} = {constantDec[key]}')
    print()

incrementDigitStr = {chr(ord('0')+d):chr(ord('0')+(d+1)%10) for d in range(10)}
decrementDigitStr = {chr(ord('0')+d):chr(ord('0')+(d-1)%10) for d in range(10)}

def shortestRoundedFpStr(numStr:str|Decimal,cfpType) -> str:
    # Try removing digits from the string (considering rounding properly)
    # Return the shortest one right before the value would change
    # numStr is the string representation of a python decimal
    # cfpType is a lambda that converts it to a ctype (c_float or c_double)
    if isinstance(numStr,Decimal): numStr = str(numStr)
    debug = False
    numVal = cfpType(numStr)
    if debug: print(f'numVal = {numVal}, len = {len(numStr)}')
    curLen = len(numStr)-1
    lastStr = numStr # shortest str found so far that does not change value
    while True:
        assert curLen > 0 and numStr[curLen-1] in '0123456789' \
            and numStr[curLen] in '0123456789'
        curStr = list(numStr[:curLen])
        if numStr[curLen] in '56789':
            # round up, otherwise round down which requires no additional work
            i = curLen-1
            while True:
                curStr[i] = incrementDigitStr[curStr[i]]
                if curStr[i] == '0': i -= 1
                else: break
        curStr = ''.join(curStr)
        curVal = cfpType(curStr)
        if curVal == numVal: # shorter string has same float value
            lastStr = curStr
            curLen -= 1
        else: return lastStr # value changed, take last one that was the same

def shortestFltStr(fltStr:str|Decimal) -> str:
    conv = lambda vStr: ctypes.c_float(float(vStr)).value
    return shortestRoundedFpStr(fltStr,conv)

def shortestDblStr(dblStr:str|Decimal) -> str:
    conv = lambda vStr: ctypes.c_double(float(vStr)).value
    return shortestRoundedFpStr(dblStr,conv)

def niceDecStr(numStr:str|Decimal) -> str:
    # format a double string nicely to split digits after decimal point into
    # groups of 10
    if isinstance(numStr,Decimal): numStr = str(numStr)
    s1 = numStr[:numStr.find('.')]
    s2 = numStr[len(s1)+1:len(s1)+1+50]
    return f'{s1}.{' '.join(s2[i:i+10] for i in range(0,50,10))} ...'

def dblHex(num:float|Decimal|str) -> str:
    # hex for the float value
    num = float(num)
    bits = struct.unpack('>Q',struct.pack('>d',num))[0]
    bits_sign = bits >> 63
    bits_exp = (bits >> 52) & 0x7ff
    bits_mant = bits & 0xfffffffffffff
    str_sign = '-' if bits_sign else '+'
    int_exp = bits_exp - 1023
    str_mant = f'{bits_mant:013x}'
    result = f'{str_sign}0x1.{str_mant}p{int_exp:+d}'
    assert (result[1:] if result[0] == '+' else result) == num.hex()
    return result

def fltHex(num:float|Decimal|str) -> str:
    # hex for the double value
    num = float(num)
    bits = struct.unpack('>I',struct.pack('>f',num))[0]
    bits_sign = bits >> 31
    bits_exp = (bits >> 23) & 0xff
    bits_mant = bits & 0x7fffff
    str_sign = '-' if bits_sign else '+'
    int_exp = bits_exp - 127
    str_mant = f'{bits_mant<<1:06x}'
    return f'{str_sign}0x1.{str_mant}p{int_exp:+d}f'

if 0: # pi and e
    print(niceDecStr(constantDec['pi']))
    print(fltHex(constantDec['pi']))
    print(dblHex(constantDec['pi']))
    print(shortestFltStr(constantDec['pi']))
    print(shortestDblStr(constantDec['pi']))
    print(niceDecStr(constantDec['e']))
    print(fltHex(constantDec['e']))
    print(dblHex(constantDec['e']))
    print(shortestFltStr(constantDec['e']))
    print(shortestDblStr(constantDec['e']))

outLines = []

if 0: # pi fractions (cNumPiMult)
    fracs = [(1,1),(2,1),(1,2),(3,2),(1,3),(2,3),(4,3),(5,3),(1,4),(3,4),(5,4),(7,4),(1,6),(5,6),(7,6),(11,6)]
    #fracs = [(1,1)]
    pi = constantDec['pi']
    outLines.append(f'template <typename T, int n, int d> static constexpr T cNumPiMult;')
    for n,d in fracs:
        valueDec = n*pi/d
        outLines.append(f'// pi * {n}/{d} = {niceDecStr(valueDec)}')
        outLines.append(f'// bits (IEEE-754): float = {fltHex(valueDec)}, double = {dblHex(valueDec)}')
        outLines.append(f'template <> constexpr float cNumPiMult<float,{n},{d}> = {shortestFltStr(valueDec)}f;')
        outLines.append(f'template <> constexpr double cNumPiMult<double,{n},{d}> = {shortestDblStr(valueDec)};')
    print()

if 0: # inverse pi fractions (cNumPiMult)
    fracs = [(1,1),(2,1),(1,2),(3,2),(1,3),(2,3),(4,3),(5,3),(1,4),(3,4),(5,4),(7,4),(1,6),(5,6),(7,6),(11,6)]
    #fracs = [(1,1)]
    pi = constantDec['pi']
    outLines.append(f'template <typename T, int n, int d> static constexpr T cNumInvPiMult;')
    for n,d in fracs:
        valueDec = d/(n*pi)
        outLines.append(f'// {d}/({n}*pi) = {niceDecStr(valueDec)}')
        outLines.append(f'// bits (IEEE-754): float = {fltHex(valueDec)}, double = {dblHex(valueDec)}')
        outLines.append(f'template <> constexpr float cNumInvPiMult<float,{d},{n}> = {shortestFltStr(valueDec)}f;')
        outLines.append(f'template <> constexpr double cNumInvPiMult<double,{d},{n}> = {shortestDblStr(valueDec)};')
    print()

def calcDecSqrt(n:int) -> Decimal:
    # newton method: x -> x - f(x)/f'(x)
    # for square roots: f(x)=x^2-n so, so x -> (x^2+n)/(2*x)
    debug = False
    assert n > 0
    x = Decimal(1)
    relErr = Decimal(1) / (Decimal(10**(DEC_PREC_CALC-2)))
    if debug: print(relErr)
    i = 0
    if debug: print(i,x)
    while True:
        i += 1
        xx = (x*x + n) / (2*x)
        if debug: print(i,xx)
        if abs(xx-x) / x < relErr: return xx
        else: x = xx

def calcDecCbrt(n:int) -> Decimal:
    # f(x)=x^3-n,f'(x)=3x^2
    # x -> (2x^3+n)/(3x^2)
    debug = False
    assert n > 0
    x = Decimal(1)
    relErr = Decimal(1) / (Decimal(10**(DEC_PREC_CALC-2)))
    if debug: print(relErr)
    i = 0
    if debug: print(i,x)
    while True:
        i += 1
        xx = (2*x*x*x + n) / (3*x*x)
        if debug: print(i,xx)
        if abs(xx-x) / x < relErr: return xx
        else: x = xx

if 0: # square roots (cNumSqrt)
    values = [2,3,5,6,7,8,10,11,12,13,14,15,17,18,19,20,21,22,23,24]
    outLines.append(f'template <typename T, int n> static constexpr T cNumSqrt;')
    for n in values:
        v = calcDecSqrt(n)
        outLines.append(f'// sqrt(n) = {niceDecStr(v)}')
        outLines.append(f'// bits (IEEE-754): float = {fltHex(v)}, double = {dblHex(v)}')
        outLines.append(f'template <> constexpr float cNumSqrt<float,{n}> = {shortestFltStr(v)}f;')
        outLines.append(f'template <> constexpr double cNumSqrt<double,{n}> = {shortestDblStr(v)};')

if 0: # inverse square roots (cNumInvSqrt)
    values = [2,3,5,6,7,8,10,11,12,13,14,15,17,18,19,20,21,22,23,24]
    outLines.append(f'template <typename T, int n> static constexpr T cNumInvSqrt;')
    for n in values:
        v = Decimal(1)/calcDecSqrt(n)
        outLines.append(f'// 1/sqrt(n) = {niceDecStr(v)}')
        outLines.append(f'// bits (IEEE-754): float = {fltHex(v)}, double = {dblHex(v)}')
        outLines.append(f'template <> constexpr float cNumInvSqrt<float,{n}> = {shortestFltStr(v)}f;')
        outLines.append(f'template <> constexpr double cNumInvSqrt<double,{n}> = {shortestDblStr(v)};')

if 0: # cube roots (cNumCbrt)
    values = [2,3,4,5,6,7,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26]
    outLines.append(f'template <typename T, int n> static constexpr T cNumCbrt;')
    for n in values:
        v = calcDecCbrt(n)
        outLines.append(f'// cbrt(n) = {niceDecStr(v)}')
        outLines.append(f'// bits (IEEE-754): float = {fltHex(v)}, double = {dblHex(v)}')
        outLines.append(f'template <> constexpr float cNumCbrt<float,{n}> = {shortestFltStr(v)}f;')
        outLines.append(f'template <> constexpr double cNumCbrt<double,{n}> = {shortestDblStr(v)};')

if 0: # inverse cube roots (cNumInvCbrt)
    values = [2,3,4,5,6,7,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26]
    outLines.append(f'template <typename T, int n> static constexpr T cNumInvCbrt;')
    for n in values:
        v = Decimal(1)/calcDecCbrt(n)
        outLines.append(f'// 1/cbrt(n) = {niceDecStr(v)}')
        outLines.append(f'// bits (IEEE-754): float = {fltHex(v)}, double = {dblHex(v)}')
        outLines.append(f'template <> constexpr float cNumInvCbrt<float,{n}> = {shortestFltStr(v)}f;')
        outLines.append(f'template <> constexpr double cNumInvCbrt<double,{n}> = {shortestDblStr(v)};')

for line in outLines:
    print(line)
