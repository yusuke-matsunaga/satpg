#
# @file faultstatus.pxi
# @brief FaultStatus の Python インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


## @brief FaultStatus の Python バージョン
cdef class FaultStatue(enum) :
    Undetected = 0
    Detected   = 1
    Untestable = 2
    Aborted    = 3

    ## @brief 内容を表す文字列を返す．
    def __str__(FaultStatus self) :
        if self == FaultStatus.Undetected :
            return 'undetected'
        if self == FaultStatus.Detected :
            return 'detected'
        if self == FaultStatus.Untestable :
            return 'untestable'
        if self == FaultStatus.Aborted :
            return 'aborted'
        assert False
