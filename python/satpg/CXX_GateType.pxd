#
# @file CXX_GateType.pxd
# @brief GateType 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


cdef extern from "GateType.h" namespace "nsYm::nsSatpg" :

    cdef enum GateType :
        kGateCONST0,
        kGateCONST1,
        kGateINPUT,
        kGateBUFF,
        kGateNOT,
        kGateAND,
        kGateNAND,
        kGateOR,
        kGateNOR,
        kGateXOR,
        kGateXNOR,
        kGateCPLX
