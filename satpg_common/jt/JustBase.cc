
/// @file JustBase.cc
/// @brief JustBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "JustBase.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス JustBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] max_id ID番号の最大値
// @param[in] val_map ノードの値を保持するクラス
JustBase::JustBase(bool td_mode,
		   ymuint max_id,
		   const ValMap& val_map) :
  mTdMode(td_mode),
  mMaxId(max_id),
  mValMap(val_map),
  mMarkArray(max_id, 0U)
{
}

// @brief デストラクタ
JustBase::~JustBase()
{
}

END_NAMESPACE_YM_SATPG
