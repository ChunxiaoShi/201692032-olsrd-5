
/*
 * The olsr.org Optimized Link-State Routing daemon(olsrd)
 * Copyright (c) 2004, Andreas Tonnesen(andreto@olsr.org)
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

#ifndef _OLSR_PACKET
#define _OLSR_PACKET

#include "olsr_protocol.h"
#include "interfaces.h"
#include "mantissa.h"

struct hello_neighbor {     /*HELLO消息的邻居结点集*/
  uint8_t status;           /*记录邻居的状态*/
  uint8_t link;             /*链接的类型*/
  union olsr_ip_addr main_address;/*邻居的主地址*/
  union olsr_ip_addr address;     /*邻居的其他地址*/
  struct hello_neighbor *next;    /*发送给的下一邻居结点*/
  olsr_linkcost cost;             /*代价*/
  uint32_t linkquality[0];        /*质量*/
};

struct hello_message {    /*消息数据包*/
  olsr_reltime vtime;     
  olsr_reltime htime;
  union olsr_ip_addr source_addr;/*源地址*/
  uint16_t packet_seq_number;    /*序列号*/
  uint8_t hop_count;      /*跳数*/
  uint8_t ttl;            /*time to live*/
  uint8_t willingness;    /*指定节点的意愿进行，有意愿的WILL_NEVER的节点被选为MPR的任意结点*/
  struct hello_neighbor *neighbors;/*消息传递的下一节点*/

};

struct tc_mpr_addr {    /*广播邻居集，将本节点选为MPR节点的邻居节点*/
  union olsr_ip_addr address;
  struct tc_mpr_addr *next;
  uint32_t linkquality[0];
};

struct tc_message {   /*TC消息数据包格式*/
  olsr_reltime vtime; /*有效时间*/
  union olsr_ip_addr source_addr;/*源地址*/
  union olsr_ip_addr originator;/*到达目的地址的倒数第二跳地址*/
  uint16_t packet_seq_number;   /*序列号*/
  uint8_t hop_count;            
  uint8_t ttl;
  uint16_t ansn;      /*本节点收到的最近一个TC分组的ANSN序列号。*/
  struct tc_mpr_addr *multipoint_relay_selector_address;
                      /*TC分组仅仅包含MPRselector(将本节点选为MPR节点的邻居节点)的地址，
                      而不是所有邻居节点的地址。因为TC分组数据包必须通过MPR节点被广播到全网中,
                      用以维护网络的拓扑信息、确保链路时刻的连通状态和更新路由表集。*/
};

/*
 *MID messages - format:
 *
 *ADDR
 *ADDR
 *ADDR
 *.....
 */

struct mid_alias {
  union olsr_ip_addr alias_addr;
  struct mid_alias *next;
};

struct mid_message {
  olsr_reltime vtime;
  union olsr_ip_addr mid_origaddr;     /* originator's address */
  uint8_t mid_hopcnt;                  /* number of hops to destination */
  uint8_t mid_ttl;                     /* ttl */
  uint16_t mid_seqno;                  /* sequence number */
  union olsr_ip_addr addr;             /* main address */
  struct mid_alias *mid_addr;          /* variable length */
};

struct unknown_message {
  uint16_t seqno;
  union olsr_ip_addr originator;
  uint8_t type;
};

void olsr_free_hello_packet(struct hello_message *);

int olsr_build_hello_packet(struct hello_message *, struct interface *);

void olsr_free_tc_packet(struct tc_message *);

int olsr_build_tc_packet(struct tc_message *);

void olsr_free_mid_packet(struct mid_message *);

#endif

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
