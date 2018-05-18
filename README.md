CRC-7 Generator polynomial G(x) = x⁷ + x³ + 1  
According to eMMC 5.1 JEDEC standard JESD84-B51.

I wrote this mainly to compare output from logic analyzer with the CRC-7
checksum both "bus" and "serial" wise.

Here's an example of usage:

<pre>
$ echo -n "test" | ./crc7mmc 
Data length: 0x04 bytes
CRC-7 (MMC) dat[7]: 0x00
CRC-7 (MMC) dat[6]: 0x77
CRC-7 (MMC) dat[5]: 0x77
CRC-7 (MMC) dat[4]: 0x53
CRC-7 (MMC) dat[3]: 0x00
CRC-7 (MMC) dat[2]: 0x65
CRC-7 (MMC) dat[1]: 0x12
CRC-7 (MMC) dat[0]: 0x36
CRC-7 (MMC) LA bus output: 0x2e a6 ce 00 a6 ce 2e 
CRC-7 (MMC) serial: 0x0a
</pre>

Compile with:  
gcc -Wpedantic crc7mmc.c -o crc7mmc

