
/// @file BtImpl.cc
/// @brief BtImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BtImpl.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス BtImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ノード番号の最大値
// @param[in] fault_type 故障の型
// @param[in] val_map ノードの値を保持するクラス
BtImpl::BtImpl(ymuint max_id,
	       FaultType fault_type,
	       const ValMap& val_map) :
  mFaultType(fault_type),
  mMarkArray(max_id, 0U),
  mValMap(val_map)
{
}

// @brief デストラクタ
BtImpl::~BtImpl()
{
}

END_NAMESPACE_YM_SATPG
