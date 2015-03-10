

#define ZERO					 0x00
#define CSC_KEYB_TIMER           0xC6  /*IO address on ELAN. This will change in 


/* Values to be used in conjunction with CSC_KEYB_TIMER */
#define KEYB_TIMER_OFF           0x00  /* Disable Timer */

#define KEYB_TIMER_25MS          25/* Elan Value 0x04 */ /* 25.4 ms +/- 0.5ms */
#define KEYB_TIMER_33MS          33/* Elan Value 0x06 */ /* 33.2 ms +/- 0.5ms */
#define KEYB_TIMER_37MS          37/* Elan Value 0x07 */ /* 37.2 ms +/- 0.5ms */
#define KEYB_TIMER_41MS          41/* Elan Value 0x08 */ /* 41.1 ms +/- 0.5ms */
#define KEYB_TIMER_45MS          45/* Elan Value 0x09 */ /* 45.0 ms +/- 0.5ms */
#define KEYB_TIMER_49MS          49/* Elan Value 0x0A */ /* 48.9 ms +/- 0.5ms */
#define KEYB_TIMER_53MS          53/* Elan Value 0x0B */ /* 52.8 ms +/- 0.5ms */
#define KEYB_TIMER_57MS          57/* Elan Value 0x0C */ /* 56.7 ms +/- 0.5ms */
#define KEYB_TIMER_61MS          61/* Elan Value 0x0D */ /* 60.6 ms +/- 0.5ms */
#define KEYB_TIMER_68MS          68/* Elan Value 0x0F */ /* 68.4 ms +/- 0.5ms */
#define KEYB_TIMER_72MS          72/* Elan Value 0x10 */ /* 72.3 ms +/- 0.5ms */
#define KEYB_TIMER_76MS          76/* Elan Value 0x11 */ /* 76.3 ms +/- 0.5ms */
#define KEYB_TIMER_84MS          84/* Elan Value 0x13 */ /* 84.1 ms +/- 0.5ms */
#define KEYB_TIMER_92MS          92/* Elan Value 0x15 */ /* 91.9 ms +/- 0.5ms */
#define KEYB_TIMER_100MS        100/* Elan Value 0x17 */ /* 99.7 ms +/- 0.5ms */
#define KEYB_TIMER_111MS        111/* Elan Value 0x1A */ /* 111.4 ms +/- 0.5ms */
#define KEYB_TIMER_123MS        123/* Elan Value 0x1D */ /* 123.2 ms +/- 0.5ms */
#define KEYB_TIMER_143MS        143/* Elan Value 0x22 */ /* 142.7 ms +/- 0.5ms */
#define KEYB_TIMER_166MS        166/* Elan Value 0x28 */ /* 166.2 ms +/- 0.5ms */
#define KEYB_TIMER_201MS        201/* Elan Value 0x31 */ /* 201.4 ms +/- 0.5ms */
#define KEYB_TIMER_248MS        248/* Elan Value 0x3D */ /* 248.3 ms +/- 0.5ms */
#define KEYB_TIMER_334MS        334/* Elan Value 0x53 */ /* 334.3 ms +/- 0.5ms */
#define KEYB_TIMER_499MS        499/* Elan Value 0x7D */ /* 498.5 ms +/- 0.5ms */
#define KEYB_TIMER_749MS        749/* Elan Value 0xBD */ /* 748.8 ms +/- 0.5ms */
#define KEYB_TIMER_999MS        999/* Elan Value 0xFD */ /* 999.0 ms +/- 0.5ms */

#define INTERVAL_DEBOUNCE         ZERO

#define INTERVAL_TYPEMATIC_DELAY  ZERO

#define INTERVAL_KEY_REPEAT		  ZERO

/* The last bit 0x01 is used for pullup */
/* When the COL is low the ROW is read 
and that shows a Key value. HI = no press
LO = press*/


