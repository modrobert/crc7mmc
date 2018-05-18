/*
    CRC-7 Generator polynomial G(x) = x⁷ + x³ + 1
    According to eMMC 5.1 JEDEC standard JESD84-B51
    Copyright (C) 2017 Robert V. <modrobert@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// Compile with: gcc -Wpedantic crc7mmc.c -o crc7mmc

#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char *argv[])
{
 unsigned char buf[1];
 unsigned char buscrc[8][7];
 unsigned char serialcrc[7];
 int i, j;
 uint8_t crc7 = 0, crcx = 0, crcy = 0;
 uint8_t ch, inv, crctemp;
 unsigned long int len = 0;

 /* clear buffers for each bus line */
 for (i = 0; i < 8; i++)
 {
  for (j = 0; j < 7; j++) buscrc[i][j] = 0;
 }
 for (j = 0; j < 7; j++) serialcrc[j] = 0;

 while (read(STDIN_FILENO, buf, sizeof(buf)) > 0)
 {
  len++;
  ch = buf[0];

  /* do the serial CRC7 round */
  for (j = 0; j < 8; j++)
  {
   /* MSB to LSB order of ch */
   inv = ((ch >> (7 - j)) & 1) ^ serialcrc[6]; // XOR required?
   serialcrc[6] = serialcrc[5];
   serialcrc[5] = serialcrc[4];
   serialcrc[4] = serialcrc[3];
   serialcrc[3] = serialcrc[2] ^ inv;
   serialcrc[2] = serialcrc[1];
   serialcrc[1] = serialcrc[0];
   serialcrc[0] = inv;
  }

  /* go through all "bits" on the bus and calculate CRC7 */
  for (i = 0; i < 8; i++)
  {
   /* MSB to LSB order of ch */
   inv = ((ch >> (7 - i)) & 1) ^ buscrc[i][6]; // XOR required?
   buscrc[i][6] = buscrc[i][5];
   buscrc[i][5] = buscrc[i][4];
   buscrc[i][4] = buscrc[i][3];
   buscrc[i][3] = buscrc[i][2] ^ inv;
   buscrc[i][2] = buscrc[i][1];
   buscrc[i][1] = buscrc[i][0];
   buscrc[i][0] = inv;
  }
 } // end while

 printf("Data length: 0x%02lx bytes\n", len);

 /* print CRC7 for each data signal of the 8 bit bus */
 for (i = 0; i < 8 ; i++)
 {
  for (j = 0; j < 7; j++)
  {
   if (buscrc[i][j] == 1) // bits represented as bytes
   {
    crcx |= 1 << j;
   }
  }
  /* for MMC think: [CRC7 7:1] + [end bit (always '1') 0:0] */
  if (argc > 1 && strcmp(argv[1], "-8") == 0) // lets make it optional
  {
   crctemp = (crcx << 1) | 0x1;
   printf("CRC-7 (MMC) dat[%d]: 0x%02x\n", 7 - i, crctemp); 
  }
  else
  {
   printf("CRC-7 (MMC) dat[%d]: 0x%02x\n", 7 - i, crcx);
  }
  crcx = 0;
 }

 /* print CRC7 as 7-8 bytes on the 8 bit bus */
 printf("CRC-7 (MMC) LA bus output: 0x");
 for (j = 0; j < 7; j++)
 {
  for (i = 0; i < 8; i++)
  {
   /* CRC7 in this case MSB to LSB according to eMMC 5.1 */
   if (buscrc[i][6 - j] == 1) // bits represented as bytes
   {
    crcy |= 1 << i;
   }
  }
  printf("%02x ", crcy);
  crcy = 0;
 }
 /* for MMC think: [CRC7 7:1] + [end bit (always '1') 0:0] */
 if (argc > 1 && strcmp(argv[1], "-8") == 0) // lets make it optional
 {
  crcy = 0xff;
  printf("%02x", crcy);
 }
 printf("\n");

 /* print CRC7 as serial data */
 for (j = 0; j < 7; j++)
 {
  if (serialcrc[j] == 1) crc7 |= 1 << (j);
 }
 /* for MMC think: [CRC7 7:1] + [end bit (always '1') 0:0] */
 if (argc > 1 && strcmp(argv[1], "-8") == 0) // lets make it optional
  crc7 = (crc7 << 1) | 0x1;
 printf("CRC-7 (MMC) serial: 0x%02x\n", crc7);

 return 0;
}

