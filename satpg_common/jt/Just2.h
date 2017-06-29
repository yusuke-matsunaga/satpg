#ifndef JUST2_H
#define JUST2_H

/// @file Just2.h
/// @brief Just2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "JustBase.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class Just2 Just2.h "td/Just2.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class Just2 :
  public JustBase
{
public:

  /// @brief コンストラクタ
  /// @param[in] td_mode 遷移故障モードの時 true にするフラグ
  /// @param[in] max_id ID番号の最大値
  /// @param[in] val_map ノードの値を保持するクラス
  Just2(bool td_mode,
	ymuint max_id,
	const ValMap& val_map);

  /// @brief デストラクタ
  virtual
  ~Just2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める．
  /// @param[in] node_list 対象のノードのリスト
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  void
  operator()(const vector<const TpgNode*>& node_list,
	     NodeValList& pi_assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  void
  justify(const TpgNode* node,
	  int time,
	  NodeValList& pi_assign_list);

  /// @brief すべてのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  void
  just_all(const TpgNode* node,
	   int time,
	   NodeValList& pi_assign_list);

  /// @brief 指定した値を持つファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] ipos ファンインの位置番号 ( 0 <= ipos < node->fanin_num() )
  /// @param[in] time タイムフレーム ( 0 or 1 )
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  void
  just_one(const TpgNode* node,
	   Val3 val,
	   int time,
	   NodeValList& pi_assign_list);

  /// @brief 指定したファンインに対する値の割当数を数える．
  /// @param[in] node 対象のノード
  /// @param[in] ipos ファンインの位置番号 ( 0 <= ipos < node->fanin_num() )
  /// @param[in] time タイムフレーム ( 0 or 1 )
  /// @return 新規に値の割り当てられるノード数を返す．
  ymuint
  count_one(const TpgNode* node,
	    ymuint ipos,
	    int time);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_SATPG

#endif // JUST2_H
