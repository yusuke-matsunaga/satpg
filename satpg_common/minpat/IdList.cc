
/// @file IdList.cc
/// @brief IdList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "IdList.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス IdList
//////////////////////////////////////////////////////////////////////

// @brief オブジェクトを作るスタティック関数
IdList*
IdList::new_obj(const vector<int>& elem_list)
{
  int n = elem_list.size();
  int n1 = n;
  if ( n1 == 0 ) {
    n1 = 1;
  }
  void* p = new char[sizeof(IdList) + sizeof(int*) * (n1 - 1)];
  IdList* obj = new (p) IdList();
  obj->mNum = n;
  for ( auto i: Range(n) ) {
    obj->mBody[i] = elem_list[i];
  }
}

END_NAMESPACE_YM_SATPG
