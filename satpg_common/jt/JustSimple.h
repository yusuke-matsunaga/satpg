#ifndef JUSTSIMPLE_H
#define JUSTSIMPLE_H

/// @file JustSimple.h
/// @brief JustSimple のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "JustBase.h"


BEGIN_NAMESPACE_YM_SATPG

class JustData;

//////////////////////////////////////////////////////////////////////
/// @class JustSimple JustSimple.h "JustSimple.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class JustSimple :
  public JustBase
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_id ノード番号の最大値
  JustSimple(int max_id);

  /// @brief デストラクタ
  virtual
  ~JustSimple();


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
	     NodeValList& pi_assign_list) override;

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
	     NodeValList& pi_assign_list) override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  void
  justify(const JustData& jd,
	  const TpgNode* node,
	  int time,
	  NodeValList& pi_assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_SATPG

#endif // JUSTSIMPLE_H
