
### @file testvector.pxi
### @brief testvector の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017 Yusuke Matsunaga
### All rights reserved.

from CXX_TestVector cimport TestVector as CXX_TestVector
from CXX_FaultType cimport FaultType as CXX_FaultType


### @brief TestVector の Python バージョン
cdef class TestVector :
    cdef CXX_TestVector _this

    ### @brief 初期化
    def __init__(TestVector self, int input_num, int dff_num, fault_type) :
        cdef CXX_FaultType c_fault_type = from_FaultType(fault_type)
        self._this.resize(input_num, dff_num, c_fault_type)

    ### @brief 入力数を返す．
    @property
    def input_num(TestVector self) :
        return self._this.input_num()

    ### @brief DFF数を返す．
    @property
    def dff_num(TestVector self) :
        return self._this.dff_num()

    ### @brief 疑似入力数を返す．
    @property
    def ppi_num(TestVector self) :
        return self._this.ppi_num()

    ### @brief 故障の種類を返す．
    @property
    def fault_type(TestVector self) :
        return to_FaultType(self._this.fault_type())

    ### @brief 疑似入力番号から対応した値を返す．
    ### @param[in] pos 位置番号 ( 0 <= pos < ppi_num )
    ###
    ### 縮退モードの時に用いられる．
    def ppi_val(TestVector self, pos) :
        return to_Val3(self._this.ppi_val(pos))

    ### @brief 入力番号から対応した値を返す．
    ### @param[in] pos 位置番号 ( 0 <= pos < input_num() )
    ###
    ### 遷移故障モードの時に用いられる．
    def input_val(TestVector self, pos) :
        return to_Val3(self._this.input_val(pos))

    ### @brief DFF番号から対応した値を返す．
    ### @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
    ###
    ### 遷移故障モードの時に用いられる．
    def dff_val(TestVector self, pos) :
        return to_Val3(self._this.dff_val(pos))

    ### @brief 入力番号から対応した２時刻目の値を返す．
    ### @param[in] pos 位置番号 ( 0 <= pos < input_num() )
    ###
    ### 遷移故障モードの時に用いられる．
    def aux_input_val(TestVector self, pos) :
        return to_Val3(self._this.aux_input_val(pos))

    ### @brief X値の個数を返す．
    @property
    def x_count(TestVector self) :
        return self._this.x_count()

    ### @brief 2進表記の文字列を返す．
    @property
    def bin_str(TestVector self) :
        cdef string c_str = self._this.bin_str()
        return c_str.decode('UTF-8')

    ### @brief 16進表記の文字列を返す．
    @property
    def hex_str(TestVector self) :
        cdef string c_str = self._this.hex_str()
        return c_str.decode('UTF-8')
