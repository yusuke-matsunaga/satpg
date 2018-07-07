#ifndef MPROWITERATOR_H
#define MPROWITERATOR_H

/// @file MpRowIterator.h
/// @brief MpRowIterator のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "MpCell.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpRowIterator MpRowIterator.h "MpRowIterator.h"
/// @brief MpCell を行方向にたどる反復子
//////////////////////////////////////////////////////////////////////
class MpRowIterator
{
public:

  /// @brief コンストラクタ
  /// @param[in] cell 対象の MpCell
  MpRowIterator(const MpCell* cell = nullptr);

  /// @brief デストラクタ
  ~MpRowIterator();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @breif dereference 演算子
  const MpCell*
  operator*() const;

  /// @brief インクリメント演算子
  MpRowIterator
  operator++();

  /// @brief インクリメント演算子(後置)
  MpRowIterator
  operator++(int);

  /// @brief 等価比較演算子
  bool
  operator==(const MpRowIterator& right) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 現在のセル
  const MpCell* mCurCell;

};

/// @brief 非等価比較演算子
bool
operator!=(const MpRowIterator& left,
	   const MpRowIterator& right);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] cell 対象の MpCell
inline
MpRowIterator::MpRowIterator(const MpCell* cell) :
  mCurCell(cell)
{
}

// @brief デストラクタ
inline
MpRowIterator::~MpRowIterator()
{
}

// @breif dereference 演算子
inline
const MpCell*
MpRowIterator::operator*() const
{
  return mCurCell;
}

// @brief インクリメント演算子
inline
MpRowIterator
MpRowIterator::operator++()
{
  if ( mCurCell != nullptr ) {
    mCurCell = mCurCell->row_next();
  }

  return *this;
}

// @brief インクリメント演算子
inline
MpRowIterator
MpRowIterator::operator++(int)
{
  MpRowIterator ans(mCurCell);

  if ( mCurCell != nullptr ) {
    mCurCell = mCurCell->row_next();
  }

  return ans;
}

// @brief 等価比較演算子
inline
bool
MpRowIterator::operator==(const MpRowIterator& right) const
{
  return mCurCell == right.mCurCell;
}

// @brief 非等価比較演算子
inline
bool
operator!=(const MpRowIterator& left,
	   const MpRowIterator& right)
{
  return !left.operator==(right);
}

END_NAMESPACE_YM_SATPG

#endif // MPROWITERATOR_H
