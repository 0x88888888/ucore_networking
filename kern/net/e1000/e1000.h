#ifndef __KERN_NET_E1000_H__
#define __KERN_NET_E1000_H__

#include <defs.h>
#include<pmm.h>
#include<pci.h>
#include<string.h>

#define TXDESC_MAXSIZE 64
#define RXDESC_MAXSIZE 128
#define PKT_MAX_SIZE 4096
extern volatile uint32_t *mmio_e1000;
//struct e1000_tx_desc txdesc_array[TXDESC_MAXSIZE] __attribute__ ((aligned(16)));

int attach_fn(struct pci_func *pcif);
int e1000_transmit(uint8_t *pkt, uint32_t length);
int e1000_recv(uint8_t *data, size_t len);
void e1000_mac(unsigned char* buf);

#define E1000_EERD	0x00014/4  /* EEPROM Read - RW */

#define E1000_STATUS   0x00008/4  /* Device Status - RO */
#define E1000_TDBAL    0x03800/4  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    0x03804 / 4  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    0x03808/4  /* TX Descriptor Length - RW */
#define E1000_TDH      0x03810/4  /* TX Descriptor Head - RW */
#define E1000_TDT      0x03818/4  /* TX Descripotr Tail - RW */
#define E1000_TCTL     0x00400/4  /* TX Control - RW */
#define E1000_TIPG     0x00410/4  /* TX Inter-packet gap -RW */

#define E1000_RCTL     0x00100 / 4  /* RX Control - RW */
#define E1000_RA       0x05400 / 4  /* Receive Address - RW Array */
#define E1000_RAL      0x05400 / 4  /* Receive Address Low - RW */
#define E1000_RAH      0x05404 / 4  /* Receive Address HIGH - RW */
#define E1000_RDBAL    0x02800 / 4  /* RX Descriptor Base Address Low - RW */
#define E1000_RDBAH    0x02804 / 4  /* RX Descriptor Base Address High - RW */
#define E1000_RDLEN    0x02808 / 4  /* RX Descriptor Length - RW */
#define E1000_RDH      0x02810 / 4  /* RX Descriptor Head - RW */
#define E1000_RDT      0x02818 / 4  /* RX Descriptor Tail - RW */


//#define E1000_TCTL_EN     0x00000002    /* enable tx */
//#define E1000_TCTL_PSP    0x00000008    /* pad short packets */
//#define E1000_TCTL_COLD   0x003ff000    /* collision distance */
//#define E1000_TCTL_CT     0x00000100    /* collision threshold */

//#define E1000_TIPG_IPGT	0
//#define E1000_TIPG_IPGR1	10
//#define E1000_TIPG_IPGR2	20

/* TX Inter-packet gap bit definitions */
#define E1000_TIPG_IPGT      0x000003FF
#define E1000_TIPG_IPGR1     0x000FFA00
#define E1000_TIPG_IPGR2     0x3FF00000

#define E1000_TCTL_EN     0x00000002    /* enable tx */
#define E1000_TCTL_PSP    0x00000008    /* pad short packets */
#define E1000_TCTL_CT     0x00000ff0    /* collision threshold */
#define E1000_TCTL_COLD   0x003ff000    /* collision distance */

#define E1000_TXD_STAT_DD    0x00000001 /* Descriptor Done */
#define E1000_TXD_CMD_EOP    0x01000000 /* End of Packet */
#define E1000_TXD_CMD_RS     0x08000000 /* Report Status */

//#define E1000_RAL0           0x05400   /* Receive Address Low  - R/W */
//#define E1000_RAH0           0x05404   /* Receive Adreess High - R/W */
#define E1000_RXD_STAT_DD       0x01    /* Descriptor Done */
#define E1000_RXD_STAT_EOP      0x02    /* End of Packet */

#define E1000_EEPROM_RW_REG_DATA   16   /* Offset to data in EEPROM read/write registers */
#define E1000_EEPROM_RW_REG_DONE   0x10 /* Offset to READ/WRITE done bit */
#define E1000_EEPROM_RW_REG_START  1    /* First bit for telling part to start operation */
#define E1000_EEPROM_RW_ADDR_SHIFT 8    /* Shift to the address bits */
#define E1000_EEPROM_POLL_WRITE    1    /* Flag for polling for write complete */
#define E1000_EEPROM_POLL_READ     0    /* Flag for polling for read complete */

 /* Receive Control */
#define E1000_RCTL_RST            0x00000001    /* Software reset */
#define E1000_RCTL_EN             0x00000002    /* enable */
#define E1000_RCTL_SBP            0x00000004    /* store bad packet */
#define E1000_RCTL_UPE            0x00000008    /* unicast promiscuous enable */
#define E1000_RCTL_MPE            0x00000010    /* multicast promiscuous enab */
#define E1000_RCTL_LPE            0x00000020    /* long packet enable */
#define E1000_RCTL_LBM_NO         0x00000000    /* no loopback mode */
#define E1000_RCTL_LBM_MAC        0x00000040    /* MAC loopback mode */
#define E1000_RCTL_LBM_SLP        0x00000080    /* serial link loopback mode */
#define E1000_RCTL_LBM_TCVR       0x000000C0    /* tcvr loopback mode */
#define E1000_RCTL_DTYP_MASK      0x00000C00    /* Descriptor type mask */
#define E1000_RCTL_DTYP_PS        0x00000400    /* Packet Split descriptor */
#define E1000_RCTL_RDMTS_HALF     0x00000000    /* rx desc min threshold size */
#define E1000_RCTL_RDMTS_QUAT     0x00000100    /* rx desc min threshold size */
#define E1000_RCTL_RDMTS_EIGTH    0x00000200    /* rx desc min threshold size */
#define E1000_RCTL_MO_SHIFT       12            /* multicast offset shift */
#define E1000_RCTL_MO_0           0x00000000    /* multicast offset 11:0 */
#define E1000_RCTL_MO_1           0x00001000    /* multicast offset 12:1 */
#define E1000_RCTL_MO_2           0x00002000    /* multicast offset 13:2 */
#define E1000_RCTL_MO_3           0x00003000    /* multicast offset 15:4 */
#define E1000_RCTL_MDR            0x00004000    /* multicast desc ring 0 */
#define E1000_RCTL_BAM            0x00008000    /* broadcast enable */
/* these buffer sizes are valid if E1000_RCTL_BSEX is 0 */
#define E1000_RCTL_SZ_2048        0x00000000    /* rx buffer size 2048 */
#define E1000_RCTL_SZ_1024        0x00010000    /* rx buffer size 1024 */
#define E1000_RCTL_SZ_512         0x00020000    /* rx buffer size 512 */
#define E1000_RCTL_SZ_256         0x00030000    /* rx buffer size 256 */
/* these buffer sizes are valid if E1000_RCTL_BSEX is 1 */
#define E1000_RCTL_SZ_16384       0x00010000    /* rx buffer size 16384 */
#define E1000_RCTL_SZ_8192        0x00020000    /* rx buffer size 8192 */
#define E1000_RCTL_SZ_4096        0x00030000    /* rx buffer size 4096 */
#define E1000_RCTL_VFE            0x00040000    /* vlan filter enable */
#define E1000_RCTL_CFIEN          0x00080000    /* canonical form enable */
#define E1000_RCTL_CFI            0x00100000    /* canonical form indicator */
#define E1000_RCTL_DPF            0x00400000    /* discard pause frames */
#define E1000_RCTL_PMCF           0x00800000    /* pass MAC control frames */
#define E1000_RCTL_BSEX           0x02000000    /* Buffer size extension */
#define E1000_RCTL_SECRC          0x04000000    /* Strip Ethernet CRC */
#define E1000_RCTL_FLXBUF_MASK    0x78000000    /* Flexible buffer size */
#define E1000_RCTL_FLXBUF_SHIFT   27            /* Flexible buffer shift */

#define VALUEATMASK(value, mask) value * ((mask) & ~((mask) << 1))


/* Receive Address */
#define E1000_RAH_AV  0x80000000        /* Receive descriptor valid */

/* Transmit Descriptor */
struct e1000_tx_desc {
    uint64_t buffer_addr;       /* Address of the descriptor's data buffer */
    union {
        uint32_t data;
        struct {
            uint16_t length;    /* Data buffer length */
            uint8_t cso;        /* Checksum offset */
            uint8_t cmd;        /* Descriptor control */
        } flags;
    } lower;
    union {
        uint32_t data;
        struct {
            uint8_t status;     /* Descriptor status */
            uint8_t css;        /* Checksum start */
            uint16_t special;
        } fields;
    } upper;
};


/* Receive Descriptor */
struct e1000_rx_desc {
    uint64_t buffer_addr; /* Address of the descriptor's data buffer */
    uint16_t length;     /* Length of data DMAed into data buffer */
    uint16_t csum;       /* Packet checksum */
    uint8_t status;      /* Descriptor status */
    uint8_t errors;      /* Descriptor Errors */
    uint16_t special;
};


struct e1000_data
{
	uint8_t buf[PKT_MAX_SIZE];
};

struct e1000_tx_desc txdesc_array[TXDESC_MAXSIZE] __attribute__ ((aligned(16)));
struct e1000_data txdata_buf[TXDESC_MAXSIZE] __attribute__ ((aligned (PGSIZE)));

struct e1000_rx_desc rxdesc_array[RXDESC_MAXSIZE] __attribute__ ((aligned(16)));
struct e1000_data rxdata_buf[RXDESC_MAXSIZE] __attribute__ ((aligned (PGSIZE)));

#endif //__KERN_NET_E1000_H__
