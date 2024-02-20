/*
 Utility.c
 Hap Codec
 
 Copyright (c) 2012-2013, Tom Butterworth and Vidvox LLC. All rights reserved.
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(_WIN32)
#include <ConditionalMacros.h>
#include <ImageCodec.h>
#include <Windows.h>
#endif

#include "hap.h"
#include "Utility.h"
#include "HapCodecSubTypes.h"
#include "PixelFormats.h"

// Utility to add an SInt32 to a CFMutableDictionary.
void addNumberToDictionary( CFMutableDictionaryRef dictionary, CFStringRef key, SInt32 numberSInt32 )
{
	CFNumberRef number = CFNumberCreate( NULL, kCFNumberSInt32Type, &numberSInt32 );
	if( ! number ) 
		return;
	CFDictionaryAddValue( dictionary, key, number );
	CFRelease( number );
}

// Utility to add a double to a CFMutableDictionary.
void addDoubleToDictionary( CFMutableDictionaryRef dictionary, CFStringRef key, double numberDouble )
{
	CFNumberRef number = CFNumberCreate( NULL, kCFNumberDoubleType, &numberDouble );
	if( ! number ) 
		return;
	CFDictionaryAddValue( dictionary, key, number );
	CFRelease( number );
}

// Utility to round up to a multiple of 16.
int roundUpToMultipleOf16( int n )
{
	if( 0 != ( n & 15 ) )
		n = ( n + 15 ) & ~15;
	return n;
}

// Utility to round up to a multiple of 4.
int roundUpToMultipleOf4( int n )
{
	if( 0 != ( n & 3 ) )
		n = ( n + 3 ) & ~3;
	return n;
}

int roundDownToMultipleOf4( int n )
{
    return n & ~3;
}

unsigned long dxtBytesForDimensions(int width, int height, OSType codecSubType)
{
	unsigned int			length = 0;
	switch (codecSubType)	{
	case kHapCodecSubType:
	case kHapAOnlyCodecSubType:
	default:
		length = roundUpToMultipleOf4(width) * roundUpToMultipleOf4(height);
		length /= 2;
		break;
	case kHapAlphaCodecSubType:
	case kHapYCoCgCodecSubType:
	case kHapYCoCgACodecSubType:
		length = roundUpToMultipleOf4(width) * roundUpToMultipleOf4(height);
		break;
	case kHap7AlphaCodecSubType:
	case kHapHDRRGBCodecSubType:
		length = roundUpToMultipleOf4(width) * roundUpToMultipleOf4(height);
		break;
	}
	return length;
	
    //unsigned long length = roundUpToMultipleOf4(width) * roundUpToMultipleOf4(height);
    //if (codecSubType == kHapCodecSubType || codecSubType == kHapAOnlyCodecSubType) length /= 2;
    //return length;
}

#ifndef __LP64__
SInt16 resourceIDForComponentType(OSType componentType, OSType resourceType)
{    
    if (resourceType == codecInfoResourceType)
    {
        switch (componentType) {
            case kHapCodecSubType:
                return 256;	//	0x0100
            case kHapAlphaCodecSubType:
                return 456;	//	0x01C8
            case kHapYCoCgCodecSubType:
                return 356;	//	0x0164
            case kHapYCoCgACodecSubType:
                return 556;	//	0x022C
            case kHap7AlphaCodecSubType:
            	return XXX;	//	0x
            case kHapHDRRGBCodecSubType:
            	return XXX;	//	0x
            default:
                return 0;
        }
    }
    return 0;
}
#endif

#if defined(_WIN32)
int hapCodecMaxTasks()
{
    /*
    Some Adobe products throw an error if they queue more than 10 buffers
    */
    TCHAR executablePath[MAX_PATH + 1];
    if (GetModuleFileName(0, executablePath, MAX_PATH + 1) != 0)
    {
        if (wcsstr(executablePath, L"Adobe") != NULL)
        {
            return 10;
        }
    }
    return 20;
}
// Mac version is in Utility.m
#endif

enum HapTextureFormat HapTextureFormatForCVPixelFormat(OSType inCVPixelFormat)
{
	switch (inCVPixelFormat)	{
	case kHapCVPixelFormat_RGB_DXT1:	return HapTextureFormat_RGB_DXT1;
	case kHapCVPixelFormat_RGBA_DXT5:	return HapTextureFormat_RGBA_DXT5;
	case kHapCVPixelFormat_YCoCg_DXT5:	return HapTextureFormat_YCoCg_DXT5;
	case kHapCVPixelFormat_A_RGTC1:		return HapTextureFormat_A_RGTC1;
	case kHapCVPixelFormat_RGBA_BC7:	return HapTextureFormat_RGBA_BPTC_UNORM;
	case kHapCVPixelFormat_RGB_BC6U:	return HapTextureFormat_RGB_BPTC_UNSIGNED_FLOAT;
	case kHapCVPixelFormat_RGB_BC6S:	return HapTextureFormat_RGB_BPTC_SIGNED_FLOAT;
	}
	return 0x00;
}
OSType CVPixelFormatForHapTextureFormat(enum HapTextureFormat inHapTextureFormat)	{
	switch (inHapTextureFormat)	{
		case HapTextureFormat_RGB_DXT1:		return kHapCVPixelFormat_RGB_DXT1;
		case HapTextureFormat_RGBA_DXT5:	return kHapCVPixelFormat_RGBA_DXT5;
		case HapTextureFormat_YCoCg_DXT5:	return kHapCVPixelFormat_YCoCg_DXT5;
		case HapTextureFormat_A_RGTC1:	return kHapCVPixelFormat_A_RGTC1;
		case HapTextureFormat_RGBA_BPTC_UNORM:	return kHapCVPixelFormat_RGBA_BC7;
		case HapTextureFormat_RGB_BPTC_UNSIGNED_FLOAT:	return kHapCVPixelFormat_RGB_BC6U;
		case HapTextureFormat_RGB_BPTC_SIGNED_FLOAT:	return kHapCVPixelFormat_RGB_BC6S;
	}
	return 0;
}

#if defined(_WIN32) && defined(DEBUG)
void debug_print_s(void *glob, const char *func, const char *s)
{
    char buffer[255];
    if (s)
    {
        _sprintf_p(buffer, sizeof(buffer), "%p %s %s\n", glob, func, s);
    }
    else
    {
        _sprintf_p(buffer, sizeof(buffer), "%p %s\n", glob, func);
    }
    OutputDebugStringA(buffer);
}

void debug_print_i(void *glob, const char *func, int e)
{
    char buffer[255];
    _sprintf_p(buffer, sizeof(buffer), "%p %s %d\n", glob, func, e);
    OutputDebugStringA(buffer);
}
#endif
