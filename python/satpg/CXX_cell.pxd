#
# @file CXX_cell.pxd
# @brief CXX_cell の cython インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2016 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string

ctypedef unsigned int ymuint


cdef extern from "ym/ym_cell.h" namespace "nsYm" :

    cdef enum CellTechnology :
        kCellTechCmos,
        kCellTechFpga

    cdef enum CellDelayModel :
        kCellDelayGenericCmos,
        kCellDelayTableLookup,
        kCellDelayPiecewiseCmos,
        kCellDelayCmos2,
        kCellDelayDcm

    cdef enum CellPinDirection :
        kCellPinInput,
        kCellPinOutput,
        kCellPinInout,
        kCellPinInternal

    cdef enum CellTimingSense :
        kCellPosiUnate,
        kCellNegaUnate,
        kCellNonUnate

    cdef enum CellTimingType :
        kCellTimingCombinational,
        kCellTimingCombinationalRise,
        kCellTimingCombinationalFall,
        kCellTimingThreeStateEnable,
        kCellTimingThreeStateDisable,
        kCellTimingThreeStateEnableRise,
        kCellTimingThreeStateEnableFall,
        kCellTimingThreeStateDisableRise,
        kCellTimingThreeStateDisableFall,
        kCellTimingRisingEdge,
        kCellTimingFallingEdge,
        kCellTimingPreset,
        kCellTimingClear,
        kCellTimingHoldRising,
        kCellTimingHoldFalling,
        kCellTimingSetupRising,
        kCellTimingSetupFalling,
        kCellTimingRecoveryRising,
        kCellTimingRecoveryFalling,
        kCellTimingSkewRising,
        kCellTimingSkewFalling,
        kCellTimingRemovalRising,
        kCellTimingRemovalFalling,
        kCellTimingNonSeqSetupRising,
        kCellTimingNonSeqSetupFalling,
        kCellTimingNonSeqHoldRising,
        kCellTimingNonSeqHoldFalling,
        kCellTimingNochangeHighHigh,
        kCellTimingNochangeHighLow,
        kCellTimingNochangeLowHigh,
        kCellTimingNochangeLowLow

    cdef enum CellVarType :
        kCellVarInputNetTransition,
        kCellVarTotalOutputNetCapacitance,
        kCellVarOutputNetLength,
        kCellVarOutputNetWireCap,
        kCellVarOutputNetPinCap,
        kCellVarRelatedOutTotalOutputNetCapacitance,
        kCellVarRelatedOutOutputNetLength,
        kCellVarRelatedOutOutputNetWireCap,
        kCellVarRelatedOutOutputNetPinCap,
        kCellVarConstrainedPinTransition,
        kCellVarRelatedPinTransition,
        kCellVarNone

    cdef enum CellPatType :
        kCellPatInput,
        kCellPatAnd,
        kCellPatXor


cdef extern from "ym/CellLibrary.h" namespace "nsYm::nsCell" :

    # CellLibrary の cython バージョン
    cdef cppclass CellLibrary :

        @staticmethod
        CellLibrary* new_obj()
        string name()
        CellTechnology technology()
        CellDelayModel delay_model()
        string bus_naming_style()
        string date()
        string revision()
        string comment()
        string time_unit()
        string volgate_unit()
        string current_unit()
        string pulling_resistance_unit()
        double capacitive_load_unit()
        string capacitive_load_unit_str()
        string leakage_power_unit()
        ymuint lu_table_template_num()
        CellLutTemplate* lu_table_template(ymuint pos)
        CellLutTemplate* lu_table_template(const char* name)
        CellBusType* bus_type(const char* name)
        ymuint cell_num()
        Cell* cell(ymuint pos)
        Cell* cell(const string& name)
        ymuint group_num()
        CellGroup* group(ymuint id)
        ymuint npn_class_num()
        CellClass* npn_class(ymuint id)
        CellGroup* const0_func()
        CellGroup* const1_func()
        CellGroup* buf_func()
        CellGroup* inv_func()
        CellClass* simple_ff_class(bool has_clear, bool has_preset)
        CellClass* simple_latch_class(bool has_clear, bool has_preset)
        ymuint pg_pat_num()
        CellPatGraph& pg_pat(ymuint id)
        ymuint pg_max_input()
        ymuint pg_node_num()
        CellPatType pg_node_type(ymuint id)
        ymuint pg_input_id(ymuint id)
        ymuint pg_input_node(ymuint input_id)
        ymuint pg_edge_num()
        ymuint pg_edge_from(ymuint id)
        ymuint pg_edge_to(ymuint id)
        ymuint pg_edge_pos(ymuint id)
        bool pg_edge_inv(ymuint id)


cdef extern from "ym/CellMislibReader.h" namespace "nsYm::nsCell" :

    # CellMislibReader の cython バージョン
    cdef cppclass CellMislibReader :

        CellMislibReader()
        bool read(const string& filename, CellLibrary* library)


cdef extern from "ym/CellDotlibReader.h" namespace "nsYm::nsCell" :

    # CellDotlibReader の cython バージョン
    cdef cppclass CellDotlibReader :

        CellDotlibReader()
        bool read(const string& filename, CellLibrary* library)


cdef extern from "ym/Cell.h" namespace "nsYm::nsCell" :

    # Cell の cython バージョン
    cdef cppclass Cell :

        Cell()
        ymuint id()
        string name()
        ymuint pin_num()


cdef extern from "ym/CellBusType.h" namespace "nsYm::nsCell" :

    # CellBusType の cython バージョン
    cdef cppclass CellBusType :

        CellBusType()
        string name()


cdef extern from "ym/CellClass.h" namespace "nsYm::nsCell" :

    # CellClass の cython バージョン
    cdef cppclass CellClass :

        CellClass()
        ymuint id()


cdef extern from "ym/CellGroup.h" namespace "nsYm::nsCell" :

    # CellGroup の cython バージョン
    cdef cppclass CellGroup :

        CellGroup()
        ymuint id()


cdef extern from "ym/CellLutTemplate.h" namespace "nsYm::nsCell" :

    # CellLutTemplate の cython バージョン
    cdef cppclass CellLutTemplate :

        const char* name()
        ymuint dimension()
        CellVarType variable_type(ymuint var)
        ymuint index_num(ymuint var)
        double index(ymuint var, ymuint pos)


cdef extern from "ym/CellPatGraph.h" namespace "nsYm::nsCell" :

    # CellPatGraph の cython バージョン
    cdef cppclass CellPatGraph :

        CellPatGraph()
        ymuint rep_id()
