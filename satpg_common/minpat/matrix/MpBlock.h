#ifndef MPBLOCK_H
#define MPBLOCK_H

/// @file MpBlock.h
/// @brief MpBlock のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "MpMatrix.h"
#include "MpHeadList.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpBlock MpBlock.h "MpBlock.h"
/// @brief MpMatrix 上のブロックを表すクラス
//////////////////////////////////////////////////////////////////////
class MpBlock
{
public:

  /// @brief コンストラクタ
  /// @param[int] matrix 対象の行列
  ///
  /// 行列全体を対象のブロックとする．
  MpBlock(MpMatrix& matrix);

  /// @brief コンストラクタ
  /// @param[int] matrix 対象の行列
  ///
  /// 指定された行と列のみを対象のブロックとする．
  MpBlock(MpMatrix& matrix,
	  const vector<int>& row_list,
	  const vector<int>& col_list);

  /// @brief コピーコンストラクタは禁止
  MpBlock(const MpBlock& src) = delete;

  /// @brief 代入演算子は禁止
  MpBlock&
  operator=(const MpBlock& src) = delete;

  /// @brief デストラクタ
  ~MpBlock();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
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

  /// @brief 行の先頭のリストを返す．
  const MpHeadList&
  row_head_list() const;

  /// @brief 行の先頭のリストを返す．
  MpHeadList&
  row_head_list();

  /// @brief 実効的な行数を返す．
  int
  row_num() const;

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

  /// @brief 列の先頭のリストを返す．
  const MpHeadList&
  col_head_list() const;

  /// @brief 列の先頭のリストを返す．
  MpHeadList&
  col_head_list();

  /// @brief 実効的な列数を返す．
  int
  col_num() const;

  /// @brief 列のコストを取り出す．
  /// @param[in] col_pos 列位置 ( 0 <= col_pos < col_size() )
  int
  col_cost(int col_pos) const;

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

  /// @brief 列を選択し，被覆される行を削除する．
  /// @param[in] col_pos 選択した列
  void
  select_col(int col_pos);

  /// @brief 列を選択しない．
  /// @param[in] col_pos 選択しない列
  void
  deselect_col(int col_pos);

  /// @brief 簡単化を行う．
  /// @param[out] selected_cols 簡単化中で選択された列の集合を追加する配列
  void
  reduce(vector<int>& selected_cols);

  /// @brief 削除スタックにマーカーを書き込む．
  void
  save();

  /// @brief 直前のマーカーまで処理を戻す．
  void
  restore();

  /// @brief 内容を表示する．
  void
  print(ostream& s) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 行支配を探し，行を削除する．
  /// @return 削除された行があったら true を返す．
  bool
  row_dominance();

  /// @brief 列支配を探し，列を削除する．
  /// @return 削除された列があったら true を返す．
  bool
  col_dominance();

  /// @brief 必須列を探し，列を選択する．
  /// @param[out] selected_cols 選択された列を追加する列集合
  /// @return 選択された列があったら true を返す．
  bool
  essential_col(vector<int>& selected_cols);

  /// @brief 行を削除する．
  /// @param[in] row_pos 削除する行番号
  void
  delete_row(int row_pos);

  /// @brief 列を削除する．
  /// @param[in] col_pos 削除する列番号
  void
  delete_col(int col_pos);

  /// @brief 行を復元する．
  void
  restore_row(MpHead* row_head);

  /// @brief 列を復元する．
  void
  restore_col(MpHead* col_head);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の行列
  MpMatrix& mMatrix;

  // 有効な行のリスト
  MpHeadList mRowHeadList;

  // 有効な列のリスト
  MpHeadList mColHeadList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 行数を返す．
inline
int
MpBlock::row_size() const
{
  return mMatrix.row_size();
}

// @brief 行の先頭を取り出す．
// @param[in] row_pos 行位置 ( 0 <= row_pos < row_size() )
inline
const MpHead*
MpBlock::row_head(int row_pos) const
{
  return mMatrix.row_head(row_pos);
}

// @brief 行の先頭を取り出す．
// @param[in] row_pos 行位置 ( 0 <= row_pos < row_size() )
inline
MpHead*
MpBlock::row_head(int row_pos)
{
  return mMatrix.row_head(row_pos);
}

// @brief 行の先頭のリストを返す．
inline
const MpHeadList&
MpBlock::row_head_list() const
{
  return mRowHeadList;
}

// @brief 行の先頭のリストを返す．
inline
MpHeadList&
MpBlock::row_head_list()
{
  return mRowHeadList;
}

// @brief 実効的な行数を返す．
inline
int
MpBlock::row_num() const
{
  return mRowHeadList.num();
}

// @brief 列数を返す．
inline
int
MpBlock::col_size() const
{
  return mMatrix.col_size();
}

// @brief 列の先頭を取り出す．
// @param[in] col_pos 列位置 ( 0 <= col_pos < col_size() )
inline
const MpHead*
MpBlock::col_head(int col_pos) const
{
  return mMatrix.col_head(col_pos);
}

// @brief 列の先頭を取り出す．
// @param[in] col_pos 列位置 ( 0 <= col_pos < col_size() )
inline
MpHead*
MpBlock::col_head(int col_pos)
{
  return mMatrix.col_head(col_pos);
}

/// @brief 列の先頭のリストを返す．
inline
const MpHeadList&
MpBlock::col_head_list() const
{
  return mColHeadList;
}

// @brief 列の先頭のリストを返す．
inline
MpHeadList&
MpBlock::col_head_list()
{
  return mColHeadList;
}

// @brief 実効的な列数を返す．
inline
int
MpBlock::col_num() const
{
  return mColHeadList.num();
}

// @brief 列のコストを取り出す．
// @param[in] col_pos 列位置 ( 0 <= col_pos < col_size() )
inline
int
MpBlock::col_cost(int col_pos) const
{
  return mMatrix.col_cost(col_pos);
}

// @brief 列を選択しない．
// @param[in] col_pos 選択しない列
inline
void
MpBlock::deselect_col(int col_pos)
{
  delete_col(col_pos);
}

END_NAMESPACE_YM_SATPG

#endif // MPBLOCK_H
