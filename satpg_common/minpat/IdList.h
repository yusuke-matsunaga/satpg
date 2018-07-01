#ifndef IDLIST_H
#define IDLIST_H

/// @file IdList.h
/// @brief IdList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "ym/Array.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class IdList IdList.h "IdList.h"
/// @brief ID番号(int) のリスト
//////////////////////////////////////////////////////////////////////
class IdList
{
public:

  /// @brief オブジェクトを作るスタティック関数
  static
  IdList*
  new_obj(const vector<int>& elem_list);

  /// @brief デストラクタ
  ~IdList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素数を返す．
  int
  num() const;

  /// @brief 要素の値を返す．
  int
  operator[](int pos) const;

  /// @brief 要素のリストを返す．
  Array<const int>
  elem_list() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  IdList();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 要素数
  int mNum;

  // リスト本体
  int mBody[1];

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
IdList::IdList()
{
}

// @brief デストラクタ
inline
IdList::~IdList()
{
}

// @brief 要素数を返す．
inline
int
IdList::num() const
{
  return mNum;
}

// @brief 要素の値を返す．
inline
int
IdList::operator[](int pos) const
{
  ASSERT_COND( pos >= 0 && pos < num() );

  return mBody[pos];
}

// @brief 要素のリストを返す．
inline
Array<const int>
IdList::elem_list() const
{
  return Array<const int>(&(mBody[0]), 0, num());
}

END_NAMESPACE_YM_SATPG

#endif // IDLIST_H
