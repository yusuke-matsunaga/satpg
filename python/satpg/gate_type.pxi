#
# @file gate_type.pxi
# @brief GateType の Python インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from enum import Enum
from CXX_GateType cimport GateType as CXX_GateType
from CXX_GateType cimport kGateCONST0
from CXX_GateType cimport kGateCONST1
from CXX_GateType cimport kGateINPUT
from CXX_GateType cimport kGateBUFF
from CXX_GateType cimport kGateNOT
from CXX_GateType cimport kGateAND
from CXX_GateType cimport kGateNAND
from CXX_GateType cimport kGateOR
from CXX_GateType cimport kGateNOR
from CXX_GateType cimport kGateXOR
from CXX_GateType cimport kGateXNOR
from CXX_GateType cimport kGateCPLX


## @brief GateType の Python バージョン
class GateType(Enum) :
    CONST0 = 0
    CONST1 = 1
    INPUT  = 2
    BUFF   = 3
    NOT    = 4
    AND    = 5
    NAND   = 6
    OR     = 7
    NOR    = 8
    XOR    = 9
    XNOR   = 10
    CPLX   = 11


## @brief C++ の GateType を Python の GateType に変換する．
def to_GateType(CXX_GateType c_val) :
    if c_val == kGateCONST0 :
        return GateType.CONST0
    elif c_val == kGateCONST1 :
        return GateType.CONST1
    elif c_val == kGateINPUT :
        return GateType.INPUT
    elif c_val == kGateBUFF :
        return GateType.BUFF
    elif c_val == kGateNOT :
        return GateType.NOT
    elif c_val == kGateAND :
        return GateType.AND
    elif c_val == kGateNAND :
        return GateType.NAND
    elif c_val == kGateOR :
        return GateType.OR
    elif c_val == kGateNOR :
        return GateType.NOR
    elif c_val == kGateXOR :
        return GateType.XOR
    elif c_val == kGateXNOR :
        return GateType.XNOR
    elif c_val == kGateCPLX :
        return GateType.CPLX


## @brief Python の GateType を C++ の GateType に変換する．
cdef CXX_GateType from_GateType(val) :
    if val == GateType.CONST0 :
        return kGateCONST0
    elif val == GateType.CONST1 :
        return kGateCONST1
    elif val == GateType.INPUT :
        return kGateINPUT
    elif val == GateType.BUFF :
        return kGateBUFF
    elif val == GateType.NOT :
        return kGateNOT
    elif val == GateType.AND :
        return kGateAND
    elif val == GateType.NAND :
        return kGateNAND
    elif val == GateType.OR :
        return kGateOR
    elif val == GateType.NOR :
        return kGateNOR
    elif val == GateType.XOR :
        return kGateXOR
    elif val == GateType.XNOR :
        return kGateXNOR
    elif val == GateType.CPLX :
        return kGateCPLX
