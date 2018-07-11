#ifndef MPCOLGRAPH_H
#define MPCOLGRAPH_H

/// @file MpColGraph.h
/// @brief MpColGraph のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpColGraph MpColGraph.h "MpColGraph.h"
/// @brief MinPatMgr の coloring 用のグラフを表すクラス
///
/// * 隣接ペア(n1, n2)の枝を明示的には持たない．
/// * 代わりにテストベクタの各ビットごとに相反するグループのリストを持つ．
/// * テストベクタ(グラフのノード)は一時的に削除される．
//////////////////////////////////////////////////////////////////////
class MpColGraph
{
public:

  /// @brief コンストラクタ
  /// @param[in] tv_list テストパタンのリスト
  MpColGraph(const vector<TestVector>& tv_list);

  /// @brief デストラクタ
  ~MpColGraph();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード数を返す．
  int
  node_num() const;

  /// @brief node が node_list のノード集合と両立する時 true を返す．
  /// @param[in] node ノード番号
  /// @param[in] node_list ノード番号のリスト
  bool
  compatible_check(int node,
		   const vector<int>& node_list) const;

  /// @brief node1 の衝突集合が node2 の衝突集合に含まれていたら true を返す．
  /// @param[in] node1, node2 ノード番号
  bool
  containment_check(int node1,
		    int node2) const;

  /// @brief ノードを削除する．
  /// @param[in] node 削除するノード番号
  void
  delete_node(int node);

  /// @brief 色数を返す．
  int
  color_num() const;

  /// @brief ノードの色を得る．
  /// @param[in] node_id ノード番号 ( 0 <= node_id < node_num() )
  int
  color(int node_id) const;

  /// @brief 新しい色を割り当てる．
  /// @return 新しい色番号を返す．
  int
  new_color();

  /// @brief ノードを色をつける．
  /// @param[in] node_id ノード番号 ( 0 <= node_id < node_num() )
  /// @param[in] color 色番号 ( 1 <= color <= color_num() )
  void
  set_color(int node_id,
	    int color);

  /// @brief ノードの集合を色をつける．
  /// @param[in] node_id_list ノード番号のリスト
  /// @param[in] color 色 ( 1 <= color <= color_num() )
  void
  set_color(const vector<int>& node_id_list,
	    int color);

  /// @brief color_map を作る．
  int
  get_color_map(vector<int>& color_map) const;

  /// @brief 隣接しているノード対に同じ色が割り当てられていないか確認する．
  bool
  verify() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 衝突リストを作る．
  /// @param[in] tv_list テストパタンのリスト
  void
  gen_conflict_list();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // テストベクタのリスト
  const vector<TestVector>& mTvList;

  // ノード(テストベクタ)数
  int mNodeNum;

  // テストベクタのビット長
  int mVectorSize;

  // ノードの削除フラグ
  vector<bool> mDeleteFlag;

  // 衝突関係にあるノード番号のリストの配列
  // サイズはテストベクタのベクタ長 x 2
  vector<vector<int>> mNodeListArray;

  // 衝突関係にあるノード集合を表す OID のリスト
  // サイズは mNodeNum
  vector<vector<int>> mOidListArray;

  // 現在使用中の色数
  int mColNum;

  // 彩色結果の配列
  // サイズは mNodeNum
  vector<int> mColorMap;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ノード数を返す．
inline
int
MpColGraph::node_num() const
{
  return mNodeNum;
}

// @brief ノードを削除する．
// @param[in] node 削除するノード番号
inline
void
MpColGraph::delete_node(int node)
{
  ASSERT_COND( node >= 0 && node < node_num() );

  mDeleteFlag[node] = true;
}

// @brief 色数を返す．
inline
int
MpColGraph::color_num() const
{
  return mColNum;
}

// @brief ノードの色を得る．
// @param[in] node_id ノード番号 ( 0 <= node_id < node_num() )
inline
int
MpColGraph::color(int node_id) const
{
  ASSERT_COND( node_id >= 0 && node_id < node_num() );

  return mColorMap[node_id];
}

// @brief 新しい色を割り当てる．
// @return 新しい色番号を返す．
inline
int
MpColGraph::new_color()
{
  return ++ mColNum;
}

// @brief ノードを色をつける．
// @param[in] node_id ノード番号 ( 0 <= node_id < node_num() )
// @param[in] color 色番号 ( 1 <= color <= color_num() )
inline
void
MpColGraph::set_color(int node_id,
		      int color)
{
  ASSERT_COND( node_id >= 0 && node_id < node_num() );
  ASSERT_COND( color >= 1 && color <= color_num() );

  mColorMap[node_id] = color;
}

// @brief ノードの集合を色をつける．
// @param[in] node_id_list ノード番号のリスト
// @param[in] color 色 ( 1 <= color <= color_num() )
inline
void
MpColGraph::set_color(const vector<int>& node_id_list,
		      int color)
{
  ASSERT_COND( color >= 1 && color <= color_num() );

  for ( auto node_id: node_id_list ) {
    set_color(node_id, color);
  }
}

END_NAMESPACE_YM_SATPG

#endif // MPCOLGRAPH_H