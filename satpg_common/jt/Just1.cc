
/// @file Just1.cc
/// @brief Just1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Just1.h"
#include "JustData.h"
#include "TpgDff.h"
#include "GateType.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

int debug = 0;

END_NONAMESPACE

// @brief Just1 を生成する．
// @param[in] max_id ID番号の最大値
Justifier*
new_Just1(int max_id)
{
  return new Just1(max_id);
}


//////////////////////////////////////////////////////////////////////
// クラス Just1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ノード番号の最大値
Just1::Just1(int max_id) :
  JustBase(max_id)
{
}

// @brief デストラクタ
Just1::~Just1()
{
}

// @brief 正当化に必要な割当を求める．
// @param[in] assign_list 値の割り当てリスト
// @param[in] val_map ノードの値を保持するクラス
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
Just1::operator()(const NodeValList& assign_list,
		  const VidMap& gvar_map,
		  const vector<SatBool3>& model,
		  NodeValList& pi_assign_list)
{
  pi_assign_list.clear();
  clear_mark();

  JustData jd(gvar_map, model);

  for ( auto nv: assign_list ) {
    const TpgNode* node = nv.node();
    int time = nv.time();
    justify(jd, node, time, pi_assign_list);
  }
}

// @brief 正当化に必要な割当を求める．
// @param[in] assign_list 値の割り当てリスト
// @param[in] val_map ノードの値を保持するクラス
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
Just1::operator()(const NodeValList& assign_list,
		  const VidMap& gvar_map,
		  const VidMap& hvar_map,
		  const vector<SatBool3>& model,
		  NodeValList& pi_assign_list)
{
  pi_assign_list.clear();
  clear_mark();

  JustData jd(gvar_map, hvar_map, model);

  for ( auto nv: assign_list ) {
    const TpgNode* node = nv.node();
    int time = nv.time();
    justify(jd, node, time, pi_assign_list);
  }
}

// @brief 正当化に必要な割当を求める．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
// @param[out] assign_list 外部入力上の値の割当リスト
void
Just1::justify(const JustData& jd,
	       const TpgNode* node,
	       int time,
	       NodeValList& assign_list)
{
  if ( mark(node, time) ) {
    return;
  }
  set_mark(node, time);

  if ( debug ) {
    cout << "justify(Node#" << node->id() << "@" << time
	 << " = " << jd.val(node, time) << ")" << endl;
  }

  if ( node->is_primary_input() ) {
    // val を記録
    jd.record_value(node, time, assign_list);
    return;
  }
  else if ( node->is_dff_output() ) {
    if ( time == 1 && jd.td_mode() ) {
      // 1時刻前のタイムフレームに戻る．
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      justify(jd, alt_node, 0, assign_list);
    }
    else {
      // val を記録
      jd.record_value(node, time, assign_list);
    }
    return;
  }

  Val3 oval = jd.val(node, time);

  switch ( node->gate_type() ) {
  case GateType::Buff:
  case GateType::Not:
    // 無条件で唯一のファンインをたどる．
    justify(jd, node->fanin_list()[0], time, assign_list);
    break;

  case GateType::And:
    if ( oval == Val3::_1 ) {
      // すべてのファンインノードをたどる．
      just_all(jd, node, time, assign_list);
    }
    else if ( oval == Val3::_0 ) {
      // 0の値を持つ最初のノードをたどる．
      just_one(jd, node, Val3::_0, time, assign_list);
    }
    break;

  case GateType::Nand:
    if ( oval == Val3::_1 ) {
      // 0の値を持つ最初のノードをたどる．
      just_one(jd, node, Val3::_0, time, assign_list);
    }
    else if ( oval == Val3::_0 ) {
      // すべてのファンインノードをたどる．
      just_all(jd, node, time, assign_list);
    }
    break;

  case GateType::Or:
    if ( oval == Val3::_1 ) {
      // 1の値を持つ最初のノードをたどる．
      just_one(jd, node, Val3::_1, time, assign_list);
    }
    else if ( oval == Val3::_0 ) {
      // すべてのファンインノードをたどる．
      just_all(jd, node, time, assign_list);
    }
    break;

  case GateType::Nor:
    if ( oval == Val3::_1 ) {
      // すべてのファンインノードをたどる．
      just_all(jd, node, time, assign_list);
    }
    else if ( oval == Val3::_0 ) {
      // 1の値を持つ最初のノードをたどる．
      just_one(jd, node, Val3::_1, time, assign_list);
    }
    break;

  case GateType::Xor:
  case GateType::Xnor:
    // すべてのファンインノードをたどる．
    just_all(jd, node, time, assign_list);
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
Just1::just_all(const JustData& jd,
		const TpgNode* node,
		int time,
		NodeValList& pi_assign_list)
{
  if ( debug ) {
    cout << "just_all(Node#" << node->id() << "@" << time
	 << " = " << jd.val(node, time) << ")" << endl;
  }

  for ( auto inode: node->fanin_list() ) {
    justify(jd, inode, time, pi_assign_list);
  }
}

// @brief 指定した値を持つファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val 値
// @param[in] time タイムフレーム ( 0 or 1 )
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
Just1::just_one(const JustData& jd,
		const TpgNode* node,
		Val3 val,
		int time,
		NodeValList& pi_assign_list)
{
  if ( debug ) {
    cout << "just_one(Node#" << node->id() << "@" << time
	 << " = " << jd.val(node, time) << ")" << endl;
  }

  for ( auto inode: node->fanin_list() ) {
    Val3 ival = jd.val(inode, time);
    if ( ival == val ) {
      justify(jd, inode, time, pi_assign_list);
      break;
    }
  }
}

END_NAMESPACE_YM_SATPG
