#ifndef JUSTIFIER_H
#define JUSTIFIER_H

/// @file Justifier.h
/// @brief Justifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "ym/SatBool3.h"


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

  /// @brief 正当化に必要な割当を求める(縮退故障用)．
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] var_map 変数番号のマップ
  /// @param[in] model SAT問題の解
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  virtual
  void
  operator()(const NodeValList& assign_list,
	     const VidMap& var_map,
	     const vector<SatBool3>& model,
	     NodeValList& pi_assign_list) = 0;

  /// @brief 正当化に必要な割当を求める(遷移故障用)．
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] var1_map 1時刻目の変数番号のマップ
  /// @param[in] var2_map 2時刻目の変数番号のマップ
  /// @param[in] model SAT問題の解
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  virtual
  void
  operator()(const NodeValList& assign_list,
	     const VidMap& var1_map,
	     const VidMap& var2_map,
	     const vector<SatBool3>& model,
	     NodeValList& pi_assign_list) = 0;

};

/// @brief JustSimple を生成する．
/// @param[in] max_id ID番号の最大値
extern
Justifier*
new_JustSimple(int max_id);

/// @brief Just1 を生成する．
/// @param[in] max_id ID番号の最大値
extern
Justifier*
new_Just1(int max_id);

/// @brief Just2 を生成する．
/// @param[in] max_id ID番号の最大値
extern
Justifier*
new_Just2(int max_id);

END_NAMESPACE_YM_SATPG

#endif // JUSTIFIER_H
