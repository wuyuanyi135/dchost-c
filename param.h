
#ifndef FLAG_EMBED

#define RX_ADDR_P0      (TX_ADDR)
#define RX_ADDR_P1      (0xE0E1E2E3E4)
#define TX_ADDR         (0xA0A1A2A3A4)

#else

#define RX_ADDR_P0      (TX_ADDR)
#define RX_ADDR_P1      (0xA0A1A2A3A4)
#define TX_ADDR         (0xE0E1E2E3E4)

#endif


/* dwire config */

#define DW_TX_SIZE      (64)
#define DW_RX_SIZE      (64)

#ifdef FLAG_EMBED
    #define DW_LOCAL_ADDR   (0x10)
#else
    #define DW_LOCAL_ADDR   (0x00)
#endif

#define DW_VAR_MAP_SIZE (4)
