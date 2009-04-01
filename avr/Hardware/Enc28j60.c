/*
 * File containing functions and defines needed by the Enc28j60
 * code-module to function properly
 * 
 * Author: Simon Kueppers
 * Email: simon.kueppers@web.de
 * Homepage: http://klinkerstein.m-faq.de
 * 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 Copyright 2008 Simon Kueppers
 * */

#include "Enc28j60.h"
#include <util/delay.h>

/* SPI Opcodes */
#define OPCODE_RCR				0x00	//Read Control Register
#define OPCODE_RBM				0x3A	//Read Buffer Memory
#define OPCODE_WCR				0x40	//Write Control Register
#define OPCODE_WBM				0x7A	//Write Buffer Memory
#define OPCODE_BFS				0x80	//Bit Field Set
#define OPCODE_BFC				0xA0	//Bit Field Clear
#define OPCODE_SRC				0xFF	//System Reset Command (Soft Reset)
/* Banks */
#define BANK0					0
#define BANK1					1
#define BANK2					2
#define BANK3					3

/* Register Adresses
 * Bits 0-5 are the register adress (REGISTER_MASK)
 * Bit 6 identifies whether the register is a MAC/MII register or an ETH register
 * because MAC and MII registers need a dummy byte while reading them (REGISTER_NEEDDUMMY) */
#define REGISTER_MASK			0x1F
#define REGISTER_NEEDDUMMY		0x80

/* Registers in BankX: (means: available in each bank) */
#define BANKX_EIE				0x1B

#define BANKX_EIR				0x1C

#define BANKX_ESTAT				0x1D
#define BANKX_ESTAT_CLKRDY		0
#define BANKX_ESTAT_TXABRT		1
#define BANKX_ESTAT_RXBUSY		2
#define BANKX_ESTAT_LATECOL		4
#define BANKX_ESTAT_BUFER		6
#define BANKX_ESTAT_INT			7

#define BANKX_ECON2				0x1E
#define BANKX_ECON2_VRPS		3
#define BANKX_ECON2_PWRSV		5
#define BANKX_ECON2_PKTDEC		6
#define BANKX_ECON2_AUTOINC		7

#define BANKX_ECON1				0x1F
#define BANKX_ECON1_BSEL0		0
#define BANKX_ECON1_BSEL1		1
#define BANKX_ECON1_RXEN		2
#define BANKX_ECON1_TXRTS		3
#define BANKX_ECON1_CSUMEN		4
#define BANKX_ECON1_DMAST		5
#define BANKX_ECON1_RXRST		6
#define BANKX_ECON1_TXRST		7

/* Registers in Bank0: */
#define BANK0_ERDPTL			(0x00)

#define BANK0_ERDPTH			(0x01)

#define BANK0_EWRPTL			(0x02)

#define BANK0_EWRPTH			(0x03)

#define BANK0_ETXSTL			(0x04)

#define BANK0_ETXSTH			(0x05)

#define BANK0_ETXNDL			(0x06)

#define BANK0_ETXNDH			(0x07)

#define BANK0_ERXSTL			(0x08)

#define BANK0_ERXSTH			(0x09)

#define BANK0_ERXNDL			(0x0A)

#define BANK0_ERXNDH			(0x0B)

#define BANK0_ERXRDPTL			(0x0C)

#define BANK0_ERXRDPTH			(0x0D)

#define BANK0_ERXWRPTL			(0x0E)

#define BANK0_ERXWRPTH			(0x0F)

#define BANK0_EDMASTL			(0x10)

#define BANK0_EDMASTH			(0x11)

#define BANK0_EDMANDL			(0x12)

#define BANK0_EDMANDH			(0x13)

#define BANK0_EDMADSTL			(0x14)

#define BANK0_EDMADSTH			(0x15)

#define BANK0_EDMACSL			(0x16)

#define BANK0_EDMACSH			(0x17)

/* Registers in Bank1: */
#define BANK1_EHT0				(0x00)

#define BANK1_EHT1				(0x01)

#define BANK1_EHT2				(0x02)

#define BANK1_EHT3				(0x03)

#define BANK1_EHT4				(0x04)

#define BANK1_EHT5				(0x05)

#define BANK1_EHT6				(0x06)

#define BANK1_EHT7				(0x07)

#define BANK1_EPMM0				(0x08)

#define BANK1_EPMM1				(0x09)

#define BANK1_EPMM2				(0x0A)

#define BANK1_EPMM3				(0x0B)

#define BANK1_EPMM4				(0x0C)

#define BANK1_EPMM5				(0x0D)

#define BANK1_EPMM6				(0x0E)

#define BANK1_EPMM7				(0x0F)

#define BANK1_EPMCSL			(0x10)

#define BANK1_EPMCSH			(0x11)

#define BANK1_EPMOL				(0x14)

#define BANK1_EPMOH				(0x15)

#define BANK1_ERXFCON			(0x18)

#define BANK1_EPKTCNT			(0x19)

/* Registers in Bank2: */
#define BANK2_MACON1			(0x00|REGISTER_NEEDDUMMY)
#define BANK2_MACON1_MARXEN		0
#define BANK2_MACON1_PASSALL	1
#define BANK2_MACON1_RXPAUS		2
#define BANK2_MACON1_TXPAUS		3

#define BANK2_MACON3			(0x02|REGISTER_NEEDDUMMY)
#define BANK2_MACON3_FULDPX		0
#define BANK2_MACON3_FRMLNEN	1
#define BANK2_MACON3_HFRMEN		2
#define BANK2_MACON3_PHDREN		3
#define BANK2_MACON3_TXCRCEN	4
#define BANK2_MACON3_PADCFG0	5
#define BANK2_MACON3_PADCFG1	6
#define BANK2_MACON3_PADCFG2	7

#define BANK2_MACON4			(0x03|REGISTER_NEEDDUMMY)
#define BANK2_MACON4_NOBKOFF	4
#define BANK2_MACON4_BPEN		5
#define BANK2_MACON4_DEFER		6

#define BANK2_MABBIPG			(0x04|REGISTER_NEEDDUMMY)

#define BANK2_MAIPGL			(0x06|REGISTER_NEEDDUMMY)

#define BANK2_MAIPGH			(0x07|REGISTER_NEEDDUMMY)

#define BANK2_MACLCON1			(0x08|REGISTER_NEEDDUMMY)

#define BANK2_MACLCON2			(0x09|REGISTER_NEEDDUMMY)

#define BANK2_MAMXFLL			(0x0A|REGISTER_NEEDDUMMY)

#define BANK2_MAMXFLH			(0x10|REGISTER_NEEDDUMMY)

#define BANK2_MICMD				(0x12|REGISTER_NEEDDUMMY)
#define BANK2_MICMD_MIIRD		0
#define BANK2_MICMD_MIISCAN		1

#define BANK2_MIREGADR			(0x14|REGISTER_NEEDDUMMY)

#define BANK2_MIWRL				(0x16|REGISTER_NEEDDUMMY)

#define BANK2_MIWRH				(0x17|REGISTER_NEEDDUMMY)

#define BANK2_MIRDL				(0x18|REGISTER_NEEDDUMMY)

#define BANK2_MIRDH				(0x19|REGISTER_NEEDDUMMY)

//Bank3:
#define BANK3_MAADR5			(0x00|REGISTER_NEEDDUMMY)

#define BANK3_MAADR6			(0x01|REGISTER_NEEDDUMMY)

#define BANK3_MAADR3			(0x02|REGISTER_NEEDDUMMY)

#define BANK3_MAADR4			(0x03|REGISTER_NEEDDUMMY)

#define BANK3_MAADR1			(0x04|REGISTER_NEEDDUMMY)

#define BANK3_MAADR2			(0x05|REGISTER_NEEDDUMMY)

#define BANK3_EBSTSD			(0x06)

#define BANK3_EBSTCON			(0x07)

#define BANK3_EBSTCSL			(0x08)

#define BANK3_EBSTCSH			(0x09)

#define BANK3_MISTAT			(0x0A|REGISTER_NEEDDUMMY)
#define BANK3_MISTAT_BUSY		0
#define BANK3_MISTAT_SCAN		1
#define BANK3_MISTAT_NVALID		2

#define BANK3_EREVID			(0x12)

#define BANK3_ECOCON			(0x15)
#define BANK3_ECOCON_COCON0		0
#define BANK3_ECOCON_COCON1		1
#define BANK3_ECOCON_COCON2		2

#define BANK3_EFLOCON			(0x17)

#define BANK3_EPAUSL			(0x18)

#define BANK3_EPAUSH			(0x19)

/* PHY Registers */
#define PHY_PHCON1				0x00
#define PHY_PHCON1_PDPXMD		8
#define PHY_PHCON1_PPWRSV		11
#define PHY_PHCON1_PLOOPBK		14
#define PHY_PHCON1_PRST			15

#define PHY_PHSTAT1				0x01

#define PHY_PHID1				0x02

#define PHY_PHID2				0x03

#define PHY_PHCON2				0x10
#define PHY_PHCON2_HDLDIS		8
#define PHY_PHCON2_JABBER		10
#define PHY_PHCON2_TXDIS		13
#define PHY_PHCON2_FRCLINK		14

#define PHY_PHSTAT2				0x11

#define PHY_PHIE				0x12

#define PHY_PHIR				0x13

#define PHY_PHLCON				0x14
#define PHY_PHLCON_STRCH		1
#define PHY_PHLCON_LFRQ0		2
#define PHY_PHLCON_LFRQ1		3
#define PHY_PHLCON_LBCFG0		4
#define PHY_PHLCON_LBCFG1		5
#define PHY_PHLCON_LBCFG2		6
#define PHY_PHLCON_LBCFG3		7
#define PHY_PHLCON_LACFG0		8
#define PHY_PHLCON_LACFG1		9
#define PHY_PHLCON_LACFG2		10
#define PHY_PHLCON_LACFG3		11

/* Useful macros */
#define NOP()			asm volatile ("nop")
#define SELECT()		do { ENC28J60_PORT &= ~(1<<ENC28J60_CS); NOP(); } while(0)
#define DESELECT()		do { ENC28J60_PORT |= (1<<ENC28J60_CS); NOP(); } while(0)

/* Reads the content of an Enc28j60 register */
ENC28J60_INLINE
uint8_t Enc28j60ReadReg(uint8_t nRegister)
{
	uint8_t nByte;

	SELECT();
	ENC28J60_SPIWRITEBYTE(OPCODE_RCR | (nRegister & REGISTER_MASK));
	if (nRegister & REGISTER_NEEDDUMMY)
		ENC28J60_SPIWRITEBYTE(0);
	nByte = ENC28J60_SPIREADBYTE();
	DESELECT();

	return nByte;
}

/* Writes to an Enc28j60 register */
ENC28J60_INLINE
void Enc28j60WriteReg( uint8_t nRegister,
		uint8_t nData)
{
	SELECT();
	ENC28J60_SPIWRITEBYTE(OPCODE_WCR | (nRegister & REGISTER_MASK));
	ENC28J60_SPIWRITEBYTE(nData);
	DESELECT();
}

/* Sets bits within an Enc28j60 register */
ENC28J60_INLINE
void Enc28j60SetMaskReg(uint8_t nRegister,
		uint8_t nMask)
{
	SELECT();
	ENC28J60_SPIWRITEBYTE(OPCODE_BFS | (nRegister & REGISTER_MASK));
	ENC28J60_SPIWRITEBYTE(nMask);
	DESELECT();
}

/* Clears bits within an Enc28j60 register */
ENC28J60_INLINE
void Enc28j60ClearMaskReg( uint8_t nRegister,
		uint8_t nMask)
{
	SELECT();
	ENC28J60_SPIWRITEBYTE(OPCODE_BFC | (nRegister & REGISTER_MASK));
	ENC28J60_SPIWRITEBYTE(nMask);
	DESELECT();
}

/* Switches the currently selected bank */
ENC28J60_INLINE
void Enc28j60SwitchBank(uint8_t nBank)
{
	/* Use built-in Bit-Set/Bit-Clear functions only while switching bank!
	 * This is important since a read-modify-write cycle could alter unwanted
	 * bits that got set or reset between read and write */
	Enc28j60ClearMaskReg(BANKX_ECON1, (3<<BANKX_ECON1_BSEL0));
	Enc28j60SetMaskReg(BANKX_ECON1, (nBank << BANKX_ECON1_BSEL0));
}

/* Reads a PHY register
 * NOTE: This function changes the currently selected bank */
ENC28J60_INLINE
uint16_t Enc28j60ReadPhy(uint8_t nRegister)
{
	Enc28j60SwitchBank(BANK2);
	Enc28j60WriteReg(BANK2_MIREGADR, nRegister);
	Enc28j60SetMaskReg(BANK2_MICMD, (1<<BANK2_MICMD_MIIRD));
	Enc28j60SwitchBank(BANK3);
	while (Enc28j60ReadReg(BANK3_MISTAT) & (1 <<BANK3_MISTAT_BUSY))
		NOP();
	Enc28j60SwitchBank(BANK2);
	Enc28j60ClearMaskReg(BANK2_MICMD, (1<<BANK2_MICMD_MIIRD));

	return ((uint16_t) Enc28j60ReadReg(BANK2_MIRDL) << 0)
	| ((uint16_t) Enc28j60ReadReg(BANK2_MIRDH) << 8);
}

/* Writes a PHY register
 * NOTE: This function changes the currently selected bank */
ENC28J60_INLINE
void Enc28j60WritePhy( uint8_t nRegister,
		uint16_t nData)
{
	Enc28j60SwitchBank(BANK2);
	Enc28j60WriteReg(BANK2_MIREGADR, nRegister);
	Enc28j60WriteReg(BANK2_MIWRL, (nData >> 0));
	Enc28j60WriteReg(BANK2_MIWRH, (nData >> 8));
	Enc28j60SwitchBank(BANK3);
	while (Enc28j60ReadReg(BANK3_MISTAT) & (1 <<BANK3_MISTAT_BUSY))
	{
		NOP();
	}
}

/* Resets Enc28j60 */
ENC28J60_INLINE
void Enc28j60Reset()
{
	SELECT();
	ENC28J60_SPIWRITEBYTE(OPCODE_SRC);
	DESELECT();

	_delay_ms(10);
}

/* Resets Enc28j60's PHY */
ENC28J60_INLINE
void Enc28j60ResetPhy()
{
	Enc28j60WritePhy(PHY_PHCON1, (1<<PHY_PHCON1_PRST));

	while (Enc28j60ReadPhy(PHY_PHCON1) & (1<<PHY_PHCON1_PRST))
	{
		NOP();
	}
}

void Enc28j60Init()
{
	/* Set Pin-Directions  */
	ENC28J60_DDR |= (1<<ENC28J60_MOSI)|(1<<ENC28J60_CS)|(1<<ENC28J60_SCK);
	ENC28J60_DDR &= ~((1<<ENC28J60_INT)|(1<<ENC28J60_MISO));

	DESELECT();

	/* Wait for the Oscillation Startup Timer */
	while (!(Enc28j60ReadReg(BANKX_ESTAT) & (1<<BANKX_ESTAT_CLKRDY)))
		NOP();

	/* Issue a system and PHY reset */
	Enc28j60Reset();
	Enc28j60ResetPhy();

	/* Do bank 0 initializations */
	Enc28j60SwitchBank(BANK0);

	/* Initialize Receive Buffer */
	Enc28j60WriteReg(BANK0_ERXSTL, (uint8_t) (ENC28J60_RXSTART >> 0));
	Enc28j60WriteReg(BANK0_ERXSTH, (uint8_t) (ENC28J60_RXSTART >> 8));
	Enc28j60WriteReg(BANK0_ERXNDL, (uint8_t) (ENC28J60_RXEND >> 0));
	Enc28j60WriteReg(BANK0_ERXNDH, (uint8_t) (ENC28J60_RXEND >> 8));
	/* Receiver Pointer */
	Enc28j60WriteReg(BANK0_ERDPTL, (uint8_t) (ENC28J60_RXSTART >> 0));
	Enc28j60WriteReg(BANK0_ERDPTH, (uint8_t) (ENC28J60_RXSTART >> 8));
	/* Errata Workaround: ERXRDPT should not be programmed with an even address
	 * so we choose RXSTART-1 which is equal to RXEND */ 
	Enc28j60WriteReg(BANK0_ERXRDPTL, (uint8_t) (ENC28J60_RXEND >> 0));
	Enc28j60WriteReg(BANK0_ERXRDPTH, (uint8_t) (ENC28J60_RXEND >> 8));
	/* and Transmit Pointer */
	Enc28j60WriteReg(BANK0_ETXSTL, (uint8_t) (ENC28J60_TXSTART >> 0));
	Enc28j60WriteReg(BANK0_ETXSTH, (uint8_t) (ENC28J60_TXSTART >> 8));

	/* Bank 2 initializations */
	Enc28j60SwitchBank(BANK2);

	/* MAC RX Enable */
	Enc28j60WriteReg(BANK2_MACON1, (1<<BANK2_MACON1_MARXEN));
	/* Append CRC, Padding bytes and check length-consistency of frames */
	Enc28j60SetMaskReg(BANK2_MACON3, (1<<BANK2_MACON3_TXCRCEN)|(1<<BANK2_MACON3_PADCFG0)|(1<<BANK2_MACON3_FRMLNEN));
	/* "For IEEE802.3 compliance" */
	Enc28j60SetMaskReg(BANK2_MACON4, (1<<BANK2_MACON4_DEFER));
	/* Set the maximum frame-length to prevent host-controller from buffer overflows
	 * frame-length + CRC (CRC will not occupy any host buffer-space) */
	Enc28j60WriteReg(BANK2_MAMXFLL, (uint8_t) ((ENC28J60_MAXFRAME + 4) >> 0));
	Enc28j60WriteReg(BANK2_MAMXFLH, (uint8_t) ((ENC28J60_MAXFRAME + 4) >> 8));
	/* non-back to back-Inter-Packet-Delay-Gap. (datasheet recommendation) */
	Enc28j60WriteReg(BANK2_MAIPGL, 0x12);
	/* datasheet recommendation */
	Enc28j60WriteReg(BANK2_MAIPGH, 0x0C);
	/* back to back-Inter-Packet-Delay-Gap. (datasheet recommendation) */
	Enc28j60WriteReg(BANK2_MABBIPG, 0x12);

	/* Bank 3 initializations */
	Enc28j60SwitchBank(BANK3);

	/* Initialize MAC-adress */
	Enc28j60WriteReg(BANK3_MAADR1, ENC28J60_MAC1);
	Enc28j60WriteReg(BANK3_MAADR2, ENC28J60_MAC2);
	Enc28j60WriteReg(BANK3_MAADR3, ENC28J60_MAC3);
	Enc28j60WriteReg(BANK3_MAADR4, ENC28J60_MAC4);
	Enc28j60WriteReg(BANK3_MAADR5, ENC28J60_MAC5);
	Enc28j60WriteReg(BANK3_MAADR6, ENC28J60_MAC6);

	/* local loopback disable */
	Enc28j60WritePhy(PHY_PHCON2, (1<<PHY_PHCON2_HDLDIS));
	/* Set LED Configuration:
	 * Errata Workaround: Highest nibble should be '0011' */
	Enc28j60WritePhy(PHY_PHLCON, (ENC28J60_LEDB<<PHY_PHLCON_LBCFG0)|(ENC28J60_LEDA<<PHY_PHLCON_LACFG0)|(1<<PHY_PHLCON_STRCH)|0x3000);

	/* Errata Workaround: Because the LED-Polarity detection circuit does not function properly,
	 * we clear the FullDuplex Bit ourself. MACON3.FULDPX is cleared anyway. */
	Enc28j60WritePhy(PHY_PHCON1, 0x0000);

	/* Enable Packet Reception */
	Enc28j60SetMaskReg(BANKX_ECON1, (1<<BANKX_ECON1_RXEN));
}

uint16_t Enc28j60Receive(uint8_t* pBuffer)
{
	uint16_t nBytes;
	uint16_t nNextPacket;

	/* Check for at least 1 waiting packet in the buffer */
	Enc28j60SwitchBank(BANK1);
	if (Enc28j60ReadReg(BANK1_EPKTCNT) == 0)
		return 0;

	SELECT();
	ENC28J60_SPIWRITEBYTE(OPCODE_RBM);

	/* Read Next Packet Pointer */
	nNextPacket = ((uint16_t) ENC28J60_SPIREADBYTE() << 0);
	nNextPacket |= ((uint16_t) ENC28J60_SPIREADBYTE() << 8);

	/* Read Received Bytecount (minus 4 to remove CRC) */
	nBytes = ((uint16_t) ENC28J60_SPIREADBYTE() << 0);
	nBytes |= ((uint16_t) ENC28J60_SPIREADBYTE() << 8);
	nBytes -= 4;

	/* 2 Bytes Status bits (unused) */
	ENC28J60_SPIREADBYTE();
	ENC28J60_SPIREADBYTE();

	/* Frame-Data */
	if (nBytes <= ENC28J60_MAXFRAME)
		ENC28J60_SPIREADCHUNK(pBuffer, nBytes);

	DESELECT();

	Enc28j60SwitchBank(BANK0);
	/* Set RX Read-Pointer and SPI Read-Pointer to next-frame-address */
	Enc28j60WriteReg(BANK0_ERDPTL , (uint8_t) (nNextPacket >> 0));
	Enc28j60WriteReg(BANK0_ERDPTH , (uint8_t) (nNextPacket >> 8));

	/* Errata Workaround: ERXRDPT should never be programmed with an even value
	 * Because the NextPacket will always point to an even value, we can subtract 1 from it */
	nNextPacket -= 1;
	if (nNextPacket == ( ((uint16_t)ENC28J60_RXSTART) - 1 ))
	{
		/* Underflow occured while subtracting 1? Use RXEND then. The ENC28J60 logic will
		 * set the pointer to the next even address, which is RXSTART. */
		nNextPacket = ENC28J60_RXEND;
	}

	Enc28j60WriteReg(BANK0_ERXRDPTL, (nNextPacket >> 0));
	Enc28j60WriteReg(BANK0_ERXRDPTH, (nNextPacket >> 8));

	/* And decrement PacketCounter */
	Enc28j60SetMaskReg(BANKX_ECON2 , (1<<BANKX_ECON2_PKTDEC));

	return nBytes;
}

void Enc28j60CopyPacket(uint8_t* pBuffer, uint16_t nBytes)
{
	uint16_t TxEnd = ENC28J60_TXSTART + nBytes;
	uint8_t i = 200;
	
	/* Wait for a previously buffered frame to be sent out completely
	 * Errata Workaround: TXRTS could remain set indefinitely.
	 * This workaround will wait for TXRTS to be cleared within a maximum of 100ms */	
	while (i--)
	{
		if (!(Enc28j60ReadReg(BANKX_ECON1) & (1<<BANKX_ECON1_TXRTS)))
			break;
		_delay_ms(0.5);
	}

	Enc28j60SwitchBank(BANK0);

	Enc28j60WriteReg(BANK0_EWRPTL, (uint8_t) (ENC28J60_TXSTART >> 0));
	Enc28j60WriteReg(BANK0_EWRPTH, (uint8_t) (ENC28J60_TXSTART >> 8));
	Enc28j60WriteReg(BANK0_ETXNDL, (uint8_t) (TxEnd >> 0));
	Enc28j60WriteReg(BANK0_ETXNDH, (uint8_t) (TxEnd >> 8));	
	
	SELECT();
	ENC28J60_SPIWRITEBYTE(OPCODE_WBM);
	/* Per packet control byte */
	ENC28J60_SPIWRITEBYTE(0);
	ENC28J60_SPIWRITECHUNK(pBuffer, nBytes);
	DESELECT();
}

void Enc28j60Send()
{
	/* Errata Workaround: Reset TX Logic */
	Enc28j60SetMaskReg(BANKX_ECON1, (1<<BANKX_ECON1_TXRST));
	Enc28j60ClearMaskReg(BANKX_ECON1, (1<<BANKX_ECON1_TXRST));

	/* Start transmission */
	Enc28j60SetMaskReg(BANKX_ECON1, (1<<BANKX_ECON1_TXRTS));
}

void Enc28j60SetClockPrescaler(uint8_t nPrescaler)
{
	Enc28j60SwitchBank(BANK3);

	if (nPrescaler == 1)
		Enc28j60WriteReg(BANK3_ECOCON, (1<<BANK3_ECOCON_COCON0));
	else if (nPrescaler == 2)
		Enc28j60WriteReg(BANK3_ECOCON, (2<<BANK3_ECOCON_COCON0));
	else if (nPrescaler == 3)
		Enc28j60WriteReg(BANK3_ECOCON, (3<<BANK3_ECOCON_COCON0));
	else if (nPrescaler == 4)
		Enc28j60WriteReg(BANK3_ECOCON, (4<<BANK3_ECOCON_COCON0));
	else if (nPrescaler == 8)
		Enc28j60WriteReg(BANK3_ECOCON, (5<<BANK3_ECOCON_COCON0));
}

uint16_t Enc28j60ChecksumTx(uint16_t Offset, uint16_t Length)
{
	/* +1 to skip Per-packet-control-byte */
	uint16_t Start = ENC28J60_TXSTART + Offset + 1;
	uint16_t End = Start + Length - 1;
	
	Enc28j60SwitchBank(BANK0);
	Enc28j60WriteReg(BANK0_EDMASTL, (uint8_t) (Start >> 0));
	Enc28j60WriteReg(BANK0_EDMASTH, (uint8_t) (Start >> 8));
	Enc28j60WriteReg(BANK0_EDMANDL, (uint8_t) (End >> 0));
	Enc28j60WriteReg(BANK0_EDMANDH, (uint8_t) (End >> 8));
	Enc28j60SetMaskReg(BANKX_ECON1, (1<<BANKX_ECON1_CSUMEN) | (1<<BANKX_ECON1_DMAST));
	
	while(Enc28j60ReadReg(BANKX_ECON1) & (1<<BANKX_ECON1_DMAST))
		NOP();
	
	return ((uint16_t) Enc28j60ReadReg(BANK0_EDMACSH) << 8) | ((uint16_t) Enc28j60ReadReg(BANK0_EDMACSL) << 0);
}

void Enc28j60CopyChecksum(uint16_t Offset, uint16_t Checksum)
{
	/* +1 to skip Per-packet-control-byte */
	uint16_t WrPtr = ENC28J60_TXSTART + Offset + 1;

	Enc28j60SwitchBank(BANK0);

	Enc28j60WriteReg(BANK0_EWRPTL, (uint8_t) (WrPtr >> 0));
	Enc28j60WriteReg(BANK0_EWRPTH, (uint8_t) (WrPtr >> 8));

	SELECT();
	ENC28J60_SPIWRITEBYTE(OPCODE_WBM);
	ENC28J60_SPIWRITECHUNK((uint8_t*) &Checksum, sizeof(Checksum));
	DESELECT();
}