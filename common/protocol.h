#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__


struct _LedNetMessage
{
    uint8_t     version;
    uint8_t     func_id;
    uint16_t    byte_count;
};
typedef struct _LedNetMessage LedNetMessage;

typedef void (*LedNetFunc)(LedNetMessage *net_msg,uint8_t data_byte);

#define LED_NET_FUNC_RAW_DATA 0

#endif
