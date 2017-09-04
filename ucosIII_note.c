/**
  ******************************************************************************
  * @file    ucosIII_note.c
  * @author  
  * @version V1.0
  * @date    21-June-2017
  * @brief   
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/


/**
  * @brief  
  * @param  
  * @retval 
  */

1. 目录与文件
	
2. 初识ucosIII
	(1) 单任务例子
	
	(2) 多任务例子
			创建了消息队列，互斥信号量

3. 临界代码段(critical Section 感觉翻译成<关键区域，关键代码段>更易于理解)

	临界段代码(critical Section)是指那些必须完整连续运行,不可被打断的代码段。

	两种方式：
		1.关中断
		
			OS_CFG_ISR_PosT_DEFERRED_EN 设置为 0(见 os_cfg.h) ,则 μC/OS- III 会采用关中断的方式来保护临界段代码;

			可测量关中断的时间
				cpu_cfg. h 头文件中的 CPU_CFG_INT_DIS_MEAS_EN 置为 1 来允许该功能

		2.给调度器上锁
		
			OS_CFG_ISR_PosT_DEFERRED_EN 设置为 1(见 os_cfg.h) ,则 μC/OS- III 会采用给调度器上锁的方式来保护临界段代码;

			可测量给调度器上锁的时间
				os_cfg.h 头文件中的 OS_CFG_SCHED_LOCK_TIME_MEAS_EN 置为 1 来允许该功能。

4. 任务管理
	两种任务类型：运行至完成任务，无限循环任务

	内存碎片
		必须谨慎对待存储器动态分配时产生
的碎片。 如果频繁创建和删除任务,那么存储器最终会布满碎片,导致
无法再使用存储器管理函数绐任务分配栈空间。 出于这种原因,在嵌入
式系统中,一般允许动态分配栈空间,但是,一旦分配栈空间,就最好不
要再释放已分配的栈空间。 也就是说，只要不把已分配的栈空间释放回
去，用户就还可以从存储器堆区中给任务分配栈空间。

	任务优先级的分配
		单调执行率调度法RMS：简单的说,执行频率最高的任务分配的优先级最高。

	栈空间的大小
		栈空间的大下应该在计算值的基础上乘以一个安全系数,一般取值在 1.5 ~ 2 之间。

		通过监视任务在运行时所使用的栈空间，可得到栈空间的实际大小，之后再乘上安全系数
		
	任务栈溢出检测（懵懂 有一些不懂）
		具有堆栈检测功能的cpu
			硬件溢出检测
			
		不具有堆栈检测功能的cpu
			软件溢出检测

	任务状态
		用户角度 5种：休眠态，就绪态，运行态，等待态，中断服务态

		内部角度 8种：就绪，延时，等待，带超时检测的等待，
	                  被挂起，延时且被挂起，等待且被挂起，带超时检测的等待且被挂起

	任务控制块TCB 
		(os.h)struct os_tcb
		os_tcb各个成员只能由ucosIII访问，不能由应用程序代码访问

	系统内部任务
		ucosIII有5个系统内部任务
			在初始化时，至少创建2个系统任务(OS_IdleTask()和OS_TickTask()),
			其他3个系统任务(OS_StatTask(),OS_TmrTask和OS_Task())根据需要选择创建。
			在os_cfg.h中宏定义配置 
				OS_CFG_STAT_TASK_EN   可允许OS_StatTask()     os_cfg.h 
				OS_CFG_TMR_EN         可允许OS_TmrTask()      os_cfg.h 
				OS_CFG_ISR_POST_DEFERRED_EN 可允许OS_IntQTask() os_cfg.h 

		1.空闲任务(OS_IdleTask( ) 、os_core.c)
		（懵懂）2.时钟节拍任务务(OS_TickTask( ) 、os_tick.c)
		3.统计任务(OS_StatTask( ) 、os_stat.c)
		4.定时任务(OS_TmrTask( ) 、os_tmr.c)
		5.中断服务管理任务(OS_IntQTask()、os_int.c)

5. 任务就绪表
	 就绪表包括两部分：一个就绪优先级位映射表 OSPrioTbl[],用来标明哪个优先级下有任务就绪;
			一个就绪任务列表 OSRdyList[],其中包含指向各个就绪任务的指针。

6. 任务调度
	抢占式调度
	调度点(临界点，任务切换点)
	时间片轮转调度(同一优先级下的多个任务，利用时间片轮转法调度)
		通过OSSchedRoundRobinCfg()来改变默认的时间片长度
		在运行时，通过OSTaSkTimeQuantaSet()来改变

			1. 任务级调度OSSched()(os_core.c)
			调度器和任务切换执行期间中断是关掉的,因为必须保证这些操作的不可分割性。

			2. 中断级调度器OSIntExit() (见 OS_core.c)
			调用OSIntExit()时,中断应该是禁止的。

			3. OS_SchedRoundRobin(OS_RDY_LIST * p_rdy_list)()

		同一优先级下的多个任务，利用时间片轮转法调度

7. 任务切换
	任务级的切换由函数OSCtxSw()来实现,该函数通过宏OS_TASK_SW()来调用。

	中断级的任务切换由函数 OSIntCtxSw()来实现。

	上述两个函数在 os_cpu_a.asm中

8. 中断管理
	典型中断服务程序 和 无需内核参与的中断服务程序

		典型中断服务程序：通过调用OSSemPost、OSTaskSemPost、OSFlagPost、
						OSQPost 或 OSTaskQPost 等“发布”函数向任务发消息或信号

		无需内核参与的中断服务程序：不通过内核函数向任务发送消息，不需要在任务代码中处理

	从中断发送消息的两种处理模式：直接发布 和 延时发布
	OS_cfg.h  OS_CFG_ISR_POST_DEFERRED_EN    直接发布为0 延时发布为1

		直接发布模式使用关闭中断的方式来保护临界段代码;
		延迟发布模式使用锁定任务调度器的方式来保护临界段代码。
		这两个模式的选择取决于应用对中断响应时间和任务响应时间的要求。

9. 任务挂起表（pend lists）
	任务挂起表管理函数

10. 时间管理
	OSTimeDly(OS_TICK dly, OS_OPT opt, OS_ERR * p_err)
		三种模式:相对模式 周期模式 绝对模式

	OSTimeDly(2, 				//在OS_CFG_STAT_TASK_RATE_HZ为1000Hz 任务延时2ms（不精准）
			  OS_OPT_TIME_DLY,  //相对模式
			  &err)

			 OS_OPT_TIME_PERIODIC  //周期模式 任务在每次执行后，再过指定的时间间隔，会被再次唤醒

			 OS_OPT_TIME_MATCH  //绝对模式 用于上电后指定的时间执行具体的操作

	OSTimeDlyHMSM(CPU_INT16U hours, CPU_INT16U minutes, CPU_INT16U seconds, CPU_INT32U milli, OS_OPT opt, OS_ERR * p_err)

	第5个参数 OS_OPT_TIME_HMSM_STRICT 检查延时参数有效性 H 0~99 M 0~59 S 0~59 M 0~999

			  OS_OPT_TIME_HMSM_NON_STRICT 任意参数 H 0~999 M 0~9999 S 不超过65535 M 不超过4294967295

	OSTimeDlyResume(OS_TCB * p_tcb, OS_ERR * p_err)
	用来唤醒一个被延时的任务

	OSTimeGet(OS_ERR * p_err)  OSTimeSet(OS_TICK ticks, OS_ERR * p_err)

	OSTimeTick(void)

11. 定时器管理
	OSTmrCreate(OS_TMR * p_tmr, CPU_CHAR * p_name, OS_TICK dly, OS_TICK period, OS_OPT opt, OS_TMR_CALLBACK_PTR p_callback, void * p_callback_arg, OS_ERR * p_err)
		创建定时器并指定其运行模式
		参数1：指向定时器的指针
		参数2：定时器名称 ，ASCII
		参数3：初始延迟值
		参数4：重复周期
		参数5：选项
				OS_OPT_TMR_ONE_SHOT       The timer counts down only once
*               OS_OPT_TMR_PERIODIC       The timer counts down and then reloads itself
				三种模式：单次定时器 无初始延时值的周期定时器 有初始延时值的周期定时器 P171图
				
		参数6：指向回调函数的指针  	void MyCallback(OS_TMR *p_tmr, void *p_arg);
		参数7：回调函数参数
		
	OSTmrDel(OS_TMR * p_tmr, OS_ERR * p_err)
		删除定时器
	OSTmrRemainGet(OS_TMR * p_tmr, OS_ERR * p_err)
		获取定时器的剩余时间
	OSTmrStart(OS_TMR * p_tmr, OS_ERR * p_err)
		启动（或重新启动）定时器计数
	OSTmrStateGet(OS_TMR * p_tmr, OS_ERR * p_err)
		获取当前定时器状态
	OSTmrStop(OS_TMR * p_tmr, OS_OPT opt, void * p_callback_arg, OS_ERR * p_err)
		停止计数器计时

12. 资源管理
	关中断/开中断

	给调度器上锁/开锁

	信号量
		信号量被用做资源共享时，只有任务才能调用其操作函数，
		而中断服务程序不能调用其操作函数。
		当用信号量发信号时，无此限制。

		OSSemCreate(OS_SEM * p_sem, CPU_CHAR * p_name, OS_SEM_CTR cnt, OS_ERR * p_err)
			建立一个信号量
		OSSemDel(OS_SEM * p_sem, OS_OPT opt, OS_ERR * p_err)
			删除一个信号量
		OSSemPend(OS_SEM * p_sem, OS_TICK timeout, OS_OPT opt, CPU_TS * p_ts, OS_ERR * p_err)
			等待一个信号量
			参数1：等待的信号量，该信号量应是已经创建的
			参数2：以时钟节拍为单位的超时时间，设为0表明该等待永远不会超时
			参数3：等待方式
					OS_OPT_PEND_BLOCKING 阻塞型
					OS_OPT_PEND_NON_BLOCKING 非阻塞型
					使用信号量来保护共享资源时，不会用到非阻塞型
		OSSemPendAbort(OS_SEM * p_sem, OS_OPT opt, OS_ERR * p_err)
			取消等待
		OSSemPost(OS_SEM * p_sem, OS_OPT opt, OS_ERR * p_err)
			释放或者发出一个信号量
			参数1：
			参数2：OS_OPT_POST_1 //该信号量仅释放给一个单独的任务（使用信号量访问共享资源必用）
		OSSemSet(OS_SEM * p_sem, OS_SEM_CTR cnt, OS_ERR * p_err)
			强制设置一个信号量的值

	互斥型信号量（MUTEX）（在抢占式内核中）
		互斥量的本质：一个具有高优先级的任务H想要访问共享资源，
					  占有该资源的任务的优先级将被提升至与任务H的一样
		互斥量与信号量一样，只有任务才能使用（中断服务程序则不行）

		OSMutexCreate(OS_MUTEX * p_mutex, CPU_CHAR * p_name, OS_ERR * p_err)
			建立一个互斥型信号量
		OSMutexDel(OS_MUTEX * p_mutex, OS_OPT opt, OS_ERR * p_err)
			删除一个互斥型信号量
		OSMutexPend(OS_MUTEX * p_mutex, OS_TICK timeout, OS_OPT opt, CPU_TS * p_ts, OS_ERR * p_err)
			等待一个互斥型信号量
		OSMutexPendAbort(OS_MUTEX * p_mutex, OS_OPT opt, OS_ERR * p_err)
			取消等待
		OSMutexPost(OS_MUTEX * p_mutex, OS_OPT opt, OS_ERR * p_err)
			释放或者发布一个互斥型信号量

		互斥量可以避免无界优先级反转

	死锁

13. 任务同步
	

		