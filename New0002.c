OS_CFG_PRIO_MAX  最大优先级个数
	
OSSched() 任务切换

不希望发生任务调度
OSSchedLock() 对调度器加锁
OSSchedUnlock() 对已经上锁的任务调度器解锁


信号量(保护共享资源 任务同步)
互斥信号量(保护共享资源)
任务内建信号量(任务同步)

以上只是同步 无法传输数据

消息队列(任务间传递数据)
消息内建队列

事件标志组

//-------------------------------------------------------

OSTaskCreat(任务控制块
			任务名字
			任务函数
			向任务函数传递参数 p_arg
			任务优先级
			任务堆栈基地址
			任务堆栈深度限位（一般是任务堆栈的10分之一） 
			任务堆栈大小
			任务内建消息队列长度（任务内建消息队列用到 否则为0）
			使用时间片的个数(时间片轮转会用到 否则为0)
			用户补充的存储区 用来扩展用的tcb
			opt选项 OS_OPT_TASK_CHK|OS_OPT_TASK_STK_CLR
			错误代码
			)
 
OSTaskDel(任务控制块
			错误代码
		)
//任务优先级
#define START_TASK_PRIO	3

//任务堆栈大小
#define START_STK_SIZE	128

//任务控制块
OS_TCB StartTaskTCB;

//任务堆栈
CPU_STK STAR_TASK_STK[START_STK_SIZE]

//任务函数声明
void start_task(void *p_arg);


//任务挂起
OSTaskSuspend(任务控制块
				错误代码)

//任务恢复
OSTaskResume(任务控制块
					错误代码)
 
/***********************************************/
时间片轮转调度
//同一优先级的任务 用时间片轮转调度进行切换
//创建任务时 给任务配置需要几个时间片 （OS_TICK）
//时间片轮转调度
OS_CFG_SCHEF_ROUND_ROBIN_EN    1u

//配置时间片轮转时间长度
OSSchedRoundRobinCfg(DEF_ENABLED   //使能
					1，//1*TICK（1/1000） 就是1ms 一个时间片长度为1ms
					错误代码
)

//放弃本次时间片
OSSchedRoundRobinYield(OS_ERR *p_err)

/*
在创建任务时 给时间片的个数过少 会导致任务没有执行完就切换到下个任务
等再切换回来 会接着上次没执行的地方继续执行 
*/

/**********************************************/
系统内部任务  5个
//空间任务

//时钟节拍任务

//统计任务（CPU使用率和个任务堆栈使用情况）
宏 OS_CFG_STAT_TASK_EN  1u
调用 OSStatTaskCPUUsageInit()
OS_CFG_STAT_TASK_PRIO  优先级 OS_CFG_PRIO_MAX-2

任务控制块中查看堆栈使用情况
CPU_STK_SIZE	StkUsed
CPU_STK_SIZE	StkFree

//定时任务
OS_CFG_TMR_EN	1u

//中断服务管理任务(可以减少中断关闭的时间 优先级为0)
OS_CFG_ISR_POST_DEFERRED_EN	1u

//ucosIII钩子函数（用来扩展其他任务的功能）
OS_app_hooks.c (钩子函数的接口在此文件中)

App_OS_IdleTaskHook(void)	//空闲任务 （可以倒计时30秒)可以用来让cpu进入低功耗模式
App_OS_StatTaskHook()	//统计任务的钩子函数

/**************************************************/
中断 UCOSIII最多嵌套250层
void USART1_IRQHandler(void)
{
	OSIntEnter();
		//中断服务程序
	OSIntExit();
}

/**************************************************/
临界段代码保护

OS_CFG_POST_DEFERRED_EN = 0 采用关中断的方式来保护临界代码段
=1 采用给调度其上锁的方式保护临界代码段

CPU_SR_ALLOC()	//想用临界段代码 需要先调用这个宏

OS_CRITICAL_ENTER()	//进入临界段代码
OS_CRITICAL_EXIT() 和 OS_CRITICAL_EXIT_NO_SCHED	//退出临界段代码



/****************************************************/ 
时间管理

//延时函数
OSTimeDly()	//相对模式 周期模式 绝对模式 [延时多少个时间片]b
OPT选项	OS_OPT_TIME_DLY     相对模式
		OS_OPT_TIME_TIMEOUT 和相对模式一样
		OS_OPT_TIME_MATCH   绝对模式


OSTimeDlyHMSM()	//只有相对模式 

//取消任务的延时
OSTimeDlyResume()	//会引发一次任务调度

//获取任务运行时间
OSTimeGet()	//获取时钟节拍计数器的值
			//获取全局变量OSTickCtr的系统时钟节拍数
//在任务开始时调用 在任务结束时调用 可以获取任务节拍数
//[任务节拍数*(1/OS_CFG_TICK_RATE_HZ)] 就是运行时间

/**************************************************/
//软件定时器

递减计数器
	计数器减到0时可以触发某种动作的执行
	这个动作通过回调函数来实现
		回调函数 函数A 函数B
			A(参数是函数B的指针) 称函数B是函数A的回调函数
	
OS_CFG_TMR_EN  1u  //使能软件定时器 

#define OS_CFG_TMR_RATE_HZ 100u //时间分辨率 Hz （100Hz---10ms）

OS_TMR tmr1; //定义软件定时器
void tmr1_callback(void *p_tmr, void *p_arg); //声明回调函数

//创建定时器
OSTmrCreate(OS_TMR *p_tmr
			CPU_CHAR *p_name
			dly 初始的延时
			period 初始延时结束之后的周期
			opt选项
			回调函数
			回调函数的参数
			错误代码
)

OPT选项
	单次定时器(只执行一次 开始倒计数dly 初始的延时)
	
	周期模式
		无初始延迟(period倒计为0 重新开始倒计period)
		有初始延迟(dly倒计为0 重新开始倒计period 到0重新倒计period)

删除定时器
OSTmrDel()

获取定时器的剩余时间
OSTmrRemainGet()

获取当前定时器状态
OSTmrStateGet()

//开启定时器
OSTmrStart(OS_TMR *p_tmr
			错误代码
)
//停止定时器倒计时
OSTmrStop(OS_TMR *p_tmr
			opt(1.OS_OPT_TMR_NONE 什么都不做 )
				(2.OS_OPT_TMR_CALLBACK 调用一次回调函数)
				(3.)
			回调函数参数
			错误代码
)

要避免在回调函数中使用阻塞调用或可以阻塞或删除定时器任务的函数

/**************************************************/
信号量(先创建信号量 再创建任务)
	信号量(用作任务同步)就是任务同步的标志 flag  无法传输内容

1.用于对共享资源的保护
2.通常用来做任务同步(中断和任务 任务和任务的同步)

	任务同步用信号量（一触发 几乎同步发生响应）

	想要获取信号量的任务必须执行等待操作,
	信号量的有效值大于1,则任务可以获取资源,
	信号量有效值为0,则任务加入等待信号量的任务表中.
	可以设置等待超过某一个设定值,该信号量仍然没有被释放掉,
则等待信号量的任务就进入就绪态,
	可以设置等待时间为0,任务一直等待

	信号量分两种：二进制信号量(只能取值0和1) 计数型信号量(大于1)
	计数型信号量范围由 OS_SEM_CTR 决定 可以为8位 16位 32位

	二进制信号量用于那些一次只能一个任务使用的资源
	计数型信号量用于某些资源可以同时被几个任务所使用

OS_SEM  MY_SEM; //定义信号量

//创建信号量
OSSemCreate(OS_SEM *p_sem
			信号量名字
			cnt 初始值
			错误代码
)

//等待信号量
OSSemPend(	OS_SEM
			timeout(设置为0 无限等待 单位OS_TICK)
			opt(1.OS_OPT_PEND_BLOCKING 任务挂起等待信号量
				2.OS_OPT_PEND_NON_BLOCKING 任务直接返回)
			p_ts(记录接收到时间戳 NULL或0不需要记录时间戳)
			错误代码
)

//发出信号量
OSSemPost(	OS_SEM
			opt(1.OS_OPT_POST_1        向等待优先级最高的任务发送信号
				2.OS_OPT_POST_ALL      向所有等待信号量的任务发送信号
				3.OS_OPT_POST_NO_SCHED 本函数不引起任务切换)
			错误代码
) 

//删除一个信号量
OSSemDel(	OS_SEM
			OPT
			错误代码
)

//取消等待信号量
OSSemPendAbrot(OS_SEM
				opt(模糊 不清楚)
				错误代码)

//强制设置一个信号量的值
OSSemSet()


先创建信号量 再创建任务

  应用
/*  信号量用作保护共享资源
创建信号量时初始值为1,为了让一个任务可以接收到信号量
OSSemPend(&MY_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); 	//请求信号量
	memcpy(share_resource,task1_str,sizeof(task1_str)); //向共享资源区拷贝数据
	delay_ms(300);
	printf("%s\r\n",share_resource);	//串口输出共享资源区数据
OSSemPost (&MY_SEM,OS_OPT_POST_1,&err);				//发送信号量
					向优先级最高的任务发送信号 让优先级最高的任务获取共享资源区
*/

/* 使用信号量做任务同步
创建信号量时初始值为0
任务1 Post
任务2 Pend 会完成一次任务同步
*/

信号量 Post一次 信号量.Ctr的值会加1
       Pend一次 信号量.Ctr的值会减1

/********************************************************/
互斥信号量

(避免优先级反转【访问共享资源区（低中高 高等待 中运行 高等低信号量） 低优先级不释放信号量 高优先级获得不了信号量 只能等低优先级完事】）
解决方法：提升低优先级的优先级为想要获得信号量的高优先级的优先级

OS_MUTEX  TEST_MUTEX;  //定义互斥信号量

//创建互斥信号量 (不像信号量那样需要初始值)
OSMutexCreate(	OS_MUTEX
				互斥信号量名字
				错误代码
)

//等待互斥信号量 (和信号量一样)
OSMutexPend(OS_MUTEX
			timeout
			opt
			CPU_TS *p_ts
			错误代码
)

//发送互斥信号量
OSMutexPost(OS_MUTEX
			opt
			错误代码
)

//删除互斥信号量
OSMutexDel()

//取消等待
OSMutexPendAbrot()

/* 互斥信号量访问共享资源
		printf("high task Pend Mutex\r\n");
OSMutexPend (&TEST_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);	//请求互斥信号量
		printf("high task Running!\r\n");
		LCD_Fill(6,131,114,313,lcd_discolor[num%14]); 				//填充区域
		LED1 = ~LED1;
OSMutexPost(&TEST_MUTEX,OS_OPT_POST_NONE,&err);				//释放互斥信号量
*/


/*********************************************************/
任务内建信号量
任务内嵌信号量(简化代码 比独立的信号量更有效 os_task.c)
每个任务都有自己的内嵌信号量
不需要创建 任务自带
/*

应用 ：任务同步
任务1中 发送任务2的控制块
(任务1发送任务2的内建信号量,任务2接收,已达到同步)

*/

//等待任务信号量
OSTaskSemPend(	OS_TICK timeout
				opt
				CPU_TS *p_ts
				错误代码
)

//发送任务信号量
OSTaskSemPost(	OS_TCB *p_tcb
				opt
				错误代码
)

//强行设置任务信号量计数值
OSTaskSemSet()

//取消等待任务信号量
OSTaskSemPendAbort()

/***************************************************/
消息队列(任务间通信)(是引用传递 不是值传递)os_q.c
	消息传递
用动态内存或全局变量 (不能用局部变量)

{*工程例程*}

消息的三部分:指向数据的指针
			数据的长度
			记录消息发布时刻的时间戳

OS_Q  MY_Q; //定义消息队列

//创建消息队列
OSQCreate(	OS_Q *p_q
			消息队列名字
			OS_MSG_QTY max_qty 消息队列大小
			错误代码
)

//等待消息
OSQPend(OS_Q *p_q               请求哪个消息队列
		timeout                 超时时间值 0一直等待
		opt                     阻塞等待/不阻塞
		OS_MSG_SIZE *p_msg_size 接收到消息大小
		CPU_TS *p_ts            时间戳
		错误代码
)
返回值就是消息指针

//向消息队列中发送一则消息
OSQPost(OS_Q *p_q               向哪个消息队列发送消息
		void *p_void            消息指针
		OS_MSG_SIZE msg_size    消息大小 字节
		OPT(1.OS_OPT_POST_ALL       发送给所有等待的消息队列
			2.OS_OPT_POST_FIFO      FIFO形式 
			3.OS_OPT_POST_LIFO      LIFO形式
			4.OS_OPT_POST_NO_SCHED  不做任务切换
		错误代码
)

//删除消息队列
OSQDel()

//清空消息队列
OSQFlush()

//取消等待消息
OSQPendAbort()

/****************************************************/
任务内建消息队列

//应用：一个任务等待一个消息队列
//		多个任务等待一个消息队列不用内建的

OS_CFG_TASK_Q_EN  1u

//等待消息
OSTaskQPend(timeout
			OPT
			OS_MSG_SIZE
			CPU_TS
			错误代码
)

返回值就是消息指针



//向任务发送一则消息
OSTaskPost(	OS_TCB
			void *p_buf
			OPT
			错误代码
)

//清空任务的消息队列
OSTaskQFlush()

//取消等待消息(*取消之后会执行一遍挂起的任务)
OSTaskQPendAbort()

/**************************************************/
04:18
事件标志组(一个任务与多个事件同步)

一个任务需要与多个事件同步
或同步 任何一个
与同步 所有事件

OS_CFG_FLAG_EN  1u

OS_FLAG_GRP EventFlags
//创建事件标志组
OSFlagCreate(OS_FLAG_GRP
			名字
			初始值
			错误代码
)

//等待事件标志组
OSFlagPend(OS_FLAG_GRP
			事件标志组哪几个bit
			超时时间
			OPT 等待所有标志清零/任意清理/所有置1/任意为1/消耗掉改变的标志位(复位)
			时间戳
			错误代码
)


//向事件标志组发布标志
OSFlagPost(OS_FLAG_GRP
			对哪些位操作
			OPT 置1/清0
			错误代码
)
返回值：改变后的值


/*******************************************************/
//任务同时等待多个信号量 和 多个消息队列

OSPendMulti(数组 （大小为等待内核的个数）
			内核对象个数
			超时时间
			OPT
			错误代码
)
返回值：为有几个内核对象可用 

/******************************************************/
//存储区










