#include "porter_stemmer.h"

/* This is the Porter stemming algorithm, coded up in ANSI C by the
   author. It may be be regarded as cononical, in that it follows the
   algorithm presented in

   Porter, 1980, An algorithm for suffix stripping, Program, Vol. 14,
   no. 3, pp 130-137,

   only differing from it at the points maked --DEPARTURE-- below.

   The algorithm as described in the paper could be exactly replicated
   by adjusting the points of DEPARTURE, but this is barely necessary,
   because (a) the points of DEPARTURE are definitely improvements, and
   (b) no encoding of the Porter stemmer I have seen is anything like
   as exact as this version, even with the points of DEPARTURE!

   You can compile it on Unix with 'gcc -O3 -o stem stem.c' after which
   'stem' takes a list of inputs and sends the stemmed equivalent to
   stdout.

   The algorithm as encoded here is particularly fast.
*/

#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

/* The main part of the stemming algorithm starts here. b is a buffer
   holding a word to be stemmed. The letters are in b[k0], b[k0+1] ...
   ending at b[k]. In fact k0 = 0 in this demo program. k is readjusted
   downwards as the stemming progresses. Zero termination is not in fact
   used in the algorithm.

   Note that only lower case sequences are stemmed. Forcing to lower case
   should be done before stem(...) is called.
*/

// static char * b;       /* buffer for word to be stemmed */
// static int k,k0,j;     /* j is a general offset into the string */

/* cons(i) is TRUE <=> b[i] is a consonant. */

int cons(char *b, int *k, int *k0, int *j, int i)
{  switch (b[i])
   {  case 'a': case 'e': case 'i': case 'o': case 'u': return FALSE;
      case 'y': return (i==*k0) ? TRUE : !cons(b, k, k0, j, i-1);
      default: return TRUE;
   }
}

/* m() measures the number of consonant sequences between k0 and j. if c is
   a consonant sequence and v a vowel sequence, and <..> indicates arbitrary
   presence,

      <c><v>       gives 0
      <c>vc<v>     gives 1
      <c>vcvc<v>   gives 2
      <c>vcvcvc<v> gives 3
      ....
*/

int m(char *b, int *k, int *k0, int *j)
{  int n = 0;
   int i = *k0;
   while(TRUE)
   {  if (i > *j) return n;
      if (! cons(b, k, k0, j, i)) break; i++;
   }
   i++;
   while(TRUE)
   {  while(TRUE)
      {  if (i > *j) return n;
            if (cons(b, k, k0, j, i)) break;
            i++;
      }
      i++;
      n++;
      while(TRUE)
      {  if (i > *j) return n;
         if (! cons(b, k, k0, j, i)) break;
         i++;
      }
      i++;
    }
}

/* vowelinstem() is TRUE <=> k0,...j contains a vowel */

int vowelinstem(char *b, int *k, int *k0, int *j)
{  int i; for (i = *k0; i <= *j; i++) if (! cons(b, k, k0, j, i)) return TRUE;
   return FALSE;
}

/* doublec(j) is TRUE <=> j,(j-1) contain a double consonant. */

int doublec(char *b, int *k, int *k0, int *j, int j2)
{  if (j2 < (*k0)+1) return FALSE;
   if (b[j2] != b[j2-1]) return FALSE;
   return cons(b, k, k0, j, j2);
}

/* cvc(i) is TRUE <=> i-2,i-1,i has the form consonant - vowel - consonant
   and also if the second c is not w,x or y. this is used when trying to
   restore an e at the end of a short word. e.g.

      cav(e), lov(e), hop(e), crim(e), but
      snow, box, tray.

*/

int cvc(char *b, int *k, int *k0, int *j, int i)
{  if (i < (*k0)+2 || !cons(b, k, k0, j, i) || cons(b, k, k0, j, i-1) || !cons(b, k, k0, j, i-2)) return FALSE;
   {  int ch = b[i];
      if (ch == 'w' || ch == 'x' || ch == 'y') return FALSE;
   }
   return TRUE;
}

/* ends(s) is TRUE <=> k0,...k ends with the string s. */

int ends(char *b, int *k, int *k0, int *j, char * s)
{  int length = s[0];
   if (s[length] != b[*k]) return FALSE; /* tiny speed-up */
   if (length > (*k)-((*k0)+1)) return FALSE;
   if (memcmp(b+(*k)-length+1,s+1,length) != 0) return FALSE;
   *j = (*k) - length;
   return TRUE;
}

/* setto(s) sets (j+1),...k to the characters in the string s, readjusting
   k. */

void setto(char *b, int *k, int *k0, int *j, char * s)
{  int length = s[0];
   memmove(b+(*j)+1,s+1,length);
   *k = (*j)+length;
}

/* r(s) is used further down. */

void r(char *b, int *k, int *k0, int *j, char * s) {
  if (m(b, k, k0, j) > 0) {
    setto(b, k, k0, j, s);
  }
}

/* step1ab() gets rid of plurals and -ed or -ing. e.g.

       caresses  ->  caress
       ponies    ->  poni
       ties      ->  ti
       caress    ->  caress
       cats      ->  cat

       feed      ->  feed
       agreed    ->  agree
       disabled  ->  disable

       matting   ->  mat
       mating    ->  mate
       meeting   ->  meet
       milling   ->  mill
       messing   ->  mess

       meetings  ->  meet

*/

void step1ab(char *b, int *k, int *k0, int *j)
{  if (b[*k] == 's')
   {  if (ends(b, k, k0, j, "\04" "sses")) {
        *k = (*k) - 2;
      } else if (ends(b, k, k0, j, "\03" "ies")) {
        setto(b, k, k0, j, "\01" "i");
      } else if (b[(*k)-1] != 's') {
        *k = (*k) - 1;
      }
   }
   if (ends(b, k, k0, j, "\03" "eed")) {
     if (m(b, k, k0, j) > 0) {
        *k = (*k) - 1;
     }
   } else if ((ends(b, k, k0, j, "\02" "ed") || ends(b, k, k0, j, "\03" "ing")) && vowelinstem(b, k, k0, j)) {
      *k = *j;
      if (ends(b, k, k0, j, "\02" "at")) setto(b, k, k0, j, "\03" "ate"); else
      if (ends(b, k, k0, j, "\02" "bl")) setto(b, k, k0, j, "\03" "ble"); else
      if (ends(b, k, k0, j, "\02" "iz")) setto(b, k, k0, j, "\03" "ize"); else
      if (doublec(b, k, k0, j, *k)) {
        *k = (*k) - 1;
         {  int ch = b[*k];
            if (ch == 'l' || ch == 's' || ch == 'z') {
              *k = (*k) + 1;
            }
         }
      }
      else if (m(b, k, k0, j) == 1 && cvc(b, k, k0, j, *k)) setto(b, k, k0, j, "\01" "e");
  }
}

/* step1c() turns terminal y to i when there is another vowel in the stem. */

void step1c(char *b, int *k, int *k0, int *j) {
  if (ends(b, k, k0, j, "\01" "y") && vowelinstem(b, k, k0, j)) {
    b[(*k)] = 'i';
  }
}


/* step2() maps double suffices to single ones. so -ization ( = -ize plus
   -ation) maps to -ize etc. note that the string before the suffix must give
   m() > 0. */

void step2(char *b, int *k, int *k0, int *j) { switch (b[(*k)-1])
{
    case 'a': if (ends(b, k, k0, j, "\07" "ational")) { r(b, k, k0, j, "\03" "ate"); break; }
              if (ends(b, k, k0, j, "\06" "tional")) { r(b, k, k0, j, "\04" "tion"); break; }
              break;
    case 'c': if (ends(b, k, k0, j, "\04" "enci")) { r(b, k, k0, j, "\04" "ence"); break; }
              if (ends(b, k, k0, j, "\04" "anci")) { r(b, k, k0, j, "\04" "ance"); break; }
              break;
    case 'e': if (ends(b, k, k0, j, "\04" "izer")) { r(b, k, k0, j, "\03" "ize"); break; }
              break;
    case 'l': if (ends(b, k, k0, j, "\03" "bli")) { r(b, k, k0, j, "\03" "ble"); break; } /*-DEPARTURE-*/

 /* To match the published algorithm, replace this line with
    case 'l': if (ends("\04" "abli")) { r("\04" "able"); break; } */

              if (ends(b, k, k0, j, "\04" "alli")) { r(b, k, k0, j, "\02" "al"); break; }
              if (ends(b, k, k0, j, "\05" "entli")) { r(b, k, k0, j, "\03" "ent"); break; }
              if (ends(b, k, k0, j, "\03" "eli")) { r(b, k, k0, j, "\01" "e"); break; }
              if (ends(b, k, k0, j, "\05" "ousli")) { r(b, k, k0, j, "\03" "ous"); break; }
              break;
    case 'o': if (ends(b, k, k0, j, "\07" "ization")) { r(b, k, k0, j, "\03" "ize"); break; }
              if (ends(b, k, k0, j, "\05" "ation")) { r(b, k, k0, j, "\03" "ate"); break; }
              if (ends(b, k, k0, j, "\04" "ator")) { r(b, k, k0, j, "\03" "ate"); break; }
              break;
    case 's': if (ends(b, k, k0, j, "\05" "alism")) { r(b, k, k0, j, "\02" "al"); break; }
              if (ends(b, k, k0, j, "\07" "iveness")) { r(b, k, k0, j, "\03" "ive"); break; }
              if (ends(b, k, k0, j, "\07" "fulness")) { r(b, k, k0, j, "\03" "ful"); break; }
              if (ends(b, k, k0, j, "\07" "ousness")) { r(b, k, k0, j, "\03" "ous"); break; }
              break;
    case 't': if (ends(b, k, k0, j, "\05" "aliti")) { r(b, k, k0, j, "\02" "al"); break; }
              if (ends(b, k, k0, j, "\05" "iviti")) { r(b, k, k0, j, "\03" "ive"); break; }
              if (ends(b, k, k0, j, "\06" "biliti")) { r(b, k, k0, j, "\03" "ble"); break; }
              break;
    case 'g': if (ends(b, k, k0, j, "\04" "logi")) { r(b, k, k0, j, "\03" "log"); break; } /*-DEPARTURE-*/

 /* To match the published algorithm, delete this line */

} }

/* step3() deals with -ic-, -full, -ness etc. similar strategy to step2. */

void step3(char *b, int *k, int *k0, int *j) { switch (b[(*k)])
{
    case 'e': if (ends(b, k, k0, j, "\05" "icate")) { r(b, k, k0, j, "\02" "ic"); break; }
              if (ends(b, k, k0, j, "\05" "ative")) { r(b, k, k0, j, "\00" ""); break; }
              if (ends(b, k, k0, j, "\05" "alize")) { r(b, k, k0, j, "\02" "al"); break; }
              break;
    case 'i': if (ends(b, k, k0, j, "\05" "iciti")) { r(b, k, k0, j, "\02" "ic"); break; }
              break;
    case 'l': if (ends(b, k, k0, j, "\04" "ical")) { r(b, k, k0, j, "\02" "ic"); break; }
              if (ends(b, k, k0, j, "\03" "ful")) { r(b, k, k0, j, "\00" ""); break; }
              break;
    case 's': if (ends(b, k, k0, j, "\04" "ness")) { r(b, k, k0, j, "\00" ""); break; }
              break;
} }

/* step4() takes off -ant, -ence etc., in context <c>vcvc<v>. */

void step4(char *b, int *k, int *k0, int *j)
{  switch (b[(*k)-1])
    {  case 'a': if (ends(b, k, k0, j, "\02" "al")) break; return;
       case 'c': if (ends(b, k, k0, j, "\04" "ance")) break;
                 if (ends(b, k, k0, j, "\04" "ence")) break; return;
       case 'e': if (ends(b, k, k0, j, "\02" "er")) break; return;
       case 'i': if (ends(b, k, k0, j, "\02" "ic")) break; return;
       case 'l': if (ends(b, k, k0, j, "\04" "able")) break;
                 if (ends(b, k, k0, j, "\04" "ible")) break; return;
       case 'n': if (ends(b, k, k0, j, "\03" "ant")) break;
                 if (ends(b, k, k0, j, "\05" "ement")) break;
                 if (ends(b, k, k0, j, "\04" "ment")) break;
                 if (ends(b, k, k0, j, "\03" "ent")) break; return;
       case 'o': if (ends(b, k, k0, j, "\03" "ion") && (b[(*j)] == 's' || b[(*j)] == 't')) break;
                 if (ends(b, k, k0, j, "\02" "ou")) break; return;
                 /* takes care of -ous */
       case 's': if (ends(b, k, k0, j, "\03" "ism")) break; return;
       case 't': if (ends(b, k, k0, j, "\03" "ate")) break;
                 if (ends(b, k, k0, j, "\03" "iti")) break; return;
       case 'u': if (ends(b, k, k0, j, "\03" "ous")) break; return;
       case 'v': if (ends(b, k, k0, j, "\03" "ive")) break; return;
       case 'z': if (ends(b, k, k0, j, "\03" "ize")) break; return;
       default: return;
    }
    if (m(b, k, k0, j) > 1) *k = *j;
}

/* step5() removes a final -e if m() > 1, and changes -ll to -l if
   m() > 1. */

void step5(char *b, int *k, int *k0, int *j)
{  *j = *k;
   if (b[*k] == 'e')
   {  int a = m(b, k, k0, j);
      if (a > 1 || (a == 1 && !cvc(b, k, k0, j, (*k)-1))) {
        *k = (*k) - 1;
      }
   }
   if (b[*k] == 'l' && doublec(b, k, k0, j, *k) && m(b, k, k0, j) > 1) {
    *k = (*k) - 1;
  }
}

/* In stem(p,i,j), p is a char pointer, and the string to be stemmed is from
   p[i] to p[j] inclusive. Typically i is zero and j is the offset to the last
   character of a string, (p[j+1] == '\0'). The stemmer adjusts the
   characters p[i] ... p[j] and returns the new end-point of the string, k.
   Stemming never increases word length, so i <= k <= j. To turn the stemmer
   into a module, declare 'stem' as extern, and delete the remainder of this
   file.
*/

int porter_stem(char * p, int i, int j)
{
    int k0 = i, k = j;
    int realJ = 0;
    char *b = p;
    if (k <= k0+1) return k; /*-DEPARTURE-*/

   /* With this line, strings of length 1 or 2 don't go through the
      stemming process, although no mention is made of this in the
      published algorithm. Remove the line to match the published
      algorithm. */

   step1ab(b, &k, &k0, &realJ);
   step1c(b, &k, &k0, &realJ);
   step2(b, &k, &k0, &realJ);
   step3(b, &k, &k0, &realJ);
   step4(b, &k, &k0, &realJ);
   step5(b, &k, &k0, &realJ);
   return k;
}
