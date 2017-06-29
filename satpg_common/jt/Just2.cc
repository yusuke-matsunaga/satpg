
/// @file Just2.cc
/// @brief Just2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Just2.h"
#include "TpgDff.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス Just2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] max_id ID番号の最大値
// @param[in] val_map ノードの値を保持するクラス
Just2::Just2(bool td_mode,
	     ymuint max_id,
	     const ValMap& val_map) :
  JustBase(td_mode, max_id, val_map)
{
}

// @brief デストラクタ
Just2::~Just2()
{
}

// @brief 正当化に必要な割当を求める．
// @param[in] node_list 対象のノードのリスト
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
Just2::operator()(const vector<const TpgNode*>& node_list,
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
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
Just2::justify(const TpgNode* node,
	       int time,
	       NodeValList& pi_assign_list)
{
  if ( justified_mark(node, time) ) {
    return;
  }
  set_justified(node, time);

  if ( node->is_primary_input() ) {
    // val を記録
    record_value(node, time, pi_assign_list);
    return;
  }

  if ( node->is_dff_output() ) {
    if ( time == 1 && td_mode() ) {
      // 1時刻前のタイムフレームに戻る．
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      justify(alt_node, 0, pi_assign_list);
    }
    else {
      // val を記録
      record_value(node, time, pi_assign_list);
      return;
    }
  }

  Val3 gval = this->gval(node, time);

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 無条件で唯一のファンインをたどる．
    just_all(node, time);
    break;

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      just_one(node, kVal0, time);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      just_one(node, kVal0, time);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      just_one(node, kVal1, time);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      just_one(node, kVal1, time);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    just_all(node, time);
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
Just2::just_all(const TpgNode* node,
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
Just2::just_one(const TpgNode* node,
		Val3 val,
		int time,
		NodeValList& pi_assign_list)
{
  ymuint ni = node->fanin_num();
  ymuint pos = ni;
  ymuint min = -1; // 符号なしなら最大の数になる．
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 ival = gval(inode, time);
    if ( ival != val ) {
      continue;
    }
    ymuint n = count_one(node, i, time);
    if ( min > n ) {
      pos = i;
      min = n;
    }
  }
  ASSERT_COND ( pos < ni );

  justify(node->fanin(pos), time, pi_assign_list);
}

// @brief 指定したファンインに対する値の割当数を数える．
// @param[in] node 対象のノード
// @param[in] ipos ファンインの位置番号 ( 0 <= ipos < node->fanin_num() )
// @param[in] time タイムフレーム ( 0 or 1 )
// @return 新規に値の割り当てられるノード数を返す．
ymuint
Just2::count_one(const TpgNode* node,
		 ymuint ipos,
		 int time)
{
  return 0;
}

END_NAMESPACE_YM_SATPG
