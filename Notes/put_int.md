## Printing numbers: put_int

**Why it's needed:** The screen only understands characters. The number 42 is one
byte (00101010), but the text "42" is two characters, '4' (52) and '2' (50).
Something has to convert numbers into digit characters, and that is put_int.

**How it works:**
1. `n % 10` gives the last digit.
2. `digit + '0'` turns it into its character (3 + '0' = '3').
3. `n / 10` removes that digit. Repeat until n is 0.
4. Digits come out reversed, so store them in `char buffer[12]` and print backwards.

**Edge cases:** n == 0 (loop never runs, handle separately), negative numbers
(print '-' first, then the magnitude).

**Bugs I hit:**
- `char *buffer` is a pointer, not storage. Use `char buffer[12]`.
- Storing `digit` instead of `digit + '0'` stores the number, not the character.
- `strlen` needs a '\0' terminator, so count digits manually instead. (i < strlen(buffer) doesn't work in the for loop)

## Signed overflow and unsigned wraparound

- INT_MIN (-2147483648) has no positive counterpart in a 32-bit int, so `-n`
  overflows. Signed overflow is undefined behavior in C.
- Fix: do the digit work in `unsigned int`. Unsigned arithmetic wraps by definition
  (modulo 2^32), like a car odometer: 0 - 1 = 4294967295.
- `magnitude = 0u - (unsigned int)n;` gives the correct positive value for every
  negative n, including INT_MIN.
- Casting signed to unsigned keeps the same bits and only changes how they are read.
  -2147483648 is 0x80000000, which as unsigned means 2147483648.

## C syntax reminders

- `'1'` is a char (one character), `"1"` is a string (char array ending in '\0').
  put_char takes single quotes, put_string takes double quotes.
- `0u` is a literal with the unsigned suffix. Suffixes only work on number
  literals (`0u`, `5UL`). For a variable, use a cast: `(unsigned int)n`.
- `u` is not a reserved keyword. It only has special meaning as a suffix directly after digits. Elsewhere it is an ordinary variable name.