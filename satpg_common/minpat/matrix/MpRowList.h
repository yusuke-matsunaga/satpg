#ifndef MPROWLIST_H
#define MPROWLIST_H

/// @file MpRowList.h
/// @brief MpRowList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "MpRowIterator.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpRowList MpRowList.h "MpRowList.h"
/// @brief MpCell を行方向にたどるためのクラス
///
/// このクラスは読み出し専用でリストの内容を変えることはできない．
//////////////////////////////////////////////////////////////////////
class MpRowList
{
public:

  using iterator = MpRowIterator;

public:

  /// @brief コンストラクタ
  /// @param[in] begin_cell 先頭の要素
  /// @param[in] end_cell 末尾の要素
  MpRowList(const MpCell* begin_cell,
	    const MpCell* end_cell);

  /// @brief デストラクタ
  ~MpRowList();


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
  operator==(const MpRowList& list1,
	     const MpRowList& list2);


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

/// @relates MpRowList
/// @brief 等価比較演算子
/// @param[in] list1, list2 オペランド
bool
operator==(const MpRowList& list1,
	   const MpRowList& list2);

/// @relates MpRowList
/// @brief 非等価比較演算子
/// @param[in] list1, list2 オペランド
bool
operator!=(const MpRowList& list1,
	   const MpRowList& list2);

/// @relates MpRowList
/// @brief 包含関係を調べる．
/// @param[in] list1, list2 オペランド
///
/// list1 が list2 の要素をすべて含んでいたら true を返す．
bool
check_containment(const MpRowList& list1,
		  const MpRowList& list2);



//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] begin_cell 先頭の要素
// @param[in] end_cell 末尾の要素
inline
MpRowList::MpRowList(const MpCell* begin_cell,
		     const MpCell* end_cell) :
  mBegin(begin_cell),
  mEnd(end_cell)
{
}

// @brief デストラクタ
inline
MpRowList::~MpRowList()
{
}

// @brief 先頭の反復子を返す．
inline
MpRowIterator
MpRowList::begin() const
{
  return MpRowIterator(mBegin);
}

// @brief 末尾の反復子を返す．
inline
MpRowIterator
MpRowList::end() const
{
  return MpRowIterator(mEnd);
}

// @brief 非等価比較演算子
// @param[in] list1, list2 オペランド
inline
bool
operator!=(const MpRowList& list1,
	   const MpRowList& list2)
{
  return !operator==(list1, list2);
}

END_NAMESPACE_YM_SATPG

#endif // MPROWLIST_H
