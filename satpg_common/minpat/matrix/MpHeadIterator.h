#ifndef MPHEADITERATOR_H
#define MPHEADITERATOR_H

/// @file MpHeadIterator.h
/// @brief MpHeadIterator のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpHeadIterator MpHeadIterator.h "MpHeadIterator.h"
/// @brief MpHead の反復子
//////////////////////////////////////////////////////////////////////
class MpHeadIterator
{
public:

  /// @brief コンストラクタ
  MpHeadIterator(const MpHead* head = nullptr);

  /// @brief デストラクタ
  ~MpHeadIterator();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @breif dereference 演算子
  const MpHead*
  operator*() const;

  /// @brief インクリメント演算子
  MpHeadIterator
  operator++();

  /// @brief インクリメント演算子
  MpHeadIterator
  operator++(int);

  /// @brief 等価比較演算子
  bool
  operator==(const MpHeadIterator& right) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 現在の ヘッダ
  const MpHead* mCurHead;

};

/// @brief 非等価比較演算子
bool
operator!=(const MpHeadIterator& left,
	   const MpHeadIterator& right);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
MpHeadIterator::MpHeadIterator(const MpHead* head) :
  mCurHead(head)
{
}

// @brief デストラクタ
inline
MpHeadIterator::~MpHeadIterator()
{
}

// @breif dereference 演算子
inline
const MpHead*
MpHeadIterator::operator*() const
{
  return mCurHead;
}

// @brief インクリメント演算子
inline
MpHeadIterator
MpHeadIterator::operator++()
{
  if ( mCurHead != nullptr ) {
    mCurHead = mCurHead->next();
  }

  return *this;
}

// @brief インクリメント演算子(後置)
inline
MpHeadIterator
MpHeadIterator::operator++(int)
{
  MpHeadIterator ans(mCurHead);

  if ( mCurHead != nullptr ) {
    mCurHead = mCurHead->next();
  }

  return ans;
}

// @brief 等価比較演算子
inline
bool
MpHeadIterator::operator==(const MpHeadIterator& right) const
{
  return mCurHead == right.mCurHead;
}

/// @brief 非等価比較演算子
inline
bool
operator!=(const MpHeadIterator& left,
	   const MpHeadIterator& right)
{
  return !left.operator==(right);
}

END_NAMESPACE_YM_SATPG

#endif // MPHEADITERATOR_H
