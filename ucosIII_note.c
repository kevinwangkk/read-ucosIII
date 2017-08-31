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

