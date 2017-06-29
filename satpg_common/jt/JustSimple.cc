
/// @file JustSimple.cc
/// @brief JustSimple の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "JustSimple.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス JustSimple
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] max_id ID番号の最大値
// @param[in] val_map ノードの値を保持するクラス
JustSimple::JustSimple(bool td_mode,
		       ymuint max_id,
		       const ValMap& val_map) :
  JustBase(td_mode, max_id, val_map)
{
}

// @brief デストラクタ
JustSimple::~JustSimple()
{
}

// @brief 正当化に必要な割当を求める．
// @param[in] node_list 対象のノードのリスト
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
JustSimple::operator()(const vector<const TpgNode*>& node_list,
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
JustSimple::justify(const TpgNode* node,
		    int time,
		    NodeValList& pi_assign_list)
{
  if ( justified_mark(node, time) ) {
    // 処理済みならなにもしない．
    return;
  }
  // 処理済みの印を付ける．
  set_justified(node, time);

  if ( node->is_primary_input() ) {
    // 外部入力なら値を記録する．
    record_value(node, time, pi_assign_list);
  }
  else if ( node->is_dff_output() ) {
    if ( time == 1 && td_mode() ) {
      // DFF の出力で1時刻目の場合は0時刻目に戻る．
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      justify(alt_node, 0, pi_assign_list);
    }
    else {
      // DFFを擬似入力だと思って値を記録する．
      record_value(node, time, pi_assign_list);
    }
  }
  else {
    // すべてのファンインに再帰する．
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      justify(inode, time, pi_assign_list);
    }
  }
}

END_NAMESPACE_YM_SATPG
