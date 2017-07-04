#
# @file CXX_satpg.pxd
# @brief CXX_satpg の cython インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2016 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_cell cimport CellLibrary


cdef extern from "Val3.h" namespace "nsYm::nsSatpg" :

    cdef enum Val3 :
        kValX,
        kVal0,
        kVal1


cdef extern from "TpgNetwork.h" namespace "nsYm::nsSatpg" :

    # TpgNetwork の cython バージョン
    cdef cppclass TpgNetwork :

        TpgNetwork()
        bool read_blif(const string& filename)
        bool read_blif(const string& filename, CellLibrary* cell_library)
        bool read_iscas89(const string& filename)


cdef extern from "TvMgr.h" namespace "nsYm::nsSatpg" :

    # TvMgr の cython バージョン
    cdef cppclass TvMgr :

        TvMgr()
        void clear()
        void init(unsigned int ni)
        TestVector* new_vector()
        void delete_vector(TestVector* tv)


cdef extern from "TestVector.h" namespace "nsYm::nsSatpg" :

    # TestVector の cython ヴァージョン
    cdef cppclass TestVector :

        unsigned int input_num()
        Val3 val3(unsigned int pos)
        unsigned int x_num()
        @staticmethod
        bool is_conflict(const TestVector& tv1, const TestVector& tv2)
        bool operator==(const TestVector& right)
        bool operator<(const TestVector& right)
        bool operator<=(const TestVector& right)
        string bin_str()
