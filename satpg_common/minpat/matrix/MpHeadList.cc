
/// @file MpHeadList.cc
/// @brief MpHeadList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "MpHeadList.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス MpHeadList
//////////////////////////////////////////////////////////////////////

// @brief 内容をセットする．
void
MpHeadList::set(const vector<MpHead*>& head_list)
{
  mNum = 0;
  MpHead* prev_head = &mDummy;
  for ( auto head: head_list ) {
    prev_head->mNext = head;
    head->mPrev = prev_head;
    prev_head = head;
    ++ mNum;
  }
  prev_head->mNext = &mDummy;
  mDummy.mPrev = prev_head;
}

// @brief 要素を追加する．
void
MpHeadList::insert(MpHead* head)
{
  MpHead* prev = mDummy.mPrev; // 末尾
  MpHead* next = &mDummy;
  int pos = head->pos();
  if ( prev == next || prev->pos() < pos ) {
    // 末尾に追加
    // よくあるパタンなので最初に調べる．
    ;
  }
  else {
    for ( prev = &mDummy; ; prev = next ) {
      next = prev->mNext;
      ASSERT_COND( next->pos() != pos );
      if ( next->pos() > pos ) {
	// prev と next の間に挿入する．
	break;
      }
      ASSERT_COND( next != &mDummy );
    }
  }
  prev->mNext = head;
  head->mPrev = prev;
  head->mNext = next;
  next->mPrev = head;
  ++ mNum;
}

// @brief 要素を削除する．
// @param[in] head 削除する要素
//
// row がこのリストに含まれていると仮定する．
void
MpHeadList::exclude(MpHead* head)
{
  ASSERT_COND( !head->mDeleted );
  head->mDeleted = true;
  -- mNum;

  MpHead* prev = head->mPrev;
  MpHead* next = head->mNext;
  prev->mNext = next;
  next->mPrev = prev;
}

// @brief exclude() で削除した行を復元する．
void
MpHeadList::restore(MpHead* head)
{
  ASSERT_COND( head->mDeleted );
  head->mDeleted = false;
  ++ mNum;

  MpHead* prev = head->mPrev;
  MpHead* next = head->mNext;
  prev->mNext = head;
  next->mPrev = head;
}

// @brief 分割したリストをマージして元にもどす．
// @param[in] src1, src2 分割したリスト
//
// src1, src2 の内容は破棄される．
void
MpHeadList::merge(MpHeadList& src1,
		  MpHeadList& src2)
{
  vector<MpHead*> row_list;
  row_list.reserve(src1.num() + src2.num());
  MpHead* head1 = src1.mDummy.mNext;
  MpHead* head2 = src2.mDummy.mNext;
  MpHead* end1 = &src1.mDummy;
  MpHead* end2 = &src2.mDummy;
  while ( head1 != end1 && head2 != end2 ) {
    if ( head1->pos() < head2->pos() ) {
      row_list.push_back(head1);
      head1 = head1->mNext;
    }
    else if ( head1->pos() > head2->pos() ) {
      row_list.push_back(head2);
      head2 = head2->mNext;
    }
    else {
      ASSERT_NOT_REACHED;
    }
  }
  for ( ; head1 != end1; head1 = head1->mNext ) {
    row_list.push_back(head1);
  }
  for ( ; head2 != end2; head2 = head2->mNext ) {
    row_list.push_back(head2);
  }
  set(row_list);
}

// @brief 等価比較演算子
// @param[in] list1, list2 オペランド
bool
operator==(const MpHeadList& list1,
	   const MpHeadList& list2)
{
  if ( list1.num() != list2.num() ) {
    return false;
  }

  MpHeadIterator it1 = list1.begin();
  MpHeadIterator end1 = list1.end();
  MpHeadIterator it2 = list2.begin();
  MpHeadIterator end2 = list2.end();
  for ( ; ; ) {
    const MpHead* head1 = *it1;
    const MpHead* head2 = *it2;
    if ( head1->pos() != head2->pos() ) {
      return false;
    }
    if ( head1->num() != head2->num() ) {
      return false;
    }

    ++ it1;
    ++ it2;
    if ( it1 == end1 ) {
      if ( it2 != end2 ) {
	return false;
      }
      break;
    }
  }

  return true;
}

END_NAMESPACE_YM_SATPG
