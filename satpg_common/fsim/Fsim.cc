
/// @file Fsim.cc
/// @brief Fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Fsim.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// Fsim の実装コード
//////////////////////////////////////////////////////////////////////

// @brief 複数の故障にスキップマークをつける．
// @param[in] fault_list 故障のリスト
//
// fault_list に含まれない故障のスキップマークは消される．
void
Fsim::set_skip(const vector<const TpgFault*>& fault_list)
{
  clear_skip_all();
  for (ymuint i = 0; i < fault_list.size(); ++ i) {
    const TpgFault* f = fault_list[i];
    set_skip(f);
  }
}

// @brief 複数の故障のスキップマークを消す．
// @param[in] fault_list 故障のリスト
//
// fault_list に含まれない故障のスキップマークは付けられる．
void
Fsim::clear_skip(const vector<const TpgFault*>& fault_list)
{
  set_skip_all();
  for (ymuint i = 0; i < fault_list.size(); ++ i) {
    const TpgFault* f = fault_list[i];
    clear_skip(f);
  }
}

END_NAMESPACE_YM_SATPG
