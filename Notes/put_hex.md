## Printing hex: put_hex

**Why it's needed:** Memory addresses, port numbers, and bit patterns are far
easier to read in hex than decimal. Every hex digit is exactly 4 bits, so
0xFF = 1111 1111, and a 32-bit value is always 8 hex digits.

**How it works:** same loop as put_int, with 16 instead of 10.
1. `n % 16` gives the last hex digit, a value 0-15.
2. Convert it to a character:
   - digit < 10: `'0' + digit` gives '0'-'9'
   - digit >= 10: `'A' - 10 + digit` gives 'A'-'F'
3. `n /= 16` removes that digit. Repeat until n is 0.
4. Digits come out reversed, so store them in `char buffer[8]` and print backwards.

**Design choice: fixed width.** I always print 8 digits (0x000B8000), padding
with leading zeros: `8 - count` zeros first, then the digits. This makes
addresses line up, and it handles n == 0 for free, because count stays 0 and
the padding loop prints 8 zeros. No special case needed.

**Why unsigned:** addresses and bit patterns have no sign. With plain `int`,
0xFFFFFFFF is negative, so `n > 0` fails and nothing prints. Also, `>>` on
negative signed values is implementation-defined, while on unsigned it always
fills with 0s.

**Bugs I hit:**
- Special-casing n == 0 printed the zeros twice, because the padding loop
  already covered it. Check whether a general path already handles the edge case
  before adding a special one.
- Using `int` for the parameter breaks on values with the top bit set, since
  they read as negative.