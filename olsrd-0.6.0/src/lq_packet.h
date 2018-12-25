
/*
 * The olsr.org Optimized Link-State Routing daemon(olsrd)
 * Copyright (c) 2004, Thomas Lopatic (thomas@lopatic.de)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of olsr.org, olsrd nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Visit http://www.olsr.org for more information.
 *
 * If you find this software useful feel free to make a donation
 * to the project. For more information see the website or contact
 * the copyright holders.
 *
 */

#ifndef _OLSR_LQ_PACKET_H
#define _OLSR_LQ_PACKET_H

#include "olsr_types.h"
#include "packet.h"
#include "mantissa.h"
#include "ipcalc.h"

#define LQ_HELLO_MESSAGE      201
#define LQ_TC_MESSAGE         202

/* deserialized OLSR header 反序列化olsr头部*/

struct olsr_common {  /*基本数据包*/
  uint8_t type;       /*消息类型。消息类型0-127由OLSR保留，
                        而128-255空间被认为是“私有”，
                        可用于协议的自定义扩展。*/
  olsr_reltime vtime; /*该字段指示接收后多长时间内节点将此信息
                        视为有效的时间。时间间隔以尾数-指数格
                        式表示。*/
  uint16_t size;      /*消息大小。以字节为单位，包括header*/
  union olsr_ip_addr orig;/*发送端ip地址*/
  uint8_t ttl;        /*time to live 跳数*/
  uint8_t hops;       /*此消息在传递过程中经历的跳数*/
  uint16_t seqno;     /*消息的序列号，这是唯一不变的！！
                        以确保消息不会被重发。每发送一个新消息
                        包就+1*/
};

/* serialized IPv4 OLSR header */

struct olsr_header_v4 {   /*分组头部*/
  uint8_t type;
  uint8_t vtime;
  uint16_t size;
  uint32_t orig;
  uint8_t ttl;
  uint8_t hops;
  uint16_t seqno;
};

/* serialized IPv6 OLSR header */

struct olsr_header_v6 {
  uint8_t type;
  uint8_t vtime;
  uint16_t size;
  unsigned char orig[16];
  uint8_t ttl;
  uint8_t hops;
  uint16_t seqno;
};

/* deserialized LQ_HELLO */

struct lq_hello_neighbor {/*邻居结点集HELLO消息的头部*/
  uint8_t link_type;      /*链路状态，有三种，非对称链路，
                            对称链路，列表中节点被选为MPR。*/
  uint8_t neigh_type;     /*邻居结点状态，对称、非对称*/
  union olsr_ip_addr addr;/*发送端ip地址*/
  struct lq_hello_neighbor *next;/*将要把HELLO消息传递给的
                                  下一个邻居结点信息*/
  uint32_t linkquality[0];/*链路质量*/
};

struct lq_hello_message {   /*（Message部分）消息数据包的头部*/
  struct olsr_common comm;  /*olsr消息*/
  olsr_reltime htime;       /*消息发送间隔*/
  uint8_t will;             /*指定节点的意愿进行，有WILL_NEVER
                              的节点被选为MPR的任意结点*/
  struct lq_hello_neighbor *neigh;/*消息传递的下一节点*/
};

/* serialized LQ_HELLO */
struct lq_hello_info_header {
  uint8_t link_code;    /*链路类型：ASYM_LINK,SYM_LINK,MPR_LINK,
                          还可提供附加信息，例如链路中断LOST_LINK*/
  uint8_t reserved;     /*保留字必须为0000 0000*/
  uint16_t size;        /*本链路消息的大小，从link_code到下一个
                          link_code前*/
};

struct lq_hello_header {
  uint16_t reserved;    /*保留字，0000 0000 0000*/
  uint8_t htime;        /*此接口的HELLO消息发送时间间隔*/
  uint8_t will;         /*描述为一个节点为其他节点携带网络流量的意愿。
                          一个具有willing-never的节点永远不会被任何节点选为MPR，
                          一个具有willing-always的节点永远会被选为MPR，
                          默认情况下，设为willing-default*/
};

/*
lq_hello_header和lq_hello_info_header共同组成HELLO消息包的头部。
*/


/* deserialized LQ_TC */
struct lq_tc_message {    /*封装后的拓扑数据包格式。*/
  struct olsr_common comm;/*olsr基本数据包*/
  union olsr_ip_addr from;/*到达目的地的倒数第二跳地址*/
  uint16_t ansn;          /*记录本节点收到的最近一个TC分组的ANSN序列号。
                            当收到一个新的TC分组时，将新的TC分组的ANSN号
                            与拓扑表中的相对应的ANSN序列号比较，
                            以此判断接收还是丢弃该消息*/
  struct tc_mpr_addr *neigh;/*指向广播邻居集
                              struct tc_mpr_addr定义在packet.h中*/
};

/* serialized LQ_TC */

struct lq_tc_header {   /*TC消息数据包头部*/
  uint16_t ansn;        /*记录本节点收到的最近一个TC分组的ANSN序列号。*/
  uint8_t lower_border; /*下一级边界*/
  uint8_t upper_border; /*上一级边界*/
};

static INLINE void
pkt_get_u8(const uint8_t ** p, uint8_t * var)
{
  *var = *(const uint8_t *)(*p);
  *p += sizeof(uint8_t);
}
static INLINE void
pkt_get_u16(const uint8_t ** p, uint16_t * var)
{
  *var = ntohs(**((const uint16_t **)p));
  *p += sizeof(uint16_t);
}
static INLINE void
pkt_get_u32(const uint8_t ** p, uint32_t * var)
{
  *var = ntohl(**((const uint32_t **)p));
  *p += sizeof(uint32_t);
}
static INLINE void
pkt_get_s8(const uint8_t ** p, int8_t * var)
{
  *var = *(const int8_t *)(*p);
  *p += sizeof(int8_t);
}
static INLINE void
pkt_get_s16(const uint8_t ** p, int16_t * var)
{
  *var = ntohs(**((const int16_t **)p));
  *p += sizeof(int16_t);
}
static INLINE void
pkt_get_s32(const uint8_t ** p, int32_t * var)
{
  *var = ntohl(**((const int32_t **)p));
  *p += sizeof(int32_t);
}
static INLINE void
pkt_get_reltime(const uint8_t ** p, olsr_reltime * var)
{
  *var = me_to_reltime(**p);
  *p += sizeof(uint8_t);
}
static INLINE void
pkt_get_ipaddress(const uint8_t ** p, union olsr_ip_addr *var)
{
  memcpy(var, *p, olsr_cnf->ipsize);
  *p += olsr_cnf->ipsize;
}
static INLINE void
pkt_get_prefixlen(const uint8_t ** p, uint8_t * var)
{
  *var = netmask_to_prefix(*p, olsr_cnf->ipsize);
  *p += olsr_cnf->ipsize;
}

static INLINE void
pkt_ignore_u8(const uint8_t ** p)
{
  *p += sizeof(uint8_t);
}
static INLINE void
pkt_ignore_u16(const uint8_t ** p)
{
  *p += sizeof(uint16_t);
}
static INLINE void
pkt_ignore_u32(const uint8_t ** p)
{
  *p += sizeof(uint32_t);
}
static INLINE void
pkt_ignore_s8(const uint8_t ** p)
{
  *p += sizeof(int8_t);
}
static INLINE void
pkt_ignore_s16(const uint8_t ** p)
{
  *p += sizeof(int16_t);
}
static INLINE void
pkt_ignore_s32(const uint8_t ** p)
{
  *p += sizeof(int32_t);
}
static INLINE void
pkt_ignore_ipaddress(const uint8_t ** p)
{
  *p += olsr_cnf->ipsize;
}
static INLINE void
pkt_ignore_prefixlen(const uint8_t ** p)
{
  *p += olsr_cnf->ipsize;
}

static INLINE void
pkt_put_u8(uint8_t ** p, uint8_t var)
{
  **((uint8_t **)p) = var;
  *p += sizeof(uint8_t);
}
static INLINE void
pkt_put_u16(uint8_t ** p, uint16_t var)
{
  **((uint16_t **)p) = htons(var);
  *p += sizeof(uint16_t);
}
static INLINE void
pkt_put_u32(uint8_t ** p, uint32_t var)
{
  **((uint32_t **)p) = htonl(var);
  *p += sizeof(uint32_t);
}
static INLINE void
pkt_put_s8(uint8_t ** p, int8_t var)
{
  **((int8_t **)p) = var;
  *p += sizeof(int8_t);
}
static INLINE void
pkt_put_s16(uint8_t ** p, int16_t var)
{
  **((int16_t **)p) = htons(var);
  *p += sizeof(int16_t);
}
static INLINE void
pkt_put_s32(uint8_t ** p, int32_t var)
{
  **((int32_t **)p) = htonl(var);
  *p += sizeof(int32_t);
}
static INLINE void
pkt_put_reltime(uint8_t ** p, olsr_reltime var)
{
  **p = reltime_to_me(var);
  *p += sizeof(uint8_t);
}
static INLINE void
pkt_put_ipaddress(uint8_t ** p, const union olsr_ip_addr *var)
{
  memcpy(*p, var, olsr_cnf->ipsize);
  *p += olsr_cnf->ipsize;
}

void olsr_output_lq_hello(void *para);

void olsr_output_lq_tc(void *para);

void olsr_input_lq_hello(union olsr_message *ser, struct interface *inif, union olsr_ip_addr *from);

extern bool lq_tc_pending;

#endif

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
