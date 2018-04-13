#
# @file CXX_Dtpg.pxd
# @brief Dtpg 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


from libcpp cimport bool
from libcpp.string cimport string
from CXX_BackTracer cimport BackTracer
from CXX_DtpgStats cimport DtpgStats
from CXX_TpgNetwork cimport TpgNetwork
from CXX_TpgFault cimport TpgFault
from CXX_TpgFFR cimport TpgFFR
from CXX_TpgMFFC cimport TpgMFFC


cdef exterm from "Dtpg.h" namespace "nsYm::nsSatpg" :

    ## @brief Dtpg の Cython バージョン
    cdef cppclass Dtpg :
        Dtpg(const string&, const string&, ostream*, bool, BackTracer&)
        void gen_ffr_cnf(const TpgNetwork&, const TpgFFR*, DtpgStats&)
        void gen_mffc_cnf(const TpgNetwork&, const TpgMFFC*, DtpgStats&)
        SatBool3 dtpg(const TpgFault*, NodeValList&, DtpgStats&)
