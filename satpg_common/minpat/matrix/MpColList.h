#ifndef MPCOLLIST_H
#define MPCOLLIST_H

/// @file MpColList.h
/// @brief MpColList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "MpColIterator.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpColList MpColList.h "MpColList.h"
/// @brief MpCell を列方向にたどるためのクラス
///
/// このクラスは読み出し専用でリストの内容を変えることはできない．
//////////////////////////////////////////////////////////////////////
class MpColList
{
public:

  using iterator = MpColIterator;

public:

  /// @brief コンストラクタ
  /// @param[in] begin_cell 先頭の要素
  /// @param[in] end_cell 末尾の要素
  MpColList(const MpCell* begin_cell,
	    const MpCell* end_cell);

  /// @brief デストラクタ
  ~MpColList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 先頭の反復子を返す．
  iterator
  begin() const;

  /// @brief 末尾の反復子を返す．
  iterator
  end() const;

  friend
  bool
  operator==(const MpColList& list1,
	     const MpColList& list2);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 先頭の要素
  const MpCell* mBegin;

  // 末尾の要素
  const MpCell* mEnd;

};

/// @relates MpColList
/// @brief 等価比較演算子
/// @param[in] list1, list2 オペランド
bool
operator==(const MpColList& list1,
	   const MpColList& list2);

/// @relates MpColList
/// @brief 非等価比較演算子
/// @param[in] list1, list2 オペランド
bool
operator!=(const MpColList& list1,
	   const MpColList& list2);

/// @relates MpColList
/// @brief 包含関係を調べる．
/// @param[in] list1, list2 オペランド
///
/// list1 が list2 の要素をすべて含んでいたら true を返す．
bool
check_containment(const MpColList& list1,
		  const MpColList& list2);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] begin_cell 先頭の要素
// @param[in] end_cell 末尾の要素
inline
MpColList::MpColList(const MpCell* begin_cell,
		     const MpCell* end_cell) :
  mBegin(begin_cell),
  mEnd(end_cell)
{
}

// @brief デストラクタ
inline
MpColList::~MpColList()
{
}

// @brief 先頭の反復子を返す．
inline
MpColIterator
MpColList::begin() const
{
  return MpColIterator(mBegin);
}

// @brief 末尾の反復子を返す．
inline
MpColIterator
MpColList::end() const
{
  return MpColIterator(mEnd);
}

// @brief 非等価比較演算子
// @param[in] list1, list2 オペランド
inline
bool
operator!=(const MpColList& list1,
	   const MpColList& list2)
{
  return !operator==(list1, list2);
}

END_NAMESPACE_YM_SATPG

#endif // MPCOLLIST_H
