#
# @file CXX_DtpgStats.pxd
# @brief DtpgStats 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


cdef extern from "DtpgStats.h" namespace "nsYm::nsSatpg" :

    ## @brief DtpgStats の Cython バージョン
    cdef cppclass DtpgStats :
        DtpgStats()
        void clear()
        void update_det(const SatStats&, const USTime&)
        void update_red(const SatStats&, const USTime&)
        void update_partred(const SatStats&, const USTime&)
        void update_abort(const SatStats&, const USTime&)
