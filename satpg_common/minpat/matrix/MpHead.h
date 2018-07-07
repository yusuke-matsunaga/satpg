#ifndef MPHEAD_H
#define MPHEAD_H

/// @file MpHead.h
/// @brief MpHead のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"

#include "MpRowList.h"
#include "MpColList.h"
#include "MpCell.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpHead MpHead.h "MpHead.h"
/// @brief Mincov 用の行と列のヘッダを表すクラス
//////////////////////////////////////////////////////////////////////
class MpHead
{
  friend class MpBlock;
  friend class MpHeadList;

public:

  /// @brief コンストラクタ
  MpHead();

  /// @brief デストラクタ
  ~MpHead();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  /// @param[in] pos 位置番号
  /// @param[in] col_flag 列の時に true とするフラグ
  void
  init(int pos,
       bool col_flag);

  /// @brief 行ヘッダの時に true を返す．
  bool
  is_row() const;

  /// @brief 列ヘッダの時に true を返す．
  bool
  is_col() const;

  /// @brief 位置番号を返す．
  int
  pos() const;

  /// @brief 要素数を返す．
  int
  num() const;

  /// @brief 行の先頭の要素を返す．
  MpCell*
  row_front() const;

  /// @brief 行の末尾の要素を返す．
  MpCell*
  row_back() const;

  /// @brief 行方向のリストを返す．
  MpRowList
  row_list() const;

  /// @brief 行方向のリストに挿入する．
  /// @param[in] cell 挿入する要素
  /// @retval true 追加が成功した．
  /// @retval false 同じ要素がすでに存在した．
  bool
  row_insert(MpCell* cell);

  /// @brief 行方向のリストから削除する．
  /// @param[in] cell 削除する要素
  void
  row_delete(MpCell* cell);

  /// @brief row_delete() で削除したセルを元の位置に戻す．
  /// @param[in] cell 削除する要素
  ///
  /// cell->row_prev(), cell->row_next() に正しい値が入っている
  void
  row_restore(MpCell* cell);

  /// @brief 列の先頭の要素を返す．
  MpCell*
  col_front() const;

  /// @brief 列の末尾の要素を返す．
  MpCell*
  col_back() const;

  /// @brief 列方向のリストを返す．
  MpColList
  col_list() const;

  /// @brief 列方向のリストに挿入する.
  /// @param[in] cell 挿入する要素
  /// @retval true 追加が成功した．
  /// @retval false 同じ要素がすでに存在した．
  bool
  col_insert(MpCell* cell);

  /// @brief 列方向のリストから削除する．
  /// @param[in] cell 削除する要素
  void
  col_delete(MpCell* cell);

  /// @brief col_delete() で削除したセルを元の位置に戻す．
  /// @param[in] cell 削除する要素
  ///
  /// cell->col_prev(), cell->col_next() に正しい値が入っている
  void
  col_restore(MpCell* cell);

  /// @brief 直前のヘッダを返す．
  const MpHead*
  prev() const;

  /// @brief 直後のヘッダを返す．
  const MpHead*
  next() const;


public:
  //////////////////////////////////////////////////////////////////////
  // public かつ mutable という異例のデータメンバ
  //////////////////////////////////////////////////////////////////////

  mutable
  int mWork;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続している要素をクリアする．
  void
  clear();

  /// @brief 要素数を増やす．
  /// @return 増加後の要素数を返す．
  int
  inc_num();

  /// @brief 要素数を減らす．
  /// @return 減少後の要素数を返す．
  int
  dec_num();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 位置番号 + 行/列フラグ
  // 最下位ビットが 0:行, 1:列 を表す．
  int mPos;

  // 要素数
  int mNum;

  // 二重連結リストのダミーヘッダ
  MpCell mDummy;

  // 直前のヘッダを指すリンク
  MpHead* mPrev;

  // 直後のヘッダを指すリンク
  MpHead* mNext;

  // 削除フラグ
  bool mDeleted;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
MpHead::MpHead() :
  mPos(0),
  mDummy(-1, -1),
  mPrev(nullptr),
  mNext(nullptr),
  mDeleted(false)
{
  clear();
}

/// @brief デストラクタ
inline
MpHead::~MpHead()
{
}

// @brief 初期化する．
// @param[in] pos 位置番号
// @param[in] col_flag 列の時に true とするフラグ
inline
void
MpHead::init(int pos,
	     bool col_flag)
{
  mPos = (pos << 1) | static_cast<int>(col_flag);
}

// @brief 行ヘッダの時に true を返す．
inline
bool
MpHead::is_row() const
{
  return !is_col();
}

// @brief 列ヘッダの時に true を返す．
inline
bool
MpHead::is_col() const
{
  return static_cast<bool>(mPos & 1);
}

// @brief 位置番号を返す．
inline
int
MpHead::pos() const
{
  return mPos >> 1;
}

// @brief 要素数を返す．
inline
int
MpHead::num() const
{
  return mNum;
}

// @brief 要素数を増やす．
inline
int
MpHead::inc_num()
{
  return ++ mNum;
}

// @brief 要素数を減らす．
inline
int
MpHead::dec_num()
{
  return -- mNum;
}

// @brief 行の先頭の要素を返す．
inline
MpCell*
MpHead::row_front() const
{
  return mDummy.mRightLink;
}

// @brief 行の末尾の要素を返す．
inline
MpCell*
MpHead::row_back() const
{
  return mDummy.mLeftLink;
}

// @brief 行方向のリストを返す．
inline
MpRowList
MpHead::row_list() const
{
  return MpRowList(row_front(), &mDummy);
}

// @brief 行方向のリストから削除する．
// @param[in] cell 削除する要素
inline
void
MpHead::row_delete(MpCell* cell)
{
  ASSERT_COND( is_row() );

  MpCell* prev = cell->mLeftLink;
  MpCell* next = cell->mRightLink;

  ASSERT_COND( prev->mRightLink == cell );
  ASSERT_COND( next->mLeftLink == cell );

  prev->mRightLink = next;
  next->mLeftLink = prev;

  dec_num();
}

// @brief row_delete() で削除したセルを元の位置に戻す．
// @param[in] cell 削除する要素
//
// cell->row_prev(), cell->row_next() に正しい値が入っている
inline
void
MpHead::row_restore(MpCell* cell)
{
  ASSERT_COND( is_row() );

  MpCell* prev = cell->mLeftLink;
  MpCell* next = cell->mRightLink;

  ASSERT_COND( prev->mRightLink == next );
  ASSERT_COND( next->mLeftLink == prev );

  prev->mRightLink = cell;
  next->mLeftLink = cell;

  inc_num();
}

// @brief 列の先頭の要素を返す．
inline
MpCell*
MpHead::col_front() const
{
  return mDummy.mDownLink;
}

// @brief 列の末尾の要素を返す．
inline
MpCell*
MpHead::col_back() const
{
  return mDummy.mUpLink;
}

// @brief 列方向のリストを返す．
inline
MpColList
MpHead::col_list() const
{
  return MpColList(col_front(), &mDummy);
}

// @brief 列方向のリストから削除する．
// @param[in] cell 削除する要素
inline
void
MpHead::col_delete(MpCell* cell)
{
  ASSERT_COND( is_col() );

  MpCell* prev = cell->mUpLink;
  MpCell* next = cell->mDownLink;

  ASSERT_COND( prev->mDownLink == cell );
  ASSERT_COND( next->mUpLink == cell );

  prev->mDownLink = next;
  next->mUpLink = prev;

  dec_num();
}

// @brief col_delete() で削除したセルを元の位置に戻す．
// @param[in] cell 削除する要素
//
// cell->col_prev(), cell->col_next() に正しい値が入っている
inline
void
MpHead::col_restore(MpCell* cell)
{
  ASSERT_COND( is_col() );

  MpCell* prev = cell->mUpLink;
  MpCell* next = cell->mDownLink;

  ASSERT_COND( prev->mDownLink == next );
  ASSERT_COND( next->mUpLink == prev );

  prev->mDownLink = cell;
  next->mUpLink = cell;

  inc_num();
}

// @brief 直前のヘッダを返す．
inline
const MpHead*
MpHead::prev() const
{
  return mPrev;
}

// @brief 直後のヘッダを返す．
inline
const MpHead*
MpHead::next() const
{
  return mNext;
}

// @brief 接続している要素をクリアする．
inline
void
MpHead::clear()
{
  mDummy.mLeftLink = &mDummy;
  mDummy.mRightLink = &mDummy;
  mDummy.mUpLink = &mDummy;
  mDummy.mDownLink = &mDummy;
  mNum = 0;
}

END_NAMESPACE_YM_SATPG

#endif // MPHEAD_H
