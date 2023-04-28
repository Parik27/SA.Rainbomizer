/*
 * gender.c
 * --------
 *
 * Functions for determining the gender of a given first name.
 *
 * Copyright (c):
 * 2007-2008:  Jörg MICHAEL, Adalbert-Stifter-Str. 11, 30655 Hannover, Germany
 *
 * SCCS: @(#) gender.c  1.2  2008-11-30
 *
 * This program is subject to the GNU Lesser General Public License (LGPL)
 * (formerly known as GNU Library General Public Licence)
 * as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this file; if not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Actually, the LGPL is __less__ restrictive than the better known GNU General
 * Public License (GPL). See the GNU Library General Public License or the file
 * LIB_GPLG.TXT for more details and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * There is one important restriction: If you modify this program in any way
 * (e.g. modify the underlying logic or translate this program into another
 * programming language), you must also release the changes under the terms
 * of the LGPL.
 * That means you have to give out the source code to your changes,
 * and a very good way to do so is mailing them to the address given below.
 * I think this is the best way to promote further development and use
 * of this software.
 *
 * If you have any remarks, feel free to e-mail to:
 *     ct@ct.heise.de
 *
 * The author's email address is:
 *    astro.joerg@googlemail.com
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gen_ext.h"
#include "umlaut_g.h"


#define  GENDER_VERSION  "version 1.2, 2008-11-30"


/****  internally, this program uses 1 byte chars  ****/
#define HASH_COUNT    256


/****  bit masks for "compare_mode"           ****/
/****  (macros "1" to "8" are in "*.h" file)  ****/
#define SEARCH_GENDER              16
 #define SEARCH_EQUIV              32
#define SEARCH_FIRST               64
#define GENDER_COMPARE_EXACT      128
#define COMPARE_ABBREVIATION      256
#define DETAILS_FOR_COUNTRY       512

#define GET_MATCH_OR_NEXT_HIGHER  1024
#define UMLAUT_EXPAND             2048
#define IGNORE_SEPARATOR          4096
#define EXPAND_PARAM_1            8192
#define EXPAND_PARAM_2           16384

/****  bit masks for "internal_mode"  ****/
#define GENDER_INITIALIZED         16
#define GENDER_GET_COUNTRY         32
#define LEV_G_COMPARE_GERMAN       64
#define LEV_G_SKIP_UPEXPAND       128
#define TRACE_GENDER              256
#define TRACE_LEV_G               512

#define CHECK_STRING  "# DO NOT CHANGE:   FILE-FORMAT DEFINITION-DATE = 2008-11-16 "
#define CACHE_LOOKBACK_COUNT       2

#define CHARS_TO_IGNORE     "<>^,´'`~°/"
#define IS_CHAR_TO_IGNORE      '\001'

#define COMPRESSED_UMLAUTS_FOR_SORTING   '-'
#define EXPANDED_UMLAUTS_FOR_SORTING     '+'
#define PSEUDO_VOWEL_FOR_ARABIC_NAMES    '_'

#define VERY_GOOD             1
#define GOOD                  2
#define MEDIUM                3

#define CHARSET_ISO_8859_1    0
#define CHARSET_UNICODE       1
#define CHARSET_UTF_8         2

/****  position and length of first names in dictionary file  ****/
#define DATA_NAME_POS          3
#define DATA_NAME_LENGTH      26
#define MAX_LINE_SIZE        100

#define POS_UMLAUT_INFO       29


/****  macros needed for the Levenshtein function  ****/
#define MATCHES_ANY_STRING    '*'
#define MATCHES_ANY_CHAR      '?'

#define MATCHES_ALL   -1000

#define WORD_SEPARATORS   "+-/*( )&,'´`.:"





/****  dictionary file info  ****/
struct gc_struct
   { unsigned int pos;    /***  first column = 0  ***/
     unsigned int n;
     int  quality_of_statistics;
     int  weight;
     int  gc_country;
     char *country_short;
     char *country_text;
   };

/****  Notice: "country_short" is mostly (but not always)  ****/
/****          identical to international country codes    ****/
static struct gc_struct gc_data[]
= {
    { 30, 0, VERY_GOOD, 60, GC_BRITAIN,   "UK",   "Great Britain" },
    { 31, 0, GOOD,       4, GC_IRELAND,   "IRE",  "Ireland"     },
    { 32, 0, VERY_GOOD,150, GC_USA,       "USA",  "U.S.A."      },

    { 33, 0, VERY_GOOD, 60, GC_ITALY,      "I",   "Italy"       },
    { 34, 0, MEDIUM,     1, GC_MALTA,      "M",   "Malta"       },
    { 35, 0, GOOD,      10, GC_PORTUGAL,   "P",   "Portugal"    },
    { 36, 0, VERY_GOOD, 40, GC_SPAIN,      "E",   "Spain"       },

    { 37, 0, VERY_GOOD, 60, GC_FRANCE,     "F",   "France"      },
    { 38, 0, VERY_GOOD, 10, GC_BELGIUM,    "B",   "Belgium"     },
    { 39, 0, VERY_GOOD,  1, GC_LUXEMBOURG, "LUX", "Luxembourg"  },
    { 40, 0, VERY_GOOD, 14, GC_NETHERLANDS,"NL",  "the Netherlands" },

    { 41, 0, GOOD,       1, GC_EAST_FRISIA,"FRI", "East Frisia" },
    { 42, 0, VERY_GOOD, 80, GC_GERMANY,    "D",   "Germany"     },
    { 43, 0, VERY_GOOD,  8, GC_AUSTRIA,    "A",   "Austria"     },
    { 44, 0, VERY_GOOD,  7, GC_SWISS,      "CH",  "Swiss"       },

    { 45, 0, VERY_GOOD,  1, GC_ICELAND,    "ICE", "Iceland"     },
    { 46, 0, VERY_GOOD,  5, GC_DENMARK,    "DK",  "Denmark"     },
    { 47, 0, GOOD,       4, GC_NORWAY,     "N",   "Norway"      },
    { 48, 0, VERY_GOOD,  8, GC_SWEDEN,     "S",   "Sweden"      },
    { 49, 0, GOOD,       5, GC_FINLAND,    "FIN", "Finland"     },

    { 50, 0, GOOD,       2, GC_ESTONIA,    "EST", "Estonia"     },
    { 51, 0, GOOD,       2, GC_LATVIA,     "LTV", "Latvia"      },
    { 52, 0, GOOD,       3, GC_LITHUANIA,  "LIT", "Lithuania"   },

    { 53, 0, GOOD,      35, GC_POLAND,     "PL",  "Poland"      },
    { 54, 0, GOOD,       8, GC_CZECH_REP,  "CZ",  "Czech Republic" },
    { 55, 0, GOOD,       7, GC_SLOVAKIA,   "SK",  "Slovakia"    },
    { 56, 0, GOOD,      11, GC_HUNGARY,    "H",   "Hungary"     },
    { 57, 0, VERY_GOOD, 22, GC_ROMANIA,    "RO",  "Romania"     },
    { 58, 0, GOOD,       9, GC_BULGARIA,   "BG",  "Bulgaria"    },

    { 59, 0, MEDIUM,     4, GC_BOSNIA,     "BIH","Bosnia and Herzegovina" },
    { 60, 0, GOOD,       5, GC_CROATIA,    "CRO", "Croatia"     },
    { 61, 0, MEDIUM,     1, GC_KOSOVO,     "KOS", "Kosovo"      },
    { 62, 0, MEDIUM,     2, GC_MACEDONIA,  "MK",  "Macedonia"   },
    { 63, 0, MEDIUM,     1, GC_MONTENEGRO, "MON", "Montenegro"  },
    { 64, 0, MEDIUM,     9, GC_SERBIA,     "SER", "Serbia"      },
    { 65, 0, MEDIUM,     2, GC_SLOVENIA,   "SLO", "Slovenia"    },
    { 66, 0, GOOD,       3, GC_ALBANIA,    "AL",  "Albania"     },
    { 67, 0, GOOD,      10, GC_GREECE,     "GR",  "Greece"      },

    { 68, 0, GOOD,     100, GC_RUSSIA,     "RUS", "Russia"      },
    { 69, 0, MEDIUM,    10, GC_BELARUS,    "BY",  "Belarus"     },
    { 70, 0, MEDIUM,     4, GC_MOLDOVA,    "MOL", "Moldova"     },
    { 71, 0, MEDIUM,    45, GC_UKRAINE,    "UKR", "Ukraine"     },
    { 72, 0, MEDIUM,     3, GC_ARMENIA,    "ARM", "Armenia"     },
    { 73, 0, MEDIUM,     4, GC_AZERBAIJAN, "AZE", "Azerbaijan"  },
    { 74, 0, MEDIUM,     5, GC_GEORGIA,    "GEO", "Georgia"     },
    { 75, 0, MEDIUM,    15,GC_KAZAKH_UZBEK,"KAZ", "Kazakhstan/Uzbekistan,etc." },

    { 76, 0, GOOD,      55, GC_TURKEY,     "TR",  "Turkey"      },
    { 77, 0, GOOD,      80, GC_ARABIA,     "AR",  "Arabia/Persia" },
    { 78, 0, MEDIUM,     4, GC_ISRAEL,     "ISR", "Israel"      },
    { 79, 0, VERY_GOOD,300, GC_CHINA,      "CHN", "China"       },
    { 80, 0, GOOD,     250, GC_INDIA,      "IND", "India/Sri Lanka" },
    { 81, 0, GOOD,      35, GC_JAPAN,      "JAP", "Japan"       },
    { 82, 0, GOOD,      12, GC_KOREA,      "KOR", "Korea"       },
    { 83, 0, GOOD,      17, GC_VIETNAM,    "VN",  "Vietnam"     },
    { 84, 0, MEDIUM,     1,    -1L,        "XX",  "other countries" },
    {  0, 0,  0,         0,    -1L,        NULL,    NULL        }
  };


static char *umlauts_unicode[]
  =  {
       "256",  "<A/>",
       "257",  "<a/>",
       "258",  "<A^>",
       "258",  "<Â>",   /** <A^> **/
       "259",  "<a^>",
       "259",  "<â>",   /** <a^> **/
       "260",  "<A,>",
       "261",  "<a,>",
       "262",  "<C´>",
       "263",  "<c´>",
       "268",  "<C^>",
       "268",  "<CH>",
       "269",  "<c^>",
       "269",  "<ch>",
       "271",  "<d´>",
       "272",  "<Ð>",
       "272",  "<DJ>",
       "273",  "<ð>",
       "273",  "<dj>",
       "274",  "<E/>",
       "275",  "<e/>",
       "278",  "<E°>",
       "279",  "<e°>",
       "280",  "<E,>",
       "281",  "<e,>",
       "282",  "<E^>",
       "282",  "<Ê>",
       "283",  "<e^>",
       "283",  "<ê>",
       "286",  "<G^>",
       "287",  "<g^>",
       "290",  "<G,>",
       "291",  "<g´>",
       "298",  "<I/>",
       "299",  "<i/>",
       "304",  "<I°>",
       "305",  "<i>",
       "306",  "<IJ>",
       "307",  "<ij>",
       "310",  "<K,>",
       "311",  "<k,>",
       "315",  "<L,>",
       "316",  "<l,>",
       "317",  "<L´>",
       "318",  "<l´>",
       "321",  "<L/>",
        "322",  "<l/>",
       "325",  "<N,>",
       "326",  "<n,>",
       "327",  "<N^>",
       "328",  "<n^>",
       "336",  "<Ö>",
       "337",  "<ö>",
       "338",  "<OE>",
       "338",  "Œ",   /** <OE> **/
       "339",  "<oe>",
       "339",  "œ",   /** <oe> **/
       "344",  "<R^>",
       "345",  "<r^>",
       "350",  "<S,>",
       "351",  "<s,>",
       "352",  "<S^>",
       "352",  "Š",   /** <S^> **/
       "352",  "<SCH>",
       "352",  "<SH>",
       "353",  "<s^>",
       "353",  "š",   /** <s^> **/
       "353",  "<sch>",
       "353",  "<sh>",
       "354",  "<T,>",
       "355",  "<t,>",
       "357",  "<t´>",
       "362",  "<U/>",
       "363",  "<u/>",
       "366",  "<U°>",
       "367",  "<u°>",
       "370",  "<U,>",
       "371",  "<u,>",
       "379",  "<Z°>",
       "380",  "<z°>",
       "381",  "<Z^>",
       "382",  "<z^>",
       "7838", "<ß>",   /***  newly defined "Großes ß"  ***/
       NULL,    NULL
     };


/****  internal variables  ****/
static int  internal_mode = GENDER_DEFAULT_TRACE_MODE | GENDER_GET_COUNTRY;
static FILE *f_names = NULL;
static int  line_size = 0;
static long record_count = 0L;
static long cache_p [CACHE_LOOKBACK_COUNT];
static char cache_text [CACHE_LOOKBACK_COUNT] [MAX_LINE_SIZE+1];

static char up_and_conv[HASH_COUNT];
static char sortchar [HASH_COUNT];
static char sortchar2[HASH_COUNT];
static char upperchar[HASH_COUNT];




/************************************************************/
/****  phonetic rules for the Levenshtein function  *********/
/************************************************************/

struct PHONETIC_RULES
  {
    char *text_1;
    char *text_2;
    int len_1;
    int len_2;
    int ph_diff;     /***  difference in hundredths of points  ***/
    int hash_group;
  };

static int ph_hash_group_begin[17];
static int ph_hash_group_end[17];
static int ph_rules_hash_del_ins [HASH_COUNT];
static int ph_rules_hash_replace [HASH_COUNT];

static struct PHONETIC_RULES ph_rules_german[] =
  {
    { "IJ",  "I",   0, 0,   10,  1 },
    { "IJ",  "J",   0, 0,   30,  1 },
    { "IY",  "I",   0, 0,   10,  1 },
    { "IY",  "Y",   0, 0,   30,  1 },
    { "I",   "J",   0, 0,   30,  1 },
    { "I",   "Y",   0, 0,   30,  1 },
    { "J",   "Y",   0, 0,   20,  1 },
    { "IE",  "I",   0, 0,    5,  1 },
    { "IE",  "",    0, 0,  120,  1 },

    { "EU",  "OI",  0, 0,   10,  2 },
    { "EU",  "OY",  0, 0,   10,  2 },
    { "OI",  "OY",  0, 0,    5,  2 },
    { "OU",  "U",   0, 0,    5,  2 },

    { "AH",  "A",   0, 0,    5,  3 },
    { "H",   "",    0, 0,   50,  3 },
    { "Å",   "AA",  0, 0,    0,  3 },
    { "Ä",   "AE",  0, 0,    0,  3 },
    { "Ä",   "A",   0, 0,   40,  3 },
    { "AE",  "A",   0, 0,   60,  3 },

    { "Ä",   "E",   0, 0,   20,  4 },
    { "Æ",   "E",   0, 0,   20,  4 },
    { "A",   "ER",  0, 0,   70,  4 },
    { "AE",  "E",   0, 0,   20,  4 },
    { "AE",  "",    0, 0,  110,  4 },

    { "AI",  "AY",  0, 0,    5,  4 },
    { "AI",  "EI",  0, 0,    5,  4 },
    { "AI",  "EY",  0, 0,   10,  4 },
    { "AY",  "EI",  0, 0,   10,  4 },
    { "AY",  "EY",  0, 0,    5,  4 },
    { "EI",  "EY",  0, 0,    5,  4 },

    { "Å",   "O",   0, 0,   70,  5 },
    { "Ö",   "OE",  0, 0,    0,  5 },
    { "Ö",   "O",   0, 0,   40,  5 },
    { "Ø",   "Ö",   0, 0,    0,  5 },
    { "Ø",   "OE",  0, 0,   10,  5 },
    { "Ø",   "O",   0, 0,   30,  5 },
    { "Œ",   "Ö",   0, 0,    5,  5 },
    { "Œ",   "OE",  0, 0,    0,  5 },
    { "OE",  "O",   0, 0,   60,  5 },
    { "OE",  "",    0, 0,  110,  5 },

    { "CHS", "X",   0, 0,  100,  6 },
    { "CKS", "X",   0, 0,   30,  6 },
    { "CK",  "K",   0, 0,    5,  6 },
    { "C",   "K",   0, 0,   30,  6 },
    { "CHS", "",    0, 0,  200,  6 },
    { "CH",  "",    0, 0,  130,  6 },
    { "CKS", "",    0, 0,  200,  6 },
    { "CK",  "",    0, 0,  110,  6 },

    { "DT",  "T",   0, 0,    5,  7 },
    { "D",   "T",   0, 0,   30,  7 },
    { "TH",  "T",   0, 0,    5,  7 },
    { "DT",  "",    0, 0,  110,  7 },

    { "KS",  "X",   0, 0,    5,  8 },
    { "GS",  "X",   0, 0,   10,  8 },
    { "G",   "K",   0, 0,   50,  8 },
    { "QU",  "KW",  0, 0,   10,  8 },
    { "Q",   "K",   0, 0,   10,  8 },

    { "NCH", "NSCH",0, 0,   10,  9 },
    { "NCH", "NSH", 0, 0,   10,  9 },
    { "NTX", "NCH", 0, 0,   20,  9 },
    { "NTX", "NSCH",0, 0,   20,  9 },
    { "NTX", "NSH", 0, 0,   20,  9 },
    { "NG",  "NK",  0, 0,   20,  9 },

    { "ß",   "SS",  0, 0,    0, 10 },
    { "ß",   "S",   0, 0,    5, 10 },
    { "SCH", "SH",  0, 0,    5, 10 },
    { "SCH", "SZ",  0, 0,   20, 10 },
    { "SCH", "S",   0, 0,  100, 10 },
    { "SCH", "",    0, 0,  200, 10 },

    { "TZ",  "Z",   0, 0,    5, 11 },
    { "TIA", "ZIA", 0, 0,   20, 11 },
    { "Z",   "C",   0, 0,   40, 11 },
    { "Z",   "S",   0, 0,   50, 11 },

    { "M",   "N",   0, 0,   70, 12 },
    { "N",   "U",   0, 0,   70, 12 },

    { "PH",  "F",   0, 0,    5, 13 },
    { "PF",  "F",   0, 0,    5, 13 },
    { "B",   "P",   0, 0,   40, 13 },

    { "F",   "V",   0, 0,   20, 14 },
    { "W",   "V",   0, 0,   20, 14 },

    { "Ü",   "UE",  0, 0,    0, 15 },
    { "Ü",   "U",   0, 0,   40, 15 },
    { "Ü",   "Y",   0, 0,   30, 15 },
    { "UE",  "U",   0, 0,   60, 15 },
    { "UE",  "",    0, 0,  110, 15 },

    { NULL,  NULL,  0, 0,    0,  0 }
  };


static char *russian_names[] =
  {
    "Aleksander",   "Oleksander",
    "Aleksandr",    "Aliaksandr",
    "Aleksandr",    "Oleksandr",
    "Aleksandra",   "Aliaksandra",
    "Aleksandra",   "Oleksandra",
    "Aleksey",      "Aliaksei",
    "Aleksey",      "Aliaksey",
    "Aleksey",      "Oleksey",
    "Aleksey",      "Oleksii",
    "Aleksey",      "Oleksij",
    "Aleksey",      "Oleksiy",
    "Alexander",    "Olexander",
    "Alexandr",     "Olexandr",
    "Alexandra",    "Olexandra",
    "Alexey",       "Olexiy",
    "Anatoliy",     "Anatol",
    "Andrey",       "Andrii",
    "Andrey",       "Andrij",
    "Andrey",       "Andriy",
    "Artiom",       "Artsiom",
    "Boris",        "Barys",
    "Boris",        "Borys",
    "Dariya",       "Daryia",
    "Denis",        "Denys",
    "Diana",        "Dziana",
    "Diyana",       "Dziyana",
    "Dmitriy",      "Dmitro",
    "Dmitriy",      "Dmytriy",
    "Dmitriy",      "Dmytro",
    "Dmitriy",      "Dzimitry",
    "Dmitriy",      "Dzmitrij",
    "Dmitriy",      "Dzmitriy",
    "Dmitry",       "Dzmitry",
    "Dzmitri",      "Dzmitri",
    "Elena",        "Ellina",
    "Evgeniy",      "Eugeni",
    "Fedor",        "Fedir",
    "Galina",       "Galyna",
    "Gennadiy",     "Henadz",
    "Gennadiy",     "Henadzi",
    "Gennadiy",     "Hennadij",
    "Gennadiy",     "Hennadiy",
    "Gennadiy",     "Hennadzy",
    "Georgiy",      "Georgii",
    "Grigorij",     "Grygorij",
    "Grigoriy",     "Grigorii",
    "Grigoriy",     "Grygorii",
    "Grigoriy",     "Grygory",
    "Igor",         "Ihar",
    "Igor",         "Ihor",
    "Ilya",         "Illya",
    "Irina",        "Iryna",
    "Jevgenija",    "Jevgenija",
    "Jevgeniy",     "Jevgeniy",
    "Jevgeniy",     "Yevgen",
    "Juri",         "Juriy",
    "Katerina",     "Kateryna",
    "Katerina",     "Katsiaryna",
    "Kiril",        "Kiryl",
    "Kiril",        "Kiryll",
    "Kiril",        "Kyrylo",
    "Konstantin",   "Kanstantsin",
    "Konstantin",   "Konstantyn",
    "Konstantin",   "Kostiantyn",
    "Konstantin",   "Kostyantin",
    "Konstantin",   "Kostyantyn",
    "Kristina",     "Khrystyna",
    "Kristina",     "Kristsina",
    "Kristina",     "Krystina",
    "Kristina",     "Krystsina",
    "Kristina",     "Krystyna",
    "Larisa",       "Larysa",
    "Leonid",       "Leanid",
    "Lubomir",      "Lubomir",
    "Ludmila",      "Ludmyla",
    "Lyudmila",     "Lyudmyla",
    "Margarita",    "Marharita",
    "Margarita",    "Marharyta",
    "Margaryta",    "Margaryta",
    "Marina",       "Maryna",
    "Mariya",       "Maryia",
    "Mariya",       "Naryja",
    "Maxim",        "Maksym",
    "Michail",      "Michailo",
    "Michail",      "Michaylo",
    "Michail",      "Mihail",
    "Mikhail",      "Mikhailo",
    "Mikhail",      "Mikhaylo",
    "Mikhail",      "Mykhailo",
    "Mikhail",      "Mykhajlo",
    "Mikhail",      "Mykhaylo",
    "Miroslav",     "Myroslav",
    "Miroslava",    "Myroslava",
    "Nadya",        "Nadija",
    "Nadya",        "Nadiya",
    "Natalya",      "Nataliia",
    "Natalya",      "Natallia",
    "Nikolay",      "Mikalai",
    "Nikolay",      "Mikalaj",
    "Nikolay",      "Mikola",
    "Nikolay",      "Mykola",
    "Oksana",       "Aksana",
    "Oleg",         "Aleg",
    "Oleg",         "Aleh",
    "Oleg",         "Oleh",
    "Olesia",       "Alesia",
    "Olesja",       "Alesja",
    "Olessja",      "Alessja",
    "Olessya",      "Alessya",
    "Olesya",       "Alesya",
    "Olga",         "Olha",
    "Olga",         "Volha",
    "Roman",        "Raman",
    "Rostislav",    "Rostyslav",
    "Sergej",       "Serhij",
    "Sergey",       "Sergii",
    "Sergey",       "Sergij",
    "Sergey",       "Sergiy",
    "Sergey",       "Serhei",
    "Sergey",       "Serhii",
    "Sergey",       "Serhiy",
    "Sergey",       "Siarhei",
    "Sergey",       "Siarhej",
    "Sergey",       "Siarhey",
    "Snezhana",     "Sniazhana",
    "Stanislav",    "Stanislau",
    "Svetlana",     "Sviatlana",
    "Svetlana",     "Svitlana",
    "Tatyana",      "Tatsiana",
    "Tatyana",      "Tatsyana",
    "Tatyana",      "Tetiana",
    "Tatyana",      "Tetjana",
    "Tatyana",      "Tetyana",
    "Vadim",        "Vadym",
    "Vadim",        "Vadzim",
    "Valentin",     "Valentyn",
    "Valentina",    "Valentyna",
    "Valentina",    "Valiantsina",
    "Valeriy",      "Valerii",
    "Valeriya",     "Valeryia",
    "Vasil",        "Vasyl",
    "Vera",         "Vira",
    "Veronika",     "Veranika",
    "Viacheslav",   "Viachaslau",
    "Viacheslav",   "Viacheslau",
    "Viktor",       "Viktar",
    "Viktoriya",    "Viktoryia",
    "Vladimir",     "Uladzimir",
    "Vladimir",     "Uladzmir",
    "Vladimir",     "Vladymyr",
    "Vladimir",     "Volodimir",
    "Vladimir",     "Volodymyr",
    "Vladislav",    "Uladzislau",
    "Vladislav",    "Vladyslav",
    "Yaroslav",     "Yaraslau",
    "Yevgenia",     "Yauhenia",
    "Yevgeniy",     "Jauhen",
    "Yevgeniy",     "Yauhen",
    "Yevgeniy",     "Yauheni",
    "Yevgeniya",    "Yauheniya",
    "Yuliya",       "Yuliia",
    "Yuri",         "Juryj",
    "Yuri",         "Yurii",
    "Yuri",         "Yurij",
    NULL,           NULL
  };




/************************************************************/
/****  common string functions  *****************************/
/************************************************************/

int initialize_gender (void)
{
  int  i,k,n;
  char *s,*s2,*s3;

  if (! (internal_mode & GENDER_INITIALIZED))
    {
      if ((int)strlen (letters_a_to_z) != 26)
        {
          if (internal_mode & TRACE_GENDER)
            {
              printf ("Error: %s  is not allowed\n",
                  "strlen (letters_a_to_z) != 26");
            }
          return (-1);
        }
      if ((int)strlen (letters_a_to_z) != (int)strlen (letters_A_to_Z))
        {
          if (internal_mode & TRACE_GENDER)
            {
              printf ("Error: %s  is not allowed\n",
                  "strlen(letters_a_to_z) != strlen(letters_a_to_z)");
            }
          return (-1);
        }
      if ((int)strlen (umlaut_lower) != (int)strlen (umlaut_upper))
        {
          if (internal_mode & TRACE_GENDER)
            {
              printf ("Error: %s  is not allowed\n",
                  "strlen(umlaut_lower) != strlen(umlaut_upper)");
            }
          return (-1);
        }

      if ((int)strlen (umlaut_lower) != (int)strlen (umlaut_sort)
      ||  (int)strlen (umlaut_lower) != (int)strlen (umlaut_sort2))
        {
          if (internal_mode & TRACE_GENDER)
            {
              printf ("Error: %s  is not allowed\n",
                  "strlen(umlaut_sort*) != strlen(umlaut_lower)");
            }
          return (-1);
        }

      internal_mode |= GENDER_INITIALIZED;

      /****  generate arrays "sortchar", "sortchar2" and "upperchar"  ****/
      for (i=0; i< HASH_COUNT; i++)
        {
          sortchar[i] = (char) i;
          sortchar2[i] = '\0';
          up_and_conv[i] = (char) i;
          upperchar[i] = (char) i;
        }
      sortchar [(unsigned char) '-'] = ' ';
      sortchar [(unsigned char) '\''] = '´';
      up_and_conv [(unsigned char) '-'] = ' ';

      s = CHARS_TO_IGNORE;
      for (i=0; s[i] != '\0'; i++)
        {
          k = (unsigned char) s[i];
          sortchar[k] = IS_CHAR_TO_IGNORE;
        }

      s = letters_a_to_z;
      s2 = letters_A_to_Z;

      for (i=0; s[i] != '\0'; i++)
        {
          k = (unsigned char) s2[i];
          sortchar[k] = s2[i];
          up_and_conv[k] = s2[i];
          upperchar[k] = s2[i];

          k = (unsigned char) s[i];
          sortchar[k] = s2[i];
          up_and_conv[k] = s2[i];
          upperchar[k] = s2[i];
        }

      s = umlaut_lower;
      s2 = umlaut_upper;

      for (i=0; s[i] != '\0'; i++)
        {
          n = (unsigned char) umlaut_conv[i];

          k = (unsigned char) s2[i];  /** "s2" **/
          up_and_conv[k] = (char) n;
          upperchar[k] = s2[i];

          k = (unsigned char) s[i];   /** "s" **/
          up_and_conv[k] = (char) n;
          upperchar[k] = s2[i];
        }

      s = umlaut_lower;
      s2 = umlaut_sort;
      s3 = umlaut_sort2;

      for (i=0; s[i] != '\0'; i++)
        {
          k = (unsigned char) s[i];
          sortchar[k] = s2[i];
          if (s3[i] != ' ')
            {
              sortchar2[k] = s3[i];
            }

          k = (unsigned char) umlaut_upper[i];
          sortchar[k] = s2[i];
          if (s3[i] != ' ')
            {
              sortchar2[k] = s3[i];
            }
        }

      /****  initialize hash values for phonetic rules  ****/
      for (i=0; i<17; i++)
        {
          ph_hash_group_begin[i] = -1;
          ph_hash_group_end[i] = -1;
        }
      for (i=0; i< HASH_COUNT; i++)
        {
          ph_rules_hash_del_ins[i] = -1;
          ph_rules_hash_replace[i] = 0;
        }

      for (i=0; ph_rules_german[i].text_1 != NULL; i++)
        {
          k = ph_rules_german[i].hash_group;

          if (ph_hash_group_begin[k] < 0)
            {
              ph_hash_group_begin[k] = i;
            }

          if (ph_hash_group_end[k] < 0
          ||  strcmp (ph_rules_german[i].text_2,"") != 0)
            {
              ph_hash_group_end[k] = i;
            }

          n = (unsigned char) ph_rules_german[i].text_1[0];
          ph_rules_hash_replace [n] |= 1 << (k-1);

          if (ph_rules_hash_del_ins[n] < 0
          &&  strcmp (ph_rules_german[i].text_2,"") == 0)
            {
              ph_rules_hash_del_ins[n] = i;
            }

          n = (unsigned char) ph_rules_german[i].text_2[0];
          ph_rules_hash_replace [n] |= 1 << (k-1);

          n = (int)strlen (ph_rules_german[i].text_1);
          ph_rules_german[i].len_1 = n;
          n = (int)strlen (ph_rules_german[i].text_2);
          ph_rules_german[i].len_2 = n;
        }
    }

  return (0);
}



static int copycut (char dest[], char src[], int len)
{
  int i,k,n;
  char c;

  k = 0;
  n = 0;
  i = 1;
  while ((c = src[k]) != '\0'  &&  c != '\n'
  &&  (k < len-1  ||  len < 0))
    {
      if (c == ' ')
        {
          i--;
        }
      else
        {
          i = 2;
        }

      if (i > 0)
        {
          dest[n] = c;
          n++;
        }
      k++;
    }

  if (n > 0  &&  dest[n-1] == ' ')
    {
      /****  delete trailing space  ****/
      n--;
    }
  dest[n] = '\0';

  return (n);
}




static int strcmp_search (char text[], char text_2[],
     int compare_mode, char umlaut_info, int *diff)

/****  Internal function for "string compare":          ****/
/****  Compare strings case-insensitive and ignore      ****/
/****  different spellings of umlauts.                  ****/
/****  Return value is the same as for normal "strcmp". ****/
/****  First arg  = "external" name                     ****/
/****  Second arg = "internal" name (with "unicode" chars) ****/

/****  Important bit masks for "compare_mode":          ****/
/****  EXPAND_PARAM_1  :   expand umlauts in "text"     ****/
/****  EXPAND_PARAM_2  :   expand umlauts in "text_2"   ****/
/****  COMPARE_ABBREVIATION :  compare abbreviation     ****/
/****                          (e.g. "C." and "Carl")   ****/

/****  This function converts "src" to upper case,      ****/
/****  compresses successive blanks and (if necessary)  ****/
/****  expands umlauts, i.e. "ä" is converted to "AE".  ****/
/****  The conversion is done "on demand" using the     ****/
/****  arrays "sortchar" and "sortchar2". The second    ****/
/****  array is needed only for umlauts and contains    ****/
/****  null values for all other chars.                 ****/
{
  int  i,i2,k,n;
  char c=0,c2=0;

  if (umlaut_info == COMPRESSED_UMLAUTS_FOR_SORTING)
    {
      compare_mode &= ~EXPAND_PARAM_2;
    }
  if (umlaut_info == EXPANDED_UMLAUTS_FOR_SORTING)
    {
      compare_mode |= EXPAND_PARAM_2;
    }

  i = 0;
  i2 = 0;
  k = 0;
  while (sortchar [(unsigned char) *text_2] == IS_CHAR_TO_IGNORE)
    {
      text_2++;
    }

  while (*text != '\0'  &&  *text_2 != '\0')
    {
      /****  find next char in first string                 ****/
      /****  (without "while (c == IS_CHAR_TO_IGNORE) ...") ****/
      c = '\0';
      if (i != 0)
        {
          if (compare_mode & EXPAND_PARAM_1)
            {
              c = sortchar2 [(unsigned char) *text];
            }
          i = 0;
          text++;
        }

      if (c == '\0')
        {
          c = sortchar [(unsigned char) *text];
          i = 1;

          if ((c == ' '  ||  c == '-')
          &&  (compare_mode & IGNORE_SEPARATOR))
            {
              text++;
              c = sortchar [(unsigned char) *text];
            }
        }

      /****  find next char in second string  ****/
      c2 = '\0';
      if (i2 != 0)
        {
          if (compare_mode & EXPAND_PARAM_2)
            {
              c2 = sortchar2 [(unsigned char) *text_2];
            }
          i2 = 0;
          text_2++;
          while (sortchar [(unsigned char) *text_2] == IS_CHAR_TO_IGNORE)
            {
              text_2++;
            }
        }

      if (c2 == '\0')
        {
          c2 = sortchar [(unsigned char) *text_2];
          i2 = 1;

          if ((c2 == ' '  ||  c2 == '-'  ||  *text_2 == '+')
          &&  (compare_mode & IGNORE_SEPARATOR))
            {
              text_2++;
              while (sortchar [(unsigned char) *text_2] == IS_CHAR_TO_IGNORE)
                {
                  text_2++;
                }
              c2 = sortchar [(unsigned char) *text_2];
            }
        }

      if (c2 == '+')
        {
          /****  "internal separator" ('+') found  ****/
          c2 = '-';
          if (c == ' '  ||  c == '-')
            {
              c2 = c;
              continue;  /****  important  ****/
            }
          else if (i == 1  &&  i2 == 1
          &&  sortchar2 [(unsigned char) *text] == '\0')
            {
              text_2++;
              while (sortchar [(unsigned char) *text_2] == IS_CHAR_TO_IGNORE)
                {
                  text_2++;
                }
              c2 = sortchar [(unsigned char) *text_2];
            }
        }

      /****  compare chars  ****/
      if ((compare_mode & COMPARE_ABBREVIATION)
      &&  c == '.'  &&  c2 != '\0')
        {
          /****  strings match like (e.g.) "St." and "Steven"  ****/
          c = c2;
          break;
        }
      if (c != c2)
        {
          /****  strings don't match  ****/
          break;
        }

      if (diff != NULL  &&  i == 1  &&  i2 == 1)
        {
          if (upperchar [(unsigned char) *text] != upperchar [(unsigned char) *text_2])
            {
              if (compare_mode & GENDER_COMPARE_EXACT)
                {
                  /****  strings don't match exactly  ****/
                  k |= 1024;
                }

              c = sortchar2 [(unsigned char) *text];
              c2 = sortchar2 [(unsigned char) *text_2];

              if (c != c2)
                {
                  if (c == '\0')
                    {
                      c = sortchar [(unsigned char) *(text+1)];
                    }
                  if (c2 == '\0')
                    {
                      c2 = sortchar [(unsigned char) *(text_2+1)];
                    }
                }

              if (c != c2)
                {
                  k++;
                  if (c == '\0')
                    {
                      k |= 128;
                    }
                  if (c2 == '\0')
                    {
                      k |= 256;
                    }
                  if (c != '\0'  &&  c2 != '\0')
                    {
                      k |= 1024;
                    }
                }

              if (c == '\0')
                {
                  c = sortchar [(unsigned char) *(text+1)];

                  if ((c == ' '  ||  c == '-')
                  &&  (compare_mode & IGNORE_SEPARATOR))
                    {
                      c = sortchar [(unsigned char) *(text+2)];
                    }
                }

              if (c2 == '\0')
                {
                  n = 1;
                  while (sortchar [(unsigned char) *(text_2+n)] == IS_CHAR_TO_IGNORE)
                    {
                      n++;
                    }
                  c2 = sortchar [(unsigned char) *(text_2+n)];
                }

              if (c != c2
              && ((compare_mode & EXPAND_PARAM_2)
              ||  ! (compare_mode & GENDER_ALLOW_COMPRESSED_UMLAUTS)))
                {
                  /****  strings don't match (e.g. "Káte" vs "Käte")  ****/
                  k |= 1024;
                }
            }
        }
    }

  if (diff != NULL)
    {
      if (c != c2  ||  (k & (128+256)) == 128+256)
        {
          /****  strings don't match (e.g. "Katé" vs "Käte")  ****/
          k |= 1024;
        }
      *diff = k;
    }

  if (*text == '\0'  ||  *text_2 == '\0')
    {
      if (c == c2)
        {
          if (i != 0  &&  *text != '\0'
          && (sortchar2 [(unsigned char) *text] == '\0'
          ||  ! (compare_mode & EXPAND_PARAM_1)))
            {
              text++;
            }
          if (i2 != 0  &&  *text_2 != '\0'
          && (sortchar2 [(unsigned char) *text_2] == '\0'
          ||  ! (compare_mode & EXPAND_PARAM_2)))
            {
              text_2++;
            }
        }

      return ((unsigned char) *text - (unsigned char) *text_2);
    }

  return ((unsigned char) c - (unsigned char) c2);
}




static void read_line (FILE *fr, char text[], int len)
{
  text[0] = '\0';
  fgets (text,len-2,fr);
  text[len-1] = '\0';
}



static void read_unicode_line (FILE *fr, char text[], int two_byte_len)
{
  int i,k;
  i = 0;
  while (! feof (fr)  &&   i < 2 * two_byte_len - 2)
    {
      k = getc (fr);

      if (feof (fr))
        {
          break;
        }
      k += 256 * getc (fr);

      if (k == 13)
        {
          /****  check for unicode EOL  ****/
          k = getc (fr);
          k += 256 * getc (fr);

          if (k == 10  ||  feof (fr))
            {
              /****  unicode EOL found  ****/
              break;
            }
         
          text[i] = (char) (k%256);
          i++;
          text[i] = (char) (k/256);
          i++;

          if (i >= 2 * two_byte_len - 2)
            {
              break;
            }
        }

      text[i] = (char) (k%256);
      i++;
      text[i] = (char) (k/256);
      i++;
    }

  text[i] = '\0';
  text[i+1] = '\0';
}




static long binary_search (FILE *fr, char search_name[],
    int offset, int len_s, int compare_mode)

/****  Do a binary search for the fist match                 ****/
/****  (all lines in dictionary file must have equal length) ****/
/****  Return value == file pos of first match               ****/
/****                  ( < 0L: no match found)               ****/
/****  "compare_mode" is the same as for "strcmp_search"     ****/
{
  long p,p1,p2;
  int  i,n;
  char text [MAX_LINE_SIZE+1];
  char temp [MAX_LINE_SIZE+1];

  if (line_size == 0  ||  record_count == 0L)
    {
      /****  determine length of first line  ****/
      i = fseek (fr, 0L, SEEK_SET);
      if (i != 0)
        {
          if (internal_mode & TRACE_GENDER)
            {
              printf ("Error: Could not position in dictionary file '%s'\n",
                  FIRST_NAME_FILE);
            }
          return (-10L);
        }

      read_line (fr, text, MAX_LINE_SIZE+1);
      if (strncmp (text, CHECK_STRING, strlen(CHECK_STRING)) != 0)
        {
          if (internal_mode & TRACE_GENDER)
            {
              i = (int)strlen (text);
              while (i > 0  &&  (text[i-1] == '\n'  ||  text[i-1] == '\r'))
                {
                  i--;
                }
              text[i] = '\0';

              printf ("Error: Invalid version of dictionary file '%s'.\n",
                   FIRST_NAME_FILE);
              printf ("File header is:  \"%s\"\n", text);
              printf ("(this should be:  \"%s\").\n", CHECK_STRING);
            }
          return (-10L);
        }

      /****  "ftell" gives the correct line size under Unix and Windows  ****/
      line_size = (int) ftell (fr);

      /****  determine file size  ****/
      i = fseek (fr,0L, SEEK_END);
      if (i != 0)
        {
          if (internal_mode & TRACE_GENDER)
            {
              printf ("Error: Could not position in dictionary file '%s'.\n",
                  FIRST_NAME_FILE);
            }
          return (-10L);
        }

      p = ftell (fr);
      record_count = (long) ((p+1L) / line_size);

      /****  reset cache variables  ****/
      for (i=0; i< CACHE_LOOKBACK_COUNT; i++)
        {
          cache_p[i] = -1L;
          strcpy (cache_text[i],"");
        }
    }

  /****  start binary search  ****/
  p1 = 0L;
  p2 = record_count;
  i = -1;
  n = -1;

  while (p1 <= p2)
    {
      p = (long) ((p1+p2) / 2L);
      n++;

      if (n < CACHE_LOOKBACK_COUNT  &&  p == cache_p[n])
        {
          /****  read data from cache  ****/
          strcpy (text, cache_text[n]);
        }
      else
        {
          /****  read from dictionary file  ****/
          i = fseek (fr, (line_size * p), SEEK_SET);
          if (i != 0)
            {
              if (internal_mode & TRACE_GENDER)
                {
                  printf ("Could not position in dictionary file '%s'.\n",
                      FIRST_NAME_FILE);
                }
              return (-1L);
            }

          read_line (fr, text, MAX_LINE_SIZE+1);

          if (n < CACHE_LOOKBACK_COUNT)
            {
              /****  copy data into cache  ****/
              cache_p[n] = p;
              strcpy (cache_text[n], text);
            }
        }

      strcpy (temp,"");
      if (text[0] != '#'  &&  (int)strlen (text) > offset)
        {
          copycut (temp, text+offset, len_s);
        }

      if (internal_mode & TRACE_GENDER)
        {
          printf ("Range = line %ld - %ld,  guess = %ld ('%s')\n",
              p1+1L, p2+1L, p+1L, temp);
        }

      /****  compare first names  ****/
      i = strcmp_search (search_name, temp, (compare_mode | IGNORE_SEPARATOR),
              text[POS_UMLAUT_INFO], NULL);

      if (i == 0)
        {
          if (p == p1)
            {
              /****  first match has been found  ****/
              break;
            }
          p2 = p;
        }
      else if (i < 0)
        {
          p2 = p - 1L;
        }
      else
        {
          p1 = p + 1L;
          p++;   /**** important  ****/
        }
    }

  if (i != 0  &&  (compare_mode & GET_MATCH_OR_NEXT_HIGHER))
    {
      if (i > 0)
        {
          /***  read next line  ****/
          read_line (fr, text, MAX_LINE_SIZE+1);
          copycut (temp, text+offset, len_s);
        }
      i = 0;
    }

  if (i == 0)
    {
      /****  match has been found  ****/
      if (internal_mode & TRACE_GENDER)
        {
          printf ("Result: name '%s' found\n", temp);
        }
      return (line_size * p);
    }

  /****  no match found  ****/
  if (internal_mode & TRACE_GENDER)
    {
      printf ("Result: name '%s' not found\n", search_name);
    }
  return (-1L);
}





/************************************************************/
/****  string functions for unicode and utf-8  **************/
/************************************************************/


static int delete_chars_to_ignore
    (char dest[], char src[], int flag_sh, int len)
{
  int i,k,n,t;

  /****  delete "CHARS_TO_IGNORE"  ****/
  i = 0;
  k = 0;
  n = 0;
  t = 0;
  while (src[i] != '\0')
    {
      if (sortchar [(unsigned char) src[i]] == IS_CHAR_TO_IGNORE)
        {
          if ((flag_sh & (1|2))
          &&  src[i] == '<'
          && (src[i+1] == 'S'  ||  src[i+1] == 's')
          && (src[i+2] == ','  ||  src[i+2] == '^')
          &&  src[i+3] == '>'  &&  k < len-3)
            {
              dest[k] = src[i+1];
              k++;
              if (flag_sh & 2)
                {
                  dest[k] = 'c';
                  k++;
                }
              dest[k] = 'h';
              k++;
              t += 3;
              i += 3;
            }
          else if (flag_sh == 0
          &&  src[i] == '<'
          && (src[i+1] == 'S'  ||  src[i+1] == 's')
          && (src[i+2] == 'H'  ||  src[i+2] == 'h')
          &&  src[i+3] == '>'  &&  k < len-1)
             {
               dest[k] = src[i+1];
               k++;
               t += 3;
               i += 3;
             }
          else if (src[i] != '\'')
            {
              n++;
              t++;
            }
          else
            {
              dest[k] = src[i];
              k++;
            }
        }
      else
        {
          dest[k] = src[i];
          k++;
        }

      while (t > 0  &&  k < len-1
      &&  src[i] == ' '  &&  src[i+1] == ' '  &&  src[i+2] == ' ')
        {
          dest[k] = ' ';
          k++;
          t--;
        }

      i++;
    }

  dest[k] = '\0';
  return (n);
}



static void conv_from_unicode_line (char dest[], char src[], int len)
{
  char *s,text[11];
  int i,k,n,x;

  /****  conversion from unicode line  ****/
  i = 0;
  k = 0;
  while (k < len-1  &&  (src[i] != '\0'  ||  src[i+1] != '\0'))
    {
      x = (unsigned char) src[i];
      i++;
      x += 256 * (unsigned char) src[i];
      i++;

      /*** print char ***/
      if (x < 256)
        {
          dest[k] = (char) x;
          k++;
        }
      else
        {
          for (n=0; (s=umlauts_unicode[n]) != NULL; n += 2)
            {
              if (atoi (s) == x)
                {
                  s = umlauts_unicode[n+1];

                  while (k < len-1  &&  *s != '\0')
                    {
                      dest[k] = *s;
                      k++;
                      s++;
                    }

                  x = -1;
                  break;
                }
            }

          if (x > 0)
            {
              /****  unknown unicode char  ****/
              sprintf (text, "<#%03d>", x);
              s = text;
              while (k < len-1  &&  *s != '\0')
                {
                  dest[k] = *s;
                  k++;
                  s++;
                }
            }
        }
    }

  dest[k] = '\0';
}



static void make_unicode_line (char dest[], char src[], int two_byte_len)
{
  char *s;
  int i,k,n,t,x,ls;

  /****  make unicode line  ****/
  i = 0;
  k = 0;
  t = 0;
  while (src[i] != '\0'  &&  k < 2*two_byte_len -2)
    {
      x = -1;
      n = 0;
      for (n=1; (s=umlauts_unicode[n]) != NULL; n += 2)
        {
          if (*s == src[i])
            {
              ls = (int) strlen (s);

              if (strncmp (s,src+i,ls) == 0)
                {
                  t += ls - 1;
                  x = n - 1;

                  break;
                }
            }
        }

      if (x >= 0)
        {
          n = atoi (umlauts_unicode[x]);
        }
      else
        {
          n = src[i];
          ls = 1;
        }

      dest[k] = (char) (n%256);
      k++;
      dest[k] = (char) (n/256);
      k++;
      i += ls;

      while (t > 0  &&  k < 2*two_byte_len -2
      &&  src[i] == ' '  &&  src[i+1] == ' '  &&  src[i+2] == ' ')
        {
          n = ' ';
          dest[k] = (char) (n%256);
          k++;
          dest[k] = (char) (n/256);
          k++;
          t--;
        }
    }

  dest[k] = '\0';
  dest[k+1] = '\0';
}



static void conv_from_utf8_line (char dest[], char src[], int len)
{
  char *s,text[11];
  int i,k,n,x;

  /****  conversion from utf-8 line  ****/
  i = 0;
  k = 0;
  while (k < len-1  &&  src[i] != '\0')
    {
      /****  read char:
          0xxx xxxx
       or 110x xxxx  10xx xxxx
       or 1110 xxxx  10xx xxxx  10xx xxxx
       or 1111 0xxx  10xx xxxx  10xx xxxx  10xx xxxx
      ****/

      x = (unsigned char) src[i];
      i++;

      if (x >= 128)
        {
          if ((x & (128+64+32)) == 128+64)
            {
              x = x & ~(128+64);

              n = (unsigned char) src[i];
              i++;
              x = 64 * x + (n-128);
            }
          else if ((x & (128+64+32+16)) == 128+64+32)
            {
              x = x & ~(128+64+32);

              n = (unsigned char) src[i];
              i++;
              x = 64 * x + (n-128);

              n = (unsigned char) src[i];
              i++;
              x = 64 * x + (n-128);
            }
          else
            {
              /****  unknown char  ****/
              x = -123;
            }
        }

      /*** print char ***/
      if (x < 0)
        {
          /****  unknown char  ****/
          dest[k] = '.';
          k++;
        }
      else if (x < 256)
        {
          dest[k] = (char) x;
          k++;
        }
      else
        {
          for (n=0; (s=umlauts_unicode[n]) != NULL; n += 2)
            {
              if (atoi (s) == x)
                {
                  s = umlauts_unicode[n+1];

                  while (k < len-1  &&  *s != '\0')
                    {
                      dest[k] = *s;
                      k++;
                      s++;
                    }

                  x = -1;
                  break;
                }
            }

          if (x > 0)
            {
              /****  unknown unicode char  ****/
              sprintf (text, "<#%03d>", x);
              s = text;
              while (k < len-1  &&  *s != '\0')
                {
                  dest[k] = *s;
                  k++;
                  s++;
                }
            }
        }
    }

  dest[k] = '\0';
}



static void make_utf8_line (char dest[], char src[], int len)
{
  char *s;
  int i,k,n,t,x,ls;

  /****  make utf-8 line  ****/
  i = 0;
  k = 0;
  t = 0;
  while (src[i] != '\0'  &&  k < len-1)
    {
      x = -1;
      n = 0;
      for (n=1; (s=umlauts_unicode[n]) != NULL; n += 2)
        {
          if (*s == src[i])
            {
              ls = (int) strlen (s);

              if (strncmp (s,src+i,ls) == 0)
                {
                  t += ls - 1;
                  x = n - 1;

                  break;
                }
            }
        }

      if (x >= 0)
        {
          n = atoi (umlauts_unicode[x]);
        }
      else
        {
          n = (unsigned char) src[i];
          ls = 1;
        }

      if (n < 128)
        {
          /****  "simple" char  ****/
          dest[k] = (char) n;
          k++;
        }
      else if (n < 2048  &&  k < len-2)
        {
          /****  "European" utf8 char  ****/
          x = 128+64 + (int)(n/64);
          dest[k] = (char) x;
          k++;

          x = 128 + (n%64);
          dest[k] = (char) x;
          k++;
        }
      else if (k < len-3)
        {
          /****  other utf8 char  ****/
          x = 128+64+32 + (int)(n/4096);
          dest[k] = (char) x;
          k++;

          n = n % 4096;
          x = 128 + (int)(n/64);
          dest[k] = (char) x;
          k++;

          x = 128 + (n%64);
          dest[k] = (char) x;
          k++;
        }
      i += ls;

      while (t > 0  &&  k < len-1
      &&  src[i] == ' '  &&  src[i+1] == ' '  &&  src[i+2] == ' ')
        {
          dest[k] = ' ';
          k++;
          t--;
        }
    }

  dest[k] = '\0';
}





/************************************************************/
/**** Levenshtein function and the like  ********************/
/************************************************************/


static int up_conv_g (char dest[], char src[], int len)

/****  Function converts "src" to upper case.               ****/
/****  The conversion is done using the array "up_and_conv" ****/
/****  (len = size of "dest" incl. '\0').                   ****/
/****  This function returns "strlen (dest)".               ****/
{
  int n;

  if (! (internal_mode & GENDER_INITIALIZED))
    {
      n = initialize_gender();

      if (n < 0  ||  ! (internal_mode & GENDER_INITIALIZED))
        {
          return (INTERNAL_ERROR_GENDER);
        }
    }

  n = 0;
  while (*src != '\0'  &&  n < len-1)
    {
      /****  find next char in "src"  ****/
      dest[n] = up_and_conv [(unsigned char) *src];
      n++;
      src++;
    }

  dest[n] = '\0';
  return (n);
}




static int calculate_limit_g (char pattern[], char text[])
/****  calculate limit for the Levenshtein function  ****/
{
  char *s;
  int  i,j,k,n,x;
  int  max_points, limit;
  int  asterisk_found, unknown;

  char wildcard_any_string = MATCHES_ANY_STRING;
  char wildcard_any_char = MATCHES_ANY_CHAR;

  if (pattern == NULL)
    {
     pattern = text;
     text = NULL;
    }
  max_points = 0;
  unknown = 0;

  for (j=1; j<=2; j++)
    {
     s = pattern;
     if (j == 2)
       {
        s = text;
       }

     if (s == NULL)
       {
        continue;
       }

     asterisk_found = 0;
     x = 0;
     i = 0;
     while (s[i] != '\0')
       {
        /****  look for word separators and wildcard_any_string/char  ****/
        k = 0;
        n = 0;
        while (s[i] == wildcard_any_string
        ||  s[i] == '.'  ||  s[i] == wildcard_any_char)
          {
           if (s[i] == wildcard_any_string
           ||  s[i] == '.')
             {
              asterisk_found = 1;
              n = 300;
             }
           if (s[i] == wildcard_any_char)
             {
              k += 100;
             }

           if (s[i+1] != wildcard_any_string
           &&  s[i+1] != '.'  &&  s[i] != wildcard_any_char)
             {
              break;
             }
           i++;
          }

        if (n > k)
          {
           k = n;
          }
        unknown += k;

        if (strchr (WORD_SEPARATORS, s[i]) == NULL)
          {
           /****  count "letters"  ****/
           x += 100;
          }
        else if (s[i] != wildcard_any_string
        &&  s[i] != '.'  &&  s[i] != wildcard_any_char)
          {
           x += 40;
          }

        i++;
       }

     if (asterisk_found  &&  unknown < 600 - x)
       {
        unknown = 600 - x;
       }

     if (j == 1  ||  x < max_points)
       {
        max_points = x;
       }
    }

  limit = max_points;
  if (limit > 0)
    {
     if (limit > 1200)
       {
        limit = 1200;
       }

     limit = (int) ((limit +40) / 3);
    }

  return (limit);
}



static void print_number_g (char *text, int number, int mode)
{
  char *s;

  if (mode >= 0  &&  number == MATCHES_ALL)
    {
     printf ("%s = %s", text, "MATCHES_ALL");
     return;
    }

  s = "";
  if (number < 0)
    {
     s = "-";
     number = -number;
    }
  printf ("%s = %s%d", text,s, (int)(number/100));

  if (number % 100 != 0)
    {
     number = number % 100;
     printf (".%d", (int)(number/10));
     
     if (number % 10 != 0)
       {
        printf ("%d", number % 10);
       }
    }
}



static int lev_diff_g
   (char *pattern, char *text, int limit, int *trigrams, int run_mode)

/****  Function for calculating Levenshtein distance         ****/
/****  (in hundredths of points) with respect to wildcards   ****/
/****  and phonetic rules.                                   ****/
/****  "text" is compared with "pattern"; allowed wildcards  ****/
/****  are <wildcard_any_string> and <wildcard_any_char>.    ****/
/****  (function works for arbitrary string lengths)         ****/

/****  This function does not discriminate between           ****/
/****  lower / upper char, including umlauts                 ****/
/****  and is also "symmetric": shorter string = pattern;    ****/
/****  "limit" may be re-calculated                          ****/

/****  If "limit >= 0", the function returns "no match",     ****/
/****  if the minimum distance (as given by "col_min")       ****/
/****  is greater than limit.                                ****/

/****  Available values for "run_mode" (bit mask):           ****/
/****  LEV_COMPARE_NORMAL :  "normal" Levenshtein distance   ****/
/****  LEV_G_SKIP_UPEXPAND  :  do not convert strings to       ****/
/****                       upper char (incl. umlaut_expand) ****/
/****  LEV_G_COMPARE_GERMAN :  use simple phonetic algorithm   ****/
/****                        for German                      ****/
/****  TRACE_LEV_G          :  activate trace option           ****/
{
 int  col_min,
      lim_1,
      p,q,r,
      lt,lp,
      d1,d2,
      i,k,n,
      x1,x2,x3,x4,
      y1,y2,y3;
 int  *d;
 int  *pos;
 int  *rep;
 int  *tri;
 char *pp;
 char *tt;
 char ct,cp,cp2,
      *s1,*s2,*s3,*s4;

 char wildcard_any_string = MATCHES_ANY_STRING;
 char wildcard_any_char = MATCHES_ANY_CHAR;

 static char p_string[51],
             t_string[51];
 static int  d_array[51];
 static int  pos_array[51];
 static int  rep_array[51];
 static int  tri_array[51];

 d = d_array;
 pos = pos_array;
 rep = rep_array;
 tri = tri_array;
 pp = p_string;
 tt = t_string;

 if (! (internal_mode & GENDER_INITIALIZED))
   {
     i = initialize_gender();

     if (i < 0  ||  ! (internal_mode & GENDER_INITIALIZED))
       {
         if (internal_mode & TRACE_GENDER)
           {
             printf ("Error: initialization failed\n");
           }
         return (INTERNAL_ERROR_GENDER);
       }
   }

 if (run_mode & LEV_G_SKIP_UPEXPAND)
   {
    lt = (int) strlen (text);
    lp = (int) strlen (pattern);
   }
 else
   {
    /****  convert strings to upper char  ****/
    lt = up_conv_g (tt, text, 51);
    lp = up_conv_g (pp, pattern, 51);

    if (lt >= 50)
      {
       lt = (int) strlen (text) + 2;
       tt = (char *) malloc ((unsigned) lt);

       if (tt == NULL)
         {
          if (run_mode & (TRACE_GENDER | TRACE_LEV_G))
            {
             printf ("Error: \"malloc\" for %d Bytes failed.\n", lt);
            }
          return (10000);
         }
       lt = up_conv_g (tt, text, lt);
      }

    if (lp >= 50)
      {
       lp = (int) strlen (pattern) + 2;
       pp = (char *) malloc ((unsigned) lp);

       if (pp == NULL)
         {
          if (run_mode & (TRACE_GENDER | TRACE_LEV_G))
            {
             printf ("Error: \"malloc\" for %d Bytes failed.\n", lp);
            }
          if (tt != t_string)
            {
             free (tt);
            }
          return (10000);
         }
       lp = up_conv_g (pp, pattern, lp);
      }

    text = tt;
    pattern = pp;
   }

 if (lt < 0  ||  lp < 0)
   {
     /****  internal error  ****/
     if (run_mode & (TRACE_GENDER | TRACE_LEV_G))
       {
        printf ("Internal error: could not initialize conv strings.\n");
       }
     return (10000);
   }

 if (3*lt < 2*lp
 && (strchr (text,wildcard_any_string) != NULL
 ||  strchr (text,' ') != NULL  ||  strchr (text,'.') != NULL))
   {
    /****  switch "text" and "pattern"  ****/
    s1 = text;
    text = pattern;
    pattern = s1;
    i = lt;
    lt = lp;
    lp = i;

    if (limit > 0  &&  limit < 10000)
      {
       /****  re-calculate limit  ****/
       x1 = calculate_limit_g (pattern, NULL);
       if (x1 < limit)
         {
           limit = x1;
         }
      }
   }

 if (limit < 0)
   {
    limit = 0;
   }
 lim_1 = limit;
 if (lim_1 >= 7  &&  lim_1 <= 30)
   {
     lim_1 += 10;
   }

 if (lt >= 50)
   {
    i = (lt+1) * sizeof (int);
                      /***  factor "4" includes arrays "pos", "rep" and "tri"  ***/
    d = (int *) malloc ((unsigned) (4*i));

    if (d == NULL)
      {
       if (run_mode & (TRACE_GENDER | TRACE_LEV_G))
         {
          printf ("Error: \"malloc\" for %d Bytes failed.\n", (4*i));
         }
       if (tt != t_string  &&  tt != p_string)
         {
          free (tt);
         }
       if (pp != p_string  &&  pp != t_string)
         {
          free (pp);
         }
       return (10000);
      }

    pos = d + i;
    rep = d + (2*i);
    tri = d + (3*i);
   }

 if (run_mode & TRACE_LEV_G)
   {
    printf ("\nLevenshtein:  Strings = '%s'%s'", pattern, text);
    print_number_g (",  Limit", limit,-1);
    printf ("\n\n         ");

    for (k=1; k<=lt; k++)
      {
       printf ("   '%c'", text[k-1]);
      }
    printf ("\n");
   }

 /****  calculate initial values ( = zero'th column)  ****/
 d[0] = 0;
 pos[0] = -1;
 rep[0] = 10000;
 tri[0] = 0;
 for (k=1; k<=lt; k++)
   {
    d[k] = 10000;
    pos[k] = -1;
    rep[k] = 10000;
    tri[k] = 0;
   }

 /****  calculate distance matrix  ****/
 for (i=0; i<=lp; i++)
   {
    cp = (i == 0) ?  '\0' : *(pattern +i-1);

    if (cp == wildcard_any_string  ||  cp == '.')
      {
        p = 0;
        q = 0;
        r = 0;
      }
    else
      {
        p = (cp == wildcard_any_char) ?  0 : 100;
        q = 100;
        r = 100;
      }

    if (q > 0)
      {
       /****  calculate error points for "delete"  ***/
       if (i >= 2  &&  cp == pattern[i-2])
         {
          /****  "<x><x>"  -->  "<x>"  ****/
          q = 5;
         }

       switch (cp)
         {
          case ' ':
          case '-':
          case '+':
          case '/':
          case '&':
          case '\'':  q = 5;
                 break;
         }

       if ((run_mode & LEV_G_COMPARE_GERMAN)
       &&  i > 0)
         {
           /****  check phonetic rules for "delete"  ***/
           x3 = ph_rules_hash_del_ins [(unsigned char) cp];

           if (x3 >= 0)
             {
               /****  evaluate phonetic rules  ****/
               while (ph_rules_german[x3].text_2 != NULL
               &&  strcmp (ph_rules_german[x3].text_2,"") == 0)
                 {
                   s1 = ph_rules_german[x3].text_1;
                   y1 = ph_rules_german[x3].len_1;

                   s4 = pattern+i-1;

                   if (cp == *s1  &&  y1 > 0)
                     {
                       /****  check phonetic rule  ****/
                       if (strncmp (s4, s1,y1) == 0)
                         {
                           y3 = 0;

                           if (pos[y3] < 0
                           ||  rep[y3]-100 > d[0] + ph_rules_german[x3].ph_diff)
                             {
                               pos[y3] = i + y1-1;
                               rep[y3] = d[0] + ph_rules_german[x3].ph_diff;

                               if (y1 >= 2  &&  y3 < lt
                               &&  ph_rules_german[x3].ph_diff >= 100)
                                 {
                                   pos[y3] |= 2048;
                                 }
                             }
                         }
                     }

                   x3++;
                 }
             }
         }
      }

    d2 = d[0];
    d[0] = (i == 0) ?  0 : (d2 + q);

    if ((run_mode & LEV_G_COMPARE_GERMAN)
    &&  (pos[0] & ~(1024+2048)) == i)
      {
        x1 = rep[0];
        if (x1 < d[0])
          {
            d[0] = x1;
          }

        if ((pos[0] & 2048)  &&  rep[1] > rep[0])
          {
            pos[1] = pos[0] & ~2048;
            rep[1] = rep[0];
          }
        pos[0] = -1;
        rep[0] = 10000;
      }

    col_min = d[0];

    for (k=1; k<=lt; k++)
      {
       /****  d[k] = minimum of three numbers  ****/
       d1 = d2;
       d2 = d[k];
       ct = text[k-1];
       n = (cp == ct) ?  0 : p;

       if (tri[k-1] > tri[k])
         {
          tri[k] = tri[k-1];
         }

       if (n == 0  &&  p > 0  &&  i <= lp-2  &&  k <= lt-2)
         {
          /****  look for trigram  ****/
          if (pattern[i] == text[k]
          &&  pattern[i+1] == text[k+1]
          &&  text[k] != wildcard_any_string  &&  text[k] != '.'
          &&  text[k] != wildcard_any_char
          &&  text[k+1] != wildcard_any_string  &&  text[k+1] != '.'
          &&  text[k+1] != wildcard_any_char)
            {
              tri[k] += 100;
            }
         }

       if (i >= 2  &&  k >= 2  &&  n != 0  &&  limit > 0)
         {
          /****  look for transpositions (e.g. "AB" --> "BA")  ****/
          cp2 = pattern[i-2];

          if (cp == text[k-2]  &&  cp2 == text[k-1]
          &&  cp != wildcard_any_string  &&  cp != '.'
           &&  cp != wildcard_any_char
          &&  cp2 != wildcard_any_string  &&  cp2 != '.'
           &&  cp2 != wildcard_any_char)
            {
              /****  transposition found  ****/
              n = 0;
            }
         }

       if (n > 0  &&  limit > 0)
         {
          /****  calculate error points for "replacement"  ***/
          switch (cp)
            {
             case ' ':
             case '-':
             case '+':
             case '/':
             case '&':
             case '\'': if (ct == ' '  ||  ct == '-'  ||  ct == '+'
                        ||  ct == '/'  ||  ct == '&'  ||  ct == '\''
                        ||  ct == wildcard_any_string)
                          {
                           /****  separator found  ****/
                           n = 5;
                          }
                   break;
            }
         }

       if ((run_mode & LEV_G_COMPARE_GERMAN)
       &&  i > 0)
         {
           /****  check phonetic rules for "replacement"  ***/
           x1 = (ph_rules_hash_replace [(unsigned char) ct])
               & (ph_rules_hash_replace [(unsigned char) cp]);

           x2 = 1;

           if (x1 != 0)
             {
               if (! (x1 & (1+2+4+8+16+32+64+128)))
                 {
                   x1 = x1 >> 8;
                   x2 += 8;
                 }
               if (! (x1 & (1+2+4+8)))
                 {
                   x1 = x1 >> 4;
                   x2 += 4;
                 }
               if (! (x1 & (1+2)))
                 {
                   x1 = x1 >> 2;
                   x2 += 2;
                 }
             }

           while (x1 != 0)
             {
               if (x1 & 1)
                 {
                   /****  hash_group found  ****/
                   x3 = ph_hash_group_begin[x2];
                   x4 = ph_hash_group_end[x2];

                   while (x3 <= x4  &&  x3 >= 0)
                     {
                       s1 = ph_rules_german[x3].text_1;
                       s2 = ph_rules_german[x3].text_2;

                       y1 = ph_rules_german[x3].len_1;
                       y2 = ph_rules_german[x3].len_2;

                       y3 = y2;
                       if (y1 > y3)
                         {
                           y3 = y1;
                         }

                       s3 = text+k-1;
                       s4 = pattern+i-1;

                       if (ct == *s1  &&  cp == *s2
                       &&  y1 > 0  &&  y2 > 0)
                         {
                           /****  check phonetic rule  ****/
                           if (strncmp (s3, s1,y1) == 0
                           &&  strncmp (s4, s2,y2) == 0
                           &&  strncmp (s3,s4, y3) != 0)
                             {
                               y3 = k + y1-1;

                               if (pos[y3] < 0
                               ||  rep[y3] > d1 + ph_rules_german[x3].ph_diff)
                                 {
                                   pos[y3] = i + y2-1;
                                   rep[y3] = d1 + ph_rules_german[x3].ph_diff;
                                 }
                             }
                         }

                       if (ct == *s2  &&  cp == *s1
                       &&  y1 > 0  &&  y2 > 0)
                         {
                           /****  check phonetic rule  ****/
                           if (strncmp (s3, s2,y2) == 0
                           &&  strncmp (s4, s1,y1) == 0
                           &&  strncmp (s3,s4, y3) != 0)
                             {
                               y3 = k + y2-1;

                               if (pos[y3] < 0
                               ||  rep[y3] > d1 + ph_rules_german[x3].ph_diff)
                                 {
                                   pos[y3] = i + y1-1;
                                   rep[y3] = d1 + ph_rules_german[x3].ph_diff;
                                 }
                             }
                         }

                       x3++;
                     }
                 }

               x1 = x1 >> 1;
               x2++;
             }

           /****  check phonetic rules for "delete"  ***/
           x3 = ph_rules_hash_del_ins [(unsigned char) cp];

           if (x3 >= 0)
             {
               while (ph_rules_german[x3].text_2 != NULL
               &&  strcmp (ph_rules_german[x3].text_2,"") == 0)
                 {
                   s1 = ph_rules_german[x3].text_1;
                   y1 = ph_rules_german[x3].len_1;

                   s4 = pattern+i-1;

                   if (cp == *s1  &&  y1 > 0)
                     {
                       /****  check phonetic rule  ****/
                       if (strncmp (s4, s1,y1) == 0)
                         {
                           y3 = k;

                           if (pos[y3] < 0
                           ||  rep[y3]-100 > d2 + ph_rules_german[x3].ph_diff)
                             {
                               pos[y3] = i + y1-1;
                               rep[y3] = d2 + ph_rules_german[x3].ph_diff;

                               if (y1 >= 2  &&  y3 < lt
                               &&  ph_rules_german[x3].ph_diff >= 100)
                                 {
                                   pos[y3] |= 2048;
                                 }
                             }
                         }
                     }

                   x3++;
                 }
             }
         }

       if (run_mode & LEV_G_COMPARE_GERMAN)
         {
           /****  check phonetic rules for "insert"  ***/
           x3 = ph_rules_hash_del_ins [(unsigned char) ct];

           if (x3 >= 0)
             {
               while (ph_rules_german[x3].text_2 != NULL
               &&  strcmp (ph_rules_german[x3].text_2,"") == 0)
                 {
                   s1 = ph_rules_german[x3].text_1;
                   y1 = ph_rules_german[x3].len_1;

                   s3 = text+k-1;

                   if (ct == *s1  &&  y1 > 0)
                     {
                       /****  check phonetic rule  ****/
                       if (strncmp (s3, s1,y1) == 0)
                         {
                           y3 = k + y1-1;

                           if (pos[y3] < 0
                           ||  rep[y3] > d[k-1] + ph_rules_german[x3].ph_diff)
                             {
                               pos[y3] = i;
                               rep[y3] = d[k-1] + ph_rules_german[x3].ph_diff;

                               if (y1 >= 2  &&  i < lp
                               &&  ph_rules_german[x3].ph_diff >= 100)
                                 {
                                   pos[y3] |= 1024;
                                 }
                             }
                         }
                     }

                   x3++;
                 }
             }
         }

       if (n >= 100
       &&  up_and_conv [(unsigned char) ct]
        == up_and_conv [(unsigned char) cp])
         {
           n = 20;
           if (ct == up_and_conv [(unsigned char) cp]
           ||  cp == up_and_conv [(unsigned char) ct])
             {
               n = 5;
             }
         }

       x1 = d1 + n;
       x2 = d2 + q;

       if (x2 < x1)
         {
          x1 = x2;
         }

       n = r;
       if (n > 0  &&  limit > 0)
         {
          /****  calculate error points for "insert"  ***/
          if (k >= 2  &&  ct == text[k-2])
            {
             /****  "<x>"  -->  "<x><x>"  ****/
             n = 5;
            }

          if (ct == wildcard_any_string)
            {
             n = 5;
            }

          switch (ct)
            {
          /** case wildcard_any_string: **/
             case ' ':
             case '-':
             case '+':
             case '/':
             case '&':
             case '\'':  n = 5;
                   break;
            }
         }

       x2 = d[k-1] + n;
       d[k] = (x1 < x2) ?  x1 : x2;

       if ((run_mode & LEV_G_COMPARE_GERMAN)
       &&  (pos[k] & ~(1024+2048)) == i)
         {
           x1 = rep[k];
           if (x1 < d[k])
             {
               d[k] = x1;
             }

           if (pos[k] & 1024)
             {
               pos[k] = (pos[k] + 1) & ~(1024+2048);
             }
           else
             {
               if ((pos[k] & 2048)  &&  rep[k+1] > rep[k])
                 {
                   pos[k+1] = pos[k] & ~2048;
                   rep[k+1] = rep[k];
                 }
               pos[k] = -1;
               rep[k] = 10000;
             }
         }

       if (d[k] < col_min)
         {
          col_min = d[k];
         }
      }

    if (run_mode & TRACE_LEV_G)
      {
       if (i == 0)
         {
          printf ("    ");
         }
       else
         {
          printf ("'%c' ",cp);
         }

       for (k=0; k<=lt; k++)
         {
          if (d[k] <= limit)
            {
             printf (" %2d.%02d", (int)(d[k]/100), d[k]%100);
            }
          else
            {
             printf ("  ----");
            }
         }
       printf ("\n");
      }

    if (col_min > limit + tri[lt])
      {
       break;
      }
   }


 if (d[lt] >= 150  &&  d[lt] <= 800
 &&  d[lt] <= lim_1)
   {
     /****  check for multiple insert  ****/
     k = lt - lp;
     s1 = text;
     s2 = pattern;

     if (k < 0)
       {
         k = - k;
         s1 = pattern;
         s2 = text;
       }

     i = 0;
     while (s1[i] == s2[i]  &&  s1[i] != '\0')
       {
         i++;
       }

     if (strcmp (s1+i+k, s2+i) == 0)
       {
         /****  multiple insert  ****/
         if ((i > 0  &&  s2[i] == '\0'
          && (s1[i] == ' '  ||  s1[i] == '-'))
         ||  (i == 0  &&  k >= 2
          && (s1[k-1] == ' '  ||  s1[k-1] == '-')))
           {
             k = 50 * k;    /** 100 * (k-1 + 1) / 2 **/

             if (k < d[lt])
               {
                 d[lt] = k;

                 if (run_mode & TRACE_LEV_G)
                   {
                     printf ("\nMultiple insert found (1):  lev = %d.%02d\n",
                         (int) (d[lt] / 100), d[lt] % 100);
                   }
               }
           }
         else if (i >= 2  &&  k >= 3)
           {
             d[lt] -= 50;
             if (k >= 5)
               {
                 d[lt] -= 50;
               }

             if (run_mode & TRACE_LEV_G)
               {
                 printf ("\nMultiple insert found (2):  lev = %d.%02d\n",
                     (int) (d[lt] / 100), d[lt] % 100);
               }
           }
       }
     else
       {
         i = lt;
         k = lp;
         while (text[i-1] == pattern[k-1]  &&  i > 0  &&  k > 0)
           {
             i--;
             k--;
           }

         if (100 * i == d[lt]  ||  100 * k == d[lt])
           {
             i = 0;
             while (text[i] == pattern[i]  &&  text[i] != '\0')
               {
                 i++;
               }

             if (i == 0)
               {
                 /****  begin of strings differ       ****/
                 /****  (e.g. "Hedwig" and "Ludwig")  ****/
                 d[lt] += 50;

                 if (run_mode & TRACE_LEV_G)
                   {
                     printf ("\nBegin of string differ:  lev = %d.%02d\n",
                         (int) (d[lt] / 100), d[lt] % 100);
                   }
               }
           }
       }
   }

 /****  check trigrams  ****/
 x1 = 15000;
 if (tri[lt] >= d[lt]  &&  d[lt] > 0)
   {
     i = (int) (15 * (lt+lp));

     x1 = i - (int)(tri[lt] / 3);
     if (x1 < 100)
       {
         x1 = 100;
       }
   }
 else if (tri[lt] >= 200  &&  d[lt] > 500)
   {
     x1 = 1000 - (int)(tri[lt] / 2);
     if (x1 < 400)
       {
         x1 = 400;
       }
   }

 if (x1 <= limit + 100  &&  x1 < d[lt] - 100)
   {
     d[lt] = x1 + 100;
     if (d[lt] > limit)
       {
         limit = d[lt];
       }

     if (run_mode & TRACE_LEV_G)
       {
         printf ("\nTrigams:  lev = %d.%02d\n",
             (int)(d[lt]/100), d[lt]%100);
       }
   }

 /****  calculate final result  ****/
 k = tri[lt];
 n = d[lt];
 if (n > limit  &&  n > k)
   {
    n = 10000;
   }

 if (tt != t_string  &&  tt != p_string)
   {
    free (tt);
   }
 if (pp != p_string  &&  pp != t_string)
   {
    free (pp);
   }
 if (d != d_array)
   {
    free (d);
   }

 if (run_mode & TRACE_LEV_G)
   {
    printf ("\n");
    printf ("Levenshtein distance = %d.%02d\n", (int)(n/100), (n%100));
    print_number_g ("trigram count", k,-1);
    printf ("\n\n");
   }

 if (trigrams != NULL)
   {
    *trigrams = k;
   }
 return (n);
}





/************************************************************/
/****  functions needed by "internal_search"  ***************/
/************************************************************/

static int conv_internal_result (char *text, int compare_mode)
{
  if (compare_mode & SEARCH_GENDER)
    {
      if (compare_mode & SEARCH_FIRST)
        {
          if (strncmp (text,"1F",2) == 0)  return (IS_FEMALE);
          if (strncmp (text,"1M",2) == 0)  return (IS_MALE);
        }

      if (strncmp (text,"F ",2) == 0)  return (IS_FEMALE);
      if (strncmp (text,"1F",2) == 0)  return (IS_MOSTLY_FEMALE);
      if (strncmp (text,"?F",2) == 0)  return (IS_MOSTLY_FEMALE);

      if (strncmp (text,"M ",2) == 0)  return (IS_MALE);
      if (strncmp (text,"1M",2) == 0)  return (IS_MOSTLY_MALE);
      if (strncmp (text,"?M",2) == 0)  return (IS_MOSTLY_MALE);

      if (strncmp (text,"? ",2) == 0)  return (IS_UNISEX_NAME);

      return (NAME_NOT_FOUND);
    }

  if (compare_mode & SEARCH_EQUIV)
    {
      if (strncmp (text,"= ",2) == 0)  return (EQUIVALENT_NAMES);

      return (NAME_NOT_FOUND);
    }

  return (INTERNAL_ERROR_GENDER);
}



static int get_frequency (char c)
{
  int x;

  x = 0;
  if (isdigit ((unsigned char) c))
    {
      x = (unsigned char) c - '0';
    }
  else if (c != ' ')
    {
      x = (unsigned char) c - 'A' + 10;
      if (x > 13  ||  x < 0)
        {
          x = 13;
        }
    }

  return (x);
}



static int multiply_frequency (int a, int b)
{
  int x;

  if (a > 13)
    {
      a = 13;
    }
  if (b > 13)
    {
      b = 13;
    }
  
  if (a < 0)
    {
      return (b);
    }
  if (b < 0)
    {
      return (a);
    }

  x = a + b - 15;
  if (x < 0)
    {
      x = 0;
    }
  if (x == 0  &&  (a*b > 0  ||  a+b >= 9))
    {
      x = 1;
    }

  return (x);
}



static char trace_buffer[2001];

static void trace_info_into_buffer
    (char text[], char name[], char name2[], int res, char data[])
/****  "calculate" trace info  ****/
{
  int  i,x,len;
  int  pos_0;
  int  freq,freq_2;
  long p,count;
  char *s,*s2;

  switch (res)
    {
      case IS_FEMALE        :  s = "is female";            break;
      case IS_MOSTLY_FEMALE :  s = "is mostly female";     break;
      case IS_MALE          :  s = "is male";              break;
      case IS_MOSTLY_MALE   :  s = "is mostly male";       break;
      case IS_A_COUPLE      :  s = "is a couple";          break;
      case IS_UNISEX_NAME   :  s = "is unisex name";       break;

      case EQUIVALENT_NAMES :  s = "names are equivalent"; break;
      case NOT_EQUAL_NAMES  :  s = "names are not equal";  break;
      case NAME_NOT_FOUND   :  s = "name not found";       break;
      case ERROR_IN_NAME    :  s = "error in name";        break;

      case INTERNAL_ERROR_GENDER :  s = "internal error";  break;
      default                    :  s = "unknown error";   break;
    }

  strcpy (trace_buffer,"");
  len = 0;

  if (name2 != NULL)
    {
      len += sprintf (trace_buffer+len, "%s '%s'%s':  '%s'", text, name,name2, s);
    }
  else if (text[0] != '\0')
    {
      len += sprintf (trace_buffer+len, "%s '%s':  '%s'", text, name, s);
    }
  else
    {
      len += sprintf (trace_buffer+len, "%s:  '%s'", name, s);
    }

  if (data != NULL  ||  (internal_mode & GENDER_GET_COUNTRY))
    {
      /****  print country  ****/
      freq = 0;
      freq_2 = 0;
      pos_0 = -1;
      count = 1L;

      for (i=0; gc_data[i].country_text != NULL; i++)
        {
          x = 0;
          if (data != NULL  &&  *data != '\0')
            {
              x = get_frequency (data[gc_data[i].pos]);
            }
          else if ((internal_mode & GENDER_GET_COUNTRY)
          &&  gc_data[i].n > 0)
            {
              x = (int) (gc_data[i].n % 16);
            }

          if (x > freq)
            {
              freq = x;
            }
        }

      freq_2 = (int)((freq+1) / 2);
      if (freq_2 <= 1)
        {
          freq_2 = freq;
        }
      if (freq_2 < freq - 3)
        {
          freq_2 = freq - 3;
        }
      freq = 0;

      s = " (country =";
      for (i=0; gc_data[i].country_text != NULL; i++)
        {
          x = 0;

          if (data != NULL  &&  *data != '\0')
            {
              x = get_frequency (data[gc_data[i].pos]);
            }
          else if ((internal_mode & GENDER_GET_COUNTRY)
          &&  gc_data[i].n > 0)
            {
              x = (int) (gc_data[i].n % 16);
            }

          if (x > 0)
            {
              if (! (internal_mode & GENDER_TRACE_ONE_COUNTRY_ONLY))
                {
                  if (internal_mode & GENDER_TRACE_SHORT_COUNTRY_NAME)
                    {
                      s2 = gc_data[i].country_short;
                    }
                  else
                    {
                      s2 = gc_data[i].country_text;
                    }
                  len += sprintf (trace_buffer+len, " %s %s[%d]", s,s2,x);
                  s = "or";
                }

              if (x >= freq_2)
                {
                  p = (long) gc_data[i].weight;
                  if (p < 1L)
                    {
                      p = 1L;
                    }
                  if (gc_data[i].n & 1024)
                    {
                      /****  country has been found  ****/
                      if (p < 60L)
                        {
                          p = 60L;
                        }
                      p = 8L * p;
                    }
                  if (x > 1)
                    {
                      p *= (1L << x);
                    }

                  if (p > count  ||  pos_0 < 0
                  || (p == count  &&  x > freq))
                    {
                      pos_0 = i;
                      freq = x;
                      count = p;
                    }
                }
            }
        }

      if (! (internal_mode & GENDER_TRACE_ONE_COUNTRY_ONLY))
        {
          if (strcmp (s,"or") == 0)
            {
              len += sprintf (trace_buffer+len, ")");
            }
        }
      else
        {
          if (pos_0 >= 0)
            {
              if (internal_mode & GENDER_TRACE_SHORT_COUNTRY_NAME)
                {
                  s2 = gc_data[pos_0].country_short;
                }
              else
                {
                  s2 = gc_data[pos_0].country_text;
                }
              len += sprintf (trace_buffer+len, " (country=%s)", s2);
            }
        }
    }
}



static void trace_info
    (char text[], char name[], char name2[], int res, char data[])
/****  output trace info  ****/
{
  trace_info_into_buffer (text, name, name2, res, data);
  printf ("%s\n", trace_buffer);
}




/************************************************************/
/****  internal "workhorses"  *******************************/
/************************************************************/


static int internal_search
     (char search_name[], int compare_mode, int country)
/****  This function searches a single given name  ****/
{
  int  i,n,x;
  int  k,k0;
  int  freq,freq_2;
  int  pos_0,pos_x[7];
  int  freq_x[7];
  int  len,res;
  long count,count_2,count_x[7];
  long p,pos_f;
  char text [MAX_LINE_SIZE+1];
  char temp [DATA_NAME_LENGTH+1];

  if (internal_mode & TRACE_GENDER)
    {
      printf ("Searching for name '%s'", search_name);

      /****  check for country  ****/
      for (i=0; gc_data[i].country_text != NULL; i++)
        {
          if (country == gc_data[i].gc_country)
            {
              printf ("  (country = %s)", gc_data[i].country_text);
              gc_data[i].n |= 1024;
            }
        }
      printf ("\n");
    }

  if (f_names == NULL)
    {
      f_names = fopen (FIRST_NAME_FILE,"rb");
      if (f_names == NULL)
        {
          if (internal_mode & TRACE_GENDER)
            {
              printf ("Error: could not open dictionary file '%s'\n",
                  FIRST_NAME_FILE);
            }
          return (INTERNAL_ERROR_GENDER);
        }
    }

  if (compare_mode & SEARCH_GENDER)
    {
     i = (int)strlen (search_name);
     if (i == 0)
       {
         return (IS_UNISEX_NAME);
       }

     if (i >= 2  &&  search_name[i-1] == '.')
       {
         compare_mode |= COMPARE_ABBREVIATION;
         internal_mode &= ~GENDER_GET_COUNTRY;
       }
    }

  if (compare_mode & UMLAUT_EXPAND)
    {
      compare_mode |= EXPAND_PARAM_1;
    }

  pos_f = binary_search (f_names, search_name, DATA_NAME_POS,
            DATA_NAME_LENGTH+1, compare_mode);

  if (pos_f < 0L)
    {
      if (pos_f == -10L)
        {
          return (INTERNAL_ERROR_GENDER);
        }
      return (NAME_NOT_FOUND);
    }

  /****  read one or more first names in dictionary file  ****/
  res = NAME_NOT_FOUND;
  fseek (f_names, pos_f, SEEK_SET);

  for (x=0; x<7; x++)
    {
      pos_x[x] = 0;
      freq_x[x] = 0;
      count_x[x] = 0L;
    }
  k0 = 500;
  n = 0;

  do
    {
      read_line (f_names,text, MAX_LINE_SIZE+1);
      len = (int)strlen (text);

      strcpy (temp,"");
      if (text[0] != '#'  &&  len > DATA_NAME_POS)
        {
          copycut (temp, text+DATA_NAME_POS, DATA_NAME_LENGTH+1);
        }

      /****  compare first names  ****/
      i = strcmp_search (search_name, temp, compare_mode,
              text [POS_UMLAUT_INFO], & k);

      if (i == 0       /****  names are matching     and     ****/
      &&  k <= k0)     /****  umlauts don't differ too much  ****/
        {
          if (k < k0)
            {
              k0 = k;
              n = 0;
            }

          res = conv_internal_result (text, compare_mode);

          if (internal_mode & TRACE_GENDER)
            {
              trace_info ("evaluating name", temp,NULL, res,text);
            }

          if (compare_mode & COMPARE_ABBREVIATION)
            {
              if (res == IS_FEMALE)  res = IS_MOSTLY_FEMALE;
              if (res == IS_MALE)  res = IS_MOSTLY_MALE;

              i = (int)strlen (search_name);

              if (i <= 3
              || (i == 4  &&  ((n & 16)  ||  ((n & 2)  &&  (n & 8)))))
                {
                  return (IS_UNISEX_NAME);
                }
            }

          /****  check country  ****/
          freq = 0;
          freq_2 = 0;
          pos_0 = -1;
          count = 1L;

          for (i=0; gc_data[i].country_text != NULL; i++)
            {
              if (len <= (int) gc_data[i].pos)
                {
                  /****  line is too short  ****/
                  return (INTERNAL_ERROR_GENDER);
                }

              if (text [gc_data[i].pos] != ' ')
                {
                  x = get_frequency (text[gc_data[i].pos]);

                  if (x > freq)
                    {
                      freq = x;
                    }
                }
            }

          freq_2 = (int)((freq+1) / 2);
          if (freq_2 <= 1)
            {
              freq_2 = freq;
            }
          if (freq_2 < freq - 3)
            {
              freq_2 = freq - 3;
            }

          freq = 0;
          for (i=0; gc_data[i].country_text != NULL; i++)
            {
              if (text [gc_data[i].pos] != ' ')
                {
                  x = get_frequency (text[gc_data[i].pos]);

                  if (x >= freq_2)    /***  freq_2 !!  ****/
                    {
                      p = (long) gc_data[i].weight;
                      if (p < 1L)
                        {
                          p = 1L;
                        }
                      if (gc_data[i].n & 1024)
                        {
                          /****  country has been found  ****/
                          if (p < 60L)
                            {
                              p = 60L;
                            }
                          p = 8L * p;
                        }
                      if (x > 1)
                        {
                          p *= (1L << x);
                        }

                      if (p > count  ||  pos_0 < 0
                      || (p == count  &&  x > freq))
                        {
                          pos_0 = i;
                          freq = x;
                          count = p;
                        }
                    }

                  if (x > (int) (gc_data[i].n % 16))
                    {
                      x -= (int) (gc_data[i].n % 16);
                      gc_data[i].n += x;
                    }
                }
            }

          switch (res)
            {
              case IS_FEMALE        :  i =  1;  x = 0;  break;
              case IS_MOSTLY_FEMALE :  i =  2;  x = 1;  break;
              case IS_MALE          :  i =  4;  x = 2;  break;
              case IS_MOSTLY_MALE   :  i =  8;  x = 3;  break;
              case IS_UNISEX_NAME   :  i = 16;  x = 4;  break;
              case EQUIVALENT_NAMES :  i = 32;  x = 5;  break;
              default               :  i = 64;  x = 6;  break;
            }

          n |= i;
          if (count > count_x[x])
            {
              pos_x[x] = pos_0;
              freq_x[x] = freq;
              count_x[x] = count;
            }
        }

      /****  "strcmp_search" with mode "IGNORE_SEPARATOR" (!!)  ****/
      i = strcmp_search (search_name, temp, (compare_mode | IGNORE_SEPARATOR),
             text [POS_UMLAUT_INFO], & k);
    }while (i == 0);


  i = 0;
  count = 0L;
  count_2 = 0L;
  freq = 0;
  freq_2 = 0;

  for (x=0; x<7; x++)
    {
      if (count_x[x] > count
      || (count_x[x] == count  &&  freq_x[x] > freq))
        {
          i = 1 << x;
          count_2 = count;
          count = count_x[x];
          freq_2 = freq;
          freq = freq_x[x];
        }
      else if (count_x[x] > count_2
      || (count_x[x] == count_2  &&  freq_x[x] > freq_2))
        {
          count_2 = count_x[x];
          freq_2 = freq_x[x];
        }
    }

  if (freq_2 > 0)
    {
      /****  calculate the (truncated) binary logarithm of (count/count_2)  ****/
      x = 5;
      if (count > 0L  &&  count < count_2)
        {
          x = 0;
          while (count < count_2)
            {
              count *= 2L;
              x--;
            }
        }
      else if (count_2 > 0  &&  count_2 <= count)
        {
          x = -1;
          while (count_2 <= count)
            {
              count_2 *= 2;
              x++;
            }
        }
      k = freq - freq_2;

      if (country != GC_ANY_COUNTRY
      && (k+k + x >= 10
      || (k >= -1  &&  x >= 7)  ||  (k >= 0  &&  x >= 4)))
        {
          /****  ignore other countries  ****/
          n = i;
        }
      else if (k+k + x >= 6
      ||  (k >= -1  &&  x >= 4)  ||  (k >= 0  &&  x >= 2))
        {
          if (i == 1  ||  i == 2)
            {
              n &= ~(1|4|8|16);
              n |= 2;
            }
          else if (i == 4  ||  i == 8)
            {
              n &= ~(1|2|4|16);
              n |= 8;
            }
        }
    }

  if ((n & (1|2|16))  &&  (n & (4|8|16)))
    {
      return (IS_UNISEX_NAME);
    }

  if (n & 2)
    {
      return (IS_MOSTLY_FEMALE);
    }
  if (n & 8)
    {
      return (IS_MOSTLY_MALE);
    }

  if (n == 1)
    {
      return (IS_FEMALE);
    }
  if (n == 4)
    {
      return (IS_MALE);
    }

  return (res);
}





static int get_gender_internal
    (char first_name[], int compare_mode, int country)
/****  This function determines the gender of a given first name  ****/

/****  Result: IS_FEMALE        :  female first name            ****/
/****          IS_MOSTLY_FEMALE :  mostly female first name     ****/
/****          IS_MALE          :  male first name              ****/
/****          IS_MOSTLY_MALE   :  mostly male first name       ****/
/****          IS_A_COUPLE      :  name is both male and female ****/
/****          IS_UNISEX_NAME   :  unisex name (can be any gender) ****/
/****          NAME_NOT_FOUND   :  name not found               ****/
/****          ERROR_IN_NAME    :  name contains an error       ****/
{
  int  i,k,n;
  int  gender,res;
  char c;
  char temp [LENGTH_FIRST_NAME+52];

  if (! (internal_mode & GENDER_INITIALIZED))
    {
      i = initialize_gender();

      if (i < 0  ||  ! (internal_mode & GENDER_INITIALIZED))
        {
          if (internal_mode & TRACE_GENDER)
            {
             printf ("Error: initialization failed\n");
            }
          return (INTERNAL_ERROR_GENDER);
        }
    }

  if (first_name[0] == '\0')
    {
      return (IS_UNISEX_NAME);
    }

  compare_mode &= ~(SEARCH_EQUIV | SEARCH_FIRST);
  compare_mode |= SEARCH_GENDER;

  copycut (temp, first_name, LENGTH_FIRST_NAME+51);

  /****  reset statistics  ****/
  for (i=0; gc_data[i].country_text != NULL; i++)
    {
      gc_data[i].n = 0;
      if (country == gc_data[i].gc_country)
        {
          gc_data[i].n |= 1024;
        }
    }

  /****  evaluate name  ****/
  k = 0;
  while (temp[k] != '\0'  &&  temp[k] != ' '
  &&  temp[k] != '-'  &&  temp[k] != '.')
    {
      k++;
    }
  if (temp[k] == '.')
    {
      k++;
    }

  if (temp[k] != '\0')
    {
     /****  search whole name  ****/
     gender = internal_search (temp, compare_mode, country);

     if (gender != NAME_NOT_FOUND)
       {
         return (gender);
       }
    }

  /****  evaluate "multiple" name  ****/
  n = 0;
  gender = NAME_NOT_FOUND;
  compare_mode |= SEARCH_FIRST;
  res = 0;

  while (temp[n] != '\0')
    {
      /****  look for end of word  ****/
      k = n;
      while (temp[k] != '\0'  &&  temp[k] != ' '
      &&  temp[k] != '-'  &&  temp[k] != '.')
        {
          k++;
        }
      if (temp[k] == '.')
        {
          k++;
        }
      c = temp[k];
      temp[k] = '\0';

      if (k > n)
        {
          /****  check this name  ****/
          if (internal_mode & TRACE_GENDER)
            {
              printf ("\n");
            }
          if ((internal_mode & GENDER_GET_COUNTRY)
          &&  ! (compare_mode & SEARCH_FIRST))
            {
              for (i=0; gc_data[i].country_text != NULL; i++)
                {
                  gc_data[i].n &= ~1024;
                  gc_data[i].n *= 16;

                  if (country == gc_data[i].gc_country)
                    {
                      gc_data[i].n |= 1024;
                    }
                }
            }

          gender = internal_search (temp+n, compare_mode, country);
          if (internal_mode & TRACE_GENDER)
            {
              trace_info ("result for", temp+n, NULL, gender, NULL);
            }

          if ((internal_mode & GENDER_GET_COUNTRY)
          &&  ! (compare_mode & SEARCH_FIRST))
            {
              for (i=0; gc_data[i].country_text != NULL; i++)
                {
                  n = (int) ((gc_data[i].n & ~1024) / 16);
                  gc_data[i].n = multiply_frequency (n, gc_data[i].n % 16);

                  if (country == gc_data[i].gc_country)
                    {
                      gc_data[i].n |= 1024;
                    }
                }
            }
          compare_mode &= ~SEARCH_FIRST;

          switch (gender)
            {
              case IS_FEMALE        :  res |=  1;  break;
              case IS_MOSTLY_FEMALE :  res |=  2;  break;
              case IS_MALE          :  res |=  4;  break;
              case IS_MOSTLY_MALE   :  res |=  8;  break;
              case IS_UNISEX_NAME   :  res |= 16;  break;
              case NAME_NOT_FOUND   :  res |= 32;  break;
              default               :  res |= 64;  break;
            }
        }

      temp[k] = c;
      n = k;
      /****  look for begin of word  ****/
      while (temp[n] == ' '  ||  temp[n] == '-'  ||  temp[n] == '.')
        {
          n++;
        }
    }

  if ((res & 1)  &&  (res & 4))
    {
      if (compare_mode & GENDER_ALLOW_COUPLE)
        {
          /****  name is both male and female  ****/
          return (IS_A_COUPLE);
        }

      /****  error in mame  ****/
      return (ERROR_IN_NAME);
    }

  if (res & 1)
    {
      return (IS_FEMALE);
    }
  if (res & 4)
    {
      return (IS_MALE);
    }

  if ((res & 2)  &&  ! (res & 8))
    {
      return (IS_MOSTLY_FEMALE);
    }
  if ((res & 8)  &&  ! (res & 2))
    {
      return (IS_MOSTLY_MALE);
    }

  if (res & (2|8|16))
    {
      return (IS_UNISEX_NAME);
    }

  return (gender);
}




static int check_nickname_internal (char first_name_1[],
     char first_name_2[], int compare_mode, int country)
/****  This function checks if two first names are "equivalent"  ****/

/****  Result: EQUIVALENT_NAMES :  names are equivalent  ****/
/****          NAME_NOT_FOUND   :  names not found       ****/
/****         INTERNAL_ERROR_GENDER :  internal error    ****/
{
  int  i,k,n;
  char a_temp [LENGTH_FIRST_NAME+3];
  char a2_temp [LENGTH_FIRST_NAME+3];

  if (! (internal_mode & GENDER_INITIALIZED))
    {
      i = initialize_gender();

      if (i < 0  ||  ! (internal_mode & GENDER_INITIALIZED))
        {
          if (internal_mode & TRACE_GENDER)
            {
             printf ("Error: initialization failed\n");
            }
          return (INTERNAL_ERROR_GENDER);
        }
    }

  n = copycut (a_temp, first_name_1, LENGTH_FIRST_NAME+3);
  k = copycut (a2_temp,first_name_2, LENGTH_FIRST_NAME+3);

  if (n > LENGTH_FIRST_NAME  ||  k > LENGTH_FIRST_NAME)
    {
      /****  name is too long  ****/
      return (ERROR_IN_NAME);
    }

  if (strcmp (a_temp,a2_temp) == 0)
    {
      /****  names are exactly the same  ****/
      return (EQUIVALENT_NAMES);
    }
  if (n == 0  ||  k == 0)
    {
      /****  at least one name is empty  ****/
      return (NOT_EQUAL_NAMES);
    }

  if (strchr (a_temp, ' ') != NULL
  ||  strchr (a_temp, '-') != NULL
  ||  strchr (a2_temp,' ') != NULL
  ||  strchr (a2_temp,'-') != NULL)
    {
      /****  at least one "multiple" name  ****/
      return (NOT_EQUAL_NAMES);
    }

  i = strcmp_search (a_temp, a2_temp, COMPARE_ABBREVIATION,0, NULL);
  if (i == 0)
    {
      /****  names are like (e.g.) "Carl" and "C."  ****/
      return (EQUIVALENT_NAMES);
    }

  /****  reset statistics  ****/
  for (i=0; gc_data[i].country_text != NULL; i++)
    {
      gc_data[i].n = 0;
      if (country == gc_data[i].gc_country)
        {
          gc_data[i].n |= 1024;
        }
    }

  compare_mode &= ~(SEARCH_GENDER | SEARCH_FIRST);
  compare_mode |= SEARCH_EQUIV;

  i = NAME_NOT_FOUND;
  if (n <= k)
    {
      sprintf (a_temp+n, " %s", a2_temp);
      i = internal_search (a_temp, compare_mode, country);
      a_temp[n] = '\0';
    }
  if (k <= n  &&  i == NAME_NOT_FOUND)
    {
      sprintf (a2_temp+k, " %s", a_temp);
      i = internal_search (a2_temp, compare_mode, country);
      a2_temp[k] = '\0';
    }

  return (i);
}




static void standardize_arabic_name (char dest[], char src[], int len)
{
  int i;
  char *s;

  (void) up_conv_g (dest, src, len);

  while ((s = strchr (dest,'K')) != NULL)
    {
      *s = 'Q';
    }

  while ((s = strchr (dest,'J')) != NULL)
    {
      *s = 'Y';
      if (*(s+1) == '\0')
        {
          *s = 'I';
        }
    }

  while ((s = strpbrk (dest,"AEIOU")) != NULL  &&  *(s+1) != '\0')
    {
      *s = PSEUDO_VOWEL_FOR_ARABIC_NAMES;
    }

  for (i=0; dest[i] != '\0'; i++)
    {
      while (dest[i+1] == dest[i])
        {
          strcpy (dest+i, dest+i+1);
        }

      if (dest[i] == '\'')
        {
          strcpy (dest+i, dest+i+1);
        }
      if (dest[i] == 'T'  &&  dest[i+1] == '\0')
        {
          dest[i] = 'D';
        }
    }

  if ((s = strchr (dest,'Y')) != NULL
  &&  s > dest  &&  *(s-1) == PSEUDO_VOWEL_FOR_ARABIC_NAMES
  &&  strchr ("AEIOU", *(s+1)) == NULL
  &&  *(s+1) != PSEUDO_VOWEL_FOR_ARABIC_NAMES  && *(s+1) != 0)
    {
      strcpy (s,s+1);
    }
}




static char nam_array[20] [DATA_NAME_LENGTH+1];

static int find_similar_name_internal 
    (char first_name[], int country, char result_string[], int len)
{
  long l0,l2,pos_f;
  int  i,k,n,x,f,p;
  int  comp_arab,flag_sh;
  int  lev_mode,limit;
  int  d_list[20];
  int  p_list[20];
  long l_list[20];

  char *s, first_char;
  char *nam_list[20];
  char text [MAX_LINE_SIZE+1];
  char this_name [DATA_NAME_LENGTH+1];
  char this_name_0 [DATA_NAME_LENGTH+1];
  char arab_pseudo_first [DATA_NAME_LENGTH+1];
  char arab_pseudo_this [DATA_NAME_LENGTH+1];

  /****  reset statistics  ****/
  s = "any country";
  comp_arab = -1;
  for (i=0; gc_data[i].country_text != NULL; i++)
    {
      gc_data[i].n = 0;
      if (country == gc_data[i].gc_country)
        {
          gc_data[i].n |= 1024;
          s = gc_data[i].country_text;
        }
      if (gc_data[i].gc_country == GC_ARABIA)
        {
          comp_arab = i;
        }
    }

  if (internal_mode & TRACE_GENDER)
    {
      printf ("\n");
      printf ("Searching for similar names and corrections to name '%s'\n",
          first_name);
      printf ("(country = '%s'):\n\n", s);
    }

  if (strcmp (first_name,"") == 0)
    {
      if (internal_mode & TRACE_GENDER)
        {
          printf ("Error: first name is empty.\n");
        }
      return (-1);
    }
  if (len < 30)
    {
      if (internal_mode & TRACE_GENDER)
        {
          printf ("Error: result string is too short.\n");
        }
      return (-1);
    }

  if (f_names == NULL)
    {
      f_names = fopen (FIRST_NAME_FILE,"rb");
      if (f_names == NULL)
        {
          if (internal_mode & TRACE_GENDER)
            {
              printf ("Error: could not open dictionary file '%s'\n",
                  FIRST_NAME_FILE);
            }
          return (-1);
        }
    }

  for (i=0; i<20; i++)
    {
      strcpy (nam_array[i],"");
      nam_list[i] = nam_array[i];
      d_list[i] = 10000;
      p_list[i] = 0;
      l_list[i] = 0L;
    }
  limit = 70;
  n = 0;

  first_char = sortchar [(unsigned char) *first_name];

  for (i=0; russian_names[i] != NULL; i+=2)
    {
      if (first_char == russian_names[i+1][0]
      &&  strcmp_search (first_name, russian_names[i+1], 0,0,NULL) == 0)
        {
          if (internal_mode & TRACE_GENDER)
            {
              printf ("Notice: '%s' is a Russian name and should be '%s'.\n",
                  russian_names[i+1], russian_names[i]);
              printf ("Search will be continued with this name.\n\n");
            }

          strncpy (nam_list[0], russian_names[i+1], DATA_NAME_LENGTH);
          nam_list[0] [DATA_NAME_LENGTH] = '\0';

          d_list[0] = 0;
          p_list[0] = 100;
          l_list[0] = 1000L;
          n = 1;

          first_name = russian_names[i];
          first_char = sortchar [(unsigned char) *first_name];
          break;
        }
    }

  /****  search first letter in dictionary file  ****/
  sprintf (this_name, "%c", first_char);

  pos_f = binary_search (f_names, this_name, DATA_NAME_POS,
            DATA_NAME_LENGTH+1, GET_MATCH_OR_NEXT_HIGHER);

  if (pos_f < 0L)
    {
      if (pos_f == -10L)
        {
          return (INTERNAL_ERROR_GENDER);
        }
      return (NAME_NOT_FOUND);
    }

  /****  read one or more first names in dictionary file  ****/
  fseek (f_names, pos_f, SEEK_SET);

  if (strchr (first_name,'.') == NULL
  &&  strchr (first_name,MATCHES_ANY_STRING) == NULL)
    {
      standardize_arabic_name
             (arab_pseudo_first, first_name, DATA_NAME_LENGTH+1);
    }
  else
    {
      comp_arab = -1;
    }

  flag_sh = 0;
  if (lev_diff_g ("*SH*", first_name, 0, NULL, EXPAND_PARAM_2) == 0)
    {
      flag_sh = 1;
    }
  if (lev_diff_g ("*SCH*", first_name, 0, NULL, EXPAND_PARAM_2) == 0)
    {
      flag_sh = 2;
    }

  /****  "Nadia" is both a Russian and an Arabic name  ****/
  s = NULL;
  if (strcmp_search (first_name, "Nadia", 0,0,NULL) == 0)
    {
      s = "Nadia";
    }
  if (strcmp_search (first_name, "Nadya", 0,0,NULL) == 0
  ||  strcmp_search (first_name, "Nadja", 0,0,NULL) == 0)
    {
      s = "Nadya";
    }
  if (s != NULL)
    {
      strcpy (nam_list[0],s);
      d_list[i] = 0;
      p_list[i] = 300;
      l_list[0] = 500L;
      n = 1;

      /****  do not read the dictionary file  ****/
      fseek (f_names, 0L, SEEK_END);
    }

  while (! feof (f_names))
    {
      /****  read first names  ****/
      text [POS_UMLAUT_INFO] = '\0';
      read_line (f_names,text, MAX_LINE_SIZE+1);

      if (strchr ("FM1?",text[0]) == NULL
      ||  text[POS_UMLAUT_INFO] == COMPRESSED_UMLAUTS_FOR_SORTING)
        {
          /****  ignore this entry  ****/
          continue;
        }

      (void) copycut (this_name, text+DATA_NAME_POS, DATA_NAME_LENGTH+1);

      if (first_char != '\0'
      &&  first_char != sortchar [(unsigned char) this_name[0]]
      &&  sortchar [(unsigned char) this_name[0]] != IS_CHAR_TO_IGNORE)
        {
          if (n > 0  &&  d_list[0] <= 100
          && (d_list[0] <= 60  ||  p_list[0] >= 190)
          && (d_list[0] >= 0  ||  strchr ("JY",arab_pseudo_first[0]) == NULL))
            {
              /****  fast search was successful  ****/
              break;
            }
          else
            {
              /****  start second search  ****/
              if (internal_mode & TRACE_GENDER)
                {
                  printf ("\n");
                  printf ("Fast search was not successful.\n");
                  printf ("Starting extended search.\n");
                  printf ("\n");
                }

              first_char = '\0';
              n = 0;
              (void) fseek (f_names, 0L, SEEK_SET);
              continue;
            }
        }

      /****  compare names  ****/
      delete_chars_to_ignore 
           (this_name_0, this_name, flag_sh, DATA_NAME_LENGTH+1);

      lev_mode = LEV_G_COMPARE_GERMAN | EXPAND_PARAM_1;
      if (text[POS_UMLAUT_INFO] == EXPANDED_UMLAUTS_FOR_SORTING)
        {
          lev_mode |= EXPAND_PARAM_2;
        }

      k = lev_diff_g (this_name_0, first_name, limit+60, NULL, lev_mode);

      if (comp_arab >= 0
      &&  text [gc_data[comp_arab].pos] != ' ')
        {
          /****  "compare Arabic"  ****/
          standardize_arabic_name
                 (arab_pseudo_this, this_name, DATA_NAME_LENGTH+1);

          if (strcmp (arab_pseudo_first, arab_pseudo_this) == 0)
            {
              switch (text [gc_data[comp_arab].pos])
                {
                  case '1':  k = (int)(k/2);  break;
                  case '2':  k = (int)(k/3);  break;
                  case '3':  k = (int)(k/4);  break;
                  case '4':  k = (int)(k/5);  break;
                  case '5':  k = (int)(k/7);  break;
                  case '6':  k = (int)(k/10); break;
                  case '7':  k = (int)(k/20); break;
                  default :   k = -1;         break;
                }
            }
        }

      if (k <= limit + 60)
        {
          /****  name has been found  ****/
          if (k < limit)
            {
              limit = k;
              if (limit < 40)
                {
                  limit = 40;
                }
            }

          l0 = 0L;
          for (i=0; gc_data[i].country_text != NULL; i++)
            {
              if (text [gc_data[i].pos] != ' ')
                {
                  x = get_frequency (text[gc_data[i].pos]);

                  p = gc_data[i].weight;
                  if (p < 1)
                    {
                      p = 1;
                    }
                  if (x <= 1  &&  p > 5)
                    {
                      /****  very rare names need adjustments  ****/
                      p = (int)((p+5)/2);
                    }

                  if (gc_data[i].n & 1024)
                    {
                      /****  country has been found  ****/
                      if (p < 60)
                        {
                          p = 60;
                        }
                      p = 8 * p;
                    }

                  /****  de-logarithmize "frequency"  ****/
                  f = 1;
                  while (x > 0)
                    {
                      f *= 2;
                      x--;
                    }

                  l2 = (long) f * (long) p;
                  l0 += l2;
                }
            }

          /****  look for first name in list of matches  ****/
          i = 0;
          while (i < n  &&  strcmp (this_name,nam_list[i]) != 0)
            {
              i++;
            }
          if (i < n)
            {
              /****  name is already in list --> add points  ****/
              l0 += l_list[i];
            }

          /****  logarithmize "l0"  ****/
          l2 = l0;
          f = 0;
          while (l2 < 4L)
            {
              l2 *= 2L;
              f--;
            }
          while (l2 >= 8L)
            {
              l2 /= 2L;
              f++;
            }

          f = 10 * f;
          switch (l2)
            {
              case 5L :  f += 3;  break;  /**  10 * 0.322  **/
              case 6L :  f += 6;  break;  /**  10 * 0.585  **/
              case 7L :  f += 8;  break;  /**  10 * 0.807  **/
            }

          p = 200 - k + f;

          if (i < n)
            {
              /****  increase points and re-sort the list  ****/
              l_list[i] = l0;
              p_list[i] = p;
              while (i > 0  &&  p_list[i] > p_list[i-1])
                {
                  /****  switch entries  ****/
                  s = nam_list[i];
                  nam_list[i] = nam_list[i-1];
                  nam_list[i-1] = s;

                  x = d_list[i];
                  d_list[i] = d_list[i-1];
                  d_list[i-1] = x;

                  l2 = l_list[i];
                  l_list[i] = l_list[i-1];
                  l_list[i-1] = l2;

                  x = p_list[i];
                  p_list[i] = p_list[i-1];
                  p_list[i-1] = x;

                  i--;
                }
            }
          else
            {
              /****  new name found  ****/
              x = 0;
              while (x < n  &&  p_list[x] >= p)
                {
                  x++;
                }

              if (x >= 20)
                {
                  /****  list is "full",  and      ****/
                  /****  match is not good enough  ****/
                  continue;
                }
              if (n < 20)
                {
                  n++;
                }

              s = nam_list[n-1];
              for (i=n-1; i>x; i--)
                {
                  nam_list[i] = nam_list[i-1];
                  d_list[i] = d_list[i-1];
                  l_list[i] = l_list[i-1];
                  p_list[i] = p_list[i-1];
                }
              nam_list[x] = s;

              strcpy (nam_list[x], this_name);
              d_list[x] = k;
              l_list[x] = l0;
              p_list[x] = p;
            }

          if (internal_mode & TRACE_GENDER)
            {
              printf ("\n");
              printf ("Name '%s' found: points = %3d, diff = %3d\n",
                  this_name, p,k);
              printf ("Current list of matches:\n");

              for (i=0; i<n; i++)
                {
                  printf ("   %2d. name: '%s'  (points = %3d,  diff = %3d)\n",
                     i+1, nam_list[i], p_list[i], d_list[i]);
                }
            }
        }
    }

  /****  count "error-free" matches  ****/
  f = 0;
  k = 0;
  p = 0;
  x = 0;
  for (i=0; i<n; i++)
    {
      if (d_list[i] <= 5)
        {
          f++;
        }
      if (d_list[i] <= 15)
        {
          k++;
        }
      if (d_list[i] <= 30)
        {
          p++;
        }

      if (d_list[i] <= 50)
        {
          x++;
          if (p_list[i] >= 300)
            {
              x++;
            }
          if (p_list[i] >= 400)
            {
              x++;
            }
        }
    }
  limit = 40;
  if (p >= 4)
    {
      limit = 30;
    }
  if (k >= 4)
    {
      /****  return "error-free" matches only  ****/
      limit = 15;
    }
  if (n >= 2  &&  d_list[0] <= 10  &&  p_list[0] >= p_list[1] + 60)
    {
      /****  return "error-free" matches only  ****/
      limit = 10;
    }
  if (f >= 4)
    {
      /****  return "error-free" matches only  ****/
      limit = 5;
    }
  if (n > 0  &&  d_list[0] == 0)
    {
      /****  best match is 100% error-free  ****/
      limit = 0;
    }


  /****  determine "min_points"  ****/
  p = p_list[0] - 40;
  i = 0;
  while (first_name[i] != '\0'
  &&  first_name[i] != '.'  &&  first_name[i] != MATCHES_ANY_STRING)
    {
      i++;
    }
  if (i < 7  &&  first_name[i] != '\0')
    {
      /****  Wildcard found  ****/
      p += 5 * (7-i);
    }

  if (x >= 3)
    {
      /****  return "good" matches only  ****/
      p = p_list[0] + 100;
    }

  strcpy (result_string,"");
  k = 0;
  x = 0;

  for (i=0; i<n; i++)
    {
      if (p_list[i] <= p  &&  d_list[i] > limit)
        {
          /****  match is not good enough  ****/
          continue;
        }

      f = (int) strlen (nam_list[i]);
      if (k+2+f >= len)
        {
          /****  result_string is too short  ****/
          break;
        }

      if (k > 0)
        {
          result_string[k] = ' ';
          k++;
        }

      strcpy (result_string+k, nam_list[i]);
      k += f;

      result_string[k] = ';';
      k++;
      x++;
    }
  result_string[k] = '\0';

  if (internal_mode & TRACE_GENDER)
    {
      printf ("\n");
      printf ("Final result:\n");

      if (x == 0)
        {
          printf ("No names found.\n");
        }
      else
        {
          printf ("%d name(s) found:  '%s'\n", x, result_string);
        }
      printf ("\n");
    }

  return (x);
}





/************************************************************/
/****  "interface functions" for unicode and utf-8  *********/
/************************************************************/


static char input_buffer_1 [101];
static char input_buffer_2 [101];
static char output_buffer [2002];


int get_gender (char first_name[], int compare_mode, int country)
{
  int i;
  i = get_gender_internal (first_name, compare_mode, country);
  return (i);
}


int check_nickname (char first_name_1[], 
         char first_name_2[], int compare_mode, int country)
{
  int i;
  i = check_nickname_internal 
         (first_name_1, first_name_2, compare_mode, country);
  return (i);
}


int find_similar_name (char first_name[],
        int country, char result_string[], int len)
{
  int  i,k,n;
  strcpy (output_buffer,"");

  n = find_similar_name_internal 
         (first_name, country, result_string, len);

  (void) delete_chars_to_ignore (result_string, result_string, 0, len);

  if (n > 0)
    {
      k = 0;
      n = 0;
      for (i=0; result_string[i] != '\0'; i++)
        {
          if (result_string[i] == ';')
            {
              k = i;
              n++;
            }
        }
      result_string[k] = '\0';
    }

  return (n);
}



int get_gender_unicode
   (char first_name[], int compare_mode, int country)
{
  int i;
  conv_from_unicode_line (input_buffer_1, first_name, 101);

  i = get_gender_internal (input_buffer_1, compare_mode, country);
  return (i);
}


int check_nickname_unicode (char first_name_1[], 
         char first_name_2[], int compare_mode, int country)
{
  int i;
  conv_from_unicode_line (input_buffer_1, first_name_1, 101);
  conv_from_unicode_line (input_buffer_2, first_name_2, 101);

  i = check_nickname_internal 
         (input_buffer_1, input_buffer_2, compare_mode, country);
  return (i);
}


int find_similar_name_unicode (char first_name[],
        int country, char result_string[], int two_byte_len)
{
  int i,k,n;
  conv_from_unicode_line (input_buffer_1, first_name, 101);
  strcpy (output_buffer,"");

  n = find_similar_name_internal 
         (input_buffer_1, country, output_buffer, 1001);
  make_unicode_line (result_string, output_buffer, two_byte_len);

  if (n > 0)
    {
      k = 0;
      n = 0;
      for (i=0; result_string[i] != '\0'  ||  result_string[i+1] != '\0'; i+=2)
        {
          if (result_string[i] == ';'  &&  result_string[i+1] == '\0')
            {
              k = i;
              n++;
            }
        }
      result_string[k] = '\0';
      result_string[k+1] = '\0';
    }

  return (n);
}



int get_gender_utf8 (char first_name[], int compare_mode, int country)
{
  int i;
  conv_from_utf8_line (input_buffer_1, first_name, 101);
  i = get_gender_internal (input_buffer_1, compare_mode, country);
  return (i);
}


int check_nickname_utf8 (char first_name_1[], 
         char first_name_2[], int compare_mode, int country)
{
  int i;
  conv_from_utf8_line (input_buffer_1, first_name_1, 101);
  conv_from_utf8_line (input_buffer_2, first_name_2, 101);

  i = check_nickname_internal 
         (input_buffer_1, input_buffer_2, compare_mode, country);
  return (i);
}


int find_similar_name_utf8 (char first_name[],
        int country, char result_string[], int one_byte_len)
{
  int i,k,n;
  conv_from_utf8_line (input_buffer_1, first_name, 101);
  strcpy (output_buffer,"");

  n = find_similar_name_internal 
         (input_buffer_1, country, output_buffer, 1001);
  make_utf8_line (result_string, output_buffer, one_byte_len);

  if (n > 0)
    {
      k = 0;
      n = 0;
      for (i=0; result_string[i] != '\0'; i++)
        {
          if (result_string[i] == ';')
            {
              k = i;
              n++;
            }
        }
      result_string[k] = '\0';
    }

  return (n);
}

const char* get_country ()
{
  int i;
  const char* country = "";
  int freq = 0;
  for (i = 0; gc_data[i].country_text != NULL; i++) {
    if (freq < gc_data[i].n)
      {
        freq = gc_data[i].n;
        country = gc_data[i].country_short;
      }
  }

  return country;
}


/************************************************************/
/****  functions used by "main"  ****************************/
/************************************************************/

void cleanup_gender (void)
{
  if (f_names != NULL)
    {
      fclose (f_names);
      f_names = NULL;
    }

  line_size = 0;
  record_count = 0L;
}



#ifdef GENDER_EXECUTABLE


void print_line (FILE *fw, int charset, char text[])
{
  int i;

  if (charset == CHARSET_ISO_8859_1)
    {
      /****  print "normal" iso8859-1 chars  ****/
      fprintf (fw, "%s\n", text);
    }

  if (charset == CHARSET_UNICODE)
    {
      /****  print unicode chars  ****/
      make_unicode_line (output_buffer, text,1001);

      for (i=0;  output_buffer[i] != '\0'  ||  output_buffer[i+1] != '\0';  i += 2)
        {
          fprintf (fw, "%c%c", output_buffer[i], output_buffer[i+1]);
        }

      /****  print unicode EOL  ****/
      i = 13;
      fprintf (fw, "%c%c", (char) (i%256), (char) (i/256));
      i = 10;
      fprintf (fw, "%c%c", (char) (i%256), (char) (i/256));
    }

  if (charset == CHARSET_UTF_8)
    {
      /****  print utf-8 chars  ****/
      make_utf8_line (output_buffer, text,1001);

      for (i=0; output_buffer[i] != '\0'; i++)
        {
          fprintf (fw, "%c", output_buffer[i]);
        }

      /****  print EOL  ****/
      fprintf (fw, "\r\n");
    }
}



void get_gender_for_file (int charset, int country,
   int detail_mode, char input_file[], char output_file[])
{
  FILE *fr,*fw;
  int  i,n;
  int  charset_r;
  int  count[7];
  int  uni_del;
  char *s;
  char text [MAX_LINE_SIZE+1];
  char name [MAX_LINE_SIZE+1];

  if ((fr = fopen (input_file,"rb")) == NULL)
    {
      printf ("error: cound not read file '%s'\n", input_file);
      return;
    }

  /****  create output file  ****/
  s = "wb";
  if (charset == CHARSET_ISO_8859_1)
    {
      s = "w";
    }
  if ((fw = fopen (output_file,s)) == NULL)
    {
      printf ("error: cound not create file '%s'\n", output_file);
      fclose (fr);
      return;
    }

  charset_r = CHARSET_ISO_8859_1;
  i = getc (fr);
  n = getc (fr);

  if (i == 255  &&  n == 254)
    {
      charset_r = CHARSET_UNICODE;
    }
  if (i == 239  &&  n == 187  &&  getc (fr) == 191)
    {
      charset_r = CHARSET_UTF_8;
    }

  (void) fseek (fr, 0L, SEEK_SET);

  if (charset_r == CHARSET_UNICODE)
    {
      /****  ignore unicode header  ****/
      (void) getc (fr);
      (void) getc (fr);
    }
  if (charset == CHARSET_UNICODE)
    {
      /****  print unicode header  ****/
      fprintf (fw, "%c%c", 255,254);
    }

  if (charset_r == CHARSET_UTF_8)
    {
      /****  ignore utf-8 header  ****/
      (void) getc (fr);
      (void) getc (fr);
      (void) getc (fr);
    }
  if (charset == CHARSET_UTF_8)
    {
      /****  print utf-8 header  ****/
      fprintf (fw, "%c%c%c", 239,187,191);
    }

  for (i=0; i<7; i++)
    {
      count[i] = 0;
    }
  uni_del = 0;

  while (! feof (fr))
    {
      /****  read data  ****/
      internal_mode |= (detail_mode & GENDER_GET_COUNTRY);

      if (charset_r == CHARSET_ISO_8859_1)
        {
          read_line (fr, text, MAX_LINE_SIZE+1);
        }
      if (charset_r == CHARSET_UNICODE)
        {
          read_unicode_line (fr, name, (int)(MAX_LINE_SIZE/2));
          conv_from_unicode_line (text, name, MAX_LINE_SIZE+1);
        }
      if (charset_r == CHARSET_UTF_8)
        {
          read_line (fr, name, 101);
          conv_from_utf8_line (text, name, MAX_LINE_SIZE+1);
        }

      /****  delete trailing '\n' and '\r'  ****/
      i = (int) strlen (text);
      while (i > 0  &&  (text[i-1] == '\n'  ||  text[i-1] == '\r'))
        {
          i--;
        }
      text[i] = '\0';

      if (text[0] == '#'  ||  text[0] == '*'
      ||  text[0] == '\r'  ||  text[0] == '\n'  ||  text[0] == '\0')
        {
          continue;
        }

      /****  remove "chars_to_ignore"  ****/
      n = delete_chars_to_ignore (name, text, 0, MAX_LINE_SIZE+1);
      if (n > 0
      &&  charset == CHARSET_ISO_8859_1)
        {
          uni_del = 1;
        }

      i = get_gender_internal (name, detail_mode, country);

      s = text;
      if (charset == CHARSET_ISO_8859_1)
        {
          s = name;
        }
      trace_info_into_buffer ("", s, NULL,i, NULL);
      print_line (fw, charset, trace_buffer);

      switch (i)
        {
          case IS_FEMALE        :  count[0]++;  break;
          case IS_MALE          :  count[1]++;  break;
          case IS_MOSTLY_FEMALE :  count[2]++;  break;
          case IS_MOSTLY_MALE   :  count[3]++;  break;
          case IS_UNISEX_NAME   :  count[4]++;  break;
          case NAME_NOT_FOUND   :  count[5]++;  break;
          default               :  count[6]++;  break;
        }
    }

  strcpy (text, "");
  print_line (fw, charset, text);

  sprintf (text, "Girl's names  : %5d", count[0]);
  print_line (fw, charset, text);

  sprintf (text, "Boy's names   : %5d", count[1]);
  print_line (fw, charset, text);

  sprintf (text, "Mostly female : %5d", count[2]);
  print_line (fw, charset, text);

  sprintf (text, "Mostly male   : %5d", count[3]);
  print_line (fw, charset, text);

  sprintf (text, "Unisex names  : %5d", count[4]);
  print_line (fw, charset, text);

  sprintf (text, "Name not found: %5d", count[5]);
  print_line (fw, charset, text);

  if (count[6] > 0)
    {
      sprintf (text, "Errors        : %5d", count[6]);
      print_line (fw, charset, text);
    }

  i = count[0] + count[1] + count[2] + count[3] + count[4]
     + count[5] + count[6];

  sprintf (text, "  total       : %5d", i);
  print_line (fw, charset, text);

  if (uni_del)
    {
      print_line (fw, charset, "");
      print_line (fw, charset, "Notice:");
      print_line (fw, charset, "Unicode chars have been \"simplified\" to ASCII.");
    }

  fclose (fr);
  fclose (fw);
}




void check_consistency (void)
/****  check consistency of program and dictionary file  ****/
{
  int  i,n,errors;
  int  compare_mode;
  long pos;
  char text [MAX_LINE_SIZE+1];
  char name [DATA_NAME_LENGTH+1];

  if (f_names == NULL)
    {
      f_names = fopen (FIRST_NAME_FILE,"rb");
      if (f_names == NULL)
        {
          printf ("Error: could not open dictionary file '%s'\n",
              FIRST_NAME_FILE);
          return;
        }
    }

  printf ("\n");
  printf ("Checking consistency of program and dictionary file.\n");
  printf ("This will take a few seconds. Please wait...\n");
  printf ("\n");

  /****  read entries in dictionary file  ****/
  fseek (f_names, 0L,SEEK_SET);
  errors = 0;
  internal_mode &= ~TRACE_GENDER;

  read_line (f_names,text, MAX_LINE_SIZE+1);
  if (strncmp (text, CHECK_STRING, (int)strlen(CHECK_STRING)) != 0)
    {
      i = (int)strlen (text);
      while (i > 0  &&  (text[i-1] == '\n'  ||  text[i-1] == '\r'))
        {
          i--;
        }
      text[i] = '\0';

      printf ("Error: Invalid version of dictionary file '%s'.\n",
         FIRST_NAME_FILE);
      printf ("File header is:  \"%s\"\n", text);
      printf ("(this should be:  \"%s\").\n", CHECK_STRING);

      return;
    }

  for (;;)
    {
      /****  read one or more first names  ****/
      read_line (f_names,text, MAX_LINE_SIZE+1);

      pos = ftell (f_names);
      if (feof (f_names))
        {
          pos = -1L;
        }

      copycut (name, text+DATA_NAME_POS, DATA_NAME_LENGTH+1);

      /****  remove "internal chars"  ****/
      n = 0;
      for (i=0; name[i] != '\0'; i++)
        {
          if (name[i] == '+')
            {
              name[i] = '-';
            }
          if (strchr (CHARS_TO_IGNORE, name[i]) == NULL)
            {
              name[n] = name[i];
              n++;
            }
        }
      name[n] = '\0';

      compare_mode = GENDER_COMPARE_EXACT;
      if (text[0] == '=')
        {
          compare_mode |= SEARCH_EQUIV;
        }
      else
        {
          compare_mode |= SEARCH_GENDER;
        }

      if (text[POS_UMLAUT_INFO] == EXPANDED_UMLAUTS_FOR_SORTING)
        {
          compare_mode |= UMLAUT_EXPAND;
        }

      if (text[0] != '#')
        {
          i = internal_search (name, compare_mode,0);

          if (i == NAME_NOT_FOUND)
            {
              printf ("Error: name '%s' not found\n", name);
              errors++;

              internal_mode |= TRACE_GENDER;
              (void) internal_search (name, compare_mode,0);
              internal_mode &= ~TRACE_GENDER;

              printf ("\n");
            }
        }

      if (pos < 0L)
        {
          break;
        }

      fseek (f_names,pos,SEEK_SET);
    }

  if (errors == 0)
    {
      printf ("Result: No inconsistencies found\n");
    }
  else
    {
      printf ("Result: %d errors found\n", errors);
    }
}




int determine_country (char *text)
{
  char *s;
  int  i,k,country;

  country = GC_ANY_COUNTRY;
  k = (int)strlen (text);

  if (isdigit (*text))
    {
      country = atoi (text);
    }
  for (i=0; gc_data[i].country_text != NULL; i++)
    {
      if (strcmp_search (text, gc_data[i].country_text, 0,0, NULL) == 0
      || ((s = strstr (gc_data[i].country_text, text)) != NULL
       &&  *text == upperchar [(unsigned char) *s]
       && (*(s+k) == '\0'  ||  *(s+k) == '/'  ||  *(s+k) == ' ')))
        {
          country = gc_data[i].gc_country;
        }
    }

  return (country);
}




void print_names_of_country (int charset, char *country_text,
   int min_freq, int max_freq, char output_file[])
/****  Print all names of a given country            ****/
/****  with frequency between min_freq and max_freq  ****/
{
  FILE *fw;
  char *s,text [MAX_LINE_SIZE+1];
  int i,k;
  int country;
  int uni_del;
  int found = 0;
  int pos = -1;

  if (f_names == NULL)
    {
      f_names = fopen (FIRST_NAME_FILE,"rb");
      if (f_names == NULL)
        {
          printf ("Error: could not open dictionary file '%s'\n",
              FIRST_NAME_FILE);
          return;
        }
    }

  /****  create output file  ****/
  s = "wb";
  if (charset == CHARSET_ISO_8859_1)
    {
      s = "w";
    }
  if ((fw = fopen (output_file,s)) == NULL)
    {
      printf ("error: cound not create file '%s'\n", output_file);
      return;
    }

  if (charset == CHARSET_UNICODE)
    {
      /****  print unicode header  ****/
      fprintf (fw, "%c%c", 255,254);
    }
  if (charset == CHARSET_UTF_8)
    {
      /****  print utf-8 header  ****/
      fprintf (fw, "%c%c%c", 239,187,191);
    }

  /****  determine country  ****/
  if (strncmp (country_text,"all",3) == 0)
    {
      country = -100;
    }
  else
    {
      country = determine_country (country_text);
    }

  for (i=0; gc_data[i].country_text != NULL; i++)
    {
      if (country != -100
      &&  country != gc_data[i].gc_country)
        {
          /****  wrong country  ****/
          continue;
        }

      if ((internal_mode & DETAILS_FOR_COUNTRY)
      &&  pos != -1)
        {
          /****  print separator lines  ****/
          print_line (fw, charset, "");
          print_line (fw, charset, "");
          print_line (fw, charset, "");

          sprintf (text, "%-*s", POS_UMLAUT_INFO+6, "");
          for (k=0; text[k] != '\0'; k++)
            {
              text[k] = '=';
            }
          print_line (fw, charset, text);
        }

      print_line (fw, charset, "");
      print_line (fw, charset, "");
      print_line (fw, charset, "");

      pos = (int) gc_data[i].pos;
      country_text = gc_data[i].country_text;
      uni_del = 0;

      /****  print header  ****/
      sprintf (text, "Names from %s:", country_text);
      print_line (fw, charset, text);
      print_line (fw, charset, "");

      if (internal_mode & DETAILS_FOR_COUNTRY)
        {
          print_line (fw, charset, "");

          sprintf (text, "%-*sfrequency", POS_UMLAUT_INFO-2, "gender name");
          print_line (fw, charset, text);
          for (k=0; text[k] != '\0'; k++)
            {
              text[k] = '-';
            }
          print_line (fw, charset, text);
        }

      /****  read entries in dictionary file  ****/
      fseek (f_names, 0L,SEEK_SET);

      while (! feof (f_names))
        {
          /****  read first names  ****/
          text [POS_UMLAUT_INFO] = '\0';
          read_line (f_names,text, MAX_LINE_SIZE+1);

          if (text[0] == '#'
          ||  text[0] == '='
          ||  text[POS_UMLAUT_INFO] == COMPRESSED_UMLAUTS_FOR_SORTING
          ||  text[pos] == ' '
          || (int)strlen (text) <= pos)
            {
              /****  don't print this entry  ****/
              continue;
            }

          k = get_frequency (text[pos]);

          if (k >= min_freq  &&  k <= max_freq)
            {
              if (internal_mode & DETAILS_FOR_COUNTRY)
                {
                  text[POS_UMLAUT_INFO] = '\0';
                  sprintf (text, "%s%2d", text,k);

                  for (k= POS_UMLAUT_INFO+2; k >= 0; k--)
                    {
                      text[k+2] = text[k];
                    }
                  text[0] = ' ';
                  text[1] = ' ';
                }
              else
                {
                  k = POS_UMLAUT_INFO;
                  while (k > 3  &&  text[k-1] == ' ')
                    {
                      k--;
                    }
                  text[k] = '\0';
                  strcpy (text, text+3);
                }

              if (charset == CHARSET_ISO_8859_1)
                {
                   k = delete_chars_to_ignore (text, text, 0, MAX_LINE_SIZE+1);
                   if (k > 0)
                     {
                       uni_del = 1;
                     }
                }

              print_line (fw, charset, text);
              found = 1;
            }
        }

      if (uni_del)
        {
          print_line (fw, charset, "");
          print_line (fw, charset, "Notice:");
          print_line (fw, charset, "Unicode chars have been \"simplified\" to ASCII.");
        }
    }

  if (pos < 0)
    {
      print_line (fw, charset, "Error: country not found.");
    }
  else if (found == 0)
    {
      print_line (fw, charset, "No names have been found.");
    }

  fclose (fw);
}




void print_statistics (int get_full_statistics)
/****  print statistics for FIRST_NAME_FILE  ****/
{
  unsigned int count,equiv;
  unsigned int female,male,unisex;
  unsigned int unclass,errors;
  int  i,n,x, last_gender;
  int  c_country[5][14];
  char *s, text [MAX_LINE_SIZE+1];
  char this_name[POS_UMLAUT_INFO-1];
  char last_name[POS_UMLAUT_INFO-1];

  if (f_names == NULL)
    {
      f_names = fopen (FIRST_NAME_FILE,"rb");
      if (f_names == NULL)
        {
          printf ("Error: could not open dictionary file '%s'\n",
              FIRST_NAME_FILE);
          return;
        }
    }

  /****  read entries in dictionary file  ****/
  fseek (f_names, 0L,SEEK_SET);

  equiv = 0;
  count = 0;
  unclass = 0;
  female = 0;
  male = 0;
  unisex = 0;
  errors = 0;
  strcpy (last_name,"");
  last_gender = 0;

  for (i=0; gc_data[i].country_text != NULL; i++)
    {
      gc_data[i].n = 0;
    }

  while (! feof (f_names))
    {
      /****  read first names  ****/
      text [POS_UMLAUT_INFO] = '\0';
      read_line (f_names,text, MAX_LINE_SIZE+1);

      if (strncmp (text,"  ",2) == 0)
        {
          errors++;
        }

                 /****  Sometimes, there is more than one way  ****/
                 /****  to "EXPAND"  (e.g. <sch> and <sh>).    ****/
      if (text[POS_UMLAUT_INFO] == EXPANDED_UMLAUTS_FOR_SORTING)
        {
          /****  don't count this entry  ****/
          continue;
        }

      strncpy (this_name, text+2, POS_UMLAUT_INFO-2);
      this_name [POS_UMLAUT_INFO-2] = '\0';

      n = strcmp_search (this_name, last_name, 
              EXPAND_PARAM_1 | EXPAND_PARAM_2, ' ', NULL);

      if (text[0] == '='  &&  n != 0)
        {
          equiv++;
        }

      if (strchr ("FM1?",text[0]) != NULL  &&  text[0] != '\0')
        {
          if (n != 0)
            {
              count++;
              last_gender = 0;
            }
          else
            {
              if (last_gender == 1)
                {
                  female--;
                }
              if (last_gender == 2)
                {
                  male--;
                }
              if (last_gender == 3)
                {
                  unisex--;
                }
            }

          if (strncmp (text,"F ",2) == 0
          ||  strncmp (text,"1F",2) == 0)
            {
              last_gender |= 1;
            }
          else if (strncmp (text,"M ",2) == 0
          ||  strncmp (text,"1M",2) == 0)
            {
              last_gender |= 2;
            }
          else
            {
              last_gender |= 3;
            }

          if (last_gender == 1)
            {
              female++;
            }
          if (last_gender == 2)
            {
              male++;
            }
          if (last_gender == 3)
            {
              unisex++;
            }

          n = 0;
          for (i=0; gc_data[i].country_text != NULL; i++)
            {
              if (text [gc_data[i].pos] != ' ')
                {
                  n++;
                  gc_data[i].n++;
                }
            }

          if (n == 0)
            {
              unclass++;
            }
        }

      strcpy (last_name, this_name);
    }

  printf ("Statistics of first names in dictionary\n");
  printf ("=======================================\n");
  printf ("\n");
  printf ("Number of first names   :  %6u\n", count);
  printf ("   girl's names         :  %6u\n", female);
  printf ("   boy's  names         :  %6u\n", male);
  printf ("   unisex names         :  %6u\n", unisex);
  printf ("Equivalent name pairs   :  %6u\n", equiv);
  printf ("\n");

  if (unclass > 0)
    {
      printf ("Names of unknown origin   : %5u\n", unclass);
      printf ("\n");
    }
  if (errors > 0)
    {
      printf ("Errors (gender is missing): %5u\n", errors);
      printf ("\n");
    }

  for (i=0; gc_data[i].country_text != NULL; i++)
    {
      printf ("Names from %-15s: %5d\n",
          gc_data[i].country_text, gc_data[i].n);
    }


  /****  print names of unknown origin  ****/
  if (unclass > 0)
    {
      fseek (f_names, 0L,SEEK_SET);
      printf ("\n");
      while (! feof (f_names))
        {
          /****  read first names  ****/
          text [POS_UMLAUT_INFO] = '\0';
          read_line (f_names,text, MAX_LINE_SIZE+1);

                    /****  Sometimes, there is more than one way  ****/
                    /****  to "EXPAND"  (e.g. <sch> and <sh>).    ****/
          if (text[POS_UMLAUT_INFO] == EXPANDED_UMLAUTS_FOR_SORTING)
            {
              /****  don't count this entry  ****/
              continue;
            }

          if (strchr ("=FM1?",text[0]) != NULL  &&  text[0] != '\0')
            {
              n = 0;
              for (i=0; gc_data[i].country_text != NULL; i++)
                {
                  if (text [gc_data[i].pos] != ' ')
                    {
                      n++;
                    }
                }

              if (n == 0)
                {
                  text[POS_UMLAUT_INFO] = '\0';
                  printf ("Unknown origin:  %s\n", text);
                }
            }
        }
    }

  if (get_full_statistics <= 0)
    {
      return;
    }


  /****  print detailed statistics for all countries  ****/
  printf ("\n");
  for (i=0; gc_data[i].country_text != NULL; i++)
    {
      /****  read entries in dictionary file  ****/
      fseek (f_names, 0L,SEEK_SET);

      for (n=0; n<5; n++)
        {
          for (x=0; x<14; x++)
            {
              c_country[n][x] = 0;
            }
        }

      while (! feof (f_names))
        {
          /****  read first names  ****/
          text [POS_UMLAUT_INFO] = '\0';
          read_line (f_names,text, MAX_LINE_SIZE+1);

          if (text[POS_UMLAUT_INFO] == EXPANDED_UMLAUTS_FOR_SORTING)
            {
              /****  don't count this entry  ****/
              continue;
            }

          if (strchr ("FM1?",text[0]) != NULL  &&  text[0] != '\0'
          &&  text [gc_data[i].pos] != ' ')
            {
              x = get_frequency (text[gc_data[i].pos]);

              if (x > 0)
                {
                  n = 0;
                  if (strncmp (text,"F ",2) == 0
                  ||  strncmp (text,"1F",2) == 0)
                    {
                      n = 1;
                    }
                  else if (strncmp (text,"M ",2) == 0
                  ||  strncmp (text,"1M",2) == 0)
                    {
                      n = 2;
                    }
                  else if (text[0] == '?')
                    {
                      n = 3;
                    }
                  else if (text[0] == '=')
                    {
                      n = 4;
                    }

                  c_country[0][0]++;
                  c_country[0][x]++;

                  if (n > 0)
                    {
                      c_country[n][0]++;
                      c_country[n][x]++;
                    }
                }
            }
        }

      switch (gc_data[i].quality_of_statistics)
        {
         case VERY_GOOD:  s = "very good";
                   break;
         case GOOD     :  s = "good";
                   break;
         case MEDIUM   :  s = "medium quality";
                   break;
         default       :  s = "??";
                   break;
        }

      printf ("\n");
      printf ("Statistics for %s  (statistics are %s):\n", gc_data[i].country_text, s);
      printf ("                rare            medium          common         total\n");

      for (n=0; n<5; n++)
        {
          switch (n)
            {
              case 0:  s = "first  names";
                   break;
              case 1:  s = "girl's names";
                   break;
              case 2:  s = "boy's  names";
                   break;
              case 3:  s = "unisex names";
                   break;
              case 4:  s = "equiv. names";
                   break;
           }

         if (n <= 3  ||  c_country[n][0] > 0)
           {
             printf ("%s:  %4d %4d %3d %3d %3d %3d %3d %3d %2d %2d %2d %1d %1d  %5d\n", s,
                 c_country[n][1], c_country[n][2], c_country[n][3], c_country[n][4],
                 c_country[n][5], c_country[n][6], c_country[n][7], c_country[n][8],
                 c_country[n][9], c_country[n][10], c_country[n][11], c_country[n][12],
                 c_country[n][13], c_country[n][0]);
           }
        }
    }
}




int main (int argc, char *argv[])
{
  int  i,k,n;
  int  charset;
  int  country = GENDER_DEFAULT_COUNTRY;
  char *input_file, *output_file;
  char text[201];

  if (argc < 2
  ||  strcmp (argv[1], "-?") == 0
  ||  strcmp (argv[1], "-h") == 0
  ||  strcmp (argv[1], "-help") == 0)
    {
     printf ("Usage:  gender  -get_gender      [ -country=<country> ]   <first_name>   [ -trace ]\n");
     printf (" or  :  gender  -check_nickname  [ -country=<country> ]  <name_1>  <name_2>  [ -trace ]\n");
     printf (" or  :  gender  -find_similar_name  [ -country=<country> ]  <first_name>   [ -trace ]\n");
     printf ("\n");
     printf (" or  :  gender  -get_gender_for_file  [ -country=<country> ]   [ -detail ]   [ -unicode | -utf8 ]   <input_file>   <output_file>\n");
     printf (" or  :  gender  -check_consistency\n");
     printf (" or  :  gender  -print_names_of_country  <country>  [ <min_frequency>   [ <max_frequency> ] ]   [ -detail ]   [ -unicode | -utf8 ]   <output_file>\n");
     printf (" or  :  gender  -statistics  [ -detail ]\n");
     printf ("\n");
     printf ("Program for checking first name and gender (%s).\n\n", GENDER_VERSION);
     printf ("\n");
     printf ("Options:\n");
     printf (" -get_gender             :  Determine the gender of a given first name.\n");
     printf (" -check_nickname         :  Check, if two first names are \"equivalent\".\n");
     printf (" -find_similar_name      :  Try to get a correction for a given first name.\n");
     printf ("\n");
     printf (" -get_gender_for_file    :  Determine the gender for all names from a text file.\n");
     printf (" -check_consistency      :  Check consistency of program and dictionary file.\n");
     printf (" -print_names_of_country :  Print all names of a given country.\n");
     printf ("                            (if <country> = \"all\", the function will be\n");
     printf ("                             called for all countries)\n");
     printf (" -statistics             :  Print statistics of first names in dictionary file.\n");
     printf ("\n");
     printf (" -lev  <name1>  <name2>  :  Call the Levenshtein function (for diagnostics only).\n");
     printf ("\n");

     return (1);
    }

  if (! (internal_mode & GENDER_INITIALIZED))
    {
      i = initialize_gender();

      if (i < 0  ||  ! (internal_mode & GENDER_INITIALIZED))
        {
          if (internal_mode & TRACE_GENDER)
            {
             printf ("Error: initialization failed\n");
            }
          return (INTERNAL_ERROR_GENDER);
        }
    }

  if (argc >= 3  &&  strncmp (argv[2],"-country=",9) == 0)
    {
      country = determine_country (argv[2] + 9);
      argv++;
      argc--;
      argv[1] = argv[0];
    }
  if (argc >= 2  &&  strncmp (argv[1],"-country=",9) == 0)
    {
      country = determine_country (argv[1] + 9);
      argv++;
      argc--;
    }

  if (strstr (argv[1],"gender_for_file") != NULL)
    {
      if (argc < 3)
        {
          printf ("Wrong # of arguments.\n");
          return (1);
        }

      charset = CHARSET_ISO_8859_1;

      for (i=2; i< argc-2; i++)
        {
          if (strncmp (argv[i],"-detail",7) == 0)
            {
              internal_mode |= ( GENDER_GET_COUNTRY
                                  | GENDER_DEFAULT_TRACE_MODE );
            }

          if (strcmp (argv[i],"-unicode") == 0)
            {
              charset = CHARSET_UNICODE;
            }
          if (strcmp (argv[i],"-utf8") == 0)
            {
              charset = CHARSET_UTF_8;
            }
        }

      input_file = argv [argc-2];
      output_file = argv [argc-1];

      get_gender_for_file (charset, country,
             GENDER_COMPARE_EXPANDED_UMLAUTS, input_file, output_file);
    }
  else if (strstr (argv[1],"gender") != NULL)
    {
      if (argc < 3)
        {
          printf ("Wrong # of arguments.\n");
          return (1);
        }

      if (argc >= 4  &&  strcmp (argv[3],"-trace") == 0)
        {
          internal_mode |= ( TRACE_GENDER | GENDER_GET_COUNTRY
                                 | GENDER_DEFAULT_TRACE_MODE );
        }

      i = get_gender (argv[2], 
              GENDER_COMPARE_EXPANDED_UMLAUTS, country);

      trace_info ("final result for", argv[2], NULL,i, NULL);
    }

  if (strstr (argv[1],"nickname") != NULL)
    {
      if (argc < 4)
        {
          printf ("Wrong # of arguments.\n");
          return (1);
        }

      if (argc >= 5  &&  strcmp (argv[4],"-trace") == 0)
        {
          internal_mode |= ( TRACE_GENDER | GENDER_GET_COUNTRY
                            | GENDER_DEFAULT_TRACE_MODE );
        }

      i = check_nickname (argv[2], argv[3],
                GENDER_COMPARE_EXPANDED_UMLAUTS, country);

      trace_info ("result for", argv[2], argv[3],i, NULL);
    }

  if (strstr (argv[1],"find") != NULL)
    {
      if (argc < 3)
        {
          printf ("Wrong # of arguments.\n");
          return (1);
        }

      if (argc >= 4  &&  strcmp (argv[3],"-trace") == 0)
        {
          internal_mode |= ( TRACE_GENDER | GENDER_DEFAULT_TRACE_MODE );
        }

      i = find_similar_name (argv[2], country, text,201);

      if (i > 0)
        {
          printf ("Result: %d name(s) found:  '%s'\n", i,text);
        }
      else
        {
          printf ("No names found.\n");
        }
    }

  if (argc >= 2  &&  strcmp (argv[1],"-check_consistency") == 0)
    {
      /****  check consistency  ****/
      check_consistency();
    }

  if (strcmp (argv[1],"-print_names_of_country") == 0)
    {
      if (argc < 4)
        {
          printf ("Wrong # of arguments.\n");
          return (1);
        }

      charset = CHARSET_ISO_8859_1;
      k = 1;
      n = 15;

      if (argc >= 4  &&  isdigit (argv[3][0]))
        {
          k = atoi (argv[3]);
          if (argc >= 5  &&  isdigit (argv[4][0]))
            {
              n = atoi (argv[4]);
            }
        }

      for (i=3; i< argc-1; i++)
        {
          if (strncmp (argv[i],"-detail",7) == 0)
            {
              internal_mode |= DETAILS_FOR_COUNTRY;
            }

          if (strcmp (argv[i],"-unicode") == 0)
            {
              charset = CHARSET_UNICODE;
            }
          if (strcmp (argv[i],"-utf8") == 0)
            {
              charset = CHARSET_UTF_8;
            }
        }

      output_file = argv [argc-1];

      print_names_of_country
              (charset, argv[2], k,n, output_file);

      internal_mode &= ~DETAILS_FOR_COUNTRY;
    }

  if (argc >= 2  &&  strncmp (argv[1],"-stat",5) == 0)
    {
      /****  print statistics  ****/
      if (argc > 2
      && (strcmp (argv[2],"-full") == 0
      ||  strncmp (argv[2],"-detail",7) == 0))
        {
          print_statistics (1);
        }
      else
        {
          print_statistics (0);
        }
    }

  if (argc >= 4  &&  strcmp (argv[1],"-lev") == 0)
    {
      /****  call the Levenshtein function  ****/
      k = LEV_G_COMPARE_GERMAN | EXPAND_PARAM_1 | EXPAND_PARAM_2 | TRACE_LEV_G;

      (void) lev_diff_g (argv[2], argv[3], 10000, NULL, k);
    }

  cleanup_gender();
  return (0);
}

const char* get_first_name_file ()
{
  return "nam_dict.txt";
}

#endif

/************************************************************/
/****  end of file "gender.c"  ******************************/
/************************************************************/
