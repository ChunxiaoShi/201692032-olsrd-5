最佳链路状态路由协议（OLSR）
===
* olsr和olsrd的关系：
the UniK OLSR daemon, is referred to as olsrd in this document.OLSR daemon is an implementation of the olsr.The implementation is **RFC3626** compliant with respect to both core and auxiliary functioning. 
 UniK - University Graduate Center

1.简介：
---
主要用于MANET网络(Mobile Ad hoc network)的路由协议（OLSR）。OLSR是根据MANET的要求，在传统的LS（Link state）协议的基础上优化的。
OLSR中的关键概念是多点转播（MPRs），MPRs是在广播洪泛的过程中挑选的转发广播的节点。
MANET是一种多跳ad-hoc无线网络，其中节点可以在拓扑中任意移动。网络没有给定的基础设施，可以在任何环境中快速设置。优化链路状态路由（OLSR）协议是用于这种移动自组织网络的路由管理协议。 

2.作用细节：
---
- （1）OLSR作为一种轮询机制，在初始化完成后，就会长期处于olsr_scheduler循环中，节点担任两种角色，一个是作为消息报发送方，一个是作为消息报接收方，节点需要不断在这两种角色之间切换。
- （2）当节点担当消息发送方角色时，节点会根据初始化时已经设定好的定时触发器来行动。当olsr_scheduler轮询到walk_timers的时候，节点都会检查所有到期的间隔触发器所指定的函数，来产生和建立节点上的各种消息，generate_hello()，generate_tc()，generate_mid()函数分别产生hello消息，tc消息，mid消息。
- （3）当节点担任接收方角色时，节点会监听多个socket，当某个socket处于可读时，就将接收到的OLSR消息解析并传递给相应的处理函数，用以更新邻域信息库、路由表等。olsr_input_hello()，olsr_input_tc() ,olsr_input_mid()函数分别处理hello消息，tc消息，mid消息。
- （4）节点除了这两个角色之外，处理完所接收到的消息分组后，如果相应的邻域结构或拓扑结构发生改变，节点则会调用olsr_calculate_routing_table()函数对整个网络的路由表进行重新计算。

3.消息包数据结构
---
- （1）OLSR使用统一的数据包格式与协议相关的所有数据进行通信。这样做的目的是在不破坏向后兼容性的情况下促进协议的可扩展性。这也提供了将不同“类型”的信息捎带成单个传输的简单方法，并且因此对于给定的实现来优化以利用由网络提供的最大帧大小。这些数据包被嵌入在UDP数据报中，以便通过网络传输。
- （2）struct olsr_common 时olsr基本数据包
在/src/lq_packet.h中定义。
具体理解见文档注释。
- *olsr有两种控制分组：HELLO和TC
- （3）HELLO消息：
	a. OLSR需要一些机制来检测邻居以及与之通信线路的状态。为此，定期发送HELLO消息。使用HELLO消息的邻居发现会话的简化版本: A首先发送一个空的HELLO消息。因为B在HELLO消息中找不到自己的地址,B接收此消息并注册A为非对称邻居。然后B发送一个HELLO声明A为非对称邻居。当A收到此消息时，它在其中找到自己的地址，因此设置B为对称邻居。这次A发送的HELLO包括B的地址，B在接收到HELLO消息时注册A为对称邻居。
	b. HELLO消息也有其他用途。生成它们并将其发送到所有单跳邻居，以实现链路感测，邻居感测，双跳邻居感测和MPR选择器感测。
	c. HELLO消息只在一跳范围内广播，不能被转发。

在/src/lq_packet.h中有头部相关定义，见注释。
在/src/packet.h中有相关定义，见注释。

lq_hello_header和lq_hello_info_header共同组成HELLO消息包的头部。格式如图：https://cococolin.github.io/images/2018_1_3_olsr/4.png
其中neighbor Interface Address是邻居结点地址列表，每一种链路类型后面都紧跟一个邻居地址列表，表明发送该HELLO分组的节点到这个邻居列表中的所有节点的链路类型是相同的。
- （4）TC消息：
TC-拓扑控制。
每个节点周期性的发送TC分组。当一个节点接收TC消息时进入拓扑消息维护模块。
收到TC消息必要时进行转发(实现拓扑泛洪);如果得到网络中一条有效的链路(拓扑)，则将其添加到拓扑表中，用以计算路由。
当检测到拓扑表发生变化时，就要转到路由建立与维护模块，重新计算路由。当收到数据包时将其转发。

例如：节点B、C、D和E是节点A的邻居节点，其中节点B、C和D
又是节点A的MPR节点。当节点B、C、D和E收到节点A发送来的TC控制消
息时:首先，它们都需要判断自己是不是节点A的MPR节点。节点B、C和D发
现自己是节点A的MPR节点，它们再判断TC控制消息是否是最新的，如果是，
则转发该TC控制消息，否则丢弃该TC控制消息;而节点E发现自己不是节点A
的MPR节点，则不转发该TC控制消息。

相关定义在/src/lq_packet.h中，见注释。
