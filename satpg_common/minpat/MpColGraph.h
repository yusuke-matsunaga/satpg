#ifndef MPCOLGRAPH_H
#define MPCOLGRAPH_H

/// @file MpColGraph.h
/// @brief MpColGraph のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "ym/Array.h"
#include "ym/HashSet.h"


BEGIN_NAMESPACE_YM_SATPG

class MpNodeList;
class MpNodeIterator;

//////////////////////////////////////////////////////////////////////
/// @class MpColGraph MpColGraph.h "MpColGraph.h"
/// @brief MinPatMgr の coloring 用のグラフを表すクラス
//////////////////////////////////////////////////////////////////////
class MpColGraph
{
  friend class MpNodeIterator;

public:

  /// @brief コンストラクタ
  /// @param[in] fault_list 故障のリスト
  /// @param[in] tv_list テストパタンのリスト
  /// @param[in] network ネットワーク
  /// @param[in] fault_type 故障の種類
  MpColGraph(const vector<const TpgFault*>& fault_list,
	     const vector<TestVector>& tv_list,
	     const TpgNetwork& network,
	     FaultType fault_type);

  /// @brief デストラクタ
  ~MpColGraph();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障数を返す．
  int
  fault_num() const;

  /// @brief 故障の被覆状態を得る．
  /// @param[in] fault_id 故障番号 ( 0 <= fault_id < fault_num() )
  bool
  is_covered(int fault_id) const;

  /// @brief 故障を被覆状態にする．
  /// @param[in] fault_id 故障番号 ( 0 <= fault_id < fault_num() )
  void
  set_covered(int fault_id);

  /// @brief ノード数を返す．
  int
  node_num() const;

  /// @brief 隣接するノード番号のリストを得る．
  /// @param[in] node_id 対象のノード番号 ( 0 <= node_id < node_num() )
  MpNodeList
  adj_list(int node_id) const;

  /// @brief 被覆する故障番号のリストを得る．
  Array<int>
  cover_list(int node_id) const;

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

  /// @brief すべての行が彩色された列で被覆されているとき true を返す．
  bool
  is_covered() const;

  /// @brief 隣接しているノード対に同じ色が割り当てられていないか確認する．
  bool
  verify() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する
  /// @param[in] fault_list 故障のリスト
  /// @param[in] tv_list テストパタンのリスト
  /// @param[in] network ネットワーク
  /// @param[in] fault_type 故障の種類
  void
  init(const vector<const TpgFault*>& fault_list,
       const vector<TestVector>& tv_list,
       const TpgNetwork& network,
       FaultType fault_type);

  /// @brief 被覆リストを作る．
  /// @param[in] fault_list 故障のリスト
  /// @param[in] tv_list テストパタンのリスト
  /// @param[in] network ネットワーク
  /// @param[in] fault_type 故障の種類
  void
  gen_cover_list(const vector<const TpgFault*>& fault_list,
		 const vector<TestVector>& tv_list,
		 const TpgNetwork& network,
		 FaultType fault_type);

  /// @brief 故障シミュレーションを行う．
  /// @param[in] fsim 故障シミュレータ
  /// @param[in] tv_base テストベクタ番号の基点
  /// @param[in] num セットしたパタン数
  void
  do_fsim(Fsim& fsim,
	  int tv_base,
	  int num);

  /// @brief 衝突リストを作る．
  /// @param[in] tv_list テストパタンのリスト
  void
  gen_conflict_list(const vector<TestVector>& tv_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 整数のリストを表す構造体
  struct IdList
  {
    IdList();

    ~IdList();

    void
    init(const vector<int>& src_list);

    // 要素数
    int mNum;

    // 実体の配列
    int* mBody;
  };

  // 故障数
  int mFaultNum;

  // 故障番号から行番号への写像
  // サイズは network.max_fault_id()
  int* mRowIdMap;

  // 故障の被覆フラグ
  bool* mCoverFlag;

  // 衝突関係にあるノード番号のリストの対の配列
  // サイズはテストベクタのベクタ長 x 2
  IdList* mConflictPairArray;

  // ノード(テストベクタ)数
  int mNodeNum;

  // ノードの隣接リストの配列
  // サイズは mNodeNum;
  IdList* mAdjListArray;

  // ノードの被覆する故障番号のリストの配列
  // サイズは mNodeNum;
  IdList* mCoverListArray;

  // 現在使用中の色数
  int mColNum;

  // 彩色結果の配列
  int* mColorMap;

};


//////////////////////////////////////////////////////////////////////
/// @class MpNodeList MpNodeList.h "MpNodeList.h"
/// @brief MpColGraph の隣接ノードリストを表すクラス
//////////////////////////////////////////////////////////////////////
class MpNodeList
{
public:

  using iterator = MpNodeIterator;

public:

  /// @brief コンストラクタ
  MpNodeList(const MpColGraph& graph,
	     int num,
	     int* body);

  /// @brief デストラクタ
  ~MpNodeList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 先頭の反復子を返す．
  MpNodeIterator
  begin() const;

  /// @brief 末尾の反復子を返す．
  MpNodeIterator
  end() const;

  /// @brief 要素数を返す．
  int
  num() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // グラフ
  const MpColGraph& mGraph;

  // 要素数
  int mNum;

  // 要素の配列
  int* mBody;

};


//////////////////////////////////////////////////////////////////////
/// @class MpNodeIterator MpNodeList.h "MpNodeList.h"
/// @brief MpNodeList の反復子
//////////////////////////////////////////////////////////////////////
class MpNodeIterator
{
public:

  /// @brief コンストラクタ
  /// @param[in] graph 対象のグラフ
  ///
  MpNodeIterator(const MpColGraph& graph,
		 int* ptr1);

  /// @brief デストラクタ
  ~MpNodeIterator();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  int
  operator*();

  /// @brief increment 演算子
  const MpNodeIterator&
  operator++();

  /// @brief 等価比較演算子
  bool
  operator==(const MpNodeIterator& right) const;

  /// @brief 非等価比較演算子
  bool
  operator!=(const MpNodeIterator& right) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のグラフ
  const MpColGraph& mGraph;

  // 第１レベルのポインタ
  int* mPtr1;

  // 第２レベルのポインタ
  int* mPtr2;

  // 第２レベルポインタの末尾
  int* mEnd2;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 故障数を返す．
inline
int
MpColGraph::fault_num() const
{
  return mFaultNum;
}

// @brief 故障の被覆状態を得る．
// @param[in] fault_id 故障番号 ( 0 <= fault_id < fault_num() )
inline
bool
MpColGraph::is_covered(int fault_id) const
{
  ASSERT_COND( fault_id >= 0 && fault_id < fault_num() );

  return mCoverFlag[fault_id];
}

// @brief 故障を被覆状態にする．
// @param[in] fault_id 故障番号 ( 0 <= fault_id < fault_num() )
inline
void
MpColGraph::set_covered(int fault_id)
{
  ASSERT_COND( fault_id >= 0 && fault_id < fault_num() );

  mCoverFlag[fault_id] = true;
}

// @brief ノード数を返す．
inline
int
MpColGraph::node_num() const
{
  return mNodeNum;
}

// @brief 隣接するノード番号のリストを得る．
// @param[in] node_id 対象のノード番号 ( 0 <= node_id < node_num() )
inline
MpNodeList
MpColGraph::adj_list(int node_id) const
{
  ASSERT_COND( node_id >= 0 && node_id < node_num() );

  const IdList& adj_list = mAdjListArray[node_id];
  return MpNodeList(*this, adj_list.mNum, adj_list.mBody);
}

// @brief 被覆する故障番号のリストを得る．
inline
Array<int>
MpColGraph::cover_list(int node_id) const
{
  ASSERT_COND( node_id >= 0 && node_id < node_num() );

  const IdList& adj_list = mCoverListArray[node_id];
  return Array<int>(adj_list.mBody, 0, adj_list.mNum);
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

// @brief コンストラクタ
inline
MpNodeList::MpNodeList(const MpColGraph& graph,
		       int num,
		       int* body) :
  mGraph(graph),
  mNum(num),
  mBody(body)
{
}

// @brief デストラクタ
inline
MpNodeList::~MpNodeList()
{
}

// @brief 先頭の反復子を返す．
inline
MpNodeIterator
MpNodeList::begin() const
{
  return MpNodeIterator(mGraph, mBody);
}

// @brief 末尾の反復子を返す．
inline
MpNodeIterator
MpNodeList::end() const
{
  return MpNodeIterator(mGraph, mBody + mNum);
}

// @brief 要素数を返す．
inline
int
MpNodeList::num() const
{
  HashSet<int> id_set;
  int n;
  for ( auto id: *this ) {
    if ( !id_set.check(id) ) {
      id_set.add(id);
      ++ n;
    }
  }

  return n;
}

// @brief コンストラクタ
// @param[in] graph 対象のグラフ
//
inline
MpNodeIterator::MpNodeIterator(const MpColGraph& graph,
			       int* ptr1) :
  mGraph(graph),
  mPtr1(ptr1),
  mPtr2(nullptr),
  mEnd2(nullptr)
{
}

// @brief デストラクタ
inline
MpNodeIterator::~MpNodeIterator()
{
}

// @brief dereference 演算子
inline
int
MpNodeIterator::operator*()
{
  if ( mPtr2 == nullptr ) {
    int id = *mPtr1;
    const MpColGraph::IdList& id_list = mGraph.mConflictPairArray[id];
    mPtr2 = id_list.mBody;
    mEnd2 = id_list.mBody + id_list.mNum;
  }
  return *mPtr2;
}

// @brief increment 演算子
inline
const MpNodeIterator&
MpNodeIterator::operator++()
{
  ++ mPtr2;
  if ( mPtr2 == mEnd2 ) {
    ++ mPtr1;
    mPtr2 = nullptr;
  }

  return *this;
}

// @brief 等価比較演算子
inline
bool
MpNodeIterator::operator==(const MpNodeIterator& right) const
{
  if ( &mGraph != &right.mGraph ) {
    return false;
  }
  if ( mPtr1 != right.mPtr1 ) {
    return false;
  }
  if ( mPtr2 != right.mPtr2 ) {
    return false;
  }
  return true;
}

// @brief 非等価比較演算子
inline
bool
MpNodeIterator::operator!=(const MpNodeIterator& right) const
{
  return !operator==(right);
}

END_NAMESPACE_YM_SATPG

#endif // MPCOLGRAPH_H
