
/// @file TestVector.cc
/// @brief TestVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TestVector.h"
#include "TpgNode.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief 割当リストからTestVectorを作るクラスメソッド
// @param[in] input_num 入力数
// @param[in] dff_numr DFF数
// @param[in] fault_type 故障の種類
// @param[in] assign_list 割当リスト
// @return assign_list から変換したテストベクタ
//
// assign_list に外部入力とDFF以外の割当が含まれている場合無視する．
TestVector
TestVector::new_from_assign_list(int input_num,
				 int dff_num,
				 FaultType fault_type,
				 const NodeValList& assign_list)
{
  TestVector tv(input_num, dff_num, fault_type);

  for ( auto nv: assign_list ) {
    const TpgNode* node = nv.node();
    ASSERT_COND( node->is_ppi() );

    Val3 val = nv.val() ? Val3::_1 : Val3::_0;

    if ( fault_type == FaultType::StuckAt ) {
      tv.set_ppi_val(node->input_id(), val);
    }
    else if ( fault_type == FaultType::TransitionDelay ) {
      int time = nv.time();
      if ( time == 0 ) {
	tv.set_ppi_val(node->input_id(), val);
      }
      else {
	ASSERT_COND( node->is_primary_input() );

	tv.set_aux_input_val(node->input_id(), val);
      }
    }
  }

  return tv;
}

END_NAMESPACE_YM_SATPG
