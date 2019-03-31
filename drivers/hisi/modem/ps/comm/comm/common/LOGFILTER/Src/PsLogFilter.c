/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : PsLogFilter.c
  版 本 号   : 初稿
  作    者   : h00313353
  生成日期   : 2015年9月28日
  最近修改   :
  功能描述   : Log过滤接口文件
  函数列表   :
  修改历史   :
  1.日    期   : 2015年9月28日
    作    者   : h00313353
    修改内容   : 创建文件

******************************************************************************/

/******************************************************************************
   1 头文件包含
******************************************************************************/
#include "PsLogFilter.h"


#define    THIS_FILE_ID        PS_FILE_ID_LOG_FILTER_C

/******************************************************************************
   2 外部函数变量声明
******************************************************************************/
extern VOS_UINT32 GUNAS_OM_LayerMsgFilter(
    const VOS_VOID                     *pstMsg
);

extern VOS_UINT32 PAM_OM_LayerMsgFilter(
    const VOS_VOID                      *pMsg
);

extern VOS_UINT32 TLPS_OM_LayerMsgFilter(
    const VOS_VOID                      *pMsg
);

/******************************************************************************
   3 私有定义
******************************************************************************/


/******************************************************************************
   4 全局变量定义
******************************************************************************/


/******************************************************************************
   5 函数实现
******************************************************************************/
/* C核过滤函数 */

/* A核过滤函数 */
/*****************************************************************************
 函 数 名  : PS_OM_LayerMsgFilter_Acpu
 功能描述  : 用于向DIAG组件进行注册的公共的LOG过滤函数
             其他组件可将自己的过滤函数添加至本函数内，
             统一向DIAG组件进行注册
             (本函数在A核任务中注册，用于过滤A核内部层间消息)
 输入参数  : const VOID *pMsg:消息指针
 输出参数  : 无
 返 回 值  : VOS_UINT32:
                返回VOS_TRUE:  表示该消息需要进行过滤
                返回VOS_FALSE: 表示该消息无需进行过滤
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2015年09月28日
    作    者   : h00313353
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 PS_OM_LayerMsgFilter_Acpu(
    const VOS_VOID                      *pMsg
)
{
    /* GUNAS */
    if (VOS_TRUE == GUNAS_OM_LayerMsgFilter(pMsg))
    {
        return VOS_TRUE;
    }

    if (VOS_TRUE == PAM_OM_LayerMsgFilter(pMsg))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}

