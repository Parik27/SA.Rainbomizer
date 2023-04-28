/*
 * readme.txt
 * ----------
 *
 * Description of program "gender.c".
 *
 * Copyright (c):
 * 2007-2008:  Jörg MICHAEL, Adalbert-Stifter-Str. 11, 30655 Hannover, Germany
 *
 * SCCS: @(#) readme.txt  1.2  2008-11-30
 *
 * This file is subject to the GNU Lesser General Public License (LGPL)
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


This file contains some important information, which cannot be found
in the German "c't" article.

Index:

1. Overview of the program "gender"
2. The dictionary file "nam_dict.txt"
3. A few words on quality of data

4. The function "get_gender"
5. The function "check_nickname"

6. The function "find_similar_name" (new in version 1.2)
    a) "Normal" use of the function
    b) "Expanding" abbreviations (e.g. "Chr.")
    c) Quality of the results: two caveats
    d) Speed of the function

7. "Standardizing" first names  (new in version 1.2)
    a) "Standardizing" Arabic first names
    b) "Standardizing" Russian first names
    c) Converting first names to utf8 or unicode

8. Other functions
9. Interfaces for unicode support (new in version 1.2)
10. Interfaces for utf-8 support (new in version 1.2)
11. Unicode chars

12. Statistics of first names in dictionary
13. Changes in "nam_dict.txt" in version 1.1
14. Changes in "nam_dict.txt" in version 1.1.1
15. Changes in "nam_dict.txt" in version 1.2

16. Frequently Asked Questions
    a) Is it possible to use this program in a commercial software project?
    b) Is a Java version of this program available?
    c) Is a PHP version of this programm available?
    d) Can I get a graphical user interface for this program?
    e) What is the speed of this program?

17. What about Wikipedia / Wiktionary ?
18. WWW resources for this program
19. References
    a) Dictionary of first names
    b) Phonetic conversion for German
    c) Error-tolerant database selects (for mail addresses)
    d) Levenshtein function
20. History of the program


========================================================================


Overview of the program "gender"


The program "gender.c" is a program for determining the gender of a given
fist nane.

List of files:

a)  gen_ext.h     (contains macros and prototypes; may be changed)
b)  umlaut_g.h    (contains lists of umlauts)
c)  gender.c      (this is the "workhorse" of the program)
d)  nam_dict.txt  (dictionary file containing 42000+ first names)

The file "nam_dict.txt", which contains a list of first names, uses the
char set "iso8859-1".

If you want to use "gender.c" as a library, delete the line
"#define GENDER_EXECUTABLE"  from the file "gen_ext.h".

Notice:
The exe file included in this download is a DOS exe, so you have to obey
the rules for 8.3 filenames under DOS.


========================================================================


The dictionary file "nam_dict.txt"


The program "gender.c" uses the dictionary file "nam_dict.txt" as a data 
source. This file contains a list of more than 42,000 first names and 
gender, plus some 700 pairs of "equivalent" names.
This list should be able to cover the vast majority of first names 
in all European contries and in some overseas countries (e.g. China, 
India, Japan, U.S.A.) as well.

Also included in this file is information on the approximate frequency 
of each name. The scale goes from 1 (=rare) to 13 (=extremely common).
The value 10 has been formatted to represent at least 2 percent of 
the population. (The values 11 to 13 have been added last.)
The scale is logarithmic. For countries with very good statistics, 
each step (down to frequency 2) represents a factor of 2. 
For example, a frequency value of 7 means that the correspondig first 
name has an absolute in the range of 0.25 % to 0.5 %.

The sorting order of the file "nam_dict.txt" is governed by the search 
algorithm of the program "gender.c". Hence, names with "expandable" 
umlauts can be found twice in this dictionary, first with sorting 
according to "expanded" umlauts, and second with sorting according to 
"compressed" umlauts (e.g. 'Ö' is sorted like "Oe" and 'O').

You don't have to reformat this file for use in a unix environment, 
because the DOS linefeeds (trailing '\r') are ignored when the file
is read.


========================================================================


A few words on quality of data


The dictionary of first names has been prepared with utmost care.

For example, the Turkish, Indian and Korean names in this dictionary 
have all been independently lassified by several native speakers.
I also took special care to list only those names which can currently 
be found.

The lesson from this?
Any modifications should be done very cautiously (and they must also 
adhere to the sorting required by the search algorithm).

For example, knowing that "Sascha" is a boy's name in Germany, the author 
never assumed the English "Sasha" to be a girl's name. 
Knowing that "Jan" is a boy's name in Germany, I never assumed it to be 
also a English short form of "Janet". Another case in point is the name 
"Esra". This is a boy's name in Germany, but a girl's name in Turkey.

Or consider the following first names:

Ildikó     female Hungarian name
Mitja      male Russian name
Elizaveta  rare name; looks like misspelled "Elizabeta"
Roelf      rare name; looks like German "Rolf" with an erroneous 'e'

Borchert, Oltmann, Sievert, Hartmann    look like common German surnames


========================================================================


The function "get_gender"


The function "get_gender" is used to check first names and determine their
gender. This function has the following return values, which are defined
in the file "gen_ext.h":

IS_FEMALE         :  female first name
IS_MOSTLY_FEMALE  :  mostly female (= unisex name, which is mostly female)
IS_MALE           :  male first name
IS_MOSTLY_MALE    :  mostly male (= unisex name, which is mostly male)
UNISEX_NAME       :  unisex name (can be male or female)

NAME_NOT_FOUND    :  name not found
ERROR_IN_NAME     :  name contains an error
ERROR_IN_DATAFILE :  error in dictionary file "nam_dict.txt"

In the case of a "multiple" first name (e.g. "Carl J.D."), "get_gender" 
checks every single name and combines the results. For example, the name 
"A. Paul" is marked as male, because "A." can be any gender and "Paul" 
is a boy's name. 

An exception are some names like (e.g.) "Jean-Claude". This name is 
directly labeled as "IS_MALE", because there is a corresponding entry 
in the dictionary file.


========================================================================


The function "check_nickname"


The function "check_nickname" is used to check whether two first names 
are "equivalent" (e.g. "Bill" and "William"). 
This function has the following return values, which are defined in the 
file "gen_ext.h":

EQUIVALENT_NAMES  :  names are equivalent
NOT_EQUAL_NAMES   :  names are not equivalent

ERROR_IN_NAME     :  name contains an error
ERROR_IN_DATAFILE :  error in dictionary file "nam_dict.txt"


========================================================================


The function "find_similar_name" (new in version 1.2)


The dictionary of first names covers a vast range of names. 

Experience has shown that, for Germany at least, any names NOT found 
in the dictionary are mostly misspellings or typographical errors, 
e.g. "Konschieta" instead of "Conchita" or "Joesph" instead of "Joseph".

To get help in these cases, the function "find_similar_name" has been 
added to the program:

   int find_similar_name (char first_name[], int country,
           char result_string[], int len);

This function evaluates three factors to arrive at the most likely 
corrections:
- Extended Levenshtein function with "true" rule-based phonetics
  (for German)
  (from the program "addr" by the same author; see references).
- Frequency of name, plus weight of country.

All figures are weighed up to give a logarithmic measure of the likelihood 
of a given correction.


a)
"Normal" use of the function

The function returns the number of matches and a result string which 
contains a list of names. If more than one name is found, they are 
separated by "; " (semicolon plus space), with the most likely name coming 
first. 

Negative return values indicate an error (e.g. result string is too short).


b)
"Expanding" abbreviations (e.g. "Chr.")

Secondly, it is also possible to use an abbreviation (e.g. "Chr.") as a 
search string. The function will then try to "expand" the search name 
and return a list of matches mainly sorted by frequency, which is a measure 
of the probability of a given name. At most 20 matches will be returned.

Since the function is likely to return more than one or two matches, the 
result string must be long enough. Otherwise the list will be truncated.

You should also be aware that the function allows for "compressed umlauts".
For example, a search for "Ulk." (with '.') will give the matches "Ülker" 
and "Ülkü".


c)
Quality of the results: two caveats

This function is pretty good, but of course not perfect.
Any names returned by this function should be seen as suggestions 
rather than valid corrections.

For example, for the name "Soonja", which is a valid Korean name 
(albeit mostly written as "Soon Ja" or "Soon-Ja"), the first entry 
in the list of matches will be "Sonja", which is much more common.

Therefore, is too "dangerous" to apply corrections automatically.

Secondly:
As noted above, the phonetic rules used in this program are based on 
the German language. 
Therefore, they might not work very well for English and French names.


d)
Speed of the function

With todays computers, the running time should in most cases be in the 
order of about one millisecond or less.


========================================================================


"Standardizing" first names  (new in version 1.2)


Another useful application for the function "find_similar_name" is 
in generating standardized spellings for first names. 

a)
"Standardizing" Arabic first names

Arabic names often have a lot of "synonyms". These synonyms arise 
because the Arabic "alphabet" does NOT record the vowels. 
Only the consonants are written down.

Hence, a search for "Muhamed" will give you the following list:
Mohamed, Mohammad, Mohammed, Mohamad, Muhammad, Muhamed, Muhammed

To get a "standardized spelling", you simply do a normal function call 
and use only the first entry from the list of names.


b)
"Standardizing" Russian first names

Name from Belarus and Ukraine often differ from their Russian "equivalent".
In many cases, these names do not even look similar:

Russland                 Ukraine        Belarus
-----------------------------------------------------------
Aleksandr, Alexander     Oleksandr      Aliaksandr
Nikolai, Nikolay         Mykola         Mikalaj
Oleg                     Oleh           Aleh
Sergei, Sergey           Serhij         Siarhei, Siarhej
Tatiana, Tatyana         Tetyana        Tatsiana
Vladimir, Wladimir       Volodymyr      Uladzimir  ("U" !!)

As with Arabic first names, to get a "standardized spelling", you simply 
do a normal function call and use only the first entry from the list of 
names.


c)
Converting first names to utf8 or unicode

If you are planning to convert a gicen database from ASCII to utf8 or unicode, 
you might also want to convert first names to their "native" spelling.

The "original" function "find_similar_name" will simply return an ASCII string 
and cannot be used for this purpose.

However, the function "find_similar_name_utf8" or "find_similar_name_unicode", 
respectively, can do the job. 

Do the following:
1. Use "get_gender" to check if first name is in the dictionary of first names.
2. If it is, feed this name to "find_similar_name_utf8" or 
   "find_similar_name_unicode", respectively.
3. If the list of matches contains exactly one entry, you are done.

Examples:
Try the names "Ayse", "Danute" or "Sasa".


========================================================================


Other functions


Cleanup function

Upon starting the program, the dictionary file "nam_dict.txt" is statically 
opened to speed up data access.

The down side is that you have to close this file when you finally exit 
the program. Call the function "cleanup_gender()" for this task.



Use a file as input for "gender.c"

If you want to use gender.c in a "batch" job to determine the gender of
all names in a given file, use a separate line for each name and call:
   gender -get_gender -file  <file>  [-detail]
The suboption "-detail" will print detailed information on the frequency
of each names in the various countries.



Consistency check

Call the program "gender" with argument "-check_consistency" to do a 
consistency check. There should be no inconsistencies, of course.



Print all names of a given country

Call:   gender -print_names_of_country  <country>  [ -unicode_file=<file>" ]
Calling:  gender -print_names_of_country  all  [ -unicode_file=<file>" ]
will do printing for all countries.

The optional suboption "-unicode_file=<file>" generates a native unicode 
file, so you don't have to reformat any internal "replacement-chars" 
(array "umlauts_unicode[]") for non-iso8859-1 umlauts.



Print statistics

Call the program "gender" with argument "-statistics" or "-statistics -full"
to get elementary or full statistics, respectively. 
Printing full statistics may take a while...


========================================================================


Interfaces for unicode support (new in version 1.2)


The following functions are available for unicode data:

1.  int get_gender_unicode
           (char first_name[], int compare_mode, int country);

2.  int check_nickname_unicode (char first_name_1[], 
           char first_name_2[], int compare_mode, int country);

3.  int find_similar_name_unicode (char first_name[], int country,
           char result_string[], int two_byte_len);

These functions work in the same way as the "normal" functions 
"get_gender", "check_nickname" and "find_similar_name".

A caveat:
You must be aware that unicode support ist restricted to Latin alphabets. 
See the chapter "unicode chars" for a list of supported chars.


========================================================================


Interfaces for utf-8 support (new in version 1.2)


The following functions are available for utf-8 data:

1.  int get_gender_utf8
           (char first_name[], int compare_mode, int country);

2.  int check_nickname_utf8 (char first_name_1[], 
           char first_name_2[], int compare_mode, int country);

3.  int find_similar_name_utf8 (char first_name[], int country,
           char result_string[], int one_byte_len);

These functions work in the same way as the "normal" functions 
"get_gender", "check_nickname" and "find_similar_name".

A caveat:
You must be aware that utf8 support ist restricted to Latin alphabets. 
See the chapter "unicode chars" for a list of supported chars.


========================================================================


Unicode chars


Internally, this program uses the char set "iso8859-1".
"Non-iso" unicode chars are represented as follows:
   256 = <A/>
   257 = <a/>
   258 = <Â>
   259 = <â>
   260 = <A,>
   261 = <a,>
   262 = <C´>
   263 = <c´>
   268 = <C^> or <CH>
   269 = <c^> or <ch>
   271 = <d´>
   272 = <Ð> or <DJ>
   273 = <ð> or <dj>
   274 = <E/>
   275 = <e/>
   278 = <E°>
   279 = <e°>
   280 = <E,>
   281 = <e,>
   282 = <Ê>
   283 = <ê>
   286 = <G^>
   287 = <g^>
   290 = <G,>
   291 = <g´>
   298 = <I/>
   299 = <i/>
   304 = <I°>
   305 = <i>
   306 = <IJ>
   307 = <ij>
   310 = <K,>
   311 = <k,>
   315 = <L,>
   316 = <l,>
   317 = <L´>
   318 = <l´>
   321 = <L/>
   322 = <l/>
   325 = <N,>
   326 = <n,>
   327 = <N^>
   328 = <n^>
   336 = <Ö>
   337 = <ö>
   338 = Œ or <OE>
   339 = œ or <oe>
   344 = <R^>
   345 = <r^>
   350 = <S,>
   351 = <s,>
   352 = Š or <S^> or <SCH> or <SH>
   353 = š or <s^> or <sch> or <sh>
   354 = <T,>
   355 = <t,>
   357 = <t´>
   362 = <U/>
   363 = <u/>
   366 = <U°>
   367 = <u°>
   370 = <U,>
   371 = <u,>
   379 = <Z°>
   380 = <z°>
   381 = <Z^>
   382 = <z^>
  7838 = <ß>    /***  newly defined "Großes ß"  ***/

A plus char ('+') "inside" a name symbolizes a '-', ' ' or an empty string
(this option applies to Chinese and Korean names only).
Thus, "Jun+Wei" represents the names "Jun-Wei", "Jun Wei" and "Junwei".


========================================================================


Statistics of first names in dictionary


Number of first names   :   44568
   girl's names         :   16921
   boy's  names         :   17740
   unisex names         :    9907
Equivalent name pairs   :     734

Names from Great Britain  :  2232
Names from Ireland        :   876
Names from U.S.A.         :  3916
Names from Italy          :  2871
Names from Malta          :   526
Names from Portugal       :  1041
Names from Spain          :  1645
Names from France         :  1821
Names from Belgium        :  1518
Names from Luxembourg     :   564
Names from the Netherlands:  3302
Names from East Frisia    :  2589   ("Ostfriesland")
Names from Germany        :  2636
Names from Austria        :  1753
Names from Swiss          :  2464
Names from Iceland        :  1302
Names from Denmark        :  1139
Names from Norway         :  1064
Names from Sweden         :   974
Names from Finland        :   873
Names from Estonia        :  1121
Names from Latvia         :   710
Names from Lithuania      :   845
Names from Poland         :   359
Names from Czech Republic :   477
Names from Slovakia       :   477
Names from Hungary        :   371
Names from Romania        :  1971
Names from Bulgaria       :  1793
Names from Bosnia and Herzegovina:  1216
Names from Croatia        :   894
Names from Kosovo         :  1002
Names from Macedonia      :   851
Names from Montenegro     :   621
Names from Serbia         :   727
Names from Slovenia       :   636
Names from Albania        :  1748
Names from Greece         :   778
Names from Russia         :   489
Names from Belarus        :   508
Names from Moldova        :   405
Names from Ukraine        :   531
Names from Armenia        :   653
Names from Azerbaijan     :   764
Names from Georgia        :   293
Names from Kazakhstan/Uzbekistan,etc.:   862
Names from Turkey         :  1806
Names from Arabia/Persia  :  2025
Names from Israel         :  1067
Names from China          :  7334
Names from India/Sri Lanka:  1455
Names from Japan          :  1384
Names from Korea          :  1376
Names from Vietnam        :   307


Statistics for Great Britain  (statistics are very good):
                rare            medium          common         total
first  names:  1485  201 143 100 100  80  80  30 10  3  0 0 0   2232
girl's names:   629   95  92  51  57  46  43  15  3  1  0 0 0   1032
boy's  names:   650   80  35  37  33  26  34  14  7  2  0 0 0    918
unisex names:   206   26  16  12  10   8   3   1  0  0  0 0 0    282

Statistics for Ireland  (statistics are good):
                rare            medium          common         total
first  names:   311  134 124  78  80  65  53  25  6  0  0 0 0    876
girl's names:   175   73  60  43  46  44  27  16  2  0  0 0 0    486
boy's  names:   112   53  49  29  32  19  24   9  4  0  0 0 0    331
unisex names:    24    8  15   6   2   2   2   0  0  0  0 0 0     59

Statistics for U.S.A.  (statistics are very good):
                rare            medium          common         total
first  names:  2646  401 292 202 190 103  56  18  8  0  0 0 0   3916
girl's names:  1468  206 155  93  92  48  27   8  1  0  0 0 0   2098
boy's  names:   953  152  99  71  69  37  22  10  7  0  0 0 0   1420
unisex names:   225   43  38  38  29  18   7   0  0  0  0 0 0    398

Statistics for Italy  (statistics are very good):
                rare            medium          common         total
first  names:  1915  283 206 162 134  94  42  26  5  3  1 0 0   2871
girl's names:   910  142 118 101  75  45  21  12  1  1  1 0 0   1427
boy's  names:   999  141  88  61  59  49  21  14  4  2  0 0 0   1438
unisex names:     6    0   0   0   0   0   0   0  0  0  0 0 0      6

Statistics for Malta  (statistics are medium quality):
                rare            medium          common         total
first  names:   137   93  74  79  47  43  30  11  7  3  2 0 0    526
girl's names:    41   42  17  35  23  23  18   2  2  1  1 0 0    205
boy's  names:    89   48  50  40  20  20  12   9  5  2  1 0 0    296
unisex names:     7    3   7   4   4   0   0   0  0  0  0 0 0     25

Statistics for Portugal  (statistics are good):
                rare            medium          common         total
first  names:   509  181  84 120  70  36  14  13  7  3  3 1 0   1041
girl's names:   273   87  38  60  39  20   8   3  2  0  0 1 0    531
boy's  names:   235   92  46  58  29  16   6  10  5  3  3 0 0    503
unisex names:     1    2   0   2   2   0   0   0  0  0  0 0 0      7

Statistics for Spain  (statistics are very good):
                rare            medium          common         total
first  names:   955  207 180 106  78  56  30  16  9  6  1 1 0   1645
girl's names:   530  110  91  46  45  34  10   9  3  1  0 1 0    880
boy's  names:   416   96  88  60  30  21  19   6  6  5  1 0 0    748
unisex names:     9    1   1   0   3   1   1   1  0  0  0 0 0     17

Statistics for France  (statistics are very good):
                rare            medium          common         total
first  names:  1136  167 113 102  94  91  76  36  4  0  2 0 0   1821
girl's names:   686  111  72  56  60  58  43  13  0  0  1 0 0   1100
boy's  names:   313   42  35  42  33  29  33  21  3  0  1 0 0    552
unisex names:   137   14   6   4   1   4   0   2  1  0  0 0 0    169

Statistics for Belgium  (statistics are very good):
                rare            medium          common         total
first  names:   489  253 204 181 186 120  55  26  1  3  0 0 0   1518
girl's names:   270  158 121  91 104  60  26   8  0  2  0 0 0    840
boy's  names:   169   76  70  82  73  51  26  17  1  1  0 0 0    566
unisex names:    50   19  13   8   9   9   3   1  0  0  0 0 0    112

Statistics for Luxembourg  (statistics are very good):
                rare            medium          common         total
first  names:    77   59  86  77  83  76  56  35 11  2  2 0 0    564
girl's names:    40   25  40  51  52  52  30  13  1  1  1 0 0    306
boy's  names:    26   30  44  24  25  23  25  20  9  1  1 0 0    228
unisex names:    11    4   2   2   6   1   1   2  1  0  0 0 0     30

Statistics for the Netherlands  (statistics are very good):
                rare            medium          common         total
first  names:  1976  426 329 233 147  99  66  22  3  1  0 0 0   3302
girl's names:  1006  226 186 119  74  58  28   9  0  0  0 0 0   1706
boy's  names:   781  150 105  91  53  28  34  13  3  1  0 0 0   1259
unisex names:   189   50  38  23  20  13   4   0  0  0  0 0 0    337

Statistics for East Frisia  (statistics are good):
                rare            medium          common         total
first  names:  1411  529 210 147  91 106  62  31  2  0  0 0 0   2589
girl's names:   592  219 106  74  51  59  34   9  0  0  0 0 0   1144
boy's  names:   651  261  93  70  36  44  28  20  2  0  0 0 0   1205
unisex names:   168   49  11   3   4   3   0   2  0  0  0 0 0    240

Statistics for Germany  (statistics are very good):
                rare            medium          common         total
first  names:  1872  189 145 132  90  86  73  41  8  0  0 0 0   2636
girl's names:   917   99  89  79  49  40  44  24  0  0  0 0 0   1341
boy's  names:   929   77  54  53  41  45  29  17  8  0  0 0 0   1253
unisex names:    26   13   2   0   0   1   0   0  0  0  0 0 0     42

Statistics for Austria  (statistics are very good):
                rare            medium          common         total
first  names:  1090  175 126  99  84  69  59  40  7  4  0 0 0   1753
girl's names:   666  109  78  58  50  47  37  16  3  1  0 0 0   1065
boy's  names:   404   63  43  40  34  22  22  24  4  3  0 0 0    659
unisex names:    20    3   5   1   0   0   0   0  0  0  0 0 0     29

Statistics for Swiss  (statistics are very good):
                rare            medium          common         total
first  names:  1252  359 289 204 169 101  53  32  5  0  0 0 0   2464
girl's names:   723  205 161 113  99  52  27  12  2  0  0 0 0   1394
boy's  names:   501  142 125  88  66  47  24  20  3  0  0 0 0   1016
unisex names:    28   12   3   3   4   2   2   0  0  0  0 0 0     54

Statistics for Iceland  (statistics are very good):
                rare            medium          common         total
first  names:   329  199 188 148 142 122 100  50 19  5  0 0 0   1302
girl's names:   177   92  92  70  76  56  49  27  9  3  0 0 0    651
boy's  names:   152  107  96  78  66  66  51  23 10  2  0 0 0    651
unisex names:     0    0   0   0   0   0   0   0  0  0  0 0 0      0

Statistics for Denmark  (statistics are very good):
                rare            medium          common         total
first  names:   308  212 175 126  96  85  58  55 23  1  0 0 0   1139
girl's names:   180  118  97  71  48  54  31  31  9  0  0 0 0    639
boy's  names:   123   90  73  52  48  31  27  24 14  1  0 0 0    483
unisex names:     5    4   5   3   0   0   0   0  0  0  0 0 0     17

Statistics for Norway  (statistics are good):
                rare            medium          common         total
first  names:   413  183 142  89  89  69  59  13  7  0  0 0 0   1064
girl's names:   234   92  73  46  46  32  31   3  1  0  0 0 0    558
boy's  names:   170   88  69  43  43  37  28  10  6  0  0 0 0    494
unisex names:     9    3   0   0   0   0   0   0  0  0  0 0 0     12

Statistics for Sweden  (statistics are very good):
                rare            medium          common         total
first  names:   315  148 125 119  96  62  44  44 20  1  0 0 0    974
girl's names:   183   87  62  67  53  31  23  19 10  0  0 0 0    535
boy's  names:   125   58  62  50  42  31  21  25 10  1  0 0 0    425
unisex names:     7    3   1   2   1   0   0   0  0  0  0 0 0     14

Statistics for Finland  (statistics are good):
                rare            medium          common         total
first  names:   244  101 129 113 107  72  60  33 13  1  0 0 0    873
girl's names:   142   57  65  61  58  30  35  13  8  0  0 0 0    469
boy's  names:   102   43  64  52  49  42  25  20  5  1  0 0 0    403
unisex names:     0    1   0   0   0   0   0   0  0  0  0 0 0      1

Statistics for Estonia  (statistics are good):
                rare            medium          common         total
first  names:   449  183 103 122  93  93  56  22  0  0  0 0 0   1121
girl's names:   232  105  49  67  48  47  29  11  0  0  0 0 0    588
boy's  names:   214   78  54  55  45  46  27  11  0  0  0 0 0    530
unisex names:     3    0   0   0   0   0   0   0  0  0  0 0 0      3

Statistics for Latvia  (statistics are good):
                rare            medium          common         total
first  names:   121  119  76 115 108  83  52  28  6  1  1 0 0    710
girl's names:    79   63  30  60  65  45  26  18  2  0  0 0 0    388
boy's  names:    42   56  46  55  43  38  26  10  4  1  1 0 0    322
unisex names:     0    0   0   0   0   0   0   0  0  0  0 0 0      0

Statistics for Lithuania  (statistics are good):
                rare            medium          common         total
first  names:   399  103  89  52  70  57  43  18  8  6  0 0 0    845
girl's names:   132   49  41  21  31  29  21   8  5  2  0 0 0    339
boy's  names:   267   54  48  31  39  28  22  10  3  4  0 0 0    506
unisex names:     0    0   0   0   0   0   0   0  0  0  0 0 0      0

Statistics for Poland  (statistics are good):
                rare            medium          common         total
first  names:    94   40  57  27  39  26  25  24 19  7  1 0 0    359
girl's names:    50   22  32  14  22  11   6  13  9  5  1 0 0    185
boy's  names:    44   18  25  13  17  15  19  11 10  2  0 0 0    174
unisex names:     0    0   0   0   0   0   0   0  0  0  0 0 0      0

Statistics for Czech Republic  (statistics are good):
                rare            medium          common         total
first  names:   149   64  48  49  48  39  30  27 10 12  1 0 0    477
girl's names:    97   40  27  32  26  23  16  20  4  3  0 0 0    288
boy's  names:    52   24  21  17  21  16  14   7  6  9  1 0 0    188
unisex names:     0    0   0   0   1   0   0   0  0  0  0 0 0      1

Statistics for Slovakia  (statistics are good):
                rare            medium          common         total
first  names:    78   49  69  97  58  50  31  24 14  6  1 0 0    477
girl's names:    42   26  32  45  34  33  16  14  4  3  1 0 0    250
boy's  names:    36   23  37  52  24  17  15  10 10  3  0 0 0    227
unisex names:     0    0   0   0   0   0   0   0  0  0  0 0 0      0

Statistics for Hungary  (statistics are good):
                rare            medium          common         total
first  names:   117   49  20  34  37  36  29  21 21  7  0 0 0    371
girl's names:    65   28  11  17  18  26  20  15  9  2  0 0 0    211
boy's  names:    52   21   8  17  19  10   9   6 12  5  0 0 0    159
unisex names:     0    0   1   0   0   0   0   0  0  0  0 0 0      1

Statistics for Romania  (statistics are very good):
                rare            medium          common         total
first  names:  1098  413 158  93  56  60  49  27 10  5  2 0 0   1971
girl's names:   724  234  99  53  36  38  25  14  4  0  1 0 0   1228
boy's  names:   374  179  59  40  20  22  24  13  6  5  1 0 0    743
unisex names:     0    0   0   0   0   0   0   0  0  0  0 0 0      0

Statistics for Bulgaria  (statistics are good):
                rare            medium          common         total
first  names:   748  241 244 142 162 126  66  42 16  6  0 0 0   1793
girl's names:   433  144 137  81  87  73  49  24  6  3  0 0 0   1037
boy's  names:   313   97 105  61  75  53  17  18 10  3  0 0 0    752
unisex names:     2    0   2   0   0   0   0   0  0  0  0 0 0      4

Statistics for Bosnia and Herzegovina  (statistics are medium quality):
                rare            medium          common         total
first  names:   277  194 195 244 126 111  58  10  1  0  0 0 0   1216
girl's names:   113   96  85 104  40  49  28   7  1  0  0 0 0    523
boy's  names:   164   95 110 135  82  62  29   3  0  0  0 0 0    680
unisex names:     0    3   0   5   4   0   1   0  0  0  0 0 0     13

Statistics for Croatia  (statistics are good):
                rare            medium          common         total
first  names:   256  217 103  97  63  67  51  31  6  2  1 0 0    894
girl's names:   127   95  44  48  36  34  26  17  1  1  0 0 0    429
boy's  names:   126  122  58  45  26  33  25  14  5  1  1 0 0    456
unisex names:     3    0   1   4   1   0   0   0  0  0  0 0 0      9

Statistics for Kosovo  (statistics are medium quality):
                rare            medium          common         total
first  names:   257  188 184 174 117  52  27   3  0  0  0 0 0   1002
girl's names:   184   90  84  81  62  26  14   3  0  0  0 0 0    544
boy's  names:    73   96  99  93  55  26  13   0  0  0  0 0 0    455
unisex names:     0    2   1   0   0   0   0   0  0  0  0 0 0      3

Statistics for Macedonia  (statistics are medium quality):
                rare            medium          common         total
first  names:   129  228 172 102  86  71  40  16  7  0  0 0 0    851
girl's names:    65   86  72  29  32  32  17  11  4  0  0 0 0    348
boy's  names:    64  140  98  72  53  39  23   5  3  0  0 0 0    497
unisex names:     0    2   2   1   1   0   0   0  0  0  0 0 0      6

Statistics for Montenegro  (statistics are medium quality):
                rare            medium          common         total
first  names:   133  119  68  85  59  77  44  25 10  1  0 0 0    621
girl's names:    58   51  30  31  20  31  20  12  6  0  0 0 0    259
boy's  names:    73   67  37  53  39  46  23  13  4  1  0 0 0    356
unisex names:     2    1   1   1   0   0   1   0  0  0  0 0 0      6

Statistics for Serbia  (statistics are medium quality):
                rare            medium          common         total
first  names:   150  139 103  61 121  68  45  27 11  2  0 0 0    727
girl's names:    79   70  49  19  52  32  22  13  5  1  0 0 0    342
boy's  names:    70   66  54  41  67  36  22  14  6  1  0 0 0    377
unisex names:     1    3   0   1   2   0   1   0  0  0  0 0 0      8

Statistics for Slovenia  (statistics are medium quality):
                rare            medium          common         total
first  names:    62  144 106  85  90  62  48  31  7  1  0 0 0    636
girl's names:    34   66  44  33  36  36  25  13  2  1  0 0 0    290
boy's  names:    28   77  62  50  51  26  22  18  5  0  0 0 0    339
unisex names:     0    1   0   2   3   0   1   0  0  0  0 0 0      7

Statistics for Albania  (statistics are good):
                rare            medium          common         total
first  names:   591  236 305 247 184 103  62  19  1  0  0 0 0   1748
girl's names:   315   97  87  75  60  49  34  12  1  0  0 0 0    730
boy's  names:   272  137 217 172 124  54  28   7  0  0  0 0 0   1011
unisex names:     4    2   1   0   0   0   0   0  0  0  0 0 0      7

Statistics for Greece  (statistics are good):
                rare            medium          common         total
first  names:   359  123  49 107  45  32  27  18 11  5  2 0 0    778
girl's names:   242   74  30  58  25  20  16   9  7  1  1 0 0    483
boy's  names:   117   49  19  49  20  12  11   9  4  4  1 0 0    295
unisex names:     0    0   0   0   0   0   0   0  0  0  0 0 0      0

Statistics for Russia  (statistics are good):
                rare            medium          common         total
first  names:    73   51  69  73  46  55  34  33 28 23  4 0 0    489
girl's names:    44   28  38  44  28  29  14  14 11  8  2 0 0    260
boy's  names:    26   19  31  29  18  26  20  19 17 15  2 0 0    222
unisex names:     3    4   0   0   0   0   0   0  0  0  0 0 0      7

Statistics for Belarus  (statistics are medium quality):
                rare            medium          common         total
first  names:    42   88  56  67  66  48  48  36 33 14 10 0 0    508
girl's names:    21   50  29  35  38  22  22  19  8  4  7 0 0    255
boy's  names:    21   35  27  32  26  26  26  17 25 10  3 0 0    248
unisex names:     0    3   0   0   2   0   0   0  0  0  0 0 0      5

Statistics for Moldova  (statistics are medium quality):
                rare            medium          common         total
first  names:    44  104  72  37  42  31  31  21 14  9  0 0 0    405
girl's names:    31   60  38  18  20  12  19   8  6  4  0 0 0    216
boy's  names:    13   44  34  19  22  19  12  13  8  5  0 0 0    189
unisex names:     0    0   0   0   0   0   0   0  0  0  0 0 0      0

Statistics for Ukraine  (statistics are medium quality):
                rare            medium          common         total
first  names:    56   65  61  68  60  68  59  37 39 17  1 0 0    531
girl's names:    28   37  30  40  40  29  25  11 18  8  1 0 0    267
boy's  names:    28   28  26  28  20  39  34  26 21  9  0 0 0    259
unisex names:     0    0   5   0   0   0   0   0  0  0  0 0 0      5

Statistics for Armenia  (statistics are medium quality):
                rare            medium          common         total
first  names:   101  101  81  66 156  68  46  24 10  0  0 0 0    653
girl's names:    51   64  18  24  68  28  20  11  6  0  0 0 0    290
boy's  names:    49   36  60  41  87  39  26  13  4  0  0 0 0    355
unisex names:     1    1   3   1   1   1   0   0  0  0  0 0 0      8

Statistics for Azerbaijan  (statistics are medium quality):
                rare            medium          common         total
first  names:    91  211 157  85  93  76  31  18  2  0  0 0 0    764
girl's names:    30   93  86  42  52  37  14   7  2  0  0 0 0    363
boy's  names:    61  115  69  42  39  39  16  11  0  0  0 0 0    392
unisex names:     0    3   2   1   2   0   1   0  0  0  0 0 0      9

Statistics for Georgia  (statistics are medium quality):
                rare            medium          common         total
first  names:    43   55  44  24  30  19  37  24 12  4  1 0 0    293
girl's names:    18   32  13   8  13   6  16   8  9  1  1 0 0    125
boy's  names:    25   23  31  16  17  13  21  16  3  3  0 0 0    168
unisex names:     0    0   0   0   0   0   0   0  0  0  0 0 0      0

Statistics for Kazakhstan/Uzbekistan,etc.  (statistics are medium quality):
                rare            medium          common         total
first  names:    93  152 134 196 159  61  45  19  3  0  0 0 0    862
girl's names:    46   79  83 101  82  30  20  14  2  0  0 0 0    457
boy's  names:    45   71  46  92  76  31  25   5  1  0  0 0 0    392
unisex names:     2    2   5   3   1   0   0   0  0  0  0 0 0     13

Statistics for Turkey  (statistics are good):
                rare            medium          common         total
first  names:   455  408 280 272 212 115  45  12  6  1  0 0 0   1806
girl's names:   102  217 107 136 108  66  22   6  3  0  0 0 0    767
boy's  names:   331  170 150 120  98  48  22   5  3  1  0 0 0    948
unisex names:    22   21  23  16   6   1   1   1  0  0  0 0 0     91

Statistics for Arabia/Persia  (statistics are good):
                rare            medium          common         total
first  names:   431  529 405 281 187 119  48  14  8  3  0 0 0   2025
girl's names:   191  189 158  97  73  50  21   7  3  0  0 0 0    789
boy's  names:   235  331 227 176 109  62  26   7  5  3  0 0 0   1181
unisex names:     5    9  20   8   5   7   1   0  0  0  0 0 0     55

Statistics for Israel  (statistics are medium quality):
                rare            medium          common         total
first  names:   279  244 123 118  92 130  57  20  4  0  0 0 0   1067
girl's names:   138  111  62  43  36  66  21  11  2  0  0 0 0    490
boy's  names:   131  124  58  72  53  62  34   9  2  0  0 0 0    545
unisex names:    10    9   3   3   3   2   2   0  0  0  0 0 0     32

Statistics for China  (statistics are very good):
                rare            medium          common         total
first  names:  5319 1126 507 194  82  46  26  23  8  3  0 0 0   7334
girl's names:     0    0   0   0   0   0   0   0  0  0  0 0 0      0
boy's  names:     0    0   0   0   0   0   0   0  0  0  0 0 0      0
unisex names:  5319 1126 507 194  82  46  26  23  8  3  0 0 0   7334

Statistics for India/Sri Lanka  (statistics are good):
                rare            medium          common         total
first  names:   225  463 371 219 116  45  15   1  0  0  0 0 0   1455
girl's names:   131  136 121  76  47  18   6   0  0  0  0 0 0    535
boy's  names:    71  289 197 102  39  11   4   1  0  0  0 0 0    714
unisex names:    23   38  53  41  30  16   5   0  0  0  0 0 0    206

Statistics for Japan  (statistics are good):
                rare            medium          common         total
first  names:   429  266 204 142 153 110  56  22  2  0  0 0 0   1384
girl's names:   182   71  64  55  47  47  29  13  1  0  0 0 0    509
boy's  names:   215  180 130  77  98  56  21   5  1  0  0 0 0    783
unisex names:    32   15  10  10   8   7   6   4  0  0  0 0 0     92

Statistics for Korea  (statistics are good):
                rare            medium          common         total
first  names:    65  579 341 136  91  50  36  33 20 22  2 1 0   1376
girl's names:     6  200 115  43  25   8   0   0  0  1  0 0 0    398
boy's  names:    24  274 125  40   6   0   0   0  0  0  0 0 0    469
unisex names:    35  105 101  53  60  42  36  33 20 21  2 1 0    509

Statistics for Vietnam  (statistics are good):
                rare            medium          common         total
first  names:    32    0  36  41  39  39  34  31 26 19  7 1 2    307
girl's names:     0    0   0   0   0   0   0   0  3  0  0 0 1      4
boy's  names:     0    0   0   0   0   0   0   1  1  1  0 0 1      4
unisex names:    32    0  36  41  39  39  34  30 22 18  7 1 0    299



========================================================================


Changes in "nam_dict.txt" in version 1.1


Arabia/Persia:
  Noor        gender reclassified: F -> ?F
  Nur         gender reclassified: F -> ?F (notice: Turkish "Nur" remains female)
  Soad        gender reclassified: M -> F

Austria:
  Aron        name added
  Cäzilie     new name added (gender = F)
  Cyrill      new name added (gender = M)
  Edelhard    new name added (gender = M)
  Karli       name added (gender = M;  notice: "Karli" is female in U.S.A.)
  Kyrill      new name added (gender = M)
  Maximillian new name added (gender = M)

Belgium:
  Jean-Luc      name added (gender = M, frequency = 5)
  Jean-Paul     name added (gender = M, frequency = 5)
  Jean-Pierre   name added (gender = M, frequency = 6)
  Marie-Joseph  new name added (gender = ?M)
  Stéphane    gender reclassified: M -> ?M

Czech Republic:
  72 names have been added, bringing the total for Czech Republic to 477.
  # Hermina   name deleted (correct is: Hermína)
  # Ji<r^>i   corrected to:  Ji<r^>í
  # = Ji<r^>i Jirka    corrected to:   = Ji<r^>í Jirka
  # Katarina  name deleted (correct is: Katarína)
  # Maria     name deleted (correct is: Mária)
  Ota         gender reclassified: M -> ?M
  # Otto      corrected to mostly:  Oto
  # Šarka     corrected to:  Šárka
  Sláva       gender reclassified: ?M -> F  (notice: Slava is "?")
  # Sonja     corrected to mostly:  So<n^>a

East Frisia ("Ostfriesland"):
  Cäcilie     name added
  Diddo       new name added (gender = M)
  Jella       new name added (gender = F)

France:
  Jean-Luc      name added (gender = M, frequency = 6)
  Jean-Paul     name added (gender = M, frequency = 6)
  Jean-Pierre   name added (gender = M, frequency = 7)
  Marie-Joseph  new name added (gender = ?M)
  Stéphane      gender reclassified: M -> ?M

ex-U.S.S.R. (Asian) (Uzbekistan):
  Noor        gender reclassified: F -> ?F

Germany:
  Aaron       name added (frequency = 2)
  Aron        name added
  Cyrill      new name added (gender = M)
  Debora      name added (frequency = 2)
  Edelhard    new name added (gender = M)
  Gottwalt    new name added (gender = M)
  Joel        name added (frequency = 2)
  Jonas       frequency changed: 2 -> 3
  Jonathan    name added (frequency = 2)
  Lea         frequency changed: 2 -> 3
  Marzel      new name added
  Meikel      name added (gender = M)
  Monique     name added

Japan
  Eitaro      new name added (gender = M)
  Ken-ichi    alternative spelling of "Kenichi"  changed to: Ken'ichi
  Shin-ichi   alternative spelling of "Shinichi" changed to: Shin'ichi

Luxembourg:
  Aloyse       gender reclassified: ?M -> ?F
  Jean-Claude  name added (gender = M, frequency = 7)
  Jean-Luc     name added (gender = M, frequency = 5)
  Jean-Paul    name added (gender = M, frequency = 7)
  Jean-Pierre  name added (gender = M, frequency = 7)
  Stéphane     gender reclassified: M -> ?M

the Netherlands:
  Riny        gender reclassified: F -> ?

Slovakia:
  50 names have been added, bringing the total for Slovakia to 477
  (and quality_of_statistics has been changed to:  GOOD).
  # Albina    name deleted (correct is: Albína)
  # Al<z^>b<ê>ta   name deleted (correct is: Al<z^>beta)
  # Bedrich   name deleted (correct is: Bed<r^>ich)
  # Emilia    name deleted (correct is: Emília)
  # Emöke     name deleted (name is Hungarian)
  # Genoveva  name deleted (correct is: Genovéva)
  # Jeanette  name deleted
  # Katarina  name deleted (correct is: Katarína)
  # Lubica    name deleted (correct is: <L´>ubica)
  # Ludmila   name deleted (correct is: <L´>udmila)
  # Mikulaš   name deleted (correct is: Mikuláš)
  # Oldrich   name deleted (correct is: Old<r^>ich)
  # R<u°><z^>ena    name deleted (correct is: Ru<z^>ena)
  # Sonja     corrected to:  So<n^>a

Swiss:
  Alin        name added (gender = M)
  Aron        name added
  Cäcilie     name added
  Charlot     name added (gender = M)
  Cyrill      new name added (gender = M, frequency = 2)
  Edelhard    new name added (gender = M)
  Jean-Claude   name added (gender = M, frequency = 6)
  Jean-Luc      name added (gender = M, frequency = 4)
  Jean-Paul     name added (gender = M, frequency = 4)
  Jean-Pierre   name added (gender = M, frequency = 6)
  Kyrill      new name added (gender = M)
  Maggie      name added
  Pesche      new name added (gender = M)
  = Peter Pesche    "equivalent" names added
  Stéphane    gender reclassified: M -> ?M

Turkey:
  # Alin      name deleted (for Turkey) (correct is: Alim)


========================================================================


Changes in "nam_dict.txt" in version 1.1.1


Germany:
  Alin        name added (gender = ?F;  Alin remains "M" in Swiss, Romania, Moldova)
  Karli       name added

India/Sri Lanka:
  Deepla      new name added (gender = F)

Italy:
  Vitantonia  new name added (gender = F)
  Vitantonio  new name added (gender = M, frequency = 2)

Sweden:
  Nike        gender reclassified: F -> ?F  (Nike remains "F" in Great Britain)


========================================================================


Changes in "nam_dict.txt" in version 1.2


ex-U.S.S.R. (Asian):
  This "country" has been split up into:
  a) Armenia, b) Azerbaijan, c) Georgia
  d) Kazakhstan/Uzbekistan,etc.
     (= Kazakhstan, Kyrgyzstan, Tajikistan, Turkmenistan and Uzbekistan)

Frequency values for the following countries have been improved
(these changes are not listed here):
   Arabia/Persia
   Bosnia, Kosovo, Macedonia, Serbia, Slovenia
   Germany, East Frisia ("Ostfriesland")
   Iceland  (inclusion of second first names, like in (e.g.) "Anna María")
   Israel
   Russia, Belarus, Moldova, Ukraine,
       Armenia, Azerbaijan, Georgia,  Kazakhstan/Uzbekistan,etc.
   Slovakia
   Turkey

   Belgium           (slight improvements)
   France            (slight improvements)
   Denmark           (slight improvements)
   Great Britain     (slight improvements)
   Italy             (slight improvements)
   Luxembourg        (slight improvements)
   the Netherlands   (slight improvements)
   Spain             (slight improvements)
   Swiss             (slight improvements)
   U.S.A.            (slight improvements)


Albania:
  50+ names have been added, bringing the total for Albania to 1748.

  # Aferdita  name deleted  (correct is: "Afërdita")
  # Bajramaliu  name deleted
  # Kornelia  name deleted
  Kudret      gender reclassified: ?F -> M
  Leonarda    name added
  # Mahmud    name deleted
  # Mehmed    name deleted  (correct is: "Mehmet")
  Nevzat      gender reclassified: ? -> M
  Remzije     gender reclassified: ?F -> F
  Sadije      gender reclassified: ?F -> F
  # Said      name deleted
  # Salih     name deleted
  Shala       gender reclassified: M -> F
  Suat        gender reclassified: ? -> M
  # Theoharis   name deleted

Arabia/Persia:
  "Syntax extension" for Arabic/Persian names:
  A plus char ('+') "inside" a name symbolizes a '-', ' ' or an empty string.

  500+ names have been added (and some mispelled or foreign names have been deleted),
  bringing the total for Arabia to 2025.

  Akram       gender reclassified: F -> M
  Al          gender reclassified: M -> ?M  (Al remains "M" in U.S.A. and Britain)
  Ala         gender reclassified: F -> ?M  (Ala remains "F" in Lithuania and Moldova)
  Alaa        gender reclassified: M -> ?M
  Altaf       gender reclassified: F -> ?M  (i.e. "M" in Arabia, except "F" in Lebanon)
  Amel        gender reclassified: ?F -> F  (Amel remains "M" in Bosnia)
  Amine       gender reclassified: F -> M
  # Amor      name deleted
  Arshad      gender reclassified: F -> M
  Aschraf     gender reclassified: F -> ?M (i.e. "F" in Persia and "M" in Lebanon)
  Ashraf      gender reclassified: F -> ?M (i.e. "F" in Persia and "M" in Lebanon)
  Azam        gender reclassified: F -> M
  Azhar       gender reclassified: F -> ?M
  Bebe        gender reclassified: M -> F  (Bebe remains "M" in Romania)
  # Beka      name deleted
  # Berrin    name deleted
  Bijan       gender reclassified: M -> ?M
  Dalal       gender reclassified: F -> ?F
  # Damira    name deleted
  Dara        gender reclassified: M -> ?F (i.e. "F" in Arabia and "M" in Persia)
  Darin       gender reclassified: M -> F  (Darin remains "M" in U.S.A.)
  Dima        gender reclassified: F -> ?F
  Fakhri      gender reclassified: F -> M
  Farookh     gender reclassified: ?M -> M
  Fathy       gender reclassified: F -> M
  Fatin       gender reclassified: F -> ?F
  # Ferhan    name deleted
  # Ferit     name deleted
  Fida        gender reclassified: F -> ?M (i.e. "M" in Arabia, except "F" in Lebanon)
  # Georghios  name deleted
  Ghazal      gender reclassified: F -> M
  # Gritta    name deleted
  Hanan       gender reclassified: M -> F  (Hanan remains "M" in Israel)
  Hayat       gender reclassified: M -> ?M (i.e. "M" in Arabia, except "F" in Lebanon)
  # Heider    name deleted
  Hilal       gender reclassified: F -> ?F  (Hilal remains "F" in Turkey)
  Husni       gender reclassified: F -> M
  Ikbal       gender reclassified: F -> ?M  (i.e. "M" in Arabia, except "F" in Lebanon)
  Ilham       gender reclassified: M -> ?M  (Ilham remains "M" in Armenia/Azerbaijan/Georgia)
  Iqbal       gender reclassified: M -> ?M  (i.e. "M" in Arabia, except "F" in Lebanon)
  # Imen      name deleted
  # Inam      name deleted
  # Jemal     name deleted
  Jihan       gender reclassified: F -> ?M (i.e. "M" in Arabia,  except "F" in Lebanon)
  # Lila      name deleted
  Kami        name added (gender = M;  Kami remains "?F" in U.S.A.)
  # Kezban    name deleted
  # Makonnen  name deleted
  Malak       gender reclassified: F -> M
  # Mamuka    name deleted
  Mouloud     gender reclassified: F -> M
  Mukhtar     gender reclassified: F -> M
  # Naina     name deleted
  Nariman     gender reclassified: M -> F  (Nariman remains "M" in Kazakhstan)
  Naseem      gender reclassified: F -> M
  # Nazik     name deleted
  Nidal       gender reclassified: M -> ?M
  Nour        gender reclassified: F -> ?F
  Nuhad       gender reclassified: M -> ?M
  Nural       gender reclassified: F -> M  (Nural remains "F" in Turkey)
  Nusrat      gender reclassified: ? -> M
  Parveen     gender reclassified: ?M -> F  (Parveen remains "?M" in India)
  Rafa        gender reclassified: M -> F  (Rafa remains "M" in Spain)
  Rakhshan    gender reclassified: F -> M
  # Reshit    name deleted
  # Ribana    name deleted
  Rim         gender reclassified: ? -> F  (Rim remains "?" in Korea)
  Saadia      gender reclassified: M -> F      
  Sabri       gender reclassified: F -> M
  Saman       gender reclassified: M -> ?M
  Samy        gender reclassified: ?M -> M
  Schahin     gender reclassified: M -> ?F
  # Semih     name deleted
  # Seref     name deleted
  Shadi       gender reclassified: F -> ?M  (i.e. "F" in Persia and "M" in Lebanon)
  Shaheen     gender reclassified: M -> ?
  Shahien     gender reclassified: M -> ?
  Shahin      gender reclassified: M -> ?
  Shala       gender reclassified: M -> F
  Shervin     gender reclassified: M -> ?M
  Shiva       gender reclassified: M -> F  (Shiva remains "M" in India)
  Sima        gender reclassified: M -> F
  Sofiane     gender reclassified: F -> M
  Sufian      gender reclassified: F -> M
  Talal       gender reclassified: F -> M
  Tamir       gender reclassified: M -> ?M  (i.e. "M" in Arabia, except "F" in Lebanon)
  Uria        gender reclassified: ? -> ?M
  # Vassilia  name deleted
  Wafa        gender reclassified: M -> F
  Wafaa       gender reclassified: M -> F
  Walli       gender reclassified: F -> M  (Walli remains "F" in East Frisia)
  # Wassila   name deleted
  Yacine      gender reclassified: F -> M
  # Yacob     name deleted
  # Yacoub    name deleted
  # Yakov     name deleted
  # Yakub     name deleted
  # Yosef     name deleted
  # Zakir     name deleted
  # Zarah     name deleted

Armenia:
  500 names have been added, bringing the total for Armenia to 653.
  # Alexandra   name deleted
  Ninel       gender reclassified: M -> F  ("Ninel" remains "M" in Romania)
  Nver        gender reclassified: M -> F

Austria:
  20+ names have been added, bringing the total for Austria to 1753.
  Adriana     frequency changed: 2 -> 1
  Alex        gender reclassified: ? -> ?M
  Augusta     frequency changed: 2 -> 1
  # Birte     name deleted
  Carolin     frequency changed: 2 -> 1
  # Constance  name deleted
  Constanze   frequency changed: 2 -> 1
  Diemut      gender reclassified: F -> M
  Dorli       frequency changed: 2 -> 1
  Edeltrud    frequency changed: 4 -> 2
  # Elena     name deleted
  Georgine    frequency changed: 2 -> 1
  # Irina     name deleted
  # Ivo       name deleted
  Jenny       frequency changed: 2 -> 1
  Jessica     frequency changed: 2 -> 1
  Kreszenz    gender reclassified: M -> F
  Kirsten     gender reclassified: ?F -> F
  Leni        frequency changed: 2 -> 1
  Leonore     frequency changed: 2 -> 1
  Lilli       frequency changed: 2 -> 1
  # Margaret  name deleted
  # Margita   name deleted
  Margreth    frequency changed: 2 -> 1
  Marlis      frequency changed: 2 -> 1
  Michele     gender reclassified: F -> ?F
  # Natalia   name deleted
  # Renato    name deleted
  Sissy       frequency changed: 2 -> 1
  Susan       frequency changed: 2 -> 1
  Tamara      frequency changed: 4 -> 2
  # Tatjana   name deleted
  Thekla      frequency changed: 2 -> 1
  # Victor    name deleted
  # William   name deleted

Azerbaijan:
  500 names have been added, bringing the total for Azerbaijan to 764.
  # Alijja    name deleted
  Arzu        gender reclassified: M -> ?F
  Nika        gender reclassified: M -> F  (Nika remains "M" in Georgia)

Belarus:
  70 names have been added, bringing the total for Belarus to 508.
  Dima        gender reclassified: F -> M
  # Sana      name deleted
  Slava       gender reclassified: ? -> ?M  (Slava remains "F" in Bulgaria)
  Slawa       gender reclassified: ?F -> ?M

Belgium:
  15+ names have been added, bringing the total for Belgium to 1518.
  # Alda      name deleted
  Cléo        gender reclassified: ? -> ?F
  # Igor      name deleted
  Jordane     gender reclassified: ? -> ?M
  Maria       gender reclassified: F -> 1F
  Modeste     gender reclassified: ? -> ?M
  Narcisse    gender reclassified: F -> ?M
  Norbert     name added  (frequency = 5)
  Renaud      gender reclassified: F -> M

Bosnia and Herzegovina:
  Admira      new name added (gender = F)
  # Elnur     name deleted
  # Luca      name deleted

Bulgaria:
  # Inna      name deleted
  Jarka       name added (gender = F)
  Jawor       new name added (gender = M)
  # Joanna    name deleted
  Kristijana  name added
  Kristiyana  new name added (gender = F)
  Volodia     new name added (gender = M)

Czech Republic:
  # Miran     name deleted
  Yvetta      name added

Croatia:
  Darijo      new name added (gender = M)
  # Jale      name deleted
  Luca        gender reclassified: F -> M
  # Narcisa   name deleted
  # Norma     name deleted
  # Svea      name deleted

Denmark:
  # Antonia   name deleted
  # Detlef    name deleted
  Lasse       gender reclassified: ?M -> M
  Rigmor      gender reclassified: M -> F
  # Svea      name deleted
  # Ulrike    name deleted
  # Waldemar  name deleted

East Frisia ("Ostfriesland"):
  240+ names have been added, bringing the total for East Frisia to 2589.
  # Agnis     name deleted
  Alex        gender reclassified: ? -> M
  # Alla      name deleted
  # Andreja   name deleted
  # Aneta     name deleted
  Assel       gender reclassified: F -> M   ("Assel" remains "F" in Kazakhstan)
  # Asta      name deleted
  # Bilge     name deleted  ("Bilge" is a Turkish name)
  # Bozo      name deleted
  # Brigit    name deleted
  # Brigitt   name deleted
  # Claudi    name deleted
  # Dianne    name deleted
  Dirtje      gender reclassified: ? -> ?F
  # Draga     name deleted
  Eicke       gender reclassified: ?M -> ?
  Eike        gender reclassified: ?M -> ?
  # Elena     name deleted
  # Elfried   name deleted
  Ellen       gender reclassified: ?F -> F
  Else        gender reclassified: ? -> ?F
  # Elza      name deleted
  # Franco    name deleted
  # Freia     name deleted
  # Frits     name deleted
  # Frowine   name deleted
  # Gerard    name deleted
  Helge       gender reclassified: ? -> ?M
  Helke       gender reclassified: ? -> ?F
  # Henk      name deleted
  # Herman    name deleted
  Helke       gender reclassified: ? -> ?F
  Hilke       gender reclassified: ? -> ?F
  Hille       gender reclassified: ? -> ?F
  Imme        gender reclassified: ? -> ?F
  # Jürg      name deleted
  Kai         gender reclassified: ?M -> M
  Kay         gender reclassified: ?M -> M
  Kirsten     gender reclassified: ?F -> F
  # Konstantin  name deleted
  # Leopold   name deleted
  Lüken       gender reclassified: ? -> ?M
  # Magarethe  name deleted
  Maria       gender reclassified: F -> 1F
  Marian      gender reclassified: M -> ?M
  # Mate      name deleted
  Meike       gender reclassified: ? -> ?F
  Mike        gender reclassified: F -> ?M
  # Nicholas  name deleted
  Nicky       gender reclassified: ?F -> ?
  # Ragnhild  name deleted
  Reinke      gender reclassified: ? -> ?M
  Renke       gender reclassified: ? -> ?M
  # Sander    name deleted
  # Sibel     name deleted
  # Sjoerd    name deleted
  Steffi      gender reclassified: ?F -> F
  # Terese    name deleted
  # Theres    name deleted
  Tjade       gender reclassified: ? -> ?M
  Tjebbe      gender reclassified: ? -> ?M
  # Tomas     name deleted
  # Victor    name deleted
  Wilke       gender reclassified: ? -> ?M
  # Willem    name deleted

Estonia:
  Anne-Ly     new name added (gender = F, frequency = 4)
  Karli       gender reclassified: F -> M  (Karli remains "F" in U.S.A.)
  Ly          gender reclassified: ? -> F  (Ly remains "?" in Vietnam)
  Rauna       gender reclassified: M -> F

Finland:
  30+ names have been added, bringing the total for Finland to 873.
  # Elin      name deleted
  # Helke     name deleted
  Kaino       gender reclassified: ?F -> F
  Lasse       gender reclassified: ?M -> M
  # Marjaleena  name deleted
  # Mies      name deleted
  # Norma     name deleted
  Soini       gender reclassified: ? -> F
  Väinämö     new name added (gender = M, frequency = 2)

France:
  Alvine      gender reclassified: ? -> ?F
  Aurèle      gender reclassified: ? -> ?M
  Bonaventure   new name added (gender = M)
  Cléo        gender reclassified: ? -> ?F
  Jordane     gender reclassified: ? -> ?M
  Modeste     gender reclassified: ? -> ?M
  Narcisse    gender reclassified: ?F -> ?M
  # Pierick   name deleted
  Renaud      gender reclassified: F -> M

Georgia:
  180+ names have been added, bringing the total for Georgia to 293.
  Koba        gender reclassified: F -> M  (Koba remains "F" in the Netherlands)

Germany:
  660+ names have been added, bringing the total for Germany to 2636.
  Alex        gender reclassified: ? -> M
  # Angelia   name deleted
  # Annegrit  name deleted
  # Borries   name deleted
  # Brigit    name deleted
  # Brigitt   name deleted
  # Cara      name deleted
  # Christianne  name deleted
  Christl     gender reclassified: ?F -> F
  Eike        gender reclassified: ?M -> ?
  # Elena     name deleted
  # Eleonora  name deleted
  Frohmut     gender reclassified: M -> ?
  = Hans Johann   "equivalent" names added
  Helge       gender reclassified: M -> ?M
  # Hendrick  name deleted
  # Irina     name deleted
  # Jes       name deleted
  # Job       name deleted
  # Jochim    name deleted
  Kai         gender reclassified: ?M -> M
  Kay         gender reclassified: ?M -> M
  Kirsten     gender reclassified: ?F -> F
  # Lotar     name deleted
  # Ludgera   name deleted
  Marian      gender reclassified: M -> ?M
  # Martine   name deleted
  # Michele   name deleted
  # Michelle  name deleted
  Nicki       gender reclassified: ?F -> ?M
  Nicky       gender reclassified: ?F -> ?M
  # Rüdeger   name deleted
  # Siegried  name deleted
  # Siegrit   name deleted
  Siegtraud   gender reclassified: M -> F
  # Tamara    name deleted
  # Tatjana   name deleted
  # Teresa    name deleted
  Uli         gender reclassified: ? -> ?M
  Ulli        gender reclassified: ? -> ?M

Great Britain:
  Dot         new name added (gender = F)
  Kat         name added
  # Leila     name deleted

Greece:
  Agapios     new name added (gender = M)
  Kalomira    new name added (gender = F)
  Lavrenti    new name added (gender = F)
  Panos       new name added (gender = M)
  Pantasis    new name added (gender = M)

Iceland:
  150+ names have been added, bringing the total for Iceland to 1302.
  # Anita     corrected to:  Aníta
  # Barður    corrected to:  Bárður
  # Beinsteinn  corrected to:  Beinteinn
  # Burgni    corrected to:  Burkni
  Daði        gender reclassified: F -> M
  # Domhildur  corrected to:  Dómhildur
  Eilífur     gender reclassified: F -> M
  # Eleonóra  name deleted
  # Elinborg  corrected to:  Elínborg
  # Franklín  name deleted
  # Friðóra   corrected to:  Friðdóra
  # Gigja     corrected to:  Gígja
  # Guðrun    corrected to:  Guðrún
  # Hálfdan   corrected to:  Hálfdán
  # Hallvardur  corrected to:  Hallvarður
  # Höskeldur   corrected to:  Höskuldur
  # Húbert     name deleted
  # Ingebjörg  name deleted
  # Ingolfur  corrected to:  Ingólfur
  # Ingríður  corrected to:  Ingiríður
  # Irena     corrected to:  Írena
  # Karla     name deleted
  # Konrað    corrected to:  Konráð
  # Laila     name deleted
  # Leópold   name deleted
  # Loá       corrected to:  Lóa
  # Lúðvik    corrected to:  Lúðvík
  # Lýdía     corrected to:  Lydía
  # Marvin    name deleted
  # Nellý     name deleted
  # Oddrun    corrected to:  Oddrún
  Ólöf        gender reclassified: M -> F
  # Ríta      name deleted
  # Sigorósk  corrected to:  Sigurósk
  # Úndina    corrected to:  Úndína

India/Sri Lanka:
  20+ names have been added, bringing the total for India to 1455.
  # Ahmad     name deleted
  # Akram     name deleted
  # Amir      name deleted
  # Arash     name deleted
  # Aref      name deleted
  # Ashraf    name deleted
  # Asma      name deleted
  # Assia     name deleted
  Bijan       gender reclassified: M -> F
  # Bobby     name deleted
  # Bonita    name deleted
  # Calista   name deleted
  # Dilshad   name deleted
  # Edda      name deleted
  # Edward    name deleted
  # Elmas     name deleted
  # Esther    name deleted
  # Giacoma   name deleted
  # Hadi      name deleted
  # Hamit     name deleted
  # Hasibe    name deleted
  Humpy       gender reclassified: ? -> ?F
  # Ikbal     name deleted
  # Inder     name deleted
  # Iqbal     name deleted
  # Iskandar  name deleted
  # Jaba      name deleted
  # Jabbar    name deleted
  # Jang      name deleted
  # Josef     name deleted
  # Kamel     name deleted
  # Kang      name deleted
  Karan       gender reclassified: F -> M  ("Karan" remains "F" in Great Britain)
  # Kasem     name deleted
  Kailash     gender reclassified: M -> ?F
  # Lalith    name deleted
  # Linda     name deleted
  # Lionel    name deleted
  # Marija    name deleted
  # Marlis    name deleted
  # Massouma  name deleted
  # Naeem     name deleted
  # Nahid     name deleted
  # Naseem    name deleted
  # Nasib     name deleted
  # Nasrin    name deleted
  # Paula     name deleted
  # Phillip   name deleted
  # Raji      name deleted
  Rathy       gender reclassified: M -> ?M
  Rati        gender reclassified: M -> ?F
  # Rima      name deleted
  # Samar     name deleted
  # Samuel    name deleted
  # Selvie    name deleted
  # Sepideh   name deleted
  # Shabbir   name deleted
  # Shahnaz   name deleted
  # Shameen   name deleted
  # Simon     name deleted
  # Stephen   name deleted
  Suman       gender reclassified: F -> ?F
  # Syed      name deleted
  # Syeda     name deleted
  # Tanja     name deleted
  # Viola     name deleted
  # Virginia  name deleted
  # Wilson    name deleted

Ireland:
  # Elena     name deleted
  Padraigin   gender reclassified: M -> F

Israel:
  400+ names have been added, bringing the total for Israel to 1067.
  # Ismael    name deleted
  # Ivo       name deleted
  Meni        gender reclassified: F -> M  ("Meni" remains "F" in Greece)
  Mika        gender reclassified: F -> M
  # Renuka    name deleted
  # Rima      name deleted
  # Risa      name deleted
  # Sana      name deleted
  # Son       name deleted
  Yoni        gender reclassified: ?F -> ?M
  Yona        gender reclassified: F -> ?

Italy:
  30+ names have been added, bringing the total for Italy to 2871.
  # Aimone    name deleted  (is problably a misspelling of "Simone")
  # Igor      name deleted
  # Laila     name deleted
  # Leila     name deleted

Japan:
  Dai         gender reclassified: ? -> M
  Gen         gender reclassified: ?M -> M
  Noe         gender reclassified: ? -> F
  Shoshi      new name added (gender = M)  (Shoshi is "F" in Israel)

Kazakhstan/Uzbekistan,etc.:
  300 names have been added, bringing the total for Kazakhstan/Uzbekistan,etc.
  to 862.

  Aiman       gender reclassified: M -> F  (Aiman remains "M" in Arabia/Persia)
  Almaz       gender reclassified: F -> M  (Almaz remains "F" in Arabia/Persia)
  Anar        gender reclassified: M -> F  (Anar remains "M" in Azerbaijan)
  Ayan        gender reclassified: F -> M
  Azam        gender reclassified: F -> M
  Azhar       gender reclassified: F -> ?
  Bakhyt      gender reclassified: M -> ?
  # Helen     name deleted
  Slava       gender reclassified: ? -> ?M  (Slava remains "F" in Bulgaria)
  Zhanat      gender reclassified: F -> ?

Kosovo:
  300+ names have been added, bringing the total for Kosovo to 1002.
  # Aferdita  name deleted  (correct is: "Afërdita")
  Hazbije     gender reclassified: ?F -> F
  Mentore     gender reclassified: M -> F  (Mentore remains "M" in Italy)
  Njomza      gender reclassified: ?F -> F
  Remzije     gender reclassified: ?F -> F
  Sadije      gender reclassified: ?F -> F
  Safete      gender reclassified: ?F -> F
  Sebahat     gender reclassified: F -> M  (Sebahat remains "F" in Turkey)
  Selvije     gender reclassified: ?F -> F

Luxembourg:
  30 names have been added, bringing the total for Luxembourg to 564.
  # Frédérick  corrected to:  Frederick
  # Géry      corrected to:  Gery
  Narcisse    gender reclassified: F -> ?M
  Renaud      gender reclassified: F -> M
  # Séverin   corrected to:  Severin
  # Véra      corrected to:  Vera

Macedonia:
  # Almas     name deleted
  Nevzat      gender reclassified: ? -> M
  # Nikita    name deleted
  # Nver      name deleted
  # Memet     name deleted

Malta:
  Aloysius    frequency changed: 2 -> 1
  # Johann    name deleted

the Netherlands:
  40 names have been added, bringing the total for the Netherlands to 3302.
  Cleo        gender reclassified: ?M -> ?F
  Cornelis    frequency changed: 3 -> 5
  # Dylan     name deleted
  Heintje     gender reclassified: M -> ?
  Maria       gender reclassified: F -> 1F
  Marien      gender reclassified: M -> ?M
  Mirjan      gender reclassified: M -> ?F  (Mirjan remains "M" in Albania)
  # Violetta  name deleted
  Vonne       gender reclassified: M -> F
  Yoni        gender reclassified: ?F -> M

Norway:
  # Inken     name deleted
  Lasse       gender reclassified: ?M -> M
  Rigmor      gender reclassified: M -> F

Poland:
  = Lech Leszek   equivalent names added

Portugal:
  Diogo       gender reclassified: F -> M

Romania:
  10+ names have been added, bringing the total for Romania to 1971.
  # Angheluta  corrected to:  Anghelu<t,>a
  Anu<t,>a     frequency changed: 2 -> 3
  # Aristita   corrected to:  Aristi<t,>a
  C<â>lina     frequency changed: 2 -> 3
  # Catita     corrected to:  Cati<t,>a
  # Chirica    name deleted
  # Chirita    name deleted
  # Constanta  corrected to:  Constan<t,>a
  # Costica    corrected to:  Costic<â>
  # Crisan     corrected to:  Cri<s,>an
  # Crisana    corrected to:  Cri<s,>ana
  # Crisanta   corrected to:  Cri<s,>anta
  # Dinica     corrected to:  Dinic<â>
  # Dinita     corrected to:  Dini<t,><â>
  # Dinuta     name deleted
  # Dochita    corrected to:  Dochi<t,>a
  # Domnita    corrected to:  Domni<t,>a
  # Filita     corrected to:  Fili<t,>a
  # Filitita   corrected to:  Filiti<t,>a
  # Florenta   corrected to:  Floren<t,>a
  Garofi<t,>a  frequency changed: 1 -> 2
  Gavril       frequency changed: 5 -> 6
  Gavril<â>    frequency changed: 3 -> 4
  # Gentiana   name deleted ("Gentiana" is an Albanian name)
  # Gheorghita  corrected to:  Gheorghi<t,><â>
  # Gherghita   corrected to:  Gherghi<t,><â>
  # Ghiorghita  corrected to:  Ghiorghi<t,><â>
  # Gica       corrected to:  Gic<â>  (and gender reclassified:  F -> M)
  # Goergina   corrected to:  Georgina
  # Grigorita  corrected to:  Grigori<t,><â>
  # Hripsime   name deleted ("Hripsime" is an Armenian name)
  # Iliuta     corrected to:  Iliu<t,><a^>
  # Inna       name deleted
  # Inocentia  corrected to:  Inocen<t,>ia
  # Inocentiu  corrected to:  Inocen<t,>iu
  Ionica       corrected to: Ionic<â>  (and gender reclassified:  F -> M)
  # Ionita     corrected to:  Ioni<t,><â>
  # Ionut      corrected to:  Ionu<t,>
  # Ionuta     corrected to:  Ionu<t,>a
  # Lazar      corrected to:  Laz<â>r
  # Lazarica   corrected to:  Laz<â>rica
  # Lazarina   corrected to:  Laz<â>rina
  # Lititia    corrected to:  Liti<t,>ia
  # Lititiea   corrected to:  Liti<t,>iea
  # Mihaita    corrected to:  Mih<â>i<t,><â>
  # Milita     corrected to:  Mili<t,>a
  # Miluca     name deleted
  Miluta       corrected to:  Milu<t,><â>  (and gender reclassified:  F -> M)
  # Mitica     corrected to:  Mitic<â>
  Mitita       corrected to: Miti<t,><â>  (and gender reclassified:  F -> M)
  # Natasa     name deleted (correct is: Nata<s,>a)
  # Neluta     corrected to:  Nelu<t,>a
  # Nelutu     corrected to:  Nelu<t,>u
  # Nica       corrected to:  Nic<â>  (and gender reclassified:  F -> M)
  # Norma      name deleted
  # Nuta       name deleted (correct is: Nu<t,>a)
  # Onita      corrected to:  Oni<t,>a
  # Oprita     corrected to:  Opri<t,>a
  Petric<â>    gender reclassified:  F -> M
  # Petrut     corrected to:  Petru<t,>
  # Petruta    corrected to:  Petru<t,>a
  # Samuila    corrected to:  Samuil<â>
  # Savastita  corrected to:  Savasti<t,>a
  # Sevastita  corrected to:  Sevasti<t,>a
  # Sica       corrected to:  Sic<â>  (and gender reclassified:  F -> M)
  # Sofita     corrected to:  Sofi<t,>a
  # Stanica    corrected to:  St<â>nic<â>
  # Steluta    corrected to:  Stelu<t,>a
  # Tertulia   corrected to:  Ter<t,>ulia
  # Thoader    name deleted
  # Ticusor    corrected to:  Ticu<s,>or
  # Tita       corrected to:  Ti<t,><â>
  # Tudorica   name deleted
  # Tudorita   corrected to:  Tudori<t,>a
  # Vasilica   corrected to:  Vasilic<â>
  # Vicentiu   corrected to:  Vicen<t,>iu
  # Vincentiu  corrected to:  Vincen<t,>iu
  # Zamfirita  corrected to:  Zamfiri<t,>a
  # Zoita      corrected to:  Zoi<t,>a

Russia:
  10+ names have been added, bringing the total for Russia to 489.
  # Irada     name deleted
  # Julietta  name deleted
  # Marietta  name deleted
  Slava       gender reclassified: ? -> ?M  (Slava remains "F" in Bulgaria)
  # Tatijana  name deleted
  # Zulfija   name deleted
  # Zulfiya   name deleted

Serbia:
  # Gina      name deleted
  
Spain:
  Cesc           new name added (gender = M, frequency = 2)
  = Cesc Francisco  equivalent names added
  Corona         new name added (gender = F)
  Elenita        new name added (gender = F)
  # Igor         name deleted
  Leontxo        new name added (gender = M)
  Macia          new name added (gender = F)
  Teodorino      new name added (gender = M)
  Xabi           new name added (gender = M)
  = Xabi Xabier  equivalent names added
  = Xabi Xavier  equivalent names added
  Xavi           new name added (gender = M)
  = Xavi Xavier  equivalent names added

Sweden:
  40+ names have been added, bringing the total for Sweden to 974.
  # Ana       name deleted
  # Constance  name deleted
  # Frederik  name deleted
  Gottfrid    frequency changed: 1 -> 2
  # Gunilla   name deleted
  # Hanne     name deleted
  Katrine     frequency changed: 1 -> 2
  Lasse       gender reclassified: ?M -> M
  # Matti     name deleted
  Ralph       frequency changed: 1 -> 2
  Rigmor      gender reclassified: M -> F

Swiss:
  20+ names have been added, bringing the total for Swiss to 2464.
  Alex        gender reclassified: ? -> ?M
  Elena       frequency changed: 5 -> 3
  # Igor      name deleted
  # Irina     name deleted
  Narcisse    gender reclassified: F -> ?F
  # Natalia   name deleted
  Renaud      gender reclassified: F -> M
  Tamara      frequency changed: 4 -> 2
  # Tatjana   name deleted
  Victor      frequency changed: 4 -> 2
  # Violetta  name deleted
  # Vivien    name deleted

Turkey:
  450 names have been added, bringing the total for Turkey to 1806.

  Af<s,>ar    gender reclassified: F -> M
  Alev        gender reclassified: ?F -> F
  # Almas     name deleted
  A<s,>k<i>n  gender reclassified: M -> ?M
  Azam        gender reclassified: F -> M
  Ba<s,>ak    gender reclassified: ?M -> F
  Bilgehan    gender reclassified: ?M -> ?F
  Burçak      gender reclassified: ?M -> ?F
  Burçin      gender reclassified: ? -> F
  Ça<g^>layan gender reclassified: F -> M
  Dünya       gender reclassified: F -> ?F
  # Efrahim   name deleted
  Elvan       gender reclassified: F -> ?F
  Emek        gender reclassified: ? -> M
  Ergül       gender reclassified: M -> ?
  Eser        gender reclassified: ? -> M
  Fatos       gender reclassified: M -> F
  # Fuad      name deleted  (correct is: "Fuat")
  Gülhan      gender reclassified: M -> F
  Gündüz      gender reclassified: ?F -> ?M
  Güner       gender reclassified: M -> ?
  Güne<s,>    gender reclassified: M -> ?M
  Güngör      gender reclassified: M -> ?M
  Günsel      gender reclassified: ?M -> F
  # Hafiz     name deleted
  # Hava      name deleted  (correct is: "Havva")
  Hayat       gender reclassified: F -> ?
  <I°>lke     gender reclassified: ?F -> F
  It<i>r      gender reclassified: ? -> M
  Jale        gender reclassified: M -> F
  Kader       gender reclassified: M -> ?
  Kudret      gender reclassified: ?F -> ?M
  # Malak     name deleted
  Merih       gender reclassified: ? -> M
  # Nahid     name deleted
  Nariman     gender reclassified: M -> F  (Nariman remains "M" in Kazakhstan)
  Nevzat      gender reclassified: ? -> M
  Ömür        gender reclassified: ?M -> ?
  Özen        gender reclassified: ? -> ?M
  # Parvin    name deleted
  Serhan      gender reclassified: ?M -> M
  Sever       gender reclassified: ? -> F
  Sezgin      gender reclassified: M -> ?M
  # Sona      name deleted  ("Sona" is an Armenian name)
  Suna        gender reclassified: ?F -> F
  Sunay       gender reclassified: M -> F
  Suphi       gender reclassified: F -> M
  Tanju       gender reclassified: ? -> M
  Tansu       gender reclassified: ?M -> ?
  Taylan      gender reclassified: ?M -> M
  Tülay       gender reclassified: ?F -> F
  # Tural     name deleted
  U<g^>ur     gender reclassified: ?M -> M
  Ufuk        gender reclassified: ?M -> M
  Ülkü        gender reclassified: ?F -> F
  Ümit        gender reclassified: ?M -> M
  Yücel       gender reclassified: M -> ?
  Ya<s,>ar    gender reclassified: ?M -> M
  Yurdagül    gender reclassified: ?F -> F
  Ziya        gender reclassified: ?M -> M

Ukraine:
  80+ names have been added, bringing the total for Ukraine to 531.
  # Lila      name deleted

U.S.A.:
  15+ names have been added, bringing the total for U.S.A. to 3916.
  # Deedee    name changed to: DeeDee
  Gale        gender reclassified: ? -> ?F
  # Latoya    corrected to:  LaToya


========================================================================


Frequently Asked Questions


a)
Is it possible to use this program in a commercial software project?

This is exactly what the Library GPL has been designed for.
See the file "LIB_GPLG.TXT" for details.

Probably the "safest" way to comply with the rules of the LGPL is
to put the program in a separate library (e.g. Windows-DLL).
In this way, only this library is subject to the LGPL and the "rest"
of the project still has the "old" rights of its owner, so you don't
have to give away your source code.


b)
Is a Java version of this program available?

The company which has written a Java version of my program "phonet"
has also promised to develop a native Java version of "gender" under
the LGPL - see:  https://opensource.softmethod.de/trac/opensource
The project name will be "gender4j".

Alternatively, you can also write a wrapper class in Java which uses 
the Java Native Interface to call a C library.
There is an excellent article (in German) telling you how to do it:
"Kaffee mit Vitamin C", c't, issue 20/2000, pp.242-247
or:  www.heise.de/ct, soft-link 0020242. 


c)
Is a PHP version of this programm available?

As of September 2008, Anatoliy Belsky ("anatoliy@belsky.info") 
is doing just that. Currently, the project is available from:
  http://gender.rubay.de .
He thinks it is likely that the project will finally be included in:
  http://pecl.php.net .

The PHP version will have the same name as the original program
(i.e. "gender"). Both the PHP version and the database of first names
included in the project will be subject to the PHP licence.


d)
Can I get a graphical user interface for this program?

Thanks to the efforts of Eike "Zed" Schmidt ("Zed_Gecko@gmx.de"), 
a simple graphical user interface for "gender" is now available.
See:  www.autohotkey.net/~Zed_Gecko/gender/WinGender.zip


e)
What is the speed of this program?

Due to the coherent use of binary search algorithms, this program runs 
very fast even on an old computer.

In order to get measurable running times, you have to do tens or even 
hundreds of thousands of function calls.


========================================================================


What about Wikipedia / Wiktionary ?


In 2009, the author plans to make his dictionary of first names available 
in Wikipedia or Wiktionary.

The dictionary of first names (file "nam_dict.txt") is subject to the 
GNU Free Documentation License. Therefore, anybody can use this file to 
e.g. create national lists of first names in Wiktionary. 

As you can see in a comment on
  http://en.wikipedia.org/wiki/List_of_Italian_given_names
it might be more appropriate to add any lists of first names to Wiktionary
rather than to Wikipedia.

If you want to do it yourself, call  
"gender -print_names_of_country  <country>  -unicode_file=<file>"
for this purpose. The suboption "-unicode_file=<file>" generates a native 
unicode file, so you don't have to reformat any internal "replacement-chars" 
(array "umlauts_unicode[]") for non-iso8859-1 umlauts.

Of course, you also have to give proper credit to author and data source.
The data source should at least be identified by:
    Source:
    File "nam_dict.txt" from www.heise.de/ct, soft-link 0717182
    (c) by Jörg MICHAEL, Hannover, Germany, 2007-2008
(To avoid typos, it is best to copy and paste these lines.)

Some additional remarks:

a)
There is not a unique well-defined rule for transcriptions from cyrillic 
alphabets. Many cyrillic names have more than one valid transcription,
and the transcription rules also vary from country to country.

For example, the following names refer to the same "original" cyrillic 
name:
 - Aleksander, Aleksandr, Alexander, Alexandr, (and sometimes even Oleksandr)
 - Maria, Marija and Mariya
 - Tatiana, Tatjana and Tatyana
 - Vladimir and Wladimir

Since this dictionary emphasizes the true frequency of a name, 
I strived to give all valid "normal" transcriptions equal or at least 
similar frequency values.

Hence, if you prepare names lists for Russia, Belarus, Ukraine or 
Bulgaria, you have to familarize yourself with the subleties of the 
various cyrillic alphabets.

b)
Last but not least, Chinese and Korean names (and also some of Arabic names)
need special formatting.
For all these countries, I have used a plus char ('+') "inside" a name 
to symbolize '-', ' ' or an empty string. Thus, "Jun+Wei" represents the 
names "Jun-Wei", "Jun Wei" and "Junwei".

In practice however, most Chinese first names appear as a "single" name,
while Korean first names are mostly written as separate names.
Therefore, the Chinese name "Xiao+Wei" should be formatted as "Xiaowei",
while the Korean name "Kyung+Hee" should be formatted as "Kyung Hee".


========================================================================


WWW resources for this program


http://www.heise.de/ct/ftp/07/17/182
or  http://www.heise.de/ct, soft-link 0717182.



Unicode and utf-8:
Can be looked up in Wikipedia.



Program "addr" (including phonetically extended Levenshtein function):

http://www.heise.de/ct/ftp/07/20/214
or  http://www.heise.de/ct, soft-link 0720214.


========================================================================


References


a)
Dictionary of first names:

J. Michael: 40000 Namen, Anredebestimmung anhand des Vornamens, 
c't, issue 17/2007, pp. 182-183.


b)
Phonetic conversion for German:

J. Michael: Doppelgänger gesucht, Ein Programm für kontextsensitive 
phonetische Textumwandlung, c't, issue 25/1999, pp. 252-261 
(also called "Hannoveraner Phonetik").


c)
Program "addr":
Error-tolerant database selects (for mail addresses):

J. Michael: Von Hinz und Kuntz, Ein Programmpaket zur fehlertoleranten 
Anschriftensuche, c't, issue 20/2007, pp. 214-219.


d)
Levenshtein function:

Vladimir I. Levenshtein: Binary Codes Capable of Correcting Deletions, 
Insertions and Reversals, Soviet Physics Doklady, vol. 10, pp. 707-709 
(1965).

G. Ebner: Wort-Arithmetik, Phonetische Ähnlichkeiten mit der 
Levenshtein-Distanz errechnet, c't, issue 07/1989, pp. 192-208.

J. Michael, Joker im Spiel, Erweiterung der Levenshtein-Funktion 
auf Wildcards, c't, issue 03/1994, pp. 230-239.

J. Michael: Von Hinz und Kuntz, Ein Programmpaket zur fehlertoleranten 
Anschriftensuche, c't, issue 20/2007, pp. 214-219.


========================================================================


History of the program


2007-05-23:  The first version of this program is submitted for
             publication in a German computer magazine (c't).
2007-08-06:  Version 1.0 is published in c't.

2007-09-07:  Version 1.1:
             gender.c:   A simple caching mechanism has been implemented
                         and some minor bugs have been corrected.
             nam_dict.txt: 50+ names (most of them from Czech Republic
                           and Slovakia) have been added
                           and a few errors have been corrected.

2007-11-15:  Version 1.1.1:  A few names have been added or reclassified.

2008-11-30:  Version 1.2:
             gender.c :  - The function "find_similar_name" 
                           (plus "standardize_arabic_name") has been added
                           (see documentation in this file).
                         - New functions to support unicode and utf-8
                           have been added (see documentation in this file).
                         - The function "internal_search" has been improved
                           and some minor bugs have been corrected.
             gen_ext.h:  The macro "GENDER_COMPARE_EXACT" has been deleted
                         and the new default is "COMPARE_EXPANDED_UMLAUTS".

             nam_dict.txt: - 3200+ names (most of them being from Asia,
                             and Germany/East Frisia) have been added
                             and a number of errors have been corrected
                             (e.g. "Rigmor" reclassified to: female).
                           - Frequency values for many countries have
                             been improved.
             "ex-U.S.S.R. (Asian)" has been split into:
                           a) Armenia, b) Azerbaijan, c) Georgia
                           d) Kazakhstan/Uzbekistan,etc.


========================================================================
(End of file "readme.txt")
