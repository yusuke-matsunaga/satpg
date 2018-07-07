#ifndef MPHEADLIST_H
#define MPHEADLIST_H

/// @file MpHeadList.h
/// @brief MpHeadList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "MpHead.h"
#include "MpHeadIterator.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpHeadList MpHeadList.h "MpHeadList.h"
/// @brief MpHead のリストを表すクラス
//////////////////////////////////////////////////////////////////////
class MpHeadList
{
  friend class MpMatrix;

public:

  using iterator = MpHeadIterator;

public:

  /// @brief コンストラクタ
  MpHeadList();

  /// @brief デストラクタ
  ~MpHeadList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  void
  clear();

  /// @brief 内容をセットする．
  void
  set(const vector<MpHead*>& head_list);

  /// @brief 要素を追加する．
  /// @param[in] head 追加する要素
  void
  insert(MpHead* head);

  /// @brief 要素を削除する．
  /// @param[in] head 削除する要素
  ///
  /// head がこのリストに含まれていると仮定する．
  void
  exclude(MpHead* head);

  /// @brief delete() で削除した要素を復元する．
  /// @param[in] head 復元する要素
  void
  restore(MpHead* head);

  /// @brief 分割したリストをマージして元にもどす．
  /// @param[in] src1, src2 分割したリスト
  ///
  /// src1, src2 の内容は破棄される．
  void
  merge(MpHeadList& src1,
	MpHeadList& src2);

  /// @brief 要素数を返す．
  int
  num() const;

  /// @brief 最初の要素を返す．
  const MpHead*
  front() const;

  /// @brief 先頭の反復子を返す．
  iterator
  begin() const;

  /// @brief 末尾の反復子を返す．
  iterator
  end() const;

  friend
  bool
  operator==(const MpHeadList& list1,
	     const MpHeadList& list2);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ダミーの先頭要素
  MpHead mDummy;

  // 要素数
  int mNum;

};

/// @relates MpHeadList
/// @brief 等価比較演算子
/// @param[in] list1, list2 オペランド
bool
operator==(const MpHeadList& list1,
	   const MpHeadList& list2);

/// @relates MpHeadList
/// @brief 非等価比較演算子
/// @param[in] list1, list2 オペランド
bool
operator!=(const MpHeadList& list1,
	   const MpHeadList& list2);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
MpHeadList::MpHeadList() :
  mNum(0)
{
}

// @brief デストラクタ
inline
MpHeadList::~MpHeadList()
{
}

// @brief 内容をクリアする．
inline
void
MpHeadList::clear()
{
  mDummy.mNext = mDummy.mPrev = &mDummy;
  mNum = 0;
}

// @brief 要素数を返す．
inline
int
MpHeadList::num() const
{
  return mNum;
}

// @brief 最初の要素を返す．
inline
const MpHead*
MpHeadList::front() const
{
  return mDummy.mNext;
}

// @brief 先頭の反復子を返す．
inline
MpHeadIterator
MpHeadList::begin() const
{
  return MpHeadIterator(mDummy.mNext);
}

// @brief 末尾の反復子を返す．
inline
MpHeadIterator
MpHeadList::end() const
{
  return MpHeadIterator(&mDummy);
}

// @relates MpHeadList
// @brief 非等価比較演算子
// @param[in] list1, list2 オペランド
inline
bool
operator!=(const MpHeadList& list1,
	   const MpHeadList& list2)
{
  return !operator==(list1, list2);
}

END_NAMESPACE_YM_SATPG

#endif // MPHEADLIST_H
