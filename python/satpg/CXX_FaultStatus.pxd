#
# @file CXX_FaultStatus.pxd
# @brief FaultStatus 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


cdef extern from "FaultStatus.h" namespace "nsYm::nsSatpg" :

    cdef enum FaultStatus :
        kFsUndetected
        kFsDetected
        kFsUntestable
        kFsAborted
