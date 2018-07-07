#ifndef MPCELL_H
#define MPCELL_H

/// @file MpCell.h
/// @brief MpCell のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "ym/UnitAlloc.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpCell MpCell.h "MpCell.h"
/// @brief mincov 用の行列要素を表すクラス
//////////////////////////////////////////////////////////////////////
class MpCell
{
  friend class MpHead;
  friend class MpBlock;

public:

  /// @brief コンストラクタ
  /// @param[in] row_pos 行番号
  /// @param[in] col_pos 列番号
  MpCell(int row_pos,
	 int col_pos);

  /// @brief デストラクタ
  ~MpCell();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 行番号を返す．
  int
  row_pos() const;

  /// @brief 列番号を返す．
  int
  col_pos() const;

  /// @brief 同じ行の直前の要素を返す．
  const MpCell*
  row_prev() const;

  /// @brief 同じ行の直後の要素を返す．
  const MpCell*
  row_next() const;

  /// @brief 同じ列の直前の要素を返す．
  const MpCell*
  col_prev() const;

  /// @brief 同じ列の直後の要素を返す．
  const MpCell*
  col_next() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 行番号
  int mRowPos;

  // 列番号
  int mColPos;

  // 左隣の要素
  MpCell* mLeftLink;

  // 右隣の要素
  MpCell* mRightLink;

  // 上の要素
  MpCell* mUpLink;

  // 下の要素
  MpCell* mDownLink;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] row_pos 行番号
// @param[in] col_pos 列番号
inline
MpCell::MpCell(int row_pos,
	       int col_pos) :
  mRowPos(row_pos),
  mColPos(col_pos)
{
}

// @brief デストラクタ
inline
MpCell::~MpCell()
{
}

// @brief 行番号を返す．
inline
int
MpCell::row_pos() const
{
  return mRowPos;
}

// @brief 列番号を返す．
inline
int
MpCell::col_pos() const
{
  return mColPos;
}

// @brief 同じ行の直前の要素を返す．
inline
const MpCell*
MpCell::row_prev() const
{
  return mLeftLink;
}

// @brief 同じ行の直後の要素を返す．
inline
const MpCell*
MpCell::row_next() const
{
  return mRightLink;
}

// @brief 同じ列の直前の要素を返す．
inline
const MpCell*
MpCell::col_prev() const
{
  return mUpLink;
}

// @brief 同じ列の直後の要素を返す．
inline
const MpCell*
MpCell::col_next() const
{
  return mDownLink;
}

END_NAMESPACE_YM_SATPG

#endif // MPCELL_H
