#ifndef MPCOLITERATOR_H
#define MPCOLITERATOR_H

/// @file MpColIterator.h
/// @brief MpColIterator のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "MpCell.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpColIterator MpColIterator.h "MpColIterator.h"
/// @brief MpCell を列方向にたどる反復子
//////////////////////////////////////////////////////////////////////
class MpColIterator
{
public:

  /// @brief コンストラクタ
  /// @param[in] cell 対象の MpCell
  MpColIterator(const MpCell* cell = nullptr);

  /// @brief デストラクタ
  ~MpColIterator();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @breif dereference 演算子
  const MpCell*
  operator*() const;

  /// @brief インクリメント演算子
  MpColIterator
  operator++();

  /// @brief インクリメント演算子(後置)
  MpColIterator
  operator++(int);

  /// @brief 等価比較演算子
  bool
  operator==(const MpColIterator& right) const;


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
operator!=(const MpColIterator& left,
	   const MpColIterator& right);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] cell 対象の MpCell
inline
MpColIterator::MpColIterator(const MpCell* cell) :
  mCurCell(cell)
{
}

// @brief デストラクタ
inline
MpColIterator::~MpColIterator()
{
}

// @breif dereference 演算子
inline
const MpCell*
MpColIterator::operator*() const
{
  return mCurCell;
}

// @brief インクリメント演算子
inline
MpColIterator
MpColIterator::operator++()
{
  if ( mCurCell != nullptr ) {
    mCurCell = mCurCell->col_next();
  }

  return *this;
}

// @brief インクリメント演算子
inline
MpColIterator
MpColIterator::operator++(int)
{
  MpColIterator ans(mCurCell);

  if ( mCurCell != nullptr ) {
    mCurCell = mCurCell->col_next();
  }

  return ans;
}

// @brief 等価比較演算子
inline
bool
MpColIterator::operator==(const MpColIterator& right) const
{
  return mCurCell == right.mCurCell;
}

// @brief 非等価比較演算子
inline
bool
operator!=(const MpColIterator& left,
	   const MpColIterator& right)
{
  return !left.operator==(right);
}

END_NAMESPACE_YM_SATPG

#endif // MPCOLITERATOR_H
