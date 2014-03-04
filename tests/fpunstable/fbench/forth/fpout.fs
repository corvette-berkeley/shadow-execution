\ This is a heavily-modified version of:

\     FPOUT.F   version 1.7

\ Intended solely for distribution with the Forth implementation
\ of Fbench.  This is:
\
\     A Floating Point Output Words package for ANS FORTH-94
\     compliant systems.
\
\ and as:
\
\     This code is public domain.  Use at your own risk.
\
\ we can use it as we wish, and have been warned.  I have modified
\ this code to increase ANS Forth compatibility and to comment out
\ and remove code which might cause problems in other Forth
\ implementations.  If you wish to use this code in another application,
\ you should not start with this code, bur rather the original, which
\ can be downloaded from:
\
\	ftp://ftp.taygeta.com/pub/Forth/Applications/ANS/fpout17.f
\
\ The following is the code, as modified:
\
\ **********************************************************************
\
\ Main words:
\
\  Compact   Formatted   String
\  -------   ---------   ------
\    FS.       FS.R       (FS.)    Scientific
\    FE.       FE.R       (FE.)    Engineering
\    F.        z_F.R        (z_F.)     Fixed-point
\    G.        G.R        (G.)     General
\
\ Notes:
\
\ 1. An ambiguous condition exists if the value of BASE is
\    not (decimal) ten or if the character string
\    representation exceeds the size of the pictured numeric
\    output string buffer.
\
\ 2. Works on either separate floating-point stack or common
\    stack forth models.
\
\ 3. Opinions vary as to the correct display for F. FS. FE.
\    One useful interpretation of the Forth-94 Standard has
\    been chosen here.
\
\ 4. Display words that specify the number of places after
\    the decimal point may use the value -1 to force compact
\    mode.  Compact mode displays all significant digits
\    with redundant zeros and signs removed.  FS. F. FE. G.
\    are displayed in compact mode.
\
\ 5. Ideally, all but the main words should be headerless
\    or else placed in a hidden vocabulary.  Code size may
\    be reduced by eliminating features not needed e.g. if
\    REPRESENT always returns flag2=true or if CHARS is a
\    no-operation.
\
\ 6. If your REPRESENT does not return an exponent of +1
\    in the case of 0.0E it will result in multiple leading
\    zeros being displayed.  This is a bug** and you should
\    have it fixed!  In the meantime, a work-around in the
\    form of an alternate (F1) is supplied.
\
\ 7. If your REPRESENT does not blank fill the remainder
\    of the buffer when NAN or other graphic string is
\    returned then unspecified trailing characters will be
\    displayed.  Again, this is a bug** in your REPRESENT.
\    Unfortunately no work-around is possible.
\
\    ** FORTH-94 is silent on this point.
\
\ History:
\
\ v1.1  15-Sep-02  es   First release
\ v1.2  14-Jan-05  es   Display decimal point if places = 0.
\                       Added (G.) G.
\ v1.3  16-Jan-05  es   Implemented compact display
\ v1.4  20-Jan-05  es   Handle NAN INF etc
\ v1.5  30-Jan-05  es   Implemented rounding. Changed (G.)
\                       G. upper limit.
\ v1.6  01-Feb-05  es   Changed (G.) to use decimal places
\                       parameter. Added G.R
\ v1.7  21-Feb-05  es   Fixed rounding error which occured
\                       under certain conditions.

DECIMAL

15 VALUE z_PRECISION	\ We use our own because some Forths don't
						\ let us reset this in a portable manner

CREATE FBUF             \ REPRESENT buffer
20 CHARS ALLOT          \ set this to your maximum PRECISION

0 VALUE BS#             \ buffer size
0 VALUE EX#             \ exponent
0 VALUE SN#             \ sign
0 VALUE EF#             \ exponent factor  1=FS. 3=FE.
0 VALUE PL#             \ +n  places right of decimal point
                        \ -1  compact display

\ Apply exponent factor
: (F0)  ( exp -- offset exp' )
  S>D EF# FM/MOD EF# * ;

\ float to ascii
: (F1)  ( F: r -- ) ( places -- c-addr u flag )
  TO PL#  z_PRECISION TO BS#
  FDUP FBUF BS# REPRESENT NIP AND ( exp=0 on err )
  PL# 0< IF  DROP z_PRECISION  ELSE  EF# 0> IF
  1- (F0) DROP 1+  THEN  PL# +  THEN  DUP >R ( size )
  z_PRECISION MIN 1 MAX TO BS#  FBUF BS# REPRESENT  DUP
  R> 0= AND ( no err & size=0 ) FBUF C@ [CHAR] 4 > AND
  IF ( round ) [CHAR] 1 FBUF C!  ROT 1+ ROT ROT  THEN
  >R  TO SN#  1- TO EX#  FBUF BS# -TRAILING  R> <# ;

\ insert string
: (F3)  ( c-addr u -- )
  0 MAX  BEGIN  DUP  WHILE  1- 2DUP CHARS + C@ HOLD
  REPEAT 2DROP ;

\ insert '0's
: (F4)  ( n -- )
  0 MAX 0 ?DO [CHAR] 0 HOLD LOOP ;

\ insert sign
: (F5)  ( -- )
  SN# SIGN  0 0 #> ;

\ trim trailing '0's
: (F6)  ( c-addr u1 -- c-addr u2 )
  PL# 0< IF
    BEGIN  DUP WHILE  1- 2DUP CHARS +
    C@ [CHAR] 0 -  UNTIL  1+  THEN
  THEN ;

: (F7)  ( n -- n n | n pl# )
   PL# 0< IF  DUP  ELSE  PL#  THEN ;

\ insert fraction string n places right of dec. point
: (F8)  ( c-addr u n -- )
  >R (F6)  R@ +
  (F7) OVER - (F4)     \ trailing 0's
  (F7) MIN  R@ - (F3)  \ fraction
  R> (F7) MIN (F4)     \ leading 0's
  [CHAR] . HOLD ;

\ split string into integer/fraction parts at n and insert
: (F9)  ( c-addr u n -- )
  >R 2DUP R@ MIN 2SWAP R> /STRING  0 (F8) (F3) ;

\ display c-addr u right-justified in field width u2
: (FB)  ( c-addr u u2 -- )
  OVER - SPACES TYPE ;

\ These are the main words

\ Convert real number r to string c-addr u in fixed-point
\ notation with n places right of the decimal point.

: (z_F.)  ( F: r -- ) ( n -- c-addr u )
  0 TO EF#  (F1) IF
    EX# 1+ DUP z_PRECISION > IF
       FBUF 0 ( dummy ) 0 (F8)
       PRECISION - (F4) (F3)
    ELSE
      DUP 0> IF
        (F9)
      ELSE
        ABS (F8) 1 (F4)
      THEN
    THEN (F5)
  THEN ;

\ Display real number r in fixed-point notation right-
\ justified in a field width u with n places right of the
\ decimal point.

: z_F.R   ( F: r -- ) ( n u -- )
  >R (z_F.) R> (FB) ;

