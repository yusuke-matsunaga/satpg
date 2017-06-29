
/// @file Just2.cc
/// @brief Just2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Just2.h"
#include "TpgDff.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief Just2 を生成する．
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] max_id ID番号の最大値
// @param[in] val_map ノードの値を保持するクラス
Justifier*
new_Just2(bool td_mode,
	  ymuint max_id,
	  const ValMap& val_map)
{
  return new Just2(td_mode, max_id, val_map);
}

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
  JustBase(td_mode, max_id, val_map),
  mWeightArray(max_id * 2, 0U),
  mTmpArray(max_id * 2, 0.0)
{
  mNodeList[0].reserve(max_id);
  mNodeList[1].reserve(max_id);
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

  mNodeList[0].clear();
  mNodeList[1].clear();
  for (vector<const TpgNode*>::const_iterator p = node_list.begin();
       p != node_list.end(); ++ p) {
    const TpgNode* node = *p;
    add_weight(node, 1);
  }

  for (int time = 0; time < 2; ++ time) {
    for (vector<const TpgNode*>::const_iterator p = mNodeList[time].begin();
	 p != mNodeList[time].end(); ++ p) {
      const TpgNode* node = *p;
      calc_value(node, time);
    }
  }

  for (vector<const TpgNode*>::const_iterator p = node_list.begin();
       p != node_list.end(); ++ p) {
    const TpgNode* node = *p;
    justify(node, 1, pi_assign_list);
  }

  for (int time = 0; time < 2; ++ time) {
    for (vector<const TpgNode*>::const_iterator p = mNodeList[time].begin();
	 p != mNodeList[time].end(); ++ p) {
      const TpgNode* node = *p;
      ymuint index = node->id() * 2 + time;
      mWeightArray[index] = 0;
      mTmpArray[index] = 0.0;
    }
    mNodeList[time].clear();
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
    just_all(node, time, pi_assign_list);
    break;

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, pi_assign_list);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      just_one(node, kVal0, time, pi_assign_list);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      just_one(node, kVal0, time, pi_assign_list);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, pi_assign_list);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      just_one(node, kVal1, time, pi_assign_list);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, pi_assign_list);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, pi_assign_list);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      just_one(node, kVal1, time, pi_assign_list);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    just_all(node, time, pi_assign_list);
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
  double min_val = DBL_MAX;
  const TpgNode* min_node = nullptr;
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 ival = gval(inode, time);
    if ( ival != val ) {
      continue;
    }
    double val = node_value(inode, time);
    if ( min_val > val ) {
      min_val = val;
      min_node = inode;
    }
  }
  ASSERT_COND ( min_node != nullptr );

  justify(min_node, time, pi_assign_list);
}

// @brief 重みの計算を行う．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
void
Just2::add_weight(const TpgNode* node,
		  int time)
{
  ymuint index = node->id() * 2 + time;

  ++ mWeightArray[index];
  if ( mWeightArray[index] > 1 ) {
    return;
  }

  if ( node->is_primary_input() ) {
    ;
  }
  else if ( node->is_dff_output() ) {
    if ( time == 1 && td_mode() ) {
      // 1時刻前のタイムフレームに戻る．
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      add_weight(alt_node, 0);
    }
  }
  else {
    Val3 gval = this->gval(node, time);

    switch ( node->gate_type() ) {
    case kGateBUFF:
    case kGateNOT:
      // 唯一のファンインノードをたどる．
      aw_all(node, time);
      break;

    case kGateAND:
      if ( gval == kVal1 ) {
	// すべてのファンインノードをたどる．
	aw_all(node, time);
      }
      else if ( gval == kVal0 ) {
	// 0の値を持つノードをたどる．
	aw_one(node, kVal0, time);
      }
      break;

    case kGateNAND:
      if ( gval == kVal1 ) {
	// 0の値を持つノードをたどる．
	aw_one(node, kVal0, time);
      }
      else if ( gval == kVal0 ) {
	// すべてのファンインノードをたどる．
	aw_all(node, time);
      }
      break;

    case kGateOR:
      if ( gval == kVal1 ) {
	// 1の値を持つノードをたどる．
	aw_one(node, kVal1, time);
      }
      else if ( gval == kVal0 ) {
	// すべてのファンインノードをたどる．
	aw_all(node, time);
      }
      break;

    case kGateNOR:
      if ( gval == kVal1 ) {
	// すべてのファンインノードをたどる．
	aw_all(node, time);
      }
      else if ( gval == kVal0 ) {
	// 1の値を持つノードをたどる．
	aw_one(node, kVal1, time);
      }
      break;

    case kGateXOR:
    case kGateXNOR:
      // すべてのファンインノードをたどる．
      aw_all(node, time);
      break;

    default:
      ASSERT_NOT_REACHED;
      break;
    }
  }

  // post order で mNodeList に入れる．
  mNodeList[time].push_back(node);
}

// @brief すべてのファンインに対して add_weight() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
void
Just2::aw_all(const TpgNode* node,
	      int time)
{
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    add_weight(inode, time);
  }
}

// @brief 指定した値を持つファンインに対して add_weight() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] ipos ファンインの位置番号 ( 0 <= ipos < node->fanin_num() )
// @param[in] time タイムフレーム ( 0 or 1 )
void
Just2::aw_one(const TpgNode* node,
	      Val3 val,
	      int time)
{
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 ival = gval(inode, time);
    if ( ival == val ) {
      add_weight(inode, time);
    }
  }
}

// @brief 見積もり値の計算を行う．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
void
Just2::calc_value(const TpgNode* node,
		  int time)
{
  ymuint index = node->id() * 2 + time;

  if ( node->is_primary_input() ) {
    mTmpArray[index] = 1.0;
    return;
  }

  if ( node->is_dff_output() ) {
    if ( time == 1 && td_mode() ) {
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      mTmpArray[index] = node_value(alt_node, 0);
    }
    else {
      mTmpArray[index] = 1.0;
    }
    return;
  }

  Val3 gval = this->gval(node, time);

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 唯一のファンインノードをたどる．
    cv_all(node, time);
    break;

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      cv_all(node, time);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つノードをたどる．
      cv_one(node, kVal0, time);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つノードをたどる．
      cv_one(node, kVal0, time);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      cv_all(node, time);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つノードをたどる．
      cv_one(node, kVal1, time);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      cv_all(node, time);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      cv_all(node, time);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つノードをたどる．
      cv_one(node, kVal1, time);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    cv_all(node, time);
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief すべてのファンインに対して calc_value() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
void
Just2::cv_all(const TpgNode* node,
	      int time)
{
  double val = 0.0;
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    calc_value(inode, time);
    val += node_value(inode, time);
  }
  mTmpArray[node->id() * 2 + time] = val;
}

// @brief 指定した値を持つファンインに対して calc_value() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] ipos ファンインの位置番号 ( 0 <= ipos < node->fanin_num() )
// @param[in] time タイムフレーム ( 0 or 1 )
void
Just2::cv_one(const TpgNode* node,
	      Val3 val,
	      int time)
{
  double min_val = DBL_MAX;
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 ival = gval(inode, time);
    if ( ival != val ) {
      continue;
    }
    calc_value(inode, time);
    double val1 = node_value(inode, time);
    if ( min_val > val1 ) {
      min_val = val1;
    }
  }
  mTmpArray[node->id() * 2 + time] = min_val;
}

// @brief 重みを考えた価値を返す．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
double
Just2::node_value(const TpgNode* node,
		  int time) const
{
  ymuint index = node->id() * 2 + time;
  return mTmpArray[index] / mWeightArray[index];
}

END_NAMESPACE_YM_SATPG
