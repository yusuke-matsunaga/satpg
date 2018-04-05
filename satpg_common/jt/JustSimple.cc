
/// @file JustSimple.cc
/// @brief JustSimple の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "JustSimple.h"
#include "JustData.h"
#include "TpgDff.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief JustSimple を生成する．
// @param[in] max_id ID番号の最大値
Justifier*
new_JustSimple(int max_id)
{
  return new JustSimple(max_id);
}


//////////////////////////////////////////////////////////////////////
// クラス JustSimple
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ID番号の最大値
JustSimple::JustSimple(int max_id) :
  JustBase(max_id)
{
}

// @brief デストラクタ
JustSimple::~JustSimple()
{
}

// @brief 正当化に必要な割当を求める(縮退故障用)．
// @param[in] assign_list 値の割り当てリスト
// @param[in] var_map 変数番号のマップ
// @param[in] model SAT問題の解
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
JustSimple::operator()(const NodeValList& assign_list,
		       const VidMap& var_map,
		       const vector<SatBool3>& model,
		       NodeValList& pi_assign_list)
{
  pi_assign_list.clear();
  clear_mark();

  JustData jd(var_map, model);

  for ( auto nv: assign_list ) {
    const TpgNode* node = nv.node();
    int time = nv.time();
    justify(jd, node, time, pi_assign_list);
  }
}

// @brief 正当化に必要な割当を求める(遷移故障用)．
// @param[in] assign_list 値の割り当てリスト
// @param[in] var1_map 1時刻目の変数番号のマップ
// @param[in] var2_map 2時刻目の変数番号のマップ
// @param[in] model SAT問題の解
/// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
JustSimple::operator()(const NodeValList& assign_list,
		       const VidMap& var1_map,
		       const VidMap& var2_map,
		       const vector<SatBool3>& model,
		       NodeValList& pi_assign_list)
{
  pi_assign_list.clear();
  clear_mark();

  JustData jd(var1_map, var2_map, model);

  for ( auto nv: assign_list ) {
    const TpgNode* node = nv.node();
    int time = nv.time();
    justify(jd, node, time, pi_assign_list);
  }
}

// @brief 正当化に必要な割当を求める．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
JustSimple::justify(const JustData& jd,
		    const TpgNode* node,
		    int time,
		    NodeValList& pi_assign_list)
{
  if ( mark(node, time) ) {
    // 処理済みならなにもしない．
    return;
  }
  // 処理済みの印を付ける．
  set_mark(node, time);

  if ( node->is_primary_input() ) {
    // 外部入力なら値を記録する．
    jd.record_value(node, time, pi_assign_list);
  }
  else if ( node->is_dff_output() ) {
    if ( time == 1 && jd.td_mode() ) {
      // DFF の出力で1時刻目の場合は0時刻目に戻る．
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      justify(jd, alt_node, 0, pi_assign_list);
    }
    else {
      // DFFを擬似入力だと思って値を記録する．
      jd.record_value(node, time, pi_assign_list);
    }
  }
  else {
    // すべてのファンインに再帰する．
    for ( auto inode: node->fanin_list() ) {
      justify(jd, inode, time, pi_assign_list);
    }
  }
}

END_NAMESPACE_YM_SATPG
