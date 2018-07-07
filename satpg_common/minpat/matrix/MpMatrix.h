#ifndef MPMATRIX_H
#define MPMATRIX_H

/// @file MpMatrix.h
/// @brief MpMatrix のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"

#include "MpHead.h"
#include "MpHeadList.h"
#include "ym/UnitAlloc.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpMatrix MpMatrix.h "MpMatrix.h"
/// @brief mincov 用の行列を表すクラス
//////////////////////////////////////////////////////////////////////
class MpMatrix
{
public:

  /// @brief コンストラクタ
  /// @param[in] row_size 行数
  /// @param[in] col_size 列数
  /// @param[in] cost_array コストの配列
  /// @param[in] elem_list 要素のリスト
  ///
  /// * elem_list は (row_pos, col_pos) の pair のリスト
  MpMatrix(int row_size,
	   int col_size,
	   const vector<int>& cost_array,
	   const vector<pair<int, int> >& elem_list);

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のオブジェクト
  MpMatrix(const MpMatrix& src);

  /// @brief 代入演算子
  /// @param[in] src コピー元のオブジェクト
  const MpMatrix&
  operator=(const MpMatrix& src);

  /// @brief デストラクタ
  ~MpMatrix();


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取り出す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 行数を返す．
  int
  row_size() const;

  /// @brief 行の先頭を取り出す．
  /// @param[in] row_pos 行位置 ( 0 <= row_pos < row_size() )
  const MpHead*
  row_head(int row_pos) const;

  /// @brief 行の先頭を取り出す．
  /// @param[in] row_pos 行位置 ( 0 <= row_pos < row_size() )
  MpHead*
  row_head(int row_pos);

  /// @brief 列数を返す．
  int
  col_size() const;

  /// @brief 列の先頭を取り出す．
  /// @param[in] col_pos 列位置 ( 0 <= col_pos < col_size() )
  const MpHead*
  col_head(int col_pos) const;

  /// @brief 列の先頭を取り出す．
  /// @param[in] col_pos 列位置 ( 0 <= col_pos < col_size() )
  MpHead*
  col_head(int col_pos);

  /// @brief 列のコストを取り出す．
  /// @param[in] col_pos 列位置 ( 0 <= col_pos < col_size() )
  int
  col_cost(int col_pos) const;

  /// @brief 列のコストの配列を取り出す．
  const int*
  col_cost_array() const;

  /// @brief 列集合のコストを返す．
  /// @param[in] col_list 列のリスト
  int
  cost(const vector<int>& col_list) const;

  /// @brief 列集合がカバーになっているか検証する．
  /// @param[in] col_list 列のリスト
  /// @retval true col_list がカバーになっている．
  /// @retval false col_list でカバーされていない行がある．
  bool
  verify(const vector<int>& col_list) const;

  /// @brief 内容を出力する．
  /// @param[in] s 出力先のストリーム
  void
  print(ostream& s) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  /// @note 行/列のサイズは不変
  void
  clear();

  /// @brief 要素を追加する．
  /// @param[in] row_pos 追加する要素の行番号
  /// @param[in] col_pos 追加する要素の列番号
  void
  insert_elem(int row_pos,
	      int col_pos);


public:
  //////////////////////////////////////////////////////////////////////
  // restore 用のスタックを操作する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief スタックが空の時 true を返す．
  bool
  stack_empty();

  /// @brief スタックに削除した行/列のヘッダと積む．
  void
  push(MpHead* head);

  /// @brief スタックから取り出す．
  MpHead*
  pop();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief サイズを変更する．
  /// @param[in] row_size 行数
  /// @param[in] col_size 列数
  ///
  /// 内容はクリアされる．
  void
  resize(int row_size,
	 int col_size);

  /// @brief 内容をコピーする．
  void
  copy(const MpMatrix& src);

  /// @brief セルの生成
  /// @param[in] row_pos 行番号
  /// @param[in] col_pos 列番号
  MpCell*
  alloc_cell(int row_pos,
	     int col_pos);

  /// @brief セルの解放
  void
  free_cell(MpCell* cell);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // セルを確保するためのアロケータ
  UnitAlloc mCellAlloc;

  // 行数
  int mRowSize;

  // 列数
  int mColSize;

  // 行の先頭の配列
  MpHead* mRowArray;

  // 列の先頭の配列
  MpHead* mColArray;

  // コストの配列
  // サイズは mColSize;
  int* mCostArray;

  // 削除の履歴を覚えておくスタック
  MpHead** mDelStack;

  // mDelStack のポインタ
  int mStackTop;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 行数を返す．
inline
int
MpMatrix::row_size() const
{
  return mRowSize;
}

// @brief 行の先頭を取り出す．
// @param[in] row_pos 行位置 ( 0 <= row_pos < row_size() )
inline
const MpHead*
MpMatrix::row_head(int row_pos) const
{
  ASSERT_COND( row_pos >= 0 && row_pos < row_size() );

  return &mRowArray[row_pos];
}

// @brief 行の先頭を取り出す．
// @param[in] row_pos 行位置 ( 0 <= row_pos < row_size() )
inline
MpHead*
MpMatrix::row_head(int row_pos)
{
  ASSERT_COND( row_pos >= 0 && row_pos < row_size() );

  return &mRowArray[row_pos];
}

// @brief 列数を返す．
inline
int
MpMatrix::col_size() const
{
  return mColSize;
}

// @brief 列の先頭を取り出す．
// @param[in] col_pos 列位置 ( 0 <= col_pos < col_size() )
inline
const MpHead*
MpMatrix::col_head(int col_pos) const
{
  ASSERT_COND( col_pos >= 0 && col_pos < col_size() );

  return &mColArray[col_pos];
}

// @brief 列の先頭を取り出す．
// @param[in] col_pos 列位置 ( 0 <= col_pos < col_size() )
inline
MpHead*
MpMatrix::col_head(int col_pos)
{
  ASSERT_COND( col_pos >= 0 && col_pos < col_size() );

  return &mColArray[col_pos];
}

// @brief 列のコストを取り出す．
// @param[in] col_pos 列位置 ( 0 <= col_pos < col_size() )
inline
int
MpMatrix::col_cost(int col_pos) const
{
  return mCostArray[col_pos];
}

// @brief 列のコストの配列を取り出す．
inline
const int*
MpMatrix::col_cost_array() const
{
  return mCostArray;
}

// @brief スタックが空の時 true を返す．
inline
bool
MpMatrix::stack_empty()
{
  return mStackTop == 0;
}

// @brief スタックに削除した行/列のヘッダを積む．
inline
void
MpMatrix::push(MpHead* head)
{
  mDelStack[mStackTop] = head;
  ++ mStackTop;
}

// @brief スタックから取り出す．
inline
MpHead*
MpMatrix::pop()
{
  -- mStackTop;
  return mDelStack[mStackTop];
}

END_NAMESPACE_YM_SATPG

#endif // MPMATRIX_H
