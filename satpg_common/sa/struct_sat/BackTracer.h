#ifndef BACKTRACER_H
#define BACKTRACER_H

/// @file BackTracer.h
/// @brief BackTracer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BtJustBase.h"
#include "ym/UnitAlloc.h"


BEGIN_NAMESPACE_YM_SATPG_SA

//////////////////////////////////////////////////////////////////////
/// @class BackTracer BackTracer.h "BackTracer.h"
/// @brief 必要なノードのみ正当化する BackTracer
//////////////////////////////////////////////////////////////////////
class BackTracer
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_id ノード番号の最大値
  /// @param[in] val_map ノードの値を保持するクラス
  BackTracer(ymuint max_id,
	     const ValMap& val_map);

  /// @brief デストラクタ
  ~BackTracer();


public:
  //////////////////////////////////////////////////////////////////////
  // BackTracer の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief バックトレースを行なう．
  /// @param[in] suf_list 十分割り当てのリスト
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  void
  run(const NodeValList& suf_list,
      NodeValList& pi_assign_list);


protected:
  //////////////////////////////////////////////////////////////////////
  // BtJustBase の仮想関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いるデータ構造
  //////////////////////////////////////////////////////////////////////

  struct NodeList
  {
    const TpgNode* mNode;

    int mTime;

    NodeList* mLink;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief solve 中で変数割り当ての正当化を行なう．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @note node の値割り当てを正当化する．
  /// @note 正当化に用いられているノードには mark3 がつく．
  /// @note mark3 がついたノードは mBwdNodeList に格納される．
  NodeList*
  justify(const TpgNode* node);

  /// @brief すべてのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  NodeList*
  just_sub1(const TpgNode* node);

  /// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[in] val 値
  NodeList*
  just_sub2(const TpgNode* node,
	    Val3 val);

  /// @brief solve 中で変数割り当ての正当化を行なう．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @note node の値割り当てを正当化する．
  /// @note 正当化に用いられているノードには mark3 がつく．
  /// @note mark3 がついたノードは mBwdNodeList に格納される．
  NodeList*
  justify0(const TpgNode* node);

  /// @brief すべてのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  NodeList*
  just0_sub1(const TpgNode* node);

  /// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[in] val 値
  NodeList*
  just0_sub2(const TpgNode* node,
	     Val3 val);

  /// @brief 新しいリストのセルを返す．
  NodeList*
  new_list_cell(const TpgNode* node,
		int time);

  /// @brief リストをマージする．
  void
  list_merge(NodeList*& dst_list,
	     NodeList* src_list);

  /// @brief リストのサイズを返す．
  static
  ymuint
  list_size(NodeList* node_list);

  /// @brief リストを削除する．
  void
  list_free(NodeList* node_list);

  /// @brief ノードの TFO に印をつける．
  void
  mark_tfo(const TpgNode* node);

  /// @brief justified マークをつけ，mJustifiedNodeList に加える．
  /// @param[in] node 対象のノード
  void
  set_justified(const TpgNode* node);

  /// @brief justified マークを読む．
  /// @param[in] node 対象のノード
  bool
  justified_mark(const TpgNode* node);

  /// @brief justified マークをつけ，mJustifiedNodeList に加える．
  /// @param[in] node 対象のノード
  void
  set_justified0(const TpgNode* node);

  /// @brief justified マークを読む．
  /// @param[in] node 対象のノード
  bool
  justified0_mark(const TpgNode* node);

  /// @brief ノードの正常値を返す．
  /// @param[in] node ノード
  /// @param[in] time 時刻 ( 0 or 1 )
  Val3
  gval(const TpgNode* node,
       int time = 1) const;

  /// @brief ノードの故障地を返す．
  /// @param[in] node ノード
  Val3
  fval(const TpgNode* node) const;

  /// @brief 入力ノードの値を記録する．
  /// @param[in] node 対象の外部入力ノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[in] time 時刻 (0 or 1)
  /// @param[out] assign_list 値の割当リスト
  void
  record_value(const TpgNode* node,
	       int time,
	       NodeValList& assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // メモリ確保用のオブジェクト
  UnitAlloc mAlloc;

  // ノード番号の最大値
  ymuint mMaxId;

  // node->id() をキーにして入力番号のリストを納める配列
  vector<NodeList*> mJustArray;

  // node->id() をキーにして入力番号のリストを納める配列
  vector<NodeList*> mJust0Array;

  // 故障の TFO につけるマーク
  vector<bool> mTfoMark;

  // 個々のノードのマークを表す配列
  vector<ymuint8> mMarkArray;

  // ノードの値を保持するクラス
  const ValMap& mValMap;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////


// @brief justified マークをつける．
// @param[in] node 対象のノード
inline
void
BackTracer::set_justified(const TpgNode* node)
{
  mMarkArray[node->id()] |= 1U;
}

// @brief justified マークを読む．
// @param[in] node 対象のノード
inline
bool
BackTracer::justified_mark(const TpgNode* node)
{
  return static_cast<bool>((mMarkArray[node->id()] >> 0) & 1U);
}

// @brief justified マークをつける．
// @param[in] node 対象のノード
inline
void
BackTracer::set_justified0(const TpgNode* node)
{
  mMarkArray[node->id()] |= 2U;
}

// @brief justified マークを読む．
// @param[in] node 対象のノード
inline
bool
BackTracer::justified0_mark(const TpgNode* node)
{
  return static_cast<bool>((mMarkArray[node->id()] >> 1) & 1U);
}

// @brief ノードの正常値を返す．
// @param[in] node ノード
// @param[in] time 時刻 ( 0 or 1 )
inline
Val3
BackTracer::gval(const TpgNode* node,
	     int time) const
{
  return mValMap.gval(node, time);
}

// @brief ノードの故障地を返す．
// @param[in] node ノード
inline
Val3
BackTracer::fval(const TpgNode* node) const
{
  return mValMap.fval(node);
}

// @brief 入力ノードの値を記録する．
// @param[in] node 対象の外部入力ノード
// @param[in] time 時刻 (0 or 1)
// @param[out] assign_list 値の割当リスト
inline
void
BackTracer::record_value(const TpgNode* node,
			 int time,
			 NodeValList& assign_list)
{
  Val3 v = gval(node, time);
  if ( v != kValX ) {
    bool bval = (v == kVal1);
    assign_list.add(node, time, bval);
  }
}

END_NAMESPACE_YM_SATPG_SA

#endif // BACKTRACER_H
