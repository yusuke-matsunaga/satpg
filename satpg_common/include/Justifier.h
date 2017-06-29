#ifndef JUSTIFIER_H
#define JUSTIFIER_H

/// @file Justifier.h
/// @brief Justifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class Justifier Justifier.h "td/Justifier.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class Justifier
{
public:

  /// @brief デストラクタ
  virtual
  ~Justifier() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める．
  /// @param[in] node_list 対象のノードのリスト
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  virtual
  void
  operator()(const vector<const TpgNode*>& node_list,
	     NodeValList& pi_assign_list) = 0;

};

/// @brief JustSimple を生成する．
/// @param[in] td_mode 遷移故障モードの時 true にするフラグ
/// @param[in] max_id ID番号の最大値
/// @param[in] val_map ノードの値を保持するクラス
extern
Justifier*
new_JustSimple(bool td_mode,
	       ymuint max_id,
	       const ValMap& val_map);

/// @brief Just1 を生成する．
/// @param[in] td_mode 遷移故障モードの時 true にするフラグ
/// @param[in] max_id ID番号の最大値
/// @param[in] val_map ノードの値を保持するクラス
extern
Justifier*
new_Just1(bool td_mode,
	  ymuint max_id,
	  const ValMap& val_map);

/// @brief Just2 を生成する．
/// @param[in] td_mode 遷移故障モードの時 true にするフラグ
/// @param[in] max_id ID番号の最大値
/// @param[in] val_map ノードの値を保持するクラス
extern
Justifier*
new_Just2(bool td_mode,
	  ymuint max_id,
	  const ValMap& val_map);

END_NAMESPACE_YM_SATPG

#endif // JUSTIFIER_H
