
/// @file Just1.cc
/// @brief Just1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Just1.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief Just1 を生成する．
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] max_id ID番号の最大値
// @param[in] val_map ノードの値を保持するクラス
Justifier*
new_Just1(bool td_mode,
	  ymuint max_id,
	  const ValMap& val_map)
{
  return new Just1(td_mode, max_id, val_map);
}

//////////////////////////////////////////////////////////////////////
// クラス Just1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] max_id ID番号の最大値
// @param[in] val_map ノードの値を保持するクラス
Just1::Just1(bool td_mode,
	     ymuint max_id,
	     const ValMap& val_map) :
  JustBase(td_mode, max_id, val_map)
{
}

// @brief デストラクタ
Just1::~Just1()
{
}

// @brief 正当化に必要な割当を求める．
// @param[in] node_list 対象のノードのリスト
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
Just1::operator()(const vector<const TpgNode*>& node_list,
		  NodeValList& pi_assign_list)
{
  pi_assign_list.clear();

  for (vector<const TpgNode*>::const_iterator p = node_list.begin();
       p != node_list.end(); ++ p) {
    const TpgNode* node = *p;
    justify(node, 1, pi_assign_list);
  }
}

// @brief 正当化に必要な割当を求める．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
// @param[out] assign_list 外部入力上の値の割当リスト
void
Just1::justify(const TpgNode* node,
	       int time,
	       NodeValList& assign_list)
{
  if ( justified_mark(node, time) ) {
    return;
  }
  set_justified(node, time);

  if ( node->is_primary_input() ) {
    // val を記録
    record_value(node, time, assign_list);
  }
  else if ( node->is_dff_output() ) {
    if ( time == 1 && td_mode() ) {
      // 1時刻前のタイムフレームに戻る．
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      justify(alt_node, 0, assign_list);
    }
    else {
      // val を記録
      record_value(node, time, assign_list);
    }
  }

  Val3 gval = this->gval(node, time);

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 無条件で唯一のファンインをたどる．
    justify(node->fanin(0), time, assign_list);
    break;

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, assign_list);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      just_one(node, kVal0, time, assign_list);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      just_one(node, kVal0, time, assign_list);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, assign_list);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      just_one(node, kVal1, time, assign_list);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, assign_list);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, assign_list);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      just_one(node, kVal1, time, assign_list);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    just_all(node, time, assign_list);
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief すべてのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
Just1::just_all(const TpgNode* node,
		int time,
		NodeValList& pi_assign_list)
{
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    justify(inode, time, pi_assign_list);
  }

}

// @brief 指定した値を持つファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val 値
// @param[in] time タイムフレーム ( 0 or 1 )
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
Just1::just_one(const TpgNode* node,
		Val3 val,
		int time,
		NodeValList& pi_assign_list)
{
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 ival = gval(inode, time);
    if ( ival == val ) {
      justify(inode, time, pi_assign_list);
      break;
    }
  }
}

END_NAMESPACE_YM_SATPG
