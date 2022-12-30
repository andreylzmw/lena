/*
+---------------------------------------+
| Reading JPEG File Metadata 1994, 1997 |
+---------------------------------------+
*/

/* Run - make jpeg && ./jpeg && python3 openimage.py */

// #define debug

#ifdef debug
	#define debug(...) printf(__VA_ARGS__)
#else
	#define debug(...)
#endif

#define M_PI 3.14159265358979323846


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

typedef unsigned char byte;

typedef enum { false, true } bool;

#define null "null"

/* color models */
#define RGB  "RGB"
#define CMYK "CMYK"
#define Gray "Gray"

/* color spaces */
#define sRGB "sRGB"

typedef struct {
	u64 size;   /* in bytes */
	char *path; /* Absolute path */
	u64 width;
	u64 height;
	char *color_space;
	char *color_model;
	u64 bits_per_sample;
	byte *data; /* decoded data width*height sized array */
} JPEG;

/*
If a 0xFF byte occurs in the compressed image data either a zero byte (0x00) or a
marker identifier follows it. Not only marker that should be found once the image
data is started is an EOI. When a 0xFF byte is found followed 1 byte (0x00) the
zero byte must be discarded.
*/

#define nul    0x00 /* JPEG reserved */
#define TEM    0x01 /* Temporary marker, arithmetic coding */
#define RESn   0x02 /* Reserved 0x02 - 0xBF */
#define SOF0   0xC0 /* Start Of Frame 0, baseline jpeg */
#define SOF1   0xC1 /* Start Of Frame 1, extended sequential, huffman */
#define SOF2   0xC2 /* Start Of Frame 2, progressive, huffman */
#define SOF3   0xC3 /* Start Of Frame 3, lossless, huffman */
#define DHT    0xC4 /* Define Huffman Table */
#define SOF5   0xC5 /* Start Of Frame 5, differential sequential, huffman */
#define SOF6   0xC6 /* Start Of Frame 6, differential progressive, huffman */
#define SOF7   0xC7 /* Start Of Frame 7, differential lossless, huffman */
#define JPG    0xC8 /* JPEG Extensions */
#define SOF9   0xC9 /* Start Of Frame 9, extended sequential, arithmetic */
#define SOF10  0xCA /* Start Of Frame 10, progressive, arithmetic */
#define SOF11  0xCB /* Start Of Frame 11, lossless, arithmetic */
#define DAC    0xCC /* Define Arithmetic Coding */
#define SOF13  0xCD /* Start Of Frame 13, differential sequential, arithmetic */
#define SOF14  0xCE /* Start Of Frame 14, differential progressive, arithmetic */
#define SOF15  0xCF /* Start Of Frame 15, differential lossless, arithmetic */
#define RST0   0xD0 /* Restart Marker 0 */
#define RST1   0xD1 /* Restart Marker 1 */
#define RST2   0xD2 /* Restart Marker 2 */
#define RST3   0xD3 /* Restart Marker 3 */
#define RST4   0xD4 /* Restart Marker 4 */
#define RST5   0xD5 /* Restart Marker 5 */
#define RST6   0xD6 /* Restart Marker 6 */
#define RST7   0xD7 /* Restart Marker 7 */
#define SOI    0xD8 /* Start Of image */
#define EOI    0xD9 /* End Of Image */
#define SOS    0xDA /* Start Of Scan */
#define DQT    0xDB /* Define Quantizaion Table */
#define DNL    0xDC /* Define Number of Lines */
#define DRI    0xDD /* Define Restart Interval */
#define DHP    0xDE /* Define Hierarchical Progression */
#define EXP    0xDF /* Expand Reference Component */
#define APP0   0xE0 /* Application Segment 0, JFIF, JFXX, CIFF, Ocad, AVI1 */
#define APP1   0xE1 /* Application Segment 1, EXIF, ExtendedXMP, QVCI, FLIR, RawThermalImage, XMP, Adobe XMP */
#define APP2   0xE2 /* Application Segment 2, ICC color profile, FPXR, MPF, PreviewImage */
#define APP3   0xE3 /* Application Segment 3, JPS, Meta, Stim, ThermalData, PreviewImage */
#define APP4   0xE4 /* Application Segment 4, Scalado, FPXR, ThermalParams, PreviewImage */
#define APP5   0xE5 /* Application Segment 5, RMETA, SamsungUniqueID, ThermalCalibration, PreviewImage */
#define APP6   0xE6 /* Application Segment 6, EPPIM, NITF, HP_TDHD, GoPro, DJI_DTAT */
#define APP7   0xE7 /* Application Segment 7, Pentax, Huawei, Qualcomm */
#define APP8   0xE8 /* Application Segment 8, SPIFF */
#define APP9   0xE9 /* Application Segment 9, MediaJukebox */
#define APP10  0xEA /* Application Segment 10, ActiveObject, PhotoStudio */
#define APP11  0xEB /* Application Segment 11, JPEG-HDR, JUMBF */
#define APP12  0xEC /* Application Segment 12, Picture Info (older digicams), Photoshop Save for Web: Ducky */
#define APP13  0xED /* Application Segment 13, Photoshop Save As: IRB, 8BIM, IPTC */
#define APP14  0xEE /* Application Segment 14, Adobe */
#define APP15  0xEF /* Application Segment 15, GraphicConverter */
#define JPG0   0xF0 /* JPEG Extension 0 */
#define JPG1   0xF1 /* JPEG Extension 1 */
#define JPG2   0xF2 /* JPEG Extension 2 */
#define JPG3   0xF3 /* JPEG Extension 3 */
#define JPG4   0xF4 /* JPEG Extension 4 */
#define JPG5   0xF5 /* JPEG Extension 5 */
#define JPG6   0xF6 /* JPEG Extension 6 */
#define JPG7   0xF7 /* JPEG Extension 7 */
#define JPG8   0xF8 /* JPEG Extension 8 */
#define JPG9   0xF9 /* JPEG Extension 9 */
#define JPG10  0xFA /* JPEG Extension 10 */
#define JPG11  0xFB /* JPEG Extension 11 */
#define JPG12  0xFC /* JPEG Extension 12 */
#define JPG13  0xFD /* JPEG Extension 13 */
#define COM    0xFE /* Comment */
#define marker 0xFF /* Segment */

/* Exif IFD tags */
#define EXIF_WIDTH       256
#define EXIF_HEIGHT      257
#define EXIF_BPS         258
#define EXIF_COLOR_SPACE 40961

#define EXIF_PIXELXD     40962
#define EXIF_PIXELYD     40963
#define EXIF_ZOOM_RATIO  41988

/* Interoperability IFD tags */

#define exifPointer  34665
#define interPointer 40965
#define gpsPointer   34853

/* ICC profile tags */
#define ICC_RGB  0x52474220
#define ICC_CMYK 0x434D594B
#define ICC_GRAY 0x47524159

/*
SOF0 (Start Of Frame 0) marker:
+----------------------+---------+---------------------------------------------------+
| Field                | Size    | Description                                       |
+----------------------+---------+---------------------------------------------------+
| Marker Identifier    | 2 bytes | 0xFF, 0xC0 to identify SOF0 marker                |
| Length               | 2 bytes | This value equals to 8 + components*3 value       |
| Data precision       | 1 byte  | This is in bits/sample, usually 8                 |
| Image height         | 2 bytes | This must be > 0                                  |
| Image width          | 2 bytes | This must be > 0                                  |
| Number of components | 1 byte  | 1 = grey scaled, 3 = color YCbCr or YIQ,          |
|                      |         | 4 = color CMYK                                    |
| Each component       | 3 bytes | Read each component data of 3 bytes. It contains, |
|                      |         | (component Id (1 byte) (1 = Y, 2 = Cb, 3 = Cr,    |
|                      |         | 4 = I, 5 = Q), sampling factors (1 byte)          |
|                      |         | (bit 0-3 vertical, 4-7 horizontal),               |
|                      |         | quantization table number (1 byte)))              |
+----------------------+---------+---------------------------------------------------+

APP0 (JFIF segment marker) marker:
-----------------------------------------------------------------------------------
| Field                   | Size    | Description                                 |
-----------------------------------------------------------------------------------
| Marker Identifier       | 2 bytes | 0xFF, 0xE0 to identify APP0 marker          |
| Length                  | 2 bytes | It must be >= 16                            |
| File Identifier Mark    | 5 bytes | This identifies JFIF                        |
| Major revision number   | 1 byte  | Should be 1, otherwise error                |
| Minor revision number   | 1 byte  | Should be 0..2, otherwise try               |
| Units for x/y densities | 1 byte  | 0 = no units, x/y - density specify ratio   |
|                         |         | instead 1 = x/y - density are dots/inch     |
|                         |         | 2 = x/y - density are dots/cm               |
| X-density               | 2 bytes | It should be <> 0                           |
| Y-density               | 2 bytes | It should be <> 0                           |
| Thumbnail width         | 1 byte  |                                             |
| Thumbnail height        | 1 byte  |                                             |
| Bytes to be read        | n bytes | For thumbnail (RGB 24 bit),                 |
|                         |         | n = width*height*3                          |
-----------------------------------------------------------------------------------

Remarks:
	- If there's no 'JFIF', or the length is < 16, the it is probably not a JFIF
	segment and should be ingored
	- Normally units=0, x-dens=1, y-dens=1, meaning that the aspect ratio is 1:1
	(evenly scaled)
	- JFIF files including thumbnails are very rare, the thumbnail can usually
	be ignored. If there's no, then width=0 and height=0. If the length doesn't
	match the thumbnail size, there is warning.

DHT (Define Huffman Table) marker:
--------------------------------------------------------------------------------------
| Field                | Size     | Description                                      |
--------------------------------------------------------------------------------------
| Marker Identifier    | 2 bytes  | 0xFF, 0xC4 to identify DHT marker                |
| Length               | 2 bytes  | This specify length of Huffman table             |
| HT information       | 1 byte   | bit 0..3: number of HT                           |
|                      |          | bit 4: type of HT, 0=DC table, 1=AC table        |
|                      |          | bit 5..7: not used, must be 0                    |
| Number of Symbols    | 16 bytes | Number of symbols with codes of length 1..16,    |
|                      |          | the sum(n) of these bytes is the total number    |
|                      |          | of codes, which must be <= 256                   |
| Symbols              | n bytes  | Table containing the symbols in order of         |
|                      |          | increasing code length (n=total number of codes) |
--------------------------------------------------------------------------------------

Remarks:
A single DHT segment may contain multiple HTs, each with its own information byte.

DRI (Define Restart Interval) marker:
------------------------------------------------------------------------------------
| Field                | Size    | Description                                     |
------------------------------------------------------------------------------------
| Marker Identifier    | 2 bytes | 0xFF, 0xDD identifies DRI marker                |
| Length               | 2 bytes | It must be 4                                    |
| Restart interval     | 2 bytes | This is units of MCU blocks, means that every   |
|                      |         | n MCU blocks a RSTn marker can be found. The    |
|                      |         | first marker will be RST0, then RST1 etc, after |
|                      |         | RST7 repeating from RST0                        |
------------------------------------------------------------------------------------

DQT (Define Quantization Table) marker:
-------------------------------------------------------------------------------------
| Field                | Size    | Description                                      |
-------------------------------------------------------------------------------------
| Marker Identifier    | 2 bytes | 0xFF, 0xDB identifies DQT                        |
| Length               | 2 bytes | This gives the length of QT                      |
| QT information       | 1 byte  | bit 0..3: number of QT                           |
|                      |         | bit 4..7: precision of QT, 0=8 bit, otherwise 16 |
| Bytes                | n bytes | This gives QT values, n = 64*(precision+1)       |
-------------------------------------------------------------------------------------

SOS (Start Of Scan) marker:
-------------------------------------------------------------------------------------
| Field                | Size    | Description                                      |
-------------------------------------------------------------------------------------
| Marker Identifier    | 2 bytes | 0xFF, 0xDA identify SOS marker                   |
| Length               | 2 bytes | This must be equal to 6+2*(number of components) |
| Number of components | 1 byte  | This must be >= 1 and <= 4, usually 1 or 3       |
| Each component       | 2 bytes | For each component, read 2 bytes. It contains,   |
|                      | 1 byte  | Component Id (1=Y, 2=Cb, 3=Cr, 4=I, 5=Q)         |
|                      | 1 byte  | Huffman table to use: bit 0..3: AC table         |
|                      |         | bit 4..7: DC table                               |
| Ignorable Bytes      | 3 bytes | We have to skip 3 bytes                          |
-------------------------------------------------------------------------------------

Remarks:
The image data (scans) is immediately following the SOS segment.

APP1 (EXIF segment marker) marker:
+-------------------------------------------------------------------------------
| Field                | Size    | Description                                 |
+------------------------------------------------------------------------------+
| Marker Identifier    | 2 bytes | 0xFF, 0xE1 to identify APP1 marker          |
| Length               | 2 bytes |                                             |
| File Identifier Mark | 6 bytes | This identifies EXIF                        |
| TIFF Header          | 8 bytes |                                             |
| 0th IFD              |   var   |                                             |
| Exif IFD             |   var   |                                             |
| Interoperability IFD |   var   |                                             |
| GPS IFD              |   var   |                                             |
| 1st IFD              |   var   |                                             |
+--------------------------------------------------------------------------------

TIFF Header:
------------------------------------------------------------------------------------------------
| Field                   | Size    | Description                                              |
------------------------------------------------------------------------------------------------
| Byte order              | 2 bytes | 0x49 0x49 - little endian, 0x4D, 0x4D - big endian       |
| TIFF ID                 | 2 bytes | Always 0x00, 0x2A                                        |
| Offset to 0th IFD       | 4 bytes | From start of the TIFF header                            |
------------------------------------------------------------------------------------------------

0th IFD:
+-------------------------+------------------+---------------------------------------+
| Field                   | Size             | Description                           |
+-------------------------+------------------+---------------------------------------+
| Field Count             | 2 bytes          |                                       |
| IFD Fields              | count * 12 bytes | Contains Offsets to Exif and GPS IFDs |
| Offset to 0th IFD       | 4 bytes          | From start of the TIFF header         |
| Field Data              |       var        | Datra that does not fit in 4 bytes    |
+-------------------------+------------------+---------------------------------------+

Exif IFD:
+-------------------------+------------------+-----------------------------------------+
| Field                   | Size             | Description                             |
+-------------------------+------------------+-----------------------------------------+
| Field Count             | 2 bytes          |                                         |
| IFD Fields              | count * 12 bytes | Contains offset to Interoperability IFD |
| Offset to 1st IFD       | 4 bytes          | From start of the TIFF header           |
| Field Data              |       var        | Datra that does not fit in 4 bytes      |
+-------------------------+------------------+-----------------------------------------+

Interoperability IFD:
+-------------------------+------------------+------------------------------------+
| Field                   | Size             | Description                        |
+-------------------------+------------------+------------------------------------+
| Field Count             | 2 bytes          |                                    |
| IFD Fields              | count * 12 bytes |                                    |
| Offset to 1st IFD       | 4 bytes          | From start of the TIFF header      |
| Field Data              |       var        | Datra that does not fit in 4 bytes |
+-------------------------+------------------+------------------------------------+

GPS IFD:
+-------------------------+------------------+------------------------------------+
| Field                   | Size             | Description                        |
+-------------------------+------------------+------------------------------------+
| Field Count             | 2 bytes          |                                    |
| IFD Fields              | count * 12 bytes |                                    |
| Offset to 1st IFD       | 4 bytes          | From start of the TIFF header      |
| Field Data              |       var        | Datra that does not fit in 4 bytes |
+-------------------------+------------------+------------------------------------+

1st IFD:
+-------------------------+------------------+----------------------------------------+
| Field                   | Size             | Description                            |
+-------------------------+------------------+----------------------------------------+
| Field Count             | 2 bytes          |                                        |
| IFD Fields              | count * 12 bytes | Offset and size of thumbnail data      |
| Offset to 1st IFD       | 4 bytes          | From start of the TIFF header          |
| Field Data              |       var        | Data that does not fit in 4 bytes      |
| Thumbnail Image Data    |       var        |                                        |
+-------------------------+------------------+----------------------------------------+

IFD Field:
+-------------------------+---------+-----------------------------------------------------------+
| Field                   | Size    | Description                                               |
+-------------------------+---------+-----------------------------------------------------------+
| Tag ID                  | 2 bytes | Tag IDs are not unique across IFDs                        |
| Type ID                 | 2 bytes | 1 - Byte                                                  |
|                         |         | 2 - ASCII (byte array)                                    |
|                         |         | 3 - Short (uint16)                                        |
|                         |         | 4 - Long (uint32)                                         |
|                         |         | 5 - Rational (2 * uint32)                                 |
|                         |         | 7 - Undefined (byte array)                                |
|                         |         | 9 - SLong (int32)                                         |
|                         |         | 10 - SRational (2 * int32)                                |
| Count or Components     | 4 bytes | Byte array - count, other - number of components          |
| Value or Offset To Data | 4 bytes | If field data exceeds 4 bytes - offfset, otherwise - data |
+-------------------------+---------+-----------------------------------------------------------+

Remark:
All offsets from start of TIFF header

ICC Profile:
+-------------------------+-------------------+
| Field                   | Size              |
+-------------------------+-------------------+
| Profile header          | 128 bytes         |
| Tag table               | 4 + 12 * each tag |
| Tagged element data     |        var        |
+-------------------------+-------------------+

ICC Profile header fields:
+----------------------------+----------+-------------------------------------------------+
| Field                      | Size     | Description                                     |
+----------------------------+----------+-------------------------------------------------+
| Profile size               | 4 bytes  |                                                 |
| CMM type                   | 4 bytes  | Preferred CMM type                              |
| Profile version            | 4 bytes  |                                                 |
| Profile/Device class       | 4 bytes  |                                                 |
| Colour space of data       | 4 bytes  | Possibly a derived space                        |
| PCS                        | 4 bytes  |                                                 |
| Data and time              | 12 bytes | Then this profile was first created             |
| 'acsp'                     | 4 bytes  | Profile file signature                          |
| Primary platform signature | 4 bytes  |                                                 |
| Profile flags              | 4 bytes  | To indicate various options for the CMM         |
| Device manufacturer        | 4 bytes  | Of the device for which this profile is created |
| Device attributes          | 8 bytes  | Unique to the particular device setup           |
| Rendering Intent           | 4 bytes  |                                                 |
| The nCIEXYZ values         | 12 bytes | Of the illuminant of the PCS                    |
| Profile creator signature  | 4 bytes  |                                                 |
| Profile ID                 | 16 bytes |                                                 |
| Reserved                   | 28 bytes | For future expansion and shall be set to zero   |
+----------------------------+----------+-------------------------------------------------+

Remark:
All fields following each other, all big endian encoded

Data colour space signatures:
+-------------------+--------------+
| Colour space type | Hex encoding |
+-------------------+--------------+
| nCIEXYZ or PCSXYZ | 58595A20     |
| CIELAB or PCSLAB  | 4C616220     |
| CIELUV            | 4C757620     |
| YCbCr             | 59436272     |
| CIEYxy            | 59787920     |
| RGB               | 52474220     |
| Gray              | 47524159     |
| HSV               | 48535620     |
| HLS               | 484C5320     |
| CMYK              | 434D594B     |
| CMY               | 434D5920     |
| 2 colour          | 32434C52     |
| 3 colour          | 33434C52     |
| 4 colour          | 34434C52     |
| 5 colour          | 35434C52     |
| 6 colour          | 36434C52     |
| 7 colour          | 37434C52     |
| 8 colour          | 38434C52     |
| 9 colour          | 39434C52     |
| 10 colour         | 41434C52     |
| 11 colour         | 42434C52     |
| 12 colour         | 43434C52     |
| 13 colour         | 44434C52     |
| 14 colour         | 45434C52     |
| 15 colour         | 46434C52     |
+-------------------+--------------+

Colour encoding types:
nCIEXYZ -> CIELAB, PCSXYZ, PCSLAB, CIELUV, CIEYxy
GRAY
RGB -> HLS, HSV, YCbCr
CMY
CMYK

*/

void setij(int *indexi, int *indexj, int a, int m[8][8]) {

	if ((*indexi) >= 8 || (*indexj) >= 8) {
		debug("!!!\n");
		exit(0);
	}

	m[(*indexi)][(*indexj)] = a;

	if ((*indexi) % 2 == 0) {
		if ((*indexj) % 2 == 0) {
			if ((*indexi) == 0 || (*indexi) == 7) (*indexj) += 1;
			else {
				(*indexi) -= 1;
				(*indexj) += 1;
			}
		} else {
			(*indexi) += 1;
			(*indexj) -= 1;
		}
	} else {
		if ((*indexj) % 2 == 0) {
			if ((*indexi) == 7) (*indexj) += 1;
			else if ((*indexj) == 0 || (*indexj) == 7) (*indexi) += 1;
			else {
				(*indexi) += 1;
				(*indexj) -= 1;
			}
		} else {
			if ((*indexj) == 7) (*indexi) += 1;
			else {
				(*indexi) -= 1;
				(*indexj) += 1;
			}
		}
	}
}

int base10to2LEN(int a) {
	if (a == 0) return 1;
	int res = 0;
	while (a != 0) {
		a /= 2;
		res++;
	}
	return res;
}

void base10to2(char res[], int a, int l) {
	int i = l-1;
	for (int j = 0; j < l; j++) res[j] = '0';
	while (a != 0) {
		res[i] = (a % 2) + '0';
		a /= 2;
		i--;
	}
}

int base2to10(char *s, int l, bool r) {
	int res = 0;

	if (r) {
		int j = 0;
		for (int i = l; i >= 0; i--) {
			int a = s[i] - '0';
			res += a * pow(2, j);
			j++;
		}
	} else {
		int j = l-1;
		for (int i = 0; i < l; i++) {
			int a = s[i] - '0';
			res += a * pow(2, j);
			j--;
		}
	}

	return res;
}

char read_bit(byte data, int i) {
	return ((data >> i) & 0x01) + '0';
}

void dec2hex(char res[2], int a) {

	int t1 = a / 16;
	int t2 = a % 16;

	res[0] = t1 + '0';
	res[1] = t2 + '0';
}

int read_bytes_le(const byte *data, int *i, int count) {
	int res = data[*i];
	int k = 8;
	for (int j = 1; j < count; j++) {
		res |= data[*i+j]<<k;
		k += 8;
	}
	*i += count;
	return res;
}

int read_bytes_be(const byte *data, int *i, int count) {
	int res = data[*i+count-1];
	int k = 8;
	for (int j = count-2; j >= 0; j--) {
		res |= data[*i+j]<<k;
		k += 8;
	}
	*i += count;
	return res;
}

int bin_to_int(char *bin, int l) {
	int res = 0;
	int j = 0;
	for (int i = l-1; i >= 0; i--) {
		int b = bin[j] - '0';
		res += b * pow(2, i);
		j++;
	}
	return res;
}

double alpha(int x) { return (x == 0) ? 1 / sqrt(2) : 1; }

void DCT(int m[8][8]) {
	int t[8][8];
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			t[i][j] = m[i][j];
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			double sum = 0;
			for (int u = 0; u < 8; u++)
				for (int v = 0; v < 8; v++)
					sum += alpha(u) * alpha(v) * t[u][v] * cos(((2*x + 1) * M_PI * u) / 16) * cos(((2*y + 1) * M_PI * v) / 16);
			sum /= 4;

			m[x][y] = sum;
		}
	}
}

int *YCbCr2RGB(int Y, int Cb, int Cr) {

	debug("Y = %d Cb = %d Cr = %d\n", Y, Cb, Cr);

	float R = Cr * 1.402 + Y;
	float B = Cb * 1.772 + Y;
    float G = (Y - 0.114 * B - 0.299 * R) / 0.587;
   
    R += 128;
    G += 128;
    B += 128;

	R = fmin(fmax(0.0, (float)R), 255.0);
	G = fmin(fmax(0.0, (float)G), 255.0);
	B = fmin(fmax(0.0, (float)B), 255.0);

	int *res = malloc(3 * sizeof(int));
	res[0] = R;
	res[1] = G;
	res[2] = B;
	return res;
}

int readDC(int datai, int dd, char image_data[]) {
	char DC[dd];
	debug("dd = %d\n", dd);
	for (int h = 0; h < dd; h++)
		DC[h] = image_data[datai+h];
	debug("DC = ");
	for (int h = 0; h < dd; h++)
		debug("%c", DC[h]);
	debug("\n");
	int value = base2to10(DC, dd, false);
	if (DC[0] == '0') value = value - pow(2, dd) + 1;
	return value;
}

int TIFFi = 0;

int scale = 0;

// 11111111111 111110 100

int exifOffset  = -1;
int interOffset = -1;
int gpsOffset   = -1;
int stOffset    = -1;
int zoom = 0;
void IFDFields(byte *data, int *i, bool encoding, JPEG *jpeg) {
	int len = (encoding == true) ? read_bytes_be(data, i, 2) : read_bytes_le(data, i, 2);

	debug("* len = %d\n", len);
	for (int j = 0; j < len; j++) {
		debug("----------------------------\n");

		int tagID = (encoding == true) ? read_bytes_be(data, i, 2) : read_bytes_le(data, i, 2);
		debug("    * tagID = %d\n", tagID);

		int typeID = (encoding == true) ? read_bytes_be(data, i, 2) : read_bytes_le(data, i, 2);
		debug("    * typeID = %d\n", typeID);

		int count = (encoding == true) ? read_bytes_be(data, i, 4) : read_bytes_le(data, i, 4);
		debug("    * count = %d\n", count);

		int size = 0;
		switch (typeID) {
		case 1:
			debug("    * Byte\n");
			size = count;
			break;
		case 2:
			debug("    * ACII (byte array)\n");
			size = count;
			break;
		case 3:
			debug("    * Short (uint16)\n");
			size = 2 * count;
			break;
		case 4:
			debug("    * Long (uint32)\n");
			size = 4 * count;
			break;
		case 5:
			debug("    * Rational (2*uint32)\n");
			size = 8 * count;
			break;
		case 7:
			debug("    * Undefined (byte array)\n");
			size = count;
			break;
		case 9:
			debug("    * SLong (int32)\n");
			size = 4 * count;
			break;
		case 10:
			debug("    * SRational (Two SLongs)\n");
			size = 8 * count;
			break;
		default:
			debug("Error in typeID %d\n", typeID);
			exit(0);
			break;
		}
		debug("    * size = %d\n", size);
		int offset = 0;
		int t = 0;
		if (size > 4) {
			offset = (encoding == true) ? read_bytes_be(data, i, 4) : read_bytes_le(data, i, 4);
			debug("    * offset = %d\n", offset);
			*i -= (4 + 4 + 2 + 2 + j * 12 + 2 + 8);
			*i += offset;
		}
		debug("    * value = ");
		switch (typeID) {
		case 1:
			t = 4 - count;
			for (int index = 0; index < count; index++) {
				byte value = (encoding == true) ? read_bytes_be(data, i, 1) : read_bytes_le(data, i, 1);
				debug("%x ", value);
			}
			debug("\n");
			break;
		case 2:
			t = 4 - count;
			for (int index = 0; index < count; index++) {
				byte value = (encoding == true) ? read_bytes_be(data, i, 1) : read_bytes_le(data, i, 1);
				/* May be broken if tagIDs are not unique between IFDs */
				debug("%c", value);
			}
			debug("\n");
			break;
		case 3:
			t = 4 - count * 2;
			u32 color_space;
			for (int index = 0; index < count; index++) {
				u16 value = (encoding == true) ? read_bytes_be(data, i, 2) : read_bytes_le(data, i, 2);

				if (tagID == EXIF_PIXELXD && value != 0) { jpeg->width = value; }
				if (tagID == EXIF_PIXELYD && value != 0) { jpeg->height = value; }

				if (tagID == EXIF_WIDTH && value != 0) { jpeg->width = value; }
				if (tagID == EXIF_HEIGHT && value != 0) { jpeg->height = value; }
				if (tagID == EXIF_BPS) (*jpeg).bits_per_sample = value;
				if (tagID == EXIF_COLOR_SPACE) {
					if (value != 0) color_space = value;
				}
				debug("%hu ", value);
			}
			if (color_space == 1) jpeg->color_space = sRGB;
			if (color_space == 65535) jpeg->color_space = null;
			debug("\n");
			break;
		case 4:
			t = 4 - count * 4;
			for (int index = 0; index < count; index++) {
				u32 value = (encoding == true) ? read_bytes_be(data, i, 4) : read_bytes_le(data, i, 4);

				if (tagID == EXIF_PIXELXD && value != 0) { jpeg->width = value; }
				if (tagID == EXIF_PIXELYD && value != 0) { jpeg->height = value; }

				if (tagID == EXIF_WIDTH && value != 0) { jpeg->width = value; }
				if (tagID == EXIF_HEIGHT && value != 0) { jpeg->height = value; }

				if (tagID == exifPointer) exifOffset = value;
				else if (tagID == gpsPointer) gpsOffset = value;
				else if (tagID == interPointer) interOffset = value;

				debug("%u ", value);
			}
			debug("\n");
			break;
		case 5:
			t = 4 - count * 8;
			for (int index = 0; index < count; index++) {
				u32 value1 = (encoding == true) ? read_bytes_be(data, i, 4) : read_bytes_le(data, i, 4);
				u32 value2 = (encoding == true) ? read_bytes_be(data, i, 4) : read_bytes_le(data, i, 4);
				if (tagID == EXIF_ZOOM_RATIO) {
					zoom = value1;
				}
				debug("%u %u", value1, value2);
			}
			debug("\n");
			break;
		case 7:
			t = 4 - count * 1;
			for (int index = 0; index < count; index++) {
				byte value = (encoding == true) ? read_bytes_be(data, i, 1) : read_bytes_le(data, i, 1);
				debug("%c", value);
			}
			debug("\n");
			break;
		case 9:
			t = 4 - count * 4;
			for (int index = 0; index < count; index++) {
				s32 value = (encoding == true) ? read_bytes_be(data, i, 4) : read_bytes_le(data, i, 4);
				debug("%d ", value);
			}
			debug("\n");
			break;
		case 10:
			t = 4 - count * 8;
			for (int index = 0; index < count; index++) {
				s32 value1 = (encoding == true) ? read_bytes_be(data, i, 4) : read_bytes_le(data, i, 4);
				s32 value2 = (encoding == true) ? read_bytes_be(data, i, 4) : read_bytes_le(data, i, 4);
				debug("%d %d", value1, value2);
			}
			debug("\n");
			break;
		default:
			debug("Error in typeID %d\n", typeID);
			exit(0);
			break;
		}

		if (size > 4) {
			*i -= (offset + size);
			*i += (4 + 4 + 2 + 2 + j * 12 + 2 + 8);
		} else {
			*i += t;
		}
	}

	stOffset = (encoding == true) ? read_bytes_be(data, i, 4) : read_bytes_le(data, i, 4);

}
int restart_interval = 0;
void read8x8(int a, int mn, int m[8][8], int DC_id, int AC_id, char image_data[], int *datai, int *huffmantable, char *huffcodes, int k, int index) {

	debug("part of image_data\n");
	for (int j = 0; j < 20; j++) {
		debug("%c", image_data[*datai+j]);

	}
	debug("\n");

	for (int p = 0; p < 8; p++)
		for (int q = 0; q < 8; q++)
			m[p][q] = 0;

	int indexi = 0;
	int indexj = 1;

	bool findDC = false;
	
	if (restart_interval != 0 && a % restart_interval == 1 && mn == 1) {
		while ((*datai) % 8 != 0) (*datai)++;
		// exit(0);
	}

	int entered = 0;
	
	while (*datai < (k-1)*8) {
		bool e = false;
		int f = 0;
		int fcod = 0;

		while (f < index) {
			int n = huffmantable[f];
			char tmp[2];
			dec2hex(tmp, n);

			int n_class = tmp[0] - '0';
			int n_id = tmp[1] - '0';

			int l = huffmantable[f+1];
			char c[l];

			for (int p = fcod; p < fcod + l; p++)
				c[p-fcod] = huffcodes[p];
			
			
			if (n_id != DC_id && n_id != AC_id) {f += 3; fcod += l; continue;}
			if (findDC && n_class == 0) { f += 3; fcod += l; continue; }
			if (!findDC && n_class != 0) { f += 3; fcod += l; continue; }

			// debug("n_id = %d\n", n_id);

			int t = *datai;
			int r = 0;
			while (r < l) {
				if (c[r] == image_data[t]) {
					t++;
					r++;
				} else break;
			}
			if (r != l) {f += 3; fcod += l; continue;}

			char d[2];
			dec2hex(d, huffmantable[f+2]);
			int dd = huffmantable[f+2];

			if (n_class == 0) {
				/* DC */
				debug("find DC\n");
				debug("code = ");
				for (int p = 0; p < l; p++)
					debug("%c", c[p]);
				debug("\n");
				findDC = true;
				entered++;
				*datai += l;
				if (dd == 0) m[0][0] = 0;
				else m[0][0] = readDC(*datai, dd, image_data);
				*datai += dd;
				debug("DC = %d\n", m[0][0]);
				// exit(0);
			} else {
				/* AC */
				debug("find AC\n");
				entered++;
				*datai += l;
				if (dd == 0) {
					e = true;
					break;
				}

				int p1 = d[0] - '0';
				int p2 = d[1] - '0';
				char AC[p2];

				/* p1 - count of zeros to write */
				bool le = false;
				for (int g = 0; g < p1; g++) {
					if (indexi >= 8 || indexj >= 8) {e = true; le = true; break;}
					setij(&indexi, &indexj, 0, m);
					entered++;
				}
				if (le) break;

				/* p2 - len of AC */
				for (int g = 0; g < p2; g++)
					AC[g] = image_data[*datai+g];
				*datai += p2;

				/* read AC */
				int value = base2to10(AC, p2, false);
				if (AC[0] == '0') value = value - pow(2, p2) + 1;

				if (indexi >= 8 || indexj >= 8) {e = true; break;}
				setij(&indexi, &indexj, value, m);

				debug("AC = %d\n", value);
			}
			if (entered >= 64) { e = true; break; }
			f += 3;
			fcod += l;
		}
		if (e) break;
	}

}

int HC_size(int size, int i, byte data[]) {
	int res = 0;
	for (int j = 0; j < size; j++)
		for (int g = 0; g < data[1+i+j]; g++)
			res += j + 1;
	return res;
}

int HT_size(int size, int i, byte data[]) {
	int res = 0;
	for (int j = 0; j < size; j++)
		for (int g = 0; g < data[1+i+j]; g++)
			res++;
	return res;
}

int QT_size(int precision) {
	return precision*precision + 2;
}

void readDQT(byte data[], int *i, int offset, int *qtable, int precision, int qid) {
	int table[precision][precision];
	
	int p = 0;
	int q = 0;
	for (int t = 0; t < precision*precision; t++) {
		int value = read_bytes_be(data, i, precision/8);
		setij(&p, &q, value, table);
	}

	qtable[offset] = qid;
	qtable[offset + 1] = precision*precision;

	for (int p = 0; p < precision; p++)
		for (int q = 0; q < precision; q++)
			qtable[offset + 2 + p*precision + q] = table[p][q];

	
}

void readHT(int size, int *huffmantable, char *huffcodes, int *index, int *hindex, int *i, byte data[]) {
	
	int num = data[(*i)];
	(*i)++;

	debug("num = %d\n", num);
	
	u8 counts[size];

	for (int j = 0; j < size; j++) counts[j] = data[*i+j];
	*i += size;

	int code = 0;

	for (int j = 0; j < size; j++) {
		for (int g = 0; g < counts[j]; g++) {
			
			// if ((*index) == 0) huffmantable = malloc(sizeof(int) * 3);
			// else huffmantable = realloc(huffmantable, ((*t)+1)*(sizeof(int) * 3));

			huffmantable[(*index)] = num;
			huffmantable[(*index)+1] = j+1;
			huffmantable[(*index)+2] = (int)data[*i];
			int len = j + 1;
			char c[len];
			base10to2(c, code, len);

			// if (*hindex == 0) huffcodes = (int*)malloc((*huff_size));
			// else huffcodes = (int*)realloc(huffcodes, (*huff_size));

			debug("len = %d\n", len);

			debug("len = %d code = %d c = ", len ,code);
			for (int k = 0; k < len; k++)
				debug("%c", c[k]);
			debug("\n");

			for (int k = 0; k < len; k++)
				huffcodes[(*hindex)+k] = c[k];

			*index += 3;
			*hindex += len;
			code++;
			(*i)++;
		}
		// exit(0);
		code <<= 1;
	}
}


int headerI = -1;
JPEG processJPEG(char *path, char *outputPath) {

	int *huffmantable;
	int *qtable;
	char *huffcodes;
	int *qnums;

	int hindex = 0;
	int qtable_size = 0;

	int index = 0;
	int huff_size = 0;
	int ind = 0;
	int t = 0;

	int SOF_number_of_components = 0;

	/* Open file for reading bytes */
	FILE *file = fopen(path, "rb");

	/* Get size = count of bytes */
	debug("read file\n");
	int err = fseek(file, 0L, SEEK_END);
	if (err) {
		debug("fseek error\n"); exit(1);
	}
	long size = ftell(file);
	rewind(file);

	/* Output model */
	JPEG jpeg;
	jpeg.size = size;
	jpeg.path = path;
	jpeg.width = 0;
	jpeg.height = 0;
	jpeg.color_space = null;
	jpeg.color_model = null;
	jpeg.bits_per_sample = 0;

	/* Read bytes */
	byte data[size];
	fread(data, size, 1, file);

	fclose(file);

	for (int i = 0; i < size; i++) {
		if (data[i] == marker) {
			i++;
			switch (data[i]) {
			case SOI:
				debug("SOI\n");
				break;
			case APP0:
				debug("APP0\n");
				i++;
				int l = read_bytes_be(data, &i, 2);
				debug("* Length = %d\n", l);

				int APP0_end = i - 2 + l;

				if (data[i] == 'J' &&
					data[i+1] == 'F' &&
					data[i+2] == 'I' &&
					data[i+3] == 'F') {
					i += 5;
				} else {
					debug("NOT APP0\n");
					for (int j = 0; j < 5; j++)
						debug("%c ", data[i+j]);
					debug("\n");
					i -= 3;
					i += l;
					break;
				}
				debug("* JFIF version = %d.%d\n", data[i], data[i+1]);
				i += 2;
				debug("* Density = %d\n", data[i]);
				i++;
				int x_density = read_bytes_be(data, &i, 2);
				int y_density = read_bytes_be(data, &i, 2);
				debug("* X-density = %d\n", x_density);
				debug("* Y-density = %d\n", y_density);
				int thumb_width = data[i]; i++;
				debug("* Thumbnail width = %d\n", thumb_width);
				int thumb_height = data[i]; i++;
				debug("* Thumbnail height = %d\n", thumb_height);
				i += thumb_width * thumb_height * 3;
				i = APP0_end;
				break;
			case APP1:
				debug("APP %x\n", data[i]);
				i++;

				int le = read_bytes_be(data, &i, 2);
				debug("* Length = %d\n", le);


				int APP1_end = i - 2 + le;
				int startI = i;

				int d = i;

				/* 'E' 'X' 'I' 'F' '0x00' '0x00' */
				if (data[i] == 'E' &&
					data[i+1] == 'x' &&
					data[i+2] == 'i' &&
					data[i+3] == 'f') {
					i += 6;
				} else {

					/* seach for it */
					// char *ICC_PROFILE = "photoshop:ICCProfile";
					// int icc_len = 20;

					// char *adobe_data = malloc(sizeof(char) * le);

					// for (int j = 0; j < le; j++) {
					// 	adobe_data[j] = data[i+j];
					// }

					// int adobe_len = le;

					// char *p = strstr(adobe_data, ICC_PROFILE);

					// if (p == NULL) break;

					// int index = p - adobe_data + icc_len + 1;

					// if (adobe_data[index] == '"') index++;

					// int tmp = index;

					// char c;
					// while ((c = adobe_data[index]) != ' ') {
					// 	index++;
					// }

					// jpeg.color_space = malloc(sizeof(char) * (index - tmp));

					// index = tmp;

					// int j = 0;
					// while ((c = adobe_data[index]) != ' ') {
					// 	jpeg.color_space[j] = c;
					// 	index++;
					// 	j++;
					// }

					i = startI;
					i += le - 2;

					// free(adobe_data);

					break;
				}

				/* TIFF Header */
				TIFFi = i;

				/* false = little, true = big */
				bool encoding = (data[i] == 0x4d) ? true : false;
				debug("* Encoding = %s\n", (encoding == true) ? "bigEndian" : "littleEndian");
				debug("* Byte Order = %x %x\n", data[i], data[i+1]); i += 2;
				i += 2;  /* TIFF ID always = 0x00, 0x2A */
				int offset_0IFD = (encoding == true) ? read_bytes_be(data, &i, 4) : read_bytes_le(data, &i, 4);
				debug("* Offset to 0th IFD = %d\n", offset_0IFD);

				i = TIFFi;
				i += offset_0IFD;

				/* 0th IFD */
				// debug("### 0th IFD ###\n");
				// IFDFields(data, &i, encoding, &jpeg);

				// if (exifOffset != -1) {
				// 	i = TIFFi;
				// 	i += exifOffset;
				// 	/* Exif IFD */
				// 	debug("### Exif IFD ###\n");
				// 	IFDFields(data, &i, encoding, &jpeg);
				// }

				// if (gpsOffset != -1) {
				// 	i = TIFFi;
				// 	i += gpsOffset;
				// 	/* Interoperability IFD */
				// 	debug("### GPS IFD ###\n");
				// 	IFDFields(data, &i, encoding, &jpeg);
				// }

				// if (interOffset != -1) {
				// 	i = TIFFi;
				// 	i += interOffset;
				// 	/* GPS IFD */
				// 	debug("### Interoperability IFD ###\n");
				// 	IFDFields(data, &i, encoding, &jpeg);
				// }

				// debug("stOffset = %d\n", stOffset);
				// if (stOffset != 0 && stOffset != -1) {
				// 	i = TIFFi;
				// 	i += stOffset;
				// 	/* 1st IFD */
				// 	debug("### 1st IFD ###\n");
				// 	IFDFields(data, &i, encoding, &jpeg);
				// }

				i = startI;
				i += le - 2;

				i = APP1_end;

				break;
			case APP2:
				debug("APP2\n");
				i++;

				int len = read_bytes_be(data, &i, 2);
				debug("* Length = %d\n", len);

				if (data[i] == 'I' &&
					data[i+1] == 'C' &&
					data[i+2] == 'C' &&
					data[i+3] == '_' &&
					data[i+4] == 'P') {
					i += 14;

				} else {
					debug("NOT APP2\n");
					for (int j = 0; j < 14; j++)
						debug("%c ", data[i+j]);
					debug("\n");
					i -= 3;
					i += len - 2;
					break;
				}

				int profile_size = read_bytes_be(data, &i, 4);
				debug("* Profile size = %u\n", profile_size);

				int CMM_type = read_bytes_be(data, &i, 4);
				debug("* CMM type = %d\n", CMM_type);

				int profile_version1 = read_bytes_be(data, &i, 1);
				int profile_version2 = read_bytes_be(data, &i, 1);
				int profile_version3 = read_bytes_be(data, &i, 1);
				int profile_version4 = read_bytes_be(data, &i, 1);

				debug("* Profile version = %x\n", profile_version1);
				debug("* Profile version = %x\n", profile_version2);
				debug("* Profile version = %x\n", profile_version3);
				debug("* Profile version = %x\n", profile_version4);

				int device_class = read_bytes_be(data, &i, 4);
				debug("* Profile/Device class = %x\n", device_class);

				int color_space = read_bytes_be(data, &i, 4);
				debug("* Color model of data = %x\n", color_space);

				int PCS = read_bytes_be(data, &i, 4);
				debug("* PCS = %x\n", PCS);

				int datetime = read_bytes_be(data, &i, 12);
				debug("* Date and time = %x\n", datetime);

				int acsp = read_bytes_be(data, &i, 4);
				debug("* 'acsp' = %x\n", acsp);

				int signature = read_bytes_be(data, &i, 4);
				debug("* Primary platform signature = %x\n", signature);

				int flags = read_bytes_be(data, &i, 4);
				debug("* Profile flags = %x\n", flags);

				int manufacture = read_bytes_be(data, &i, 4);
				debug("* Device manufacture = %x\n", manufacture);

				int model = read_bytes_be(data, &i, 4);
				debug("* Device model = %x\n", model);

				int attributes = read_bytes_be(data, &i, 8);
				debug("* Device attributes = %x\n", attributes);

				int rendering = read_bytes_be(data, &i, 4);
				debug("* Rendering Intent = %x\n", rendering);

				int nCIEXYZ = read_bytes_be(data, &i, 12);
				debug("* The nCIEXYZ values of the illuminant of the PCS = %x\n", nCIEXYZ);

				int create_signature = read_bytes_be(data, &i, 4);
				debug("* Profile create signature = %x\n", create_signature);

				int id = read_bytes_be(data, &i, 16);
				debug("* Profile ID = %x\n", id);

				int reserved = read_bytes_be(data, &i, 28);
				debug("* Reserved = %x\n", reserved);

				headerI = i;
				/* Tag table */
				int n = read_bytes_be(data, &i, 4);
				debug("* n = %d\n", n);

				for (int j = 1; j <= n; j++) {
					debug("* tag %d\n", j);
					int tag_signature = read_bytes_be(data, &i, 4);
					debug("  * tag_signature = %X\n", tag_signature);
					u32 offset = read_bytes_be(data, &i, 4);
					debug("  * offset = %u\n", offset);
					u32 size = read_bytes_be(data, &i, 4);
					debug("  * size = %u\n", size);

					i = headerI;
					i += offset;
					for (int g = 0; g < size; g++) debug("%c", data[i+g]);
					debug("\n");
					i = headerI;
					i += 4 + j*12;
				}

				break;
			case APP13:
			case APP14:
				debug("APP13\n");
				i++;
				int APP13_len = read_bytes_be(data, &i, 2);
				debug("APP13_len = %d\n", APP13_len);
				int APP13_end = i - 2 + APP13_len;
				i = APP13_end;
				break;
			case DQT: {
				debug("DQT\n");
				i++;
				debug("alloc table\n");
				int length = read_bytes_be(data, &i, 2);
				debug("* Length = %d\n", length);

				int DQT_end = i - 2 + length;
				char dqt_data[2];
				dec2hex(dqt_data, data[i]);
				int precision = (dqt_data[0] - '0' == 0) ? 8 : 16;
				int qid = dqt_data[1] - '0';
				debug("qid = %d\n", qid);
				i++;
				debug("* precision = %d\n", precision);

				if (qtable_size == 0) {
					qtable_size += QT_size(precision);
					debug("qtable_size = %d\n", qtable_size);
					qtable = malloc(qtable_size * sizeof(int));
					readDQT(data, &i, 0, qtable, precision, qid);
				} else {
					qtable_size += QT_size(precision);
					qtable = realloc(qtable, qtable_size * sizeof(int));
					readDQT(data, &i, precision*precision + 2, qtable, precision, qid);
				}
				
				i = DQT_end;
				
				break;
			} 
			case SOF2:
				debug("SOF2\n");
				exit(0);
			case SOF0: {
				debug("SOF %x\n", data[i]);
				i++;
				int sof_le = read_bytes_be(data, &i, 2);
				debug("* Length = %d\n", sof_le);

				int SOF_end = i - 2 + sof_le;

				jpeg.bits_per_sample = data[i];
				debug("* Data precision = %llu\n", jpeg.bits_per_sample);
				i++;

				int height = read_bytes_be(data, &i, 2);
				int width = read_bytes_be(data, &i, 2);

				if (jpeg.width == height || jpeg.height == width) {
					jpeg.width = width;
					jpeg.height = height;
				}

				if (jpeg.height == 0 || jpeg.width == zoom || jpeg.height == zoom || zoom <= 0) jpeg.height = height;
				if (jpeg.width == 0 || jpeg.width == zoom || jpeg.height == zoom || zoom <= 0) jpeg.width = width;

				SOF_number_of_components = data[i];
				if (SOF_number_of_components == 1) jpeg.color_model = Gray;
				if (SOF_number_of_components == 3) jpeg.color_model = RGB;
				if (SOF_number_of_components == 4) jpeg.color_model = CMYK;
				debug("* Number of components = %d\n", SOF_number_of_components);
				i++;
				// exit(0);
				qnums = malloc(SOF_number_of_components * 4 * sizeof(int));

				for (int j = 0; j < SOF_number_of_components; j++) {
					int id = data[i]; i++;
					char d[2];
					dec2hex(d, data[i]);
					int h = d[0] - '0';
					int v = d[1] - '0';
					i++;
					int qid = data[i]; i++;

					qnums[ind] = id;
					qnums[ind+1] = h;
					qnums[ind+2] = v;
					qnums[ind+3] = qid;

					debug("id = %d\n", qnums[ind]);
					debug("H = %d\n", qnums[ind+1]);
					debug("V = %d\n", qnums[ind+2]);
					debug("qid = %d\n", qnums[ind+3]);

					ind += 4;
				}
				// exit(0);
				i = SOF_end;
				break;

			} case DHT: {
				debug("DHT\n");

				i++;
				int dht_len = read_bytes_be(data, &i, 2);
				debug("* Length = %d\n", dht_len);

				int DHT_end = i - 2 + dht_len;

				int count = 16;

				if (t == 0) {
					t += HT_size(16, i, data);
					huff_size += HC_size(16, i, data);
					debug("t = %d\n", t);
					debug("huff_size = %d\n", huff_size);
					huffmantable = malloc(sizeof(int)*t*3);
					huffcodes = malloc(huff_size);
					readHT(count, huffmantable, huffcodes, &index, &hindex, &i, data);
				} else {
					t += HT_size(16, i, data);
					huff_size += HC_size(16, i, data);
					debug("t = %d\n", t);
					debug("huff_size = %d\n", huff_size);
					huffmantable = realloc(huffmantable, sizeof(int)*t*3);
					huffcodes = realloc(huffcodes, huff_size);
					readHT(count, huffmantable, huffcodes, &index, &hindex, &i, data);
				}
				
				
				if (data[i] == 0xFF) break;
				
				t += HT_size(16, i, data);
				huff_size += HC_size(16, i, data);
				debug("t = %d\n", t);
				debug("huff_size = %d\n", huff_size);
				huffmantable = realloc(huffmantable, sizeof(int)*t*3);
				huffcodes = realloc(huffcodes, huff_size);
				readHT(count, huffmantable, huffcodes, &index, &hindex, &i, data);
				
				t += HT_size(16, i, data);
				huff_size += HC_size(16, i, data);
				debug("t = %d\n", t);
				debug("huff_size = %d\n", huff_size);
				huffmantable = realloc(huffmantable, sizeof(int)*t*3);
				huffcodes = realloc(huffcodes, huff_size);
				readHT(count, huffmantable, huffcodes, &index, &hindex, &i, data);

				t += HT_size(16, i, data);
				huff_size += HC_size(16, i, data);
				debug("t = %d\n", t);
				debug("huff_size = %d\n", huff_size);
				huffmantable = realloc(huffmantable, sizeof(int)*t*3);
				huffcodes = realloc(huffcodes, huff_size);
				readHT(count, huffmantable, huffcodes, &index, &hindex, &i, data);

				i = DHT_end;
				// exit(0);
				break;
			} case SOS: {
				debug("SOS\n");

				debug("qid = 0\n");
				for (int i = 0; i < 8; i++) {
					for (int j = 0; j < 8; j++)
						debug("%5d", qtable[2 + i*8 + j]);
					debug("\n");
				}

				debug("qid = 1\n");
				for (int i = 0; i < 8; i++) {
					for (int j = 0; j < 8; j++)
						debug("%5d", qtable[2 + 64 + 2 + i*8 + j]);
					debug("\n");
				}

				// exit(0);

				int j = 0;
				int r = 0;
				while (j < index) {
					debug("num = %d\n", huffmantable[j]);
					debug("len = %d\n", huffmantable[j+1]);
					int l = huffmantable[j+1];
					debug("l = %d r = %d\n", l, r);
					debug("code = ");
					for (int p = r; p < r + huffmantable[j+1]; p++)
						debug("%c", huffcodes[p]);
					debug("\n");
					debug("data = %X\n", huffmantable[j+2]);
					debug("-------------------\n");
					j += 3;
					r += l;
				}

				// exit(0);


				i++;
				int sos_len = read_bytes_be(data, &i, 2);
				debug("* Length = %d\n", sos_len);

				int SOS_end = i - 2 + sos_len;

				int SOS_number_of_components = data[i];
				debug("* Number of components = %d\n", SOS_number_of_components);
				i++;
				int huffids[SOS_number_of_components*2];
				for (int j = 0; j < SOS_number_of_components * 2; j += 2) {
					debug("    * id = %d\n", data[i]);
					i++;
					debug("     * data = %d\n", data[i]);
					char d[2];
					dec2hex(d, data[i]);
					huffids[j] = d[0] - '0';
					huffids[j+1] = d[1] - '0';
					i++;
				}
				i += 3; /* Skip 3 bytes */
				i = SOS_end;
				int el = 0;
				int k = 1;
				char *image_data;

				int test = 0;
				int h = 0;
				int h2 = 0;
				int restart_marker = 0xD0;
				while (1) { 
					if (data[i] == 0xFF && data[i+1] == 0xD9) break;
					if (restart_interval != 0 && data[i] == 0xFF && data[i+1] == restart_marker) {
						restart_marker++;
						if (restart_marker == 0xD8) restart_marker = 0xD0;
						i += 2;
						continue;
					}
					
					/* read and append data[i] byte to image_data */
					char append[8];
					base10to2(append, data[i], 8);
					if (k == 1) image_data = malloc(k*8);
					else image_data = realloc(image_data, k*8);
					for (int j = 0; j < 8; j++)
						image_data[(k-1)*8+j] = append[j];
					// debug("read %d\n", k);

					// debug("%02x ", data[i]);
					// h++;
					// if (h == 32) {
					// 	h2++;
					// 	h = 0;
					// 	debug("\n");
					// 	if (h2 == 20) break;
					// }

					/* it's ok if input was 0xFF, just skip 0x00 */
					if (data[i] == 0xFF && data[i+1] == 0x00) i++;

					i++;
					k++;

				}
				// exit(0);
				debug("\n");

				// debug("test = %d\n", test);
				// exit(0);
				// debug("k = %d\n", k);
				// debug("image_data = ");
				// FILE *test = fopen("test.txt", "w");
				// for (int j = 0; j < (k-1) * 8; j++) {
				// 	debug("%c", image_data[j]);
				// 	fprintf(test, "%c", image_data[j]);
				// }
				// fclose(test);
				// debug("\n");
				// exit(0);


				/* Huffman decoding */
				
				debug("huffids = ");
				for (int j = 0; j < SOS_number_of_components * 2; j++) {
					debug("%d ", huffids[j]);
				}
				debug("\n");

				

				int H_max = 0;
				for (int j = 1; j <= SOF_number_of_components; j += 4)
					if (qnums[j] > H_max) H_max = qnums[j];

				int V_max = 0;
				for (int j = 2; j <= SOF_number_of_components; j += 4)
					if (qnums[j] > V_max) V_max = qnums[j];

				

				int blocks_count = 0;
				int width_prime = jpeg.width;
				int height_prime = jpeg.height;

				

				int y_count, cb_count, cr_count;

				if (V_max == 0) V_max = 1;
				if (H_max == 0) H_max = 1;

				if (SOF_number_of_components == 1) {
					y_count = 1;
					cb_count = 0;
					cr_count = 0;
				} else {
					y_count = H_max * V_max;
					cb_count = y_count / ((H_max / qnums[5]) * (V_max / qnums[6]));
					cr_count = y_count / ((H_max / qnums[9]) * (V_max / qnums[10]));
				}
					
				debug("%d : %d : %d\n", y_count, cb_count, cr_count);


				int datai = 0;

				int gl_i = 0;
				int gl_j = 0;

				/* Fix width and height (Rick prime) */
				while (width_prime % (H_max * 8) != 0) width_prime++;
				while (height_prime % (V_max * 8) != 0) height_prime++;

				int **R = malloc(sizeof(int*) * height_prime);
				for (int p = 0; p < height_prime; p++)
					R[p] = malloc(sizeof(int) * width_prime);

				int **G = malloc(sizeof(int*) * height_prime);
				for (int p = 0; p < height_prime; p++)
					G[p] = malloc(sizeof(int) * width_prime);

				int **B = malloc(sizeof(int*) * height_prime);
				for (int p = 0; p < height_prime; p++)
					B[p] = malloc(sizeof(int) * width_prime);

				for (int p = 0; p < height_prime; p++)
					for (int q = 0; q < width_prime; q++) {
						R[p][q] = 0;
						G[p][q] = 0;
						B[p][q] = 0;
					}
				int y_last_DC = 0;
				int cb_last_DC = 0;
				int cr_last_DC = 0;

				debug("width = %d\n", jpeg.width);
				debug("height = %d\n", jpeg.height);

				debug("width_prime = %d\n", width_prime);
				debug("height_prime = %d\n", height_prime);
				// exit(0);
				blocks_count = width_prime * height_prime / ((H_max * 8) * (V_max * 8));
				for (int a = 1; a <= blocks_count; a++) {
					
					debug("Reading new block\n");
					int y[V_max * 8][H_max * 8];

					int cb_x, cb_y;
					int cr_x, cr_y;

					if (SOF_number_of_components == 1) {
						cb_x = 1;
						cb_y = 1;
						cr_x = 1;
						cr_y = 1;
					} else {
						cb_x = qnums[6];
						cb_y = qnums[5];
						cr_x = qnums[10];
						cr_y = qnums[9];
					}
					int cb[cb_x * 8][cb_y * 8];
					int cr[cr_x * 8][cr_y * 8];
					

					int y_i = 0;
					int y_j = 0;

					int cb_i = 0;
					int cb_j = 0;

					int cr_i = 0;
					int cr_j = 0;

					int type = 1;
					for (int j = 1; j <= y_count + cb_count + cr_count; j++) {

						debug("Reading new block component type = %d\n", type);
			
						int m[8][8];

						/* Configure ids for huffman tables */
						int DC_id, AC_id;
						if (type <= y_count) {DC_id = huffids[0]; AC_id = huffids[1];}
						else if (type - y_count <= cb_count) {DC_id = huffids[2]; AC_id = huffids[3];}
						else if (type - y_count - cb_count <= cr_count) {DC_id = huffids[4]; AC_id = huffids[5];}

						/* Read default 8x8 matrix */
						debug("Read default 8x8 matrix\n");
	
						read8x8(a, j, m, DC_id, AC_id, image_data, &datai, huffmantable, huffcodes, k, index);
						debug("Matrix %d : %d\n", a, j);
						debug("m = \n");
						for (int p = 0; p < 8; p++) {
							for (int q = 0; q < 8; q++)
								debug("%5d", m[p][q]);
							debug("\n");
						}
						/* Fix DCs */
						debug("Fix DCs\n");
						if (type <= y_count) {
							m[0][0] += y_last_DC;
							y_last_DC = m[0][0];
						} else if (type - y_count <= cb_count) {
							m[0][0] += cb_last_DC;
							cb_last_DC = m[0][0];
						} else if (type - y_count - cb_count <= cr_count) {
							m[0][0] += cr_last_DC;
							cr_last_DC = m[0][0];
						}
						
						/* Configure qids for quantization */
						int qid = 0;
						if (type <= y_count) qid = qnums[3];
						else if (type - y_count <= cb_count) qid = qnums[7];
						else if (type - y_count - cb_count <= cr_count) qid = qnums[11];

						debug("qid = %d\n", qid);
						qid = 0;

						int *_qtable;
						int u = 0;
						while (u < qtable_size) {
							int l = qtable[u+1];
							if (qtable[u] == qid) {
								u += 2;
								_qtable = malloc(sizeof(int) * l);
								for (int g = 0; g < l; g++)
									_qtable[g] = qtable[u+g];
								u += l;
								break;
							} else u += 2 + l;
						}

						for (int p = 0; p < 8; p++)
							for (int q = 0; q < 8; q++)
								m[p][q] *= _qtable[p*8 + q];

						free(_qtable);

						/* DCT */
						DCT(m);

						debug("DCT = \n");
						for (int p = 0; p < 8; p++) {
							for (int q = 0; q < 8; q++)
								debug("%5d", m[p][q]);
							debug("\n");
						}

						for (int p = 0; p < 8; p++) {
							for (int q = 0; q < 8; q++) {
								if (type <= y_count) y[p+y_i][q+y_j] = m[p][q];
								else if (type - y_count <= cb_count) cb[p+cb_i][q+cb_j] = m[p][q];
								else if (type - y_count - cb_count <= cr_count) cr[p+cr_i][q+cr_j] = m[p][q];
							}
						}

						// FIXME (works only in 4:2:0)
						y_j += 8;
						if (y_j == H_max * 8) {
							y_i += 8;
							y_j = 0;
						
						}

						// if (type == y_count + cb_count + cr_count) type=0;
						type++;

					}

					if (restart_interval != 0 && a % restart_interval == 0) {
						y_last_DC = 0;
						cb_last_DC = 0;
						cr_last_DC = 0;
					}

					
					
					// debug("y = \n");
					// for (int q = 0; q < 8; q++) {
					// 	for (int p = 0; p < 8; p++)
					// 		debug("%4d ", y[q][p]);
					// 	debug("\n");
					// }

					// debug("cb = \n");
					// for (int q = 0; q < 8; q++) {
					// 	for (int p = 0; p < 8; p++)
					// 		debug("%4d ", cb[q][p]);
					// 	debug("\n");
					// }

					// debug("cr = \n");
					// for (int q = 0; q < 8; q++) {
					// 	for (int p = 0; p < 8; p++)
					// 		debug("%4d ", cr[q][p]);
					// 	debug("\n");
					// }

					/* YCbCr to RGB */

					int r[V_max * 8][H_max * 8];
					int g[V_max * 8][H_max * 8];
					int b[V_max * 8][H_max * 8];

					for (int p = 0; p < 8 * V_max; p++)
						for (int q = 0; q < 8 * H_max; q++) {
							int *res;
							if (SOF_number_of_components == 1) {
								res = YCbCr2RGB(
									y[p][q],
									0,
									0
								);
							} else {
								res = YCbCr2RGB(
									y[p][q],
									cb[p/(V_max/qnums[6])][q/(H_max/qnums[5])],
									cr[p/(V_max/qnums[10])][q/(H_max/qnums[9])]
								);
							}

							r[p][q] = res[0];
							g[p][q] = res[1];
							b[p][q] = res[2];
							free(res);
						}

					debug("r = \n");
					for (int p = 0; p < 8; p++) {
						for (int q = 0; q < 8; q++)
							debug("%5d", r[p][q]);
						debug("\n");
					}

					debug("g = \n");
					for (int p = 0; p < 8; p++) {
						for (int q = 0; q < 8; q++)
							debug("%5d", g[p][q]);
						debug("\n");
					}

					debug("b = \n");
					for (int p = 0; p < 8; p++) {
						for (int q = 0; q < 8; q++)
							debug("%5d", b[p][q]);
						debug("\n");
					}

					/* Append 8x8 block to image */
					debug("Append 8x8 block to image\n");
					for (int p = 0; p < V_max * 8; p++) {
						for (int q = 0; q < H_max * 8; q++) {
							R[p+gl_i][q+gl_j] = r[p][q];
							G[p+gl_i][q+gl_j] = g[p][q];
							B[p+gl_i][q+gl_j] = b[p][q];
						}
					}

					/* Configure offsets */
					debug("Configure offsets\n");
					gl_j += H_max * 8;
					if (gl_j >= width_prime) {
						gl_j = 0;
						gl_i += V_max * 8;
					}
				}

				FILE *outputFile = fopen(outputPath, "w+");
				fprintf(outputFile, "%s\n", path);
				fprintf(outputFile, "%ld %ld\n", jpeg.width, jpeg.height);

				for (int p = 0; p < jpeg.height; p++)
					for (int q = 0; q < jpeg.width; q++)
						fprintf(outputFile, "%d %d %d\n", R[p][q], G[p][q], B[p][q]);

				fclose(outputFile);
				free(image_data);
				free(R);
				free(G);
				free(B);
				break;
			}
			case DRI:
				debug("DRI\n");
				i++;
				int dri_len = read_bytes_be(data, &i, 2);
				debug("* Length = %d\n", dri_len);

				int DRI_end = i - 2 + dri_len;

				restart_interval = read_bytes_be(data, &i, 2);
				debug("* Restart interval = %d\n", restart_interval);

				i = DRI_end;
				break;
			case EOI:
				debug("EOI\n");
				break;
			}
			i--;
		}
	}
	

	/* Reset global variables */
	TIFFi = 0;
	scale = 0;
	exifOffset  = -1;
	interOffset = -1;
	gpsOffset   = -1;
	stOffset    = -1;
	zoom = 0;
	headerI = -1;

	/* Clear memory */
	free(huffcodes);
	free(huffmantable);
	free(qtable);
	free(qnums);

	return jpeg;
}

bool equalsJPEG(JPEG a, JPEG b) {
	if (strcmp(a.path, b.path)) { debug("path %s != %s\n", a.path, b.path); return false; }
	if (a.size != b.size) { debug("size %llu != %llu\n", a.size, b.size); return false; }
	if (a.width != b.width) { debug("width %llu != %llu\n", a.width, b.width); return false; }
	if (a.height != b.height) { debug("height %llu != %llu\n", a.height, b.height); return false; }
	if (strcmp(a.color_space, b.color_space)) { debug("color_space %s != %s\n", a.color_space, b.color_space); return false; }
	if (strcmp(a.color_model, b.color_model)) { debug("color_model %s != %s\n", a.color_model, b.color_model); return false; }
	return true;
}

int main(int argc, char *args[]) {

	processJPEG(args[1], args[2]);

	/* Tests with 4:4:4 YCbCr sampling */
	// processJPEG("../Baseline-JPEG-Decoder/profile.jpg");
	// processJPEG("../Downloads/SubejctiveTest/1/2.jpg");
	// processJPEG("../Downloads/SubejctiveTest/15/1.jpg");
	// processJPEG("../Downloads/SubejctiveTest/2/7.jpg");

	/* Tests with 4:2:0 YCbCr sampling */
	// processJPEG("../Downloads/SubejctiveTest/1/1.jpg");
	// processJPEG("../Downloads/CID2013/IS1/co1/IS_I_C01_D01.jpg");
	// processJPEG("data/agri.jpeg");

	/* Tests with 4:2:2 YCbCr sampling */
	// processJPEG("data/031307-2.jpg");

	/* Tests with 4:4:0 YCbCr sampling */
	// processJPEG("data/tests/28-hex_value.jpg");

	/* Tests with 4:1:1 YCbCr sampling */
	// processJPEG("")

	/* Tests with 4:1:0 YCbCr sampling */
	// processJPEG("")
		
	return 0;
}
