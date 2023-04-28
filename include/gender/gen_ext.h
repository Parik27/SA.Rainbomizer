/*
 * gen_ext.h
 * ---------
 *
 * Constants and prototypes for the program "gender.c".
 *
 * Copyright (c):
 * 2007-2008:  Joerg MICHAEL, Adalbert-Stifter-Str. 11, 30655 Hannover, Germany
 *
 * SCCS: @(#) gen_ext.h  1.2  2008-11-30
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
 * (However, since __this__ program is intended to be customized, all changes
 * covered by TO-DO comments are free.)
 *
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


#ifndef _GENDER_EXT_INCLUDED_

#define _GENDER_EXT_INCLUDED_


#ifdef __cplusplus
extern "C"
{
#endif

/****  If you want to use "gender.c" as a library,  ****/
/****  delete the following macro:                  ****/
//#define GENDER_EXECUTABLE


/****  Defaults for the "main"-function.  ****/
/****  TO-DO: Change them, if necessary:  ****/
#define GENDER_DEFAULT_COUNTRY       GC_ANY_COUNTRY
#define GENDER_DEFAULT_TRACE_MODE    ( GENDER_TRACE_ALL_COUNTRIES | GENDER_TRACE_FULL_COUNTRY_NAME )



/****  TO-DO: Change the following macros if necessary.        ****/
/****  (NOTE: These macros are independent from corresponding  ****/
/****   definitions in the file "FIRST_NAME_FILE")             ****/
#define IS_FEMALE              'F'
#define IS_MOSTLY_FEMALE       'f'
#define IS_MALE                'M'
#define IS_MOSTLY_MALE         'm'
#define IS_UNISEX_NAME         '?'
#define IS_A_COUPLE            'C'

#define EQUIVALENT_NAMES       '='
#define NOT_EQUAL_NAMES        '!'
#define NAME_NOT_FOUND         ' '
#define ERROR_IN_NAME          'E'
#define INTERNAL_ERROR_GENDER  'I'


/****  File with list of first names and gender.  ****/
/****  TO-DO:  Add full pathname  ****/
#define FIRST_NAME_FILE  get_first_name_file()

/****  If you need a configurable filename, do it this way:  ****/
/****     char *first_file_name = "<your_file_name>";    ****/
/****     #define FIRST_FILE_NAME  first_file_name       ****/


/****  TO-DO:  If you use a bigger field length  ****/
/****    for first names, change this macro      ****/
#define LENGTH_FIRST_NAME  40

#if (LENGTH_FIRST_NAME < 20)
  #error "LENGTH_FIRST_NAME < 20  is not allowed"
#endif



/******************************************************************/
/****  macros for "compare_mode" / "trace_mode" (do not change) ***/
/****  (macros for "0" represent the default)                   ***/
/******************************************************************/

#define GENDER_COMPARE_EXPANDED_UMLAUTS     0
#define GENDER_TRACE_ALL_COUNTRIES          0
#define GENDER_TRACE_FULL_COUNTRY_NAME      0

#define GENDER_ALLOW_COMPRESSED_UMLAUTS     1
#define GENDER_ALLOW_COUPLE                 2

#define GENDER_TRACE_ONE_COUNTRY_ONLY       4
#define GENDER_TRACE_SHORT_COUNTRY_NAME     8



/************************************************************/
/****  macros for "country" (do not change)  ****************/
/************************************************************/

#define GC_ANY_COUNTRY   0

#define GC_BRITAIN       1
#define GC_IRELAND       2
#define GC_USA           3

#define GC_SPAIN         4
#define GC_PORTUGAL      5
#define GC_ITALY         6
#define GC_MALTA         7

#define GC_FRANCE        8
#define GC_BELGIUM       9
#define GC_LUXEMBOURG   10
#define GC_NETHERLANDS  11

#define GC_GERMANY      12
#define GC_EAST_FRISIA  13
#define GC_AUSTRIA      14
#define GC_SWISS        15

#define GC_ICELAND      16
#define GC_DENMARK      17
#define GC_NORWAY       18
#define GC_SWEDEN       19
#define GC_FINLAND      20
#define GC_ESTONIA      21
#define GC_LATVIA       22
#define GC_LITHUANIA    23

#define GC_POLAND       24
#define GC_CZECH_REP    25
#define GC_SLOVAKIA     26
#define GC_HUNGARY      27
#define GC_ROMANIA      28
#define GC_BULGARIA     29

#define GC_BOSNIA       30
#define GC_CROATIA      31
#define GC_KOSOVO       32
#define GC_MACEDONIA    33
#define GC_MONTENEGRO   34
#define GC_SERBIA       35
#define GC_SLOVENIA     36
#define GC_ALBANIA      37
#define GC_GREECE       38

#define GC_RUSSIA       39
#define GC_BELARUS      40
#define GC_MOLDOVA      41
#define GC_UKRAINE      42
#define GC_ARMENIA      43
#define GC_AZERBAIJAN   44
#define GC_GEORGIA      45
#define GC_KAZAKH_UZBEK 46

#define GC_TURKEY       47
#define GC_ARABIA       48
#define GC_ISRAEL       49
#define GC_CHINA        50
#define GC_INDIA        51
#define GC_JAPAN        52
#define GC_KOREA        53
#define GC_VIETNAM      54



/************************************************************/
/****  function prototypes  *********************************/
/************************************************************/

int initialize_gender (void);
int get_gender (char first_name[], int compare_mode, int country);
const char* get_country ();
int get_gender_unicode (char first_name[], int compare_mode, int country);
int get_gender_utf8 (char first_name[], int compare_mode, int country);

int check_nickname (char first_name_1[], 
         char first_name_2[], int compare_mode, int country);
int check_nickname_unicode (char first_name_1[], 
         char first_name_2[], int compare_mode, int country);
int check_nickname_utf8 (char first_name_1[], 
         char first_name_2[], int compare_mode, int country);

int find_similar_name (char first_name[], int country,
         char result_string[], int len);
int find_similar_name_unicode (char first_name[], int country,
         char result_string[], int two_byte_len);
int find_similar_name_utf8 (char first_name[], int country,
         char result_string[], int one_byte_len);

/**
 void standardize_arabic_name (char dest[], char src[], int len);
**/

void cleanup_gender (void);
const char* get_first_name_file ();

#ifdef __cplusplus
}
#endif

#endif

/************************************************************/
/****  end of file "gen_ext.h"  *****************************/
/************************************************************/
